#include <ctime>
#include <iostream>
#include <string>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) {
  time_t time(seconds);
  auto dt = gmtime(&time);
  return std::to_string(dt->tm_hour) + ":" + std::to_string(dt->tm_min) + ":" +
         std::to_string(dt->tm_sec);
}
