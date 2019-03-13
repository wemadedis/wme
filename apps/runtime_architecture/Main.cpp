#include <iostream>

#include "rte/EntryPoint.h"
#include "rte/Scene.h"
#include "rte/GameObject.h"
#include "rte/Component.h"

#include "FloatComponent.h"

void Initialize()
{   
    Scene* scene = CreateScene();
    u64 floatCompID = scene->DefineComponent<FloatComponent>();
    GameObject* go = scene->CreateGameObject();
    scene->AddComponentTo(floatCompID, go->GetId());
}