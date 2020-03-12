#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <string>
#include <vector>

class Processor {
 public:
  Processor()
      : user_(0),
        nice_(0),
        system_(0),
        idle_(0),
        iowait_(0),
        irq_(0),
        softirq_(0),
        steal_(0),
        guest_(0),
        guest_nice_(0){};
  Processor(const std::vector<std::string> data);
  float Utilization();

 private:
  unsigned long long user_;
  unsigned long long nice_;
  unsigned long long system_;
  unsigned long long idle_;
  unsigned long long iowait_;
  unsigned long long irq_;
  unsigned long long softirq_;
  unsigned long long steal_;
  unsigned long long guest_;
  unsigned long long guest_nice_;
};

#endif
