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

#include "NiPSEmitterPlanarAngleCtlr.h"

NiImplementRTTI(NiPSEmitterPlanarAngleCtlr, NiPSEmitterFloatCtlr);

//---------------------------------------------------------------------------
NiPSEmitterPlanarAngleCtlr::NiPSEmitterPlanarAngleCtlr(
    const NiFixedString& kEmitterName) :
    NiPSEmitterFloatCtlr(kEmitterName)
{
}
//---------------------------------------------------------------------------
NiPSEmitterPlanarAngleCtlr::NiPSEmitterPlanarAngleCtlr() 
{
}
//---------------------------------------------------------------------------
void NiPSEmitterPlanarAngleCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = m_pkEmitter->GetPlanarAngle();
}
//---------------------------------------------------------------------------
void NiPSEmitterPlanarAngleCtlr::SetTargetFloatValue(float fValue)
{
    m_pkEmitter->SetPlanarAngle(fValue);
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSEmitterPlanarAngleCtlr);
//---------------------------------------------------------------------------
void NiPSEmitterPlanarAngleCtlr::CopyMembers(
    NiPSEmitterPlanarAngleCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSEmitterFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSEmitterPlanarAngleCtlr);
//---------------------------------------------------------------------------
void NiPSEmitterPlanarAngleCtlr::LoadBinary(NiStream& kStream)
{
    NiPSEmitterFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSEmitterPlanarAngleCtlr::LinkObject(NiStream& kStream)
{
    NiPSEmitterFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSEmitterPlanarAngleCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSEmitterFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSEmitterPlanarAngleCtlr::SaveBinary(NiStream& kStream)
{
    NiPSEmitterFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSEmitterPlanarAngleCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSEmitterFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSEmitterPlanarAngleCtlr::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPSEmitterFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSEmitterPlanarAngleCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
