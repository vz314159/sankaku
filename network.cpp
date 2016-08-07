#include "network.h"

#include <regex>

#include "curl/curl.h"

Network::Network(size_t bpsDown, size_t bpsUp)
    : m_thread([this]() { m_worker(); }), m_worker(*this, bpsUp, bpsDown) {}

Network::~Network() {
  m_worker.quit();
  m_thread.join();
}

void Network::pushRequest(std::unique_ptr<Request> req) {
  m_worker.push(std::move(req));
}
