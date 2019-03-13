#pragma once

class Component
{
public:
    Component() = default;
    
    virtual void Update() = 0;
};