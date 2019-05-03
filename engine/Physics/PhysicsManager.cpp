#include "rte/PhysicsManager.hpp"
#include "rte/Collision.hpp"
#include "rte/GlmWrapper.hpp"
#include "rte/NotImplementedException.hpp"
#include "rte/PhysicsComponent.hpp"
#include "rte/RenderStructs.h"
#include "rte/Utility.hpp"

namespace RTE::Physics
{

static bool ContactProcessedCallback(
    btManifoldPoint &cp,
    void *bodyA,
    void *bodyB)
{
    auto GetPhysicsComponent = [=](void *src) {
        auto rb = static_cast<btRigidBody *>(src);
        return static_cast<StdComponents::PhysicsComponent *>(rb->getUserPointer());
    };

    auto compA = GetPhysicsComponent(bodyA);
    auto compB = GetPhysicsComponent(bodyB);

    Collision *col = (Collision *)cp.m_userPersistentData;
    bool collisionBegin = (col == nullptr);

    // New collision
    if (collisionBegin)
    {
        static CollisionId id = 0;
        id++;
        col = PhysicsManager::GetCollisionSlot();
        col->Id = id;
        col->BodyA = compA;
        col->BodyB = compB;
        cp.m_userPersistentData = col;
    }

    OnCollisionData colDataA;
    colDataA.Id = col->Id;
    colDataA.GoId = compB->GetGameObjectId();
    colDataA.Point = Convert(cp.getPositionWorldOnA());
    colDataA.NewCollision = collisionBegin;
    compA->Collisions->push(colDataA);

    OnCollisionData colDataB;
    colDataB.Id = col->Id;
    colDataB.GoId = compA->GetGameObjectId();
    colDataB.Point = Convert(cp.getPositionWorldOnB());
    colDataB.NewCollision = collisionBegin;
    compB->Collisions->push(colDataB);

    return true;
}

static bool ContactDestroyedCallback(void *data)
{
    Collision *col = (Collision *)data;

    EndCollisionData colDataA{col->Id};
    EndCollisionData colDataB{col->Id};

    col->BodyA->EndCollisions->push(colDataA);
    col->BodyB->EndCollisions->push(colDataB);

    PhysicsManager::FreeCollisionSlot(col);
    return true;
}

void PhysicsManager::Update(float deltaTime)
{
    Step(deltaTime);
}

btDiscreteDynamicsWorld *PhysicsManager::CreateDefaultDynamicsWorld()
{
    SetupBulletCallbacks();

    //* Extend: Make this extendable
    // todo: (danh) Mon 29/04 - 22:26: Allocator? Struct?
    auto Broadphase = new btDbvtBroadphase();
    auto CollisionConfiguration = new btDefaultCollisionConfiguration();
    auto CollisionDispatcher = new btCollisionDispatcher(CollisionConfiguration);
    auto ConstraintSolver = new btSequentialImpulseConstraintSolver();

    auto world = new btDiscreteDynamicsWorld(
        CollisionDispatcher,
        Broadphase,
        ConstraintSolver,
        CollisionConfiguration);
    return world;
}

void PhysicsManager::SetupBulletCallbacks()
{
    gContactProcessedCallback = ContactProcessedCallback;
    gContactDestroyedCallback = ContactDestroyedCallback;
}

PhysicsManager::PhysicsManager(RTE::RTEConfig &config)
{
    _instance = this;
    SetFramesPerSecond(60);
    _physicsWorld = CreateDefaultDynamicsWorld();
    _collisionPool = new GenericPool<Collision>(config.PhysicsConfig.MaxCollisionCount);
    SetGravity(_defaultGravity);
}

PhysicsManager::
    ~PhysicsManager()
{
    delete _physicsWorld;
}

void PhysicsManager::Step(float deltaTime)
{
    _physicsWorld->stepSimulation(deltaTime, _maxSubSteps, _fixedTimeStep);
}

PhysicsManager *PhysicsManager::_instance = nullptr;

btCollisionShape *PhysicsManager::GetCollisionShapeFromColliderType(
    ColliderType type,
    ColliderData data)
{
    switch (type)
    {
    case ColliderType::BOX:
        return new btBoxShape(Convert(data.Box.HalfExtents));
    case ColliderType::CAPSULE:
        return new btCapsuleShape(data.Capsule.Radius, data.Capsule.Height);
    case ColliderType::CYLLINDER:
        return new btCylinderShape(Convert(data.Cyllinder.HalfExtents));
    case ColliderType::SPHERE:
        return new btSphereShape(data.Sphere.Radius);
    case ColliderType::PLANE:
        return new btStaticPlaneShape(Convert(data.Plane.N), data.Plane.D);
    default:
        throw RTEException("Unsupported Collider Type");
    }
}

RigidBody *PhysicsManager::CreateRigidBody(
    Rendering::Transform &trans,
    float mass,
    std::vector<Collider> colliders,
    void *rigidBodyOwner)
{
    glm::quat rotQ = glm::quat_cast(trans.RotationMatrix());
    btMotionState *motionState =
        new btDefaultMotionState(
            btTransform(
                btQuaternion(rotQ.x, rotQ.y, rotQ.z, rotQ.w),
                btVector3(trans.Pos.x, trans.Pos.y, trans.Pos.z)));
    btVector3 fallInertia(0, 0, 0);

    btBoxShape *boxShape = new btBoxShape({10, 10, 10});

    btRigidBody::btRigidBodyConstructionInfo info =
        btRigidBody::btRigidBodyConstructionInfo(1000, motionState, boxShape);
    btRigidBody *bulletRigidBody = new btRigidBody(info);
    boxShape->calculateLocalInertia(1000, fallInertia);

    bulletRigidBody->setUserPointer(rigidBodyOwner);
    _physicsWorld->addRigidBody(bulletRigidBody);

    return new RigidBody(bulletRigidBody);
}

glm::vec3 PhysicsManager::GetGravity()
{
    return Convert(_physicsWorld->getGravity());
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

Collision *PhysicsManager::GetCollisionSlot()
{
    return _instance->_collisionPool->CreateObject();
}

void PhysicsManager::FreeCollisionSlot(Collision *col)
{
    _instance->_collisionPool->FreeObject(col);
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
