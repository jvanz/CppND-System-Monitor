#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid);
  // move contrstuctor needed to work with algorithms functions
  Process(const Process&& p);
  // move assign needed to work with algorithms functions
  Process& operator=(const Process&&);
  Process& operator=(const Process&);
  int Pid() const;
  std::string User() const;
  std::string Command() const;
  float CpuUtilization() const;
  std::string Ram() const;
  long int UpTime() const;
  bool operator<(Process const& a) const;

 private:
  int pid_;
  std::string user_;
  std::string command_;
  std::string ram_;
  long int uptime_;
  float cpu_;
};

#endif
