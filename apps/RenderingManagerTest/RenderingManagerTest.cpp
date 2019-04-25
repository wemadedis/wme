#include <rte/RTE.hpp>

#include "PlayerController.hpp"

#include <iostream>

using namespace RTE;

void ConfigureGame(RTEConfig &config)
{
    config.WindowConfig.WindowHeight = 1000;
    config.WindowConfig.ApplicationName = "Banana";
    config.WindowConfig.WindowName = "Eggplant";
    config.GraphicsConfig.UseRaytracing = false;
    config.AssetConfig.Meshes = { Utilities::GetFileFromAssets("models/monkey.ply") };
}

void OnGameStart(Runtime::SceneManager &sceneManager)
{
    using namespace Runtime;
    using namespace StdComponents;

    Scene *scene = sceneManager.MakeScene();
    sceneManager.SetActiveScene(scene);

    // Init components pools
    uint64_t pcIndex = scene->DefineComponent<PlayerController>();
    uint64_t transIndex = scene->DefineComponent<TransformComponent>();
    uint64_t physIndex = scene->DefineComponent<PhysicsComponent>();
    uint64_t meshIndex = scene->DefineComponent<MeshComponent>();

    // Setup our game object
    GameObject *go = scene->CreateGameObject();
    PlayerController *pcComp = scene->AddComponent<PlayerController>(pcIndex, go);
    TransformComponent *transComp = scene->AddComponent<TransformComponent>(transIndex, go);
    PhysicsComponent *physComp = scene->AddComponent<PhysicsComponent>(physIndex, go);
    MeshComponent *meshComp = scene->AddComponent<MeshComponent>(meshIndex, go);


    auto monkey = RTE::Utilities::GetFileFromAssets("models/monkey.ply");
    //meshComp->Initialize(monkey);
    GameObject *go2 = scene->CreateGameObject();
    MeshComponent *meshComp2 = scene->AddComponent<MeshComponent>(meshIndex, go2);
    //meshComp2->Initialize(monkey);
    // Collider
    RTE::Physics::Collider boxCollider;
    boxCollider.Type = RTE::Physics::ColliderType::BOX;
    boxCollider.Data.Box.HalfExtents = glm::vec3(1);
    physComp->Initialize(transComp, 100, {boxCollider});

    pcComp->Initialize(transComp, physComp);
}