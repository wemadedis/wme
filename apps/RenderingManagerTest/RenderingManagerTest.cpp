#include <rte/RTE.hpp>

#include "PlayerController.hpp"
#include "imgui/imgui.h"
#include "rte/CameraComponent.hpp"
#include "rte/Definitions.hpp"
#include "rte/StdComponentsMain.hpp"
#include <iostream>

using namespace RTE;

void ConfigureGame(RTEConfig &config)
{
    config.WindowConfig.WindowHeight = 800;
    config.WindowConfig.WindowWidth = 800;
    config.WindowConfig.WindowName = "Eggplant";
    config.GraphicsConfig.UseRaytracing = true;
    config.AssetConfig.Meshes = {Utilities::GetFileFromAssets("models/monkey.ply")};
    config.AssetConfig.Textures = {Utilities::GetFileFromAssets("textures/rte.png")};
}

using namespace RTE::StdComponents;
void SetTransformGUI(std::string name, TransformComponent *tc)
{
    tc->SetGUIDraw([=](){
        ImGui::Begin("Inspector");
        ImGui::Text(name.c_str());
        ImGui::DragFloat("X Pos:", &tc->Transform.Pos.x, 0.1f);
        ImGui::DragFloat("Y Pos:", &tc->Transform.Pos.y, 0.1f);
        ImGui::DragFloat("Z Pos:", &tc->Transform.Pos.z, 0.1f);

        ImGui::DragFloat("X Rot:", &tc->Transform.Rot.x, 0.5f);
        ImGui::DragFloat("Y Rot:", &tc->Transform.Rot.y, 0.5f);
        ImGui::DragFloat("Z Rot:", &tc->Transform.Rot.z, 0.5f);
        ImGui::End();
    });
}

void SetPointLightGUI(std::string name, PointLightComponent *pl)
{
    pl->SetGUIDraw([=](){
        ImGui::Begin("Inspector");
        ImGui::Text(name.c_str());
        ImGui::DragFloat("X Radius:", &pl->Radius, 0.1f);
        ImGui::ColorPicker3("Color", &pl->Color[0]);
        ImGui::End();
    });
}

void OnGameStart(Runtime::SceneManager &sceneManager)
{
    using namespace Runtime;
    using namespace StdComponents;

    Scene *scene = sceneManager.MakeScene();
    sceneManager.SetActiveScene(scene);

    // Init components pools
    ComponentPoolId transIndex = scene->DefineComponent<TransformComponent, 50>();
    ComponentPoolId meshIndex = scene->DefineComponent<MeshComponent, 50>();
    ComponentPoolId pcIndex = scene->DefineComponent<PlayerController, 50>();
    ComponentPoolId camIndex = scene->DefineComponent<CameraComponent, 50>();
    ComponentPoolId plIndex = scene->DefineComponent<PointLightComponent, 50>();
    ComponentPoolId dlIndex = scene->DefineComponent<DirectionalLightComponent, 50>();

    // Setup our game object
    GameObject *go = scene->CreateGameObject();
    TransformComponent *transComp = scene->AddComponent<TransformComponent>(transIndex, go);
    MeshComponent *meshComp = scene->AddComponent<MeshComponent>(meshIndex, go);
    transComp->Transform.Rot.x = -90;

    GameObject *go2 = scene->CreateGameObject();

    CameraComponent *camera = scene->AddComponent<CameraComponent>(camIndex, go2);
    camera->SetEnabled(true);
    TransformComponent *trans2 = scene->AddComponent<TransformComponent>(transIndex, go2);
    trans2->Transform.Pos = glm::vec3(0.0f, 0.0f, 10.0f);
    camera->Initialize(trans2);
    camera->BackgroundColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    PlayerController *pc = scene->AddComponent<PlayerController>(pcIndex, go);
    pc->SetEnabled(true);
    pc->Initialize(trans2, nullptr, camera);

    GameObject *go3 = scene->CreateGameObject();
    auto trans3 = scene->AddComponent<TransformComponent>(transIndex, go3);
    auto pointLight = scene->AddComponent<PointLightComponent>(plIndex, go3);
    trans3->Transform.Pos = glm::vec3(0.0f, 1.0f, 2.0f);
    pointLight->Initialize(trans3, Colors::White, 5.0f);
    SetTransformGUI("Point Light - Transform", trans3);
    SetPointLightGUI("Point Light - Properties", pointLight);

    GameObject *go4 = scene->CreateGameObject();
    auto trans4 = scene->AddComponent<TransformComponent>(transIndex, go4);
    //auto dirLight = scene->AddComponent<DirectionalLightComponent>(dlIndex, go4);
    trans4->Transform.Pos = glm::vec3(0.0f, 0.0f, 0.0f);
    //dirLight->Initialize(trans4, Colors::White);

    GameObject *go5 = scene->CreateGameObject();
    auto trans5 = scene->AddComponent<TransformComponent>(transIndex, go5);
    trans5->Transform.Scale = glm::vec3(5.0f);
    trans5->Transform.Pos.y = -1.0f;
    trans5->Transform.Rot.x = 90.0f;
    trans5->Transform.Scale = glm::vec3(10,10,10);
    auto quadMesh = scene->AddComponent<MeshComponent>(meshIndex, go5);

    quadMesh->Initialize(trans5, RenderingManager::QUAD);
    quadMesh->Material.Reflectivity = 0.5f;

    quadMesh->SetGUIDraw([=]() {
        ImGui::Begin("Inspector");
        ImGui::DragFloat("Reflectivity", &quadMesh->Material.Reflectivity, 0.002f, 0.0f, 1.0f);
        ImGui::DragFloat("Specular", &quadMesh->Material.Specular, 0.002f, 0.0f, 1.0f);
        ImGui::DragFloat("Quad X Rot:", &trans5->Transform.Rot.x, 0.1f, 0.0f, 360.1f);
        ImGui::DragFloat("Field of view:", &camera->FieldOfView, 0.1f, 0.0f, 180.0f);
        ImGui::DragFloat("Far plane:", &camera->FarPlane, 0.1f, 0.0f, 180.0f);
        ImGui::DragFloat("Near plane:", &camera->NearPlane, 0.1f, 0.0f, 180.0f);
        ImGui::End();
    });

    auto monkey = RTE::Utilities::GetFileFromAssets("models/monkey.ply");
    meshComp->Initialize(transComp, monkey);
}