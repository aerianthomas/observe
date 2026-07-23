#include <observe/app/application.h>

#include "canvas_demo_layer.h"

namespace
{

    int canvasDemoMain()
    {
        observe::Application app;
        canvas_demo::CanvasDemoLayer canvasLayer;

        app.addLayer(&canvasLayer);

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
    return canvasDemoMain();
}

int main()
{
    return canvasDemoMain();
}

#else

int main()
{
    return canvasDemoMain();
}

#endif
