#ifndef QUERY_H
#define QUERY_H

#include "SankakuChannel/context.h"
#include "request.h"

namespace Sankaku {
class Query : public Request {
 public:
  Query(std::string query, Sankaku::Context ctx);

  std::unique_ptr<Request> onFinished(std::string data, size_t code) override;

 private:
  Sankaku::Context m_context;
};
}

#endif  // QUERY_H
