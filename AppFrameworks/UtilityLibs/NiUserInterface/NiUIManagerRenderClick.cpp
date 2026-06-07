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
#include "NiUserInterfacePCH.h"

#include "NiUIManagerRenderClick.h"
#include "NiUIManager.h"
#include <NiRenderer.h>

NiImplementRTTI(NiUIManagerRenderClick, NiRenderClick);

//---------------------------------------------------------------------------
NiUIManagerRenderClick::NiUIManagerRenderClick() : m_fRenderTime(0.0f)
{
}
//---------------------------------------------------------------------------
unsigned int NiUIManagerRenderClick::GetNumObjectsDrawn() const
{
    // NiUIManager does not currently internally track the number of objects
    // that it renders during its Draw function, so no meaningful count can
    // be returned here.
    return 1;
}
//---------------------------------------------------------------------------
float NiUIManagerRenderClick::GetCullTime() const
{
    // This render click does not perform any culling, so the cull time is
    // always zero.
    return 0.0f;
}
//---------------------------------------------------------------------------
float NiUIManagerRenderClick::GetRenderTime() const
{
    return m_fRenderTime;
}
//---------------------------------------------------------------------------
void NiUIManagerRenderClick::PerformRendering(unsigned int)
{
    // If no UI manager exists, do nothing.
    NiUIManager* pkUIManager = NiUIManager::GetUIManager();
    if (!pkUIManager)
    {
        return;
    }

    // Get renderer pointer.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);

    // Draw UI manager.
    float fBeginTime = NiGetCurrentTimeInSec();
    pkUIManager->Draw(pkRenderer);

    // Update rendering statistics.
    m_fRenderTime = NiGetCurrentTimeInSec() - fBeginTime;
}
//---------------------------------------------------------------------------
