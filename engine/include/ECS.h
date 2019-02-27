typedef int Index;

struct Component
{};

struct MeshComponent : public Component
{
    Index MeshHandle;
    Index Transform;
};

struct InputComponent : public Component
{
    
};
