#pragma once

#include <cstdint>

#include <Bullet/btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

#include "rte/CollisionId.hpp"
#include "rte/RTEConfig.hpp"
#include "rte/RigidBody.hpp"

namespace RTE
{
/**
 * @brief This namespace holds classes, structs, and functions 
 * related to RTE's Physics module
 */
namespace Physics
{

/**
 * @brief Manages physics
 * 
 */
class PhysicsManager
{
  private:
    /**
     * @brief PhysicsWorld instance
     * 
     */
    btDiscreteDynamicsWorld *_physicsWorld;

    /**
     * @brief Create a Default Dynamics World object
     * @details
     *  A default dynamics world uses:
     *   - btDbvtBroadphase
     *   - btDefaultCollisionConfiguration
     *   - btCollisionDispatcher
     *   - btSequentialImpulseContraintSolver
     * @return btDiscreteDynamicsWorld* 
     */
    btDiscreteDynamicsWorld *CreateDefaultDynamicsWorld();

    /**
     * @brief Initializes Bullet global collision callbacks with
     *  RTE functions so they can be used.
     * 
     */
    void SetupBulletCallbacks();

    /**
     * @brief Init for 
     * 
     */
    const glm::vec3 _defaultGravity = {0, -9.8f, 0};

    /**
     * @brief The number of frames per second used for the physics system
     *  Changed by the default constructor, or via setter method #PhysicsManager(uint32_t)
     */
    uint32_t _framesPerSecond = -1;

    /**
     * @brief The fixed time step used for physics simulation
     * 
     */
    float _fixedTimeStep = -1;

    //! Control this value
    /**
     * @brief Maximum number of substeps
     * @details 
     * In the case that time since last frame is larger than #_fixedTimeStep 
     * then bullet will split it into multiple substeps. This value represents the
     * maximum number of sub steps it will be split into.
     */
    uint32_t _maxSubSteps = 1;

  public:
    /**
     * @brief Construct a new Physics Manager object
     * 
     */
    PhysicsManager(RTEConfig &config);

    /**
     * @brief Destroy the Physics Manager object
     * 
     */
    ~PhysicsManager();

    /**
     * @brief Step the physics world by /p deltaTime seconds
     * 
     * @param deltaTime Time to step in seconds
     */
    void Step(float deltaTime);

    /**
     * @brief Create a Rigid Body object and add it to the physics world
     * 
     * @return RigidBody* A pointer to the created physics rigid body
     */
    RigidBody *CreateRigidBody();

    glm::vec3 GetGravity();

    void SetGravity(float x, float y, float z);

    void SetGravity(glm::vec3 gravity);

    uint32_t GetFramesPerSecond();
    void SetFramesPerSecond(uint32_t framesPerSecond);
};
} // namespace Physics
} // namespace RTE
