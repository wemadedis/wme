#include "rte/CameraComponent.hpp"
#include "rte/RenderingManager.h"



namespace RTE::StdComponents
{


CameraComponent::~CameraComponent()
{

}

void CameraComponent::Initialize(TransformComponent *transformComponent)
{
    _renderingManager = RenderingManager::GetInstance();
    _transform = transformComponent;
    _renderingManager->RegisterCameraComponent(this);
}

TransformComponent* CameraComponent::GetTransformComponent()
{
    return _transform;
}
//TODO: How to document that?
//https://www.3dgep.com/understanding-the-view-matrix/#The_View_Matrix
glm::mat4 CameraComponent::ViewMatrix()
{
    glm::vec3 pos = _transform->Transform.Pos;
    glm::vec3 rot = _transform->Transform.Rot;
    float cosPitch = cos(glm::radians(rot.x));
    float sinPitch = sin(glm::radians(rot.x));
    float cosYaw = cos(glm::radians(rot.y));
    float sinYaw = sin(glm::radians(rot.y));

    glm::vec3 x = { cosYaw, 0, -sinYaw };
    glm::vec3 y = { sinYaw * sinPitch, cosPitch, cosYaw * sinPitch };
    glm::vec3 z = { sinYaw * cosPitch, -sinPitch, cosPitch * cosYaw };

    glm::mat4 viewMatrix = {
        glm::vec4(       x.x,            -y.x,            z.x,      0 ),
        glm::vec4(       x.y,            -y.y,            z.y,      0 ),
        glm::vec4(       x.z,            -y.z,            z.z,      0 ),
        glm::vec4( -glm::dot( x, pos ), -glm::dot( y, pos ), -glm::dot( z, pos ), 1 )
    };
     
    return viewMatrix;
}

glm::mat4 CameraComponent::ProjectionMatrix()
{
    glm::vec2 frameSize = glm::vec2(_renderingManager->GetRendererFrameSize());
    if(Projection == ProjectionMode::PERSPECTIVE)
    {
        return glm::perspective(glm::radians(FieldOfView), frameSize.x/frameSize.y, NearPlane, FarPlane);
    }
    return glm::ortho(-frameSize.x*0.5f, frameSize.x*0.5f, -frameSize.y*0.5f, frameSize.y*0.5f, NearPlane, FarPlane);
}

void CameraComponent::Update(float deltaTime)
{

}

} // namespace RTE::StdComponents


