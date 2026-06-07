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

// Precompiled Header
#include "NiVisualTrackerPCH.h"

#include "NiVisualTrackerRenderClick.h"

NiImplementRTTI(NiVisualTrackerRenderClick, NiRenderClick);

//---------------------------------------------------------------------------
unsigned int NiVisualTrackerRenderClick::GetNumObjectsDrawn() const
{
    return m_uiNumObjectsDrawn;
}
//---------------------------------------------------------------------------
float NiVisualTrackerRenderClick::GetCullTime() const
{
    // This render click does not perform any culling, so the cull time is
    // always zero.
    return 0.0f;
}
//---------------------------------------------------------------------------
float NiVisualTrackerRenderClick::GetRenderTime() const
{
    return m_fRenderTime;
}
//---------------------------------------------------------------------------
void NiVisualTrackerRenderClick::PerformRendering(unsigned int)
{
    // Get renderer pointer.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);

    // Set up the renderer's camera data. Not strictly necessary for the view
    // transform or frustum, but needed for the viewport.
    pkRenderer->SetScreenSpaceCameraData(&m_kViewport);

    // Draw the NiVisualTracker objects.
    float fBeginTime = NiGetCurrentTimeInSec();
    NiTListIterator kIter = m_kVisualTrackers.GetHeadPos();
    while (kIter)
    {
        NiVisualTracker* pkVisualTracker = m_kVisualTrackers.GetNext(kIter);
        NIASSERT(pkVisualTracker);
        pkVisualTracker->Draw();
    }

#if defined(_PS3)
    if (m_spSPUTracker)
        m_spSPUTracker->Draw();
#endif

    // Update rendering statistics.
    m_fRenderTime = NiGetCurrentTimeInSec() - fBeginTime;
    m_uiNumObjectsDrawn = m_kVisualTrackers.GetSize();
}
//---------------------------------------------------------------------------
#if defined(_PS3)
//---------------------------------------------------------------------------
NiSPUVisualTracker* NiVisualTrackerRenderClick::GetSPUTracker() const
{
    return m_spSPUTracker;
}
//---------------------------------------------------------------------------
void NiVisualTrackerRenderClick::SetSPUTracker(NiSPUVisualTracker* pkTracker)
{
    m_spSPUTracker = pkTracker;
}
//---------------------------------------------------------------------------
#endif
