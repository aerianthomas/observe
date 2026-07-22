#include <observe/app/application.h>

#include "imgui_layer.h"

namespace
{

    int imguiDemoMain()
    {
        observe::Application app;
        imgui_demo::ImGuiLayer imguiLayer;

        app.addLayer(&imguiLayer);

        if (!app.initialize())
        {
            return -1;
        }

        app.run();

        return 0;
    }

}

#ifdef _WIN32

#include <windows.h>

int WINAPI WinMain(
    HINSTANCE,
    HINSTANCE,
    LPSTR,
    int)
{
    return imguiDemoMain();
}

int main()
{
    return imguiDemoMain();
}

#else

int main()
{
    return imguiDemoMain();
}

#endif
