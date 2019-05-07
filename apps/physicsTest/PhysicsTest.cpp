#include <imgui/imgui.h>
#include <rte/RTE.hpp>

#include "PlayerController.hpp"
#include "rte/TestGenericPool.hpp"

#include <iostream>

using namespace RTE;
using namespace RTE::Runtime;
using namespace RTE::StdComponents;
using namespace RTE::Utilities;
using namespace RTE::Physics;

const RelativeFilePath BoxPath = "models/box.ply";
const RelativeFilePath PlanePath = "models/plane.ply";
const RelativeFilePath RedPath = "textures/red.png";
const RelativeFilePath GreenPath = "textures/green.png";
const RelativeFilePath BluePath = "textures/blue.png";
const RelativeFilePath CyanPath = "textures/cyan.png";

AbsoluteFilePath AbsBoxPath;
AbsoluteFilePath AbsPlanePath;
AbsoluteFilePath AbsRedPath;
AbsoluteFilePath AbsGreenPath;
AbsoluteFilePath AbsBluePath;
AbsoluteFilePath AbsCyanPath;

void ConfigureGame(RTEConfig &config)
{
    AbsBoxPath = GetFileFromAssets(BoxPath);
    AbsPlanePath = GetFileFromAssets(PlanePath);
    AbsRedPath = GetFileFromAssets(RedPath);
    AbsGreenPath = GetFileFromAssets(GreenPath);
    AbsBluePath = GetFileFromAssets(BluePath);
    AbsCyanPath = GetFileFromAssets(CyanPath);

    config.WindowConfig.WindowName = "Physics Test";
    config.GraphicsConfig.UseRaytracing = false;
    config.WindowConfig.WindowWidth = 1200;
    config.WindowConfig.WindowHeight = 800;
    config.AssetConfig.Meshes = {AbsBoxPath, AbsPlanePath};
    config.AssetConfig.Textures = {AbsRedPath, AbsGreenPath, AbsBluePath, AbsCyanPath};
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

Collider MakeBoxCollider(glm::vec3 halfExtents)
{
    Collider boxCollider;
    boxCollider.Type = ColliderType::BOX;
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

    box.Trans->SetGUIDraw([=]() {
        ImGui::Begin("Box");
        ImGui::DragFloat("pos x", &box.Trans->Transform.Pos.x);
        ImGui::DragFloat("pos y", &box.Trans->Transform.Pos.y);
        ImGui::DragFloat("pos z", &box.Trans->Transform.Pos.z);
        ImGui::DragFloat("rot x", &box.Trans->Transform.Rot.x);
        ImGui::DragFloat("rot y", &box.Trans->Transform.Rot.y);
        ImGui::DragFloat("rot z", &box.Trans->Transform.Rot.z);
        if (ImGui::Button("Set to 100"))
        {
            box.Trans->Transform.Pos.y = 100;
        }
        if (ImGui::Button("Clear forces"))
        {
            box.Phys->GetRigidBody()->ClearForces();
        }
        ImGui::End();
    });
    box.Trans->Initialize({0, 25, 6}, {0, 0, 0}, {1, 1, 1});
    box.Phys->Initialize(box.Trans, 5, {MakeBoxCollider({1, 1, 1})});
    box.Mesh->Initialize(box.Trans, AbsBoxPath, AbsBluePath);
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

    plane.Trans->SetGUIDraw([=]() {
        ImGui::Begin("Ground");
        ImGui::DragFloat("pos x", &plane.Trans->Transform.Pos.x);
        ImGui::DragFloat("pos y", &plane.Trans->Transform.Pos.y);
        ImGui::DragFloat("pos z", &plane.Trans->Transform.Pos.z);
        ImGui::DragFloat("rot x", &plane.Trans->Transform.Rot.x);
        ImGui::DragFloat("rot y", &plane.Trans->Transform.Rot.y);
        ImGui::DragFloat("rot z", &plane.Trans->Transform.Rot.z);
        ImGui::End();
    });
    plane.Trans->Initialize({0, -0.5, 6}, {30, 0, 0}, {1, 1, 1});
    plane.Phys->Initialize(plane.Trans, 0, {MakeBoxCollider({5, 1, 5})});

    plane.Mesh->Initialize(plane.Trans, AbsPlanePath, AbsCyanPath);

    return plane;
}

ComponentIds DefineComponents(Scene *scene)
{
    ComponentIds componentIds;
    componentIds.PCPoolId = scene->DefineComponent<PlayerController, 1>();
    componentIds.TransformPoolId = scene->DefineComponent<TransformComponent, 6>();
    componentIds.PhysicsPoolId = scene->DefineComponent<PhysicsComponent, 2>();
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
    TransformComponent *sunTransform = scene->AddComponent<TransformComponent>(componentIds.TransformPoolId, sun);
    DirectionalLightComponent *sunLight = scene->AddComponent<DirectionalLightComponent>(componentIds.DirLightPoolId, sun);
    sunTransform->Initialize({0, 0, 0}, {-45, -45, 0}, {0, 0, 0});
    sunLight->Initialize(sunTransform, Colors::White);

    // Camera
    GameObject *camera = scene->CreateGameObject();
    TransformComponent *cameraTransform = scene->AddComponent<TransformComponent>(componentIds.TransformPoolId, camera);
    CameraComponent *cameraComp = scene->AddComponent<CameraComponent>(componentIds.CameraPoolId, camera);
    cameraComp->BackgroundColor = Colors::Black;
    cameraTransform->Initialize(glm::vec3(20, 20, 40), glm::vec3(-20, 25, 0), {1, 1, 1});
    cameraComp->Initialize(cameraTransform);

    // Static box
    StaticBox staticBox = MakeStaticBox(scene, componentIds);

    // Controlled box
    //ControlledBox controlledBox = MakeControlledBox(scene, componentIds);

    // Background

    // Ground
    Plane groundPlane = MakeGround(scene, componentIds);
}
//Plane bgPlane = MakeBackground(scene, componentIds);