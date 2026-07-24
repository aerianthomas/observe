int runExample();

#if OBSERVE_WINDOWS
#include <windows.h>
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int)
{
  return runExample();
}
#else
int main(int, char **)
{
  return runExample();
}
#endif
