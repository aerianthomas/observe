#pragma once

namespace observe
{

    class Window;

    class ImGuiContext
    {
    public:
        ImGuiContext() = default;
        ~ImGuiContext();

        bool initialize(
            Window &window);

        void shutdown();

        void beginFrame();
        void endFrame();

    private:
        bool m_initialized = false;
    };

}