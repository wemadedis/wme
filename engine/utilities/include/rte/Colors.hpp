#pragma once

#include "rte/GlmWrapper.hpp"

namespace RTE
{
/**
 * @brief Hosts color shortcuts.
 * 
 * Good resource for color values
 * https://en.wikipedia.org/wiki/List_of_colors:_A%E2%80%93F
 */
namespace Colors
{
typedef glm::vec4 Color;
static Color Black = glm::vec4(0, 0, 0, 1);
static Color White = glm::vec4(0, 0, 0, 1);
static Color Red = glm::vec4(1, 0, 0, 1);
static Color Green = glm::vec4(0, 1, 0, 1);
static Color Blue = glm::vec4(0, 0, 1, 1);
static Color Yellow = glm::vec4(1, 1, 0, 1);
static Color Magenta = glm::vec4(1, 0, 1, 1);
static Color Cyan = glm::vec4(0, 1, 1, 1);
static Color LightGray = glm::vec4(0.8, 0.8, 0.8, 1);
static Color DarkGray = glm::vec4(0.3, 0.3, 0.3, 1);
} // namespace Colors

} // namespace RTE