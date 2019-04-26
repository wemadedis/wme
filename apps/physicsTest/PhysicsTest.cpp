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
    using namespace StdComponents;

    Scene *scene = sceneManager.MakeScene();
    sceneManager.SetActiveScene(scene);

	// Testing
    uint64_t transIndex = scene->DefineComponent<TransformComponent>();
    GameObject *go = scene->CreateGameObject();
    TransformComponent *transComp0 = scene->AddComponent<TransformComponent>(transIndex, go);
	transComp0->Initialize(glm::vec3(10,10,10), glm::vec3(0), glm::vec3(1));
	TransformComponent *transComp1 = scene->AddComponent<TransformComponent>(transIndex, go);
	transComp1->Initialize(glm::vec3(5,5,5), glm::vec3(0), glm::vec3(2));
	int32_t memes = 5;
	
	/*

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
	*/
}