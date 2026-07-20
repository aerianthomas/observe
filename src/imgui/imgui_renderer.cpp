#include <observe/imgui/imgui_renderer.h>

#include <imgui.h>

#include <bgfx/bgfx.h>
#include <bx/math.h>

#include <cstring>

namespace observe
{

    namespace
    {
        // ImTextureID is a 64-bit value (see imconfig.h). We pack a bgfx
        // texture handle into it together with a non-zero marker byte, so a
        // valid texture whose handle.idx happens to be 0 never collides with
        // ImTextureID_Invalid (which is 0).
        struct PackedTexture
        {
            bgfx::TextureHandle handle;
            uint16_t marker;
            uint32_t padding;
        };

        static_assert(
            sizeof(PackedTexture) == sizeof(ImTextureID),
            "PackedTexture must be exactly as large as ImTextureID");

        ImTextureID pack(bgfx::TextureHandle handle)
        {
            PackedTexture packed{handle, 1, 0};

            ImTextureID id;
            std::memcpy(&id, &packed, sizeof(id));

            return id;
        }

        bgfx::TextureHandle unpack(ImTextureID id)
        {
            PackedTexture packed;
            std::memcpy(&packed, &id, sizeof(packed));

            return packed.handle;
        }
    }

    ImGuiRenderer::~ImGuiRenderer()
    {
        shutdown();
    }

    bool ImGuiRenderer::initialize()
    {
        ImGuiIO &io = ImGui::GetIO();

        // Tells ImGui: (1) we can handle draw commands with VtxOffset > 0,
        // which lets it keep using 16-bit indices for large meshes, and
        // (2) we manage textures ourselves via ImTextureData requests
        // (WantCreate / WantUpdates / WantDestroy) rather than the old
        // single-texture GetTexDataAsRGBA32()/SetTexID() path.
        io.BackendFlags |=
            ImGuiBackendFlags_RendererHasVtxOffset |
            ImGuiBackendFlags_RendererHasTextures;

        m_layout
            .begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
            .end();

        if (!m_shader.load(
                "shaders/vs_imgui.bin",
                "shaders/fs_imgui.bin"))
        {
            return false;
        }

        m_texUniform =
            bgfx::createUniform(
                "s_texColor",
                bgfx::UniformType::Sampler);

        if (!bgfx::isValid(m_texUniform))
        {
            return false;
        }

        m_initialized = true;

        return true;
    }

    void ImGuiRenderer::render()
    {
        if (!m_initialized)
            return;

        ImDrawData *drawData = ImGui::GetDrawData();

        if (!drawData)
            return;

        // Honor any pending texture create/update/destroy requests before
        // submitting draw calls that reference them.
        if (drawData->Textures != nullptr)
        {
            for (ImTextureData *texData : *drawData->Textures)
            {
                switch (texData->Status)
                {
                case ImTextureStatus_WantCreate:
                {
                    bgfx::TextureHandle handle =
                        bgfx::createTexture2D(
                            static_cast<uint16_t>(texData->Width),
                            static_cast<uint16_t>(texData->Height),
                            false,
                            1,
                            bgfx::TextureFormat::RGBA8,
                            0);

                    bgfx::setName(handle, "ImGui Texture");

                    bgfx::updateTexture2D(
                        handle,
                        0,
                        0,
                        0,
                        0,
                        static_cast<uint16_t>(texData->Width),
                        static_cast<uint16_t>(texData->Height),
                        bgfx::copy(
                            texData->GetPixels(),
                            static_cast<uint32_t>(texData->GetSizeInBytes())));

                    texData->SetTexID(pack(handle));
                    texData->SetStatus(ImTextureStatus_OK);

                    break;
                }

                case ImTextureStatus_WantUpdates:
                {
                    bgfx::TextureHandle handle =
                        unpack(texData->GetTexID());

                    const uint32_t bpp =
                        static_cast<uint32_t>(texData->BytesPerPixel);

                    const uint32_t pitch =
                        static_cast<uint32_t>(texData->GetPitch());

                    for (const ImTextureRect &rect : texData->Updates)
                    {
                        const bgfx::Memory *mem =
                            bgfx::alloc(rect.w * rect.h * bpp);

                        const uint8_t *src =
                            static_cast<const uint8_t *>(
                                texData->GetPixelsAt(rect.x, rect.y));

                        for (int row = 0; row < rect.h; ++row)
                        {
                            std::memcpy(
                                mem->data + row * rect.w * bpp,
                                src + row * pitch,
                                rect.w * bpp);
                        }

                        bgfx::updateTexture2D(
                            handle,
                            0,
                            0,
                            rect.x,
                            rect.y,
                            rect.w,
                            rect.h,
                            mem);
                    }

                    texData->SetStatus(ImTextureStatus_OK);

                    break;
                }

                case ImTextureStatus_WantDestroy:
                {
                    bgfx::TextureHandle handle =
                        unpack(texData->GetTexID());

                    if (bgfx::isValid(handle))
                        bgfx::destroy(handle);

                    texData->SetTexID(ImTextureID_Invalid);
                    texData->SetStatus(ImTextureStatus_Destroyed);

                    break;
                }

                default:
                    break;
                }
            }
        }

        // Avoid rendering when minimized, and scale for retina-style
        // displays where screen coordinates != framebuffer pixels.
        const int32_t width =
            static_cast<int32_t>(
                drawData->DisplaySize.x * drawData->FramebufferScale.x);

        const int32_t height =
            static_cast<int32_t>(
                drawData->DisplaySize.y * drawData->FramebufferScale.y);

        if (width <= 0 || height <= 0)
            return;

        bgfx::setViewName(m_viewId, "ImGui");
        bgfx::setViewMode(m_viewId, bgfx::ViewMode::Sequential);

        {
            float ortho[16];

            const float x = drawData->DisplayPos.x;
            const float y = drawData->DisplayPos.y;
            const float w = drawData->DisplaySize.x;
            const float h = drawData->DisplaySize.y;

            bx::mtxOrtho(
                ortho,
                x,
                x + w,
                y + h,
                y,
                0.0f,
                1000.0f,
                0.0f,
                bgfx::getCaps()->homogeneousDepth);

            bgfx::setViewTransform(m_viewId, nullptr, ortho);

            bgfx::setViewRect(
                m_viewId,
                0,
                0,
                static_cast<uint16_t>(width),
                static_cast<uint16_t>(height));
        }

        const ImVec2 clipPos = drawData->DisplayPos;
        const ImVec2 clipScale = drawData->FramebufferScale;

        for (int i = 0; i < drawData->CmdListsCount; ++i)
        {
            const ImDrawList *drawList = drawData->CmdLists[i];

            const uint32_t numVertices =
                static_cast<uint32_t>(drawList->VtxBuffer.size());

            const uint32_t numIndices =
                static_cast<uint32_t>(drawList->IdxBuffer.size());

            bgfx::TransientVertexBuffer tvb;
            bgfx::TransientIndexBuffer tib;

            if (!bgfx::allocTransientBuffers(
                    &tvb,
                    m_layout,
                    numVertices,
                    &tib,
                    numIndices,
                    sizeof(ImDrawIdx) == 4))
            {
                // Not enough space left in the transient buffers this
                // frame; drop the remaining draw lists rather than crash.
                break;
            }

            std::memcpy(
                tvb.data,
                drawList->VtxBuffer.Data,
                numVertices * sizeof(ImDrawVert));

            std::memcpy(
                tib.data,
                drawList->IdxBuffer.Data,
                numIndices * sizeof(ImDrawIdx));

            for (const ImDrawCmd &cmd : drawList->CmdBuffer)
            {
                if (cmd.UserCallback)
                {
                    cmd.UserCallback(drawList, &cmd);
                    continue;
                }

                if (cmd.ElemCount == 0)
                    continue;

                ImVec4 clipRect;
                clipRect.x = (cmd.ClipRect.x - clipPos.x) * clipScale.x;
                clipRect.y = (cmd.ClipRect.y - clipPos.y) * clipScale.y;
                clipRect.z = (cmd.ClipRect.z - clipPos.x) * clipScale.x;
                clipRect.w = (cmd.ClipRect.w - clipPos.y) * clipScale.y;

                if (clipRect.x >= static_cast<float>(width) ||
                    clipRect.y >= static_cast<float>(height) ||
                    clipRect.z < 0.0f ||
                    clipRect.w < 0.0f)
                {
                    continue;
                }

                const uint16_t x =
                    static_cast<uint16_t>(bx::max(clipRect.x, 0.0f));

                const uint16_t y =
                    static_cast<uint16_t>(bx::max(clipRect.y, 0.0f));

                bgfx::setScissor(
                    x,
                    y,
                    static_cast<uint16_t>(bx::min(clipRect.z, 65535.0f) - x),
                    static_cast<uint16_t>(bx::min(clipRect.w, 65535.0f) - y));

                bgfx::setState(
                    BGFX_STATE_WRITE_RGB |
                    BGFX_STATE_WRITE_A |
                    BGFX_STATE_MSAA |
                    BGFX_STATE_BLEND_FUNC(
                        BGFX_STATE_BLEND_SRC_ALPHA,
                        BGFX_STATE_BLEND_INV_SRC_ALPHA));

                bgfx::setTexture(
                    0,
                    m_texUniform,
                    unpack(cmd.GetTexID()));

                bgfx::setVertexBuffer(
                    0,
                    &tvb,
                    cmd.VtxOffset,
                    numVertices);

                bgfx::setIndexBuffer(
                    &tib,
                    cmd.IdxOffset,
                    cmd.ElemCount);

                bgfx::submit(m_viewId, m_shader.handle());
            }
        }
    }

    void ImGuiRenderer::shutdown()
    {
        if (!m_initialized)
            return;

        // ImGuiRenderer is destroyed before ImGuiContext (see Application's
        // member order), so the ImGui context - and its font atlas
        // ImTextureData - are still alive here. Destroy whatever GPU
        // textures we created for it explicitly, since no further render()
        // call will come along to process a WantDestroy request for them.
        for (ImTextureData *texData : ImGui::GetPlatformIO().Textures)
        {
            bgfx::TextureHandle handle = unpack(texData->GetTexID());

            if (bgfx::isValid(handle))
            {
                bgfx::destroy(handle);

                texData->SetTexID(ImTextureID_Invalid);
                texData->SetStatus(ImTextureStatus_Destroyed);
            }
        }

        if (bgfx::isValid(m_texUniform))
        {
            bgfx::destroy(m_texUniform);
            m_texUniform = BGFX_INVALID_HANDLE;
        }

        m_shader.destroy();

        m_initialized = false;
    }

}