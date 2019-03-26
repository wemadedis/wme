#pragma once

#include <cstdint>
#include <vector>

#include "rte/Scene.h"

// ENTRY POINT FOR RTE GAMES
void Initialize();
// TODO: (danh) Sun 24/03 - 18:38: Hide this
Scene scene;

Scene *CreateScene()
{
    return &scene;
}


int main(int argc, char const *argv[])
{
    Initialize();

    int maxFrames = 10;
    for (int frameIndex = 0; frameIndex < maxFrames; frameIndex++)
    {
        scene.UpdateComponents();
    }

    return 0;
}
