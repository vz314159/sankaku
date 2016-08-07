#include "query.h"

#include <regex>
#include <vector>

#include "SankakuChannel/postdownloader.h"
#include "config.h"

Sankaku::Query::Query(std::string query, Sankaku::Context ctx)
    : m_context(std::move(ctx)) {
  setUrl("https://chan.sankakucomplex.com/?" + std::move(query));
  setMethod(Request::GET);
}

std::unique_ptr<Request> Sankaku::Query::onFinished(std::string data,
                                                    size_t code) {
  if (code == 200) {
    std::vector<std::string> posts;
    std::regex postRX("/post/show/([0-9]*)\"", std::regex::extended);
    for (auto it = std::sregex_iterator(data.begin(), data.end(), postRX);
         it != std::sregex_iterator(); ++it) {
      std::smatch m = *it;
      posts.push_back("https://chan.sankakucomplex.com/post/show/" +
                      m[1].str());
    }
    if (posts.empty()) {
      return nullptr;
    }
    return Sankaku::PostDownloader::make(std::move(posts),
                                         std::move(m_context));
  } else {
    printf("[WW]: http returned code %lu <%s>\n", code, url().c_str());
#ifdef SHOW_PROBLEMATIC
    printf("[DD]: problematic page:\n%s\n", data.c_str());
#endif
    return nullptr;
  }
}
