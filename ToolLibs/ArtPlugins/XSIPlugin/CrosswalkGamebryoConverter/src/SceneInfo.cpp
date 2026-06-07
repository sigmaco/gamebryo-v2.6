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

#include "CrosswalkGamebryoSceneInfo.h"
#include "CrosswalkGamebryoImpl.h"

#include "Scene.h"
#include "SceneInfo.h"

namespace epg
{
    //---------------------------------------------------------------------------
    // Create a converter with the specified settings.
    //---------------------------------------------------------------------------
    SceneInfo::SceneInfo(CSLScene& in_Scene)
        : m_FramesPerSecond(0.0f)
        , m_StartTime(0.0f)
        , m_EndTime(0.0f)
        , m_TimingIsInSeconds(true)
    {
        CSLSceneInfo* info = in_Scene.SceneInfo();
        if (info)
        {
            m_FramesPerSecond      = info->GetFrameRate();
            m_TimingIsInSeconds    = (info->GetTimingType() == CSLSceneInfo::SI_SECONDS);
            m_TimeConversionFactor = (m_TimingIsInSeconds ? 1.0f : (1.0f / m_FramesPerSecond));
            m_StartTime            = info->GetStart() * m_TimeConversionFactor;
            m_EndTime              = info->GetEnd() * m_TimeConversionFactor;
        }
    }

    //---------------------------------------------------------------------------
    // Reset the scene info.
    //---------------------------------------------------------------------------
    void SceneInfo::Clear()
    {
        m_FramesPerSecond = 0.0f;
    }

    //---------------------------------------------------------------------------
    // Retrieve the frame rate as frames-per-second.
    //---------------------------------------------------------------------------
    float SceneInfo::GetFramesPerSecond() const
    {
        return m_FramesPerSecond;
    }

    //---------------------------------------------------------------------------
    // Retrieve the time in seconds when the scene begins.
    //---------------------------------------------------------------------------
    float SceneInfo::GetStartTime() const
    {
        return m_StartTime;
    }

    //---------------------------------------------------------------------------
    // Retrieve the time in seconds when the scene ends.
    //---------------------------------------------------------------------------
    float SceneInfo::GetEndTime() const
    {
        return m_EndTime;
    }

    //---------------------------------------------------------------------------
    // Check if the time values found in Crosswalk FCurves are in seconds or in frames.
    //---------------------------------------------------------------------------
    bool SceneInfo::IsTimingInSeconds() const
    {
        return m_TimingIsInSeconds;
    }

    //---------------------------------------------------------------------------
    // Convert an XSI time value to seconds,
    // taking into consideration the time format and playback rate.
    // We also round the times to a particular precision to avoid
    // slight innacuracies.
    //---------------------------------------------------------------------------
    float SceneInfo::ConvertTime(SI_Float in_XSITime) const
    {
        return in_XSITime * m_TimeConversionFactor;
    }

    //---------------------------------------------------------------------------
}
