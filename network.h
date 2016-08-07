#ifndef NETWORK_H
#define NETWORK_H

#include <memory>
#include <thread>

#include "networkworker.h"
#include "request.h"

class Network {
 public:
  Network(size_t bpsDown, size_t bpsUp);
  ~Network();

  template <typename T>
  void push(T request) {
    pushRequest(std::unique_ptr<T>(new T(std::move(request))));
  }

 private:
  void pushRequest(std::unique_ptr<Request> req);

  std::thread m_thread;
  NetworkWorker m_worker;
};

#endif  // NETWORK_H
