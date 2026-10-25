#include <iostream>
#include <stdexcept>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include "d3d12.h"
#include "dxgi1_6.h"
#include "wrl.h"

#include "SDL3/SDL.h"

using namespace Microsoft::WRL;

void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
        throw std::runtime_error("Runtime error in Lux Engine!");
}

int main()
{
    std::cout << "LuxEngine: Windows build\n";
    std::cout << "Windows process ID: " << GetCurrentProcessId() << '\n';
    std::cout << "C++ runtime check passed.\n";

    // Init SDL
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

    // Init D3D12
    UINT dxgiFactoryFlags = 0;
#ifdef LUX_DEBUG
    ComPtr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
    {
        debugController->EnableDebugLayer();
        dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    }
#endif

    ComPtr<IDXGIFactory4> factory;
    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

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
