#include <rte/RTE.hpp>
#include "PlayerController.hpp"
#include "ObjectStructs.hpp"

#include <iostream>


using namespace RTE;

void ConfigureGame(RTEConfig &config)
{
    config.WindowConfig.WindowHeight = 720;
    config.WindowConfig.WindowWidth = 1280;
    config.WindowConfig.WindowName = "rEfLeCtIoNs ArE tHinGs";
    config.GraphicsConfig.UseRaytracing = true;
    config.AssetConfig.Meshes = {
        "E:\\projects\\rte\\engine\\assets\\models\\monkey.ply",
        "E:\\projects\\rte\\engine\\assets\\models\\box.ply",
        "E:\\projects\\rte\\engine\\assets\\models\\floor.ply"
    };
}

ComponentIds initComponentPools(RTE::Runtime::Scene *scene)
{
    using namespace Runtime;
    using namespace StdComponents;
    ComponentIds componentIds;

    componentIds.transformIndex = scene->DefineComponent<TransformComponent, 4>();
    componentIds.meshIndex = scene->DefineComponent<MeshComponent, 2>();
    componentIds.playerControllerIndex = scene->DefineComponent<PlayerController, 1>();
    componentIds.cameraIndex = scene->DefineComponent<CameraComponent, 1>();
    componentIds.pointLightIndex = scene->DefineComponent<PointLightComponent, 1>();

    return componentIds;
}

Player createPlayer(Runtime::Scene *scene, ComponentIds compIds)
{
    using namespace Runtime;
    using namespace StdComponents;
    Player player;

    player.go = scene->CreateGameObject();

    player.tc = scene->AddComponent<TransformComponent>(compIds.transformIndex, player.go);
    player.cc = scene->AddComponent<CameraComponent>(compIds.cameraIndex, player.go);
    player.pc = scene->AddComponent<PlayerController>(compIds.playerControllerIndex, player.go);

    player.tc->Initialize(glm::vec3(0.f, 1.f, -5.f), glm::vec3(0.f, 180.f, 0.f), glm::vec3(1));
    player.cc->Initialize(player.tc);
    player.pc->Initialize(player.tc, player.cc);

    return player;
}

Monkey createMonkey(Runtime::Scene *scene, ComponentIds compIds, SimpleTransform st)
{
    using namespace Runtime;
    using namespace StdComponents;
    Monkey monkey;

    monkey.go = scene->CreateGameObject();
    monkey.tc = scene->AddComponent<TransformComponent>(compIds.transformIndex, monkey.go);
    monkey.mc = scene->AddComponent<MeshComponent>(compIds.meshIndex, monkey.go);

    std::string monkeyPath = "E:\\projects\\rte\\engine\\assets\\models\\monkey.ply";
    monkey.tc->Initialize(st.pos, st.rot, st.scale);
    monkey.mc->Initialize(monkey.tc, monkeyPath, "");

    return monkey;
}

PLight createPointLight(Runtime::Scene *scene, ComponentIds compIds, SimpleTransform st, float radius)
{
    using namespace Runtime;
    using namespace StdComponents;

    PLight plight;

    plight.go = scene->CreateGameObject();
    plight.tc = scene->AddComponent<TransformComponent>(compIds.transformIndex, plight.go);
    plight.plc = scene->AddComponent<PointLightComponent>(compIds.pointLightIndex, plight.go);

    plight.tc->Initialize(st.pos, st.rot, st.scale);
    plight.plc->Initialize(plight.tc, Colors::LightGray, radius);

    return plight;
}


Floor createFloor(Runtime::Scene *scene, ComponentIds compIds)
{
    using namespace Runtime;
    using namespace StdComponents;

    Floor floor;
    floor.go = scene->CreateGameObject();
    floor.tc = scene->AddComponent<TransformComponent>(compIds.transformIndex, floor.go);
    floor.mc = scene->AddComponent<MeshComponent>(compIds.meshIndex, floor.go);

    std::string floorPath = "E:\\projects\\rte\\engine\\assets\\models\\floor.ply";
    floor.tc->Initialize(glm::vec3(0), glm::vec3(0,-0.1f, 0), glm::vec3(1));
    floor.mc->Initialize(floor.tc, floorPath, "");
    // floor.mc->Material.Specular = 0;

    return floor;
}

void OnGameStart(Runtime::SceneManager &sceneManager)
{
    using namespace Runtime;
    using namespace StdComponents;

    Scene *scene = sceneManager.MakeScene();
    sceneManager.SetActiveScene(scene);
    ComponentIds componentIds = initComponentPools(scene);

    // create player + camera
    Player player = createPlayer(scene, componentIds);

    // create monkey
    SimpleTransform monkeyTransform;
    monkeyTransform.pos = glm::vec3(0.f,1.2f,2.f);
    monkeyTransform.rot = glm::vec3(-90.f,180.f,0.f);
    monkeyTransform.scale = glm::vec3(1);
    Monkey monkey = createMonkey(scene, componentIds, monkeyTransform);

    // create light
    SimpleTransform lightTransform;
    lightTransform.pos = glm::vec3(0,5,0);
    lightTransform.rot = glm::vec3();
    lightTransform.scale = glm::vec3();
    PLight pLight = createPointLight(scene, componentIds, lightTransform, 35.f);

    Floor floor = createFloor(scene, componentIds);
    

}

