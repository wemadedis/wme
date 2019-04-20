#pragma once

#include <Bullet/btBulletDynamicsCommon.h>

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
