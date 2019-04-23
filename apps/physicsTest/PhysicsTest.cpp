#include <rte/RTE.hpp>

#include <iostream>

using namespace RTE;

void ConfigureGame(RTEConfig &config)
{
    std::cout << "Configuring" << std::endl;
    config.WindowConfig.ApplicationName = "Banana";
}

void OnGameStart(Runtime::SceneManager &sceneManager)
{
    using namespace Runtime;
    using namespace StandardComponents;

    Scene *scene = sceneManager.MakeScene();
    sceneManager.SetActiveScene(scene);
    GameObject *go = scene->CreateGameObject();

    uint64_t transIndex = scene->DefineComponent<TransformComponent>();
    uint64_t physIndex = scene->DefineComponent<PhysicsComponent>();

    TransformComponent *transComp = scene->AddComponent<TransformComponent>(transIndex, go);
    PhysicsComponent *physComp = scene->AddComponent<PhysicsComponent>(physIndex, go);
    physComp->Initialize(transComp);
    
}