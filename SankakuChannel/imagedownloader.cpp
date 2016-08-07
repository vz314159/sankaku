#include "imagedownloader.h"

#include <fstream>
#include <regex>

#include "SankakuChannel/postdownloader.h"
#include "config.h"

Sankaku::ImageDownloader::ImageDownloader(std::string path,
                                          std::vector<std::string> cont,
                                          Context ctx)
    : m_cont(std::move(cont)), m_context(std::move(ctx)) {
  setUrl(std::move(path));
  setMethod(Request::GET);
}

std::unique_ptr<Request> Sankaku::ImageDownloader::onFinished(std::string data,
                                                              size_t code) {
  if (code == 200) {
    printf("[DD]: image downloaded [size: %lu] <%s>\n", data.length(),
           url().c_str());
    std::string filename;
    std::regex filenameRX("/([^/]*)\\?", std::regex::extended);
    std::smatch m;
    if (std::regex_search(url(), m, filenameRX)) {
      filename = m_context.targetDir() + "/" + m[1].str();
#ifdef SHOW_SAVED
      printf("[DD]: saving file to \"%s\"\n", filename.c_str());
#endif
      if (!m_context.targetDir().empty()) {
        std::ofstream file(filename);
        if (file.is_open()) {
          file.write(data.c_str(), data.length());
          if (!file.good()) {
            printf("[WW]: failed to save file [%s]\n", filename.c_str());
          }
          m_context.savedPosts().insert(m[1].str());
        } else {
          printf("[WW]: failed to open file [%s]\n", filename.c_str());
        }
      } else {
        printf("[WW]: target directory not specified\n");
      }
    } else {
      printf("[WW]: filename not found <%s>\n", url().c_str());
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
  if (m_cont.empty()) {
    return nullptr;
  }
  return Sankaku::PostDownloader::make(std::move(m_cont), std::move(m_context));
}
