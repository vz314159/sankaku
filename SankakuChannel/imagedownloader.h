#ifndef IMAGEDOWNLOADER_H
#define IMAGEDOWNLOADER_H

#include <vector>
#include <string>

#include "SankakuChannel/context.h"
#include "request.h"
#include "utility.h"

namespace Sankaku {
class ImageDownloader : public Request, public Makeable<ImageDownloader> {
 public:
  ImageDownloader(std::string path, std::vector<std::string> cont,
                  Sankaku::Context ctx);

  std::unique_ptr<Request> onFinished(std::string data, size_t code);

 private:
  std::vector<std::string> m_cont;
  Sankaku::Context m_context;
};
}

#endif  // IMAGEDOWNLOADER_H
