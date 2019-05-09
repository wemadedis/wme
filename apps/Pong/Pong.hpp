#pragma once
#include <rte/RTE.hpp>

#include "BallController.hpp"
#include "GameController.hpp"
#include "PaddleController.hpp"

#include <string>

struct Models
{
    std::string BoxPath = "models/box1x1x1.ply";
    std::string SpherePath = "models/sphere.ply";
};

struct Components
{
    ComponentPoolId TransformId;
    ComponentPoolId PhysicsId;
    ComponentPoolId DirectionalLightId;
    ComponentPoolId PointLightId;
    ComponentPoolId CameraId;
    ComponentPoolId MeshId;
    ComponentPoolId PaddleControllerId;
    ComponentPoolId BallControllerId;
    ComponentPoolId GameControllerId;
};

struct CameraGO
{
    RTE::Runtime::GameObject *GO;
    RTE::StdComponents::TransformComponent *Trans;
    RTE::StdComponents::CameraComponent *Cam;
};

struct DirLightGO
{
    RTE::Runtime::GameObject *GO;
    RTE::StdComponents::TransformComponent *Trans;
    RTE::StdComponents::DirectionalLightComponent *DirLight;
};

struct BoxGO
{
    RTE::Runtime::GameObject *GO;
    RTE::StdComponents::TransformComponent *Trans;
    RTE::StdComponents::MeshComponent *Mesh;
};

struct CollidableGO
{
    RTE::Runtime::GameObject *GO;
    RTE::StdComponents::TransformComponent *Trans;
    RTE::StdComponents::MeshComponent *Mesh;
    RTE::StdComponents::PhysicsComponent *Phys;
};

struct BallGO
{
    RTE::Runtime::GameObject *GO;
    RTE::StdComponents::TransformComponent *Trans;
    RTE::StdComponents::MeshComponent *Mesh;
    RTE::StdComponents::PhysicsComponent *Phys;
    BallController *Controller;
    GameController *GameController;
};

struct PaddleGO
{
    RTE::Runtime::GameObject *GO;
    RTE::StdComponents::TransformComponent *Trans;
    RTE::StdComponents::MeshComponent *Mesh;
    RTE::StdComponents::PhysicsComponent *Phys;
    PaddleController *Controller;
};

struct TriggerGO
{
    RTE::Runtime::GameObject *GO;
    RTE::StdComponents::TransformComponent *Trans;
    RTE::StdComponents::PhysicsComponent *Phys;
};