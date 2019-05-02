#pragma once
#include "RenderStructs.h"

namespace RTE::Rendering::Primitives
{
/**
 * @brief Creates a cylinder with a radius and height
 * 
 * @param radius The radius of the cylinder
 * @param height The height of the cylinder
 * @param face The amount of faces the cylinder will consist of
 * @return Returns the cylinder created from the parameters, as a Mesh.
 */
Mesh MakeCylinder(float radius, float height, int faces);

/**
 * @brief Creates XY-plane aligned, 1x1 quad.
 * @return Returns the quad as a Mesh.
 */
Mesh MakeQuad();
};