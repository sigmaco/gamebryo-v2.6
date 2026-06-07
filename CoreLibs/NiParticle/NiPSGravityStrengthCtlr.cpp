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

#include "NiPSGravityStrengthCtlr.h"
#include "NiPSParticleSystem.h"
#include "NiPSSimulatorForcesStep.h"
#include "NiPSGravityForce.h"

NiImplementRTTI(NiPSGravityStrengthCtlr, NiPSForceFloatCtlr);

//---------------------------------------------------------------------------
NiPSGravityStrengthCtlr::NiPSGravityStrengthCtlr(
    const NiFixedString& kForceName) :
    NiPSForceFloatCtlr(kForceName)
{
}
//---------------------------------------------------------------------------
NiPSGravityStrengthCtlr::NiPSGravityStrengthCtlr() 
{
}
//---------------------------------------------------------------------------
bool NiPSGravityStrengthCtlr::InterpTargetIsCorrectType(
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

    return NiIsKindOf(NiPSGravityForce, pkForce);
}
//---------------------------------------------------------------------------
void NiPSGravityStrengthCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = ((NiPSGravityForce*) m_pkForce)->GetStrength();
}
//---------------------------------------------------------------------------
void NiPSGravityStrengthCtlr::SetTargetFloatValue(float fValue)
{
    ((NiPSGravityForce*) m_pkForce)->SetStrength(fValue);
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSGravityStrengthCtlr);
//---------------------------------------------------------------------------
void NiPSGravityStrengthCtlr::CopyMembers(NiPSGravityStrengthCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSForceFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSGravityStrengthCtlr);
//---------------------------------------------------------------------------
void NiPSGravityStrengthCtlr::LoadBinary(NiStream& kStream)
{
    NiPSForceFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSGravityStrengthCtlr::LinkObject(NiStream& kStream)
{
    NiPSForceFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSGravityStrengthCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSForceFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSGravityStrengthCtlr::SaveBinary(NiStream& kStream)
{
    NiPSForceFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSGravityStrengthCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSForceFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSGravityStrengthCtlr::GetViewerStrings(NiViewerStringsArray* 
    pkStrings)
{
    NiPSForceFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSGravityStrengthCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
