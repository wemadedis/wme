#include "rte/Component.h"

bool Component::GetEnabled()
{
    return _enabled;
}

void Component::SetEnabled(bool enabled)
{
    _enabled = enabled;
}