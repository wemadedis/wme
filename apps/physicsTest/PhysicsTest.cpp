#include <rte/RTE.hpp>

#include "PlayerController.hpp"

#include <iostream>

using namespace RTE;

void ConfigureGame(RTEConfig &config)
{
    config.WindowConfig.WindowHeight = 100;
    config.WindowConfig.WindowName = "Eggplant";
    config.GraphicsConfig.UseRaytracing = false;
}

void OnGameStart(Runtime::SceneManager &sceneManager)
{
    using namespace Runtime;
    using namespace StdComponents;

    Scene *scene = sceneManager.MakeScene();
    sceneManager.SetActiveScene(scene);

    // Init components pools
    ComponentId pcIndex = scene->DefineComponent<PlayerController, 1>();
    ComponentId transIndex = scene->DefineComponent<TransformComponent, 2>();
    ComponentId physIndex = scene->DefineComponent<PhysicsComponent, 2>();

    GameObject *go = scene->CreateGameObject();

    auto *transComp0 = scene->AddComponent<TransformComponent>(transIndex, go);
    auto *transComp1 = scene->AddComponent<TransformComponent>(transIndex, go);

    transComp0->Initialize(glm::vec3(10, 10, 10), glm::vec3(0), glm::vec3(1));
    transComp1->Initialize(glm::vec3(5, 5, 5), glm::vec3(0), glm::vec3(2));

    PlayerController *pcComp = scene->AddComponent<PlayerController>(pcIndex, go);
    PhysicsComponent *physComp = scene->AddComponent<PhysicsComponent>(physIndex, go);

    // Collider
    RTE::Physics::Collider boxCollider;
    boxCollider.Type = RTE::Physics::ColliderType::BOX;
    boxCollider.Data.Box.HalfExtents = glm::vec3(1);
    physComp->Initialize(transComp0, 100, {boxCollider});
    pcComp->Initialize(transComp0, physComp);
}