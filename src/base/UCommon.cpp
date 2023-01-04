#include "UCommon.h"

#include <sstream>
#include <format>

namespace UCommon
{
    uint8_t HexToInt(std::string Hex)
    {
        std::stringstream ststr;
        uint8_t out;

        ststr << std::hex << Hex;
        ststr >> out;
        return out;
    }

    template<typename T>
    TRGB<T> HexToRGB(std::string Hex)
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

    std::string BuildResolutionString(int x, int y)
    {
        return std::format("{}x{}", x, y);
    }

    bool ParseResolutionString(const std::string ResolutionString, int& x, int& y)
    {
        /*var
            Pieces: TStringDynArray;*/
            
        auto Pieces = SplitString(LowerCase(ResolutionString), 1, ["x"]);
        bool Result = false;

        if ((Length(Pieces) > 1) and (Length(Pieces[0]) > 0) and (Length(Pieces[1]) > 0))
        {
            x = std::stoi(Pieces[0]);
            y = std::stoi(Pieces[1]);
        }
        // FIXME: legacy code as long as SplitString is not fixed
        else if (Pos("x", ResolutionString) > 0)
        {
            x = std::stoi(Copy(ResolutionString, 1, Pos("x", ResolutionString)-1));
            y = std::stoi(Copy(ResolutionString, Pos("x", ResolutionString)+1, 1000));
        } 
        else 
            Exit;

        // verify if resolution has proper values
        return (x > 0 && y > 0);
    }
}