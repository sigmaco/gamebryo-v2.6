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

#include "NiBSplinePoint3Interpolator.h"

NiImplementRTTI(NiBSplinePoint3Interpolator, NiBSplineInterpolator);

//---------------------------------------------------------------------------
NiBSplinePoint3Interpolator::NiBSplinePoint3Interpolator() : 
    NiBSplineInterpolator(NULL, NULL), 
    m_kPoint3CPHandle(NiBSplineData::INVALID_HANDLE)
{
}
//---------------------------------------------------------------------------
NiBSplinePoint3Interpolator::NiBSplinePoint3Interpolator(
    NiBSplineData* pkData, NiBSplineData::Handle kPoint3CPHandle,
    NiBSplineBasisData* pkBasisData) : NiBSplineInterpolator(pkData,
    pkBasisData), m_kPoint3CPHandle(kPoint3CPHandle)
{
}
//---------------------------------------------------------------------------
NiBSplinePoint3Interpolator::NiBSplinePoint3Interpolator(
    const NiPoint3& kPoseValue) : NiBSplineInterpolator(),
    m_kPoint3Value(kPoseValue), 
    m_kPoint3CPHandle(NiBSplineData::INVALID_HANDLE)
{
}
//---------------------------------------------------------------------------
bool NiBSplinePoint3Interpolator::IsPoint3ValueSupported() const
{
    return true;
}
//---------------------------------------------------------------------------
unsigned short NiBSplinePoint3Interpolator::GetChannelCount() const
{
    return 1;
}
//---------------------------------------------------------------------------
bool NiBSplinePoint3Interpolator::Update(float fTime, 
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
        float afValues[3];

        NIASSERT(m_spData);
        NIASSERT(m_spBasisData);
        m_spData->GetValueDegree3(fNormTime, (float*)afValues, 3, 
            m_spBasisData, m_kPoint3CPHandle);
        m_kPoint3Value = NiPoint3(afValues[0], afValues[1], afValues[2]);
    }
    
    kValue = m_kPoint3Value;

    m_fLastTime = fTime;
    return true;
}
//---------------------------------------------------------------------------
bool NiBSplinePoint3Interpolator::GetChannelPosed(
    unsigned short usChannel) const
{
    if (POINT3 == usChannel)
    {
        return (GetControlPointCount(usChannel) <= 0);
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
unsigned int NiBSplinePoint3Interpolator::GetDimension(
    unsigned short usChannel) const
{
    if (POINT3 == usChannel)
        return 3;
    else
        return 0;
}
//---------------------------------------------------------------------------
unsigned int NiBSplinePoint3Interpolator::GetDegree(
    unsigned short usChannel) const
{
    if (POINT3 == usChannel)
        return 3;
    else
        return 0;
}
//---------------------------------------------------------------------------
void NiBSplinePoint3Interpolator::SetPoseValue(const NiPoint3& rkPoseValue)
{
    m_kPoint3Value = rkPoseValue;
    if (m_spData)
        m_kPoint3CPHandle = NiBSplineData::INVALID_HANDLE;
    m_spData = NULL;
}
//---------------------------------------------------------------------------
NiBSplineData::Handle NiBSplinePoint3Interpolator::GetControlHandle(
    unsigned short usChannel) const
{
    switch(usChannel)
    {
        case POINT3:
            return m_kPoint3CPHandle;
        default:
            return NiBSplineData::INVALID_HANDLE;
    }
}
//---------------------------------------------------------------------------
void NiBSplinePoint3Interpolator::SetControlHandle(
    NiBSplineData::Handle kControlHandle, 
    unsigned short usChannel)
{
    switch(usChannel)
    {
        case POINT3:
            m_kPoint3CPHandle = kControlHandle;
            break;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiBSplinePoint3Interpolator);
//---------------------------------------------------------------------------
void NiBSplinePoint3Interpolator::CopyMembers(
    NiBSplinePoint3Interpolator* pkDest,
    NiCloningProcess& kCloning)
{
    NiBSplineInterpolator::CopyMembers(pkDest, kCloning);

    pkDest->m_kPoint3Value = m_kPoint3Value;
    pkDest->m_kPoint3CPHandle = m_kPoint3CPHandle;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBSplinePoint3Interpolator);

//---------------------------------------------------------------------------
void NiBSplinePoint3Interpolator::LoadBinary(NiStream& kStream)
{
    NiBSplineInterpolator::LoadBinary(kStream);

    m_kPoint3Value.LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_kPoint3CPHandle);
}
//---------------------------------------------------------------------------
void NiBSplinePoint3Interpolator::LinkObject(NiStream& kStream)
{
    NiBSplineInterpolator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBSplinePoint3Interpolator::RegisterStreamables(NiStream& kStream)
{
    if (!NiBSplineInterpolator::RegisterStreamables(kStream))
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
void NiBSplinePoint3Interpolator::SaveBinary(NiStream& kStream)
{
    NiBSplineInterpolator::SaveBinary(kStream);

    m_kPoint3Value.SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_kPoint3CPHandle);
}
//---------------------------------------------------------------------------
bool NiBSplinePoint3Interpolator::IsEqual(NiObject* pkObject)
{
    if (!NiBSplineInterpolator::IsEqual(pkObject))
    {
        return false;
    }

    NiBSplinePoint3Interpolator* pkDest = 
        (NiBSplinePoint3Interpolator*) pkObject;

    if (m_kPoint3Value != pkDest->m_kPoint3Value)
    {
        return false;
    }

    if (pkDest->m_kPoint3CPHandle != m_kPoint3CPHandle)
    {
        return false;
    }


    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiBSplinePoint3Interpolator::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiBSplineInterpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiBSplinePoint3Interpolator::ms_RTTI
        .GetName()));

    // m_kPoint3Value->GetViewerString(pkStrings);
    
    pkStrings->Add(NiGetViewerString("m_kPoint3CPHandle",
        m_kPoint3CPHandle));
    
}
//---------------------------------------------------------------------------
