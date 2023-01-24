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
 * $URL: svn://basisbit@svn.code.sf.net/p/ultrastardx/svn/trunk/src/base/URecord.pas $
 * $Id: URecord.pas 2814 2011-04-06 23:31:15Z k-m_schindler $
 */

#include <string>
#include <vector>
#include <array>
#include <list>
#include <queue>
#include <mutex>

#include "UMusic.h"

namespace URecord
{
    /*
    uses
      Classes,
      Math,
      sdl2,
      SysUtils,
      UCommon,
      UMusic,
      UIni;
      */
    constexpr int BaseToneFreq = 440; // A4 concert pitch of 440 Hz
    constexpr int NumHalftones = 49;  // C2 to C6 (1046.5023 Hz)

    // pitch detection algorithm (PDA)
    enum class TPDAType
    {
        PDA_AMDF,   // average magnitude difference function
        PDA_CAMDF  // circular average magnitude difference function
    };

    typedef std::array<double, NumHalftones> TFrequencyArray;
    typedef std::array<int, NumHalftones> TDelayArray;
    typedef std::array<double, NumHalftones> TCorrelationArray;

    class TCaptureBuffer
    {
    private:

        UMusic::TAudioVoiceStream::SPtr fVoiceStream; // stream for voice passthrough
        std::mutex fAnalysisBufferLock;
        UMusic::TAudioFormatInfo fAudioFormat;
        
        TDelayArray Delays;

        std::string GetToneString(); // converts a tone to its string representation;

        void BoostBuffer(std::vector<int16_t>& Buffer);
        void ProcessNewBuffer(std::vector<int16_t>& Buffer);

        //must be called when AudioFormat changes
        unsigned int prevSampleRate = -1;
        void SetDelays();
        // we call it to analyze sound by checking AMDF
        bool AnalyzePitch(TPDAType PDA);
        // use this to check one frequency by AMDF
        TCorrelationArray AverageMagnitudeDifference();
        TCorrelationArray CircularAverageMagnitudeDifference();

        static constexpr TFrequencyArray generateFrequencies();
        static constexpr TFrequencyArray Frequencies = generateFrequencies();

    public:

        //static inline bool first = true;

        TCaptureBuffer();
        ~TCaptureBuffer();

        constexpr size_t MaxBufferSize = 4096;
        std::deque<int16_t> AnalysisBuffer; // newest 4096 samples (MUST BE POWER OF TWO!)
        TMemoryStream LogBuffer;              // full buffer

        // pitch detection
        // TODO: remove ToneValid, set Tone/ToneAbs=-1 if invalid instead
        bool ToneValid;    // true if Tone contains a valid value (otherwise it contains noise)
        int Tone;    // tone relative to one octave (e.g. C2=C3=C4). Range: 0-11
        int ToneAbs;    // absolute (full range) tone (e.g. C2<>C3). Range: 0..NumHalftones-1

        // methods
        void StartCapture(UMusic::TAudioFormatInfo Format);
        void StopCapture();

        void Clear();

        // use to analyze sound from buffers to get new pitch
        void AnalyzeBuffer();

        float MaxSampleVolume();
        //property ToneString: string READ GetToneString;
        //property AudioFormat: TAudioFormatInfo READ fAudioFormat;

        typedef std::shared_ptr<TCaptureBuffer> SPtr;
    };

    const std::string DEFAULT_SOURCE_NAME = "[Default]";


    struct TAudioInputSource
    {
        std::string Name;
    };

    // soundcard input-devices information
    class TAudioInputDevice
    {
    public:

        virtual ~TAudioInputDevice() = default;

        int CfgIndex;   // index of this device in Ini.InputDeviceConfig
        std::string Name;    // soundcard name
        std::vector<TAudioInputSource> Source; // soundcard input-sources
        int SourceRestore;  // source-index that will be selected after capturing (-1: not detected)
        int MicSource;  // source-index of mic (-1: none detected)

        UMusic::TAudioFormatInfo AudioFormat; // capture format info (e.g. 44.1kHz SInt16 stereo)
        std::vector<TCaptureBuffer::SPtr> CaptureChannel; // sound-buffer references used for mono or stereo channel's capture data

        void LinkCaptureBuffer(int ChannelIndex, TCaptureBuffer::SPtr Sound);

        // TODO: add Open/Close functions so Start/Stop becomes faster
        //function Open():    bool; virtual; abstract;
        //function Close():   bool; virtual; abstract;
        virtual bool Start() = 0;
        virtual bool Stop() = 0;

        virtual float GetVolume() = 0;
        virtual void SetVolume(float Volume) = 0;

        //TODO:: typedef in parent not working as intended!!!
        typedef std::shared_ptr<TAudioInputSource> SPtr;
    };

    typedef std::vector<bool> TBooleanDynArray;

    class TAudioInputProcessor
    {
    public:

        std::vector<TCaptureBuffer> Sound; // sound-buffers for every player
        std::vector<TAudioInputDevice> DeviceList;


        void UpdateInputDeviceConfig();

        /**
         * Validates the mic settings.
         * If a player was assigned to multiple mics a popup will be displayed
         * with the ID of the player.
         * The return value is the player number of the first player that is not
         * configured correctly or 0 if all players are correct.
         */
        int ValidateSettings();

        /**
         * Checks if players 1 to PlayerCount are configured correctly.
         * A player is configured if a device's channel is assigned to him.
         * For each player (up to PlayerCount) the state will be in PlayerState.
         * If a player's state is true the player is configured, otherwise not.
         * The return value is the player number of the first player that is not
         * configured correctly or 0 if all players are correct.
         * The PlayerState array is zero based (index 0 for player 1).
         */
        int CheckPlayersConfig(uint32_t PlayerCount, TBooleanDynArray& PlayerState);

        /**
         * Same as the array version but it does not output a state for each player.
         */
        int CheckPlayersConfig(uint32_t PlayerCount);

        /**
         * Handle microphone input
         */
        void HandleMicrophoneData(std::vector<uint8_t>& Buffer, const TAudioInputDevice& InputDevice);
    };

    class TAudioInputBase : public UMusic::IAudioInput
    {
    private:

        bool Started;

    protected:

        std::string UnifyDeviceName(const std::string name, int deviceIndex);

    public:

        virtual std::string GetName() = 0;
        virtual bool InitializeRecord() = 0;
        virtual bool FinalizeRecord() override;

        void CaptureStart() override;
        void CaptureStop() override;
    };

    //typedef  TSmallIntArray = array [0..(MaxInt div SizeOf(SmallInt))-1] of SmallInt;
    //PSmallIntArray = ^TSmallIntArray;

    TAudioInputProcessor AudioInputProcessor();

    inline TAudioInputProcessor singleton_AudioInputProcessor = nil;
}