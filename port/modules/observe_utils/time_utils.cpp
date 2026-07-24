#include "time_utils.h"

namespace observe::time
{
  std::string formatTime(const Time &time, const char *format_string)
  {
    static constexpr int kMaxLength = 100;

    auto t = std::chrono::system_clock::to_time_t(time);
    tm time_info{};
#if OBSERVE_WINDOWS
    localtime_s(&time_info, &t);
#else
    localtime_r(&t, &time_info);
#endif

    char buffer[kMaxLength];
    if (std::strftime(buffer, kMaxLength, format_string, &time_info) == 0)
      return "";

    return buffer;
  }
}
