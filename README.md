# 多线程同步的实例

## 问题描述

甜筒店里进来10个顾客，每个顾客(customer)购买若干个甜筒(cone)，可以假设最少购买1个，最多购买4个。每一个甜筒都需要一个店员(clerk)去做(假设clerk的数量足够多)，店员做完每一个甜筒之后，需要向经理(manager)申请检测，检测合格后才可以交给顾客。但是经理每次只能接受一个店员的申请，检测一个甜筒，并以一定的概率通过这个甜筒是否合格。经理需要等到所有顾客要求的甜筒合格后才可以下班走人。顾客等待他所需要的所有的甜筒做完之后，他要去收银员(cashier)那里付钱。顾客必须排队，而收银员按照排队的顺序依次给顾客结账。顾客结完账后可以顺利走人。

## 问题分析

### Manager vs. Clerk

**分析一**：有一个manager线程和多个clerk线程，因为manager一次只能处理一个clerk的请求，处理完这个clerk的请求后，需要返回给该clerk(而不是其他的clerk)一个结果(clerk做的甜筒是否合格)。在这个clerk得到结果之前不能有别的clerk请求manager，否则，这个结果会被污染。所以整个clerk请求manager以及manager返回结果给clerk的过程，是一个临界区，需要一个互斥量(mutex)，每一个clerk想进入这个临界区必须先得到这个互斥量的锁。

**分析二**：manager线程和clerk线程之间存在两个同步问题，manager在等待clerk申请(request)，clerk需要向manager发出申请(request)，这是第一个同步。除此之外，manager在完成(finish)检测之后，需要通知clerk已经完成。与此同时clerk也在等待manager完成(finish)检测的信号，这是第二个同步。基于上述分析，这里需要两个信号量(Semaphore)来处理这两个同步问题。

基于分析一和分析二，我们可以定义这样一个数据结构，这个数据结构包含一个`mutex`变量`mx`，两个`Semaphore`，分别是`requested`和`finished`，以及一个存储检测结果的变量`passed`。

```c++
struct Inspection {
  Inspection(bool passed_, int requested_count, int finished_count);
  Inspection();
  bool passed;
  Semaphore requested;
  Semaphore finished;
  std::mutex mx;
};
```

`manager`线程的伪代码：

```c++
void Manager(int totalConesNeeded) {
  int numApproved = 0;
  int numInspected = 0;
  while (numApproved < totalConesNeeded) {
    sw(inspection.requested);
    ++numInspected;
    inspection.passed = RandomChance(0, 1);
    if (inspection.passed) ++numApproved;
    ss(inspection.finished);
  }
}
```

`clerk`的伪代码：

```c++
void clerk(Semaphore semaphoreToSignal) {
  bool passed = false;
  while (not passed) {
    MakeCone();
    lock(inspection.mx);
    ss(inspection.requested);
    sw(inspection.finished);
    passed = inspection.passed;
    unlock(inspection.mx);
  }
  ss(semaphoreToSignal);
}
```

伪代码说明：

- `sw`表示线程等待在某个信号量上，`ss`表示唤醒等待在该信号量上的其他线程。
- `inspection`中两个信号量`requested`和`finished`都应该初始化为0。
- 在clerk线程中临界区，上面分析一种已经讨论过必须要加锁🔒。获得锁的clerk线程完成整个请求后解锁🔓。
- 在整个临界区中，clerk线程先是唤醒manager，请求manager检查，然后等待manager结束检查，最后得到检查结果。
- `ss(semaphoreToSignal)`在下面会讨论，其实是和customer线程的一个同步，在通过manager检测后通知customer线程，甜筒🍦已经做好。

### Customer vs. Clerk

**分析一**：因为一个customer会买若干个🍦，每个🍦都需要一个clerk去做，所以一个customer线程会对应若干个clerk线程。

**分析二**：customer线程和clerk线程是一对多的关系，manager线程和clerk线程也是一对多的关系。但是它们是有区别的。第一，customer线程是发起请求的线程，而manager线程是等待请求的线程；第二，前一对线程中的clerk线程只需要各自通知customer线程，通知的过程中，除了信号量之外，不涉及其他数据竞争。不像后一对线程，clerk线程需要等待manager的请求结果完成，才能允许别的clerk再次请求；第三，customer线程需要等待所有clerk线程发出🍦已经做好的信号，才可以继续下一步操作。

**分析三**：customer要买`numCones`个甜筒，那么，只需要等待`numCones`次clerk线程的回复就可以，而不需要知道是哪一个clerk线程回复的。

customer线程伪代码：

```c++
void Customer(int numCones) {
  BrowseInShop();
  Semaphore clerkDone(0);
  for (int i = 0; i < numCones; ++i) {
    ThreadNew(Clerk, clerkDone);
  }
  for (int i = 0; i < numCones; ++i) {
    sw(clerkDone);
  }
  ...
}
```

代码分析：

- 一个customer线程会启动`numCones`个clerk线程。`for`循环。
- 初始化一个信号量`clerkDone`为零。
- 无差别等待所有的该customer启动的clerk线程完成。一共等待同一个信号量`numCones`次。

### Cashier vs. Customer

**分析一**：因为我们问题中已经说明有10个customer线程，所以，有一个cashier线程，有10个customer线程。

**分析二**：cashier线程和customer线程也是一对多线程的模式。多个customer线程需要领取排队的号码，所以存在数据竞争。这个领取号码牌的过程，形成临界区，所有需要一个互斥量，或者一个二值信号量(binary semaphore)。

**分析三**：cashier线程和customer线程通信过程中，有两个同步。与manager线程和clerk线程不同之处是，cashiere线程和customre线程通信过程中没有结果产生。这是两者关键的不同处。不存在结果污染的问题。

**分析四**：cashier线程依顺序处理排队的customer，处理完成后通知排在那个位置上的customer。所以customer线程等待在位置相关的信号量上。注意，我们需要考虑，假如cashier线程如果处理完`place`位置上的请求，并且通知`place`位置上的customer，是否存在一种可能，在某个customer线程等待在`place`位置上的信号量之前，cashier线程已经向`place`位置的 信号量发出通知。我们可以有很多方式来实现信号量，来完成线程间的通信，具体参考[c++线程间通信](http://192.168.199.107:10080/w/zhangli_mainpage/%E7%BA%BF%E7%A8%8B%E9%97%B4%E9%80%9A%E4%BF%A1/)。

根据上述的分析，可以定义一个这样的数据结构：

```c++
struct Line {
  Line(int number_, int requested, int customer, int lock);
  int number;
  Semaphore requested;
  Semaphore customer[10];
  Semaphore lock;
};
```

初始化一个`Line`对象`line(0, 0, 0, 1)`。`lock`信号量初始化为1，相当于一个互斥量。

cashier线程伪代码：

```c++
void Cashier() {
  for (int i = 0; i < 10; ++i) {
    sw(line.requested);
    checkout(i);
    ss(line.customer[i]);
  }
}
```

cashier线程代码比较简单，依次按顺序处理每个排队上的请求，处理完再通知该位置上的的customer。

customer线程的伪代码：

```c++
void Customer(int numCones) {
  BrowseInShop();
  Semaphore clerkDone(0);
  for (int i = 0; i < numCones; ++i) {
    ThreadNew(Clerk, clerkDone);
  }
  for (int i = 0; i < numCones; ++i) {
    sw(clerkDone);
  }
  
  WalkToCashier();
  
  sw(line.lock);
  int place = line.number++;
  ss(line.lock);
  
  ss(line.requested);
  sw(line.customer[place]);
  
  GoHome();
}
```

代码分析：

- 每个customer线程需要得到一个锁，进入临界区，拿到当前位置的号码牌。
- `sw(line.customer[place])`排队等待。

到这里为止所有线程之间的过程已经分析完毕。

### Main 函数

main函数伪代码：

```c++
int main() {
  int totalCones = 0;
  for (int i = 0; i < 10; ++i) {
    int numCones = RandomInteger(1, 4);
    ThreadNew(Customer, numCones);
    totalCones += numCones;
  }
  
  ThreadNew(Cashier);
  ThreadNew(Manager totalCones);
  
  return 0;
}
```

## C++代码实现

[1]: https://github.com/xiaoxinyi/manager-clerk-customer-cashier	"C++代码实现"
