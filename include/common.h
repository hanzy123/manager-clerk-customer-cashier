#include "utils.h"
#include <mutex>
#include <chrono>
#include <vector>

extern std::mutex cout_mx;
extern const int num_customer;

struct Line {
  Line(int number_, int requested, int customer, int lock);
  int number;
  Semaphore requested;
  std::vector<std::unique_ptr<Semaphore>> customer;
  Semaphore lock;
};


struct Inspection {
  Inspection(bool passed_, int requested_count, int finished_count);
  Inspection();
  bool passed;
  Semaphore requested;
  Semaphore finished;
  std::mutex mx;
};

extern Inspection inspection;
extern Line line;
