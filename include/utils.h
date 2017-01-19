#ifndef UTILS_H_
#define UTILS_H_

#include <thread>
#include <mutex>
#include <iostream>

// Provide a scoped thread
class ThreadRAII {
 public:
  enum class DtorAction {join, detach};

  ThreadRAII() {}

  ThreadRAII(std::thread&& t, DtorAction a);

  ~ThreadRAII();

  ThreadRAII(const ThreadRAII&) = delete;
	ThreadRAII& operator=(const ThreadRAII&) = delete;
	
  ThreadRAII(ThreadRAII&&) = default;
  ThreadRAII& operator=(ThreadRAII&&) = default;

  inline std::thread& get() { return t; }

 private:
  DtorAction action;
  std::thread t;
};


// Provide funcions for cout used in multi-thread programming
static std::ostream& print_one(std::ostream& os) {
  return os;
}

template<class A0, class ... Args>
std::ostream& print_one(std::ostream& os, const A0& a0, const Args& ... args) {
  os << a0;
  return print_one(os, args...);
}

template<class ... Args>
std::ostream& print(std::ostream& os, const Args& ... args) {
  return print_one(os, args...);
}

template<class ... Args>
void atomicCout(std::mutex& mx, const Args& ... args) {
  std::lock_guard<std::mutex> lk(mx);
  print(std::cout, args...);
}


// Provide Semaphore class for synchronization between threads.
class Semaphore {
 public:
  explicit Semaphore(int count_ = 0) :
      count(count_) {}

  Semaphore(const Semaphore&) = delete;
  Semaphore& operator=(const Semaphore&) = delete;

  Semaphore(Semaphore&&) = default;
  Semaphore& operator=(Semaphore&&) = default;

  inline void notify() {
    std::lock_guard<std::mutex> lk(mx);
    ++count;
    cv.notify_one();
  }

  inline void wait() {
    std::unique_lock<std::mutex> lk(mx);
    cv.wait(lk, [this]() { return count > 0; });
    --count;
  }
 private:
  std::mutex mx;
  std::condition_variable cv;
  int count;
};


bool randomPassed();
int getRandomInt(int min, int max);

#endif
