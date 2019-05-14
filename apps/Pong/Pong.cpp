#include "Pong.hpp"

using namespace RTE;
using namespace RTE::StdComponents;
using namespace RTE::Runtime;
using namespace RTE::Physics;
using namespace RTE::Platform;
using namespace RTE::Utilities;

Models models;
Sounds sounds;
Textures textures;

void ConfigureGame(RTEConfig &config)
{
    config.GraphicsConfig.UseRaytracing = true;
    config.GraphicsConfig.FramesPerSecond = 120;
    config.PhysicsConfig.DebugDrawColliders = false;
    config.WindowConfig.WindowWidth = 1000;
    config.WindowConfig.WindowHeight = 800;
    config.WindowConfig.WindowName = "RAYTRACING: PO(N)G-CHAMP!";

    models.BoxPath = GetFileFromAssets(models.BoxPath);
    models.SpherePath = GetFileFromAssets(models.SpherePath);

    textures.BrickPath = GetFileFromAssets(textures.BrickPath);

    config.AssetConfig.Meshes = {
        models.BoxPath,
        models.SpherePath};
    config.AssetConfig.Textures = {
        textures.BrickPath};

    sounds.PongHitPath = GetFileFromAssets(sounds.PongHitPath);
}

Components GetComponentPoolIds(Scene &scene)
{
    Components c;
    // 2 For upper/lower walls, 2 for paddles, 1 for ball, 2 for  goal-colliders,
    // 1 for camera, 1 for directional light, 1 for background.
    c.TransformId = scene.DefineComponent<TransformComponent, 11>();
    c.MeshId = scene.DefineComponent<MeshComponent, 6>();
    c.PhysicsId = scene.DefineComponent<PhysicsComponent, 7>();
    c.CameraId = scene.DefineComponent<CameraComponent, 1>();
    c.PaddleControllerId = scene.DefineComponent<PaddleController, 2>();
    c.PointLightId = scene.DefineComponent<PointLightComponent, 1>();
    c.BallControllerId = scene.DefineComponent<BallController, 1>();
    c.GameControllerId = scene.DefineComponent<GameController, 1>();
    c.AudioCompId = scene.DefineComponent<AudioComponent, 1>();
    c.ListenerId = scene.DefineComponent<ListenerComponent, 1>();

    return c;
}

TriggerGO MakeTrigger(Scene &scene, Components &comps)
{
    TriggerGO go;
    go.GO = scene.CreateGameObject();

    go.Trans = scene.AddComponent<TransformComponent>(comps.TransformId, go.GO)
                   ->WithScale({1, 1, 1});

    go.Phys = scene.AddComponent<PhysicsComponent>(comps.PhysicsId, go.GO);
    Collider c;
    c.Data.Box.HalfExtents = {0.5, 4.5, 0.5};
    c.Type = ColliderType::BOX;
    go.Phys->Initialize(go.Trans, 1, {c});
    go.Phys->GetRigidBody()->SetStatic(true);
    return go;
}

TriggerGO MakeLeftTrigger(Scene &scene, Components &comps)
{
    TriggerGO go = MakeTrigger(scene, comps);
    go.Trans->WithPosition({-5.1, 0, 0});
    return go;
}

TriggerGO MakeRightTrigger(Scene &scene, Components &comps)
{
    TriggerGO go = MakeTrigger(scene, comps);
    go.Trans->WithPosition({5.1, 0, 0});
    return go;
}

CameraGO MakeCamera(Scene &scene, Components &comps)
{
    CameraGO go;
    go.GO = scene.CreateGameObject();
    go.Trans = scene.AddComponent<TransformComponent>(comps.TransformId, go.GO)
                   ->WithPosition({0, 0, 15});

    go.Cam = scene.AddComponent<CameraComponent>(comps.CameraId, go.GO);
    go.Cam->Initialize(go.Trans);
    go.Cam->BackgroundColor = Colors::Cyan;
    go.Listener = scene.AddComponent<ListenerComponent>(comps.ListenerId, go.GO);
    go.Listener->Initialize(go.Trans);
    return go;
}

BoxGO MakeBackground(Scene &scene, Components &comps)
{
    BoxGO go;
    go.GO = scene.CreateGameObject();
    go.Trans = scene.AddComponent<TransformComponent>(comps.TransformId, go.GO)
                   ->WithPosition({0, 0, -1})
                   ->WithScale({20, 20, 1});

    go.Mesh = scene.AddComponent<MeshComponent>(comps.MeshId, go.GO);
    go.Mesh->Initialize(go.Trans, models.BoxPath);
    go.Mesh->Material.Reflectivity = 0.5;
    go.Mesh->Material.Color = Colors::Red;
    return go;
}

BallGO MakeBall(Scene &scene, Components &comps, GameObjectId left, GameObjectId right)
{
    BallGO go;
    go.GO = scene.CreateGameObject();

    go.Trans = scene.AddComponent<TransformComponent>(comps.TransformId, go.GO)
                   ->WithScale({0.5, 0.5, 0.5});

    go.Mesh = scene.AddComponent<MeshComponent>(comps.MeshId, go.GO);
    go.Mesh->Initialize(go.Trans, models.SpherePath);
    go.Mesh->Material.Color = Colors::Blue;

    go.Phys = scene.AddComponent<PhysicsComponent>(comps.PhysicsId, go.GO);
    Collider col;
    col.Data.Sphere.Radius = 0.25f;
    col.Type = ColliderType::SPHERE;
    go.Phys->Initialize(go.Trans, 10, {col}, true);
    go.Phys->GetRigidBody()->SetGravity({0, 0, 0});
    go.Phys->GetRigidBody()->SetAngularFactor({0, 0, 0});
    go.Phys->GetRigidBody()->SetLinearFactor({1, 1, 0});
    go.Audio = scene.AddComponent<AudioComponent>(comps.AudioCompId, go.GO);
    go.Audio->Initialize(sounds.PongHitPath, go.Trans);
    go.Audio->SetLoop(false);

    go.Controller = scene.AddComponent<BallController>(comps.BallControllerId, go.GO);
    go.GameController = scene.AddComponent<GameController>(comps.GameControllerId, go.GO);

    go.Controller->Initialize(go.Phys, go.Trans, go.Audio, go.GameController, left, right);

    go.GameController->Initialize(go.Controller, go.Trans, 3);
    go.GameController->SetGUIDraw([=]() {
        go.GameController->ShowScore();
    });

    return go;
}

CollidableGO MakeWall(Scene &scene, Components &comps)
{
    CollidableGO go;
    go.GO = scene.CreateGameObject();
    go.Trans = scene.AddComponent<TransformComponent>(comps.TransformId, go.GO)
                   ->WithScale({9, 1, 1});

    go.Mesh = scene.AddComponent<MeshComponent>(comps.MeshId, go.GO);
    go.Mesh->Initialize(go.Trans, models.BoxPath, textures.BrickPath);

    go.Phys = scene.AddComponent<PhysicsComponent>(comps.PhysicsId, go.GO);
    Collider col;
    col.Data.Box.HalfExtents = {4.5, 0.5, 0.5};
    col.Type = ColliderType::BOX;
    go.Phys->Initialize(go.Trans, 0, {col});
    go.Phys->GetRigidBody()->SetStatic(true);
    return go;
}

CollidableGO MakeTopWall(Scene &scene, Components &comps)
{
    CollidableGO go = MakeWall(scene, comps);
    go.Trans->WithPosition({0, 5, 0});
    return go;
}

CollidableGO MakeBottomWall(Scene &scene, Components &comps)
{
    CollidableGO go = MakeWall(scene, comps);
    go.Trans->WithPosition({0, -5, 0});
    return go;
}

PaddleGO MakePaddle(Scene &scene, Components &comps)
{
    PaddleGO go;
    go.GO = scene.CreateGameObject();
    go.Trans = scene.AddComponent<TransformComponent>(comps.TransformId, go.GO)
                   ->WithScale({0.25, 4, 1});

    go.Mesh = scene.AddComponent<MeshComponent>(comps.MeshId, go.GO);
    go.Mesh->Initialize(go.Trans, models.BoxPath);
    go.Mesh->Material.Color = Colors::Yellow;

    go.Phys = scene.AddComponent<PhysicsComponent>(comps.PhysicsId, go.GO);
    Collider col;
    col.Data.Box.HalfExtents = {0.125, 2, 0.5};
    col.Type = ColliderType::BOX;

    go.Phys->Initialize(go.Trans, 1000, {col});
    go.Phys->GetRigidBody()->SetGravity({0, 0, 0});
    go.Phys->GetRigidBody()->SetLinearFactor({0, 1, 0});
    go.Phys->GetRigidBody()->SetAngularFactor({0, 0, 0});

    go.Controller = scene.AddComponent<PaddleController>(comps.PaddleControllerId, go.GO);
    return go;
}

PaddleGO MakeLeftPaddle(Scene &scene, Components &comps)
{
    PaddleGO go = MakePaddle(scene, comps);
    go.Trans->WithPosition({-3.5, 0, 0});
    go.Controller->Initialize(go.Phys, go.Trans, true);
    return go;
}

PaddleGO MakeRightPaddle(Scene &scene, Components &comps)
{
    PaddleGO go = MakePaddle(scene, comps);
    go.Trans->WithPosition({3.5, 0, 0});
    go.Controller->Initialize(go.Phys, go.Trans, false);
    return go;
}

PointLightGO MakeSunlight(Scene &scene, Components &comps)
{
    PointLightGO go;
    go.GO = scene.CreateGameObject();
    go.Trans = scene.AddComponent<TransformComponent>(comps.TransformId, go.GO)
                   ->WithPosition({0, 0, 5});

    go.DirLight = scene.AddComponent<PointLightComponent>(comps.PointLightId, go.GO);
    go.DirLight->Initialize(go.Trans, Colors::White, 20);
    return go;
}

void OnGameStart(SceneManager &sceneManager)
{
    Scene &scene = *sceneManager.GetActiveScene();

    Components comps = GetComponentPoolIds(scene);

    CameraGO camera = MakeCamera(scene, comps);

    TriggerGO leftGoal = MakeLeftTrigger(scene, comps);
    TriggerGO rightGoal = MakeRightTrigger(scene, comps);

    PointLightGO sunLight = MakeSunlight(scene, comps);

    BallGO ball = MakeBall(scene, comps, leftGoal.GO->GetId(), rightGoal.GO->GetId());

    BoxGO bg = MakeBackground(scene, comps);

    CollidableGO top = MakeTopWall(scene, comps);
    CollidableGO bottom = MakeBottomWall(scene, comps);

    PaddleGO leftPaddle = MakeLeftPaddle(scene, comps);
    PaddleGO rightPaddle = MakeRightPaddle(scene, comps);
}