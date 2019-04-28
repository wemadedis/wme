#include "rte/Component.h"

namespace RTE::Runtime
{

bool Component::GetEnabled()
{
    return _enabled;
}

void Component::SetEnabled(bool enabled)
{
    _enabled = enabled;
}

} // namespace RTE::Runtime