#include "rte/AudioListener.hpp"


namespace RTE::Audio
{

void AudioListener::SetPosition(glm::vec3 position)
{
    // _position = position;
    sf::Listener::setPosition(position.x, position.y, position.z);
}

glm::vec3 AudioListener::GetPosition()
{
    auto position = sf::Listener::getPosition();
    return glm::vec3{position.x, position.y, position.z};
}

void AudioListener::SetGlobalVolume(float volume)
{
    sf::Listener::setGlobalVolume(volume);
}

float AudioListener::GetGlobalVolume()
{
    return sf::Listener::getGlobalVolume();
}

void AudioListener::SetDirection(glm::vec3 direction)
{
    sf::Listener::setDirection(direction.x, direction.y, direction.z);
}

glm::vec3 AudioListener::GetDirection()
{
    auto direction = sf::Listener::getDirection();
    // _direction = glm::vec3{direction.x, direction.y, direction.z};
    return glm::vec3{direction.x, direction.y, direction.z};
}

void AudioListener::SetUpVector(glm::vec3 up)
{
    sf::Listener::setUpVector(up.x, up.y, up.z);
}

glm::vec3 AudioListener::GetUpVector()
{
    auto up = sf::Listener::getUpVector();
    return glm::vec3{up.x, up.y, up.z};
}

}