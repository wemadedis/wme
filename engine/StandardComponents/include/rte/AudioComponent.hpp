#include "rte/Component.h"

namespace RTE::StandardComponents
{

class AudioComponent : public Component
{
public:
    AudioComponent();
    virtual void Update() override;

};

// End RTE::StandardComponents namespace
};