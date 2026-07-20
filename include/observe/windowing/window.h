#pragma once

#include <string>

struct SDL_Window;

namespace observe
{

    class Window
    {
    public:
        Window() = default;
        ~Window();

        bool create(const std::string &title,
                    int width,
                    int height);

        void destroy();

        bool pollEvents();

        // Returns true (once) if the window's pixel size has changed since
        // the last call, and writes the new size into outWidth/outHeight.
        bool consumeResize(int &outWidth, int &outHeight) noexcept;

        void *nativeHandle() const;
        SDL_Window *handle() const noexcept;

        int width() const noexcept;
        int height() const noexcept;

    private:
        SDL_Window *m_window = nullptr;

        int m_width = 0;
        int m_height = 0;

        bool m_sizeChanged = false;
    };

}