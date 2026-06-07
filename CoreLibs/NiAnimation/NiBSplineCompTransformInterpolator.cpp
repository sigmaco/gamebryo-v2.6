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

#include "NiBSplineCompTransformInterpolator.h"

NiImplementRTTI(NiBSplineCompTransformInterpolator,
    NiBSplineTransformInterpolator);

//---------------------------------------------------------------------------
NiBSplineCompTransformInterpolator::NiBSplineCompTransformInterpolator() : 
    NiBSplineTransformInterpolator()
{
    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        m_afCompScalars[ui] = NI_INFINITY;
    }
}
//---------------------------------------------------------------------------
NiBSplineCompTransformInterpolator::NiBSplineCompTransformInterpolator(
    NiBSplineData* pkData, NiBSplineData::Handle kTransCPHandle,
    NiBSplineData::Handle kRotCPHandle, NiBSplineData::Handle kScaleCPHandle,
    NiBSplineBasisData* pkBasisData): 
    NiBSplineTransformInterpolator(pkData, kTransCPHandle,
    kRotCPHandle, kScaleCPHandle, pkBasisData)
{
    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        m_afCompScalars[ui] = NI_INFINITY;
    }
}
//---------------------------------------------------------------------------
NiBSplineCompTransformInterpolator::NiBSplineCompTransformInterpolator(
    NiQuatTransform kPoseValue) : NiBSplineTransformInterpolator(kPoseValue)
{
    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        m_afCompScalars[ui] = NI_INFINITY;
    }
}
//---------------------------------------------------------------------------
bool NiBSplineCompTransformInterpolator::Update(float fTime, 
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
        m_spData->GetCompactedValueDegree3(fNormTime, (float*)afValues, 3, 
            m_spBasisData, m_kTransCPHandle, 
            m_afCompScalars[POSITION_OFFSET], 
            m_afCompScalars[POSITION_RANGE]);
        m_kTransformValue.SetTranslate(NiPoint3(afValues[0],
            afValues[1], afValues[2]));
    }
    if (NiBSplineData::INVALID_HANDLE != m_kRotCPHandle)
    {
        NIASSERT(m_spData);
        NIASSERT(m_spBasisData);
        m_spData->GetCompactedValueDegree3(fNormTime, (float*)afValues, 4, 
            m_spBasisData, m_kRotCPHandle, 
            m_afCompScalars[ROTATION_OFFSET], 
            m_afCompScalars[ROTATION_RANGE]);
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
        m_spData->GetCompactedValueDegree3(fNormTime, (float*)afValues, 1, 
            m_spBasisData, m_kScaleCPHandle, 
            m_afCompScalars[SCALE_OFFSET], 
            m_afCompScalars[SCALE_RANGE]); 
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
void NiBSplineCompTransformInterpolator::SetOffset(float fOffset,
    unsigned short usChannel)
{
    switch (usChannel)
    {
        case POSITION:
            m_afCompScalars[POSITION_OFFSET] = fOffset;
            break;
        case ROTATION:
            m_afCompScalars[ROTATION_OFFSET] = fOffset;
            break;
        case SCALE:
            m_afCompScalars[SCALE_OFFSET] = fOffset;
            break;
    }
}
//---------------------------------------------------------------------------
void NiBSplineCompTransformInterpolator::SetHalfRange(float fHalfRange, 
    unsigned short usChannel)
{
    switch (usChannel)
    {
        case POSITION:
            m_afCompScalars[POSITION_RANGE] = fHalfRange;
            break;
        case ROTATION:
            m_afCompScalars[ROTATION_RANGE] = fHalfRange;
            break;
        case SCALE:
            m_afCompScalars[SCALE_RANGE] = fHalfRange;
            break;
    }
}
//---------------------------------------------------------------------------
float NiBSplineCompTransformInterpolator::GetOffset(
    unsigned short usChannel) const
{
    switch (usChannel)
    {
        case POSITION:
            return m_afCompScalars[POSITION_OFFSET];
        case ROTATION:
            return m_afCompScalars[ROTATION_OFFSET];
        case SCALE:
            return m_afCompScalars[SCALE_OFFSET];
    }
    return NI_INFINITY;
}
//---------------------------------------------------------------------------
float NiBSplineCompTransformInterpolator::GetHalfRange(
    unsigned short usChannel) const
{
    switch (usChannel)
    {
        case POSITION:
            return m_afCompScalars[POSITION_RANGE];
        case ROTATION:
            return m_afCompScalars[ROTATION_RANGE];
        case SCALE:
            return m_afCompScalars[SCALE_RANGE];
    }
    return NI_INFINITY;
}
//---------------------------------------------------------------------------
bool NiBSplineCompTransformInterpolator::UsesCompressedControlPoints() const
{
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiBSplineCompTransformInterpolator);
//---------------------------------------------------------------------------
void NiBSplineCompTransformInterpolator::CopyMembers(
    NiBSplineCompTransformInterpolator* pkDest,
    NiCloningProcess& kCloning)
{
    NiBSplineTransformInterpolator::CopyMembers(pkDest, kCloning);

    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        pkDest->m_afCompScalars[ui] = m_afCompScalars[ui];
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBSplineCompTransformInterpolator);

//---------------------------------------------------------------------------
void NiBSplineCompTransformInterpolator::LoadBinary(NiStream& kStream)
{
    NiBSplineTransformInterpolator::LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_afCompScalars, NUM_SCALARS);
}
//---------------------------------------------------------------------------
void NiBSplineCompTransformInterpolator::LinkObject(NiStream& kStream)
{
    NiBSplineTransformInterpolator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBSplineCompTransformInterpolator::RegisterStreamables(NiStream& kStream)
{
    if (!NiBSplineTransformInterpolator::RegisterStreamables(kStream))
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
void NiBSplineCompTransformInterpolator::SaveBinary(NiStream& kStream)
{
    NiBSplineTransformInterpolator::SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_afCompScalars, NUM_SCALARS);
}
//---------------------------------------------------------------------------
bool NiBSplineCompTransformInterpolator::IsEqual(NiObject* pkObject)
{
    if (!NiBSplineTransformInterpolator::IsEqual(pkObject))
    {
        return false;
    }

    NiBSplineCompTransformInterpolator* pkDest = 
        (NiBSplineCompTransformInterpolator*) pkObject;

    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        if (m_afCompScalars[ui] != pkDest->m_afCompScalars[ui])
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiBSplineCompTransformInterpolator::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiBSplineTransformInterpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiBSplineCompTransformInterpolator::ms_RTTI.GetName()));   
}
//---------------------------------------------------------------------------
