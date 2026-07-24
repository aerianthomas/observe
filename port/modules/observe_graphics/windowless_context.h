#pragma once

namespace observe
{
#if OBSERVE_MAC
  void *windowlessContext();
#else
  static void *windowlessContext()
  {
    return nullptr;
  }
#endif
}
