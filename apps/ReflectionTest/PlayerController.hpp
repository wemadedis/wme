#pragma once

#include <rte/RTE.hpp>

#include "rte/WindowManager.h"
#include "rte/Utility.hpp"
#include "imgui/imgui.h"


class PlayerController : public RTE::Runtime::Component
{

private:
    RTE::StdComponents::TransformComponent *_transform;
    RTE::StdComponents::CameraComponent *_camera;
    double oldX = 0.0;
    double oldY = 0.0;
    double dx;
    double dy;
    float dt = 0.0f;
    bool cursor = true;
public:
    void Initialize(
        RTE::StdComponents::TransformComponent *trans,
        RTE::StdComponents::CameraComponent *cam)
    {
        _transform = trans;
        _camera = cam;
        _transform->Transform.Rot.x = glm::radians(-90.0f);
        SetGUIDraw([&](){
            ImGui::Begin("Wazzup");
            ImGui::DragFloat("X", &_transform->Transform.Rot.x, 0.1f, -0.1f, 360.1f);
            ImGui::DragFloat("Y", &_transform->Transform.Rot.y, 0.1f, -0.1f, 360.1f);
            ImGui::Text(std::to_string(oldX).c_str());
            ImGui::Text(std::to_string(oldY).c_str());
            ImGui::Text(std::to_string(dx).c_str());
            ImGui::Text(std::to_string(dy).c_str());
            ImGui::End();
        });
        RTE::Platform::WindowManager::GetInstance()->RegisterMousePositionCallback([&](double x, double y){
            if(oldX != x)
            {
                dx = oldX - x;
                oldX = x;
                _transform->Transform.Rot.y += dx*dt*50.0f;
                if(_transform->Transform.Rot.y > 360) _transform->Transform.Rot.y = _transform->Transform.Rot.y - 360;
                if(_transform->Transform.Rot.y < 0) _transform->Transform.Rot.y = 360 - _transform->Transform.Rot.y;
            }
            if(oldY != y)
            {
                dy = oldY - y;
                oldY = y;
                _transform->Transform.Rot.x += dy*dt*50.0f;
                if(_transform->Transform.Rot.x > 35) _transform->Transform.Rot.x = 35;
                if(_transform->Transform.Rot.x < -35) _transform->Transform.Rot.x = -35;
            }
        });
    }

    void Update(float deltaTime)
    {
        dt = deltaTime;

        if (RTE::Platform::WindowManager::GetKey(GLFW_KEY_W) == GLFW_PRESS)
        {
            _transform->Transform.Pos += glm::vec3(_transform->Transform.ModelMatrix() * glm::vec4(0.0f,0.0f,-1.0f,0.0f))*deltaTime;
        }
        if (RTE::Platform::WindowManager::GetKey(GLFW_KEY_A) == GLFW_PRESS)
        {
            _transform->Transform.Pos += glm::vec3(_transform->Transform.ModelMatrix() * glm::vec4(-1.0f,0.0f,0.0f,0.0f))*deltaTime;
        }
        if (RTE::Platform::WindowManager::GetKey(GLFW_KEY_S) == GLFW_PRESS)
        {
            _transform->Transform.Pos += glm::vec3(_transform->Transform.ModelMatrix() * glm::vec4(0.0f,0.0f,1.0f,0.0f))*deltaTime;
        }
        if (RTE::Platform::WindowManager::GetKey(GLFW_KEY_D) == GLFW_PRESS)
        {
            _transform->Transform.Pos += glm::vec3(_transform->Transform.ModelMatrix() * glm::vec4(1.0f,0.0f,0.0f,0.0f))*deltaTime;
        }
        if (RTE::Platform::WindowManager::GetKey(GLFW_KEY_Q) == GLFW_PRESS)
        {
            _transform->Transform.Pos += glm::vec3(_transform->Transform.ModelMatrix() * glm::vec4(0.0f,-1.0f,0.0f,0.0f))*deltaTime;
        }
        if (RTE::Platform::WindowManager::GetKey(GLFW_KEY_E) == GLFW_PRESS)
        {
            _transform->Transform.Pos += glm::vec3(_transform->Transform.ModelMatrix() * glm::vec4(0.0f,1.0f,0.0f,0.0f))*deltaTime;
        }
        if (RTE::Platform::WindowManager::GetKey(GLFW_KEY_T))
        {
            if(cursor){
                RTE::Platform::WindowManager::CursorHide();
                cursor = false;
            }
            else
            {
                RTE::Platform::WindowManager::CursorShow();
                cursor = true;
            }
        }
        if (RTE::Platform::WindowManager::GetKey(GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            std::exit(0);
        }
    }
};