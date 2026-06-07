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

#include "NiClickRenderStep.h"

NiImplementRTTI(NiClickRenderStep, NiRenderStep);

//---------------------------------------------------------------------------
NiClickRenderStep::NiClickRenderStep() : m_uiNumObjectsDrawn(0),
    m_fCullTime(0.0f), m_fRenderTime(0.0f)
{
}
//---------------------------------------------------------------------------
bool NiClickRenderStep::SetOutputRenderTargetGroup(
    NiRenderTargetGroup* pkOutputRenderTargetGroup)
{
    // Get list of render clicks.
    const NiTPointerList<NiRenderClickPtr>& kRenderClicks = GetRenderClicks();

    if (kRenderClicks.GetSize() > 0)
    {
        // Set the render target group for the last render click in the list
        // to be the value passed into this function.
        kRenderClicks.GetTail()->SetRenderTargetGroup(
            pkOutputRenderTargetGroup);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiRenderTargetGroup* NiClickRenderStep::GetOutputRenderTargetGroup()
{
    // Get list of render clicks.
    const NiTPointerList<NiRenderClickPtr>& kRenderClicks = GetRenderClicks();

    NiRenderTargetGroup* pkOutputRenderTargetGroup = NULL;
    if (kRenderClicks.GetSize() > 0)
    {
        // Get the render target group for the last render click in the list.
        pkOutputRenderTargetGroup = kRenderClicks.GetTail()
            ->GetRenderTargetGroup();
        if (!pkOutputRenderTargetGroup)
        {
            // If the render click's render target is NULL, return the default
            // render target group.
            NiRenderer* pkRenderer = NiRenderer::GetRenderer();
            NIASSERT(pkRenderer);
            pkOutputRenderTargetGroup =
                pkRenderer->GetDefaultRenderTargetGroup();
        }
    }

    return pkOutputRenderTargetGroup;
}
//---------------------------------------------------------------------------
unsigned int NiClickRenderStep::GetNumObjectsDrawn() const
{
    return m_uiNumObjectsDrawn;
}
//---------------------------------------------------------------------------
float NiClickRenderStep::GetCullTime() const
{
    return m_fCullTime;
}
//---------------------------------------------------------------------------
float NiClickRenderStep::GetRenderTime() const
{
    return m_fRenderTime;
}
//---------------------------------------------------------------------------
void NiClickRenderStep::PerformRendering()
{
    // Get the list of render clicks to execute.
    const NiTPointerList<NiRenderClickPtr>& kRenderClicks = GetRenderClicks();

    // Render list of clicks.
    RenderClicks(kRenderClicks);
}
//---------------------------------------------------------------------------
void NiClickRenderStep::RenderClicks(
    const NiTPointerList<NiRenderClickPtr>& kRenderClicks)
{
    // Get renderer pointer.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);

    // Get the current frame ID from the renderer.
    const unsigned int uiFrameID = pkRenderer->GetFrameID();

    // Reset rendering statistics.
    m_uiNumObjectsDrawn = 0;
    m_fCullTime = m_fRenderTime = 0.0f;

    // Iterate over render clicks.
    NiTListIterator kIter = kRenderClicks.GetHeadPos();
    while (kIter)
    {
        NiRenderClick* pkRenderClick = kRenderClicks.GetNext(kIter);
        NIASSERT(pkRenderClick);

        // Only perform rendering if the render click indicates that it should
        // be performed.
        if (pkRenderClick->ShouldPerformRendering(uiFrameID))
        {
            // Execute the render click, passing it the current frame ID.
            pkRenderClick->Render(uiFrameID);

            // Update rendering statistics.
            m_uiNumObjectsDrawn += pkRenderClick->GetNumObjectsDrawn();
            m_fCullTime += pkRenderClick->GetCullTime();
            m_fRenderTime += pkRenderClick->GetRenderTime();
        }
    }
}
//---------------------------------------------------------------------------
