#ifndef CLERK_H_
#define CLERK_H_

#include "utils.h"

class Clerk {
 public:
  Clerk(int id_);
  void operator()(Semaphore& clerkDone);
 private:
  int id;
};


#endif
