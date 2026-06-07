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
#include "NiUserInterfacePCH.h"

#include "NiNavInterpToController.h"

#include <NiNode.h>
#include "NiNavManager.h"

//---------------------------------------------------------------------------
NiNavInterpToController::NiNavInterpToController(
    NiAVObject* pkInterpToObject, 
    NiAVObject* pkControlledObject, 
    const char* pcName,
    float fInterpTime
    )
    : NiNavBaseController(pcName),
    m_spInterpToObject(pkInterpToObject),
    m_spControlledObject(pkControlledObject),
    m_kNavState(),
    m_fTime(0),
    m_fInterpTime(fInterpTime),
    m_kStartingTransform()
{
}
//---------------------------------------------------------------------------
NiNavInterpToController::~NiNavInterpToController()
{
    m_spInterpToObject = NULL;
    m_spControlledObject = NULL;
}
//---------------------------------------------------------------------------
inline void NiNavInterpToController::SetInterpToObject(
    NiAVObject* pkInterpToObject)
{
    m_spInterpToObject = pkInterpToObject;
}
//---------------------------------------------------------------------------
inline void NiNavInterpToController::SetControlledObject(
    NiAVObject* pkControlledObject)
{
    m_spControlledObject = pkControlledObject;
}
//---------------------------------------------------------------------------
const char* NiNavInterpToController::GetControlName(unsigned int) const
{
    NIASSERT(0); // NiNavInterpToController has no controls
    return NULL;
}
//---------------------------------------------------------------------------
void NiNavInterpToController::MouseInput(unsigned int, float)
{
}
//---------------------------------------------------------------------------
void NiNavInterpToController::GamePadInput(unsigned int, float)
{
}
//---------------------------------------------------------------------------
bool NiNavInterpToController::SetActive(bool bActive, NavState& kCurState)
{
    if (!NiNavBaseController::SetActive(bActive, kCurState))
        return false;

    m_bActive = bActive;

    if (m_bActive)
    {
        m_kNavState = kCurState;
        RestartInterpolation();
    }
    else
    {
        kCurState = m_kNavState;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiNavInterpToController::UpdatePose()
{
    if (!m_spControlledObject || !m_spInterpToObject)
        return;

    m_fTime += NiNavManager::GetNavManager()->GetFrameTime();

    float fInterp = NiClamp(m_fTime / m_fInterpTime, 0.f, 1.f);

    NiTransform kInterpolatedResult;
    NiTransform::Interpolate(
        fInterp,
        m_kStartingTransform,
        m_spInterpToObject->GetWorldTransform(),
        kInterpolatedResult);

    m_spControlledObject->SetLocalFromWorldTransform(kInterpolatedResult);
}
//---------------------------------------------------------------------------
void NiNavInterpToController::RestartInterpolation()
{
    if (!m_spControlledObject)
        return;

    m_kStartingTransform = m_spControlledObject->GetWorldTransform();
    m_fTime = 0;
}
//---------------------------------------------------------------------------
