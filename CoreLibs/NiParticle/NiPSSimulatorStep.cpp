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

#include "NiPSSimulatorStep.h"

NiImplementRTTI(NiPSSimulatorStep, NiObject);

//---------------------------------------------------------------------------
void NiPSSimulatorStep::Detach(NiPSParticleSystem*)
{
    // Clear out input and output streams and semantics arrays. This function
    // should be called by all derived classes to ensure that this deletion
    // occurs.
    m_kInputSemantics.RemoveAll();
    m_kInputStreams.RemoveAll();
    m_kOutputSemantics.RemoveAll();
    m_kOutputStreams.RemoveAll();
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiPSSimulatorStep::CopyMembers(
    NiPSSimulatorStep* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiPSSimulatorStep::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSSimulatorStep::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSSimulatorStep::RegisterStreamables(NiStream& kStream)
{
    return NiObject::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSSimulatorStep::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSSimulatorStep::IsEqual(NiObject* pkObject)
{
    return NiObject::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSSimulatorStep::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSSimulatorStep::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
