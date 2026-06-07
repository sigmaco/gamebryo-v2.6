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

#include "NiRenderView.h"

NiImplementRootRTTI(NiRenderView);

NiFixedString NiRenderView::ms_kDefaultName;

//---------------------------------------------------------------------------
const NiVisibleArray& NiRenderView::GetPVGeometry(unsigned int uiFrameID)
{
    // PV Geometry list needs to be recalculated if the cached frame ID is
    // not the same as the passed-in frame ID, or if the passed-in frame ID
    // is FORCE_PV_GEOMETRY_UPDATE. Note that this will automatically update
    // the PV list on the call after the one in which FORCE_PV_GEOMETRY_UPDATE 
    // was passed in, which is intended behavior.
    if (uiFrameID != m_uiLastFrameID || uiFrameID == FORCE_PV_GEOMETRY_UPDATE)
    {
        m_kCachedPVGeometry.RemoveAll();
        CalculatePVGeometry();
    }

    m_uiLastFrameID = uiFrameID;

    return m_kCachedPVGeometry;
}
//---------------------------------------------------------------------------
