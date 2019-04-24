#include "rte/Listener.hpp"


namespace RTE::Audio
{

void Listener::SetPosition(glm::vec3 position)
{
    sf::Listener::setPosition(position.x, position.y, position.z);
}

glm::vec3 Listener::GetPosition()
{
    auto position = sf::Listener::getPosition();
    return glm::vec3{position.x, position.y, position.z};
}

void Listener::SetGlobalVolume(float volume)
{
    sf::Listener::setGlobalVolume(volume);
}

float Listener::GetGlobalVolume()
{
    return sf::Listener::getGlobalVolume();
}

void Listener::SetDirection(glm::vec3 direction)
{
    sf::Listener::setDirection(direction.x, direction.y, direction.z);
}

glm::vec3 Listener::GetDirection()
{
    auto direction = sf::Listener::getDirection();
    return glm::vec3{direction.x, direction.y, direction.z};
}

void Listener::SetUpVector(glm::vec3 up)
{
    sf::Listener::setUpVector(up.x, up.y, up.z);
}

glm::vec3 Listener::GetUpVector()
{
    auto up = sf::Listener::getUpVector();
    return glm::vec3{up.x, up.y, up.z};
}

}