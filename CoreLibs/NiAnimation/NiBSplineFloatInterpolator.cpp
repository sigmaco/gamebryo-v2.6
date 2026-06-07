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

#include "NiBSplineFloatInterpolator.h"

NiImplementRTTI(NiBSplineFloatInterpolator, NiBSplineInterpolator);

//---------------------------------------------------------------------------
NiBSplineFloatInterpolator::NiBSplineFloatInterpolator() : 
    NiBSplineInterpolator(NULL, NULL),
    m_kFloatCPHandle(NiBSplineData::INVALID_HANDLE)
{
}
//---------------------------------------------------------------------------
NiBSplineFloatInterpolator::NiBSplineFloatInterpolator(
    NiBSplineData* pkData, NiBSplineData::Handle kFloatCPHandle,
    NiBSplineBasisData* pkBasisData) : NiBSplineInterpolator(pkData, 
    pkBasisData), m_kFloatCPHandle(kFloatCPHandle)
{
}
//---------------------------------------------------------------------------
NiBSplineFloatInterpolator::NiBSplineFloatInterpolator(
    float kPoseValue) : NiBSplineInterpolator(),
    m_fFloatValue(kPoseValue), 
    m_kFloatCPHandle(NiBSplineData::INVALID_HANDLE)
{
}
//---------------------------------------------------------------------------
bool NiBSplineFloatInterpolator::IsFloatValueSupported() const
{
    return true;
}
//---------------------------------------------------------------------------
unsigned short NiBSplineFloatInterpolator::GetChannelCount() const
{
    return 1;
}
//---------------------------------------------------------------------------
bool NiBSplineFloatInterpolator::Update(float fTime, 
    NiObjectNET*, float& fValue)
{
    if (!TimeHasChanged(fTime))
    {
        fValue = m_fFloatValue;
        return true;
    }

    if (NiBSplineData::INVALID_HANDLE != m_kFloatCPHandle)
    {
        // Normalize the time to the range of the keys
        float fNormTime = (fTime - m_fStartTime)/(m_fEndTime - m_fStartTime);

        NIASSERT(m_spData);
        NIASSERT(m_spBasisData);
        m_spData->GetValueDegree3(fNormTime, &m_fFloatValue, 1, 
            m_spBasisData, m_kFloatCPHandle);
    }
    
    fValue = m_fFloatValue;

    m_fLastTime = fTime;
    return true;
}
//---------------------------------------------------------------------------
bool NiBSplineFloatInterpolator::GetChannelPosed(
    unsigned short usChannel) const
{
    if (FLOAT == usChannel)
    {
        return (GetControlPointCount(usChannel) <= 0);
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
unsigned int NiBSplineFloatInterpolator::GetDimension(
    unsigned short usChannel) const
{
    if (FLOAT == usChannel)
        return 1;
    else
        return 0;
}
//---------------------------------------------------------------------------
unsigned int NiBSplineFloatInterpolator::GetDegree(
    unsigned short usChannel) const
{
    if (FLOAT == usChannel)
        return 3;
    else
        return 0;
}
//---------------------------------------------------------------------------
void NiBSplineFloatInterpolator::SetPoseValue(float fPoseValue)
{
    m_fFloatValue = fPoseValue;
    if (m_spData)
        m_kFloatCPHandle = NiBSplineData::INVALID_HANDLE;
    m_spData = NULL;
}
//---------------------------------------------------------------------------
NiBSplineData::Handle NiBSplineFloatInterpolator::GetControlHandle(
    unsigned short usChannel) const
{
    switch(usChannel)
    {
        case FLOAT:
            return m_kFloatCPHandle;
    }
    return NiBSplineData::INVALID_HANDLE;
}
//---------------------------------------------------------------------------
void NiBSplineFloatInterpolator::SetControlHandle(
    NiBSplineData::Handle kControlHandle, 
    unsigned short usChannel)
{
    switch(usChannel)
    {
        case FLOAT:
            m_kFloatCPHandle = kControlHandle;
            break;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiBSplineFloatInterpolator);
//---------------------------------------------------------------------------
void NiBSplineFloatInterpolator::CopyMembers(
    NiBSplineFloatInterpolator* pkDest,
    NiCloningProcess& kCloning)
{
    NiBSplineInterpolator::CopyMembers(pkDest, kCloning);

    pkDest->m_fFloatValue = m_fFloatValue;

    pkDest->m_kFloatCPHandle = m_kFloatCPHandle;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBSplineFloatInterpolator);

//---------------------------------------------------------------------------
void NiBSplineFloatInterpolator::LoadBinary(NiStream& kStream)
{
    NiBSplineInterpolator::LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_fFloatValue);
    NiStreamLoadBinary(kStream, m_kFloatCPHandle);
}
//---------------------------------------------------------------------------
void NiBSplineFloatInterpolator::LinkObject(NiStream& kStream)
{
    NiBSplineInterpolator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBSplineFloatInterpolator::RegisterStreamables(NiStream& kStream)
{
    if (!NiBSplineInterpolator::RegisterStreamables(kStream))
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
void NiBSplineFloatInterpolator::SaveBinary(NiStream& kStream)
{
    NiBSplineInterpolator::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fFloatValue);
    NiStreamSaveBinary(kStream, m_kFloatCPHandle);
}
//---------------------------------------------------------------------------
bool NiBSplineFloatInterpolator::IsEqual(NiObject* pkObject)
{
    if (!NiBSplineInterpolator::IsEqual(pkObject))
    {
        return false;
    }

    NiBSplineFloatInterpolator* pkDest = 
        (NiBSplineFloatInterpolator*) pkObject;

    if (m_fFloatValue != pkDest->m_fFloatValue)
    {
        return false;
    }

    if (pkDest->m_kFloatCPHandle != m_kFloatCPHandle)
    {
        return false;
    }


    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiBSplineFloatInterpolator::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiBSplineInterpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiBSplineFloatInterpolator::ms_RTTI
        .GetName()));

    pkStrings->Add(NiGetViewerString("m_fFloatValue", m_fFloatValue));
    
    pkStrings->Add(NiGetViewerString("m_kFloatCPHandle",
        m_kFloatCPHandle));
    
}
//---------------------------------------------------------------------------
