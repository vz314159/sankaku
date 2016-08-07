#include "tags.h"

#include <cassert>
#include <istream>
#include <ostream>

Tags::Tags() {}

std::ostream &operator<<(std::ostream &stream, const Tags &tags) {
  uint32_t picCount = tags.m_tags.size();
  stream.write(static_cast<char *>(static_cast<void *>(&picCount)), 4);
  if (!stream.good()) {
    return stream;
  }
  for (const auto &pic : tags.m_tags) {
    stream.write(pic.first.c_str(), pic.first.length() + 1);
    if (!stream.good()) {
      return stream;
    }
    uint32_t tagCount = pic.second.size();
    stream.write(static_cast<char *>(static_cast<void *>(&tagCount)), 4);
    if (!stream.good()) {
      return stream;
    }
    for (const auto &t : pic.second) {
      stream.write(t.c_str(), t.length() + 1);
      if (!stream.good()) {
        return stream;
      }
    }
  }
  return stream;
}

std::istream &operator>>(std::istream &stream, Tags &tags) {
  uint32_t picCount;
  stream.read(static_cast<char *>(static_cast<void *>(&picCount)), 4);
  if (!stream.good()) {
    return stream;
  }
  for (size_t i = 0; i < picCount; ++i) {
    std::string name;
    char c;
    while (true) {
      stream.read(&c, 1);
      if (!stream.good()) {
        return stream;
      }
      if (c == '\0') {
        break;
      } else {
        name += c;
      }
    }
    std::unordered_set<std::string> &current = tags.m_tags[name];
    uint32_t tagCount;
    stream.read(static_cast<char *>(static_cast<void *>(&tagCount)), 4);
    if (!stream.good()) {
      return stream;
    }
    for (size_t j = 0; j < tagCount; ++j) {
      std::string tagName;
      char c;
      while (true) {
        stream.read(&c, 1);
        if (!stream.good()) {
          return stream;
        }
        if (c == '\0') {
          break;
        } else {
          tagName += c;
        }
      }
      current.insert(tagName);
    }
  }
  return stream;
}
