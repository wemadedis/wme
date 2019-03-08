#pragma once

#include <cstdint>
#include <vector>

typedef uint64_t GameObject;

void Initialize();

class Scene
{
    std::vector<GameObject> gameObjects;
};

class Component
{
    virtual void Update(); 
};

int main(int argc, char const *argv[])
{
    Initialize();

    return 0;
}
