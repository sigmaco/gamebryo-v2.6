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

#include "NiPSEmitterRotSpeedVarCtlr.h"

NiImplementRTTI(NiPSEmitterRotSpeedVarCtlr, NiPSEmitterFloatCtlr);

//---------------------------------------------------------------------------
NiPSEmitterRotSpeedVarCtlr::NiPSEmitterRotSpeedVarCtlr(
    const NiFixedString& kEmitterName) :
    NiPSEmitterFloatCtlr(kEmitterName)
{
}
//---------------------------------------------------------------------------
NiPSEmitterRotSpeedVarCtlr::NiPSEmitterRotSpeedVarCtlr() 
{
}
//---------------------------------------------------------------------------
void NiPSEmitterRotSpeedVarCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = m_pkEmitter->GetRotSpeedVar();
}
//---------------------------------------------------------------------------
void NiPSEmitterRotSpeedVarCtlr::SetTargetFloatValue(float fValue)
{
    m_pkEmitter->SetRotSpeedVar(fValue);
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSEmitterRotSpeedVarCtlr);
//---------------------------------------------------------------------------
void NiPSEmitterRotSpeedVarCtlr::CopyMembers(
    NiPSEmitterRotSpeedVarCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSEmitterFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSEmitterRotSpeedVarCtlr);
//---------------------------------------------------------------------------
void NiPSEmitterRotSpeedVarCtlr::LoadBinary(NiStream& kStream)
{
    NiPSEmitterFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSEmitterRotSpeedVarCtlr::LinkObject(NiStream& kStream)
{
    NiPSEmitterFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSEmitterRotSpeedVarCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSEmitterFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSEmitterRotSpeedVarCtlr::SaveBinary(NiStream& kStream)
{
    NiPSEmitterFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSEmitterRotSpeedVarCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSEmitterFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSEmitterRotSpeedVarCtlr::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPSEmitterFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSEmitterRotSpeedVarCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
