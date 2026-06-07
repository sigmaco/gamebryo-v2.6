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

#include "NiPSEmitterRotAngleVarCtlr.h"

NiImplementRTTI(NiPSEmitterRotAngleVarCtlr, NiPSEmitterFloatCtlr);

//---------------------------------------------------------------------------
NiPSEmitterRotAngleVarCtlr::NiPSEmitterRotAngleVarCtlr(
    const NiFixedString& kEmitterName) :
    NiPSEmitterFloatCtlr(kEmitterName)
{
}
//---------------------------------------------------------------------------
NiPSEmitterRotAngleVarCtlr::NiPSEmitterRotAngleVarCtlr() 
{
}
//---------------------------------------------------------------------------
void NiPSEmitterRotAngleVarCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = m_pkEmitter->GetRotAngleVar();
}
//---------------------------------------------------------------------------
void NiPSEmitterRotAngleVarCtlr::SetTargetFloatValue(float fValue)
{
    m_pkEmitter->SetRotAngleVar(fValue);
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSEmitterRotAngleVarCtlr);
//---------------------------------------------------------------------------
void NiPSEmitterRotAngleVarCtlr::CopyMembers(
    NiPSEmitterRotAngleVarCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSEmitterFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSEmitterRotAngleVarCtlr);
//---------------------------------------------------------------------------
void NiPSEmitterRotAngleVarCtlr::LoadBinary(NiStream& kStream)
{
    NiPSEmitterFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSEmitterRotAngleVarCtlr::LinkObject(NiStream& kStream)
{
    NiPSEmitterFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSEmitterRotAngleVarCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSEmitterFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSEmitterRotAngleVarCtlr::SaveBinary(NiStream& kStream)
{
    NiPSEmitterFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSEmitterRotAngleVarCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSEmitterFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSEmitterRotAngleVarCtlr::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPSEmitterFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSEmitterRotAngleVarCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
