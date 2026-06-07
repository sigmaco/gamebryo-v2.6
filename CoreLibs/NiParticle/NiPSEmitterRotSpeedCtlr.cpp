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

#include "NiPSEmitterRotSpeedCtlr.h"

NiImplementRTTI(NiPSEmitterRotSpeedCtlr, NiPSEmitterFloatCtlr);

//---------------------------------------------------------------------------
NiPSEmitterRotSpeedCtlr::NiPSEmitterRotSpeedCtlr(
    const NiFixedString& kEmitterName) :
    NiPSEmitterFloatCtlr(kEmitterName)
{
}
//---------------------------------------------------------------------------
NiPSEmitterRotSpeedCtlr::NiPSEmitterRotSpeedCtlr() 
{
}
//---------------------------------------------------------------------------
void NiPSEmitterRotSpeedCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = m_pkEmitter->GetRotSpeed();
}
//---------------------------------------------------------------------------
void NiPSEmitterRotSpeedCtlr::SetTargetFloatValue(float fValue)
{
    m_pkEmitter->SetRotSpeed(fValue);
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSEmitterRotSpeedCtlr);
//---------------------------------------------------------------------------
void NiPSEmitterRotSpeedCtlr::CopyMembers(
    NiPSEmitterRotSpeedCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSEmitterFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSEmitterRotSpeedCtlr);
//---------------------------------------------------------------------------
void NiPSEmitterRotSpeedCtlr::LoadBinary(NiStream& kStream)
{
    NiPSEmitterFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSEmitterRotSpeedCtlr::LinkObject(NiStream& kStream)
{
    NiPSEmitterFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSEmitterRotSpeedCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSEmitterFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSEmitterRotSpeedCtlr::SaveBinary(NiStream& kStream)
{
    NiPSEmitterFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSEmitterRotSpeedCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSEmitterFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSEmitterRotSpeedCtlr::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPSEmitterFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSEmitterRotSpeedCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
