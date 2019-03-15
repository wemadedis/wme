void BeforeEverything()
{
    mesh_data meshData = ImportMesh("MyMesh.obj");
    texture_data tData = ImportTexture("MyTexture.png");
}

void main
{
    mesh *m = go->AddComponent<MeshComponent>();
    m->material.color = Color::RED;
    
    m->SetMesh(meshData);
    m->SetTexture(tData);
}
