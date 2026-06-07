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

#include "NiPSFieldAttenuationCtlr.h"
#include "NiPSParticleSystem.h"
#include "NiPSSimulatorForcesStep.h"
#include "NiPSFieldForce.h"

NiImplementRTTI(NiPSFieldAttenuationCtlr, NiPSForceFloatCtlr);

//---------------------------------------------------------------------------
NiPSFieldAttenuationCtlr::NiPSFieldAttenuationCtlr(
    const NiFixedString& kForceName) :
    NiPSForceFloatCtlr(kForceName)
{
}
//---------------------------------------------------------------------------
NiPSFieldAttenuationCtlr::NiPSFieldAttenuationCtlr() 
{
}
//---------------------------------------------------------------------------
bool NiPSFieldAttenuationCtlr::InterpTargetIsCorrectType(
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
void NiPSFieldAttenuationCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = ((NiPSFieldForce*) m_pkForce)->GetAttenuation();
}
//---------------------------------------------------------------------------
void NiPSFieldAttenuationCtlr::SetTargetFloatValue(float fValue)
{
    ((NiPSFieldForce*) m_pkForce)->SetAttenuation(fValue);
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSFieldAttenuationCtlr);
//---------------------------------------------------------------------------
void NiPSFieldAttenuationCtlr::CopyMembers(NiPSFieldAttenuationCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSForceFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSFieldAttenuationCtlr);
//---------------------------------------------------------------------------
void NiPSFieldAttenuationCtlr::LoadBinary(NiStream& kStream)
{
    NiPSForceFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSFieldAttenuationCtlr::LinkObject(NiStream& kStream)
{
    NiPSForceFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSFieldAttenuationCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSForceFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSFieldAttenuationCtlr::SaveBinary(NiStream& kStream)
{
    NiPSForceFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSFieldAttenuationCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSForceFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSFieldAttenuationCtlr::GetViewerStrings(NiViewerStringsArray* 
    pkStrings)
{
    NiPSForceFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSFieldAttenuationCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
