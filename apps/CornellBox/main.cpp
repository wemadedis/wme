#include <rte/RTE.hpp>
#include "PlayerController.hpp"
#include "Rotator.hpp"
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
    config.WindowConfig.WindowName = "Test";
    config.GraphicsConfig.UseRaytracing = true;
    config.GraphicsConfig.FramesPerSecond = 1000;
    config.AssetConfig.Meshes = { AbsMonkeyPath, AbsBoxPath, AbsFloorPath };
    config.AssetConfig.Textures = { AbsGreenPath, AbsRedPath };
	config.PhysicsConfig.MaxCollisionCount = 100000;
}

ComponentIds initComponentPools(RTE::Runtime::Scene *scene)
{
    using namespace Runtime;
    using namespace StdComponents;
    ComponentIds componentIds;

    componentIds.transformIndex = scene->DefineComponent<TransformComponent, 50000>();
    componentIds.meshIndex = scene->DefineComponent<MeshComponent, 50000>();
    componentIds.audioIndex = scene->DefineComponent<AudioComponent, 50000>();
    componentIds.rotatorIndex = scene->DefineComponent<Rotator, 50000>();
	componentIds.physicsIndex = scene->DefineComponent<PhysicsComponent, 50000>();

    componentIds.playerControllerIndex = scene->DefineComponent<PlayerController, 1>();
    componentIds.cameraIndex = scene->DefineComponent<CameraComponent, 1>();
    componentIds.listenerIndex = scene->DefineComponent<ListenerComponent, 1>();
    
    componentIds.pointLightIndex = scene->DefineComponent<PointLightComponent, 1>();
    componentIds.directLightIndex = scene->DefineComponent<DirectionalLightComponent, 1>();

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

    monkey.tc->Initialize(st.pos, st.rot, st.scale);
    monkey.mc->Initialize(monkey.tc, AbsMonkeyPath, "");

    return monkey;
}

Monkey createRotatingMonkey(Runtime::Scene *scene, ComponentIds compIds, SimpleTransform st)
{
    using namespace Runtime;
    using namespace StdComponents;
    Monkey monkey;

    monkey.go = scene->CreateGameObject();
    monkey.tc = scene->AddComponent<TransformComponent>(compIds.transformIndex, monkey.go);
    monkey.mc = scene->AddComponent<MeshComponent>(compIds.meshIndex, monkey.go);
    monkey.rc = scene->AddComponent<Rotator>(compIds.rotatorIndex, monkey.go);

    monkey.tc->Initialize(st.pos, st.rot, st.scale);
    monkey.mc->Initialize(monkey.tc, AbsMonkeyPath, "");
    monkey.rc->Initialize(monkey.tc);

    return monkey;
}

Monkey createSoundMonkey(Runtime::Scene *scene, ComponentIds compIds, SimpleTransform st)
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

DirectLight createDirectLight(Runtime::Scene *scene, ComponentIds compIds, SimpleTransform st)
{
    using namespace Runtime;
    using namespace StdComponents;
    DirectLight directLight;

    directLight.go = scene->CreateGameObject();
    directLight.tc = scene->AddComponent<TransformComponent>(compIds.transformIndex, directLight.go);
    directLight.dlc = scene->AddComponent<DirectionalLightComponent>(compIds.directLightIndex, directLight.go);

    directLight.tc->Initialize(st.pos, st.rot, st.scale);
    directLight.dlc->Initialize(directLight.tc, Colors::White);

    return directLight;
}

void CornellBox(Runtime::Scene *scene, ComponentIds componentIds)
{
    using namespace Runtime;
    using namespace StdComponents;

    SimpleTransform playerSt = {glm::vec3(0.f, 2.5f, 9.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(1)};
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
    ceiling.mc->Material.Specular = 0.0f;

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

void MonkeySoundTest(Runtime::Scene* scene, ComponentIds componentIds)
{
	// create player + camera
	SimpleTransform playerSt;
	playerSt.pos = glm::vec3(0.f, 1.f, -4.f);
	playerSt.rot = glm::vec3(-45.f, 180.f, 0.f);
	playerSt.scale = glm::vec3(1);

    Player player = createPlayer(scene, componentIds, playerSt);

    // create monkey
    SimpleTransform monkeyTransform;
    monkeyTransform.pos = glm::vec3(0.f,1.f,3.f);
    // monkeyTransform.rot = glm::vec3(-90.f,180.f,0.f);
    monkeyTransform.rot = glm::vec3(0);
    monkeyTransform.scale = glm::vec3(1);
    Monkey monkey = createSoundMonkey(scene, componentIds, monkeyTransform);
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

void LotsOfMonkeys(Runtime::Scene *scene, ComponentIds componentIds)
{
	SimpleTransform playerSt;
    int size = 200;
	playerSt.pos = glm::vec3(0.5f*2.5f*(size-1), 30.f, -45.f);
	playerSt.rot = glm::vec3(-25.f, 180.f, 0.f);
	playerSt.scale = glm::vec3(1);
    Player player = createPlayer(scene, componentIds, playerSt);

    DirectLight directLight = createDirectLight(scene, componentIds, {glm::vec3(0), glm::vec3(-90.f, 0.f, 0.f), glm::vec3(1)});

    SimpleTransform floorSt = {glm::vec3(0), glm::vec3(0), glm::vec3(1000.f, 0.1f, 1000.f)};
    Box floor = createBox(scene, componentIds, floorSt);

    SimpleTransform monkeyTs;
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            monkeyTs = {
                glm::vec3(2.5f*i, 1.f, 2.5f*j), // * Position
                glm::vec3(0), // * Rotation
                glm::vec3(1) // * Scale
            };

            Monkey monkey = createRotatingMonkey(scene, componentIds, monkeyTs);
            // monkey.ac->Play();
            monkey.rc->setRotMagnitude(glm::vec3(20.f, 20.f, 20.f));
			monkey.mc->Material.Color = Colors::Blue;
            player.pc->addGeometryData(monkey.verts, monkey.faces, monkey.triangles);
        }
    }
}

void PhysicsBoxTest(Runtime::Scene* scene, ComponentIds componentIds)
{
	using namespace RTE::Physics;
	SimpleTransform playerSt;
	playerSt.pos = glm::vec3(-3.5f, 10.f, -10.f);
	playerSt.rot = glm::vec3(-40.f, 180.f, 0.f);
	playerSt.scale = glm::vec3(1);
	Player player = createPlayer(scene, componentIds, playerSt);

	DirectLight directLight = createDirectLight(scene, componentIds, { glm::vec3(0), glm::vec3(-90.f, 0.f, 0.f), glm::vec3(1) });

	SimpleTransform floorSt = { glm::vec3(0), glm::vec3(0,0,10.f), glm::vec3(1000.f, 0.1f, 1000.f) };
	Box floor = createBox(scene, componentIds, floorSt);
	floor.pc = scene->AddComponent<PhysicsComponent>(componentIds.physicsIndex, floor.go);
	Collider col;
	col.Data.Box.HalfExtents = { 500, 0.05f, 500 };
	col.Type = ColliderType::BOX;

	floor.pc->Initialize(floor.tc, 100.f, { col });
	floor.pc->GetRigidBody()->SetGravity({ 0, 0, 0 });
	floor.pc->GetRigidBody()->SetLinearFactor({ 0, 0, 0 });
	floor.pc->GetRigidBody()->SetAngularFactor({ 0, 0, 0 });

	int size = 5;
	for (int x = 0; x < size; x++)
	{
		for (int y = 0; y < size; y++)
		{
			for (int z = 0; z < size; z++)
			{
				Collider boxCol;
				boxCol.Data.Box.HalfExtents = { 0.4, 0.4, 0.4 };
				boxCol.Type = ColliderType::BOX;

				SimpleTransform boxTs;
				boxTs.pos = glm::vec3(x, y + 10.5f, z);
				boxTs.rot = glm::vec3(0);
				boxTs.scale = glm::vec3(0.8);

				auto box = createBox(scene, componentIds, boxTs);
				box.pc = scene->AddComponent<PhysicsComponent>(componentIds.physicsIndex, box.go);
				box.pc->Initialize(box.tc, 5.f, { boxCol });
				box.pc->GetRigidBody()->SetGravity({ 0, -1, 0 });

				// box.mc->Material.Color = glm::vec4(x * 25, y * 25, z * 25, 0);
				box.mc->Material.Color = Colors::Blue;

			}
		}
	}
}


void OnGameStart(Runtime::SceneManager &sceneManager)
{
    using namespace Runtime;
    using namespace StdComponents;

    Scene *scene = sceneManager.MakeScene();
    sceneManager.SetActiveScene(scene);
    ComponentIds componentIds = initComponentPools(scene);

    //CornellBox(scene, componentIds);
    //MonkeySoundTest(scene, componentIds);
    //LotsOfMonkeys(scene, componentIds);
	PhysicsBoxTest(scene, componentIds);
    

}

