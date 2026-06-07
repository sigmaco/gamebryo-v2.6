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

#include "NiBSplineCompPoint3Interpolator.h"

NiImplementRTTI(NiBSplineCompPoint3Interpolator,
    NiBSplinePoint3Interpolator);

//---------------------------------------------------------------------------
NiBSplineCompPoint3Interpolator::NiBSplineCompPoint3Interpolator() : 
    NiBSplinePoint3Interpolator()
{
    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        m_afCompScalars[ui] = NI_INFINITY;
    }
}
//---------------------------------------------------------------------------
NiBSplineCompPoint3Interpolator::NiBSplineCompPoint3Interpolator(
    NiBSplineData* pkData, NiBSplineData::Handle kPoint3CPHandle,
    NiBSplineBasisData* pkBasisData): 
    NiBSplinePoint3Interpolator(pkData, kPoint3CPHandle, pkBasisData)
{
    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        m_afCompScalars[ui] = NI_INFINITY;
    }
}
//---------------------------------------------------------------------------
NiBSplineCompPoint3Interpolator::NiBSplineCompPoint3Interpolator(
    const NiPoint3& kPoseValue) : NiBSplinePoint3Interpolator(kPoseValue)
{
    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        m_afCompScalars[ui] = NI_INFINITY;
    }
}
//---------------------------------------------------------------------------
bool NiBSplineCompPoint3Interpolator::Update(float fTime, 
    NiObjectNET*, NiPoint3& kValue)
{
    if (!TimeHasChanged(fTime))
    {
        kValue = m_kPoint3Value;
        return true;
    }

    if (NiBSplineData::INVALID_HANDLE != m_kPoint3CPHandle)
    {
        // Normalize the time to the range of the keys
        float fNormTime = (fTime - m_fStartTime)/(m_fEndTime - m_fStartTime);
        float afValues[4];

        NIASSERT(m_spData);
        NIASSERT(m_spBasisData);
        m_spData->GetCompactedValueDegree3(fNormTime, (float*)afValues, 3, 
            m_spBasisData, m_kPoint3CPHandle, 
            m_afCompScalars[POINT3_OFFSET], 
            m_afCompScalars[POINT3_RANGE]);
        m_kPoint3Value = NiPoint3(afValues[0], afValues[1], afValues[2]);
    }
    
    kValue = m_kPoint3Value;

    m_fLastTime = fTime;
    return true;
}
//---------------------------------------------------------------------------
void NiBSplineCompPoint3Interpolator::SetOffset(float fOffset,
    unsigned short usChannel)
{
    switch (usChannel)
    {
        case POINT3:
            m_afCompScalars[POINT3_OFFSET] = fOffset;
            break;
    }
}
//---------------------------------------------------------------------------
void NiBSplineCompPoint3Interpolator::SetHalfRange(float fHalfRange, 
    unsigned short usChannel)
{
    switch (usChannel)
    {
        case POINT3:
            m_afCompScalars[POINT3_RANGE] = fHalfRange;
            break;
    }
}
//---------------------------------------------------------------------------
float NiBSplineCompPoint3Interpolator::GetOffset(
    unsigned short usChannel) const
{
    switch (usChannel)
    {
        case POINT3:
            return m_afCompScalars[POINT3_OFFSET];
    }
    return NI_INFINITY;
}
//---------------------------------------------------------------------------
float NiBSplineCompPoint3Interpolator::GetHalfRange(
    unsigned short usChannel) const
{
    switch (usChannel)
    {
        case POINT3:
            return m_afCompScalars[POINT3_RANGE];
    }
    return NI_INFINITY;
}
//---------------------------------------------------------------------------
bool NiBSplineCompPoint3Interpolator::UsesCompressedControlPoints() const
{
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiBSplineCompPoint3Interpolator);
//---------------------------------------------------------------------------
void NiBSplineCompPoint3Interpolator::CopyMembers(
    NiBSplineCompPoint3Interpolator* pkDest,
    NiCloningProcess& kCloning)
{
    NiBSplinePoint3Interpolator::CopyMembers(pkDest, kCloning);

    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        pkDest->m_afCompScalars[ui] = m_afCompScalars[ui];
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBSplineCompPoint3Interpolator);

//---------------------------------------------------------------------------
void NiBSplineCompPoint3Interpolator::LoadBinary(NiStream& kStream)
{
    NiBSplinePoint3Interpolator::LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_afCompScalars, NUM_SCALARS);
}
//---------------------------------------------------------------------------
void NiBSplineCompPoint3Interpolator::LinkObject(NiStream& kStream)
{
    NiBSplinePoint3Interpolator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBSplineCompPoint3Interpolator::RegisterStreamables(NiStream& kStream)
{
    if (!NiBSplinePoint3Interpolator::RegisterStreamables(kStream))
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
void NiBSplineCompPoint3Interpolator::SaveBinary(NiStream& kStream)
{
    NiBSplinePoint3Interpolator::SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_afCompScalars, NUM_SCALARS);
}
//---------------------------------------------------------------------------
bool NiBSplineCompPoint3Interpolator::IsEqual(NiObject* pkObject)
{
    if (!NiBSplinePoint3Interpolator::IsEqual(pkObject))
    {
        return false;
    }

    NiBSplineCompPoint3Interpolator* pkDest = 
        (NiBSplineCompPoint3Interpolator*) pkObject;

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
void NiBSplineCompPoint3Interpolator::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiBSplinePoint3Interpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiBSplineCompPoint3Interpolator::ms_RTTI.GetName()));   
}
//---------------------------------------------------------------------------
