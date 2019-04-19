#include <Bullet/btBulletDynamicsCommon.h>
#include <imgui/imgui.h>
#include "RTE/EntryPoint.h"

void Initialize()
{
    ImGui::Begin("EGGS!");
    btDbvtBroadphase *world = new btDbvtBroadphase();
}