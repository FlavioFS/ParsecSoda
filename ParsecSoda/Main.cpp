// Dear ImGui: standalone example application for DirectX 11
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_colors.h"
#include "imgui/implot.h"
#include <d3d11.h>
#include <tchar.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <SDL.h>
#include "resource.h"
#include "Hosting.h"
#include "Texture.h"
#include "globals/AppIcons.h"
#include "globals/AppFonts.h"
#include "globals/AppColors.h"
#include "Widgets/LoginWidget.h"
#include "Widgets/NavBar.h"
#include "Widgets/HostInfoWidget.h"
#include "Widgets/HostSettingsWidget.h"
#include "Widgets/ChatWidget.h"
#include "Widgets/LogWidget.h"
#include "Widgets/GuestListWidget.h"
#include "Widgets/GamepadsWidget.h"
#include "Widgets/StylePickerWidget.h"
#include "Widgets/AudioSettingsWidget.h"
#include "Widgets/VideoWidget.h"
#include "Widgets/InfoWidget.h"
#include "Widgets/VersionWidget.h"
#include "Widgets/ThumbnailsWidget.h"
#include "Widgets/MasterOfPuppetsWidget.h"

using namespace std;

// Data
static ID3D11Device*            g_pd3dDevice = NULL;
static ID3D11DeviceContext*     g_pd3dDeviceContext = NULL;
static IDXGISwapChain*          g_pSwapChain = NULL;
static ID3D11RenderTargetView*  g_mainRenderTargetView = NULL;
Hosting g_hosting;

// Forward declarations of helper functions
bool CreateDeviceD3D(HWND hWnd);
void CleanupDeviceD3D();
void CreateRenderTarget();
void CleanupRenderTarget();
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Main code
int CALLBACK WinMain( _In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();

    MetadataCache::loadPreferences();

    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_CLASSDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0L;
    wc.cbWndExtra = 0L;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
    wc.hCursor = NULL;
    wc.hbrBackground = NULL;
    wc.lpszMenuName = NULL;
    wc.lpszClassName = _T("Parsec Soda");
    wc.hIconSm = NULL;
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(
        wc.lpszClassName, _T("Parsec Soda"), WS_OVERLAPPEDWINDOW,
        MetadataCache::preferences.windowX, MetadataCache::preferences.windowY,
        MetadataCache::preferences.windowW, MetadataCache::preferences.windowH,
        NULL, NULL, wc.hInstance, NULL
    );

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImPlot::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

    // =====================================================================
    // 
    //  Initialize modules
    // 
    // =====================================================================
    AppIcons::init(g_pd3dDevice);
    AppFonts::init(io);
    AppColors::init();
    g_hosting.init();

    HostSettingsWidget hostSettingsWindow(g_hosting, [&hwnd](bool isRunning) {
            SetWindowTextW(hwnd, isRunning ? L"⚫ [LIVE] Parsec Soda" : L"Parsec Soda");
    });
    LoginWidget loginWindow(g_hosting, hostSettingsWindow);
    LogWidget logWindow(g_hosting);
    GuestListWidget guestsWindow(g_hosting);
    GamepadsWidget gamepadsWindow(g_hosting);
    AudioSettingsWidget audioSettingswidget(g_hosting);
    VideoWidget videoWidget(g_hosting);
    HostInfoWidget hostInfoWidget(g_hosting);
    MasterOfPuppetsWidget masterOfPuppets(g_hosting);
    
    FLASHWINFO fi;
    fi.cbSize = sizeof(FLASHWINFO);
    fi.hwnd = hwnd;
    fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
    fi.uCount = 0;
    fi.dwTimeout = 0;
    ChatWidget chatWindow(g_hosting, [&hwnd, &fi]() {
        FlashWindowEx(&fi);
    });

    ImVec4 clear_color = ImVec4(0.01f, 0.01f, 0.01f, 1.00f);
    ImGui::loadStyle();

    bool showHostSettings = true;
    bool showChat = true;
    bool showLog = true;
    bool showGuests = true;
    bool showGamepads = true;
    bool showMasterOfPuppets = false;
    bool showAudio = false;
    bool showVideo = false;
    bool showStyles = true;
    bool showInfo = false;
    bool showLogin = true;
    bool showThumbs = false;

    ParsecSession& g_session = g_hosting.getSession();

    thread t;
    t = thread([&]() {
        g_hosting.fetchAccountData(true);
        showLogin = !g_hosting.getSession().isValid();
        t.detach();
    });

    // =====================================================================

    // Main loop
    bool done = false;
    while (!done)
    {
        // Poll and handle messages (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();


        // =====================================================================
        // 
        //  Window rendering
        // 
        // =====================================================================
        ImGui::SetMouseCursor(ImGuiMouseCursor_Arrow);

        VersionWidget::render();

        if (showLogin)
        {
            loginWindow.render(showLogin);
        }
        else
        {
            if (showHostSettings)       hostSettingsWindow.render();
            if (showChat)               chatWindow.render();
            if (showLog)                logWindow.render();
            if (showGuests)             guestsWindow.render();
            if (showGamepads)           gamepadsWindow.render();
            if (showMasterOfPuppets)    masterOfPuppets.render();
            if (showAudio)              audioSettingswidget.render();
            if (showVideo)              videoWidget.render();
            if (showInfo)               InfoWidget::render();
            if (showThumbs)             ThumbnailsWidget::render(g_session);
            NavBar::render(
                g_hosting,
                showLogin, showHostSettings, showGamepads, showMasterOfPuppets, showChat,
                showGuests, showThumbs, showLog, showAudio, showVideo, showInfo
            );
            hostInfoWidget.render();
        }

        //if (showStyles)         StylePickerWidget::render();

        //ImGui::ShowDemoWindow();
        //ImPlot::ShowDemoWindow();

        // =====================================================================

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0); // Present with vsync
        //g_pSwapChain->Present(0, 0); // Present without vsync
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;    
    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_QUIT:
    case WM_DESTROY:
        g_hosting.release();
        RECT windowRect;
        if (GetWindowRect(hWnd, &windowRect))
        {
            MetadataCache::preferences.windowX = windowRect.left;
            MetadataCache::preferences.windowY = windowRect.top;
            MetadataCache::preferences.windowW = windowRect.right - windowRect.left;
            MetadataCache::preferences.windowH = windowRect.bottom - windowRect.top;
            MetadataCache::savePreferences();
        }
        Sleep(1000);
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}
