#pragma once

#include <observe/app/application_layer.h>

#include "imgui_context.h"
#include "imgui_renderer.h"

namespace imgui_demo
{

    // Thin adapter that lets the ImGui backend plug into Application via
    // the generic ApplicationLayer interface, so Application itself never
    // needs to know ImGui exists.
    class ImGuiLayer : public observe::ApplicationLayer
    {
    public:
        bool initialize(observe::Window &window) override;
        void shutdown() override;

        void update() override;
        void render() override;

    private:
        // Declaration order matters: ImGuiRenderer must be destroyed
        // before ImGuiContext, since it needs the ImGui context (and its
        // font atlas ImTextureData) still alive to clean up the GPU
        // textures it owns. Members destruct in reverse declaration
        // order, so m_renderer is declared after m_context.
        ImGuiContext m_context;
        ImGuiRenderer m_renderer;
    };

}
