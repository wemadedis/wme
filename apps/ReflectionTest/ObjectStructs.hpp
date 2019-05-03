#pragma once

#include "rte/RTE.hpp";
#include "PlayerController.hpp"

using namespace RTE::StdComponents;
using namespace RTE::Runtime;


struct ComponentIds {
    ComponentId transformIndex;
    ComponentId meshIndex;
    ComponentId playerControllerIndex;
    ComponentId cameraIndex;
    ComponentId pointLightIndex;
};

struct Player {
    GameObject *go;
    TransformComponent *tc;
    CameraComponent *cc;
    PlayerController *pc;
};

struct SimpleTransform {
    glm::vec3 pos;
    glm::vec3 rot;
    glm::vec3 scale;
};

struct Monkey {
    GameObject *go;
    TransformComponent *tc;
    MeshComponent *mc;
};

struct PLight {
    GameObject *go;
    TransformComponent *tc;
    PointLightComponent *plc;
};

struct Floor {
    GameObject *go;
    TransformComponent *tc;
    MeshComponent *mc;
};

struct DirectionalLight {
    GameObject *go;
    
};