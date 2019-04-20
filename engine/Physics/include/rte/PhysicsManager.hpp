#pragma once

#include <cstdint>

#include <Bullet/btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

#include "rte/CollisionId.hpp"
#include "rte/RTEModule.hpp"
#include "rte/RigidBody.hpp"

/**
 * @brief This namespace holds classes, structs, and functions related to RTE's Physics module
 * 
 */
namespace RTE::Physics
{

/**
 * @brief Bullet Physics callback for contact started
 * 
 * @param manifold The persistent manifold for this contact
 */
static void ContactStartedCallback(btPersistentManifold *const &manifold);

/**
 * @brief Bullet Physics callback for contact destroyed
 * 
 * @param data Whatever data passed to it
 * @return true Dunno their documentation doesn't specify
 * @return false Dunno their documentation doesn't specify
 */
static bool ContactDestroyedCallback(void *data);

/**
 * @brief Bullet Physics callback for contact destroyed
 * 
 * @param data ManifoldPoint 
 * @return true Dunno their documentation doesn't specify
 * @return false Dunno their documentation doesn't specify
 */
static bool ContactProcessedCallback(btManifoldPoint &cp, void *body0, void *body1);

/**
 * @brief Bullet physics callback for contact ended
 * 
 * @param manifold 
 */
static void ContactEndedCallback(btPersistentManifold *const &manifold);

/**
 * @brief Manages physics
 * 
 */
class PhysicsManager : public RTEModule
{
private:
    /**
     * @brief PhysicsWorld instance
     * 
     */
    btDiscreteDynamicsWorld *_physicsWorld;

    /**
     * @brief Create a Default Dynamics World object
     * @detail
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
    uint32_t _framesPerSecond = 60;

    /**
     * @brief The fixed time step used for physics simulation
     * 
     */
    float _fixedTimeStep = 1 / (float)_framesPerSecond;
    //! Control this value
    uint32_t _maxSubSteps = 10;

public:
    /**
     * @brief Construct a new Physics Manager object
     * 
     */
    PhysicsManager(uint32_t framesPerSecond);

    /**
     * @brief Destroy the Physics Manager object
     * 
     */
    ~PhysicsManager();

    /**
     * @brief Step the physics world by @param deltaTime seconds
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
}; // namespace RTE::Physics
