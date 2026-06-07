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

#include "NiPSAirFieldInheritedVelocityCtlr.h"
#include "NiPSParticleSystem.h"
#include "NiPSSimulatorForcesStep.h"
#include "NiPSAirFieldForce.h"

NiImplementRTTI(NiPSAirFieldInheritedVelocityCtlr, NiPSForceFloatCtlr);

//---------------------------------------------------------------------------
NiPSAirFieldInheritedVelocityCtlr::NiPSAirFieldInheritedVelocityCtlr(
    const NiFixedString& kForceName) :
    NiPSForceFloatCtlr(kForceName)
{
}
//---------------------------------------------------------------------------
NiPSAirFieldInheritedVelocityCtlr::NiPSAirFieldInheritedVelocityCtlr() 
{
}
//---------------------------------------------------------------------------
bool NiPSAirFieldInheritedVelocityCtlr::InterpTargetIsCorrectType(
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

    return NiIsKindOf(NiPSAirFieldForce, pkForce);
}
//---------------------------------------------------------------------------
void NiPSAirFieldInheritedVelocityCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = ((NiPSAirFieldForce*) m_pkForce)->GetInheritedVelocity();
}
//---------------------------------------------------------------------------
void NiPSAirFieldInheritedVelocityCtlr::SetTargetFloatValue(float fValue)
{
    ((NiPSAirFieldForce*) m_pkForce)->SetInheritedVelocity(fValue);
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSAirFieldInheritedVelocityCtlr);
//---------------------------------------------------------------------------
void NiPSAirFieldInheritedVelocityCtlr::CopyMembers(
    NiPSAirFieldInheritedVelocityCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSForceFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSAirFieldInheritedVelocityCtlr);
//---------------------------------------------------------------------------
void NiPSAirFieldInheritedVelocityCtlr::LoadBinary(NiStream& kStream)
{
    NiPSForceFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSAirFieldInheritedVelocityCtlr::LinkObject(NiStream& kStream)
{
    NiPSForceFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSAirFieldInheritedVelocityCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSForceFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSAirFieldInheritedVelocityCtlr::SaveBinary(NiStream& kStream)
{
    NiPSForceFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSAirFieldInheritedVelocityCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSForceFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSAirFieldInheritedVelocityCtlr::GetViewerStrings(NiViewerStringsArray* 
    pkStrings)
{
    NiPSForceFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSAirFieldInheritedVelocityCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
