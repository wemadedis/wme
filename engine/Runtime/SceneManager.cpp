#include "rte/SceneManager.hpp"
#include "rte/NotImplementedException.hpp"

namespace RTE::Runtime
{
SceneManager::SceneManager()
{
}

Scene *SceneManager::MakeScene()
{
    return new Scene();
}

Scene *SceneManager::GetActiveScene()
{
    return _activeScene;
}

void SceneManager::Update(float deltaTime)
{
    if (_activeScene != nullptr)
    {
        _activeScene->UpdateComponents(deltaTime);
    }
}

void SceneManager::SetActiveScene(Scene *scene)
{
    _activeScene = scene;
}
} // namespace RTE::Runtime