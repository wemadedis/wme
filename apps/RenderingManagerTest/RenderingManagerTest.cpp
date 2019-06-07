#include <rte/RTE.hpp>

#include "PlayerController.hpp"
#include "imgui/imgui.h"
#include "rte/CameraComponent.hpp"
#include "rte/Definitions.hpp"
#include "rte/StdComponentsMain.hpp"
#include <iostream>

using namespace RTE;
using namespace RTE::StdComponents;
using namespace RTE::Runtime;

void ConfigureGame(RTEConfig &config)
{
    config.WindowConfig.WindowHeight = 800;
    config.WindowConfig.WindowWidth = 800;
    config.WindowConfig.WindowName = "Eggplant";
    config.GraphicsConfig.UseRaytracing = true;
    config.GraphicsConfig.FramesPerSecond = 144;
    config.AssetConfig.Meshes = {RTE::Utilities::GetFileFromAssets("models/monkey.ply"), RTE::Utilities::GetFileFromAssets("models/realPlane.fbx")};
    config.AssetConfig.Textures = {RTE::Utilities::GetFileFromAssets("textures/rte.png")};
}


ComponentPoolId transIndex;
ComponentPoolId meshIndex;
ComponentPoolId pcIndex;
ComponentPoolId camIndex;
ComponentPoolId plIndex;
ComponentPoolId dlIndex;
ComponentPoolId orIndex;
Scene *scene;


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
        //ImGui::ColorPicker4("Color", &pl->Color[0]);
        ImGui::End();
    });
}

GameObject* GO()
{
    return scene->CreateGameObject();
}

template<typename T>
T* GetComponent(GameObject *go)
{
    auto comps = go->GetComponents();
    for(uint32_t compIndex = 0; compIndex < comps->size() ; compIndex++)
    {
        T* component  = dynamic_cast<T*>(comps->at(compIndex));
        if(component != nullptr) return component;
    }
    return nullptr;
}

TransformComponent* TransComp(GameObject* go)
{
    return scene->AddComponent<TransformComponent>(transIndex, go);
}

MeshComponent* MeshComp(GameObject* go)
{
    return scene->AddComponent<MeshComponent>(meshIndex, go);
}

PointLightComponent* PLComp(GameObject* go)
{
    return scene->AddComponent<PointLightComponent>(plIndex, go);
}

GameObject* CreateQuadObject()
{
    GameObject *gameObject = GO();
    auto tc = TransComp(gameObject);
    auto mc = MeshComp(gameObject);
    mc->Initialize(tc, RTE::Utilities::GetFileFromAssets("models/realPlane.fbx"), RTE::Utilities::GetFileFromAssets("textures/rte.png"));
    return gameObject;
}

GameObject* CreatePointLight()
{
    GameObject *gameObject = GO();
    auto transComp = TransComp(gameObject);
    auto pointLight = PLComp(gameObject);
    pointLight->Initialize(transComp, Colors::White, 5.0f);
    return gameObject;
}


class ObjectRotator : public Runtime::Component
{
    TransformComponent *t;
public:
    void Initialize(TransformComponent *trans)
    {
        t = trans;
    }

    void Update(float deltaTime)
    {
        t->Transform.Rot.x += 20.0f * deltaTime;
        t->Transform.Rot.y += 20.0f * deltaTime;
        t->Transform.Rot.z += 20.0f * deltaTime;
    }
};


GameObject* CreateMeshObject(std::string meshPath, glm::vec3 pos)
{
    GameObject *gameObject = GO();
    auto tc = TransComp(gameObject);
    tc->Transform.Pos = pos;
    auto mc = MeshComp(gameObject);
    mc->Initialize(tc, RTE::Utilities::GetFileFromAssets(meshPath), RTE::Utilities::GetFileFromAssets("textures/rte.png"));
    auto or = scene->AddComponent<ObjectRotator>(orIndex, gameObject);
    or->Initialize(tc);
    return gameObject;
}


void OnGameStart(Runtime::SceneManager &sceneManager)
{

    scene = sceneManager.MakeScene();
    sceneManager.SetActiveScene(scene);

    // Init components pools
    transIndex = scene->DefineComponent<TransformComponent, 11000>();
    meshIndex = scene->DefineComponent<MeshComponent, 11000>();
    pcIndex = scene->DefineComponent<PlayerController, 50>();
    camIndex = scene->DefineComponent<CameraComponent, 50>();
    plIndex = scene->DefineComponent<PointLightComponent, 50>();
    dlIndex = scene->DefineComponent<DirectionalLightComponent, 50>();
    orIndex = scene->DefineComponent<ObjectRotator, 11000>();

    // Setup our game object
    GameObject *go = scene->CreateGameObject();
    TransformComponent *transComp = scene->AddComponent<TransformComponent>(transIndex, go);
    MeshComponent *meshComp = scene->AddComponent<MeshComponent>(meshIndex, go);
    transComp->Transform.Rot.x = -90;
    auto monkey = RTE::Utilities::GetFileFromAssets("models/monkey.ply");
    meshComp->Material.Color = Colors::Red;
    meshComp->Initialize(transComp, monkey);

    GameObject *go2 = scene->CreateGameObject();

    CameraComponent *camera = scene->AddComponent<CameraComponent>(camIndex, go2);
    camera->BackgroundColor = Colors::White;
    TransformComponent *trans2 = scene->AddComponent<TransformComponent>(transIndex, go2);
    trans2->Transform.Pos = glm::vec3(0.0f, 0.0f, 10.0f);
    camera->Initialize(trans2);
    //camera->BackgroundColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
    PlayerController *pc = scene->AddComponent<PlayerController>(pcIndex, go);
    pc->SetEnabled(true);
    pc->Initialize(trans2, nullptr, camera);

    for(int x = 0; x < 5; x++)
    {
        for(int z = 0; z < 5; z++)
        {
            CreateMeshObject("models/monkey.ply", glm::vec3(x*2,0.0f,z*2));
        }
    }

    auto pointLight = CreatePointLight();
    auto plComp = GetComponent<PointLightComponent>(pointLight);
    auto plTrans = plComp->GetTransformComponent();
    plTrans->Transform.Pos = glm::vec3(0.0f, 1.0f, 2.0f);
    SetPointLightGUI("Point Light - Properties", plComp);
    SetTransformGUI("Point Light - Transform", plTrans);



    GameObject *go4 = scene->CreateGameObject();
    auto trans4 = scene->AddComponent<TransformComponent>(transIndex, go4);
    auto dirLight = scene->AddComponent<DirectionalLightComponent>(dlIndex, go4);
    trans4->Transform.Rot = glm::vec3(0.0f, 0.0f, 0.0f);
    dirLight->Initialize(trans4, Colors::White);


    auto quad = CreateQuadObject();
    auto quadMeshComp = GetComponent<MeshComponent>(quad);
    auto &quadTrans = quadMeshComp->GetTransformComponent()->Transform;

    quadMeshComp->Material.Reflectivity = 0.0f;
    quadTrans.Pos.z = -10.0f;
    quadTrans.Rot.x = -90.0f;
    quadTrans.Scale = glm::vec3(0.5,1.0,0.5);


    auto quad2 = CreateQuadObject();
    auto quadMeshComp2 = GetComponent<MeshComponent>(quad2);
    auto quadTrans2 = quadMeshComp2->GetTransformComponent();

    quadTrans2->Transform.Pos.y = -1.0f;
    
    quadTrans2->Transform.Scale = glm::vec3(0.5,1.0,0.5);
    quadMeshComp2->Material.Reflectivity = 0.5f;

    quadMeshComp2->SetGUIDraw([=]() {
        ImGui::Begin("Inspector");
        ImGui::DragFloat("Reflectivity", &quadMeshComp2->Material.Reflectivity, 0.002f, 0.0f, 1.0f);
        ImGui::DragFloat("Specular", &quadMeshComp2->Material.Specular, 0.002f, 0.0f, 1.0f);
        ImGui::DragFloat("Shininess", &quadMeshComp2->Material.Shininess, 0.1f, 0.0f, 1000.0f);
        ImGui::DragFloat("DirLight X Rot:", &trans4->Transform.Rot.x, 0.1f, 0.0f, 360.1f);
        ImGui::DragFloat("Monkey X Rot:", &transComp->Transform.Scale.x, 0.1f, 0.0f, 360.1f);
        ImGui::DragFloat("Field of view:", &camera->FieldOfView, 0.1f, 0.0f, 180.0f);
        ImGui::DragFloat("Far plane:", &camera->FarPlane, 0.1f, 0.0f, 180.0f);
        ImGui::DragFloat("Near plane:", &camera->NearPlane, 0.1f, 0.0f, 180.0f);
        ImGui::ColorPicker4("Color", &meshComp->Material.Color[0]);
        ImGui::End();
    });


}