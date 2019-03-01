#include "rte/ECS.h"
#include <iostream>

int totalFrames = 10;
int frameIndex = 0;

#define FLOAT_COMPONENT 1

void main()
{
    ECSCore *ecs = new ECSCore();
    std::cout << "Started" << std::endl;

    // Init ECS
    ecs->AddComponent<float>();

    // Every frame
    for(; frameIndex < totalFrames; frameIndex++)
    {
        ecs->Update();
    }
    std::cout << "Ended" << std::endl;
}
