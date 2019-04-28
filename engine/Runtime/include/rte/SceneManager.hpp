#pragma once

#include "rte/RTEModule.hpp"
#include "rte/Scene.h"

namespace RTE::Runtime
{
/**
 * @brief Manages Scenes. You're welcome
 * 
 */
class SceneManager : public RTEModule
{
private:
    /**
     * @brief The currently active scene, only the active scene
     * is updated
     * 
     */
    Scene *_activeScene = nullptr;

public:
    /**
     * @brief Construct a new Scene Manager object
     * 
     */
    SceneManager();

    /**
     * @brief Update the currently active scene
     * 
     * @param deltaTime Time since last frame
     */
    void Update(float deltaTime) override;

    /**
     * @brief Create a new scene
     * 
     * @return Scene* 
     */
    Scene *MakeScene();

    /**
     * @brief Get the currently active Scene
     * 
     * @return Scene* The active scene
     */
    Scene *GetActiveScene();

    /**
     * @brief Set the currently active Scene
     * 
     * @param scene The param to become active
     */
    void SetActiveScene(Scene *scene);
};
} // namespace RTE::Runtime
