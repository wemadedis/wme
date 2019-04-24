#pragma once

#include "rte/RTEConfig.hpp"
#include "rte/RTECore.hpp"

#include <cstdint>
#include <iostream>
#include <string>

/**
 * @brief Prints a debug message in the format: Debug:([HH:MM:SS])[FILE]: [LINE]: [toPrint]
 * Example: Debug(12:05:51)..\engine\core\RTECore.cpp: 59: Hello there 
 * 
 */
#define Debug(toPrint) RTE::Utilities::PrintDebug(toPrint, __FILE__, __LINE__)

/**
 * @brief Prints a debug message in the format: Warning:([HH:MM:SS])[FILE]: [LINE]: [toPrint]
 * Example: Warning(12:05:51)..\engine\core\RTECore.cpp: 59: Hello there 
 * 
 */
#define Warning(toPrint) RTE::Utilities::PrintWarning(toPrint, __FILE__, __LINE__)

/**
 * @brief Prints a debug message in the format: Error:([HH:MM:SS])[FILE]: [LINE]: [toPrint]
 * Example: Error(12:05:51)..\engine\core\RTECore.cpp: 59: Hello there 
 * 
 */
#define Error(toPrint) RTE::Utilities::PrintError(toPrint, __FILE__, __LINE__)


namespace RTE::Utilities
{
/**
 * @brief Get the absolute path to File From Assets folder
 * 
 * @param path The relative path to requested asset
 * @return std::string The absolute file path the requested asset
 */
std::string GetFileFromAssets(std::string path);

/**
 * @brief Prints toPrint to output with a timestamp, filename, and line number
 * @param toPrint Message
 * @param file File to print
 * @param line Line to print
 */
void PrintDebug(std::string toPrint, const char *file, uint32_t line);

/**
 * @brief Prints toPrint to output with a timestamp, filename, and line number
 * @param toPrint Message
 * @param file File to print
 * @param line Line to print
 */
void PrintWarning(std::string toPrint, const char *file, uint32_t line);

/**
 * @brief Prints toPrint to output with a timestamp, filename, and line number
 * @param toPrint Message
 * @param file File to print
 * @param line Line to print
 */
void PrintError(std::string toPrint, const char *file, uint32_t line);

} // namespace RTE::Utilities