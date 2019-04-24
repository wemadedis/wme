#include <rte/RTE.hpp>

#include "PlayerController.hpp"

#include <iostream>

using namespace RTE;

void ConfigureGame(RTEConfig &config)
{
    config.WindowConfig.WindowHeight = 100;
    config.WindowConfig.ApplicationName = "Banana";
    config.WindowConfig.WindowName = "Eggplant";
    config.GraphicsConfig.UseRaytracing = false;
}

void OnGameStart(Runtime::SceneManager &sceneManager)
{
    using namespace Runtime;
    using namespace StandardComponents;

    Scene *scene = sceneManager.MakeScene();
    sceneManager.SetActiveScene(scene);

    // Init components pools
    uint64_t pcIndex = scene->DefineComponent<PlayerController>();
    uint64_t transIndex = scene->DefineComponent<TransformComponent>();
    uint64_t physIndex = scene->DefineComponent<PhysicsComponent>();

    // Setup our game object
    GameObject *go = scene->CreateGameObject();
    PlayerController *pcComp = scene->AddComponent<PlayerController>(pcIndex, go);
    TransformComponent *transComp = scene->AddComponent<TransformComponent>(transIndex, go);
    PhysicsComponent *physComp = scene->AddComponent<PhysicsComponent>(physIndex, go);

    // Collider
    RTE::Physics::Collider boxCollider;
    boxCollider.Type = RTE::Physics::ColliderType::BOX;
    boxCollider.Data.Box.HalfExtents = glm::vec3(1);
    physComp->Initialize(transComp, 100, {boxCollider});

    pcComp->Initialize(transComp, physComp);
}