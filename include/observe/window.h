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

        void *nativeHandle() const;
        SDL_Window *handle() const noexcept;

        int width() const noexcept;
        int height() const noexcept;

    private:
        SDL_Window *m_window = nullptr;

        int m_width = 0;
        int m_height = 0;
    };

}