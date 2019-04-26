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
    uint64_t camIndex = scene->DefineComponent<CameraComponent>();
    // Setup our game object
    GameObject *go = scene->CreateGameObject();
    TransformComponent *transComp = scene->AddComponent<TransformComponent>(transIndex, go);
    MeshComponent *meshComp = scene->AddComponent<MeshComponent>(meshIndex, go);
    PlayerController *pc = scene->AddComponent<PlayerController>(pcIndex, go);

    Camera cam;
    glm::vec3 pos = {0.0f, 0.0f, 10.0f};
    cam.Position = pos;
    cam.ViewMatrix = glm::lookAt(pos, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    cam.ProjectionMatrix = glm::perspective(glm::radians(45.0f), (float)800 / (float)600, 0.1f, 100.0f);
    cam.ProjectionMatrix[1][1] *= -1;

    GameObject *go2 = scene->CreateGameObject();
    //TransformComponent *trans2 = scene->AddComponent<TransformComponent>(transIndex, go2);
    CameraComponent *camera = scene->AddComponent<CameraComponent>(camIndex, go2);
    TransformComponent *trans2 = new TransformComponent();
    trans2->Transform.Pos = glm::vec3(0.0f, 0.0f, 10.0f);
    //trans2->Transform.Pos = glm::vec3(0.0f, 0.0f, 10.0f);

    camera->Initialize(trans2);

    auto monkey = RTE::Utilities::GetFileFromAssets("models/monkey.ply");
    meshComp->Initialize(transComp, monkey);
    pc->Initialize(transComp, nullptr, camera);

}