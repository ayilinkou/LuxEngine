#include <iostream>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include "SDL3/SDL.h"

int main()
{
    std::cout << "LuxEngine: Windows build\n";
    std::cout << "Windows process ID: " << GetCurrentProcessId() << '\n';
    std::cout << "C++ runtime check passed.\n";

    bool bSuccess = SDL_Init(SDL_INIT_VIDEO);
    if (!bSuccess)
    {
        std::cout << "SDL_Init() failed!\n";
        std::cout << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Window* pWindow = SDL_CreateWindow("Lux Engine", 1280, 720, SDL_WINDOW_RESIZABLE);
    if (!pWindow)
    {
        std::cout << "SDL_CreateWindow() failed!\n";
        std::cout << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    bool bRunning = true;
    while (bRunning)
    {
        SDL_Event event{};
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT || event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
            {
                bRunning = false;
            }
        }

        SDL_Delay(16);
    }

    SDL_DestroyWindow(pWindow);
    SDL_Quit();

    return 0;
}
