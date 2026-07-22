#pragma once

namespace observe { class Window; }

namespace imgui_demo
{

    class ImGuiContext
    {
    public:
        ImGuiContext() = default;
        ~ImGuiContext();

        bool initialize(
            observe::Window &window);

        void shutdown();

        void beginFrame();
        void endFrame();

    private:
        bool m_initialized = false;
    };

}
