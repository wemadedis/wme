#include "rte/PhysicsManager.hpp"

#include <glm/glm.hpp>

#include "rte/NotImplementedException.hpp"

namespace RTE::Physics
{

static void ContactStartedCallback(btPersistentManifold *const &manifold)
{
    throw NotImplementedException();
}

static bool ContactProcessedCallback(btManifoldPoint &cp, void *body0, void *body1)
{
    throw NotImplementedException();
}

static bool ContactDestroyedCallback(void *data)
{
    throw NotImplementedException();
}

static void ContactEndedCallback(btPersistentManifold *const &manifold)
{
    throw NotImplementedException();
}

void PhysicsManager::Update(float deltaTime)
{
    Step(deltaTime);
}

PhysicsManager::PhysicsManager(RTE::RTEConfig &config)
{
    _instance = this;
    SetFramesPerSecond(config.GraphicsConfig.FramesPerSecond);
    _physicsWorld = CreateDefaultDynamicsWorld();
    SetGravity(_defaultGravity);
}

static btDbvtBroadphase *Broadphase;
static btDefaultCollisionConfiguration *CollisionConfiguration;
static btCollisionDispatcher *CollisionDispatcher;
static btSequentialImpulseConstraintSolver *ConstraintSolver;

btDiscreteDynamicsWorld *PhysicsManager::CreateDefaultDynamicsWorld()
{
    SetupBulletCallbacks();

    //! Make this configurable
    //? Make default world configurable too?
    Broadphase = new btDbvtBroadphase();
    CollisionConfiguration = new btDefaultCollisionConfiguration();
    CollisionDispatcher = new btCollisionDispatcher(CollisionConfiguration);
    ConstraintSolver = new btSequentialImpulseConstraintSolver();
    auto world = new btDiscreteDynamicsWorld(
        CollisionDispatcher,
        Broadphase,
        ConstraintSolver,
        CollisionConfiguration);
    return world;
}

void PhysicsManager::SetupBulletCallbacks()
{
    gContactStartedCallback = ContactStartedCallback;
    gContactProcessedCallback = ContactProcessedCallback;
    gContactEndedCallback = ContactEndedCallback;
    gContactDestroyedCallback = ContactDestroyedCallback;
}

PhysicsManager::~PhysicsManager()
{
    delete _physicsWorld;
}

void PhysicsManager::Step(float deltaTime)
{
    _physicsWorld->stepSimulation(deltaTime, _maxSubSteps, _fixedTimeStep);
}

PhysicsManager *PhysicsManager::_instance = nullptr;

RigidBody CreateRigidBody()
{
    btMotionState *motionState = new btDefaultMotionState();
    btBoxShape *boxShape = new btBoxShape({10, 10, 10});
    btRigidBody::btRigidBodyConstructionInfo info = btRigidBody::btRigidBodyConstructionInfo(0.0f, motionState, boxShape);
    btRigidBody *bulletRigidBody = new btRigidBody(info);
    return RigidBody(bulletRigidBody);
}

glm::vec3 PhysicsManager::GetGravity()
{
    const btVector3 gravity = _physicsWorld->getGravity();
    return glm::vec3(gravity.getX(), gravity.getY(), gravity.getZ());
}

void PhysicsManager::SetGravity(float x, float y, float z)
{
    _physicsWorld->setGravity({x, y, z});
}

void PhysicsManager::SetGravity(glm::vec3 gravity)
{
    SetGravity(gravity.x, gravity.y, gravity.z);
}

uint32_t PhysicsManager::GetFramesPerSecond()
{
    return _framesPerSecond;
}

void PhysicsManager::SetFramesPerSecond(uint32_t framesPerSecond)
{
    _framesPerSecond = framesPerSecond;
    _fixedTimeStep = 1 / (float)framesPerSecond;
}

PhysicsManager *PhysicsManager::GetInstance()
{
    return _instance;
}

}; // namespace RTE::Physics
