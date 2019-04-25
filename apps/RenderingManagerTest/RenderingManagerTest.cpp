#include <rte/RTE.hpp>

#include "PlayerController.hpp"
#include "rte/CameraComponent.hpp"
#include <iostream>

using namespace RTE;

void ConfigureGame(RTEConfig &config)
{
    config.WindowConfig.WindowHeight = 1000;
    config.WindowConfig.ApplicationName = "Banana";
    config.WindowConfig.WindowName = "Eggplant";
    config.GraphicsConfig.UseRaytracing = true;
    config.AssetConfig.Meshes = { Utilities::GetFileFromAssets("models/monkey.ply") };
}

void OnGameStart(Runtime::SceneManager &sceneManager)
{
    using namespace Runtime;
    using namespace StdComponents;

    Scene *scene = sceneManager.MakeScene();
    sceneManager.SetActiveScene(scene);

    // Init components pools
    uint64_t transIndex = scene->DefineComponent<TransformComponent>();
    uint64_t meshIndex = scene->DefineComponent<MeshComponent>();
    uint64_t pcIndex = scene->DefineComponent<PlayerController>();
    // Setup our game object
    GameObject *go = scene->CreateGameObject();
    TransformComponent *transComp = scene->AddComponent<TransformComponent>(transIndex, go);
    MeshComponent *meshComp = scene->AddComponent<MeshComponent>(meshIndex, go);
    PlayerController *pc = scene->AddComponent<PlayerController>(pcIndex, go);

    auto monkey = RTE::Utilities::GetFileFromAssets("models/monkey.ply");
    meshComp->Initialize(transComp, monkey);
    pc->Initialize(transComp, nullptr);

}