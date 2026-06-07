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
#include "NiAnimationConstants.h"
#include "NiMultiTargetTransformController.h"
#include <NiAVObject.h>

NiImplementRTTI(NiMultiTargetTransformController, NiInterpController);

//---------------------------------------------------------------------------
NiMultiTargetTransformController::~NiMultiTargetTransformController()
{
}
//---------------------------------------------------------------------------
void NiMultiTargetTransformController::Update(float)
{
    NiOutputDebugString("NiMultiTargetTransformController::Update: "
        "This class has been deprecated and does nothing.\n");
    return;
}
//---------------------------------------------------------------------------
unsigned short NiMultiTargetTransformController::GetInterpolatorCount() const
{
    NiOutputDebugString(
        "NiMultiTargetTransformController::GetInterpolatorCount: "
        "This class has been deprecated and does nothing.\n");
    return 0;
}
//---------------------------------------------------------------------------
const char* NiMultiTargetTransformController::GetInterpolatorID(
    unsigned short)
{
    NiOutputDebugString("NiMultiTargetTransformController::GetInterpolatorID: "
        "This class has been deprecated and does nothing.\n");
    return NULL;
}
//---------------------------------------------------------------------------
unsigned short NiMultiTargetTransformController::GetInterpolatorIndex(
    const char*) const
{
    NiOutputDebugString(
        "NiMultiTargetTransformController::GetInterpolatorIndex: "
        "This class has been deprecated and does nothing.\n");
    return INVALID_INDEX;
}
//---------------------------------------------------------------------------
NiInterpolator* NiMultiTargetTransformController::GetInterpolator(
    unsigned short) const
{
    NiOutputDebugString("NiMultiTargetTransformController::GetInterpolator: "
        "This class has been deprecated and does nothing.\n");
    return NULL;
}
//---------------------------------------------------------------------------
void NiMultiTargetTransformController::SetInterpolator(
    NiInterpolator*, unsigned short)
{
    NiOutputDebugString("NiMultiTargetTransformController::SetInterpolator: "
        "This class has been deprecated and does nothing.\n");
}
//---------------------------------------------------------------------------
void NiMultiTargetTransformController::ResetTimeExtrema()
{
    NiOutputDebugString("NiMultiTargetTransformController::ResetTimeExtrema: "
        "This class has been deprecated and does nothing.\n");
}
//---------------------------------------------------------------------------
NiEvaluator* NiMultiTargetTransformController::CreatePoseEvaluator(
    unsigned short)
{
    NiOutputDebugString(
        "NiMultiTargetTransformController::CreatePoseEvaluator: "
        "This class has been deprecated and does nothing.\n");
    return NULL;
}
//---------------------------------------------------------------------------
NiInterpolator* NiMultiTargetTransformController::CreatePoseInterpolator(
    unsigned short)
{
    NiOutputDebugString(
        "NiMultiTargetTransformController::CreatePoseInterpolator: "
        "This class has been deprecated and does nothing.\n");
    return NULL;
}
//---------------------------------------------------------------------------
void NiMultiTargetTransformController::SynchronizePoseInterpolator(
    NiInterpolator*, unsigned short)
{
    NiOutputDebugString(
        "NiMultiTargetTransformController::SynchronizePoseInterpolator: "
        "This class has been deprecated and does nothing.\n");
}
//---------------------------------------------------------------------------
NiBlendInterpolator* NiMultiTargetTransformController::
    CreateBlendInterpolator(unsigned short, bool, 
    float, unsigned char) const
{
    NiOutputDebugString(
        "NiMultiTargetTransformController::CreateBlendInterpolator: "
        "This class has been deprecated and does nothing.\n");
    return NULL;
}
//---------------------------------------------------------------------------
void NiMultiTargetTransformController::GuaranteeTimeRange(float,
    float)
{
    NiOutputDebugString(
        "NiMultiTargetTransformController::GuaranteeTimeRange: "
        "This class has been deprecated and does nothing.\n");
}
//---------------------------------------------------------------------------
bool NiMultiTargetTransformController::InterpolatorIsCorrectType(
    NiInterpolator*, unsigned short) const
{
    NiOutputDebugString(
        "NiMultiTargetTransformController::InterpolatorIsCorrectType: "
        "This class has been deprecated and does nothing.\n");
    return false;
}
//---------------------------------------------------------------------------
bool NiMultiTargetTransformController::TargetIsRequiredType() const
{
    return NiIsKindOf(NiAVObject, m_pkTarget);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiMultiTargetTransformController);
//---------------------------------------------------------------------------
void NiMultiTargetTransformController::CopyMembers(
    NiMultiTargetTransformController* pkDest, NiCloningProcess& kCloning)
{
    NiInterpController::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiMultiTargetTransformController);
//---------------------------------------------------------------------------
void NiMultiTargetTransformController::LoadBinary(NiStream& kStream)
{
    NiInterpController::LoadBinary(kStream);

    // This class has been deprecated and no longer contains any
    // meaningful data members.

    if (kStream.GetFileVersion() < NiAnimationConstants::GetPoseVersion())
    {
        // Read the values from the stream, then ignore them.
        unsigned short usNumInterps;
        NiStreamLoadBinary(kStream, usNumInterps);
    
        for (unsigned short us = 0; us < usNumInterps; us++)
        {
            // Read stored m_ppkTargets[us].
            kStream.ResolveLinkID();
        }
    }
}
//---------------------------------------------------------------------------
void NiMultiTargetTransformController::LinkObject(NiStream& kStream)
{
    NiInterpController::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiMultiTargetTransformController::RegisterStreamables(NiStream& kStream)
{
    return NiInterpController::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiMultiTargetTransformController::SaveBinary(NiStream& kStream)
{
    // This class has been deprecated. SaveBinary should no longer be called 
    // since all NiMultiTargetTransformController objects should be eliminated
    // by NiOldAnimationConverter during loading. Exporters should no longer 
    // create this controller.
    NIASSERT(false);

    NiInterpController::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiMultiTargetTransformController::IsEqual(NiObject* pkObject)
{
    if (!NiInterpController::IsEqual(pkObject))
    {
        return false;
    }

    // This class has been deprecated and no longer contains any
    // meaningful data members.

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiMultiTargetTransformController::GetViewerStrings(NiViewerStringsArray* 
    pkStrings)
{
    NiInterpController::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiMultiTargetTransformController::ms_RTTI
        .GetName()));

    // This class has been deprecated and no longer contains any
    // meaningful data members.
}
//---------------------------------------------------------------------------
