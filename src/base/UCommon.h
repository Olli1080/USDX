﻿#pragma once
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
 * $URL: https://ultrastardx.svn.sourceforge.net/svnroot/ultrastardx/trunk/src/base/UCommon.pas $
 * $Id: UCommon.pas 2241 2010-04-15 17:57:15Z whiteshark0 $
 */

#include "../switches.h"

#include <vector>
#include <array>
#include <string>

namespace UCommon
{
/*
uses
  SysUtils,
  Classes,
  {$IFDEF MSWINDOWS}
  Windows,
  UPlatformWindows,
  {$ENDIF}
  UConfig,
  ULog,
  UPath;
*/
  typedef std::vector<int> TIntegerDynArray;
  typedef std::vector<std::string> TStringDynArray;
  typedef std::vector<std::string> TUTF8StringDynArray;

  const char SepWhitespace[4] = {'\t', 'n', '\r', ' '}; // tab, lf, cr, space

  const int SDL_BUTTON_WHEELUP = 1001; // emulated MouseButton ID for mouse wheel up. @Note some high number to prevent conflict. @see sdl_mouse.inc
  const int SDL_BUTTON_WHEELDOWN = 1002; // emulated MouseButton ID for mouse wheel down. @Note some high number to prevent conflict. @see sdl_mouse.inc

/**
 * Splits a string into pieces separated by Separators.
 * MaxCount specifies the max. number of pieces. If it is <= 0 the number is
 * not limited. If > 0 the last array element will hold the rest of the string
 * (with leading separators removed).
 *
 * Examples:
 *   SplitString(" split  me now ", 0) -> ["split", "me", "now"]
 *   SplitString(" split  me now ", 1) -> ["split", "me now"]
 */
TStringDynArray SplitString(const std::string Str, int MaxCount = 0, TSysCharSet Separators = SepWhitespace, bool RemoveEmpty = true): ;

bool StringInArray(const std::string Value, std::vector<std::string> Strings);

bool StringDeleteFromArray(TIntegerDynArray& InArray, const int InIndex);
bool StringDeleteFromArray(TStringDynArray& InStrings, const int InIndex);
bool StringDeleteFromArray(TUTF8StringDynArray& InStrings, const int InIndex);

std::string GetStringWithNoAccents(std::string str);

template<typename T>
struct TRGB
{
    T R;
    T G;
    T B;

    bool operator<(const TRGB<T>& rhs) const
    {
        if (R > rhs.R)
            return false;
        if (G > rhs.G)
            return false;
        return B < rhs.B;
    }

    template<typename S>
    friend TRGB<T> operator/(const TRGB& lhs<T>, S scalar)
    {
        return { lhs.R / scalar, lhs.G / scalar, lhs.B / scalar };
    }

    template<typename K = T>
    TRGB<K> sqrt() const
    {
        return { std::sqrt(R), std::sqrt(G), std::sqrt(B) };
    }
};

template <typename T> requires std::floating_point<T>
TRGB<T> convertNormalized(const TRGB<uint8_t>& in)
{
    return (TRGB<T>
    {
        in.R,
        in.G,
        in.B
    }) / 255.0;
}

struct TRGBA : TRGB<double>
{
   double A;

   template <typename ScalarType>
   friend TRGBA operator*(const TRGBA& lhs, const ScalarType& s)
   {
       return { lhs.R * s, lhs.G * s, lhs.B, lhs.A * s };
   }

   template <typename ScalarType>
   static TRGBA intensity(const TRGBA& lhs, const ScalarType& s)
   {
       return { lhs.R * s, lhs.G * s, lhs.B, lhs.A };
   }
};

struct TTexCoords
{
    double X1;
    double Y1;
    double X2;
    double Y2;
};

uint8_t HexToInt(std::string Hex);
std::string RGBToHex(int R, int G, int B);

template<typename T>
TRGB<T> HexToRGB(std::string Hex);

enum TMessageType 
{
  mtInfo, mtError
};

void ShowMessage(const std::string msg, TMessageType msgType = mtInfo);
/*
{$IFDEF FPC}
function RandomRange(aMin: int; aMax: int): int;
{$ENDIF}
*/
void DisableFloatingPointExceptions();
void SetDefaultNumericLocale();
void RestoreNumericLocale();

/*
{$IFNDEF MSWINDOWS}
void ZeroMemory(Destination: pointer; Length: dword);
function MakeLong(a, b: word): longint;
{$ENDIF}
*/
// A stable alternative to TList.Sort() (use TList.Sort() if applicable, see below)
//void MergeSort(List: TList; CompareFunc: TListSortCompare);

//function GetAlignedMem(Size: uint32_t; Alignment: int): pointer;
//void FreeAlignedMem(P: pointer);

//function Equals(A, B: string; CaseSensitive: bool = false): bool; overload;

int GetArrayIndex(const std::vector<std::string> SearchArray, std::string Value, bool CaseInsensitiv = false);
int GetArrayIndex(const std::vector<int> SearchArray, int Value);

bool ParseResolutionString(const std::string ResolutionString, int& x, int& y);
std::string BuildResolutionString(int x, int y);

/*
uses
  Math,
  {$IFDEF Delphi}
  Dialogs,
  {$ENDIF}
  sdl2,
  UFilesystem,
  UMain,
  UUnicodeUtils;
*/

/*
function StringInArray(const Value: string; Strings: array of string): bool;
var I: Integer;
begin
  Result := True;
  for I := Low(Strings) to High(Strings) do
    if Strings[i] = Value then Exit;
  Result := False;
end;

function StringDeleteFromArray(var InArray: TIntegerDynArray; const InIndex: int): bool;
begin
  Result := false;
  if (InIndex >= 0) and (InIndex < Length(InArray)) then
  begin
    Result := true;
    Move(InArray[InIndex + 1], InArray[InIndex], SizeOf(InArray[0]) * (Length(InArray) - InIndex - 1));
    SetLength(InArray, Length(InArray) - 1);
  end;
end;

function StringDeleteFromArray(var InStrings: TStringDynArray; const InIndex: int): bool;
var
  i: int;
begin
  Result := false;
  if (InIndex >= 0) and (InIndex < Length(InStrings)) then
  begin
    Result := true;
    for i := High(InStrings) downto InIndex+1 do
    begin
      InStrings[i-1] := InStrings[i];
    end;
    SetLength(InStrings, Length(InStrings) - 1);
  end;
end;

function StringDeleteFromArray(var InStrings: TUTF8StringDynArray; const InIndex: int): bool;
var
  i: int;
begin
  Result := false;
  if (InIndex >= 0) and (InIndex < Length(InStrings)) then
  begin
    Result := true;
    for i := High(InStrings) downto InIndex+1 do
    begin
      InStrings[i-1] := InStrings[i];
    end;
    SetLength(InStrings, Length(InStrings) - 1);
  end;
end;

function SplitString(const Str: string; MaxCount: int; Separators: TSysCharSet; RemoveEmpty: bool): TStringDynArray;

  // Adds Str[StartPos..Endpos-1] to the result array.
  void AddSplit(StartPos, EndPos: int);
  begin
    if (not RemoveEmpty) or (EndPos > StartPos) then
    begin
      SetLength(Result, Length(Result)+1);
      Result[High(Result)] := Copy(Str, StartPos, EndPos-StartPos);
    end;
  end;

var
  I, Count: int;
  Start: int;
begin
  Start := 0;
  Count := 0;
  SetLength(Result, 0);

  for I := 1 to Length(Str) do
  begin
    if (Str[I] in Separators) then
    begin
      inc(Count);
      AddSplit(Start+1, I);
      Start := I;
      if (MaxCount > 0) and (Count >= MaxCount) then Break
    end;
  end;

  // last component
  if (Start < Length(Str)+1) then
    AddSplit(Start+1, Length(Str)+1);
end;
*/
    /*
const std::array<std::string, 43> Accents = { "ç", "á", "é", "í", "ó", "ú", "ý", "à", "è", "ì", "ò", "ù", "ã", "õ", "ñ", "ä", "ë", "ï", "ö", "ü", "ÿ", "â", "ê", "î", "ô", "û", "ą", "ć", "ł", "ś", "ź", "!", "¡", "\"", "&", "(", ")", "?", "¿", ",", ".", ":", ";" };
const std::array<std::string, 43> NoAccents = { "c", "a", "e", "i", "o", "u", "y", "a", "e", "i", "o", "u", "a", "o", "n", "a", "e", "i", "o", "u", "y", "a", "e", "i", "o", "u", "a", "c", "l", "s", "z", "", "", "", "", "", "", "", "", "", "", "", "" };

std::string GetStringWithNoAccents(std::string str)
//var
  //i: int;
  //tmp: string;
{
	std::string tmp = str;//Utf8ToAnsi(str);
    for i : = 0 to High(Accents) do
        begin
        str : = StringReplace(str, Accents[i], NoAccents[i], [rfReplaceAll, rfIgnoreCase]);
    end;

Result: = str;
}



// data used by the ...Locale() functions
{$IF Defined(Linux) or Defined(FreeBSD)}

var
  PrevNumLocale: string;

const
  LC_NUMERIC  = 1;

function setlocale(category: int; locale: pchar): pchar; cdecl; external "c" name "setlocale";

{$IFEND}
*/
// In Linux and maybe MacOSX some units (like cwstring) call setlocale(LC_ALL, "")
// to set the language/country specific locale (e.g. charset) for this application.
// Unfortunately, LC_NUMERIC is set by this call too.
// It defines the decimal-separator and other country-specific numeric settings.
// This parameter is used by the C string-to-float parsing functions atof() and strtod().
// After changing LC_NUMERIC some external C-based libs (like projectM) are not
// able to parse strings correctly
// (e.g. in Germany "0.9" is not recognized as a valid number anymore but "0,9" is).
// So we reset the numeric settings to the default ("C").
// Note: The behaviour of Pascal parsing functions (e.g. strtofloat()) is not
//   changed by this because it doesn"t use the locale-settings.
// TODO:
// - Check if this is needed in MacOSX (at least the locale is set in cwstring)
// - Find out which libs are concerned by this problem.
//   If only projectM is concerned by this problem set and restore the numeric locale
//   for each call to projectM instead of changing it globally.

/*void SetDefaultNumericLocale();
begin
  {$IF Defined(LINUX) or Defined(FreeBSD)}
  PrevNumLocale := setlocale(LC_NUMERIC, nil);
  setlocale(LC_NUMERIC, "C");
  {$IFEND}
end;

void RestoreNumericLocale();
begin
  {$IF Defined(LINUX) or Defined(FreeBSD)}
  setlocale(LC_NUMERIC, PChar(PrevNumLocale));
  {$IFEND}
end;*/

/*
 * If an invalid floating point operation was performed the Floating-point unit (FPU)
 * generates a Floating-point exception (FPE). Dependending on the settings in
 * the FPU"s control-register (interrupt mask) the FPE is handled by the FPU itself
 * (we will call this as "FPE disabled" later on) or is passed to the application
 * (FPE enabled).
 * If FPEs are enabled a floating-point division by zero (e.g. 10.0 / 0.0) is
 * considered an error and an exception is thrown. Otherwise the FPU will handle
 * the error and return the result infinity (INF) (10.0 / 0.0 = INF) without
 * throwing an error to the application.
 * The same applies to a division by INF that either raises an exception
 * (FPE enabled) or returns 0.0 (FPE disabled).
 * Normally (as with C-programs), Floating-point exceptions (FPE) are DISABLED
 * on program startup (at least with Intel CPUs), but for some strange reasons
 * they are ENABLED in pascal (both delphi and FPC) by default.
 * Many libs operating with floating-point values rely heavily on the C-specific
 * behaviour. So using them in delphi is a ticking time-bomb because sooner or
 * later they will crash because of an FPE (this problem occurs massively
 * in OpenGL-based libs like projectM). In contrast to this no error will occur
 * if the lib is linked to a C-program.
 *
 * Further info on FPUs:
 * For x86 and x86_64 CPUs we have to consider two FPU instruction sets.
 * The math co-processor i387 (aka 8087 or x87) set introduced with the i386
 * and SSE (Streaming SIMD Extensions) introduced with the Pentium3.
 * Both of them have separate control-registers (x87: FPUControlWord, SSE: MXCSR)
 * to control FPEs. Either has (among others) 6bits to enable/disable several
 * exception types (Invalid,Denormalized,Zero,Overflow,Underflow,Precision).
 * Those exception-types must all be masked (=1) to get the default C behaviour.
 * The control-registers can be set with the asm-ops FLDCW (x87) and LDMXCSR (SSE).
 * Instead of using assembler code, we can use Set8087CW() provided by delphi and
 * FPC to set the x87 control-word. FPC also provides SetSSECSR() for SSE"s MXCSR.
 * Note that both Delphi and FPC enable FPEs (e.g. for div-by-zero) on program
 * startup but only FPC enables FPEs (especially div-by-zero) for SSE too.
 * So we have to mask FPEs for x87  in Delphi and FPC and for SSE in FPC only.
 * FPC and Delphi both provide a SetExceptionMask() for control of the FPE
 * mask. SetExceptionMask() sets the masks for x87 in Delphi and for x87 and SSE
 * in FPC (seems as if Delphi [2005] is not SSE aware). So SetExceptionMask()
 * is what we need and it even is plattform and CPU independent.
 *
 * Pascal OpenGL headers (like the Delphi standard ones or JEDI-SDL headers)
 * already call Set8087CW() to disable FPEs but due to some bugs in the JEDI-SDL
 * headers they do not work properly with FPC. I already patched them, so they
 * work at least until they are updated the next time. In addition Set8086CW()
 * does not suffice to disable FPEs because the SSE FPEs are not disabled by this.
 * FPEs with SSE are a big problem with some libs because many linux distributions
 * optimize code for SSE or Pentium3 (for example: int(INF) which convert the
 * double value "infinity" to an int might be automatically optimized by
 * using SSE"s CVTSD2SI instruction). So SSE FPEs must be turned off in any case
 * to make USDX portable.
 *
 * Summary:
 * Call this function on initialization to make sure FPEs are turned off.
 * It will solve a lot of errors with FPEs in external libs.
 */
//void DisableFloatingPointExceptions();
//begin
  /*
  // We will use SetExceptionMask() instead of Set8087CW()/SetSSECSR().
  // Note: Leave these lines for documentation purposes just in case
  //       SetExceptionMask() does not work anymore (due to bugs in FPC etc.).
  {$IF Defined(CPU386) or Defined(CPUI386) or Defined(CPUX86_64)}
  Set8087CW($133F);
  {$IFEND}
  {$IF Defined(FPC)}
  if (has_sse_support) then
    SetSSECSR($1F80);
  {$IFEND}
  */

  // disable all of the six FPEs (x87 and SSE) to be compatible with C/C++ and
  // other libs which rely on the standard FPU behaviour (no div-by-zero FPE anymore).
 /* SetExceptionMask([exInvalidOp, exDenormalized, exZeroDivide,
                    exOverflow, exUnderflow, exPrecision]);
end;

{$IFNDEF MSWINDOWS}
void ZeroMemory(Destination: pointer; Length: dword);
begin
  FillChar(Destination^, Length, 0);
end;

function MakeLong(A, B: word): longint;
begin
  Result := (LongInt(B) shl 16) + A;
end;

{$ENDIF}

{$IFDEF FPC}
function RandomRange(aMin: int; aMax: int): int;
begin
  RandomRange := Random(aMax - aMin) + aMin ;
end;
{$ENDIF}


{$IFDEF FPC}
var
  MessageList: TStringList;
  ConsoleHandler: TThreadID;
  // Note: TRTLCriticalSection is defined in the units System and Libc, use System one
  ConsoleCriticalSection: System.TRTLCriticalSection;
  ConsoleEvent: PRTLEvent;
  ConsoleQuit: bool;
{$ENDIF}
*/
/*
 * The console-handlers main-function.
 * TODO: create a quit-event on closing.
 */
    /*
function ConsoleHandlerFunc(param: pointer): PtrInt;
var
  i: int;
  quit: bool;
begin
  quit := false;
  while (not quit) do
  begin
    // wait for new output or quit-request
    RTLeventWaitFor(ConsoleEvent);

    System.EnterCriticalSection(ConsoleCriticalSection);
    // output pending messages
    for i := 0 to MessageList.Count - 1 do
    begin
      _ConsoleWriteLn(MessageList[i]);
    end;
    MessageList.Clear();

    // use local quit-variable to avoid accessing
    // ConsoleQuit outside of the critical section
    if (ConsoleQuit) then
      quit := true;

    RTLeventResetEvent(ConsoleEvent);
    System.LeaveCriticalSection(ConsoleCriticalSection);
  end;
  result := 0;
end;
{$ENDIF}

void InitConsoleOutput();
begin
  Log := TLog.Create;
  {$IFDEF FPC}
  // init thread-safe output
  MessageList := TStringList.Create();
  System.InitCriticalSection(ConsoleCriticalSection);
  ConsoleEvent := RTLEventCreate();
  ConsoleQuit := false;
  // must be a thread managed by FPC. Otherwise (e.g. SDL-thread)
  // it will crash when using Writeln.
  ConsoleHandler := BeginThread(@ConsoleHandlerFunc);
  {$ENDIF}
end;

void FinalizeConsoleOutput();
begin
  {$IFDEF FPC}
  // terminate console-handler
  System.EnterCriticalSection(ConsoleCriticalSection);
  ConsoleQuit := true;
  RTLeventSetEvent(ConsoleEvent);
  System.LeaveCriticalSection(ConsoleCriticalSection);
  WaitForThreadTerminate(ConsoleHandler, 0);
  // free data
  System.DoneCriticalsection(ConsoleCriticalSection);
  RTLeventDestroy(ConsoleEvent);
  MessageList.Free();
  {$ENDIF}
  Log.Free;
end;
*/
/*
 * FPC uses threadvars (TLS) managed by FPC for console output locking.
 * Using WriteLn() from external threads (like in SDL callbacks)
 * will crash the program as those threadvars have never been initialized.
 * The solution is to create an FPC-managed thread which has the TLS data
 * and use it to handle the console-output (hence it is called Console-Handler)
 */
    /*
void ConsoleWriteLn(const msg: string);
begin
{$IFDEF CONSOLE}
  {$IFDEF FPC}
  // TODO: check for the main-thread and use a simple _ConsoleWriteLn() then?
  //GetCurrentThreadThreadId();
  System.EnterCriticalSection(ConsoleCriticalSection);
  MessageList.Add(msg);
  RTLeventSetEvent(ConsoleEvent);
  System.LeaveCriticalSection(ConsoleCriticalSection);
  {$ELSE}
  _ConsoleWriteLn(msg);
  {$ENDIF}
{$ENDIF}
end;

void ShowMessage(const msg: String; msgType: TMessageType);
{$IFDEF MSWINDOWS}
var Flags: uint32_t;
{$ENDIF}
begin
{$IF Defined(MSWINDOWS)}
  case msgType of
    mtInfo:  Flags := MB_ICONINFORMATION or MB_OK;
    mtError: Flags := MB_ICONERROR or MB_OK;
    else Flags := MB_OK;
  end;
  MessageBox(0, PChar(msg), PChar(USDXVersionStr()), Flags);
{$ELSE}
  ConsoleWriteln(msg);
{$IFEND}
end;
*/
/*
initialization
  InitConsoleOutput();

finalization
  FinalizeConsoleOutput();

end.*/
};