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
#include "NiMainPCH.h"
#include "NiShadowGenerator.h"
#include "NiShadowRenderClick.h"

NiImplementRTTI(NiShadowRenderClick, NiViewRenderClick);

//---------------------------------------------------------------------------
void NiShadowRenderClick::PerformRendering(unsigned int uiFrameID)
{
    NIASSERT(m_pkGenerator);

    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    pkRenderer->SetRenderShadowCasterBackfaces(
        m_pkGenerator->GetRenderBackfaces());
    pkRenderer->SetRenderShadowTechnique(
        m_pkGenerator->GetShadowTechnique());

    NiViewRenderClick::PerformRendering(uiFrameID);

    SetBit(false, DIRTY);

    m_uiLastRenderedFrameID = uiFrameID;
}
//---------------------------------------------------------------------------
