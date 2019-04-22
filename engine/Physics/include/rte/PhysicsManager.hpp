#pragma once

#include <cstdint>

#include <Bullet/btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

#include "rte/CollisionId.hpp"
#include "rte/RTEConfig.hpp"
#include "rte/RTEModule.hpp"
#include "rte/RigidBody.hpp"

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
 * @return true Bullet documentation doesn't specify
 * @return false Bullet documentation doesn't specify
 */
static bool ContactDestroyedCallback(void *data);

/**
 * @brief Bullet physics callback for callback processed. 
 *  This is called whenever two rigidbodies are colliding.
 * 
 * @param manifoldPoint The point of collision
 * @param body0 The first body of the colliding rigidbodies
 * @param body1 The second body of the colliding rigidbodies
 * @return true Bullet documentation doesn't specify
 * @return false Bullet documentation doesn't specify
 */
static bool ContactProcessedCallback(btManifoldPoint &manifoldPoint, void *body0, void *body1);

/**
 * @brief Bullet physics callback for contact ended
 * 
 * @param manifold 
 */
static void ContactEndedCallback(btPersistentManifold *const &manifold);

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
     *  Changed by the default constructor, or via setter 
     * method RTE::Physics::PhysicsManager(RTEConfig &config)
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

    void Update(float deltaTime) override;

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
} // namespace RTE::Physics
