#pragma once
/* UltraStar Deluxe - Karaoke Game
 *
 * UltraStar Deluxe is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING. If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/base/UPlatform.pas $
 * $Id: UPlatform.pas 2424 2010-05-28 22:35:34Z k-m_schindler $
 */
#include "../switches.h"

#include <filesystem>

#include "USingletonApp.h"
#include "UConfig.h"

namespace UPlatformBase
{
    // Comment by Eddie:
    // This unit defines an interface for platform specific utility functions.
    // The Interface is implemented in separate files for each platform:
    // UPlatformWindows, UPlatformLinux and UPlatformMacOSX.
    class TPlatformBase : public USingletonApp::ApplicationSingleton
    {
        TPlatformBase()
            : ApplicationSingleton(UConfig::USDXVersionStr())
        {}
    };
}