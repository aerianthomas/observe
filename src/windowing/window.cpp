#include <observe/windowing/window.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_properties.h>

// #include <iostream>

namespace observe
{

    Window::~Window()
    {
        destroy();
    }

    bool Window::create(const std::string &title,
                        int width,
                        int height)
    {
        m_width = width;
        m_height = height;

        if (!SDL_Init(SDL_INIT_VIDEO))
            return false;

        m_window = SDL_CreateWindow(
            title.c_str(),
            width,
            height,
            SDL_WINDOW_RESIZABLE);

        return m_window != nullptr;
    }

    void Window::destroy()
    {
        if (m_window)
        {
            SDL_DestroyWindow(m_window);
            m_window = nullptr;
        }

        SDL_Quit();
    }

    bool Window::pollEvents()
    {
        SDL_Event event;

        while (SDL_PollEvent(&event))
        {
            for (const EventListener &listener : m_eventListeners)
                listener(event);

            switch (event.type)
            {
            case SDL_EVENT_QUIT:
                return false;
                /*SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED rather than
                SDL_EVENT_WINDOW_RESIZED — the latter reports the
                window's logical/point size, while bgfx's swapchain
                needs actual framebuffer pixels. On Windows these
                are usually equal, but it's the technically correct
                event and it costs nothing.*/
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                m_width = event.window.data1;
                m_height = event.window.data2;
                m_sizeChanged = true;
                break;

            default:
                break;
            }
        }

        return true;
    }

    bool Window::consumeResize(int &outWidth, int &outHeight) noexcept
    {
        if (!m_sizeChanged)
            return false;

        outWidth = m_width;
        outHeight = m_height;
        m_sizeChanged = false;

        return true;
    }

    void Window::addEventListener(EventListener listener)
    {
        m_eventListeners.push_back(std::move(listener));
    }

    void *Window::nativeHandle() const
    {
        SDL_PropertiesID props = SDL_GetWindowProperties(m_window);

        void *hwnd = SDL_GetPointerProperty(
            props,
            SDL_PROP_WINDOW_WIN32_HWND_POINTER,
            nullptr);

        // std::cout << "SDL_Window = " << m_window << '\n';
        // std::cout << "HWND       = " << hwnd << '\n';

        return hwnd;
    }

    SDL_Window *Window::handle() const noexcept
    {
        return m_window;
    }

    int Window::width() const noexcept
    {
        return m_width;
    }

    int Window::height() const noexcept
    {
        return m_height;
    }

}
