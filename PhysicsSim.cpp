#include "PhysicsSim.h"
#include <chrono>
#include "GraphicsEngine.h"
#include "Object.h"

using namespace std;

float PhysicsSim::GRAVITY = 1119.8f;

vector<Object*> PhysicsSim::objects;

void PhysicsSim::Physics()
{
    auto current_time = chrono::high_resolution_clock::now();
    float tick = chrono::duration<float>(current_time - last_update_time).count();
    
    // Add velocity to the objects
    for (Object *obj : PhysicsSim::objects)
        obj->UpdateVelocity(tick);

    // Update all the positions
    for (Object *obj : PhysicsSim::objects)
        obj->UpdatePosition(tick);

    // Check for object collisions
    for (int i = 0; i < PhysicsSim::objects.size(); i++)
    {
        for (int j = i + 1; j < PhysicsSim::objects.size(); j++)
        {
            if (PhysicsSim::objects.at(i)->CheckCollision(PhysicsSim::objects.at(j)))
            {
                PhysicsSim::objects.at(i)->ResolveCollision(PhysicsSim::objects.at(j));
            }
        }
    }

    last_update_time = chrono::high_resolution_clock::now();
}

int main(int argc, char* argv[])
{
    GraphicsEngine::CreateScreen();
    
    while (true)
    {
        PhysicsSim::Physics();
        
        // Draw the objects
        if (!GraphicsEngine::DrawScreen(PhysicsSim::objects))
            break;
    }
    return 0;
}
