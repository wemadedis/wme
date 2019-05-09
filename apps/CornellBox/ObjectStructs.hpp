#pragma once

#include "rte/RTE.hpp";
#include "PlayerController.hpp"
#include "Rotator.hpp"

using namespace RTE::StdComponents;
using namespace RTE::Runtime;


struct ComponentIds {
    ComponentId transformIndex;
    ComponentId meshIndex;
    ComponentId playerControllerIndex;
    ComponentId cameraIndex;
    ComponentId pointLightIndex;
    ComponentId audioIndex;
    ComponentId listenerIndex;
    ComponentId directLightIndex;
    ComponentId rotatorIndex;
};

struct Player {
    GameObject *go;
    TransformComponent *tc;
    CameraComponent *cc;
    PlayerController *pc;
    ListenerComponent *lc;
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
    AudioComponent *ac;
    Rotator *rc;
    const int verts = 507;
    const int faces = 500;
    const int triangles = 968;
};

struct PLight {
    GameObject *go;
    TransformComponent *tc;
    PointLightComponent *plc;
};

struct Box {
    GameObject *go;
    TransformComponent *tc;
    MeshComponent *mc;
};

struct Wall {
    GameObject *go;
    TransformComponent *tc;
    MeshComponent *mc;
};

struct DirectLight {
    GameObject *go;
    TransformComponent *tc;
    DirectionalLightComponent *dlc;
};