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
#include "NiAnimationPCH.h"

#include "NiColorInterpController.h"
#include "NiColorInterpolator.h"
#include "NiConstColorEvaluator.h"
#include "NiBlendColorInterpolator.h"
#include <NiColor.h>

NiImplementRTTI(NiColorInterpController, NiSingleInterpController);

//---------------------------------------------------------------------------
NiColorInterpController::NiColorInterpController()
{
}
//---------------------------------------------------------------------------
NiEvaluator* NiColorInterpController::CreatePoseEvaluator(
#ifdef NIDEBUG
    unsigned short usIndex)
#else
    unsigned short)
#endif
{
    NIASSERT(usIndex == 0);
    NiColorA kValue;
    GetTargetColorValue(kValue);
    return NiNew NiConstColorEvaluator(kValue);
}
//---------------------------------------------------------------------------
NiInterpolator* NiColorInterpController::CreatePoseInterpolator(
    unsigned short usIndex)
{
    NI_UNUSED_ARG(usIndex);
    NIASSERT(usIndex == 0);
    NiColorA kValue;
    GetTargetColorValue(kValue);
    return NiNew NiColorInterpolator(kValue);
}
//---------------------------------------------------------------------------
void NiColorInterpController::SynchronizePoseInterpolator(NiInterpolator* 
    pkInterp, 
    unsigned short usIndex)
{
    NI_UNUSED_ARG(usIndex);
    NiColorInterpolator* pkColorInterp = 
        NiDynamicCast(NiColorInterpolator, pkInterp);

    NIASSERT(usIndex == 0);
    NIASSERT(pkColorInterp);
    NiColorA kValue;
    GetTargetColorValue(kValue);
    pkColorInterp->SetPoseValue(kValue);
}   
//---------------------------------------------------------------------------
NiBlendInterpolator* NiColorInterpController::CreateBlendInterpolator(
    unsigned short, bool bManagerControlled,
    float fWeightThreshold,
    unsigned char ucArraySize) const
{
    return NiNew NiBlendColorInterpolator(bManagerControlled, fWeightThreshold,
        ucArraySize);
}
//---------------------------------------------------------------------------
bool NiColorInterpController::InterpolatorIsCorrectType(
    NiInterpolator* pkInterpolator, 
    unsigned short usIndex) const
{
    NI_UNUSED_ARG(usIndex);
    NIASSERT(usIndex == 0);
    return pkInterpolator->IsColorAValueSupported();
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiColorInterpController::CopyMembers(NiColorInterpController* pkDest,
    NiCloningProcess& kCloning)
{
    NiSingleInterpController::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiColorInterpController::LoadBinary(NiStream& kStream)
{
    NiSingleInterpController::LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiColorInterpController::LinkObject(NiStream& kStream)
{
    NiSingleInterpController::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiColorInterpController::RegisterStreamables(NiStream& kStream)
{
    return NiSingleInterpController::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiColorInterpController::SaveBinary(NiStream& kStream)
{
    NiSingleInterpController::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiColorInterpController::IsEqual(NiObject* pkObject)
{
    return NiSingleInterpController::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiColorInterpController::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiSingleInterpController::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiColorInterpController::ms_RTTI
        .GetName()));
}
//---------------------------------------------------------------------------
