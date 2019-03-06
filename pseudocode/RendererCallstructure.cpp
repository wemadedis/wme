Renderer* renderer;

void Update()
{
    // ...After scene update
    renderer->Render();
}

[Player, Mesh, Input, Transform]
void HandlePlayer()
{
    if(MovementInput)
    {
        moveLeft;
        sceneManager->MarkDirty(mesh);
    }
}

void Init(MeshPool* data)
{
    data.dirty()
}

void Render()
{
    data.dirty
}

void SwapRenderer(Renderer* newRenderer)
{
    newRenderer->Init(dataPointer);
    renderer = newRenderer;
}
