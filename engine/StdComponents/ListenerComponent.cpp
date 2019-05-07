#include "rte/ListenerComponent.hpp"

namespace RTE::StdComponents
{

ListenerComponent::ListenerComponent()
{}

ListenerComponent::~ListenerComponent()
{}

void ListenerComponent::Initialize(TransformComponent* transformComponent, bool enabled)
{
    _transformComponent = transformComponent;
    _enabled = enabled;
}

void ListenerComponent::Update(float deltaTime)
{
    if (_enabled)
    {
        using namespace RTE::Audio;
        Listener::SetPosition(_transformComponent->Transform.Pos);
        Listener::SetDirection(-_transformComponent->Transform.DirectionVector());
    }
}

void ListenerComponent::Enable()
{
    _enabled = true;
}

void ListenerComponent::Disable()
{
    _enabled = false;
}

bool ListenerComponent::GetState()
{
    return _enabled;
}

}