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

#include "NiPSAirFieldSpreadCtlr.h"
#include "NiPSParticleSystem.h"
#include "NiPSSimulatorForcesStep.h"
#include "NiPSAirFieldForce.h"

NiImplementRTTI(NiPSAirFieldSpreadCtlr, NiPSForceFloatCtlr);

//---------------------------------------------------------------------------
NiPSAirFieldSpreadCtlr::NiPSAirFieldSpreadCtlr(
    const NiFixedString& kForceName) :
    NiPSForceFloatCtlr(kForceName)
{
}
//---------------------------------------------------------------------------
NiPSAirFieldSpreadCtlr::NiPSAirFieldSpreadCtlr() 
{
}
//---------------------------------------------------------------------------
bool NiPSAirFieldSpreadCtlr::InterpTargetIsCorrectType(
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
void NiPSAirFieldSpreadCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = ((NiPSAirFieldForce*) m_pkForce)->GetSpread();
}
//---------------------------------------------------------------------------
void NiPSAirFieldSpreadCtlr::SetTargetFloatValue(float fValue)
{
    ((NiPSAirFieldForce*) m_pkForce)->SetSpread(fValue);
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSAirFieldSpreadCtlr);
//---------------------------------------------------------------------------
void NiPSAirFieldSpreadCtlr::CopyMembers(
    NiPSAirFieldSpreadCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSForceFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSAirFieldSpreadCtlr);
//---------------------------------------------------------------------------
void NiPSAirFieldSpreadCtlr::LoadBinary(NiStream& kStream)
{
    NiPSForceFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSAirFieldSpreadCtlr::LinkObject(NiStream& kStream)
{
    NiPSForceFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSAirFieldSpreadCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSForceFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSAirFieldSpreadCtlr::SaveBinary(NiStream& kStream)
{
    NiPSForceFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSAirFieldSpreadCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSForceFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSAirFieldSpreadCtlr::GetViewerStrings(NiViewerStringsArray* 
    pkStrings)
{
    NiPSForceFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSAirFieldSpreadCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
