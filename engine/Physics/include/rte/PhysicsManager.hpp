#pragma once

#include <cstdint>
#include <vector>

#include <Bullet/btBulletDynamicsCommon.h>
#include <glm/glm.hpp>

#include "rte/Collision.hpp"
#include "rte/GenericPool.hpp"
#include "rte/PhysicsDebugDraw.h"
#include "rte/RTEConfig.hpp"
#include "rte/RTEModule.hpp"
#include "rte/RigidBody.hpp"

// Forward declaration
class RigidBody;

namespace RTE::Physics
{
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
 * @brief Converts a bullet vector3 to a glm vector3 
 * 
 * @param vec Vector to convert
 * @return glm::vec3 Converted vector
 */
static inline glm::vec3 Convert(btVector3 vec)
{
    return glm::vec3(vec.getX(), vec.getY(), vec.getZ());
}

/**
 * @brief Converts a bullet vector3 to a glm vector3 
 * 
 * @param vec Vector to convert
 * @return glm::vec3 Converted vector
 */
static inline btQuaternion Convert(glm::quat quat)
{
    return btQuaternion(quat.x, quat.y, quat.z, quat.w);
}

/**
 * @brief Converts a bullet vector3 to a glm vector3 
 * 
 * @param vec Vector to convert
 * @return glm::vec3 Converted vector
 */
static inline glm::quat Convert(btQuaternion quat)
{
    return glm::quat(
        static_cast<float>(quat.getW()),
        static_cast<float>(quat.getX()),
        static_cast<float>(quat.getY()),
        static_cast<float>(quat.getZ()));
}

/**
 * @brief Converts a glm vector to a bullet vector
 * 
 * @param vec Vector to convert
 * @return btVector3 Converted vector
 */
static inline btVector3 Convert(glm::vec3 vec)
{
    return btVector3(vec.x, vec.y, vec.z);
}

/**
 * @brief Converts a bullet transform into an RTE transform
 * 
 * @param t Transform to convert
 * @return RTE::Rendering::Transform Converted transform
 */
static inline Rendering::Transform Convert(btTransform t)
{
    Rendering::Transform trans;
    trans.Pos = Convert(t.getOrigin());
    trans.Rot = glm::eulerAngles(Convert(t.getRotation()));
    trans.Scale = glm::vec3(1, 1, 1);
    return trans;
}

/**
 * @brief Converts an RTE transform transform into a bullet transform
 * 
 * @param t Transform to convert
 * @return btTransform Converted transform
 */
static inline btTransform Convert(Rendering::Transform t)
{
    btTransform trans;
    trans.setOrigin(Convert(t.Pos));

    trans.setRotation(Convert(t.RotationMatrix()));
    return trans;
}

/**
 * @brief The different types of colliders
 * 
 */
enum class ColliderType
{
    BOX = 0,
    CYLLINDER,
    CAPSULE,
    PLANE,
    SPHERE
};

/**
 * @brief The data required to initialize a collider type
 * 
 */
union ColliderData {
    struct
    {
        glm::vec3 HalfExtents;
    } Box, Cyllinder;

    struct
    {
        float Radius;
    } Sphere;

    struct
    {
        glm::vec3 N;
        float D;
    } Plane;
    struct
    {
        float Radius;
        float Height;
    } Capsule;
};

/**
 * @brief Combines local collider offset with collider type and initialization data.
 * 
 */
struct Collider
{
    Rendering::Transform ColliderTransform;
    ColliderType Type;
    ColliderData Data;
};

/**
 * @brief Manages physics the physics of RTE
 * 
 * Does physics stepping every update.
 * 
 */
class PhysicsManager : public RTEModule
{
private:
    static PhysicsManager *_instance;
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

    GenericPool<Collision> *_collisionPool;

    /**
     * @brief Initializes Bullet global collision callbacks with
     *  RTE functions so they can be used.
     * 
     */
    void SetupBulletCallbacks();

    /**
     * @brief Maximum number of substeps
     * @details 
     * In the case that time since last frame is larger than #_fixedTimeStep 
     * then bullet will split it into multiple substeps. This value represents the
     * maximum number of sub steps it will be split into.
     */
    uint32_t _maxSubSteps = 10;

    /**
     * @brief Draws the collider boundaries
     * 
     */
    PhysicsDebugDraw *_debugDraw = nullptr;

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
     * 
     * @param deltaTime Time to step in seconds
     */
    void Step(float deltaTime);

    /**
     * @brief Updates the physics world
     * @param deltaTime Time since last frame
     */
    void Update(float deltaTime) override;

    /**
     * @brief Takes the information from the collider type and data and use it 
     * to initialize the corresponding btCollisionShape
     * 
     * @param type Type of collider to make
     * @param data Data used to make that collider
     * @return btCollisionShape* The created collider
     */
    btCollisionShape *GetCollisionShapeFromColliderType(
        ColliderType type,
        ColliderData data);

    /**
     * @brief Create a Rigid Body object and add it to the physics world
     * 
     * 
     * @param trans Initial transform of the produced RigidBody
     * @param mass The mass of this rigidbody
     * @return RigidBody* A pointer to the created physics rigid body
     */
    RigidBody *CreateRigidBody(
        Rendering::Transform &trans,
        float mass,
        std::vector<Collider> colliders,
        void *rigidBodyOwner);

    /**
     * @brief Removes a rigidbody from the physics world
     * 
     * @param rb rigidbody to remove
     */
    void RemoveRigidBody(RigidBody *rb);
    /**
     * @brief Get the Gravity of the physics world
     * 
     */
    glm::vec3 GetGravity();
    /**
     * @brief Set the world gravity using individual x, y, z components.
     * @return glm::vec3 Gravity
     * 
     * @param x Gravity X
     * @param y Gravity Y
     * @param z Gravity Z
     */
    void SetGravity(float x, float y, float z);

    /**
     * @brief Set the world gravity using a glm vector 
     * 
     * @param gravity The new gravity 

     */
    void SetGravity(glm::vec3 gravity);

    /**
     * @brief Get a reference to the instance
     * 
     * @return PhysicsManager* Pointer 
     */
    static PhysicsManager *GetInstance();

    /**
     * @brief Gets a free collision slot from the collision pool 
     * 
     * @return Collision* Available collision
     */
    static Collision *PhysicsManager::GetCollisionSlot();

    /**
     * @brief Frees the collision slot taken by passed collision
     * 
     * @param col Collision to free
     */
    static void PhysicsManager::FreeCollisionSlot(Collision *col);

    /**
     * @brief Get the Physics Debug Drawer
     * 
     * @return PhysicsDebugDraw* The drawer
     */
    PhysicsDebugDraw *GetPhysicsDebugDraw();
};
} // namespace RTE::Physics
