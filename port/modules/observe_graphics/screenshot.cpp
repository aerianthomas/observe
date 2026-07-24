#include "screenshot.h"

#include <bimg/bimg.h>
#include <bx/file.h>

namespace observe
{
  void Screenshot::save(const char *path) const
  {
    bx::FileWriter writer;
    bx::Error error;
    if (bx::open(&writer, path, false, &error))
    {
      bimg::imageWritePng(&writer, width_, height_, width_ * 4, data_.data(),
                          bimg::TextureFormat::RGBA8, false, &error);
      bx::close(&writer);
    }
  }

  void Screenshot::save(const std::string &path) const
  {
    save(path.c_str());
  }
}