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

#include "NiPSEmitterCtlr.h"

NiImplementRTTI(NiPSEmitterCtlr, NiSingleInterpController);

//---------------------------------------------------------------------------
NiPSEmitterCtlr::NiPSEmitterCtlr(const NiFixedString& kEmitterName) :
    m_pkEmitter(NULL)
{
    SetEmitterName(kEmitterName);
}
//---------------------------------------------------------------------------
NiPSEmitterCtlr::NiPSEmitterCtlr() : m_pkEmitter(NULL)
{
}
//---------------------------------------------------------------------------
void NiPSEmitterCtlr::SetTarget(NiObjectNET* pkTarget)
{
    // Clear the emitter pointer.
    m_pkEmitter = NULL;

    // Call the base class SetTarget.
    NiSingleInterpController::SetTarget(pkTarget);

    // If the target was set to something other than NULL, retreive the
    // modifier pointer using the name.
    if (m_pkTarget)
    {
        GetEmitterPointerFromName();
    }
}
//---------------------------------------------------------------------------
const char* NiPSEmitterCtlr::GetCtlrID() 
{
    return m_kEmitterName;
}
//---------------------------------------------------------------------------
bool NiPSEmitterCtlr::InterpTargetIsCorrectType(NiObjectNET* pkTarget) 
    const
{  
    if (!NiIsKindOf(NiPSParticleSystem, pkTarget) || !m_kEmitterName.Exists())
    {
        return false;
    }

    if (m_pkTarget)
    {
        if (((NiPSParticleSystem*) pkTarget)->GetEmitterByName(m_kEmitterName))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiPSEmitterCtlr::CopyMembers(NiPSEmitterCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiSingleInterpController::CopyMembers(pkDest, kCloning);

    pkDest->m_kEmitterName = m_kEmitterName;
}
//---------------------------------------------------------------------------
void NiPSEmitterCtlr::ProcessClone(NiCloningProcess& kCloning)
{
    NiSingleInterpController::ProcessClone(kCloning);

    NiObject* pkObject = NULL;
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(this, pkObject));
    NiPSEmitterCtlr* pkDest = (NiPSEmitterCtlr*) pkObject;

    // The emitter pointer should exist at this point and should have been
    // cloned.
    NIVERIFY(kCloning.m_pkCloneMap->GetAt(m_pkEmitter, pkObject));
    pkDest->m_pkEmitter = (NiPSEmitter*) pkObject;
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiPSEmitterCtlr::LoadBinary(NiStream& kStream)
{
    NiSingleInterpController::LoadBinary(kStream);

    kStream.LoadFixedString(m_kEmitterName);
}
//---------------------------------------------------------------------------
void NiPSEmitterCtlr::LinkObject(NiStream& kStream)
{
    NiSingleInterpController::LinkObject(kStream);
}
//---------------------------------------------------------------------------
void NiPSEmitterCtlr::PostLinkObject(NiStream& kStream)
{
    NiSingleInterpController::PostLinkObject(kStream);

    if (m_pkTarget)
    {
        GetEmitterPointerFromName();
    }
}
//---------------------------------------------------------------------------
bool NiPSEmitterCtlr::RegisterStreamables(NiStream& kStream)
{
    if (!NiSingleInterpController::RegisterStreamables(kStream))
    {
        return false;
    }

    kStream.RegisterFixedString(m_kEmitterName);

    return true;
}
//---------------------------------------------------------------------------
void NiPSEmitterCtlr::SaveBinary(NiStream& kStream)
{
    NiSingleInterpController::SaveBinary(kStream);

    kStream.SaveFixedString(m_kEmitterName);
}
//---------------------------------------------------------------------------
bool NiPSEmitterCtlr::IsEqual(NiObject* pkObject)
{
    if (!NiSingleInterpController::IsEqual(pkObject))
    {
        return false;
    }

    NiPSEmitterCtlr* pkDest = (NiPSEmitterCtlr*) pkObject;

    if (pkDest->m_kEmitterName != m_kEmitterName)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPSEmitterCtlr::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiSingleInterpController::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSEmitterCtlr::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("Emitter Name", m_kEmitterName));
}
//---------------------------------------------------------------------------
