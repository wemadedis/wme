#include <rte/RTE.hpp>

#include "PlayerController.hpp"
#include "rte/CameraComponent.hpp"
#include "rte/StdComponentsMain.hpp"

#include <iostream>

using namespace RTE;

void ConfigureGame(RTEConfig &config)
{
    config.WindowConfig.WindowHeight = 500;
    config.WindowConfig.WindowWidth = 500;
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
    uint64_t transIndex = scene->DefineComponent<TransformComponent>();
    uint64_t meshIndex = scene->DefineComponent<MeshComponent>();
    uint64_t pcIndex = scene->DefineComponent<PlayerController>();
    uint64_t camIndex = scene->DefineComponent<CameraComponent>();
    uint64_t plIndex = scene->DefineComponent<PointLightComponent>();
    uint64_t dlIndex = scene->DefineComponent<DirectionalLightComponent>();
    // Setup our game object
    GameObject *go = scene->CreateGameObject();
    TransformComponent *transComp = scene->AddComponent<TransformComponent>(transIndex, go);
    MeshComponent *meshComp = scene->AddComponent<MeshComponent>(meshIndex, go);
    PlayerController *pc = scene->AddComponent<PlayerController>(pcIndex, go);
    transComp->SetEnabled(true);
    meshComp->SetEnabled(true);
    pc->SetEnabled(true);
    GameObject *go2 = scene->CreateGameObject();


    CameraComponent *camera = scene->AddComponent<CameraComponent>(camIndex, go2);
    camera->SetEnabled(true);
    TransformComponent *trans2 = scene->AddComponent<TransformComponent>(transIndex, go2);
    trans2->Transform.Pos = glm::vec3(0.0f, 0.0f, 10.0f);
    camera->Initialize(trans2);

    GameObject *go3 = scene->CreateGameObject();
    auto trans3 = scene->AddComponent<TransformComponent>(transIndex, go3);
    auto pointLight = scene->AddComponent<PointLightComponent>(plIndex, go3);
    trans3->Transform.Pos = glm::vec3(0.0f, 2.0f, 0.0f);
    pointLight->Initialize(trans3, Colors::Yellow, 5.0f);

    GameObject *go4 = scene->CreateGameObject();
    auto trans4 = scene->AddComponent<TransformComponent>(transIndex, go4);
    auto dirLight = scene->AddComponent<DirectionalLightComponent>(dlIndex, go4);
    trans4->Transform.Pos = glm::vec3(0.0f, 0.0f, 0.0f);
    dirLight->Initialize(trans4, Colors::White);


    auto monkey = RTE::Utilities::GetFileFromAssets("models/monkey.ply");
    meshComp->Initialize(transComp, monkey);
    pc->Initialize(transComp, nullptr, camera);

}