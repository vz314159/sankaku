#ifndef NETWORKWORKER_H
#define NETWORKWORKER_H

#include <stddef.h>
#include <condition_variable>
#include <mutex>
#include <queue>

#include "request.h"

class Network;
class NetworkWorker {
 public:
  NetworkWorker(Network &net, size_t uploadRate, size_t downloadRate);
  ~NetworkWorker();

  void operator()();
  void quit();
  void push(std::unique_ptr<Request> req);

 private:
  static size_t curl_cb_write(void *buffer, size_t size, size_t nmemb,
                              void *userp);
  static size_t curl_cb_header_write(char *buffer, size_t size, size_t nmemb,
                                     void *userp);
  size_t getHTTPErrorCode(const std::string &headers);
  bool running() const;

  mutable std::mutex m_mutex;
  bool m_running;
  Network &m_network;
  size_t m_uploadRate;
  size_t m_downloadRate;
  std::condition_variable m_waitEmpty;
  std::queue<std::unique_ptr<Request>> m_requests;
};

#endif  // NETWORKWORKER_H
