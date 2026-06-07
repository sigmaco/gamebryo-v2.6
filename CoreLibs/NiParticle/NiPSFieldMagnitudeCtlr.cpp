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

#include "NiPSFieldMagnitudeCtlr.h"
#include "NiPSParticleSystem.h"
#include "NiPSSimulatorForcesStep.h"
#include "NiPSFieldForce.h"

NiImplementRTTI(NiPSFieldMagnitudeCtlr, NiPSForceFloatCtlr);

//---------------------------------------------------------------------------
NiPSFieldMagnitudeCtlr::NiPSFieldMagnitudeCtlr(
    const NiFixedString& kForceName) :
    NiPSForceFloatCtlr(kForceName)
{
}
//---------------------------------------------------------------------------
NiPSFieldMagnitudeCtlr::NiPSFieldMagnitudeCtlr() 
{
}
//---------------------------------------------------------------------------
bool NiPSFieldMagnitudeCtlr::InterpTargetIsCorrectType(
    NiObjectNET* pkTarget) const
{
    if (!NiPSForceFloatCtlr::InterpTargetIsCorrectType(pkTarget))
    {
        return false;
    }

    NiPSSimulator* pkSimulator = ((NiPSParticleSystem*) pkTarget)
        ->GetSimulator();
    NIASSERT(pkSimulator);
    NiPSSimulatorForcesStep* pkForcesStep = NULL;
    for (NiUInt32 ui = 0; ui < pkSimulator->GetStepCount(); ++ui)
    {
        pkForcesStep = NiDynamicCast(NiPSSimulatorForcesStep,
            pkSimulator->GetStepAt(ui));
        if (pkForcesStep)
        {
            break;
        }
    }
    NIASSERT(pkForcesStep);
    NiPSForce* pkForce = pkForcesStep->GetForceByName(m_kForceName);
    NIASSERT(pkForce);

    return NiIsKindOf(NiPSFieldForce, pkForce);
}
//---------------------------------------------------------------------------
void NiPSFieldMagnitudeCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = ((NiPSFieldForce*) m_pkForce)->GetMagnitude();
}
//---------------------------------------------------------------------------
void NiPSFieldMagnitudeCtlr::SetTargetFloatValue(float fValue)
{
    ((NiPSFieldForce*) m_pkForce)->SetMagnitude(fValue);
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSFieldMagnitudeCtlr);
//---------------------------------------------------------------------------
void NiPSFieldMagnitudeCtlr::CopyMembers(NiPSFieldMagnitudeCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSForceFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSFieldMagnitudeCtlr);
//---------------------------------------------------------------------------
void NiPSFieldMagnitudeCtlr::LoadBinary(NiStream& kStream)
{
    NiPSForceFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSFieldMagnitudeCtlr::LinkObject(NiStream& kStream)
{
    NiPSForceFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSFieldMagnitudeCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSForceFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSFieldMagnitudeCtlr::SaveBinary(NiStream& kStream)
{
    NiPSForceFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSFieldMagnitudeCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSForceFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSFieldMagnitudeCtlr::GetViewerStrings(NiViewerStringsArray* 
    pkStrings)
{
    NiPSForceFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSFieldMagnitudeCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
