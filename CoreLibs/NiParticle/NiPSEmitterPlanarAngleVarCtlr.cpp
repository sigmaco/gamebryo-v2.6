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

#include "NiPSEmitterPlanarAngleVarCtlr.h"

NiImplementRTTI(NiPSEmitterPlanarAngleVarCtlr, NiPSEmitterFloatCtlr);

//---------------------------------------------------------------------------
NiPSEmitterPlanarAngleVarCtlr::NiPSEmitterPlanarAngleVarCtlr(
    const NiFixedString& kEmitterName) :
    NiPSEmitterFloatCtlr(kEmitterName)
{
}
//---------------------------------------------------------------------------
NiPSEmitterPlanarAngleVarCtlr::NiPSEmitterPlanarAngleVarCtlr() 
{
}
//---------------------------------------------------------------------------
void NiPSEmitterPlanarAngleVarCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = m_pkEmitter->GetPlanarAngleVar();
}
//---------------------------------------------------------------------------
void NiPSEmitterPlanarAngleVarCtlr::SetTargetFloatValue(float fValue)
{
    m_pkEmitter->SetPlanarAngleVar(fValue);
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSEmitterPlanarAngleVarCtlr);
//---------------------------------------------------------------------------
void NiPSEmitterPlanarAngleVarCtlr::CopyMembers(
    NiPSEmitterPlanarAngleVarCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSEmitterFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSEmitterPlanarAngleVarCtlr);
//---------------------------------------------------------------------------
void NiPSEmitterPlanarAngleVarCtlr::LoadBinary(NiStream& kStream)
{
    NiPSEmitterFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSEmitterPlanarAngleVarCtlr::LinkObject(NiStream& kStream)
{
    NiPSEmitterFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSEmitterPlanarAngleVarCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSEmitterFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSEmitterPlanarAngleVarCtlr::SaveBinary(NiStream& kStream)
{
    NiPSEmitterFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSEmitterPlanarAngleVarCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSEmitterFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSEmitterPlanarAngleVarCtlr::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPSEmitterFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSEmitterPlanarAngleVarCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
