#pragma once

#include <cstdint>
#include <vector>

#include "rte/Scene.h"

// ENTRY POINT FOR RTE GAMES
void Initialize();
Scene *scene;
Scene *CreateScene()
{
    scene = new Scene();
    return scene;
}


int main(int argc, char const *argv[])
{
    Initialize();

    int maxFrames = 10;
    for (int frameIndex = 0; frameIndex < maxFrames; frameIndex++)
    {
        scene->UpdateComponents();
    }

    return 0;
}
