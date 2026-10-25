#include <iostream>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

int main()
{
    std::cout << "LuxEngine: Windows build\n";
    std::cout << "Windows process ID: " << GetCurrentProcessId() << '\n';
    std::cout << "C++ runtime check passed.\n";
    return 0;
}
