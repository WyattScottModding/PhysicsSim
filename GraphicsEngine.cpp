#include "GraphicsEngine.h"

#include <algorithm>
#include <iostream>
#include <ostream>

#include "Object.h"
#include "PhysicsSim.h"

using namespace std;

HWND GraphicsEngine::hwnd;
HDC GraphicsEngine::hdc;

int GraphicsEngine::SCREEN_WIDTH = 1200;
int GraphicsEngine::SCREEN_HEIGHT = 800;

float GraphicsEngine::SquareWidth = 80.0f;
Object* GraphicsEngine::HoldingObject;
float GraphicsEngine::HoldingObjectSize = 10;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    //cout << "Current state: " << uMsg << endl;
    switch (uMsg)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            GraphicsEngine::DrawScreen(PhysicsSim::objects);
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
            break;
        }
    case WM_LBUTTONDOWN:
        {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);

            if (!GraphicsEngine::HoldingObject)
                GraphicsEngine::HoldingObject = new Object(make_pair((float)x, (float)y), GraphicsEngine::HoldingObjectSize);
            else
                GraphicsEngine::HoldingObject->position = make_pair((float)x, (float)y);

            SetCapture(hwnd);
            return 0;
        }
    case WM_MOUSEMOVE:
        {
            if (wParam & MK_LBUTTON)
            {
                int x = LOWORD(lParam);
                int y = HIWORD(lParam);

                if (GraphicsEngine::HoldingObject)
                {
                    GraphicsEngine::HoldingObject->position = make_pair((float)x, (float)y);
                    GraphicsEngine::HoldingObject->radius = GraphicsEngine::HoldingObjectSize;
                    GraphicsEngine::HoldingObject->mass = 3.14159f * GraphicsEngine::HoldingObjectSize * GraphicsEngine::HoldingObjectSize;

                }
            }
            break;
        }

    case WM_LBUTTONUP:
        {
            PhysicsSim::objects.push_back(GraphicsEngine::HoldingObject);
            GraphicsEngine::HoldingObject = nullptr;
            ReleaseCapture();
            
            return 0;
        }
    case WM_SIZE:
        {
            // Extract the new width and height
            int newWidth = LOWORD(lParam);
            int newHeight = HIWORD(lParam);

            // Update the screen size variables
            GraphicsEngine::SCREEN_WIDTH = newWidth;
            GraphicsEngine::SCREEN_HEIGHT = newHeight;

            PhysicsSim::Physics();
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
    case WM_MOVE:
        {
            PhysicsSim::Physics();
            InvalidateRect(hwnd, nullptr, FALSE);
            return 0;
        }
    case WM_ENTERSIZEMOVE:
        {
            SetTimer(hwnd, 1, 16, NULL);  // ~60 FPS
            return 0;
        }
    case WM_EXITSIZEMOVE:
        {
            KillTimer(hwnd, 1);
            return 0;
        }
    case WM_TIMER:
        {
            // Continue physics and rendering during resize-hold
            PhysicsSim::Physics();
            GraphicsEngine::DrawScreen(PhysicsSim::objects);
            return 0;
        }
    case WM_MOUSEWHEEL:
        {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);

            GraphicsEngine::HoldingObjectSize = clamp(GraphicsEngine::HoldingObjectSize + 0.05f * delta, 10.0f, 80.0f);

            if (GraphicsEngine::HoldingObject)
            {
                GraphicsEngine::HoldingObject->radius = GraphicsEngine::HoldingObjectSize;
                GraphicsEngine::HoldingObject->mass = 3.14159f * GraphicsEngine::HoldingObjectSize * GraphicsEngine::HoldingObjectSize;
            }
            GraphicsEngine::DrawScreen(PhysicsSim::objects);
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

    // Allow resizing by adding WS_THICKFRAME
    DWORD windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME;

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

    for (Object *object : objects)
        object->Draw(hdcMem);

    if (GraphicsEngine::HoldingObject)
        GraphicsEngine::HoldingObject->Draw(hdcMem);
    
    BitBlt(hdc, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, hdcMem, 0, 0, SRCCOPY);

    SelectObject(hdcMem, oldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(hwnd, hdc);
    
    return true;
}

void GraphicsEngine::ClearScreen(HDC hdc)
{
    RECT rect;
    GetClientRect(hwnd, &rect);
    FillRect(hdc, &rect, (HBRUSH)GetStockObject(LTGRAY_BRUSH));
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