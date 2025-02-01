#include "PhysicsSim.h"

#include <chrono>
#include <iostream>
#include <ostream>

#include "GraphicsEngine.h"

#include "Object.h"

using namespace std;

float PhysicsSim::GRAVITY = 1119.8f;

vector<Object*> PhysicsSim::objects;

int main(int argc, char* argv[])
{
    GraphicsEngine::CreateScreen();

    float tick = 1.0 / 60.0;
    
    while (true)
    {
        auto current_time = chrono::high_resolution_clock::now();
        
        // Draw the objects
        if (!GraphicsEngine::DrawScreen(PhysicsSim::objects))
            break;

        // Add velocity to the objects
        for (Object *obj : PhysicsSim::objects)
            obj->UpdateVelocity(tick);

        // Update all the positions
        for (Object *obj : PhysicsSim::objects)
            obj->UpdatePosition(tick);
        


        auto end_time = chrono::high_resolution_clock::now();
        tick = chrono::duration<float>(end_time - current_time).count();

        //cout << tick << endl;
    }
    return 0;
}
