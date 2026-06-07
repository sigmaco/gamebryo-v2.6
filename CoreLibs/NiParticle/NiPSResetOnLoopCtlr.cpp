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
#include "NiParticlePCH.h"

#include "NiPSResetOnLoopCtlr.h"
#include "NiPSParticleSystem.h"

NiImplementRTTI(NiPSResetOnLoopCtlr, NiTimeController);

//---------------------------------------------------------------------------
NiPSResetOnLoopCtlr::NiPSResetOnLoopCtlr() :
    m_fLastScaledTime(-NI_INFINITY)
{
}
//---------------------------------------------------------------------------
void NiPSResetOnLoopCtlr::Update(float fTime)
{
    SetCycleType(LOOP);

    if (!m_pkTarget || DontDoUpdate(fTime))
    {
        return;
    }

    if (m_fLastScaledTime == -NI_INFINITY)
    {
        m_fLastScaledTime = m_fScaledTime;
    }

    if (m_fLastScaledTime > m_fScaledTime)
    {
        // Controller has looped, so reset particle system.
        ((NiPSParticleSystem*) m_pkTarget)->ResetParticleSystem();
    }

    m_fLastScaledTime = m_fScaledTime;
}
//---------------------------------------------------------------------------
bool NiPSResetOnLoopCtlr::TargetIsRequiredType() const
{
    return NiIsKindOf(NiPSParticleSystem, m_pkTarget);
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSResetOnLoopCtlr);
//---------------------------------------------------------------------------
void NiPSResetOnLoopCtlr::CopyMembers(NiPSResetOnLoopCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiTimeController::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSResetOnLoopCtlr);
//---------------------------------------------------------------------------
void NiPSResetOnLoopCtlr::LoadBinary(NiStream& kStream)
{
    NiTimeController::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSResetOnLoopCtlr::LinkObject(NiStream& kStream)
{
    NiTimeController::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSResetOnLoopCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiTimeController::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSResetOnLoopCtlr::SaveBinary(NiStream& kStream)
{
    NiTimeController::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSResetOnLoopCtlr::IsEqual(NiObject* pkObject)
{
    return NiTimeController::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSResetOnLoopCtlr::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiTimeController::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSResetOnLoopCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
