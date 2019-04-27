#pragma once

#include "glm/glm.hpp"

#include "SFML/Audio.hpp"

namespace RTE::Audio
{
class Listener {
public:
    static void SetPosition(glm::vec3);
    static glm::vec3 GetPosition();
    static void SetGlobalVolume(float);
    static float GetGlobalVolume();
    static void SetDirection(glm::vec3);
    static glm::vec3 GetDirection();
    static void SetUpVector(glm::vec3);
    static glm::vec3 GetUpVector();
};
}