#pragma once

#include <functional>
#include <string>
#include <vector>

struct SDL_Window;
union SDL_Event;

namespace observe
{

    class Window
    {
    public:
        // Window currently wraps SDL3 directly (see handle(), below), so
        // this callback is intentionally SDL-typed rather than some
        // abstracted observe::Event - there's no backend-independent event
        // type in observe yet. If observe ever grows a proper
        // cross-backend windowing abstraction, this signature would need
        // to change alongside it.
        using EventListener = std::function<void(const SDL_Event &)>;

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

        // Registers a callback invoked once per SDL event, for every event
        // pollEvents() sees - e.g. so an ImGui backend can forward events
        // to ImGui_ImplSDL3_ProcessEvent() without Window needing to know
        // ImGui exists. Listeners are never unregistered individually;
        // whatever a listener captures must stay alive for as long as this
        // Window does.
        void addEventListener(EventListener listener);

        void *nativeHandle() const;
        SDL_Window *handle() const noexcept;

        int width() const noexcept;
        int height() const noexcept;

    private:
        SDL_Window *m_window = nullptr;

        int m_width = 0;
        int m_height = 0;

        bool m_sizeChanged = false;

        std::vector<EventListener> m_eventListeners;
    };

}
