#include "postdownloader.h"

#include <regex>

#include "SankakuChannel/imagedownloader.h"
#include "config.h"

Sankaku::PostDownloader::PostDownloader(std::vector<std::string> posts,
                                        Sankaku::Context context)
    : m_posts(std::move(posts)), m_context(std::move(context)) {
  setMethod(Request::GET);
  setUrl(m_posts.back());
  m_posts.pop_back();
}

std::unique_ptr<Request> Sankaku::PostDownloader::onFinished(std::string data,
                                                             size_t code) {
  if (code == 200) {
    std::string original;
    std::regex postRX("Original:[^/]*//([^\"]*)\"", std::regex::extended);
    std::smatch m;
    if (std::regex_search(data, m, postRX)) {
      original = m[1].str();
      std::regex filenameRX("/([^./]*)\\.([^?]*)\\?", std::regex::extended);
      std::smatch m;
      if (std::regex_search(original, m, filenameRX)) {
        std::string filenameMain = m[1].str();
        std::string filenameExt = m[2].str();
        std::string filenameFull = m[1].str() + "." + m[2].str();
        if (m_context.allowedExtensions().find(filenameExt) !=
            m_context.allowedExtensions().end()) {
          if (m_context.savedPosts().find(filenameFull) ==
                  m_context.savedPosts().end()) {
            std::unordered_set<std::string> &tags =
                m_context.tags().tags()[filenameFull];
            std::regex tagRX(
                "(general|character|medium)><a href=\"/\\?tags=([^\"]*)\"",
                std::regex::extended);
            bool checkTags = true;
            std::string badTag;
            for (auto it =
                     std::sregex_iterator(data.begin(), data.end(), tagRX);
                 it != std::sregex_iterator(); ++it) {
              std::smatch m = *it;
              std::string tagName = m[2].str();
#ifdef SHOW_FOUNDTAGS
              printf("[DD]: found tag [%s]\n", tagName.c_str());
#endif
              tags.insert(tagName);
              if (m_context.tagBlacklist().find(tagName) !=
                  m_context.tagBlacklist().end()) {
                checkTags = false;
                badTag = tagName;
              }
            }
            if (checkTags) {
              return Sankaku::ImageDownloader::make(std::move(original),
                                                    std::move(m_posts),
                                                    std::move(m_context));
            } else {
#ifdef SHOW_SKIPPED
              printf("[DD]: skipping bad tag [%s] <%s>\n", badTag.c_str(),
                     url().c_str());
#endif
            }
          } else {
            printf("[WW]: image already saved <%s>\n", url().c_str());
          }
        } else {
          printf("[WW]: image has bad extension [%s] <%s>\n",
                 filenameExt.c_str(), url().c_str());
        }
      } else {
        printf("[WW]: filename not found <%s>\n", url().c_str());
#ifdef SHOW_PROBLEMATIC
        printf("[DD]: problematic page:\n%s\n", data.c_str());
#endif
      }
    } else {
      printf("[WW]: original image not found <%s>\n", url().c_str());
#ifdef SHOW_PROBLEMATIC
      printf("[DD]: problematic page:\n%s\n", data.c_str());
#endif
    }
  } else {
    printf("[WW]: http returned code %lu <%s>\n", code, url().c_str());
#ifdef SHOW_PROBLEMATIC
    printf("[DD]: problematic page:\n%s\n", data.c_str());
#endif
  }
  return continuation();
}

std::unique_ptr<Request> Sankaku::PostDownloader::continuation() {
  if (m_posts.empty()) {
    return nullptr;
  }
  return PostDownloader::make(std::move(m_posts), std::move(m_context));
}
