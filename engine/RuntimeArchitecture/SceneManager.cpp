#include "rte/SceneManager.hpp"
#include "rte/NotImplementedException.hpp"

namespace RTE
{
namespace Runtime
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
    //throw NotImplementedException();
}

void SceneManager::SetActiveScene(Scene *scene)
{
    _activeScene = scene;
}
} // namespace Runtime
} // namespace RTE