#include <unistd.h>
#include <cstddef>
#include <iostream>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

System::System() {
  kernel_ = LinuxParser::Kernel();
  operating_system_ = LinuxParser::OperatingSystem();
  running_processes_ = LinuxParser::RunningProcesses();
  total_processes_ = LinuxParser::TotalProcesses();
  uptime_ = LinuxParser::UpTime();
  memory_utilizaion_ = LinuxParser::MemoryUtilization();
  cpu_ = Processor(LinuxParser::CpuUtilization());
}

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }

// Return a container composed of the system's processes
vector<Process>& System::Processes() {
  processes_.clear();
  auto pids = LinuxParser::Pids();
  for (auto pid : pids) {
    processes_.emplace_back(pid);
  }
  std::sort(processes_.begin(), processes_.end());
  std::reverse(processes_.begin(), processes_.end());
  return processes_;
}

// Return the system's kernel identifier (string)
std::string System::Kernel() { return kernel_; }

// Return the system's memory utilization
float System::MemoryUtilization() { return memory_utilizaion_; }

// Return the operating system name
std::string System::OperatingSystem() { return operating_system_; }

// Return the number of processes actively running on the system
int System::RunningProcesses() { return running_processes_; }

// Return the total number of processes on the system
int System::TotalProcesses() { return total_processes_; }

// Return the number of seconds since the system started running
long int System::UpTime() { return uptime_; }
