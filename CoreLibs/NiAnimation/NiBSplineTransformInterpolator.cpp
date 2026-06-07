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

#include "NiBSplineTransformInterpolator.h"

NiImplementRTTI(NiBSplineTransformInterpolator, NiBSplineInterpolator);

//---------------------------------------------------------------------------
NiBSplineTransformInterpolator::NiBSplineTransformInterpolator() : 
    NiBSplineInterpolator(NULL, NULL), 
    m_kTransCPHandle(NiBSplineData::INVALID_HANDLE), 
    m_kRotCPHandle(NiBSplineData::INVALID_HANDLE),
    m_kScaleCPHandle(NiBSplineData::INVALID_HANDLE)
{
}
//---------------------------------------------------------------------------
NiBSplineTransformInterpolator::NiBSplineTransformInterpolator(
    NiBSplineData* pkData, NiBSplineData::Handle kTransCPHandle,
    NiBSplineData::Handle kRotCPHandle, NiBSplineData::Handle kScaleCPHandle,
    NiBSplineBasisData* pkBasisData) : NiBSplineInterpolator(pkData, 
    pkBasisData), m_kTransCPHandle(kTransCPHandle), 
    m_kRotCPHandle(kRotCPHandle),
    m_kScaleCPHandle(kScaleCPHandle)
{
}
//---------------------------------------------------------------------------
NiBSplineTransformInterpolator::NiBSplineTransformInterpolator(
    const NiQuatTransform& kPoseValue) : NiBSplineInterpolator(),
    m_kTransformValue(kPoseValue), 
    m_kTransCPHandle(NiBSplineData::INVALID_HANDLE),
    m_kRotCPHandle(NiBSplineData::INVALID_HANDLE),
    m_kScaleCPHandle(NiBSplineData::INVALID_HANDLE)
{
}
//---------------------------------------------------------------------------
bool NiBSplineTransformInterpolator::IsTransformValueSupported() const
{
    return true;
}
//---------------------------------------------------------------------------
unsigned short NiBSplineTransformInterpolator::GetChannelCount() const
{
    return 3;
}
//---------------------------------------------------------------------------
bool NiBSplineTransformInterpolator::Update(float fTime, 
    NiObjectNET*, NiQuatTransform& kValue)
{
    if (!TimeHasChanged(fTime))
    {
        kValue = m_kTransformValue;
            
        if (m_kTransformValue.IsTransformInvalid())
            return false;
        return true;
    }

    // Normalize the time to the range of the keys
    float fNormTime = (fTime - m_fStartTime)/(m_fEndTime - m_fStartTime);
    float afValues[4];

    if (NiBSplineData::INVALID_HANDLE != m_kTransCPHandle)
    {
        NIASSERT(m_spData);
        NIASSERT(m_spBasisData);
        m_spData->GetValueDegree3(fNormTime, (float*)afValues, 3, 
            m_spBasisData, m_kTransCPHandle);
        m_kTransformValue.SetTranslate(NiPoint3(afValues[0],
            afValues[1], afValues[2]));
    }
    if (NiBSplineData::INVALID_HANDLE != m_kRotCPHandle)
    {
        NIASSERT(m_spData);
        NIASSERT(m_spBasisData);
        m_spData->GetValueDegree3(fNormTime, (float*)afValues, 4, 
            m_spBasisData, m_kRotCPHandle);
        NiQuaternion kQuat(afValues[0], afValues[1], afValues[2], 
            afValues[3]);
        // We're using a fast normalize here because the BSpline 
        // curve interpolation should be pretty close to being
        // unitized already. This just snaps it back to the 
        // surface of the hypersphere.
        kQuat.FastNormalize();
        m_kTransformValue.SetRotate(kQuat);
    }
    if (NiBSplineData::INVALID_HANDLE != m_kScaleCPHandle)
    {
        NIASSERT(m_spData);
        NIASSERT(m_spBasisData);
        m_spData->GetValueDegree3(fNormTime, (float*)afValues, 1, 
            m_spBasisData, m_kScaleCPHandle); 
        m_kTransformValue.SetScale(afValues[0]);
    }
    
    kValue = m_kTransformValue;
    if (m_kTransformValue.IsTransformInvalid())
    {
        return false;
    }

    m_fLastTime = fTime;
    return true;
}
//---------------------------------------------------------------------------
bool NiBSplineTransformInterpolator::GetChannelPosed(
    unsigned short usChannel) const
{
    switch(usChannel)
    {
        case POSITION:
            if (GetControlPointCount(usChannel) > 0)
                return false;
            else 
                return m_kTransformValue.IsTranslateValid();
        case ROTATION:
            if (GetControlPointCount(usChannel) > 0)
                return false;
            else 
                return m_kTransformValue.IsRotateValid();
        case SCALE:
            if (GetControlPointCount(usChannel) > 0)
                return false;
            else 
                return m_kTransformValue.IsScaleValid();
        default:
            return false;
    }
}
//---------------------------------------------------------------------------
unsigned int NiBSplineTransformInterpolator::GetDimension(
    unsigned short usChannel) const
{
    switch(usChannel)
    {
        case POSITION:
            return 3;
        case ROTATION:
            return 4;
        case SCALE:
            return 1;
        default:
            return 0;
    }
}
//---------------------------------------------------------------------------
unsigned int NiBSplineTransformInterpolator::GetDegree(
    unsigned short usChannel) const
{
    switch(usChannel)
    {
        case POSITION:
            return 3;
        case ROTATION:
            return 3;
        case SCALE:
            return 3;
        default:
            return 0;
    }
}
//---------------------------------------------------------------------------
void NiBSplineTransformInterpolator::SetPoseTranslate(
    const NiPoint3& kTranslate)
{
    m_kTransformValue.SetTranslate(kTranslate);
    if (m_spData)
        m_kTransCPHandle = NiBSplineData::INVALID_HANDLE;
}
//---------------------------------------------------------------------------
void NiBSplineTransformInterpolator::SetPoseRotate(
    const NiQuaternion& kRotate)
{
    m_kTransformValue.SetRotate(kRotate);
    if (m_spData)
        m_kRotCPHandle = NiBSplineData::INVALID_HANDLE;
}
//---------------------------------------------------------------------------
void NiBSplineTransformInterpolator::SetPoseScale(float fScale)
{
    m_kTransformValue.SetScale(fScale);
    if (m_spData)
        m_kScaleCPHandle = NiBSplineData::INVALID_HANDLE;
}
//---------------------------------------------------------------------------
void NiBSplineTransformInterpolator::SetPoseValue(
    const NiQuatTransform& kPoseValue)
{
    SetPoseTranslate(kPoseValue.GetTranslate());
    SetPoseRotate(kPoseValue.GetRotate());
    SetPoseScale(kPoseValue.GetScale());
    m_spData = NULL;
}
//---------------------------------------------------------------------------
NiBSplineData::Handle NiBSplineTransformInterpolator::GetControlHandle(
    unsigned short usChannel) const
{
    switch(usChannel)
    {
        case POSITION:
            return m_kTransCPHandle;
        case ROTATION:
            return m_kRotCPHandle;
        case SCALE:
            return m_kScaleCPHandle;
        default:
            return NiBSplineData::INVALID_HANDLE;
    }
}
//---------------------------------------------------------------------------
void NiBSplineTransformInterpolator::SetControlHandle(
    NiBSplineData::Handle kControlHandle, 
    unsigned short usChannel)
{
    switch(usChannel)
    {
        case POSITION:
            m_kTransCPHandle = kControlHandle;
            break;
        case ROTATION:
            m_kRotCPHandle = kControlHandle;
            break;
        case SCALE:
            m_kScaleCPHandle = kControlHandle;
            break;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiBSplineTransformInterpolator);
//---------------------------------------------------------------------------
void NiBSplineTransformInterpolator::CopyMembers(
    NiBSplineTransformInterpolator* pkDest,
    NiCloningProcess& kCloning)
{
    NiBSplineInterpolator::CopyMembers(pkDest, kCloning);

    pkDest->m_kTransformValue = m_kTransformValue;
    pkDest->m_kTransCPHandle = m_kTransCPHandle;
    pkDest->m_kRotCPHandle = m_kRotCPHandle;
    pkDest->m_kScaleCPHandle = m_kScaleCPHandle;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBSplineTransformInterpolator);

//---------------------------------------------------------------------------
void NiBSplineTransformInterpolator::LoadBinary(NiStream& kStream)
{
    NiBSplineInterpolator::LoadBinary(kStream);

    m_kTransformValue.LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_kTransCPHandle);
    NiStreamLoadBinary(kStream, m_kRotCPHandle);
    NiStreamLoadBinary(kStream, m_kScaleCPHandle);
}
//---------------------------------------------------------------------------
void NiBSplineTransformInterpolator::LinkObject(NiStream& kStream)
{
    NiBSplineInterpolator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBSplineTransformInterpolator::RegisterStreamables(NiStream& kStream)
{
    if (!NiBSplineInterpolator::RegisterStreamables(kStream))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiBSplineTransformInterpolator::SaveBinary(NiStream& kStream)
{
    NiBSplineInterpolator::SaveBinary(kStream);

    m_kTransformValue.SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_kTransCPHandle);
    NiStreamSaveBinary(kStream, m_kRotCPHandle);
    NiStreamSaveBinary(kStream, m_kScaleCPHandle);
}
//---------------------------------------------------------------------------
bool NiBSplineTransformInterpolator::IsEqual(NiObject* pkObject)
{
    if (!NiBSplineInterpolator::IsEqual(pkObject))
    {
        return false;
    }

    NiBSplineTransformInterpolator* pkDest = 
        (NiBSplineTransformInterpolator*) pkObject;

    if (m_kTransformValue != pkDest->m_kTransformValue)
    {
        return false;
    }

    if (pkDest->m_kTransCPHandle != m_kTransCPHandle ||
        pkDest->m_kRotCPHandle != m_kRotCPHandle ||
        pkDest->m_kScaleCPHandle != m_kScaleCPHandle)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiBSplineTransformInterpolator::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiBSplineInterpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiBSplineTransformInterpolator::ms_RTTI
        .GetName()));

    m_kTransformValue.GetViewerStrings(pkStrings);
    
    pkStrings->Add(NiGetViewerString("m_kTransCPHandle",
        m_kTransCPHandle));
    pkStrings->Add(NiGetViewerString("m_kRotCPHandle",
        m_kRotCPHandle));
    pkStrings->Add(NiGetViewerString("m_kScaleCPHandle",
        m_kScaleCPHandle));
}
//---------------------------------------------------------------------------
