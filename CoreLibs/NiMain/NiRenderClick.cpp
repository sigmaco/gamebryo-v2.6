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

#include "NiRenderClick.h"

NiImplementRootRTTI(NiRenderClick);

NiFixedString NiRenderClick::ms_kDefaultName;

//---------------------------------------------------------------------------
void NiRenderClick::Render(unsigned int uiFrameID)
{
    // Pre-processing callback.
    if (m_pfnPreProcessingCallback)
    {
        if (!m_pfnPreProcessingCallback(this, m_pvPreProcessingCallbackData))
        {
            return;
        }
    }

    // Get renderer pointer.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);

    // Set up the render target group, if it exists. If not, use the default
    // render target group.
    NiRenderTargetGroup* pkRenderTargetGroup = m_spRenderTargetGroup;
    if (!pkRenderTargetGroup)
    {
        pkRenderTargetGroup = pkRenderer->GetDefaultRenderTargetGroup();
    }

    // Calculate the effective clear mode (taking into account requests for
    // a single frame clear, e.g., after a device reset)
    unsigned int uiEffectiveClearMode =
        m_uiClearMode | m_uiSingleFrameClearMode;
    m_uiSingleFrameClearMode = 0;

    NIASSERT(pkRenderTargetGroup);
    const NiRenderTargetGroup* pkCurrentRenderTargetGroup =
        pkRenderer->GetCurrentRenderTargetGroup();
    if (pkCurrentRenderTargetGroup == pkRenderTargetGroup)
    {
        if (uiEffectiveClearMode)
        {
            if (GetUseRendererBackgroundColor())
            {
                pkRenderer->ClearBuffer(&m_kViewport, uiEffectiveClearMode);
            }
            else if (GetPersistBackgroundColorToRenderer())
            {
                pkRenderer->SetBackgroundColor(m_kBackgroudColor);
                pkRenderer->ClearBuffer(&m_kViewport, uiEffectiveClearMode);
            }
            else
            {
                NiColorA kColor;
                pkRenderer->GetBackgroundColor(kColor);
                pkRenderer->SetBackgroundColor(m_kBackgroudColor);
                pkRenderer->ClearBuffer(&m_kViewport, uiEffectiveClearMode);
                pkRenderer->SetBackgroundColor(kColor);
            }
        }
    }
    else
    {
        if (pkCurrentRenderTargetGroup)
        {
            pkRenderer->EndUsingRenderTargetGroup();
        }

        if (GetUseRendererBackgroundColor())
        {
            pkRenderer->BeginUsingRenderTargetGroup(pkRenderTargetGroup,
                uiEffectiveClearMode);
        }
        else if (GetPersistBackgroundColorToRenderer())
        {
            pkRenderer->SetBackgroundColor(m_kBackgroudColor);
            pkRenderer->BeginUsingRenderTargetGroup(pkRenderTargetGroup,
                uiEffectiveClearMode);
        }
        else
        {
            NiColorA kColor;
            pkRenderer->GetBackgroundColor(kColor);
            pkRenderer->SetBackgroundColor(m_kBackgroudColor);
            pkRenderer->BeginUsingRenderTargetGroup(pkRenderTargetGroup,
                uiEffectiveClearMode);
            pkRenderer->SetBackgroundColor(kColor);
        }

    }

    // Custom rendering step.
    PerformRendering(uiFrameID);

    // Post-processing callback.
    if (m_pfnPostProcessingCallback)
    {
        if (!m_pfnPostProcessingCallback(this, m_pvPostProcessingCallbackData))
        {
            return;
        }
    }
}
//---------------------------------------------------------------------------
