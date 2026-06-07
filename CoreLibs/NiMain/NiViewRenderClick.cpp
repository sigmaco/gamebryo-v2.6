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

#include "NiViewRenderClick.h"
#include "NiRenderObject.h"

NiImplementRTTI(NiViewRenderClick, NiRenderClick);

NiRenderListProcessorPtr NiViewRenderClick::ms_spDefaultProcessor;

//---------------------------------------------------------------------------
unsigned int NiViewRenderClick::GetNumObjectsDrawn() const
{
    return m_uiNumObjectsDrawn;
}
//---------------------------------------------------------------------------
float NiViewRenderClick::GetCullTime() const
{
    return m_fCullTime;
}
//---------------------------------------------------------------------------
float NiViewRenderClick::GetRenderTime() const
{
    return m_fRenderTime;
}
//---------------------------------------------------------------------------
void NiViewRenderClick::PerformRendering(unsigned int uiFrameID)
{
    // If there are no views, return without rendering.
    if (m_kRenderViews.GetSize() == 0)
    {
        return;
    }

    // Get renderer pointer.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);

    // Get a pointer to the render list processor, using the default if none
    // has been specified.
    NiRenderListProcessor* pkProcessor = m_spProcessor;
    if (!pkProcessor)
    {
        pkProcessor = ms_spDefaultProcessor;
    }

    // Reset rendering statistics.
    m_uiNumObjectsDrawn = 0;
    m_fCullTime = m_fRenderTime = 0.0f;

    // Iterate over all render views.
    NiTListIterator kIter = m_kRenderViews.GetHeadPos();
    while (kIter)
    {
        NiRenderView* pkRenderView = m_kRenderViews.GetNext(kIter);
        NIASSERT(pkRenderView);

        // Set up the renderer's camera data.
        pkRenderView->SetCameraData(m_kViewport);

        // Get set of potentially visible geometry.
        float fBeginTime = NiGetCurrentTimeInSec();
        const NiVisibleArray& kGeometryToProcess =
            pkRenderView->GetPVGeometry(uiFrameID);

        // Update rendering statistics.
        m_fCullTime += NiGetCurrentTimeInSec() - fBeginTime;
        m_uiNumObjectsDrawn += kGeometryToProcess.GetCount();

        // Resize processed geometry list, if necessary (grow only).
        NIASSERT(m_kProcessedGeometry.GetCount() == 0);
        if (m_kProcessedGeometry.GetAllocatedSize() <
            kGeometryToProcess.GetCount())
        {
            m_kProcessedGeometry.SetAllocatedSize(
                kGeometryToProcess.GetCount());
        }

        // Use render list processor to pre-process the array of geometry.
        fBeginTime = NiGetCurrentTimeInSec();
        pkProcessor->PreRenderProcessList(&kGeometryToProcess,
            m_kProcessedGeometry, m_pvProcessorData);

        // Immediately render geometry returned from render list processor.
        const unsigned int uiGeometryCount = m_kProcessedGeometry.GetCount();
        for (unsigned int ui = 0; ui < uiGeometryCount; ui++)
        {
            NiAVObject* pkAVObj = &m_kProcessedGeometry.GetAt(ui);
            NIASSERT(NiIsKindOf(NiRenderObject, pkAVObj));
            reinterpret_cast<NiRenderObject*>(pkAVObj)->
                RenderImmediate(pkRenderer);
        }

        // Use render list processor to post-process the array of geometry
        // that was just rendered.
        pkProcessor->PostRenderProcessList(m_kProcessedGeometry,
            m_pvProcessorData);

        // Update rendering statistics.
        m_fRenderTime += NiGetCurrentTimeInSec() - fBeginTime;

        // Ensure array is clear for next render view.
        m_kProcessedGeometry.RemoveAll();
    }
}
//---------------------------------------------------------------------------
