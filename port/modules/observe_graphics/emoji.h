#pragma once

#include <memory>

namespace observe
{
  class EmojiRasterizerImpl;

  class EmojiRasterizer
  {
  public:
    static EmojiRasterizer &instance()
    {
      static EmojiRasterizer instance;
      return instance;
    }

    void drawIntoBuffer(char32_t emoji, int font_size, int write_width, unsigned int *dest,
                        int dest_width, int x, int y);

  private:
    EmojiRasterizer();
    ~EmojiRasterizer();

    std::unique_ptr<EmojiRasterizerImpl> impl_;
  };
}
