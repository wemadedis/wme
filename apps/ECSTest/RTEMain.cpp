#include <iostream>

#include "rte/ECSCore.h"
#include "MultiplyByTwoSystem.h"
#include "FloatComponent.h"
#include "RTE.h"

int totalFrames = 10;
int frameIndex = 0;

void main()
{
    ECSCore *ecs = new ECSCore();
    std::cout << "Started" << std::endl;

    // Add components to ECSCore, gives back the MASK for this comp
    FloatComponent* f = new FloatComponent();
    uint64_t floatMask = ecs->AddComponent<FloatComponent>(*f);
    

    // Declare systems
    MultiplyByTwoSystem* mult2System = new MultiplyByTwoSystem(ecs, floatMask);

    ecs->AddSystem(mult2System);
    
    // Add entity
    uint64_t e1 = ecs->GetFreeEntity();

    ecs->Mask[e1] = floatMask;

    // Every frame
    for(; frameIndex < totalFrames; frameIndex++)
    {
        ecs->Update();
    }
    std::cout << "Ended" << std::endl;
}
