#include <emscripten.h>
#include <emscripten/bind.h>
#include <queue>
#include "evaluate.h"
#include "syzygy/tbprobe.h"
#include "uci.h"
#if LSFW_NNUE_COUNT > 0
# include "nnue/evaluate_nnue.h"
# include "nnue/nnue_architecture.h"
#endif

#if LSFW_NNUE_COUNT == 2
# define GET_USE_NNUE(x) ""
#elif LSFW_NNUE_COUNT == 1
# define GET_USE_NNUE(x) Stockfish::Options.count("Use NNUE") > 0 ? "setoption name Use NNUE value " + std::string(x ? "true" : "false") : ""
# define EvalFileDefaultNameBig EvalFileDefaultName
# define EvalFileDefaultNameSmall EvalFileDefaultName
#else
# define GET_USE_NNUE(x) ""
# define EvalFileDefaultNameBig ""
# define EvalFileDefaultNameSmall ""
#endif

struct Command : public std::streambuf {
  enum { UCI, NNUE } type;
  std::string uci;
  std::shared_ptr<void> ptr = nullptr;
  int index;

  Command(const char *text) : type(UCI), uci(text) {
    std::free((void*)text);
  }
  Command(char *buf, size_t sz, int index) : type(NNUE), ptr((void*)buf, std::free), index(index) {
    setg(buf, buf, buf + sz);
  }

  using std::streambuf::seekoff;
  using std::streambuf::seekpos;
};

struct {
  std::mutex m;
  std::queue<Command> q;
  std::condition_variable cv;

  void push(Command el) {
    std::unique_lock<std::mutex> lock(m);
    q.push(el);
    lock.unlock();
    cv.notify_one();
  }

  Command pop() {
    std::unique_lock<std::mutex> lock(m);
    while (q.empty()) cv.wait(lock);
    Command el = std::move(q.front());
    q.pop();
    return el;
  }
} inQ;

EMSCRIPTEN_KEEPALIVE std::string js_getline() {
  auto cmd = inQ.pop();
  if (cmd.type == cmd.UCI) return cmd.uci;
#if LSFW_NNUE_COUNT > 0
  else if (cmd.type == cmd.NNUE) {
    if (!cmd.ptr) return GET_USE_NNUE(false);
    std::istream in(&cmd);
    auto success = 
# if LSFW_NNUE_COUNT == 2
      Stockfish::Eval::NNUE::load_eval(in, Stockfish::Eval::NNUE::NetSize(cmd.index));
# else
      Stockfish::Eval::NNUE::load_eval("", in) ? std::make_optional(0) : std::nullopt;
# endif
    if (!success.has_value()) std::cerr << "BAD_NNUE " << cmd.index << std::endl;
    return GET_USE_NNUE(success.has_value());
  }
#endif
  return "";
}

extern "C" {
  EMSCRIPTEN_KEEPALIVE void uci(const char *utf8) {
    inQ.push(Command(utf8));
  }

  EMSCRIPTEN_KEEPALIVE void setNnueBuffer(char *buf, size_t sz, int index) {
    inQ.push(Command(buf, sz, index));
  }

  EMSCRIPTEN_KEEPALIVE const char * getRecommendedNnue(int index) {
    return index == 1 ? EvalFileDefaultNameSmall : EvalFileDefaultNameBig;
  }
}

// stubs for tbprobe.cpp (so we don't need -sALLOW_UNIMPLEMENTED_SYSCALLS)
#if LSFW_NNUE_COUNT > 0
  namespace Stockfish::Tablebases {
#else
  namespace Tablebases {
#endif
  int MaxCardinality = 0;
  void     init(const std::string& paths) {}
  WDLScore probe_wdl(Position& pos, ProbeState* result) { return WDLDraw; }
  int      probe_dtz(Position& pos, ProbeState* result) { return 0; }
#if LSFW_NNUE_COUNT == 2
  bool     root_probe(Position& pos, Search::RootMoves& rootMoves, bool rule50) { return false; }
  bool     root_probe_wdl(Position& pos, Search::RootMoves& rootMoves, bool rule50) { return false; }
  Config   rank_root_moves(const OptionsMap& options, Position& pos, Search::RootMoves& rootMoves) {
    return Config();
  }
#else
  bool root_probe(Position& pos, Search::RootMoves& rootMoves) { return false; }
  bool root_probe_wdl(Position& pos, Search::RootMoves& rootMoves) { return false; }
#endif
}
