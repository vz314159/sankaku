#include "context.h"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <istream>
#include <ostream>

#include "config.h"

bool Sankaku::Context::load() {
#ifdef SHOW_CONTEXTIO
  printf("[DD]: loading context [%s]\n", storageFile().c_str());
#endif
  std::ifstream file(storageFile());
  if (file.is_open()) {
    const size_t magicNumber = 12 * 256 + 34;
    uint32_t magic;
    file.read(static_cast<char *>(static_cast<void *>(&magic)), 4);
    if (!file.good()) {
      return false;
    }
    if (magic != magicNumber) {
      return false;
    }
    std::unordered_set<std::string> tmp;
    if (!loadStringSet(tmp, file)) {
      return false;
    }
    m_allowedExtensions = std::move(tmp);
    if (!loadStringSet(tmp, file)) {
      return false;
    }
    m_savedPosts = std::move(tmp);
    if (!loadStringSet(tmp, file)) {
      return false;
    }
    m_tagBlacklist = std::move(tmp);
    std::string tgt;
    if (!loadString(tgt, file)) {
      return false;
    }
    m_targetDir = std::move(tgt);
    Tags tag;
    file >> tag;
    if (!file.good()) {
      return false;
    }
    m_tags = std::move(tag);
  } else {
    printf("[WW]: failed to open context storage [%s]\n",
           storageFile().c_str());
    return false;
  }
  return true;
}

bool Sankaku::Context::save() {
#ifdef SHOW_CONTEXTIO
  printf("[DD]: saving context [%s]\n", storageFile().c_str());
#endif
  std::ofstream file(storageFile());
  if (file.is_open()) {
    const size_t magicNumber = 12 * 256 + 34;
    uint32_t magic = magicNumber;
    file.write(static_cast<char *>(static_cast<void *>(&magic)), 4);
    if (!file.good()) {
      return false;
    }
    if (!saveStringSet(m_allowedExtensions, file)) {
      return false;
    }
    if (!saveStringSet(m_savedPosts, file)) {
      return false;
    }
    if (!saveStringSet(m_tagBlacklist, file)) {
      return false;
    }
    if (!saveString(m_targetDir, file)) {
      return false;
    }
    file << m_tags;
    if (!file.good()) {
      return false;
    }
  } else {
    printf("[WW]: failed to open context storage [%s]\n",
           storageFile().c_str());
    return false;
  }
  return true;
}

std::string Sankaku::Context::storageFile() const {
  if (!m_storageLocation.empty()) {
    return m_storageLocation + "/.context";
  }
  return "";
}

bool Sankaku::Context::saveString(const std::string &str,
                                  std::ostream &stream) {
  stream.write(str.c_str(), str.length() + 1);
  if (!stream.good()) {
    return false;
  }
  return true;
}

bool Sankaku::Context::loadString(std::string &str, std::istream &stream) {
  std::getline(stream, str, '\0');
  if (!stream.good()) {
    return false;
  }
  return true;
}

bool Sankaku::Context::saveStringSet(const std::unordered_set<std::string> &str,
                                     std::ostream &stream) {
  uint32_t count = str.size();
  stream.write(static_cast<char *>(static_cast<void *>(&count)), 4);
  if (!stream.good()) {
    return false;
  }
  for (const auto &s : str) {
    if (!saveString(s, stream)) {
      return false;
    }
  }
  return true;
}

bool Sankaku::Context::loadStringSet(std::unordered_set<std::string> &str,
                                     std::istream &stream) {
  uint32_t count;
  stream.read(static_cast<char *>(static_cast<void *>(&count)), 4);
  if (!stream.good()) {
    return false;
  }
  std::string tmp;
  for (size_t i = 0; i < count; ++i) {
    if (!loadString(tmp, stream)) {
      return false;
    }
    str.emplace(std::move(tmp));
  }
  return true;
}

Sankaku::Context::Context(std::string filename)
    : m_storageLocation(std::move(filename)) {
  if (!storageFile().empty()) {
    if (!load()) {
      printf("[EE]: failed to load context\n");
    }
  }
}

Sankaku::Context::~Context() {
  if (!storageFile().empty()) {
    if (!save()) {
      printf("[EE]: failed to save context\n");
    }
  }
}

Sankaku::Context::Context(Sankaku::Context &&other)
    : m_allowedExtensions(std::move(other.m_allowedExtensions)),
      m_savedPosts(std::move(other.m_savedPosts)),
      m_tagBlacklist(std::move(other.m_tagBlacklist)),
      m_targetDir(std::move(other.m_targetDir)),
      m_tags(std::move(other.m_tags)),
      m_storageLocation(std::move(other.m_storageLocation)) {
  other.m_storageLocation.clear();
}

std::string &Sankaku::Context::targetDir() { return m_targetDir; }

const std::unordered_set<std::string> &Sankaku::Context::allowedExtensions()
    const {
  return m_allowedExtensions;
}

std::unordered_set<std::string> &Sankaku::Context::allowedExtensions() {
  return m_allowedExtensions;
}

const std::unordered_set<std::string> &Sankaku::Context::savedPosts() const {
  return m_savedPosts;
}

std::unordered_set<std::string> &Sankaku::Context::savedPosts() {
  return m_savedPosts;
}

const std::unordered_set<std::string> &Sankaku::Context::tagBlacklist()
    const {
  return m_tagBlacklist;
}

std::unordered_set<std::string> &Sankaku::Context::tagBlacklist() {
  return m_tagBlacklist;
}

Tags &Sankaku::Context::tags() { return m_tags; }

const Tags &Sankaku::Context::tags() const { return m_tags; }

const std::string &Sankaku::Context::storageLocation() const {
  return m_storageLocation;
}

std::string &Sankaku::Context::storageLocation() { return m_storageLocation; }
