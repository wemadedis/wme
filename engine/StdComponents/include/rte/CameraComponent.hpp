#pragma once
#include "rte/Component.h"
#include "rte/TransformComponent.hpp"

namespace RTE::Rendering
{
    class RenderingManager;
}

namespace RTE::StdComponents
{
using namespace RTE::Rendering;
class CameraComponent : public Runtime::Component
{
public:
    enum class ProjectionMode { PERSPECTIVE, ORTHOGONAL };
private:
    RenderingManager *_renderingManager;
    TransformComponent *_transform;

    
    
public:
    ProjectionMode Projection = ProjectionMode::PERSPECTIVE;
    float FieldOfView = 60.0f;
    float NearPlane = 0.5f, FarPlane = 1000.0f;
    CameraComponent() = default;
    ~CameraComponent();
    void Initialize(TransformComponent *transformComponent);
    TransformComponent* GetTransformComponent();
    
    virtual void Update(float deltaTime) override;
};

}