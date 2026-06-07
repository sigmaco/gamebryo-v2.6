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

#include "NiPSEmitterLifeSpanCtlr.h"

NiImplementRTTI(NiPSEmitterLifeSpanCtlr, NiPSEmitterFloatCtlr);

//---------------------------------------------------------------------------
NiPSEmitterLifeSpanCtlr::NiPSEmitterLifeSpanCtlr(
    const NiFixedString& kEmitterName) :
    NiPSEmitterFloatCtlr(kEmitterName)
{
}
//---------------------------------------------------------------------------
NiPSEmitterLifeSpanCtlr::NiPSEmitterLifeSpanCtlr() 
{
}
//---------------------------------------------------------------------------
void NiPSEmitterLifeSpanCtlr::GetTargetFloatValue(float& fValue)
{
    fValue = m_pkEmitter->GetLifeSpan();
}
//---------------------------------------------------------------------------
void NiPSEmitterLifeSpanCtlr::SetTargetFloatValue(float fValue)
{
    m_pkEmitter->SetLifeSpan(fValue);
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPSEmitterLifeSpanCtlr);
//---------------------------------------------------------------------------
void NiPSEmitterLifeSpanCtlr::CopyMembers(
    NiPSEmitterLifeSpanCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSEmitterFloatCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPSEmitterLifeSpanCtlr);
//---------------------------------------------------------------------------
void NiPSEmitterLifeSpanCtlr::LoadBinary(NiStream& kStream)
{
    NiPSEmitterFloatCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSEmitterLifeSpanCtlr::LinkObject(NiStream& kStream)
{
    NiPSEmitterFloatCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSEmitterLifeSpanCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSEmitterFloatCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSEmitterLifeSpanCtlr::SaveBinary(NiStream& kStream)
{
    NiPSEmitterFloatCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSEmitterLifeSpanCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSEmitterFloatCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSEmitterLifeSpanCtlr::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiPSEmitterFloatCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSEmitterLifeSpanCtlr::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
