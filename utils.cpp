#include "utils.h"

ThreadRAII::ThreadRAII(std::thread&& t, DtorAction a) :
    action(a), t(std::move(t)) {}

ThreadRAII::~ThreadRAII() {
  if (t.joinable()) {
    if (this->action == DtorAction::join) {
      t.join();
    } else {
      t.detach();
    }
  }
}


bool randomPassed() {
  double r = ((double) rand() / RAND_MAX);
  if (r > 0.5) return true;
  return false;
}

int getRandomInt(int min, int max) {
  int randNum = rand() % (max-min + 1) + min;
  return randNum;
}
