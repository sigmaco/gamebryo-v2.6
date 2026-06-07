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

#include "NiPSEmitterRotAngleCtlr.h"

NiImplementRTTI(NiPSEmitterRotAngleCtlr, NiPSEmitterFloatCtlr);

//---------------------------------------------------------------------------
NiPSEmitterRotAngleCtlr::NiPSEmitterRotAngleCtlr(
    const NiFixedString& kEmitterName) :
    NiPSEmitterFloatCtlr(kEmitterName)
{
}
//---------------------------------------------------------------------------
NiPSEmitterRotAngleCtlr::NiPSEmitterRotAngleCtlr() 
{
}
//---------------------------------------------------------------------------
void NiPSEmitterRotAngleCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = m_pkEmitter->GetRotAngle();
}
//---------------------------------------------------------------------------
void NiPSEmitterRotAngleCtlr::SetTargetFloatValue(float fValue)
{
    m_pkEmitter->SetRotAngle(fValue);
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSEmitterRotAngleCtlr);
//---------------------------------------------------------------------------
void NiPSEmitterRotAngleCtlr::CopyMembers(
    NiPSEmitterRotAngleCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSEmitterFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSEmitterRotAngleCtlr);
//---------------------------------------------------------------------------
void NiPSEmitterRotAngleCtlr::LoadBinary(NiStream& kStream)
{
    NiPSEmitterFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSEmitterRotAngleCtlr::LinkObject(NiStream& kStream)
{
    NiPSEmitterFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSEmitterRotAngleCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSEmitterFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSEmitterRotAngleCtlr::SaveBinary(NiStream& kStream)
{
    NiPSEmitterFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSEmitterRotAngleCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSEmitterFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSEmitterRotAngleCtlr::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPSEmitterFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSEmitterRotAngleCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
