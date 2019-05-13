#pragma once

#include "rte/Colors.hpp"
#include "rte/Definitions.hpp"
#include "rte/GlmWrapper.hpp"
#include "rte/RTEProjectProperties.hpp"

#include <cstdint>
#include <string>
#include <vector>

namespace RTE
{
struct GraphicsConfig
{
    /// @brief The maximum framerate for the engine
    uint32_t FramesPerSecond = 240;

    /** 
     * @brief Whether or not this application will try to use NVidia Raytracing.
     * @details
     * Default value of this variable is whether or not Raytracing is available.
     * Setting this to false when Raytracing is not available will result in an
     * error.
     */
    bool UseRaytracing = false;
    bool RaytracingAvailable = false;
};

struct WindowConfig
{
    /// @brief Window width in pixels
    uint32_t WindowWidth = 1000;

    /// @brief Window height in pixels
    uint32_t WindowHeight = 1000;

    /// @brief The Title of the window
    std::string WindowName = "RTEGame";
};

struct PhysicsConfig
{
    uint32_t MaxCollisionCount = 1000;
    glm::vec3 Gravity = {0, -9.8f, 0};
    bool DebugDrawColliders = true;
};

struct AssetConfig
{
    /**
     * @brief Path to the root of the asset directory
     * 
     */
    AbsoluteFilePath AssetRootDir = ENGINE_ASSET_DIR;
    /// @brief File paths to the textures
    std::vector<AbsoluteFilePath> Textures;
    /// @brief File paths to the mesh geometry data
    std::vector<AbsoluteFilePath> Meshes;
};

/**
 * @brief Describes RTE project settings
 */
class RTEConfig
{
public:
    GraphicsConfig GraphicsConfig;
    WindowConfig WindowConfig;
    AssetConfig AssetConfig;
    PhysicsConfig PhysicsConfig;
};
} // namespace RTE