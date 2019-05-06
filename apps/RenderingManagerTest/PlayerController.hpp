#pragma once

#include <rte/RTE.hpp>

#include "rte/WindowManager.h"
#include "rte/Utility.hpp"
#include "imgui/imgui.h"
#include "rte/RenderingManager.h"

class PlayerController : public RTE::Runtime::Component
{

private:
    RTE::StdComponents::TransformComponent *_transform;
    RTE::StdComponents::PhysicsComponent *_physics;
    RTE::StdComponents::CameraComponent *_camera;
    double oldX = 0.0;
    double oldY = 0.0;
    double dx;
    double dy;
    float dt = 0.0f;
    bool rtx = false;
public:
    void Initialize(
        RTE::StdComponents::TransformComponent *trans,
        RTE::StdComponents::PhysicsComponent *phys,
        RTE::StdComponents::CameraComponent *cam)
    {
        _transform = trans;
        _physics = phys;
        _camera = cam;
        _transform->Transform.Rot.x = glm::radians(-90.0f);
        auto wm = RTE::Platform::WindowManager::GetInstance();
        wm->RegisterMousePositionCallback([&](double x, double y){
            if(oldX != x)
            {
                dx = oldX - x;
                oldX = x;
                if(!wm->GetKey(GLFW_KEY_LEFT_SHIFT))
                {
                    if(dx < 5.0f) _transform->Transform.Rot.y += dx*dt*150.0f;
                    if(_transform->Transform.Rot.y > 360) _transform->Transform.Rot.y = _transform->Transform.Rot.y - 360;
                    if(_transform->Transform.Rot.y < 0) _transform->Transform.Rot.y = 360 - _transform->Transform.Rot.y;
                }
                
            }
            if(oldY != y)
            {
                dy = oldY - y;
                oldY = y;
                if(!wm->GetKey(GLFW_KEY_LEFT_SHIFT))
                {
                    if(dy < 5.0f) _transform->Transform.Rot.x += dy*dt*150.0f;
                    if(_transform->Transform.Rot.x > 35) _transform->Transform.Rot.x = 35;
                    if(_transform->Transform.Rot.x < -35) _transform->Transform.Rot.x = -35;
                }
            }
        });
        
        SetGUIDraw([=]() {
            ImGui::Begin("Inspector");
            ImGui::Checkbox("RTX ON:", &rtx);
            ImGui::End();
        });
    }

    void Update(float deltaTime)
    {
        RTE::Rendering::RenderingManager::GetInstance()->SetRTEnabled(rtx);
        dt = deltaTime;

        if (RTE::Platform::WindowManager::GetKey(GLFW_KEY_W) == GLFW_PRESS)
        {
            _transform->Transform.Pos += glm::vec3(_transform->Transform.ModelMatrix() * glm::vec4(0.0f,0.0f,-5.0f,0.0f))*deltaTime;
        }
        if (RTE::Platform::WindowManager::GetKey(GLFW_KEY_A) == GLFW_PRESS)
        {
            _transform->Transform.Pos += glm::vec3(_transform->Transform.ModelMatrix() * glm::vec4(-5.0f,0.0f,0.0f,0.0f))*deltaTime;
        }
        if (RTE::Platform::WindowManager::GetKey(GLFW_KEY_S) == GLFW_PRESS)
        {
            _transform->Transform.Pos += glm::vec3(_transform->Transform.ModelMatrix() * glm::vec4(0.0f,0.0f,5.0f,0.0f))*deltaTime;
        }
        if (RTE::Platform::WindowManager::GetKey(GLFW_KEY_D) == GLFW_PRESS)
        {
            _transform->Transform.Pos += glm::vec3(_transform->Transform.ModelMatrix() * glm::vec4(5.0f,0.0f,0.0f,0.0f))*deltaTime;
        }
        if (RTE::Platform::WindowManager::GetKey(GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            _transform->Transform.Pos += glm::vec3(0,5,0)*deltaTime;
        }
        if (RTE::Platform::WindowManager::GetKey(GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        {
            _transform->Transform.Pos += glm::vec3(0,-5,0)*deltaTime;
        }
    }
};