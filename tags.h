#ifndef TAGS_H
#define TAGS_H

#include <unordered_map>
#include <unordered_set>
#include <iosfwd>

class Tags {
 public:
  Tags();

  inline std::unordered_map<std::string, std::unordered_set<std::string>>
      &tags() {
    return m_tags;
  }
  inline const std::unordered_map<std::string, std::unordered_set<std::string>>
      &tags() const {
    return m_tags;
  }

 private:
  friend std::ostream &operator<<(std::ostream &stream, const Tags &tags);
  friend std::istream &operator>>(std::istream &stream, Tags &tags);

  std::unordered_map<std::string, std::unordered_set<std::string>> m_tags;
};
#endif  // TAGS_H
