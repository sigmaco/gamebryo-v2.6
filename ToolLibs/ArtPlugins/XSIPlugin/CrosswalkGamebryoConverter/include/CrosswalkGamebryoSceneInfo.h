// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2008 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#ifndef XSI_GAMEBRYO_CONVERTER_SCENE_INFO_H
#define XSI_GAMEBRYO_CONVERTER_SCENE_INFO_H

#include "SLTypes.h"

//---------------------------------------------------------------------------
// External references.
//---------------------------------------------------------------------------

class XSIEXPORT CSLScene;

namespace epg
{
    //---------------------------------------------------------------------------
    /// Global information about an XSI scene.
    //---------------------------------------------------------------------------

    class SceneInfo
    {
    public:
        /** Create a converter with the specified settings.
            @param SceneInfoSettings& the settings to be used for conversion.
        */
        SceneInfo(CSLScene& in_Scene);

        /// Reset the scene info.
        void Clear();

        /** Retrieve the frame rate as frames-per-second.
            @return the number of frames per seconds.
        */
        float GetFramesPerSecond() const;

        /// Retrieve the time in seconds when the scene begins.
        float GetStartTime() const;

        /// Retrieve the time in seconds when the scene ends.
        float GetEndTime() const;

        /** Check if the time values found in Crosswalk
            FCurves are in seconds or in frames.
            @return true if time is in seconds.
        */
        bool IsTimingInSeconds() const;

        /** Convert an XSI time value to seconds,
            taking into consideration the time format and playback rate.
            We also round the times to a particular precision to avoid
            slight innacuracies.
        */
        float ConvertTime(SI_Float in_XSITime) const;

    private:
        float           m_FramesPerSecond;
        float           m_StartTime;
        float           m_EndTime;
        float           m_TimeConversionFactor;
        bool            m_TimingIsInSeconds;
    };
}

#endif /* XSI_GAMEBRYO_CONVERTER_SCENE_INFO_H */
