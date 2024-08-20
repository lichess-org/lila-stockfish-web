#include "glue.hpp"

#include "evaluate.h"
#include "syzygy/tbprobe.h"
#include "uci.h"

# include "nnue/nnue_architecture.h"

extern Stockfish::UCIEngine uci_global;

# define GET_USE_NNUE(x) ""

EMSCRIPTEN_KEEPALIVE std::string js_getline() {
  auto cmd = inQ.pop();
  if (cmd.type == cmd.UCI) return cmd.uci;
  else if (cmd.type == cmd.NNUE) {
    if (!cmd.ptr) return GET_USE_NNUE(false);
    std::istream in(&cmd);
    bool success;
    
    if (cmd.index == 0) {uci_global.engine.load_big_network(in); success = true;}
    else if (cmd.index == 1) {uci_global.engine.load_small_network(in); success = true;}
    else success = false;

    if (!success) std::cerr << "BAD_NNUE " << cmd.index << std::endl;
    return GET_USE_NNUE(success.has_value());
  }
  return "";
}

const char* getRecommendedNnueName(int index) {
  return index == 1 ? EvalFileDefaultNameSmall : EvalFileDefaultNameBig;
}

// stubs for tbprobe.cpp (so we don't need -sALLOW_UNIMPLEMENTED_SYSCALLS)
namespace Stockfish::Tablebases {
  int MaxCardinality = 0;
  void     init(const std::string& paths) {}
  WDLScore probe_wdl(Position& pos, ProbeState* result) { return WDLDraw; }
  int      probe_dtz(Position& pos, ProbeState* result) { return 0; }
  bool     root_probe(Position& pos, Search::RootMoves& rootMoves, bool rule50) { return false; }
  bool     root_probe_wdl(Position& pos, Search::RootMoves& rootMoves, bool rule50) { return false; }
  Config   rank_root_moves(const OptionsMap& options, Position& pos, Search::RootMoves& rootMoves, bool rankDTZ) {
    return Config();
  }
}
