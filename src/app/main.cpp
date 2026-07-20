#include <observe/app/application.h>
// #include <iostream>

int main()
{
    observe::Application app;

    if (!app.initialize())
    {
        // std::cout << "Application initialization failed\n";
        // std::cin.get();
        return -1;
    }

    app.run();

    return 0;
}