#include "rte/RTEConfig.hpp"

using namespace RTE;

RTEConfig::RTEConfig()
{
}

RTEConfig *RTEConfig::GetInstance()
{
    if(_instance == nullptr)
    {
        _instance = new RTEConfig();
    }
    return _instance;
}
RTEConfig *RTEConfig::_instance = nullptr;