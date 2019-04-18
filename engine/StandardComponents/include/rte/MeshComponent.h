#include "rte/Component.h"
#include "rte/RenderStructs.h"
#include "rte/RendererHandles.h"

namespace RTE::StandardComponents
{
    using namespace RTE::Rendering;
class MeshComponent : public Component
{
    MeshInstanceHandle mesh;


public:
    MeshComponent();
    void Initialize(MeshHandle mesh);
    void SetTexture(TextureHandle texture);
    virtual void Update() override;
};


};