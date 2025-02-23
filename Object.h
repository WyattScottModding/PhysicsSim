#pragma once
#include <utility>

#include "GraphicsEngine.h"

class Object
{
public:
    std::pair<float, float> position;
    float radius;
    COLORREF color;
    float mass;

    std::pair<float, float> velocity;
public:
    Object(std::pair<float, float> position, float radius);

    COLORREF getRandomColor();
    
    void Draw(HDC hdcMem);

    void UpdatePosition(float tick);
    void UpdateVelocity(float tick);
    bool CheckCollision(Object* other);
    void ResolveCollision(Object* other);
    bool IsOverlap(const std::vector<float>& projectionsA, const std::vector<float>& projectionsB);
};
