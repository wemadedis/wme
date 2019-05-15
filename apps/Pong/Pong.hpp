#pragma once
#include <rte/RTE.hpp>

#include "BallController.hpp"
#include "GameController.hpp"
#include "PaddleController.hpp"

#include <string>

struct Models
{
    std::string BoxPath = "models/box.ply";
    std::string SpherePath = "models/sphere.ply";
};

struct Sounds
{
    std::string PongHitPath = "audio/pong_hit.wav";
};

struct Textures
{
    std::string BrickPath = "textures/bricks.png";
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
    ComponentPoolId AudioCompId;
    ComponentPoolId ListenerId;
};

struct CameraGO
{
    RTE::Runtime::GameObject *GO;
    RTE::StdComponents::TransformComponent *Trans;
    RTE::StdComponents::CameraComponent *Cam;
    RTE::StdComponents::ListenerComponent *Listener;
};

struct PointLightGO
{
    RTE::Runtime::GameObject *GO;
    RTE::StdComponents::TransformComponent *Trans;
    RTE::StdComponents::PointLightComponent *DirLight;
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
    RTE::StdComponents::PointLightComponent *Light;
    RTE::StdComponents::AudioComponent *Audio;
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