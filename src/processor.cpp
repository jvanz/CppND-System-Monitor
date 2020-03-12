#include <unistd.h>
#include <iostream>
#include <string>

#include "linux_parser.h"
#include "processor.h"

using std::stoi;

Processor::Processor(const std::vector<std::string> data) {
  user_ = stoi(data[0]);
  nice_ = stoi(data[1]);
  system_ = stoi(data[2]);
  idle_ = stoi(data[3]);
  iowait_ = stoi(data[4]);
  irq_ = stoi(data[5]);
  softirq_ = stoi(data[6]);
  steal_ = stoi(data[7]);
  guest_ = stoi(data[8]);
  guest_nice_ = stoi(data[9]);
}

// Return the aggregate CPU utilization
float Processor::Utilization() {
  return (float)LinuxParser::ActiveJiffies() / LinuxParser::Jiffies();
}
