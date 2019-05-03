#include <rte/RTE.hpp>

#include "PlayerController.hpp"
#include "rte/TestGenericPool.hpp"

#include <iostream>

using namespace RTE;
using namespace RTE::Runtime;
using namespace RTE::StdComponents;
using namespace RTE::Utilities;

const RelativeFilePath BoxPath = "models/box.ply";
const RelativeFilePath BoxTexturePath = "textures/rte.png";
const RelativeFilePath PlanePath = "models/plane.ply";
const RelativeFilePath PlaneTexturePath = "textures/rte.png";

AbsoluteFilePath AbsBoxPath;
AbsoluteFilePath AbsBoxTexturePath;
AbsoluteFilePath AbsPlanePath;
AbsoluteFilePath AbsPlaneTexturePath;

void ConfigureGame(RTEConfig &config)
{
    AbsBoxPath = GetFileFromAssets(BoxPath);
    AbsBoxTexturePath = GetFileFromAssets(BoxTexturePath);
    AbsPlanePath = GetFileFromAssets(PlanePath);
    AbsPlaneTexturePath = GetFileFromAssets(PlaneTexturePath);
    config.WindowConfig.WindowName = "Physics Test";
    config.GraphicsConfig.UseRaytracing = true;
    config.WindowConfig.WindowWidth = 600;
    config.WindowConfig.WindowHeight = 700;
    config.AssetConfig.Meshes = {AbsBoxPath,
                                 AbsPlanePath};
    config.AssetConfig.Textures = {AbsBoxTexturePath, AbsPlaneTexturePath};
}

struct ComponentIds
{
    ComponentPoolId TransformPoolId;
    ComponentPoolId PhysicsPoolId;
    ComponentPoolId MeshPoolId;
    ComponentPoolId PCPoolId;
    ComponentPoolId DirLightPoolId;
    ComponentPoolId CameraPoolId;
};

struct StaticBox
{
    GameObject *Go;
    TransformComponent *Trans;
    PhysicsComponent *Phys;
    MeshComponent *Mesh;
};

struct ControlledBox
{
    GameObject *Go;
    TransformComponent *Trans;
    PhysicsComponent *Phys;
    PlayerController *Controller;
    MeshComponent *Mesh;
};

struct Plane
{
    GameObject *Go;
    TransformComponent *Trans;
    PhysicsComponent *Phys;
    MeshComponent *Mesh;
};

RTE::Physics::Collider MakeBoxCollider(glm::vec3 halfExtents)
{
    RTE::Physics::Collider boxCollider;
    boxCollider.Type = RTE::Physics::ColliderType::BOX;
    boxCollider.Data.Box.HalfExtents = halfExtents;
    return boxCollider;
}

StaticBox MakeStaticBox(Scene *scene, ComponentIds &comps)
{
    StaticBox box;
    box.Go = scene->CreateGameObject();
    box.Trans = scene->AddComponent<TransformComponent>(comps.TransformPoolId, box.Go);
    box.Phys = scene->AddComponent<PhysicsComponent>(comps.PhysicsPoolId, box.Go);
    box.Mesh = scene->AddComponent<MeshComponent>(comps.MeshPoolId, box.Go);

    box.Trans->Initialize({0, 1, 0}, {0, 0, 0}, {1, 1, 1});
    box.Phys->Initialize(box.Trans, 1, {MakeBoxCollider(glm::vec3(1))});
    box.Mesh->Initialize(box.Trans, AbsBoxPath, AbsBoxTexturePath);
    return box;
}

ControlledBox MakeControlledBox(Scene *scene, ComponentIds &comps)
{
    ControlledBox box;
    box.Go = scene->CreateGameObject();
    box.Trans = scene->AddComponent<TransformComponent>(comps.TransformPoolId, box.Go);
    box.Phys = scene->AddComponent<PhysicsComponent>(comps.PhysicsPoolId, box.Go);
    box.Mesh = scene->AddComponent<MeshComponent>(comps.MeshPoolId, box.Go);
    box.Controller = scene->AddComponent<PlayerController>(comps.PCPoolId, box.Go);

    box.Trans->Initialize({0, 5, 0}, {0, 0, 0}, {1, 1, 1});
    box.Phys->Initialize(box.Trans, 1, {MakeBoxCollider(glm::vec3(1))});
    box.Mesh->Initialize(box.Trans, AbsBoxPath, AbsBoxTexturePath);
    box.Controller->Initialize(box.Trans, box.Phys);
    return box;
}

Plane MakePlane(Scene *scene, ComponentIds &comps)
{
    Plane plane;
    plane.Go = scene->CreateGameObject();
    plane.Trans = scene->AddComponent<TransformComponent>(comps.TransformPoolId, plane.Go);
    plane.Phys = scene->AddComponent<PhysicsComponent>(comps.PhysicsPoolId, plane.Go);
    plane.Mesh = scene->AddComponent<MeshComponent>(comps.MeshPoolId, plane.Go);
    return plane;
}

Plane MakeGround(Scene *scene, ComponentIds &comps)
{
    Plane plane = MakePlane(scene, comps);

    glm::vec3 scale(glm::vec3(10, 1, 10));
    plane.Trans->Initialize({0, -10, 0}, {0, 0, 0}, scale);
    plane.Phys->Initialize(plane.Trans, 1, {MakeBoxCollider(scale)});
    plane.Mesh->Initialize(plane.Trans, AbsBoxPath, AbsBoxTexturePath);
    plane.Phys->GetRigidBody()->SetKinematic(true);
    return plane;
}

Plane MakeBackground(Scene *scene, ComponentIds &comps)
{
    Plane plane = MakePlane(scene, comps);

    glm::vec3 scale(glm::vec3(10, 1, 10));
    plane.Trans->Initialize({0, 0, 0}, {0, 0, glm::radians(-90.0f)}, scale);
    plane.Phys->Initialize(plane.Trans, 1, {MakeBoxCollider(scale)});
    plane.Mesh->Initialize(plane.Trans, AbsBoxPath, AbsBoxTexturePath);
    plane.Phys->GetRigidBody()->SetKinematic(true);
    return plane;
}

ComponentIds DefineComponents(Scene *scene)
{
    ComponentIds componentIds;
    componentIds.PCPoolId = scene->DefineComponent<PlayerController, 1>();
    componentIds.TransformPoolId = scene->DefineComponent<TransformComponent, 6>();
    componentIds.PhysicsPoolId = scene->DefineComponent<PhysicsComponent, 4>();
    componentIds.MeshPoolId = scene->DefineComponent<MeshComponent, 4>();
    componentIds.DirLightPoolId = scene->DefineComponent<DirectionalLightComponent, 1>();
    componentIds.CameraPoolId = scene->DefineComponent<CameraComponent, 1>();
    return componentIds;
}

void OnGameStart(Runtime::SceneManager &sceneManager)
{
    Scene *scene = sceneManager.GetActiveScene();

    // Init components pools
    ComponentIds componentIds = DefineComponents(scene);

    // Sunlight
    GameObject *sun = scene->CreateGameObject();
    TransformComponent *sunLocation = scene->AddComponent<TransformComponent>(componentIds.TransformPoolId, sun);
    DirectionalLightComponent *sunLight = scene->AddComponent<DirectionalLightComponent>(componentIds.DirLightPoolId, sun);
    sunLocation->Initialize({0, 0, 0}, {50, 50, 0}, {0, 0, 0});
    sunLight->Initialize(sunLocation, Colors::Cyan);

    // Camera
    GameObject *camera = scene->CreateGameObject();
    TransformComponent *cameraTransform = scene->AddComponent<TransformComponent>(componentIds.TransformPoolId, camera);
    CameraComponent *cameraComp = scene->AddComponent<CameraComponent>(componentIds.CameraPoolId, camera);
    cameraTransform->Initialize(glm::vec3(0, 0, 50), glm::vec3(0, 0, 0), {1, 1, 1});
    cameraComp->Initialize(cameraTransform);

    // Static box
    StaticBox staticBox = MakeStaticBox(scene, componentIds);

    // Controlled box
    ControlledBox controlledBox = MakeControlledBox(scene, componentIds);

    // Background
    Plane bgPlane = MakeBackground(scene, componentIds);

    // Ground
    Plane groundPlane = MakeGround(scene, componentIds);
}