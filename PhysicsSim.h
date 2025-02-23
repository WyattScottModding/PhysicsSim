#include <chrono>
#include <vector>

class Object;

class PhysicsSim
{
public:
    static std::vector<Object*> objects;
    static float GRAVITY;

    static inline std::chrono::time_point last_update_time = std::chrono::high_resolution_clock::now();
public:
    static void Physics();
};