#pragma once

#include <cstdint>
#include <string>

/**
 * @brief Describes RTE project settings
 */
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

    bool Fullscreen = false;

    /// @brief The title of the app
    char *ApplicationName = "RTEGame";

    /// @brief The Title of the main window, if null, #ApplicationName is used.
    char *WindowName = nullptr;
};

struct RTEConfig
{
    GraphicsConfig GraphicsConfig;
    WindowConfig WindowConfig;
};
} // namespace RTE