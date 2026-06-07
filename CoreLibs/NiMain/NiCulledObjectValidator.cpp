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

#include "NiCulledObjectValidator.h"

NiImplementRTTI(NiCulledObjectValidator, NiRenderClickValidator);

//---------------------------------------------------------------------------
bool NiCulledObjectValidator::ValidateClick(NiRenderClick*,
    unsigned int uiFrameID)
{
    // Execute the render click if the object, camera, or culling process is
    // null.
    if (!m_spObject || !m_spCamera || !m_spCullingProcess)
    {
        return true;
    }

    // Avoid performing more than one culling operation for the same frame,
    // unless m_bForceUpdate has been set to true. This will always be true
    // for the very first call to this function.
    if (uiFrameID == m_uiLastFrameID && !m_bForceUpdate)
    {
        return m_bLastReturnValue;
    }

    // Use the culling process to cull the geometry object against the camera's
    // frustum. The culling process must contain an internal NiVisibleSet, as
    // none is provided here.
    m_spCullingProcess->Cull(m_spCamera, m_spObject, &m_kVisibleSet);

    // The visible set will be empty if the object is culled.
    bool bIsCulled = (m_kVisibleSet.GetCount() == 0);

    // Cache this frame's ID and return value.
    m_uiLastFrameID = uiFrameID;
    m_bLastReturnValue = (bIsCulled ? m_bReturnOnCulled : !m_bReturnOnCulled);
    m_bForceUpdate = false;

    // Clear out visible set.
    m_kVisibleSet.RemoveAll();

    return m_bLastReturnValue;
}
//---------------------------------------------------------------------------
