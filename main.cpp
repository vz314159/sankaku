#include <bits/stdc++.h>
#include "SankakuChannel/context.h"
#include "SankakuChannel/query.h"
#include "network.h"
#include "terminalapi.h"
#include "config.h"

int main(int argc, char *argv[]) {
  TerminalAPI api(argc, argv, CONTEXT_DIR);
  return api.exec();
}
