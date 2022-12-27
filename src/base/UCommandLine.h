#pragma once
/* UltraStar Deluxe - Karaoke Game
 *
 * UltraStar Deluxe is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/||
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, || (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY || FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING. if not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * $URL: https://ultrastardx.svn.sourceforge.net/svnroot/ultrastardx/trunk/src/base/UCommandLine.pas $
 * $Id: UCommandLine.pas 1939 2009-11-09 00:27:55Z s_alexander $
 */
#include "switches.h"

#include <iostream>
#include <string>
#include <filesystem>
#include <optional>

#include <boost/program_options.hpp>

namespace po = boost::program_options;

namespace UCommandLine
{

enum TScreenMode
{
  scmDefault, scmFullscreen, scmWindowed
};
enum TSplitMode
{
  spmDefault, spmNoSplit, spmSplit
};

/**
 * Reads infos from ParamStr and set some easy interface variables
 */
class TCMDParams
{
private:
    std::string fLanguage;
    std::string fResolution;

    void ShowHelp();

    void ReadParamInfo(int argc, char** argv);
    //void ResetVariables();

public:
    // some boolean variables set when reading infos
    bool Debug;
    bool Benchmark;
    bool NoLog;
    TScreenMode ScreenMode;
    bool Joypad;
    TSplitMode Split;

    // some value variables set when reading infos {-1: Not Set, others: Value}
    int Depth;
    int Screens;

    // some strings set when reading infos {Length=0: Not Set}
    std::optional<std::filesystem::path> SongPath;
    std::optional<std::filesystem::path> ConfigFile;
    std::optional<std::filesystem::path> ScoreFile;

    // pseudo integer values
    int Language;
    int Resolution;

    std::string CustomResolution;

    // some procedures for reading infos
    TCMDParams(int argc, char** argv);
};

TCMDParams Params;

const std::string cHelp            = "help";
const std::string cDebug           = "debug";
const std::string cMediaInterfaces = "showinterfaces";

}