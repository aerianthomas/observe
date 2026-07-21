#include <observe/app/application.h>

int observeMain()
{
    observe::Application app;

    if (!app.initialize())
    {
        return -1;
    }

    app.run();

    return 0;
}

#ifdef _WIN32

#include <windows.h>

int WINAPI WinMain(
    HINSTANCE,
    HINSTANCE,
    LPSTR,
    int)
{
    return observeMain();
}

int main()
{
    return observeMain();
}

#else

int main()
{
    return observeMain();
}

#endif