#include "rte/SceneManager.hpp"

namespace RTE
{
namespace Runtime
{
Scene *SceneManager::MakeScene()
{
    return new Scene();
}
} // namespace Runtime
} // namespace RTE