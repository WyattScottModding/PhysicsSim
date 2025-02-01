#pragma once
#include <utility>

#include "GraphicsEngine.h"

class Object
{
public:
    std::pair<float, float> position;
    float width;
    COLORREF color;

    std::pair<float, float> velocity;
public:
    Object(std::pair<float, float> position, float width);

    COLORREF getRandomColor();
    
    void Draw(HDC hdcMem);

    void UpdatePosition(float tick);
    void UpdateVelocity(float tick);
};
