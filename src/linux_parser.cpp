#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

static std::istringstream ReadFileLine(std::ifstream& filestream,
                                       const string& prefix) {
  if (filestream.is_open()) {
    string line;
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      string line_prefix;
      linestream >> line_prefix;
      if (line_prefix == prefix) {
        return linestream;
      }
    }
  }
  return std::istringstream("");
}

static std::ifstream GetProcessFile(const int pid, const string& filename) {
  return std::ifstream(LinuxParser::kProcDirectory + std::to_string(pid) +
                       filename);
}

static vector<string> ReadPidStat(int pid) {
  vector<string> values;
  std::ifstream stream = GetProcessFile(pid, LinuxParser::kStatFilename);
  if (stream.is_open()) {
    string line;
    std::getline(stream, line);
    std::istringstream linestream(line);
    string value;
    while (linestream >> value) {
      values.push_back(value);
    }
  }
  return values;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> os >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  int total = 0;
  int free = 0;
  int cache = 0;
  int sreclaimable = 0;
  int shmem = 0;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    string line;
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      string line_prefix;
      linestream >> line_prefix;
      if (line_prefix == "MemTotal:") {
        linestream >> total;
      } else if (line_prefix == "MemFree:") {
        linestream >> free;
      } else if (line_prefix == "Cached:") {
        linestream >> cache;
      } else if (line_prefix == "SReclaimable:") {
        linestream >> sreclaimable;
      } else if (line_prefix == "Shmem:") {
        linestream >> shmem;
      }
    }
  }
  auto used = total - free;
  return (float)(used - (cache + sreclaimable + shmem)) / total;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  long uptime = 0;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime;
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  auto cpu = LinuxParser::CpuUtilization();
  long total = 0;
  for (auto value : cpu) {
    total += std::stol(value);
  }
  return total;
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  auto values = ReadPidStat(pid);
  return std::stol(values[13]) + std::stol(values[14]) + std::stol(values[15]) +
         std::stol(values[16]);
}

// Read and return the number of active jiffies for a PID
long LinuxParser::TotalJiffies(int pid) {
  auto values = ReadPidStat(pid);
  long total = 0;
  for (auto value : values) {
    total += std::stol(value);
  }
  return total;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  return LinuxParser::Jiffies() - LinuxParser::IdleJiffies();
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  auto cpu = LinuxParser::CpuUtilization();
  return std::stol(cpu[3]) + std::stol(cpu[4]);
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpu_usage;
  std::ifstream stream(kProcDirectory + kStatFilename);
  std::istringstream linestream = ReadFileLine(stream, "cpu");
  string usage;
  while (linestream >> usage) {
    cpu_usage.push_back(usage);
  }
  return cpu_usage;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  int total_processes = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  std::istringstream linestream = ReadFileLine(stream, "processes");
  linestream >> total_processes;
  return total_processes;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int running_processes = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  std::istringstream linestream = ReadFileLine(stream, "procs_running");
  linestream >> running_processes;
  return running_processes;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  std::ifstream stream = GetProcessFile(pid, kCmdlineFilename);
  std::string command;
  if (stream.is_open()) {
    std::getline(stream, command);
    // remove the null char
    std::replace(command.begin(), command.end(), '\0', ' ');
  }
  return command;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  std::ifstream stream = GetProcessFile(pid, kStatusFilename);
  long vmsize;
  if (stream.is_open()) {
    auto line = ReadFileLine(stream, "VmSize:");
    line >> vmsize;
  }
  if (vmsize == 0) return std::to_string(vmsize);
  return std::to_string(vmsize / 1000);
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  std::ifstream stream = GetProcessFile(pid, kStatusFilename);
  string uid;
  if (stream.is_open()) {
    auto line = ReadFileLine(stream, "Uid:");
    line >> uid;
  }
  return uid;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string uid = LinuxParser::Uid(pid);
  string user;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    string line;
    string x;
    string id;
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> user >> x >> id;
      if (id == uid) {
        break;
      }
    }
  }
  return user;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  auto values = ReadPidStat(pid);
  // return the uptime seconds
  return LinuxParser::UpTime() - (stol(values[21]) / sysconf(_SC_CLK_TCK));
}
