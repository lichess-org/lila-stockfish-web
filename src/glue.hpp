#pragma once

#include <emscripten.h>
#include <emscripten/bind.h>
#include <queue>

struct Command : public std::streambuf {
  enum { UCI, NNUE } type;
  std::string uci;
  std::shared_ptr<void> ptr = nullptr;
  int index;

  Command(const char* text) : type(UCI), uci(text) { std::free((void*)text); }
  Command(char* buf, size_t sz, int index)
    : type(NNUE), ptr((void*)buf, std::free), index(index) {
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

const char* getRecommendedNnueName(int index);

EMSCRIPTEN_KEEPALIVE std::string js_getline();

extern "C" {
EMSCRIPTEN_KEEPALIVE void uci(const char* utf8) { inQ.push(Command(utf8)); }

EMSCRIPTEN_KEEPALIVE void setNnueBuffer(char* buf, size_t sz, int index) {
  inQ.push(Command(buf, sz, index));
}

EMSCRIPTEN_KEEPALIVE const char* getRecommendedNnue(int index) {
  return getRecommendedNnueName(index);
}
}