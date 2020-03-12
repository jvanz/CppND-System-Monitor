#include <unistd.h>
#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {
  command_ = LinuxParser::Command(pid);
  ram_ = LinuxParser::Ram(pid);
  user_ = LinuxParser::User(pid);
  uptime_ = LinuxParser::UpTime(pid);
  long seconds = LinuxParser::UpTime() -
                 (LinuxParser::UpTime(pid_) / sysconf(_SC_CLK_TCK));
  cpu_ = (float)(LinuxParser::ActiveJiffies(pid_) / sysconf(_SC_CLK_TCK)) /
         seconds;
}

Process::Process(const Process&& p) {
  pid_ = p.Pid();
  command_ = p.Command();
  ram_ = p.Ram();
  user_ = p.User();
  uptime_ = p.UpTime();
  cpu_ = p.CpuUtilization();
}

Process& Process::operator=(const Process&& p) {
  pid_ = p.Pid();
  command_ = p.Command();
  ram_ = p.Ram();
  user_ = p.User();
  uptime_ = p.UpTime();
  cpu_ = p.CpuUtilization();
  return *this;
}

// Return this process's ID
int Process::Pid() const { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() const { return cpu_; };

// Return the command that generated this process
string Process::Command() const { return command_; }

// Return this process's memory utilization
string Process::Ram() const { return ram_; }

// Return the user (name) that generated this process
string Process::User() const { return user_; }

// Return the age of this process (in seconds)
long int Process::UpTime() const { return uptime_; }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
  return CpuUtilization() < a.CpuUtilization();
}
