#ifndef POSTDOWNLOADER_H
#define POSTDOWNLOADER_H

#include <memory>
#include <vector>

#include "SankakuChannel/context.h"
#include "request.h"
#include "utility.h"

namespace Sankaku {
class PostDownloader : public Request, public Makeable<PostDownloader> {
 public:
  PostDownloader(std::vector<std::string> posts, Sankaku::Context context);

  std::unique_ptr<Request> onFinished(std::string data, size_t code) override;

 private:
  std::unique_ptr<Request> continuation();

  std::vector<std::string> m_posts;
  Sankaku::Context m_context;
};
}

#endif  // POSTDOWNLOADER_H
