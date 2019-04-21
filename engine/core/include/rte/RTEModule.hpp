#pragma once

namespace RTE
{
class RTEModule
{
public:
    virtual void Update(float deltaTime) = 0;
};
} // namespace RTE
