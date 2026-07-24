#pragma once

#include <string>

namespace observe
{
  static constexpr int kDefaultChildProcessTimeoutMs = 10000;
  static constexpr size_t kMaxOutputSize = 1024 * 1024;

  bool spawnChildProcess(const std::string &command, const std::string &arguments,
                         std::string &output, int timeout_ms = kDefaultChildProcessTimeoutMs);
}
