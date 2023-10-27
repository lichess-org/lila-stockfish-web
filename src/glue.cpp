#include <emscripten.h>
#include <emscripten/bind.h>
#include <queue>
#include "evaluate.h"
#include "syzygy/tbprobe.h"
#include "nnue/evaluate_nnue.h"
#include "uci.h"

struct Command : public std::streambuf {
  enum { UCI, NNUE } type;

  Command(const char *text) : type(UCI), uci(text) {
    std::free((void*)text);
  }
  Command(char *buf, size_t sz) : type(NNUE), ptr((void*)buf, std::free) {
    setg(buf, buf, buf + sz);
  }

  std::string uci;
  std::shared_ptr<void> ptr = nullptr;
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
  else if (cmd.type == cmd.NNUE) {
    if (!cmd.ptr)
      return Stockfish::Options.count("Use NNUE") > 0 ? "setoption name Use NNUE value false" : "";
    
    std::istream in(&cmd);
    bool success = Stockfish::Eval::NNUE::load_eval(EvalFileDefaultName, in);
    if (!success) std::cerr << "BAD_NNUE" << std::endl;
    if (Stockfish::Options.count("Use NNUE") > 0)
      return "setoption name Use NNUE value " + std::string(success ? "true" : "false");
  }
  return "";
}

extern "C" {
  EMSCRIPTEN_KEEPALIVE const char * getRecommendedNnue() {
    return EvalFileDefaultName;
  }

  EMSCRIPTEN_KEEPALIVE void uci(const char *utf8) {
    inQ.push(Command(utf8));
  }

  EMSCRIPTEN_KEEPALIVE void setNnueBuffer(char *buf, size_t sz) {
    inQ.push(Command(buf, sz));
  }
}

// stubs for tbprobe.cpp (so we don't need -sALLOW_UNIMPLEMENTED_SYSCALLS)
namespace Stockfish::Tablebases {

  int MaxCardinality = 0;

  void init(const std::string& paths) {}
  WDLScore probe_wdl(Position& pos, ProbeState* result) { return WDLDraw; }
  int probe_dtz(Position& pos, ProbeState* result) { return 0; }
  bool root_probe(Position& pos, Search::RootMoves& rootMoves) { return false; }
  bool root_probe_wdl(Position& pos, Search::RootMoves& rootMoves) { return false; }
}
