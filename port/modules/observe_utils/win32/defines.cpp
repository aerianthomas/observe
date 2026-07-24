#include "defines.h"

#include "string_utils.h"

#ifndef NOMINMAX
#define NOMINMAX 1
#endif
#include <windows.h>

namespace observe
{
  void debugLogString(const char *file, unsigned int line, const String &log_message)
  {
    OutputDebugStringA((std::string(file) + " (" + std::to_string(line) + ") ").c_str());
    OutputDebugStringW(log_message.toWide().c_str());
    if (log_message.isEmpty() || log_message[log_message.size() - 1] != '\n')
      OutputDebugStringW(L"\n");
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
    __debugbreak();
  }

  void forceCrash()
  {
    __debugbreak();
  }
}
