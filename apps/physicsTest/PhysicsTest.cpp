#include <Bullet/btBulletCollisionCommon.h>
#include <Bullet/btBulletDynamicsCommon.h>

#include "RTE/EntryPoint.h"


void Initialize()
{
    btDynamicsWorld *world = new btDiscreteDynamicsWorld(nullptr, nullptr, nullptr, nullptr);
}