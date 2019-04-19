#include <iostream>

#include "rte/EntryPoint.h"
#include "rte/Scene.h"
#include "rte/GameObject.h"
#include "rte/Component.h"

#include "FloatComponent.h"

void Initialize()
{
    Scene *scene = CreateScene();

    uint64_t floatCompIndex = scene->DefineComponent<FloatComponent>();

    GameObject *go = scene->CreateGameObject();

    FloatComponent *comp = scene->AddComponent<FloatComponent>(floatCompIndex, go);
    comp->Initialize(10);
}
