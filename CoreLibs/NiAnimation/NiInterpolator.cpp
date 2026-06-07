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
#include "NiInterpolator.h"
#include <NiQuaternion.h>
#include <NiPoint3.h>
#include <NiColor.h>
#include "NiQuatTransform.h"

NiImplementRTTI(NiInterpolator, NiObject);

const NiBool NiInterpolator::INVALID_BOOL = NiBool(2);
const float NiInterpolator::INVALID_FLOAT = -FLT_MAX;
const NiQuaternion NiInterpolator::INVALID_QUATERNION = NiQuaternion(
    -FLT_MAX, -FLT_MAX, -FLT_MAX, -FLT_MAX);
const NiPoint3 NiInterpolator::INVALID_POINT3 = NiPoint3(-FLT_MAX,
    -FLT_MAX, -FLT_MAX);
const NiColorA NiInterpolator::INVALID_COLORA = NiColorA(-FLT_MAX,
    -FLT_MAX, -FLT_MAX, -FLT_MAX);

//---------------------------------------------------------------------------
NiInterpolator::NiInterpolator() : m_fLastTime(-NI_INFINITY)
{
}
//---------------------------------------------------------------------------
NiInterpolator::~NiInterpolator()
{
}
//---------------------------------------------------------------------------
bool NiInterpolator::Update(float, NiObjectNET*, bool&) 
{
    NIASSERT(!"Bool values are not supported by this interpolator!");
    return false;
}
//---------------------------------------------------------------------------
bool NiInterpolator::Update(float, NiObjectNET*, float&) 
{
    NIASSERT(!"Float values are not supported by this interpolator!");
    return false;
}
//---------------------------------------------------------------------------
bool NiInterpolator::Update(float, NiObjectNET*, NiQuaternion&) 
{
    NIASSERT(!"Quaternion values are not supported by this interpolator!");
    return false;
}
//---------------------------------------------------------------------------
bool NiInterpolator::Update(float, NiObjectNET*, NiPoint3&) 
{
    NIASSERT(!"Point3 values are not supported by this interpolator!");
    return false;
}
//---------------------------------------------------------------------------
bool NiInterpolator::Update(float, NiObjectNET*, NiColorA&) 
{
    NIASSERT(!"ColorA values are not supported by this interpolator!");
    return false;
}
//---------------------------------------------------------------------------
bool NiInterpolator::Update(float, NiObjectNET*, NiQuatTransform&) 
{
    NIASSERT(!"Transform values are not supported by this interpolator!");
    return false;
}
//---------------------------------------------------------------------------
bool NiInterpolator::IsBoolValueSupported() const
{
    return false;
}
//---------------------------------------------------------------------------
bool NiInterpolator::IsFloatValueSupported() const
{
    return false;
}
//---------------------------------------------------------------------------
bool NiInterpolator::IsQuaternionValueSupported() const
{
    return false;
}
//---------------------------------------------------------------------------
bool NiInterpolator::IsPoint3ValueSupported() const
{
    return false;
}
//---------------------------------------------------------------------------
bool NiInterpolator::IsColorAValueSupported() const
{
    return false;
}
//---------------------------------------------------------------------------
bool NiInterpolator::IsTransformValueSupported() const
{
    return false;
}
//---------------------------------------------------------------------------
void NiInterpolator::Collapse()
{
}
//---------------------------------------------------------------------------
void NiInterpolator::GetActiveTimeRange(float& fBeginKeyTime,
    float& fEndKeyTime) const
{
    fBeginKeyTime = 0.0f;
    fEndKeyTime = 0.0f;
}
//---------------------------------------------------------------------------
void NiInterpolator::GuaranteeTimeRange(float, float)
{
}
//---------------------------------------------------------------------------
NiInterpolator* NiInterpolator::GetSequenceInterpolator(float, float)
{
    NiInterpolator* pkInterp = (NiInterpolator*) Clone();
    return pkInterp;
}
//---------------------------------------------------------------------------
bool NiInterpolator::AlwaysUpdate() const
{
    return false;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
void NiInterpolator::CopyMembers(NiInterpolator* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
void NiInterpolator::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    if (kStream.GetFileVersion() >= NiAnimationConstants::GetPoseVersion() &&
        kStream.GetFileVersion() < NiAnimationConstants::GetScratchPadVersion())
    {
        // The conversion from sequence-based interpolators to evaluators
        // made this streamed channel type data obsolete.
        NiInt8 aiInterpChannelTypes[4];
        for (unsigned int ui = 0; ui < 4; ui++)
        {
            NiStreamLoadBinary(kStream, aiInterpChannelTypes[ui]);
        }
    }
}
//---------------------------------------------------------------------------
void NiInterpolator::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiInterpolator::RegisterStreamables(NiStream& kStream)
{
    return NiObject::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiInterpolator::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiInterpolator::IsEqual(NiObject* pkObject)
{
    return NiObject::IsEqual(pkObject);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiInterpolator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiInterpolator::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------
