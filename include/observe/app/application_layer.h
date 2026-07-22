#pragma once

namespace observe
{

    class Window;

    // Extension point for per-frame update/render logic that plugs into
    // Application's loop without Application needing to know what the
    // layer actually is - e.g. a debug/tooling overlay (see
    // examples/imgui_demo), and eventually observe::ui itself.
    //
    // Application never owns a layer's lifetime. Whoever constructs a
    // layer registers it via Application::addLayer(), keeps it alive for
    // as long as it's registered, and destroys it afterwards.
    class ApplicationLayer
    {
    public:
        virtual ~ApplicationLayer() = default;

        // Called once, after Window/Renderer are ready but before the
        // first frame. Return false to abort Application::initialize().
        virtual bool initialize(Window &window)
        {
            (void)window;
            return true;
        }

        // Called once, after the run() loop exits.
        virtual void shutdown() {}

        // Called once per frame, before the renderer draws its own
        // content - build/update this layer's state here.
        virtual void update() {}

        // Called once per frame, after the renderer's own content is
        // submitted but before the frame is presented, so this layer's
        // own draw calls land in the same bgfx frame.
        virtual void render() {}
    };

}
