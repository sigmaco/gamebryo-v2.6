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

#include "NiBSplineCompFloatInterpolator.h"

NiImplementRTTI(NiBSplineCompFloatInterpolator,
    NiBSplineFloatInterpolator);

//---------------------------------------------------------------------------
NiBSplineCompFloatInterpolator::NiBSplineCompFloatInterpolator() : 
    NiBSplineFloatInterpolator()
{
    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        m_afCompScalars[ui] = NI_INFINITY;
    }
}
//---------------------------------------------------------------------------
NiBSplineCompFloatInterpolator::NiBSplineCompFloatInterpolator(
    NiBSplineData* pkData, NiBSplineData::Handle kFloatCPHandle,
    NiBSplineBasisData* pkBasisData): 
    NiBSplineFloatInterpolator(pkData, kFloatCPHandle, pkBasisData)
{
    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        m_afCompScalars[ui] = NI_INFINITY;
    }
}
//---------------------------------------------------------------------------
NiBSplineCompFloatInterpolator::NiBSplineCompFloatInterpolator(
    const float fPoseValue) : NiBSplineFloatInterpolator(fPoseValue)
{
    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        m_afCompScalars[ui] = NI_INFINITY;
    }
}
//---------------------------------------------------------------------------
bool NiBSplineCompFloatInterpolator::Update(float fTime, 
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
        m_spData->GetCompactedValueDegree3(fNormTime, &m_fFloatValue, 1, 
            m_spBasisData, m_kFloatCPHandle, 
            m_afCompScalars[FLOAT_OFFSET], 
            m_afCompScalars[FLOAT_RANGE]);
    }
    
    fValue = m_fFloatValue;

    m_fLastTime = fTime;
    return true;
}
//---------------------------------------------------------------------------
void NiBSplineCompFloatInterpolator::SetOffset(float fOffset,
    unsigned short usChannel)
{
    switch (usChannel)
    {
        case FLOAT:
            m_afCompScalars[FLOAT_OFFSET] = fOffset;
            break;
    }
}
//---------------------------------------------------------------------------
void NiBSplineCompFloatInterpolator::SetHalfRange(float fHalfRange, 
    unsigned short usChannel)
{
    switch (usChannel)
    {
        case FLOAT:
            m_afCompScalars[FLOAT_RANGE] = fHalfRange;
            break;
    }
}
//---------------------------------------------------------------------------
float NiBSplineCompFloatInterpolator::GetOffset(
    unsigned short usChannel) const
{
    switch (usChannel)
    {
        case FLOAT:
            return m_afCompScalars[FLOAT_OFFSET];
    }
    return NI_INFINITY;
}
//---------------------------------------------------------------------------
float NiBSplineCompFloatInterpolator::GetHalfRange(
    unsigned short usChannel) const
{
    switch (usChannel)
    {
        case FLOAT:
            return m_afCompScalars[FLOAT_RANGE];
    }
    return NI_INFINITY;
}
//---------------------------------------------------------------------------
bool NiBSplineCompFloatInterpolator::UsesCompressedControlPoints() const
{
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiBSplineCompFloatInterpolator);
//---------------------------------------------------------------------------
void NiBSplineCompFloatInterpolator::CopyMembers(
    NiBSplineCompFloatInterpolator* pkDest,
    NiCloningProcess& kCloning)
{
    NiBSplineFloatInterpolator::CopyMembers(pkDest, kCloning);

    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        pkDest->m_afCompScalars[ui] = m_afCompScalars[ui];
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBSplineCompFloatInterpolator);

//---------------------------------------------------------------------------
void NiBSplineCompFloatInterpolator::LoadBinary(NiStream& kStream)
{
    NiBSplineFloatInterpolator::LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_afCompScalars, NUM_SCALARS);
}
//---------------------------------------------------------------------------
void NiBSplineCompFloatInterpolator::LinkObject(NiStream& kStream)
{
    NiBSplineFloatInterpolator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBSplineCompFloatInterpolator::RegisterStreamables(NiStream& kStream)
{
    if (!NiBSplineFloatInterpolator::RegisterStreamables(kStream))
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
void NiBSplineCompFloatInterpolator::SaveBinary(NiStream& kStream)
{
    NiBSplineFloatInterpolator::SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_afCompScalars, NUM_SCALARS);
}
//---------------------------------------------------------------------------
bool NiBSplineCompFloatInterpolator::IsEqual(NiObject* pkObject)
{
    if (!NiBSplineFloatInterpolator::IsEqual(pkObject))
    {
        return false;
    }

    NiBSplineCompFloatInterpolator* pkDest = 
        (NiBSplineCompFloatInterpolator*) pkObject;

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
void NiBSplineCompFloatInterpolator::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiBSplineFloatInterpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiBSplineCompFloatInterpolator::ms_RTTI.GetName()));   
}
//---------------------------------------------------------------------------
