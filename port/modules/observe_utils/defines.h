#pragma once

#include <cstdarg>

namespace observe
{
  constexpr float kPi = 3.14159265358979323846f;

  class String;
  void debugLogString(const char *file, unsigned int line, const String &log_message);
  void debugLogArgs(const char *file, unsigned int line, const char *format, va_list arg_list);

  template <typename T>
  void debugLog(const char *file, unsigned int line, T message, ...)
  {
    debugLogString(file, line, message);
  }

  template <>
  inline void debugLog(const char *file, unsigned int line, const char *format, ...)
  {
    va_list args;
    va_start(args, format);
    debugLogArgs(file, line, format, args);
    va_end(args);
  }

  void debugAssert(bool condition, const char *file, unsigned int line);
  void forceCrash();
}

#define OBSERVE_FORCE_CRASH() observe::forceCrash()

#ifndef NDEBUG

#define OBSERVE_LOG(log, ...) observe::debugLog(__FILE__, int(__LINE__), log, ##__VA_ARGS__)

#define OBSERVE_ASSERT(condition) observe::debugAssert((condition), __FILE__, int(__LINE__))
#define no_except

namespace observe
{
  template <typename T>
  class InstanceCounter
  {
  public:
    static InstanceCounter &instance()
    {
      static InstanceCounter instance;
      return instance;
    }

    ~InstanceCounter() { OBSERVE_ASSERT(count_ == 0); }

    void add() { count_++; }
    void remove() { count_--; }

  private:
    int count_ = 0;
  };

  template <typename T>
  class LeakChecker
  {
  public:
    LeakChecker() { InstanceCounter<T>::instance().add(); }

    LeakChecker(const LeakChecker &other) { InstanceCounter<T>::instance().add(); }

    ~LeakChecker() { InstanceCounter<T>::instance().remove(); }
  };
}

#define OBSERVE_LEAK_CHECKER(className)             \
  friend class observe::InstanceCounter<className>; \
  static const char *vaLeakCheckerName()           \
  {                                                \
    return #className;                             \
  }                                                \
  observe::LeakChecker<className> leakChecker;

#else
#define OBSERVE_ASSERT(x) ((void)0)
#define OBSERVE_LOG(x, ...) ((void)0)
#define OBSERVE_LEAK_CHECKER(className)
#define no_except noexcept
#endif

#if OBSERVE_WINDOWS
#define OBSERVE_STDCALL __stdcall
#else
#define OBSERVE_STDCALL
#endif