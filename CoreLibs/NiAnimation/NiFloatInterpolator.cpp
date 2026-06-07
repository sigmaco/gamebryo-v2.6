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

#include "NiFloatInterpolator.h"

NiImplementRTTI(NiFloatInterpolator, NiKeyBasedInterpolator);

//---------------------------------------------------------------------------
NiFloatInterpolator::NiFloatInterpolator() :
    m_fFloatValue(INVALID_FLOAT), m_spFloatData(NULL), m_uiLastIdx(0)
{
}
//---------------------------------------------------------------------------
NiFloatInterpolator::NiFloatInterpolator(NiFloatData* pkFloatData) :
    m_fFloatValue(INVALID_FLOAT), m_spFloatData(pkFloatData), m_uiLastIdx(0)
{
}
//---------------------------------------------------------------------------
NiFloatInterpolator::NiFloatInterpolator(float fPoseValue) :
    m_fFloatValue(fPoseValue), m_spFloatData(NULL), m_uiLastIdx(0)
{
}
//---------------------------------------------------------------------------
bool NiFloatInterpolator::IsFloatValueSupported() const
{
    return true;
}
//---------------------------------------------------------------------------
unsigned short NiFloatInterpolator::GetKeyChannelCount() const
{
    return 1;
}
//---------------------------------------------------------------------------
unsigned int NiFloatInterpolator::GetKeyCount(unsigned short) const
{
    if (!m_spFloatData)
        return 0;
    else
        return m_spFloatData->GetNumKeys();
}
//---------------------------------------------------------------------------
NiAnimationKey::KeyType NiFloatInterpolator::GetKeyType(
    unsigned short) const
{
    if (!m_spFloatData)
        return NiAnimationKey::NOINTERP;

    unsigned int uiNumKeys;
    NiFloatKey::KeyType eType;
    unsigned char ucSize;
    m_spFloatData->GetAnim(uiNumKeys, eType, ucSize);
    return eType; 
}
//---------------------------------------------------------------------------
NiAnimationKey::KeyContent NiFloatInterpolator::GetKeyContent(
    unsigned short) const
{
    return NiAnimationKey::FLOATKEY;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiFloatInterpolator::GetKeyArray(unsigned short)
    const
{
    if (!m_spFloatData)
        return NULL;

    unsigned int uiNumKeys;
    NiFloatKey::KeyType eType;
    unsigned char ucSize;
    NiFloatKey * pkKeys = m_spFloatData->GetAnim(uiNumKeys, eType, 
        ucSize);
    return pkKeys; 
}
//---------------------------------------------------------------------------
unsigned char NiFloatInterpolator::GetKeyStride(unsigned short)
    const
{
    if (!m_spFloatData)
        return 0;

    unsigned int uiNumKeys;
    NiFloatKey::KeyType eType;
    unsigned char ucSize;
    m_spFloatData->GetAnim(uiNumKeys, eType, ucSize);
    return ucSize; 
}
//---------------------------------------------------------------------------
bool NiFloatInterpolator::GetChannelPosed(unsigned short) const
{
    if (m_spFloatData)
        return false;
    if (m_fFloatValue == INVALID_FLOAT)
        return false;
    return true;
}
//---------------------------------------------------------------------------
void NiFloatInterpolator::Collapse()
{
    if (m_spFloatData)
    {
        unsigned int uiNumKeys;
        NiAnimationKey::KeyType eType;
        unsigned char ucSize;
        NiFloatKey* pkKeys = m_spFloatData->GetAnim(uiNumKeys, eType, ucSize);
        if (uiNumKeys == 0)
        {
            m_spFloatData = NULL;
            m_fFloatValue = INVALID_FLOAT;
        }
        else
        {
            NiAnimationKey::IsPosedFunction isposed = 
                NiFloatKey::GetIsPosedFunction(eType);
            NIASSERT(isposed);
            if (isposed(pkKeys, uiNumKeys))
            {
                m_fFloatValue = pkKeys->GetKeyAt(0, ucSize)->GetValue();
                m_spFloatData = NULL;
            }
        }
    }
}

//---------------------------------------------------------------------------
void NiFloatInterpolator::SetPoseValue(float fPoseValue)
{
    m_spFloatData = NULL;
    m_fFloatValue = fPoseValue;
}
//---------------------------------------------------------------------------
void NiFloatInterpolator::GuaranteeTimeRange(float fStartTime, 
    float fEndTime)
{
    if (m_spFloatData)
    {
        m_spFloatData->GuaranteeKeysAtStartAndEnd(fStartTime, 
            fEndTime);
    }
}
//---------------------------------------------------------------------------
NiInterpolator* NiFloatInterpolator::GetSequenceInterpolator(float fStartTime,
    float fEndTime)
{
    NiFloatInterpolator* pkSeqInterp = (NiFloatInterpolator*)
        NiKeyBasedInterpolator::GetSequenceInterpolator(fStartTime, fEndTime);
    if (m_spFloatData)
    {
        NiFloatDataPtr spNewFloatData = m_spFloatData->GetSequenceData(
            fStartTime, fEndTime);
        pkSeqInterp->SetFloatData(spNewFloatData);
        pkSeqInterp->m_uiLastIdx = 0;
    }

    return pkSeqInterp;
}
//---------------------------------------------------------------------------
bool NiFloatInterpolator::Update(float fTime,
    NiObjectNET*, float& fValue)
{
    if (!TimeHasChanged(fTime))
    {
         fValue = m_fFloatValue;
         if (m_fFloatValue == INVALID_FLOAT)
            return false;
         return true;
    }

    unsigned int uiNumKeys;
    NiFloatKey::KeyType eType;
    unsigned char ucSize;
    NiFloatKey* pkKeys = GetKeys(uiNumKeys, eType, ucSize);
    if (uiNumKeys > 0)
    {
        m_fFloatValue = NiFloatKey::GenInterp(fTime, pkKeys, eType,
            uiNumKeys, m_uiLastIdx, ucSize);
    }
    
    if (m_fFloatValue == INVALID_FLOAT)
    {
        return false;
    }

    fValue = m_fFloatValue;
    m_fLastTime = fTime;
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiFloatInterpolator);
//---------------------------------------------------------------------------
void NiFloatInterpolator::CopyMembers(NiFloatInterpolator* pkDest,
    NiCloningProcess& kCloning)
{
    NiKeyBasedInterpolator::CopyMembers(pkDest, kCloning);

    pkDest->m_fFloatValue = m_fFloatValue;
    pkDest->m_spFloatData = m_spFloatData;
    pkDest->m_uiLastIdx = m_uiLastIdx;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiFloatInterpolator);
//---------------------------------------------------------------------------
void NiFloatInterpolator::LoadBinary(NiStream& kStream)
{
    NiKeyBasedInterpolator::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_fFloatValue);
    m_spFloatData = (NiFloatData*) kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiFloatInterpolator::LinkObject(NiStream& kStream)
{
    NiKeyBasedInterpolator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiFloatInterpolator::RegisterStreamables(NiStream& kStream)
{
    if (!NiKeyBasedInterpolator::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spFloatData)
        m_spFloatData->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiFloatInterpolator::SaveBinary(NiStream& kStream)
{
    NiKeyBasedInterpolator::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_fFloatValue);
    kStream.SaveLinkID(m_spFloatData);
}
//---------------------------------------------------------------------------
bool NiFloatInterpolator::IsEqual(NiObject* pkObject)
{
    if (!NiKeyBasedInterpolator::IsEqual(pkObject))
    {
        return false;
    }

    NiFloatInterpolator* pkDest = (NiFloatInterpolator*) pkObject;

    if (m_fFloatValue != pkDest->m_fFloatValue)
        return false;

    if ((m_spFloatData && !pkDest->m_spFloatData) ||
        (!m_spFloatData && pkDest->m_spFloatData) ||
        (m_spFloatData && !m_spFloatData->IsEqual(pkDest->m_spFloatData)))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiFloatInterpolator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiKeyBasedInterpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiFloatInterpolator::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_fFloatValue", m_fFloatValue));
    pkStrings->Add(NiGetViewerString("m_spFloatData", m_spFloatData));
}
//---------------------------------------------------------------------------
