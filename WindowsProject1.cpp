// WindowsProject1.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "WindowsProject1.h"
#include <windowsx.h> // For GET_X_LPARAM and GET_Y_LPARAM
#include <cmath> // For sqrt function
#include <objidl.h> // For IStream and other COM interfaces required by GDI+
#include <gdiplus.h> // For GDI+ drawing
#pragma comment (lib,"Gdiplus.lib") // Link Gdiplus.lib

#define MAX_LOADSTRING 60

// 可调整参数区域
#define CIRCLE_MAX_DIAMETER 1000 // 圆圈的最大直径（像素）。当鼠标快速移动时，圆圈会放大到此尺寸。
                                 // 调整此值可以改变圆圈的最大显示大小。
#define CIRCLE_MIN_DIAMETER 1 // 圆圈的最小直径（像素）。当鼠标静止或慢速移动时，圆圈会缩小到此尺寸。
                              // 调整此值可以改变圆圈的最小显示大小。
#define PEN_THICKNESS 2 // 圆圈边框的粗细（像素）。
                        // 调整此值可以改变圆圈边框的粗细。

// 窗口尺寸需要比圆圈直径大，以避免边框被裁剪。
// 额外空间 = PEN_THICKNESS * 2 (因为边框内外各一半，所以需要两倍的粗细来完全容纳)
#define WINDOW_SIZE (CIRCLE_MAX_DIAMETER + PEN_THICKNESS * 2)
#define MIN_WINDOW_SIZE (CIRCLE_MIN_DIAMETER + PEN_THICKNESS * 2)

#define TRANSPARENT_COLOR RGB(0, 0, 1) // 窗口的透明色键。此颜色区域将完全透明，允许鼠标穿透。
                                       // 通常不需要调整，除非此颜色与绘图颜色冲突。
#define MICROSOFT_BLUE RGB(0, 120, 215) // 圆圈边框的颜色（微软蓝）。
                                        // 可以调整为其他RGB颜色值，例如 RGB(255, 0, 0) 为红色。

#define IDT_MOUSE_TIMER 1 // 定时器ID，用于内部识别定时器。通常不需要调整。
#define MOUSE_TIMER_INTERVAL 10 // 定时器触发间隔（毫秒）。值越小，鼠标跟随和缩放动画越流畅，但CPU占用可能略高。
                                // 建议范围：10-50毫秒。

// 鼠标速度阈值（像素/秒）。当鼠标速度超过此阈值时，圆圈保持最大尺寸；低于此阈值时，圆圈开始缩小。
// 调整此值可以改变圆圈对鼠标速度的敏感度。值越大，需要更快的鼠标移动才能保持大尺寸。
#define MOUSE_SPEED_THRESHOLD 1000 // 例如：1000像素/秒

// 缩放步长（像素）。每次定时器更新时，圆圈尺寸变化的像素量。
// 调整此值可以改变圆圈缩放的速度。值越小，缩放过程越平滑、越慢。
#define RESIZE_STEP 2 // 建议范围：1-5

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

POINT g_lastMousePos; // Stores the last mouse position for speed calculation
ULONGLONG g_lastUpdateTime; // Stores the last time for speed calculation
int g_currentWindowSize; // Stores the current animated window size

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
// INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM); // No longer needed

using namespace Gdiplus; // Use Gdiplus namespace

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize GDI+
    GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, nullptr);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        GdiplusShutdown(gdiplusToken); // Shutdown GDI+ on failure
        return FALSE;
    }

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    GdiplusShutdown(gdiplusToken); // Shutdown GDI+
    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex{};

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW; // No CS_DROPSHADOW needed for simple transparency
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = CreateSolidBrush(TRANSPARENT_COLOR); // Set background to transparent color key
    wcex.lpszMenuName   = nullptr; // Remove menu
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   HWND hWnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TRANSPARENT, // Always on top, layered, and click-through
       szWindowClass, szTitle, WS_POPUP, // Borderless window
       CW_USEDEFAULT, 0, WINDOW_SIZE, WINDOW_SIZE, // Initial window size based on max circle diameter
       nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   // Set the transparent color key
   SetLayeredWindowAttributes(hWnd, TRANSPARENT_COLOR, 0, LWA_COLORKEY);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   // Initialize global variables for mouse speed tracking and dynamic sizing
   GetCursorPos(&g_lastMousePos);
   g_lastUpdateTime = GetTickCount64(); // 建议使用GetTickCount64()而不是GetTickCount()
   g_currentWindowSize = WINDOW_SIZE; // Start with full window size

   // Set a timer to continuously update the window position and size based on mouse cursor
   SetTimer(hWnd, IDT_MOUSE_TIMER, MOUSE_TIMER_INTERVAL, nullptr);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_TIMER:
    {
        if (wParam == IDT_MOUSE_TIMER)
        {
            // Get mouse coordinates relative to the screen
            POINT pt;
            GetCursorPos(&pt);

            // Calculate mouse speed
            ULONGLONG currentTime = GetTickCount64();
            long deltaTime = static_cast<long>(currentTime - g_lastUpdateTime); // in milliseconds

            if (deltaTime > 0) // Avoid division by zero
            {
                long dx = pt.x - g_lastMousePos.x;
                long dy = pt.y - g_lastMousePos.y;
                double distance = sqrt((double)(dx * dx + dy * dy));
                double speed = (distance / deltaTime) * 1000.0; // pixels per second

                int targetSize;
                int targetCircleDiameter;
                if (speed > MOUSE_SPEED_THRESHOLD)
                {
                    targetCircleDiameter = CIRCLE_MAX_DIAMETER; // Fast movement, keep large
                }
                else
                {
                    targetCircleDiameter = CIRCLE_MIN_DIAMETER; // Slow/still movement, shrink
                }

                // Calculate target window size based on target circle diameter and pen thickness
                targetSize = targetCircleDiameter + PEN_THICKNESS * 2;

                // Smoothly adjust current window size towards target size
                if (g_currentWindowSize < targetSize)
                {
                    g_currentWindowSize += RESIZE_STEP;
                    if (g_currentWindowSize > targetSize) g_currentWindowSize = targetSize;
                }
                else if (g_currentWindowSize > targetSize)
                {
                    g_currentWindowSize -= RESIZE_STEP;
                    if (g_currentWindowSize < targetSize) g_currentWindowSize = targetSize;
                }
            }

            g_lastMousePos = pt;
            g_lastUpdateTime = currentTime;

            // Calculate new window position to center the circle on the mouse
            // The window's top-left corner should be (mouse_x - half_current_window_size, mouse_y - half_current_window_size)
            int x = pt.x - g_currentWindowSize / 2;
            int y = pt.y - g_currentWindowSize / 2;

            // Get current window position and size
            RECT currentWindowRect;
            GetWindowRect(hWnd, &currentWindowRect);

            // Only move/resize if the position or size has changed to avoid flickering
            if (currentWindowRect.left != x || currentWindowRect.top != y ||
                currentWindowRect.right - currentWindowRect.left != g_currentWindowSize ||
                currentWindowRect.bottom - currentWindowRect.top != g_currentWindowSize)
            {
                // Move and resize the window
                SetWindowPos(hWnd, HWND_TOPMOST, x, y, g_currentWindowSize, g_currentWindowSize, SWP_NOACTIVATE);

                // Invalidate the window to force a repaint
                InvalidateRect(hWnd, nullptr, TRUE);
            }
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        // Create a GDI+ Graphics object
        Graphics graphics(hdc);

        // Enable anti-aliasing for smooth drawing
        graphics.SetSmoothingMode(SmoothingModeAntiAlias);

        // Create a GDI+ Pen for the circle border (Microsoft Blue)
        Pen pen(Color(255, GetRValue(MICROSOFT_BLUE), GetGValue(MICROSOFT_BLUE), GetBValue(MICROSOFT_BLUE)), (Gdiplus::REAL)PEN_THICKNESS); // Alpha 255, use defined thickness

        // Create a GDI+ SolidBrush for the transparent interior (same as background color key)
        SolidBrush brush(Color(255, GetRValue(TRANSPARENT_COLOR), GetGValue(TRANSPARENT_COLOR), GetBValue(TRANSPARENT_COLOR)));

        // Get client rect to draw the circle within the window
        RECT clientRect;
        GetClientRect(hWnd, &clientRect);

        // Fill the background with the transparent color first to ensure proper transparency
        graphics.FillRectangle(&brush,
            static_cast<INT>(clientRect.left),
            static_cast<INT>(clientRect.top),
            static_cast<INT>(clientRect.right - clientRect.left),
            static_cast<INT>(clientRect.bottom - clientRect.top));

        // Draw the hollow circle. The ellipse should be drawn slightly inward from the client rect
        // to account for the pen thickness, ensuring it's fully visible.
        // The drawing rectangle should be (left + PEN_THICKNESS / 2, top + PEN_THICKNESS / 2, width - PEN_THICKNESS, height - PEN_THICKNESS)
        graphics.DrawEllipse(&pen,
            static_cast<INT>(clientRect.left + PEN_THICKNESS / 2),
            static_cast<INT>(clientRect.top + PEN_THICKNESS / 2),
            static_cast<INT>(clientRect.right - clientRect.left - PEN_THICKNESS),
            static_cast<INT>(clientRect.bottom - clientRect.top - PEN_THICKNESS));

        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        KillTimer(hWnd, IDT_MOUSE_TIMER); // Kill the timer when the window is destroyed
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
