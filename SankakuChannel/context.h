#ifndef CONTEXT_H
#define CONTEXT_H

#include <iosfwd>
#include <string>
#include <unordered_set>

#include "tags.h"

namespace Sankaku {
class Context {
 public:
  Context(std::string filename = "");
  ~Context();

  Context(const Context &) = delete;
  Context(Context &&other);

  const std::string &targetDir() const;
  std::string &targetDir();
  const std::unordered_set<std::string> &allowedExtensions() const;
  std::unordered_set<std::string> &allowedExtensions();
  const std::unordered_set<std::string> &savedPosts() const;
  std::unordered_set<std::string> &savedPosts();
  const std::unordered_set<std::string> &tagBlacklist() const;
  std::unordered_set<std::string> &tagBlacklist();
  Tags &tags();
  const Tags &tags() const;
  const std::string &storageLocation() const;
  std::string &storageLocation();

 private:
  bool load();
  bool save();
  std::string storageFile() const;
  static bool saveString(const std::string &str, std::ostream &stream);
  static bool loadString(std::string &str, std::istream &stream);
  static bool saveStringSet(const std::unordered_set<std::string> &str,
                            std::ostream &stream);
  static bool loadStringSet(std::unordered_set<std::string> &str,
                            std::istream &stream);

  std::unordered_set<std::string> m_allowedExtensions;
  std::unordered_set<std::string> m_savedPosts;
  std::unordered_set<std::string> m_tagBlacklist;
  std::string m_targetDir;
  Tags m_tags;
  std::string m_storageLocation;
};
}  // namespace Sankaku

#endif  // CONTEXT_H
