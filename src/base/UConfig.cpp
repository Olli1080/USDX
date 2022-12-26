#include "UConfig.h"

namespace UConfig 
{
std::string UConfig::USDXShortVersionStr()
{
    return USDX_STRING + USDXStateVersionStr();
}

std::string UConfig::USDXVersionStr()
{
    return USDX_STRING + " V " +
        std::to_string(USDX_VERSION_MAJOR) + "." +
        std::to_string(USDX_VERSION_MINOR) + "." +
        std::to_string(USDX_VERSION_RELEASE) +
        USDXStateVersionStr() + " Build";
}

std::string UConfig::USDXStateVersionStr()
{
    return USDX_VERSION_STATE != "" ? " " + USDX_VERSION_STATE : "";
}
}