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

#include "NiBSplineCompColorInterpolator.h"

NiImplementRTTI(NiBSplineCompColorInterpolator,
    NiBSplineColorInterpolator);

//---------------------------------------------------------------------------
NiBSplineCompColorInterpolator::NiBSplineCompColorInterpolator() : 
    NiBSplineColorInterpolator()
{
    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        m_afCompScalars[ui] = NI_INFINITY;
    }
}
//---------------------------------------------------------------------------
NiBSplineCompColorInterpolator::NiBSplineCompColorInterpolator(
    NiBSplineData* pkData, NiBSplineData::Handle kColorCPHandle,
    NiBSplineBasisData* pkBasisData): 
    NiBSplineColorInterpolator(pkData, kColorCPHandle, pkBasisData)
{
    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        m_afCompScalars[ui] = NI_INFINITY;
    }
}
//---------------------------------------------------------------------------
NiBSplineCompColorInterpolator::NiBSplineCompColorInterpolator(
    const NiColorA& kPoseValue) : NiBSplineColorInterpolator(kPoseValue)
{
    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        m_afCompScalars[ui] = NI_INFINITY;
    }
}
//---------------------------------------------------------------------------
bool NiBSplineCompColorInterpolator::Update(float fTime, 
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
        m_spData->GetCompactedValueDegree3(fNormTime, (float*)afValues, 4, 
            m_spBasisData, m_kColorACPHandle, 
            m_afCompScalars[COLORA_OFFSET], 
            m_afCompScalars[COLORA_RANGE]);
        m_kColorAValue = NiColorA(afValues[0], afValues[1], afValues[2], 
            afValues[3]);
    }
    
    kValue = m_kColorAValue;

    m_fLastTime = fTime;
    return true;
}
//---------------------------------------------------------------------------
void NiBSplineCompColorInterpolator::SetOffset(float fOffset,
    unsigned short usChannel)
{
    switch (usChannel)
    {
        case COLORA:
            m_afCompScalars[COLORA_OFFSET] = fOffset;
            break;
    }
}
//---------------------------------------------------------------------------
void NiBSplineCompColorInterpolator::SetHalfRange(float fHalfRange, 
    unsigned short usChannel)
{
    switch (usChannel)
    {
        case COLORA:
            m_afCompScalars[COLORA_RANGE] = fHalfRange;
            break;
    }
}
//---------------------------------------------------------------------------
float NiBSplineCompColorInterpolator::GetOffset(
    unsigned short usChannel) const
{
    switch (usChannel)
    {
        case COLORA:
            return m_afCompScalars[COLORA_OFFSET];
    }
    return NI_INFINITY;
}
//---------------------------------------------------------------------------
float NiBSplineCompColorInterpolator::GetHalfRange(
    unsigned short usChannel) const
{
    switch (usChannel)
    {
        case COLORA:
            return m_afCompScalars[COLORA_RANGE];
    }
    return NI_INFINITY;
}
//---------------------------------------------------------------------------
bool NiBSplineCompColorInterpolator::UsesCompressedControlPoints() const
{
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiBSplineCompColorInterpolator);
//---------------------------------------------------------------------------
void NiBSplineCompColorInterpolator::CopyMembers(
    NiBSplineCompColorInterpolator* pkDest,
    NiCloningProcess& kCloning)
{
    NiBSplineColorInterpolator::CopyMembers(pkDest, kCloning);

    for (unsigned int ui = 0; ui < NUM_SCALARS; ui++)
    {
        pkDest->m_afCompScalars[ui] = m_afCompScalars[ui];
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBSplineCompColorInterpolator);

//---------------------------------------------------------------------------
void NiBSplineCompColorInterpolator::LoadBinary(NiStream& kStream)
{
    NiBSplineColorInterpolator::LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_afCompScalars, NUM_SCALARS);
}
//---------------------------------------------------------------------------
void NiBSplineCompColorInterpolator::LinkObject(NiStream& kStream)
{
    NiBSplineColorInterpolator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBSplineCompColorInterpolator::RegisterStreamables(NiStream& kStream)
{
    if (!NiBSplineColorInterpolator::RegisterStreamables(kStream))
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
void NiBSplineCompColorInterpolator::SaveBinary(NiStream& kStream)
{
    NiBSplineColorInterpolator::SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_afCompScalars, NUM_SCALARS);
}
//---------------------------------------------------------------------------
bool NiBSplineCompColorInterpolator::IsEqual(NiObject* pkObject)
{
    if (!NiBSplineColorInterpolator::IsEqual(pkObject))
    {
        return false;
    }

    NiBSplineCompColorInterpolator* pkDest = 
        (NiBSplineCompColorInterpolator*) pkObject;

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
void NiBSplineCompColorInterpolator::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    NiBSplineColorInterpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiBSplineCompColorInterpolator::ms_RTTI.GetName()));   
}
//---------------------------------------------------------------------------
