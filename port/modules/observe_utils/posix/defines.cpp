#include "defines.h"

#include "string_utils.h"

#include <iostream>

namespace observe
{
  inline void debugLogString(const char *file, unsigned int line, const String &log_message)
  {
    std::cerr << file << " (" << line << ") " << log_message.toUtf8();
    if (log_message.isEmpty() || log_message[log_message.size() - 1] != '\n')
      std::cerr << std::endl;
  }

  void debugLogArgs(const char *file, unsigned int line, const char *format, va_list arg_list)
  {
    static constexpr int kSize = 500;
    char buffer[kSize];
    std::vsnprintf(buffer, sizeof(buffer), format, arg_list);
    debugLogString(file, line, buffer);
  }

  void debugAssert(bool condition, const char *file, unsigned int line)
  {
    if (condition)
      return;

    debugLogString(file, line, "Assertion failed");
#if defined(__clang__)
    __builtin_debugtrap();
#else
    __builtin_trap();
#endif
  }

  void forceCrash()
  {
    __builtin_trap();
  }
}