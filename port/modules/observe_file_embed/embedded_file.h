#pragma once

namespace observe
{
  struct EmbeddedFile
  {
    const char *name = nullptr;
    const unsigned char *data = nullptr;
    int size = 0;
  };
}
