#include "glue.hpp"


#include "evaluate.h"
#include "syzygy/tbprobe.h"
#include "uci.h"

#define GET_USE_NNUE(x) ""
#define EvalFileDefaultNameBig ""
#define EvalFileDefaultNameSmall ""

EMSCRIPTEN_KEEPALIVE std::string js_getline() {
  auto cmd = inQ.pop();
  if (cmd.type == cmd.UCI) return cmd.uci;

  return "";
}

const char* getRecommendedNnueName(int index) {
  return index == 1 ? EvalFileDefaultNameSmall : EvalFileDefaultNameBig;
}

// stubs for tbprobe.cpp (so we don't need -sALLOW_UNIMPLEMENTED_SYSCALLS)

namespace Tablebases {
int MaxCardinality = 0;
void init(const std::string& paths) {}
WDLScore probe_wdl(Position& pos, ProbeState* result) { return WDLDraw; }
int probe_dtz(Position& pos, ProbeState* result) { return 0; }

bool root_probe(Position& pos, Search::RootMoves& rootMoves) { return false; }
bool root_probe_wdl(Position& pos, Search::RootMoves& rootMoves) { return false; }
}  // namespace Tablebases

