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

#include "NiBSplineColorInterpolator.h"

NiImplementRTTI(NiBSplineColorInterpolator, NiBSplineInterpolator);

//---------------------------------------------------------------------------
NiBSplineColorInterpolator::NiBSplineColorInterpolator() : 
    NiBSplineInterpolator(NULL, NULL),
    m_kColorACPHandle(NiBSplineData::INVALID_HANDLE)
{
}
//---------------------------------------------------------------------------
NiBSplineColorInterpolator::NiBSplineColorInterpolator(
    NiBSplineData* pkData, NiBSplineData::Handle kColorACPHandle,
    NiBSplineBasisData* pkBasisData) : NiBSplineInterpolator(pkData,
    pkBasisData), m_kColorACPHandle(kColorACPHandle)
{
}
//---------------------------------------------------------------------------
NiBSplineColorInterpolator::NiBSplineColorInterpolator(
    const NiColorA& kPoseValue) : NiBSplineInterpolator(),
    m_kColorAValue(kPoseValue), 
    m_kColorACPHandle(NiBSplineData::INVALID_HANDLE)
{
}
//---------------------------------------------------------------------------
bool NiBSplineColorInterpolator::IsColorAValueSupported() const
{
    return true;
}
//---------------------------------------------------------------------------
unsigned short NiBSplineColorInterpolator::GetChannelCount() const
{
    return 1;
}
//---------------------------------------------------------------------------
bool NiBSplineColorInterpolator::Update(float fTime, 
    NiObjectNET*, NiColorA& kValue)
{
    if (!TimeHasChanged(fTime))
    {
        kValue = m_kColorAValue;
        return true;
    }

    if (NiBSplineData::INVALID_HANDLE != m_kColorACPHandle)
    {
        // Normalize the time to the range of the keys
        float fNormTime = (fTime - m_fStartTime)/(m_fEndTime - m_fStartTime);
        float afValues[4];

        NIASSERT(m_spData);
        NIASSERT(m_spBasisData);
        m_spData->GetValueDegree3(fNormTime, (float*)afValues, 4, 
            m_spBasisData, m_kColorACPHandle);
        m_kColorAValue = NiColorA(afValues[0], afValues[1], afValues[2], 
            afValues[3]);
    }
    
    kValue = m_kColorAValue;

    m_fLastTime = fTime;
    return true;
}
//---------------------------------------------------------------------------
bool NiBSplineColorInterpolator::GetChannelPosed(
    unsigned short usChannel) const
{
    if (COLORA == usChannel)
    {
        return (GetControlPointCount(usChannel) <= 0);
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
unsigned int NiBSplineColorInterpolator::GetDimension(
    unsigned short usChannel) const
{
    if (COLORA == usChannel)
        return 4;
    else
        return 0;
}
//---------------------------------------------------------------------------
unsigned int NiBSplineColorInterpolator::GetDegree(
    unsigned short usChannel) const
{
    if (COLORA == usChannel)
        return 3;
    else
        return 0;
}
//---------------------------------------------------------------------------
void NiBSplineColorInterpolator::SetPoseValue(NiColorA& rkPoseValue)
{
    m_kColorAValue = rkPoseValue;
    if (m_spData)
        m_kColorACPHandle = NiBSplineData::INVALID_HANDLE;
    m_spData = NULL;
}
//---------------------------------------------------------------------------
NiBSplineData::Handle NiBSplineColorInterpolator::GetControlHandle(
    unsigned short usChannel) const
{
    switch(usChannel)
    {
        case COLORA:
            return m_kColorACPHandle;
    }
    return NiBSplineData::INVALID_HANDLE;
}
//---------------------------------------------------------------------------
void NiBSplineColorInterpolator::SetControlHandle(
    NiBSplineData::Handle kControlHandle, 
    unsigned short usChannel)
{
    switch(usChannel)
    {
        case COLORA:
            m_kColorACPHandle = kControlHandle;
            break;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiBSplineColorInterpolator);
//---------------------------------------------------------------------------
void NiBSplineColorInterpolator::CopyMembers(
    NiBSplineColorInterpolator* pkDest,
    NiCloningProcess& kCloning)
{
    NiBSplineInterpolator::CopyMembers(pkDest, kCloning);

    pkDest->m_kColorAValue = m_kColorAValue;
    pkDest->m_kColorACPHandle = m_kColorACPHandle;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBSplineColorInterpolator);

//---------------------------------------------------------------------------
void NiBSplineColorInterpolator::LoadBinary(NiStream& kStream)
{
    NiBSplineInterpolator::LoadBinary(kStream);

    m_kColorAValue.LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_kColorACPHandle);
}
//---------------------------------------------------------------------------
void NiBSplineColorInterpolator::LinkObject(NiStream& kStream)
{
    NiBSplineInterpolator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBSplineColorInterpolator::RegisterStreamables(NiStream& kStream)
{
    if (!NiBSplineInterpolator::RegisterStreamables(kStream))
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
void NiBSplineColorInterpolator::SaveBinary(NiStream& kStream)
{
    NiBSplineInterpolator::SaveBinary(kStream);

    m_kColorAValue.SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_kColorACPHandle);
}
//---------------------------------------------------------------------------
bool NiBSplineColorInterpolator::IsEqual(NiObject* pkObject)
{
    if (!NiBSplineInterpolator::IsEqual(pkObject))
    {
        return false;
    }

    NiBSplineColorInterpolator* pkDest = 
        (NiBSplineColorInterpolator*) pkObject;

    if (m_kColorAValue != pkDest->m_kColorAValue)
    {
        return false;
    }
    if (pkDest->m_kColorACPHandle != m_kColorACPHandle)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiBSplineColorInterpolator::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiBSplineInterpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiBSplineColorInterpolator::ms_RTTI
        .GetName()));

    //m_kColorAValue.GetViewerStrings(pkStrings);
    pkStrings->Add(NiGetViewerString("m_kColorACPHandle",
        m_kColorACPHandle));
    
}
//---------------------------------------------------------------------------
