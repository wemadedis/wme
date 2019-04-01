#include "rte/Component.h"
#include "rte/RenderStructs.h"

namespace RTE::StandardComponents
{

class MeshComponent : public Component
{

public:
    MeshComponent();
    void Initialize(RTE::Rendering::Mesh *mesh);
    void SetTexture(RTE::Rendering::Texture *texture);
};


};