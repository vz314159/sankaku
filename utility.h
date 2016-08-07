#ifndef UTILITY_H
#define UTILITY_H

#include <memory>

template <typename T>
class Makeable {
 public:
  template <typename... Args>
  static std::unique_ptr<T> make(Args... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
  }
};

#endif  // UTILITY_H
