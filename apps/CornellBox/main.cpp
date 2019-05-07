#include <rte/RTE.hpp>
#include "PlayerController.hpp"
#include "ObjectStructs.hpp"
#include "rte/Utility.hpp"

#include <iostream>
using namespace RTE;
using namespace RTE::Utilities;


const RelativeFilePath RedPath = "textures/red.png";
const RelativeFilePath GreenPath = "textures/green.png";

const RelativeFilePath BoxPath = "models/box1x1x1.ply";
const RelativeFilePath MonkeyPath = "models/monkey1x1x1.ply";
const RelativeFilePath FloorPath = "models/floor.ply";

const RelativeFilePath MonoSoundPath = "audio/mono.wav";

AbsoluteFilePath AbsRedPath;
AbsoluteFilePath AbsGreenPath;

AbsoluteFilePath AbsBoxPath;
AbsoluteFilePath AbsMonkeyPath;
AbsoluteFilePath AbsFloorPath;

AbsoluteFilePath AbsMonoSoundPath;

void ConfigureGame(RTEConfig &config)
{
    AbsRedPath = GetFileFromAssets(RedPath);
    AbsGreenPath = GetFileFromAssets(GreenPath);
    AbsBoxPath = GetFileFromAssets(BoxPath);
    AbsMonkeyPath = GetFileFromAssets(MonkeyPath);
    AbsFloorPath = GetFileFromAssets(FloorPath);
    AbsMonoSoundPath = GetFileFromAssets(MonoSoundPath);

    config.WindowConfig.WindowHeight = 720;
    config.WindowConfig.WindowWidth = 1280;
    config.WindowConfig.WindowName = "rEfLeCtIoNs ArE tHinGs";
    config.GraphicsConfig.UseRaytracing = true;
    config.GraphicsConfig.FramesPerSecond = 120;
    config.AssetConfig.Meshes = { AbsMonkeyPath, AbsBoxPath, AbsFloorPath };
    config.AssetConfig.Textures = { AbsGreenPath, AbsRedPath };
}

ComponentIds initComponentPools(RTE::Runtime::Scene *scene)
{
    using namespace Runtime;
    using namespace StdComponents;
    ComponentIds componentIds;

    componentIds.transformIndex = scene->DefineComponent<TransformComponent, 10>();
    componentIds.meshIndex = scene->DefineComponent<MeshComponent, 10>();
    componentIds.playerControllerIndex = scene->DefineComponent<PlayerController, 1>();
    componentIds.cameraIndex = scene->DefineComponent<CameraComponent, 1>();
    componentIds.pointLightIndex = scene->DefineComponent<PointLightComponent, 1>();
    componentIds.audioIndex = scene->DefineComponent<AudioComponent, 1>();
    componentIds.listenerIndex = scene->DefineComponent<ListenerComponent, 1>();

    return componentIds;
}

Player createPlayer(Runtime::Scene *scene, ComponentIds compIds, SimpleTransform st)
{
    using namespace Runtime;
    using namespace StdComponents;
    Player player;

    player.go = scene->CreateGameObject();

    player.tc = scene->AddComponent<TransformComponent>(compIds.transformIndex, player.go);
    player.cc = scene->AddComponent<CameraComponent>(compIds.cameraIndex, player.go);
    player.pc = scene->AddComponent<PlayerController>(compIds.playerControllerIndex, player.go);
    player.lc = scene->AddComponent<ListenerComponent>(compIds.listenerIndex, player.go);

    player.tc->Initialize(st.pos, st.rot, st.scale);
    player.cc->Initialize(player.tc);
    player.pc->Initialize(player.tc, player.cc);
    player.lc->Initialize(player.tc);

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
    monkey.ac = scene->AddComponent<AudioComponent>(compIds.audioIndex, monkey.go);

    monkey.tc->Initialize(st.pos, st.rot, st.scale);
    monkey.mc->Initialize(monkey.tc, AbsMonkeyPath, "");
    monkey.ac->Initialize(AbsMonoSoundPath, monkey.tc);

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


Box createBox(Runtime::Scene *scene, ComponentIds compIds, SimpleTransform st)
{
    using namespace Runtime;
    using namespace StdComponents;

    Box box;
    box.go = scene->CreateGameObject();
    box.tc = scene->AddComponent<TransformComponent>(compIds.transformIndex, box.go);
    box.mc = scene->AddComponent<MeshComponent>(compIds.meshIndex, box.go);

    box.tc->Initialize(st.pos, st.rot, st.scale);
    box.mc->Initialize(box.tc, AbsBoxPath, "");

    return box;
}

Wall createWall(Runtime::Scene *scene, ComponentIds compIds, SimpleTransform ts)
{
    using namespace Runtime;
    using namespace StdComponents;

    Wall wall;
    wall.go = scene->CreateGameObject();
    wall.tc = scene->AddComponent<TransformComponent>(compIds.transformIndex, wall.go);
    wall.mc = scene->AddComponent<MeshComponent>(compIds.meshIndex, wall.go);

    wall.tc->Initialize(ts.pos, ts.rot, ts.scale);
    wall.mc->Initialize(wall.tc, AbsFloorPath, "");

    return wall;
}

void CornellBox(Runtime::Scene *scene, ComponentIds componentIds)
{
    using namespace Runtime;
    using namespace StdComponents;

    SimpleTransform playerSt = {glm::vec3(0.f, 1.f, 6.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(1)};
    Player player = createPlayer(scene, componentIds, playerSt);
    SimpleTransform st = {
        glm::vec3(0.f, 4.3f, 0.f), //* Position
        glm::vec3(0.f, 0.f, 0.f), //* Rotation
        glm::vec3(1.f, 1.f, 1.f)  //* Scale
    };
    PLight pointLight = createPointLight(scene, componentIds, st, 15.f);

    SimpleTransform wallSt = {
        glm::vec3(0.f, 0.f, 0.f), //* Position
        glm::vec3(0.f, 0.f, 0.f), //* Rotation
        glm::vec3(6.f, 0.1f, 8.f)  //* Scale
    };
    Box floor = createBox(scene, componentIds, wallSt);

    wallSt.pos = glm::vec3(0.f, 5.f, 0.f);
    Box ceiling = createBox(scene, componentIds, wallSt);

    wallSt.pos = glm::vec3(3.0f, 2.5f, 0.f);
    wallSt.rot = glm::vec3(0.f, 0.f, 0.f);
    wallSt.scale = glm::vec3(0.1f, 5.f, 8.0f);
    Box wallLeft = createBox(scene, componentIds, wallSt);
    wallLeft.mc->SetTexture(AbsRedPath);
    wallLeft.mc->Material.Reflectivity = 0.2f;

    wallSt.pos = glm::vec3(-3.0f, 2.5f, 0.f);
    Box wallRight = createBox(scene, componentIds, wallSt);
    wallRight.mc->SetTexture(AbsGreenPath);
    wallRight.mc->Material.Reflectivity = 0.2f;

    wallSt.pos = glm::vec3(0.f, 2.5f, -4.f);
    wallSt.scale = glm::vec3(6.f, 5.f, 0.1f);
    Box wallFront = createBox(scene, componentIds, wallSt);

    SimpleTransform boxSt;
    boxSt.pos = glm::vec3(-0.75f, 1.5f, -1.5f);
    boxSt.rot = glm::vec3(0.f, 115.f, 0.f);
    boxSt.scale = glm::vec3(2.f, 3.f, 2.0f);
    Box box1 = createBox(scene, componentIds, boxSt);
    box1.mc->Material.Reflectivity = 0.15f;

    boxSt.pos = glm::vec3(1.1f, 0.65f, 1.5f);
    boxSt.rot = glm::vec3(0.f, 75.f, 0.f);
    boxSt.scale = glm::vec3(1.3f, 1.3f, 1.3f);
    Box box2 = createBox(scene, componentIds, boxSt);
    box2.mc->Material.Reflectivity = 0.15f;
}

void MonkeyTest(Runtime::Scene *scene, ComponentIds componentIds)
{
    // create player + camera
    SimpleTransform playerSt = {glm::vec3(0.f, 1.f, -4.f), glm::vec3(0.f, 180.f, 0.f), glm::vec3(1)};
    Player player = createPlayer(scene, componentIds, playerSt);

    // create monkey
    SimpleTransform monkeyTransform;
    monkeyTransform.pos = glm::vec3(0.f,1.f,3.f);
    // monkeyTransform.rot = glm::vec3(-90.f,180.f,0.f);
    monkeyTransform.rot = glm::vec3(0);
    monkeyTransform.scale = glm::vec3(1);
    Monkey monkey = createMonkey(scene, componentIds, monkeyTransform);
    monkey.ac->SetLoop(true);
    monkey.ac->Play();

    // create light
    SimpleTransform lightTransform;
    lightTransform.pos = glm::vec3(0,5,0);
    lightTransform.rot = glm::vec3();
    lightTransform.scale = glm::vec3();
    PLight pLight = createPointLight(scene, componentIds, lightTransform, 35.f);

    Wall wallTop = createWall(scene, componentIds,{glm::vec3(0), glm::vec3(0), glm::vec3(1)});
    Wall wallBottom = createWall(scene, componentIds,{glm::vec3(0.f, 10.f, 0.f), glm::vec3(0), glm::vec3(1)});

    Wall wallBack = createWall(scene, componentIds, {glm::vec3(0.f, 5.f, 5.f), glm::vec3(90, 0, 0), glm::vec3(1)});
    Wall wallLeft = createWall(scene, componentIds, {glm::vec3(5.f, 5.f, 0.f), glm::vec3(0, 0, 90), glm::vec3(1)});
    Wall wallRight = createWall(scene, componentIds, {glm::vec3(-5.f, 5.f, 0.f), glm::vec3(0, 0, 90), glm::vec3(1)});
    Wall wallFront = createWall(scene, componentIds, {glm::vec3(0.f, 5.f, -5.f), glm::vec3(90, 0, 0), glm::vec3(1)});
    
    wallLeft.mc->Material.Reflectivity=0.75f;
    wallRight.mc->Material.Reflectivity=0.75f;
}


void OnGameStart(Runtime::SceneManager &sceneManager)
{
    using namespace Runtime;
    using namespace StdComponents;

    Scene *scene = sceneManager.MakeScene();
    sceneManager.SetActiveScene(scene);
    ComponentIds componentIds = initComponentPools(scene);

    // CornellBox(scene, componentIds);
    MonkeyTest(scene, componentIds);
    

}

