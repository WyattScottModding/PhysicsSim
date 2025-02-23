#include "Object.h"
#include <algorithm>
#include <ostream>

#include "PhysicsSim.h"

using namespace std;

const float TerminalVelocity = 2000.0f;
const float Bounce = 0.4f;
const float InelasticFactor = 0.7f;
const float DynamicFriction = 0.99f;
const float StaticFriction = 0.95f;
const float FrictionThreshold = 0.1f;
const float EnergyLoss = 0.85f;



Object::Object(std::pair<float, float> position, float radius)
{
    this->position = position;
    this->radius = radius;
    this->color = getRandomColor();
    this->mass = 3.14159f * radius * radius;

    this->velocity = make_pair(0, 0);
}

COLORREF Object::getRandomColor()
{
    return RGB(rand() % 256, rand() % 256, rand() % 256);
}

void Object::Draw(HDC hdcMem)
{
    // Set brush color
    HBRUSH hBrush = CreateSolidBrush(color);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdcMem, hBrush);

    // Remove the border by setting pen to null
    HPEN hNullPen = CreatePen(PS_SOLID, 0, RGB(0, 0, 0));
    HPEN hOldPen = (HPEN)SelectObject(hdcMem, hNullPen);

    float bufferRadius = radius + 0.5f;
    

    // Draw the circle using the Ellipse function
    Ellipse(
        hdcMem, 
        static_cast<LONG>(position.first - bufferRadius), // Left
        static_cast<LONG>(position.second - bufferRadius), // Top
        static_cast<LONG>(position.first + bufferRadius), // Right
        static_cast<LONG>(position.second + bufferRadius) // Bottom
    );

    // Cleanup GDI objects
    SelectObject(hdcMem, hOldBrush);
    SelectObject(hdcMem, hOldPen);
    DeleteObject(hBrush);
    DeleteObject(hNullPen);
}


void Object::UpdatePosition(float tick)
{
    // Prevent objects from leaving the screen
    pair<float, float> nextPosition = make_pair(this->position.first + this->velocity.first * tick, this->position.second + this->velocity.second * tick);

    // Update x position
    if (nextPosition.first + radius > static_cast<float>(GraphicsEngine::SCREEN_WIDTH))
    {
        nextPosition.first = static_cast<float>(GraphicsEngine::SCREEN_WIDTH) - radius;

        if (abs(this->velocity.first < 1.0f))
            this->velocity.first = 0.0f;
        else
            this->velocity.first = -this->velocity.first * Bounce;
    }
    else if (nextPosition.first - radius < 0)
    {
        nextPosition.first = radius;

        if (abs(this->velocity.first < 1.0f))
            this->velocity.first = 0.0f;
        else
            this->velocity.first = -this ->velocity.first * Bounce;
    }

    // Update y position
    if (nextPosition.second + radius > static_cast<float>(GraphicsEngine::SCREEN_HEIGHT))
    {
        nextPosition.second = static_cast<float>(GraphicsEngine::SCREEN_HEIGHT) - radius;

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

    // Check for object collisions
    for (Object* obj : PhysicsSim::objects)
    {
        if (obj == this) continue;

        if (CheckCollision(obj))
        {
            ResolveCollision(obj);
        }
    }
}

void Object::UpdateVelocity(float tick)
{
    pair<float, float> nextVelocity; 

    if (this->position.second >= static_cast<float>(GraphicsEngine::SCREEN_HEIGHT) - radius)
    {
        nextVelocity = this->velocity;

        // Friction with the ground
        nextVelocity.first *= (abs(nextVelocity.first) > 1.0f) ? DynamicFriction : StaticFriction;
    }
    else
        nextVelocity = make_pair(this->velocity.first, this->velocity.second + PhysicsSim::GRAVITY * tick);

    // Terminal Velocity
    nextVelocity.first = clamp(nextVelocity.first, -TerminalVelocity, TerminalVelocity);
    nextVelocity.second = clamp(nextVelocity.second, -TerminalVelocity, TerminalVelocity);

    // Adjustment for floating-point errors
    if (abs(nextVelocity.first) < FrictionThreshold)
        nextVelocity.first = 0.0f;
    if (abs(nextVelocity.second) < FrictionThreshold)
        nextVelocity.second = 0.0f;
    
    this->velocity = nextVelocity;
}

bool Object::CheckCollision(Object* other)
{
    float dx = this->position.first - other->position.first;
    float dy = this->position.second - other->position.second;
    float distanceSquared = dx * dx + dy * dy;

    float radiusSum = this->radius + other->radius;
    float radiusSumSquared = radiusSum * radiusSum;

    // Introduce a small epsilon to prevent unwanted gaps
    constexpr float epsilon = 5.0f;
    return distanceSquared <= (radiusSumSquared + epsilon);
}


// Function to check if projections overlap
bool Object::IsOverlap(const std::vector<float>& projectionsA, const std::vector<float>& projectionsB)
{
    float minA = *std::min_element(projectionsA.begin(), projectionsA.end());
    float maxA = *std::max_element(projectionsA.begin(), projectionsA.end());
    float minB = *std::min_element(projectionsB.begin(), projectionsB.end());
    float maxB = *std::max_element(projectionsB.begin(), projectionsB.end());

    return (minA <= maxB) && (minB <= maxA);  // Check if projections overlap
}


void Object::ResolveCollision(Object* other)
{
    // Compute the vector between object centers
    float dx = other->position.first - this->position.first;
    float dy = other->position.second - this->position.second;
    float distance = sqrt(dx * dx + dy * dy);
    
    if (distance == 0.0f) return; // Prevent division by zero

    // Compute collision normal (unit vector)
    float normalX = dx / distance;
    float normalY = dy / distance;

    // Push objects apart based on overlap, considering their masses
    float overlap = (this->radius + other->radius) - distance;
    if (overlap > 0)
    {
        float totalMass = this->mass + other->mass;
        this->position.first -= normalX * (overlap * (other->mass / totalMass));
        this->position.second -= normalY * (overlap * (other->mass / totalMass));

        other->position.first += normalX * (overlap * (this->mass / totalMass));
        other->position.second += normalY * (overlap * (this->mass / totalMass));
    }
    else
    {
        return;
    }

    // Compute velocities along normal direction
    float velocityA = (this->velocity.first * normalX + this->velocity.second * normalY) * EnergyLoss;
    float velocityB = (other->velocity.first * normalX + other->velocity.second * normalY) * EnergyLoss;

    // Calculate new velocities using elastic collision formula with mass
    float totalMass = this->mass + other->mass;
    float newVelocityA = ((this->mass - other->mass) * velocityA + 2 * other->mass * velocityB) / totalMass;
    float newVelocityB = ((other->mass - this->mass) * velocityB + 2 * this->mass * velocityA) / totalMass;

    // Update velocity vectors
    this->velocity.first += (newVelocityA - velocityA) * normalX;
    this->velocity.second += (newVelocityA - velocityA) * normalY;

    other->velocity.first += (newVelocityB - velocityB) * normalX;
    other->velocity.second += (newVelocityB - velocityB) * normalY;

    // Apply friction based on normal force
    float normalForce = distance > 0.0f ? overlap * 1.2f : 0.0f;
    float frictionForce = normalForce * DynamicFriction;

    float tangentX = -normalY;
    float tangentY = normalX;

    this->velocity.first -= tangentX * frictionForce / this->mass;
    this->velocity.second -= tangentY * frictionForce / this->mass;

    other->velocity.first -= tangentX * frictionForce / other->mass;
    other->velocity.second -= tangentY * frictionForce / other->mass;
}
