#include "rte/Utility.hpp"

#include "rte/RTEConfig.hpp"
#include "rte/RTECore.hpp"

#include <chrono>
#include <cstdarg>
#include <ctime>

namespace RTE::Utilities
{
AbsoluteFilePath GetFileFromAssets(RelativeFilePath path)
{
    std::ostringstream ss;
    AbsoluteFilePath folder = RTECore::Config.AssetConfig.AssetRootDir;
    ss << folder << path;
    return ss.str();
}

static const std::string DEBUG_STRING = "Debug";
static const std::string WARNING_STRING = "Warning";
static const std::string ERROR_STRING = "Error";
static const std::string EXCEPTION_STRING = "Exception";

std::string MinTwoWide(uint32_t n)
{
    std::ostringstream ss;
    if (n < 10)
    {
        ss << "0" << n;
    }
    else
    {
        ss << n;
    }
    return ss.str();
}

std::string GetTimestamp()
{
    using namespace std::chrono;
    system_clock::time_point now = system_clock::now();
    time_t now_c = system_clock::to_time_t(now - hours(24));
    tm local_tm = *localtime(&now_c);
    std::ostringstream ss;
    ss << MinTwoWide(local_tm.tm_hour) << ":"
       << MinTwoWide(local_tm.tm_min) << ":"
       << MinTwoWide(local_tm.tm_sec);
    return ss.str();
}

void PrintWithTimestamp(std::string warningType,
                        std::string toPrint)
{
    std::string timestamp = GetTimestamp();
    std::cout << warningType
              << "(" << timestamp << ") "
              << toPrint << std::endl;
}

// PrintDebug("This is good!");
// Debug(14:10:20): RTEThis is good!
void PrintWithTimestampAndFile(std::string warningType,
                               std::string toPrint,
                               const char *file, uint32_t line)
{
    std::string timestamp = GetTimestamp();
    std::cout << warningType
              << "(" << timestamp << ") "
              << file << ": " << line << ": "
              << toPrint << std::endl;
}

void PrintDebug(std::string toPrint, const char *file, uint32_t line)
{
    PrintWithTimestampAndFile(DEBUG_STRING, toPrint, file, line);
}

void PrintWarning(std::string toPrint, const char *file, uint32_t line)
{
    PrintWithTimestampAndFile(WARNING_STRING, toPrint, file, line);
}

void PrintError(std::string toPrint, const char *file, uint32_t line)
{
    PrintWithTimestampAndFile(ERROR_STRING, toPrint, file, line);
}

void PrintException(std::string toPrint)
{
    PrintWithTimestamp(EXCEPTION_STRING, toPrint);
}
} // namespace RTE::Utilities
