#pragma once

#include <observe/windowing/window.h>
#include <observe/graphics/renderer.h>

#include <observe/imgui/imgui_context.h>
#include <observe/imgui/imgui_renderer.h>

namespace observe
{

    class Application
    {
    public:
        bool initialize();

        void run();

    private:
        Window m_window;
        Renderer m_renderer;

        ImGuiContext m_imgui;
        ImGuiRenderer m_imguiRenderer;
    };

}