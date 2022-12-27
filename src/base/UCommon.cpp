#include "UCommon.h"

#include <sstream>
#include <format>

namespace UCommon
{
    int HexToInt(std::string Hex)
    {
        std::stringstream ststr;
        uint8_t out;

        ststr << std::hex << Hex;
        ststr >> out;
        return out;
    }

    TRGB HexToRGB(std::string Hex)
    {
        TRGB Col;
        Col.R = HexToInt(Hex.substr(0, 2));
        Col.G = HexToInt(Hex.substr(2, 2));
        Col.B = HexToInt(Hex.substr(4, 2));

        return Col;
    }

    std::string RGBToHex(int R, int G, int B)
    {
        return std::format("{0:x}{1:x}{2:x}", R, G, B);
    }
}