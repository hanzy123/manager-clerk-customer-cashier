#ifndef CUSTOMER_H_
#define CUSTOMER_H_

class Customer {
 public:
  Customer(int id_);
  void operator()(int numCones);
 private:
  int id;
};

#endif
