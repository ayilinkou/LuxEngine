#include <array>
#include <iostream>
#include <stdexcept>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

#include "directx/d3d12.h"
#include "dxguids/dxguids.h"
#include "directx/d3dx12.h"
#include "dxgi1_6.h"
#include "wrl.h"

#include "SDL3/SDL.h"
#include "SDL3/SDL_properties.h"
#include "SDL3/SDL_video.h"

constexpr UINT MAX_FRAMES_IN_FLIGHT = 2u;
constexpr UINT WIDTH = 1280u;
constexpr UINT HEIGHT = 720u;

using namespace Microsoft::WRL;

inline void ThrowIfFailed(HRESULT hr)
{
    if (FAILED(hr))
        throw std::runtime_error("Runtime error in Lux Engine!");
}

void GetHardwareAdapter(IDXGIFactory1* pFactory, IDXGIAdapter1** ppAdapter,
                        bool requestHighPerformanceAdapter = false)
{
    *ppAdapter = nullptr;

    ComPtr<IDXGIAdapter1> adapter;
    ComPtr<IDXGIFactory6> factory6;
    if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
    {
        for (UINT adapterIndex = 0u; SUCCEEDED(factory6->EnumAdapterByGpuPreference(
                 adapterIndex,
                 requestHighPerformanceAdapter ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE
                                               : DXGI_GPU_PREFERENCE_UNSPECIFIED,
                 IID_PPV_ARGS(&adapter)));
             adapterIndex++)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // don't select the Basic Render Driver adapter
                continue;
            }

            // check if the adapter supports D3D12, but don't create the actual device yet
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0,
                                            __uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }

    if (!adapter.Get())
    {
        for (UINT adapterIndex = 0u; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter));
             adapterIndex++)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // don't select the Basic Render Driver adapter
                continue;
            }

            // check if the adapter supports D3D12, but don't create the actual device yet
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0,
                                            __uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }

    *ppAdapter = adapter.Detach();
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

    SDL_Window* pWindow = SDL_CreateWindow("Lux Engine", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE);
    if (!pWindow)
    {
        std::cout << "SDL_CreateWindow() failed!\n";
        std::cout << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    HWND hwnd = static_cast<HWND>(SDL_GetPointerProperty(
        SDL_GetWindowProperties(pWindow), SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));

    // Init D3D12
    // Create device
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

    ComPtr<ID3D12Device> device;

    bool bUseWarpDevice = false;
    if (bUseWarpDevice)
    {
        ComPtr<IDXGIAdapter> warpAdapter;
        ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
        ThrowIfFailed(
            D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device)));
    }
    else
    {
        ComPtr<IDXGIAdapter1> hardwareAdapter;
        GetHardwareAdapter(factory.Get(), &hardwareAdapter);
        ThrowIfFailed(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_12_0,
                                        IID_PPV_ARGS(&device)));
    }

    // Create command queue
    ComPtr<ID3D12CommandQueue> commandQueue;
    D3D12_COMMAND_QUEUE_DESC queueDesc{};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    ThrowIfFailed(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)));

    // Create swapchain
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
    swapChainDesc.BufferCount = MAX_FRAMES_IN_FLIGHT;
    swapChainDesc.Width = WIDTH;
    swapChainDesc.Height = HEIGHT;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1u;

    ComPtr<IDXGISwapChain1> swapChain1;
    ThrowIfFailed(factory->CreateSwapChainForHwnd(commandQueue.Get(), hwnd, &swapChainDesc, nullptr,
                                                  nullptr, &swapChain1));

    // no fullscreen transitions
    ThrowIfFailed(factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));

    ComPtr<IDXGISwapChain3> swapChain;
    ThrowIfFailed(swapChain1.As(&swapChain));
    UINT frameIndex = swapChain->GetCurrentBackBufferIndex();

    // Create descriptor heaps
    ComPtr<ID3D12DescriptorHeap> rtvHeap;
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
    rtvHeapDesc.NumDescriptors = MAX_FRAMES_IN_FLIGHT;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    ThrowIfFailed(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)));

    UINT rtvDescriptorSize =
        device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // Create frame resources
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle{};
    rtvHeap->GetCPUDescriptorHandleForHeapStart(&rtvHandle);

    std::array<ComPtr<ID3D12Resource>, MAX_FRAMES_IN_FLIGHT> renderTargets;
    for (UINT i = 0u; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        ThrowIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i])));
        device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.Offset(1, rtvDescriptorSize);
    }

    // Create command allocator
    ComPtr<ID3D12CommandAllocator> commandAllocator;
    ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                 IID_PPV_ARGS(&commandAllocator)));

    // Create root signature
    // Create pipeline state
    // Create command list
    // Create vertex buffer
    // Create sync objects

    // D3d12 Init complete

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
