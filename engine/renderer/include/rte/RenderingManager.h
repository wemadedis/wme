#pragma once
#include "rte/Renderer.h"


namespace RTE::Rendering
{
class RenderingManager
{
private:
    static RenderingManager *_instance;
public:
    RenderingManager();
    ~RenderingManager();
};

};

