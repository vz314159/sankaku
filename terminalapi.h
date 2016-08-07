#ifndef TERMINALAPI_H
#define TERMINALAPI_H

#include <string>

class TerminalAPI {
 public:
  TerminalAPI(int argc, char *argv[], std::string workDir = "");

  int exec();

 private:
  bool displayContext();
  bool fetchQuery();
  void printUsage();
  bool reset();

  int m_argc;
  char **m_argv;
  std::string m_workDir;
};

#endif  // TERMINALAPI_H
