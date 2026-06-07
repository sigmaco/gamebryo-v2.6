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
#include "NiFontPCH.h"

#include "Ni2DStringRenderClick.h"

NiImplementRTTI(Ni2DStringRenderClick, NiRenderClick);

//---------------------------------------------------------------------------
unsigned int Ni2DStringRenderClick::GetNumObjectsDrawn() const
{
    return m_uiNumObjectsDrawn;
}
//---------------------------------------------------------------------------
float Ni2DStringRenderClick::GetCullTime() const
{
    // This render click does not perform any culling, so the cull time is
    // always zero.
    return 0.0f;
}
//---------------------------------------------------------------------------
float Ni2DStringRenderClick::GetRenderTime() const
{
    return m_fRenderTime;
}
//---------------------------------------------------------------------------
void Ni2DStringRenderClick::PerformRendering(unsigned int)
{
    // Get renderer pointer.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);

    // Set up the renderer's camera data. Not strictly necessary for the view
    // transform or frustum, but needed for the viewport.
    pkRenderer->SetScreenSpaceCameraData(&m_kViewport);

    // Draw the Ni2DString objects.
    float fBeginTime = NiGetCurrentTimeInSec();
    NiTListIterator kIter = m_k2DStrings.GetHeadPos();
    while (kIter)
    {
        Ni2DString* pk2DString = m_k2DStrings.GetNext(kIter);
        NIASSERT(pk2DString);
        pk2DString->Draw(pkRenderer);
    }

    // Update rendering statistics.
    m_fRenderTime = NiGetCurrentTimeInSec() - fBeginTime;
    m_uiNumObjectsDrawn = m_k2DStrings.GetSize();
}
//---------------------------------------------------------------------------
