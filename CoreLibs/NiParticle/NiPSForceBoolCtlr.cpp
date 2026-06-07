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

#include "NiPSForceBoolCtlr.h"
#include <NiBoolInterpolator.h>
#include <NiConstBoolEvaluator.h>
#include <NiBlendBoolInterpolator.h>

NiImplementRTTI(NiPSForceBoolCtlr, NiPSForceCtlr);

//---------------------------------------------------------------------------
NiPSForceBoolCtlr::NiPSForceBoolCtlr(const NiFixedString& kForceName) :
    NiPSForceCtlr(kForceName)
{
}
//---------------------------------------------------------------------------
NiPSForceBoolCtlr::NiPSForceBoolCtlr()
{
}
//---------------------------------------------------------------------------
NiEvaluator* NiPSForceBoolCtlr::CreatePoseEvaluator(
#ifdef NIDEBUG
    unsigned short usIndex)
#else
    unsigned short)
#endif
{
    NIASSERT(usIndex == 0);
    bool bValue;
    GetTargetBoolValue(bValue);
    float fValue = bValue ? 1.0f : 0.0f;
    return NiNew NiConstBoolEvaluator(fValue);
}
//---------------------------------------------------------------------------
NiInterpolator* NiPSForceBoolCtlr::CreatePoseInterpolator(
    unsigned short usIndex)
{
    NI_UNUSED_ARG(usIndex);
    NIASSERT(usIndex == 0);
    bool bValue;
    GetTargetBoolValue(bValue);
    return NiNew NiBoolInterpolator(bValue);
}
//---------------------------------------------------------------------------
void NiPSForceBoolCtlr::SynchronizePoseInterpolator(NiInterpolator* 
    pkInterp,
    unsigned short usIndex)
{
    NI_UNUSED_ARG(usIndex);
    NiBoolInterpolator* pkBoolInterp = 
        NiDynamicCast(NiBoolInterpolator, pkInterp);

    NIASSERT(usIndex == 0);
    NIASSERT(pkBoolInterp);
    bool bValue;
    GetTargetBoolValue(bValue);
    pkBoolInterp->SetPoseValue(bValue);
}
//---------------------------------------------------------------------------
NiBlendInterpolator* NiPSForceBoolCtlr::CreateBlendInterpolator(
    unsigned short usIndex,
    bool bManagerControlled,
    float fWeightThreshold,
    unsigned char ucArraySize) const
{
    NI_UNUSED_ARG(usIndex);
    NIASSERT(usIndex == 0);
    return NiNew NiBlendBoolInterpolator(bManagerControlled, fWeightThreshold,
        ucArraySize);
}
//---------------------------------------------------------------------------
void NiPSForceBoolCtlr::Update(float fTime)
{
    if (GetManagerControlled())
    {
        return;
    }

    if (DontDoUpdate(fTime) &&
        (!m_spInterpolator || !m_spInterpolator->AlwaysUpdate()))
    {
        return;
    }

    if (m_spInterpolator)
    {
        bool bValue;
        if (m_spInterpolator->Update(m_fScaledTime, m_pkTarget, bValue))
        {
            SetTargetBoolValue(bValue);
        }
    }
}
//---------------------------------------------------------------------------
bool NiPSForceBoolCtlr::UpdateValue(float fTime, bool bBool, 
    unsigned short usIndex)
{
    NI_UNUSED_ARG(fTime);
    NI_UNUSED_ARG(usIndex);
    NIASSERT(GetManagerControlled());
    SetTargetBoolValue(bBool);
    return true;
}
//---------------------------------------------------------------------------
bool NiPSForceBoolCtlr::InterpolatorIsCorrectType(
    NiInterpolator* pkInterpolator, 
    unsigned short usIndex) const
{
    NI_UNUSED_ARG(usIndex);
    NIASSERT(usIndex == 0);
    return pkInterpolator->IsBoolValueSupported();
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiPSForceBoolCtlr::CopyMembers(NiPSForceBoolCtlr* pkDest,
    NiCloningProcess& kCloning)
{
    NiPSForceCtlr::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiPSForceBoolCtlr::LoadBinary(NiStream& kStream)
{
    NiPSForceCtlr::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiPSForceBoolCtlr::LinkObject(NiStream& kStream)
{
    NiPSForceCtlr::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPSForceBoolCtlr::RegisterStreamables(NiStream& kStream)
{
    return NiPSForceCtlr::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiPSForceBoolCtlr::SaveBinary(NiStream& kStream)
{
    NiPSForceCtlr::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiPSForceBoolCtlr::IsEqual(NiObject* pkObject)
{
    return NiPSForceCtlr::IsEqual(pkObject);
}
//---------------------------------------------------------------------------
// Viewer Strings
//---------------------------------------------------------------------------
void NiPSForceBoolCtlr::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiPSForceCtlr::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPSForceBoolCtlr::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
