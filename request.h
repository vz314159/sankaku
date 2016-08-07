#ifndef REQUEST_H
#define REQUEST_H

#include <memory>

class Request {
 public:
  enum Method { POST, GET };

  Request();

  virtual ~Request() {}

  virtual std::unique_ptr<Request> onFinished(std::string data,
                                              size_t code) = 0;

  virtual void onProgress() const {}

  const std::string &url() const { return m_url; }

  Method method() const { return m_method; }

  const std::string &postData() const { return m_postData; }

 protected:
  void setUrl(std::string url) { m_url = std::move(url); }

  void setMethod(Method m) { m_method = m; }

  void setPostData(std::string data) { m_postData = std::move(data); }

 private:
  std::string m_url;
  std::string m_postData;
  Method m_method;
};

#endif  // REQUEST_H
