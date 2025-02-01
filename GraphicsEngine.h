#pragma once
#include <utility>
#include <windows.h>
#include <vector>

class Object;

class GraphicsEngine
{
public:
    static HWND hwnd;
    static HDC hdc;

    static float SquareWidth;

    static int SCREEN_WIDTH;
    static int SCREEN_HEIGHT;
public:
    static void CreateScreen();
    static bool DrawScreen(std::vector<Object*> &objects);

    static void ClearScreen(HDC hdc);
    static void DrawSquare(HDC hdcMem, std::pair<int, int> position, COLORREF color);
};
