#include "networkworker.h"

#include <regex>
#include <string>

#include "curl/curl.h"
#include "config.h"

NetworkWorker::NetworkWorker(Network &net, size_t uploadRate,
                             size_t downloadRate)
    : m_running(true),
      m_network(net),
      m_uploadRate(uploadRate),
      m_downloadRate(downloadRate) {}

NetworkWorker::~NetworkWorker() {
  std::unique_lock<std::mutex> lock(m_mutex);
  m_running = false;
  lock.unlock();
  m_waitEmpty.notify_all();
}

void NetworkWorker::operator()() {
  curl_global_init(CURL_GLOBAL_DEFAULT);
  CURL *hndl = curl_easy_init();
  curl_off_t rateDown = m_downloadRate;
  curl_off_t rateUp = m_uploadRate;
  std::string data;
  std::string header;
  curl_easy_setopt(hndl, CURLOPT_MAX_RECV_SPEED_LARGE, rateDown);
  curl_easy_setopt(hndl, CURLOPT_MAX_SEND_SPEED_LARGE, rateUp);
  curl_easy_setopt(hndl, CURLOPT_FOLLOWLOCATION, 1);
#ifdef SHOW_CURLVERBOSE
  curl_easy_setopt(hndl, CURLOPT_VERBOSE, 1);
#endif
  curl_easy_setopt(hndl, CURLOPT_WRITEFUNCTION, curl_cb_write);
  curl_easy_setopt(hndl, CURLOPT_WRITEDATA, &data);
  curl_easy_setopt(hndl, CURLOPT_HEADERFUNCTION, curl_cb_header_write);
  curl_easy_setopt(hndl, CURLOPT_HEADERDATA, &header);
  //curl_easy_setopt(hndl, CURLOPT_COOKIEFILE, "cookie");
  //curl_easy_setopt(hndl, CURLOPT_COOKIEJAR, "cookie");
  while (running()) {
    std::unique_lock<std::mutex> lock(m_mutex);
    while (m_requests.empty() && m_running) {
      m_waitEmpty.wait(lock);
    }
    if (m_requests.empty()) {
      break;
    }
    std::unique_ptr<Request> req = std::move(m_requests.front());
    m_requests.pop();
    while (req) {
      struct curl_slist *headersList = NULL;
      std::vector<std::string> headers;
      headers.emplace_back(
          "User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/537.36 "
          "(KHTML, like Gecko) Chrome/41.0.2228.0 Safari/537.36");
      if (req->method() == Request::POST) {
        curl_easy_setopt(hndl, CURLOPT_POSTFIELDSIZE, req->postData().length());
        curl_easy_setopt(hndl, CURLOPT_POSTFIELDS, req->postData().c_str());
        headersList = curl_slist_append(
            headersList, "Content-Type: application/x-www-form-urlencoded");
      } else {
        curl_easy_setopt(hndl, CURLOPT_HTTPGET, 1);
      }
      for (const auto &hd : headers) {
        headersList = curl_slist_append(headersList, hd.c_str());
      }

      curl_easy_setopt(hndl, CURLOPT_HTTPHEADER, headersList);
      curl_easy_setopt(hndl, CURLOPT_URL, req->url().c_str());

      CURLcode res;
      if ((res = curl_easy_perform(hndl))) {
        printf("[WW]: request error: %s <%s>\n", curl_easy_strerror(res),
               req->url().c_str());
      }
      curl_slist_free_all(headersList);
      size_t code = getHTTPErrorCode(header);
#ifdef SHOW_CURLVERBOSE
      printf("%s\n", header.c_str());
#endif
      req = req->onFinished(std::move(data), code);
      data.clear();
      header.clear();
    }
  }
  curl_easy_cleanup(hndl);
  curl_global_cleanup();
}

void NetworkWorker::quit() {
  std::unique_lock<std::mutex>(m_mutex);
  m_running = false;
}

void NetworkWorker::push(std::unique_ptr<Request> req) {
  std::unique_lock<std::mutex> lock(m_mutex);
  m_requests.emplace(std::move(req));
  lock.unlock();
  m_waitEmpty.notify_one();
}

size_t NetworkWorker::curl_cb_write(void *buffer, size_t size, size_t nmemb,
                                 void *userp) {
  std::string *data = reinterpret_cast<std::string *>(userp);
  size_t total = size * nmemb;
  *data += std::string(reinterpret_cast<char *>(buffer), total);
  return total;
}

size_t NetworkWorker::curl_cb_header_write(char *buffer, size_t size, size_t nmemb,
                                        void *userp) {
  std::string *data = reinterpret_cast<std::string *>(userp);
  size_t total = size * nmemb;
  *data += std::string(reinterpret_cast<char *>(buffer), total);
  return total;
}

bool NetworkWorker::running() const {
  std::unique_lock<std::mutex>(m_mutex);
  return m_running || !m_requests.empty();
}

size_t NetworkWorker::getHTTPErrorCode(const std::string &headers) {
  std::regex rx("HTTP[^ ]* ([0-9]*) ", std::regex::extended);
  size_t lastCode = 0;
  for (auto it = std::sregex_iterator(headers.begin(), headers.end(), rx);
       it != std::sregex_iterator(); ++it) {
    std::smatch m = *it;
    lastCode = std::atoi(m[1].str().c_str());
  }
  return lastCode;
}
