#include "terminalapi.h"

#include <algorithm>
#include <vector>
#include "SankakuChannel/context.h"
#include "SankakuChannel/query.h"
#include "network.h"

TerminalAPI::TerminalAPI(int argc, char *argv[], std::string workDir)
    : m_argc(argc), m_argv(argv), m_workDir(std::move(workDir)) {}

int TerminalAPI::exec() {
  if (m_argc < 2) {
    printUsage();
    return 0;
  }
  std::string opt(m_argv[1]);
  bool success = true;
  m_argc -= 2;
  m_argv += 2;
  if (opt == "context") {
    success = displayContext();
  } else if (opt == "fetch") {
    success = fetchQuery();
  } else if (opt == "reset") {
    success = reset();
  } else {
    printf("[WW]: unknown option [%s]\n", opt.c_str());
    success = false;
  }
  if (!success) {
    printUsage();
  }
  return success ? 1 : 0;
}

bool TerminalAPI::displayContext() {
  bool any = false;
  bool raw = false;
  bool showExtensions = false;
  bool showKnown = false;
  bool showTarget = false;
  bool showBlack = false;
  bool showTags = false;
  for (int i = 0; i < m_argc; ++i) {
    std::string opt(m_argv[i]);
    if (opt == "raw") {
      raw = true;
    } else if (opt == "extensions") {
      showExtensions = true;
      any = true;
    } else if (opt == "posts") {
      showKnown = true;
      any = true;
    } else if (opt == "target") {
      showTarget = true;
      any = true;
    } else if (opt == "blacklist") {
      showBlack = true;
      any = true;
    } else if (opt == "tags") {
      showTags = true;
      any = true;
    } else {
      printf("[WW]: bad argument for context [%s]\n", opt.c_str());
      return false;
    }
  }
  if (!any) {
    showBlack = true;
    showExtensions = true;
    showKnown = true;
    showTags = true;
    showTarget = true;
  }
  Sankaku::Context ctx(m_workDir);
  if (showExtensions) {
    if (!raw) {
      printf("Allowed extensions:\n");
    }
    for (const auto &e : ctx.allowedExtensions()) {
      if (!raw) {
        printf("\"");
      }
      printf("%s", e.c_str());
      if (!raw) {
        printf("\", ");
      } else {
        printf("\n");
      }
    }
    if (!raw) {
      printf("\n\n");
    }
  }
  if (showKnown) {
    if (!raw) {
      printf("Known posts [%lu]:\n", ctx.savedPosts().size());
    }
    const size_t knownLimit = 5;
    size_t cnt = 0;
    for (const auto &n : ctx.savedPosts()) {
      if (!raw) {
        if (cnt == knownLimit) {
          printf("\tand %lu more ...\n", ctx.savedPosts().size() - knownLimit);
          break;
        }
        printf("\"");
      }
      printf("%s", n.c_str());
      if (!raw) {
        printf("\"");
      }
      printf("\n");
      ++cnt;
    }
    if (!raw) {
      printf("\n");
    }
  }
  if (showTarget) {
    if (!raw) {
      printf("Target directory:\n\"");
    }
    printf("%s", ctx.targetDir().c_str());
    if (!raw) {
      printf("\"\n");
    }
    printf("\n");
  }
  if (showBlack) {
    if (!raw) {
      printf("Tag blacklist [%lu]:\n", ctx.tagBlacklist().size());
    }
    const size_t tagLimit = 3;
    size_t cnt = 0;
    for (const auto &t : ctx.tagBlacklist()) {
      if (!raw) {
        if (cnt == tagLimit) {
          printf("\tand %lu more ...\n", ctx.tagBlacklist().size() - tagLimit);
          break;
        }
        printf("\"");
      }
      printf("%s", t.c_str());
      if (!raw) {
        printf("\"");
      }
      printf("\n");
      ++cnt;
    }
    if (!raw) {
      printf("\n");
    }
  }

  if (showTags) {
    std::unordered_map<std::string, size_t> tags;
    for (const auto &pt : ctx.tags().tags()) {
      for (const auto &t : pt.second) {
        if (tags.find(t) == tags.end()) {
          tags[t] = 0;
        }
        ++tags[t];
      }
    }
    std::vector<std::pair<std::string, size_t>> tagData;
    for (const auto &t : tags) {
      tagData.push_back({t.first, t.second});
    }
    std::sort(tagData.begin(), tagData.end(),
              [](const std::pair<std::string, size_t> &a,
                 const std::pair<std::string, size_t> &b) {
                return b.second < a.second;
              });
    if (!raw) {
      printf("Tags [%lu]:\n", tagData.size());
    }
    std::string data;
    const size_t badTagLimit = 5;
    size_t cnt = 0;
    for (const auto &t : tagData) {
      if (!raw) {
        if (cnt == badTagLimit) {
          data += "\tand " + std::to_string(tagData.size() - badTagLimit) +
                  " more ...\n";
          break;
        }
      }
      data += std::to_string(t.second) + "\t";
      if (!raw) {
        data += "\"";
      }
      data += t.first;
      if (!raw) {
        data += "\"";
      }
      data += "\n";
      ++cnt;
    }
    if (!raw) {
      data += "\n";
    }
    printf("%s", data.c_str());
  }
  ctx.storageLocation() = "";
  return true;
}

bool TerminalAPI::fetchQuery() {
  Sankaku::Context ctx(m_workDir);
  if (m_argc < 1) {
    printf("[WW]: missing query string\n");
    return false;
  }
  if (m_argc > 1) {
    printf("[WW]: unexpected arguments\n");
    return false;
  }
  std::string query(m_argv[0]);
  Network net(1 << 20, 4);
  net.push(Sankaku::Query(query, std::move(ctx)));
  return true;
}

void TerminalAPI::printUsage() {
  printf(
      "sankaku\n\nUsage:\n\n\tcontext\textensions|post|target|"
      "blacklist|tags[raw]\n\t\tDisplay current context.\n\t\tAdd 'raw' "
      "to the query in order to receive unformatted "
      "data.\n\n\t\tExample:\n\t\t\t"
      "sankaku context blacklist raw\n\n\tfetch\t{query}\n\t\tSaves all the "
      "images "
      "matching 'query'.\n\t\tThis behaves as if the query string was used on "
      "the website.\n\n\t\tExample:\n\t\t\tsankaku fetch "
      "\"tags=water+female&page=2\"\n\n\treset\t[--init]\n\t\tClears the "
      "context and all the data stored within.\n\t\tThis includes the "
      "information about saved pictures.\n\t\tAdding '--init' will fill the "
      "freshly cleaned context with predefined defaults.\n\n");
}

bool TerminalAPI::reset() {
  bool init = false;
  for (int i = 0; i < m_argc; ++i) {
    std::string opt(m_argv[i]);
    if (opt == "--init") {
      init = true;
    } else {
      printf("[WW]: unexpected argument [%s]\n", opt.c_str());
      return false;
    }
  }
  Sankaku::Context ctx;
  ctx.storageLocation() = m_workDir;
  if (init) {
    ctx.targetDir() = m_workDir;
    ctx.allowedExtensions() = {"png", "jpg", "webm", "gif", "mp4"};
    ctx.tagBlacklist() = {"urine", "bug", "worms", "insect", "beetle", "bdsm"};
  }
  return true;
}
