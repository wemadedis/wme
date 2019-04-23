#pragma once

#include "rte/Component.h"
#include "rte/PhysicsManager.hpp"
#include "rte/RigidBody.hpp"
#include "rte/TransformComponent.hpp"

namespace RTE::StandardComponents

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
     * @brief Initializes this component using a TransformComponent* that will 
     * be synced
     * 
     * @param transformComp Transform to sync physics world with
     */
    void Initialize(TransformComponent *transformComp);

    /**
     * @brief Updates this component's transformation related to the physics
     * world 
     * 
     * @param deltaTime Time since last frame 
     */
    void Update(float deltaTime) override;

    /**
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
} // namespace RTE::StandardComponents
