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
    config.WindowConfig.WindowHeight = 400;
    config.WindowConfig.WindowWidth = 1000;
    config.WindowConfig.WindowName = "Eggplant";
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

RTE::Physics::Collider MakePlaneCollider()
{
    Physics::Collider planeCollider;
    planeCollider.Type = Physics::ColliderType::PLANE;
    planeCollider.Data.Plane.N = {0, 1, 0};
    planeCollider.Data.Plane.D = 0;
    return planeCollider;
}

StaticBox MakeStaticBox(Scene *scene, ComponentIds &comps)
{
    StaticBox box;
    box.Go = scene->CreateGameObject();
    box.Trans = scene->AddComponent<TransformComponent>(comps.TransformPoolId, box.Go);
    box.Phys = scene->AddComponent<PhysicsComponent>(comps.PhysicsPoolId, box.Go);
    box.Mesh = scene->AddComponent<MeshComponent>(comps.MeshPoolId, box.Go);

    box.Trans->Initialize();
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

    box.Trans->Initialize();
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

    glm::vec3 scale(glm::vec3(100, 1, 100));
    plane.Trans->Initialize(glm::vec3(0), glm::vec3(0), scale);
    plane.Phys->Initialize(plane.Trans, 1, {MakeBoxCollider(scale)});
    plane.Mesh->Initialize(plane.Trans, AbsBoxPath, AbsBoxTexturePath);
    return plane;
}

Plane MakeBackground(Scene *scene, ComponentIds &comps)
{
    Plane plane = MakePlane(scene, comps);

    glm::vec3 scale(glm::vec3(50, 1, 50));
    plane.Trans->Initialize(glm::vec3(0), glm::vec3(0), scale);
    plane.Phys->Initialize(plane.Trans, 1, {MakeBoxCollider(scale)});
    plane.Mesh->Initialize(plane.Trans, AbsBoxPath, AbsBoxTexturePath);
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
    sunLight->Initialize(sunLocation, Colors::Red);

    // Camera
    GameObject *camera = scene->CreateGameObject();
    CameraComponent *cameraComp = scene->AddComponent<CameraComponent>(componentIds.CameraPoolId, camera);
    TransformComponent *cameraTransform = scene->AddComponent<TransformComponent>(componentIds.TransformPoolId, camera);
    cameraTransform->Initialize(glm::vec3(0, 0, 50), glm::vec3(0, 0, 0), glm::vec3(1));
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