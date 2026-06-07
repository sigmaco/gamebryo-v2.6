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

#include "NiRenderFrame.h"
#include "NiRenderer.h"

NiFixedString NiRenderFrame::ms_kDefaultName;

//---------------------------------------------------------------------------
void NiRenderFrame::Draw()
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

    // Open rendering frame.
    pkRenderer->BeginFrame();

    // Iterate over render steps.
    NiTListIterator kStepsIter = m_kRenderSteps.GetHeadPos();
    while (kStepsIter)
    {
        NiRenderStep* pkRenderStep = m_kRenderSteps.GetNext(kStepsIter);
        NIASSERT(pkRenderStep);

        // Execute the render step if it is active.
        if (pkRenderStep->GetActive())
        {
            pkRenderStep->Render();
        }
    }

    // Close render target group if one is open. This is to prevent each
    // render step from having to close its render target group during Render.
    // Thus, each render step does not close its render target group and only
    // opens it if it is not already open. The last open render target group
    // is closed here.
    if (pkRenderer->IsRenderTargetGroupActive())
    {
        pkRenderer->EndUsingRenderTargetGroup();
    }

    // Close rendering frame.
    pkRenderer->EndFrame();

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
void NiRenderFrame::Display()
{
    // Get renderer pointer.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);

    // Display the frame.
    pkRenderer->DisplayFrame();
}
//---------------------------------------------------------------------------
