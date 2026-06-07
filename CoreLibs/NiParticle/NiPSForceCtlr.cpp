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

#include "NiPSForceCtlr.h"
#include "NiPSParticleSystem.h"
#include "NiPSSimulatorForcesStep.h"

NiImplementRTTI(NiPSForceCtlr, NiSingleInterpController);

//---------------------------------------------------------------------------
NiPSForceCtlr::NiPSForceCtlr(const NiFixedString& kForceName) : m_pkForce(NULL)
{
    SetForceName(kForceName);
}
//---------------------------------------------------------------------------
NiPSForceCtlr::NiPSForceCtlr() : m_pkForce(NULL)
{
}
//---------------------------------------------------------------------------
void NiPSForceCtlr::SetTarget(NiObjectNET* pkTarget)
{
    // Clear the Force pointer.
    m_pkForce = NULL;

    // Call the base class SetTarget.
    NiSingleInterpController::SetTarget(pkTarget);

    // If the target was set to something other than NULL, retreive the
    // modifier pointer using the name.
    if (m_pkTarget)
    {
        GetForcePointerFromName();
    }
}
//---------------------------------------------------------------------------
const char* NiPSForceCtlr::GetCtlrID() 
{
    return m_kForceName;
}
//---------------------------------------------------------------------------
bool NiPSForceCtlr::InterpTargetIsCorrectType(NiObjectNET* pkTarget) 
    const
{  
    if (!NiIsKindOf(NiPSParticleSystem, pkTarget) || !m_kForceName.Exists())
    {
        return false;
    }

    if (m_pkTarget)
    {
        NiPSSimulator* pkSimulator = ((NiPSParticleSystem*) pkTarget)
            ->GetSimulator();
        if (!pkSimulator)
        {
            return false;
        }

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
        if (!pkForcesStep)
        {
            return false;
        }

        if (pkForcesStep->GetForceByName(m_kForceName))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void NiPSForceCtlr::GetForcePointerFromName()
{
    NIASSERT(m_pkTarget && m_kForceName.Exists());
    NiPSSimulator* pkSimulator = ((NiPSParticleSystem*) m_pkTarget)
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
    m_pkForce = pkForcesStep->GetForceByName(m_kForceName);
    NIASSERT(m_pkForce);
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiPSForceCtlr::CopyMembers(NiPSForceCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiSingleInterpController::CopyMembers(pkDest, kCloning);

    pkDest->m_kForceName = m_kForceName;
}
//---------------------------------------------------------------------------
void NiPSForceCtlr::ProcessClone(NiCloningProcess& kCloning)
{
    NiSingleInterpController::ProcessClone(kCloning);

    NiObject* pkObject = NULL;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkObject));
    NiPSForceCtlr* pkDest = (NiPSForceCtlr*) pkObject;

    // The force pointer should exist at this point and should have been
    // cloned.
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(m_pkForce, pkObject));
    pkDest->m_pkForce = (NiPSForce*) pkObject;
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiPSForceCtlr::LoadBinary(NiStream& kStream)
{
    NiSingleInterpController::LoadBinary(kStream);

    kStream.LoadFixedString(m_kForceName);
}
//---------------------------------------------------------------------------
void NiPSForceCtlr::LinkObject(NiStream& kStream)
{
    NiSingleInterpController::LinkObject(kStream);
}
//---------------------------------------------------------------------------
void NiPSForceCtlr::PostLinkObject(NiStream& kStream)
{
    NiSingleInterpController::PostLinkObject(kStream);

    if (m_pkTarget)
    {
        GetForcePointerFromName();
    }
}
//---------------------------------------------------------------------------
bool NiPSForceCtlr::RegisterStreamables(NiStream& kStream)
{
    if (!NiSingleInterpController::RegisterStreamables(kStream))
    {
        return false;
    }

    kStream.RegisterFixedString(m_kForceName);

    return true;
}
//---------------------------------------------------------------------------
void NiPSForceCtlr::SaveBinary(NiStream& kStream)
{
    NiSingleInterpController::SaveBinary(kStream);

    kStream.SaveFixedString(m_kForceName);
}
//---------------------------------------------------------------------------
bool NiPSForceCtlr::IsEqual(NiObject* pkObject)
{
    if (!NiSingleInterpController::IsEqual(pkObject))
    {
        return false;
    }

    NiPSForceCtlr* pkDest = (NiPSForceCtlr*) pkObject;

    if (pkDest->m_kForceName != m_kForceName)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSForceCtlr::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiSingleInterpController::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSForceCtlr::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("Force Name", m_kForceName));
}
//---------------------------------------------------------------------------
