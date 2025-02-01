#include "Object.h"
#include <algorithm>
#include <iostream>
#include <ostream>

#include "PhysicsSim.h"

using namespace std;

const float TerminalVelocity = 500.0f;
const float Bounce = 0.4f;

const int Floor = GraphicsEngine::SCREEN_HEIGHT - 40;

Object::Object(std::pair<float, float> position, float width)
{
    this->position = position;
    this->width = width;
    this->color = getRandomColor();

    this->velocity = make_pair(0, 0);
}

COLORREF Object::getRandomColor()
{
    return RGB(rand() % 256, rand() % 256, rand() % 256);
}

void Object::Draw(HDC hdcMem)
{
    LONG left = static_cast<LONG>(position.first - width/2);
    LONG top = static_cast<LONG>(position.second - width/2);
    LONG right = static_cast<LONG>(position.first + width/2);
    LONG bottom = static_cast<LONG>(position.second + width/2);

    RECT rect = { left, top, right, bottom };

    SetDCBrushColor(hdcMem, color);
    FillRect(hdcMem, &rect, (HBRUSH)GetStockObject(DC_BRUSH));
}

void Object::UpdatePosition(float tick)
{
    // Prevent objects from leaving the screen
    pair<float, float> nextPosition = make_pair(this->position.first + this->velocity.first * tick, this->position.second + this->velocity.second * tick);

    // Update x position
    if (nextPosition.first > static_cast<float>(GraphicsEngine::SCREEN_WIDTH))
    {
        nextPosition.first = static_cast<float>(GraphicsEngine::SCREEN_WIDTH);

        if (abs(this->velocity.first < 1.0f))
            this->velocity.first = 0.0f;
        else
            this->velocity.first = -this->velocity.first * Bounce;
    }
    else if (nextPosition.first < 0)
    {
        nextPosition.first = 0.0f;

        if (abs(this->velocity.first < 1.0f))
            this->velocity.first = 0.0f;
        else
            this->velocity.first = -this ->velocity.first * Bounce;
    }

    // Update y position
    if (nextPosition.second + width/2 > static_cast<float>(Floor))
    {
        nextPosition.second = static_cast<float>(Floor) - width/2;

        if (abs(this->velocity.second < 1.0f))
            this->velocity.second = 0.0f;
        else
            this->velocity.second = -this->velocity.second * Bounce;
    }
    else if (nextPosition.second < 0)
    {
        nextPosition.second = 0.0f;

        if (abs(this->velocity.second < 1.0f))
            this->velocity.second = 0.0f;
        else
            this->velocity.second = -this->velocity.second * Bounce;
    }
    
    this->position = nextPosition;
}

void Object::UpdateVelocity(float tick)
{
    pair<float, float> nextVelocity;

    if (this->position.second >= Floor - width/2)
        nextVelocity = this->velocity;
    else
        nextVelocity = make_pair(this->velocity.first, this->velocity.second + PhysicsSim::GRAVITY * tick);

    nextVelocity.first = clamp(nextVelocity.first, -TerminalVelocity, TerminalVelocity);
    nextVelocity.second = clamp(nextVelocity.second, -TerminalVelocity, TerminalVelocity);

    if (abs(nextVelocity.first) < 0.1f)
        nextVelocity.first = 0.0f;
    if (abs(nextVelocity.second) < 0.1f)
        nextVelocity.second = 0.0f;

    cout << "velocity: " << nextVelocity.first << ", " << nextVelocity.second << endl;
    
    this->velocity = nextVelocity;
}