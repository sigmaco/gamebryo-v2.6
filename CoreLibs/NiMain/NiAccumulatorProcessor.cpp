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

#include "NiAccumulatorProcessor.h"
#include "NiRenderer.h"

NiImplementRTTI(NiAccumulatorProcessor, NiRenderListProcessor);

//---------------------------------------------------------------------------
void NiAccumulatorProcessor::PreRenderProcessList(
    const NiVisibleArray* pkInput, NiVisibleArray& kOutput, void*)
{
    // If the input array pointer is null, do nothing.
    if (!pkInput)
    {
        return;
    }

    // Propogate all objects from input array to output array.
    const unsigned int uiInputCount = pkInput->GetCount();
    for (unsigned int ui = 0; ui < uiInputCount; ui++)
    {
        kOutput.Add(pkInput->GetAt(ui));
    }

    // If the accumulator pointer is null, defer rendering of all objects.
    if (!m_spAccumulator)
    {
        return;
    }

    // Get renderer pointer.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);

    // Get camera data from renderer.
    pkRenderer->GetCameraData(*m_spDummyCamera);

    if (!m_spAccumulator->IsAccumulating())
    {
        // Start accumulating.
        m_spAccumulator->StartAccumulating(m_spDummyCamera);
    }

    // Register the input object array.
    m_spAccumulator->RegisterObjectArray(kOutput);

    if (m_bFinishAccumulating)
    {
        // Finish accumulating (renders all deferred objects).
        m_spAccumulator->FinishAccumulating();
    }

    // Clear the output array, since all objects have been rendered.
    kOutput.RemoveAll();
}
//---------------------------------------------------------------------------
