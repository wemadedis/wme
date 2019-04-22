#pragma once

#include "rte/RTEConfig.hpp"
#include "rte/RTECore.hpp"

#include <cstdint>
#include <iostream>
#include <string>

#define Debug(toPrint) RTE::Utilities::PrintDebug(toPrint, __FILE__, __LINE__)
#define Warning(toPrint) RTE::Utilities::PrintWarning(toPrint, __FILE__, __LINE__)
#define Error(toPrint) RTE::Utilities::PrintError(toPrint, __FILE__, __LINE__)

namespace RTE::Utilities
{
std::string GetFileFromAssets(std::string path);

void PrintDebug(std::string toPrint, const char *file, uint32_t line);
void PrintWarning(std::string toPrint, const char *file, uint32_t line);
void PrintError(std::string toPrint, const char *file, uint32_t line);
} // namespace RTE::Utilities