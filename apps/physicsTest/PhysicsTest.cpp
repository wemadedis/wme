#include <rte/RTE.hpp>

#include "PlayerController.hpp"
#include "rte/TestGenericPool.hpp"

#include <iostream>

using namespace RTE;
using namespace RTE::Runtime;
using namespace RTE::StdComponents;

const RelativeFilePath BoxPath = "models/box.ply";
const RelativeFilePath PlanePath = "models/plane.ply";
const RelativeFilePath BackgroundTexture = "textures/rte.png";
const RelativeFilePath BoxTexture = "texture/";

void ConfigureGame(RTEConfig &config)
{
    config.WindowConfig.WindowHeight = 400;
    config.WindowConfig.WindowWidth = 1000;
    config.WindowConfig.WindowName = "Eggplant";
}

RTE::Physics::Collider MakeBoxCollider()
{
    RTE::Physics::Collider boxCollider;
    boxCollider.Type = RTE::Physics::ColliderType::BOX;
    boxCollider.Data.Box.HalfExtents = glm::vec3(1);
    return boxCollider;
}

struct ComponentIds
{
    ComponentPoolId TransformPoolId;
    ComponentPoolId PhysicsPoolId;
    ComponentPoolId MeshPoolId;
    ComponentPoolId PCPoolId;
};

struct StaticBox
{
    GameObject *Go;
    GameObjectId GoId;
    TransformComponent *Trans;
    PhysicsComponent *Phys;
    MeshComponent *Mesh;
};

struct ControlledBox
{
    GameObject *Go;
    GameObjectId GoId;
    TransformComponent *Trans;
    PhysicsComponent *Phys;
    PlayerController *Controller;
    MeshComponent *Mesh;
};

StaticBox MakeStaticBox(Scene *scene, ComponentIds &comps)
{
    GameObject *go = scene->CreateGameObject();
    StaticBox box;
    box.Trans = scene->AddComponent<TransformComponent>(comps.TransformPoolId, go);
    box.Phys = scene->AddComponent<PhysicsComponent>(comps.PhysicsPoolId, go);
    box.Mesh = scene->AddComponent<MeshComponent>(comps.MeshPoolId, go);

    box.Trans->Initialize();
    box.Phys->Initialize(box.Trans, 1, {MakeBoxCollider()});
    return box;
}

ControlledBox MakeControlledBox(Scene *scene, ComponentIds &comps)
{
    GameObject *go = scene->CreateGameObject();
    ControlledBox box;
    box.Trans = scene->AddComponent<TransformComponent>(comps.TransformPoolId, go);
    box.Phys = scene->AddComponent<PhysicsComponent>(comps.PhysicsPoolId, go);
    box.Mesh = scene->AddComponent<MeshComponent>(comps.MeshPoolId, go);
    box.Controller = scene->AddComponent<PlayerController>(comps.PCPoolId, go);

    box.Trans->Initialize();
    box.Phys->Initialize(box.Trans, 1, {MakeBoxCollider()});
    box.Mesh->Initialize(box.Trans, "", "");
    return box;
}

void MakeGround(Runtime::SceneManager &sceneManager)
{
}

void MakeBackground(Runtime::SceneManager &sceneManager)
{
}

void OnGameStart(Runtime::SceneManager &sceneManager)
{
    Scene *scene = sceneManager.GetActiveScene();

    // Init components pools
    ComponentIds componentIds;
    componentIds.PCPoolId = scene->DefineComponent<PlayerController, 1>();
    componentIds.TransformPoolId = scene->DefineComponent<TransformComponent, 4>();
    componentIds.PhysicsPoolId = scene->DefineComponent<PhysicsComponent, 2>();
    componentIds.MeshPoolId = scene->DefineComponent<MeshComponent, 4>();

    // Static box
    StaticBox staticBox = MakeStaticBox(scene, componentIds);

    // Controlled box
    ControlledBox controlledBox = MakeControlledBox(scene, componentIds);

    // Background
    GameObject *bg = scene->CreateGameObject();
    TransformComponent *bgTransform = scene->AddComponent<TransformComponent>(componentIds.TransformPoolId, bg);

    // Ground
    GameObject *ground = scene->CreateGameObject();
    TransformComponent *groundTransform = scene->AddComponent<TransformComponent>(componentIds.TransformPoolId, ground);
}