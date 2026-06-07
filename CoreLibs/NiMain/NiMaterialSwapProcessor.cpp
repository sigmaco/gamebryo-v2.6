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

#include "NiMaterialSwapProcessor.h"
#include "NiRenderer.h"
#include "NiRenderObject.h"

NiImplementRTTI(NiMaterialSwapProcessor, NiRenderListProcessor);

//---------------------------------------------------------------------------
void NiMaterialSwapProcessor::PreRenderProcessList(
    const NiVisibleArray* pkInput, NiVisibleArray& kOutput, void*)
{
    // If the input array pointer is null, do nothing.
    if (!pkInput)
    {
        return;
    }

    const unsigned int uiInputCount = pkInput->GetCount();

    // If the material pointer is null, defer rendering of all objects.
    if (!m_spMaterial)
    {
        for (unsigned int ui = 0; ui < uiInputCount; ui++)
        {
            kOutput.Add(pkInput->GetAt(ui));
        }
        return;
    }

    // Get renderer pointer.
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);

    // Iterate over input array, swapping material and rendering.
    for (unsigned int ui = 0; ui < uiInputCount; ui++)
    {
        NiRenderObject& kMesh = pkInput->GetAt(ui);

        // Backup current active material.
        const NiMaterial* pkPreviousMaterial = kMesh.GetActiveMaterial();

        // Apply and set active material to new material.
        kMesh.ApplyAndSetActiveMaterial(m_spMaterial,
            m_uiMaterialExtraData);

        // Render object.
        kMesh.RenderImmediate(pkRenderer);
        
        // Restore previous active material.
        kMesh.SetActiveMaterial(pkPreviousMaterial);
    }

    // Don't add any geometry objects to the output array.
}
//---------------------------------------------------------------------------
