#pragma once

#include <rte/RTE.hpp>
#include "glm/glm.hpp"

using namespace RTE;
using namespace StdComponents;
class Rotator : public Runtime::Component
{
private:
    TransformComponent *_transform;
    float _xRotMagnitude;
    float _yRotMagnitude;
    float _zRotMagnitude;

public:
    Rotator(){}
    void Initialize(TransformComponent *tc)
    {
        _transform = tc;
        _xRotMagnitude = 0.f;
        _zRotMagnitude = 0.f;
        _yRotMagnitude = 0.f;
    }

    void setRotMagnitude(glm::vec3 rotMagnitude)
    {
        _xRotMagnitude = rotMagnitude.x;
        _yRotMagnitude = rotMagnitude.y;
        _zRotMagnitude = rotMagnitude.z;
    }

    void Update(float deltaTime)
    {
        _transform->Transform.Rot.x += _xRotMagnitude*deltaTime;
        _transform->Transform.Rot.y += _yRotMagnitude*deltaTime;
        _transform->Transform.Rot.z += _zRotMagnitude*deltaTime;

        if(_transform->Transform.Rot.x > 360) _transform->Transform.Rot.x -= 360;
        if(_transform->Transform.Rot.y > 360) _transform->Transform.Rot.y -= 360;
        if(_transform->Transform.Rot.z > 360) _transform->Transform.Rot.z -= 360;
    }


};