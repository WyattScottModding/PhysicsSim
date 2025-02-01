#include "GraphicsEngine.h"

#include <iostream>
#include <ostream>

#include "Object.h"
#include "PhysicsSim.h"

using namespace std;

HWND GraphicsEngine::hwnd;
HDC GraphicsEngine::hdc;

int GraphicsEngine::SCREEN_WIDTH = 800;
int GraphicsEngine::SCREEN_HEIGHT = 600;

float GraphicsEngine::SquareWidth = 80.0f;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            EndPaint(hwnd, &ps);
            return 0;
        }
    case WM_KEYDOWN:
        {
            if (wParam == VK_ESCAPE)
            {
                PostQuitMessage(0);
                return 0;
            }
        }
    case WM_LBUTTONDOWN:
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            // Left mouse button is pressed
            printf("Mouse Pressed at (%d, %d)\n", x, y);

            PhysicsSim::objects.push_back(new Object(make_pair((float)x, (float)y), 50.0f));
            
            return 0;
        }

    case WM_LBUTTONUP:
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            // Left mouse button is released
            printf("Mouse Released at (%d, %d)\n", x, y);
            return 0;
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void GraphicsEngine::CreateScreen()
{
    const wchar_t* className = L"GraphicsEngine";
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = className;

    RegisterClass(&wc);

    DWORD windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MAXIMIZEBOX;


    GraphicsEngine::hwnd = CreateWindowEx(0, className, L"Physics Sim", windowStyle,
        CW_USEDEFAULT, CW_USEDEFAULT, SCREEN_WIDTH, SCREEN_HEIGHT, nullptr, nullptr,
        wc.hInstance, nullptr);

    if (hwnd == nullptr)
        std::cout << "Error creating window" << std::endl;

    ShowWindow(hwnd, SW_SHOW);

    RECT clientRect;
    GetClientRect(hwnd, &clientRect);
}

bool GraphicsEngine::DrawScreen(vector<Object*> &objects)
{
    MSG msg = {};
    if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            return false;

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    hdc = GetDC(hwnd);

    HDC hdcMem = CreateCompatibleDC(hdc);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdc, SCREEN_WIDTH, SCREEN_HEIGHT);
    HGDIOBJ oldBitmap = SelectObject(hdcMem, hBitmap);

    ClearScreen(hdcMem);
    //DrawSquare(hdcMem, std::make_pair(400, 40), RGB(20, 200, 20));

    for (Object *object : objects)
        object->Draw(hdcMem);
    
    BitBlt(hdc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, hdcMem, 0, 0, SRCCOPY);

    SelectObject(hdcMem, oldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(hwnd, hdc);
    
    return true;
}

void GraphicsEngine::ClearScreen(HDC hdc)
{
}

void GraphicsEngine::DrawSquare(HDC hdcMem, std::pair<int, int> position, COLORREF color)
{
    LONG left = static_cast<LONG>(position.first - SquareWidth/2);
    LONG top = static_cast<LONG>(position.second - SquareWidth/2);
    LONG right = static_cast<LONG>(position.first + SquareWidth/2);
    LONG bottom = static_cast<LONG>(position.second + SquareWidth/2);

    RECT rect = { left, top, right, bottom };

    SetDCBrushColor(hdcMem, color);
    FillRect(hdcMem, &rect, (HBRUSH)GetStockObject(DC_BRUSH));
}
















