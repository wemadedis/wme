
#include <queue>
#pragma once
#include <vector>

#include "rte/Collision.hpp"
#include "rte/Component.h"
#include "rte/PhysicsManager.hpp"
#include "rte/RigidBody.hpp"
#include "rte/TransformComponent.hpp"

namespace RTE::StdComponents
{

/**
 * @brief Standard component which contains a RigidBody and takes care
 *  of interaction with the physics world.
 */
class PhysicsComponent : public Runtime::Component
{
private:
    /**
     * @brief PhysicsComponent has a reference to a TransformComponent in 
     * order to sync positions.
     * 
     */
    TransformComponent *_transformComponent;

    /**
     * @brief A reference to the RigidBody contained by this component.
     * 
     */
    Physics::RigidBody *_rigidBody;

public:
    /**
     * @brief If this is false, this component wont receive collision callbacks.
     * It can still interact with other components in the physics system.
     * 
     */
    bool ReceiveCollisionCallbacks = false;
    // queue of collision starts
    std::queue<Physics::OnCollisionData> *Collisions;
    // queue of collision ends
    std::queue<Physics::EndCollisionData> *EndCollisions;

    /**
     * @brief Initializes this physics component 
     * 
     * @param transformComp real-world position of the object. This will be 
     *  synced with the physics-world 
     * @param mass Mass of this object.
     * @param colliders The colliders for this object
     * @param receiveCallbacks Whether or not this component should receive callbacks.
     *          Do not pass true to this unless the component using this empties out the
     *          collision queues.
     */
    void Initialize(
        TransformComponent *transformComp,
        float mass,
        std::vector<Physics::Collider> colliders,
        bool receiveCallbacks = false);

    /**
     * @brief Updates this component's transformation related to the physics
     * world 
     * 
     * @param deltaTime Time since last frame 
    */
    void Update(float deltaTime) override;

    /**
     *
     * @brief Get the Rigid Body object for this component
     * 
     * @return Physics::RigidBody* Rigidbody to retuurn
     */
    Physics::RigidBody *GetRigidBody();

    /**
     * @brief Updates the given transform based on the changes in the 
     * physics world
     * 
     * @param transform World to update. 
     */
    void UpdateFromPhysicsWorld(Rendering::Transform &transform);
};
} // namespace RTE::StdComponents
