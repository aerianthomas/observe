#pragma once

#include <chrono>
#include <string>

namespace observe::time
{
  typedef std::chrono::time_point<std::chrono::system_clock> Time;

  inline long long milliseconds()
  {
    auto now = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
  }

  inline long long microseconds()
  {
    auto now = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::microseconds>(now).count();
  }

  inline int seconds()
  {
    auto now = std::chrono::system_clock::now().time_since_epoch();
    return std::chrono::duration_cast<std::chrono::seconds>(now).count();
  }

  inline Time now()
  {
    return std::chrono::system_clock::now();
  }

  std::string formatTime(const Time &time, const char *format_string);
}
