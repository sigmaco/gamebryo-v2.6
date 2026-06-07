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

#include "NiPSForceActiveCtlr.h"
#include "NiPSForce.h"

NiImplementRTTI(NiPSForceActiveCtlr, NiPSForceBoolCtlr);

//---------------------------------------------------------------------------
NiPSForceActiveCtlr::NiPSForceActiveCtlr(const NiFixedString& kForceName) :
    NiPSForceBoolCtlr(kForceName)
{
}
//---------------------------------------------------------------------------
NiPSForceActiveCtlr::NiPSForceActiveCtlr()
{
}
//---------------------------------------------------------------------------
void NiPSForceActiveCtlr::GetTargetBoolValue(bool& bValue)
{
    NIASSERT(m_pkForce);
    bValue = m_pkForce->GetActive();
}
//---------------------------------------------------------------------------
void NiPSForceActiveCtlr::SetTargetBoolValue(bool bValue)
{
    NIASSERT(m_pkForce);
    if (m_pkForce->GetActive() != bValue)
    {
        m_pkForce->SetActive(bValue);
    }
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSForceActiveCtlr);
//---------------------------------------------------------------------------
void NiPSForceActiveCtlr::CopyMembers(NiPSForceActiveCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSForceBoolCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSForceActiveCtlr);
//---------------------------------------------------------------------------
void NiPSForceActiveCtlr::LoadBinary(NiStream& kStream)
{
    NiPSForceBoolCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSForceActiveCtlr::LinkObject(NiStream& kStream)
{
    NiPSForceBoolCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSForceActiveCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSForceBoolCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSForceActiveCtlr::SaveBinary(NiStream& kStream)
{
    NiPSForceBoolCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSForceActiveCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSForceBoolCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSForceActiveCtlr::GetViewerStrings(NiViewerStringsArray*
    pkStrings)
{
    NiPSForceBoolCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSForceActiveCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
