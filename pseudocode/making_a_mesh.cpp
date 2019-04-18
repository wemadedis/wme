void Initialize()
{
    GameObject player = Scene.GetGameObject();
    auto mc = player.AddComponent<MeshComponent>();

    MeshHandle dragonHandle =  AssetManager::ImportModel("Path/To/My/File.ply");

    mc.Init(dragonHandle);
}