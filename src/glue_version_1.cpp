#include "glue.hpp"

#include "evaluate.h"
#include "syzygy/tbprobe.h"
#include "uci.h"

# include "nnue/evaluate_nnue.h"
# include "nnue/nnue_architecture.h"

#define GET_USE_NNUE(x) Stockfish::Options.count("Use NNUE") > 0 ? "setoption name Use NNUE value " + std::string(x ? "true" : "false") : ""
#define EvalFileDefaultNameBig EvalFileDefaultName
#define EvalFileDefaultNameSmall EvalFileDefaultName

EMSCRIPTEN_KEEPALIVE std::string js_getline() {
  auto cmd = inQ.pop();
  if (cmd.type == cmd.UCI)
    return cmd.uci;
  else if (cmd.type == cmd.NNUE) {
    if (!cmd.ptr) return GET_USE_NNUE(false);
    std::istream in(&cmd);
    auto success =
      Stockfish::Eval::NNUE::load_eval("", in) ? std::make_optional(0) : std::nullopt;
    if (!success.has_value()) std::cerr << "BAD_NNUE " << cmd.index << std::endl;
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
void init(const std::string& paths) {}
WDLScore probe_wdl(Position& pos, ProbeState* result) { return WDLDraw; }
int probe_dtz(Position& pos, ProbeState* result) { return 0; }

bool root_probe(Position& pos, Search::RootMoves& rootMoves) { return false; }
bool root_probe_wdl(Position& pos, Search::RootMoves& rootMoves) { return false; }
}  // namespace Stockfish::Tablebases
