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

// NifRenderViewCameraZoomState.h

#include "stdafx.h"
#include "NifRenderViewCameraZoomState.h"
#include "NifRenderView.h"
#include "NifDoc.h"

//---------------------------------------------------------------------------
CNifRenderViewCameraZoomState::CNifRenderViewCameraZoomState(
    CNifRenderView* pkView) : CNifRenderViewCameraTranslationState(pkView)
{
}
//---------------------------------------------------------------------------
CNifRenderViewCameraZoomState::~CNifRenderViewCameraZoomState()
{
    m_pkCameraInfo = NULL;
}
//---------------------------------------------------------------------------
void CNifRenderViewCameraZoomState::UpdateDevices()
{ 
    CNifRenderViewBufferedUIState::UpdateDevices();
    if(m_bTrackingMouse)
    {
        CPoint kDelta = m_kLastPoint - m_kBeginPoint;
        kDelta = -kDelta;
        NiPoint2 kBounds = m_pkView->GetScreenBounds();

        m_kDeltaTranslate += NiPoint3::UNIT_X * ((float)kDelta.y) / 
            kBounds.y * 100.0f * 0.1f * m_fSceneScale;
        
        m_kBeginPoint = m_kLastPoint;
    }

    if(m_fWheelDelta != 0.0f)
    {
        m_kDeltaTranslate += NiPoint3::UNIT_X * m_fWheelDelta * 0.1f *
            m_fSceneScale;
    }
}
//---------------------------------------------------------------------------

bool CNifRenderViewCameraZoomState::IsAxisConstraintAllowed(UIAxisConstraint
                                                            eMode)
{
    return false;
}