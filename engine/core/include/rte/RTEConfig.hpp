#pragma once

#include "rte/Colors.hpp"
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
     * @brief Color of a ray miss OR rasterized clear color
     * Alpha channel is ignored
     */
    Colors::Color BackgroundColor = Colors::Cyan;

    /** 
     * @brief Whether or not this application will try to use NVidia Raytracing.
     * @details
     * Default value of this variable is whether or not Raytracing is available.
     * Setting this to false when Raytracing is not available will result in an
     * immediate error.
     */
    bool UseRaytracing = false;
};

struct WindowConfig
{
    /// @brief Window width in pixels
    uint32_t WindowWidth = 1000;

    /// @brief Window height in pixels
    uint32_t WindowHeight = 1000;

    /// @brief Set the window to be fullscreen
    bool Fullscreen = false;

    /// @brief The Title of the window
    std::string WindowName = "RTEGame";
};

struct AssetConfig
{
    /**
     * @brief Path to the root of the asset directory
     * 
     */
    char *AssetRootDir = ENGINE_ASSET_DIR;
    /// @brief File paths to the textures
    std::vector<std::string> Textures;
    /// @brief File paths to the mesh geometry data
    std::vector<std::string> Meshes;
};

/**
 * @brief Describes RTE project settings
 */
struct RTEConfig
{
    GraphicsConfig GraphicsConfig;
    WindowConfig WindowConfig;
    AssetConfig AssetConfig;
};
} // namespace RTE