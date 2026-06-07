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

#include "NiBoolInterpolator.h"

NiImplementRTTI(NiBoolInterpolator, NiKeyBasedInterpolator);

//---------------------------------------------------------------------------
NiBoolInterpolator::NiBoolInterpolator() :
    m_bBoolValue(INVALID_BOOL), m_spBoolData(NULL), m_uiLastIdx(0)
{
}
//---------------------------------------------------------------------------
NiBoolInterpolator::NiBoolInterpolator(NiBoolData* pkBoolData) :
    m_bBoolValue(INVALID_BOOL), m_spBoolData(pkBoolData), m_uiLastIdx(0)
{
}
//---------------------------------------------------------------------------
NiBoolInterpolator::NiBoolInterpolator(bool bPoseValue) :
    m_bBoolValue(bPoseValue), m_spBoolData(NULL), m_uiLastIdx(0)
{
}
//---------------------------------------------------------------------------
bool NiBoolInterpolator::IsBoolValueSupported() const
{
    return true;
}
//---------------------------------------------------------------------------
unsigned short NiBoolInterpolator::GetKeyChannelCount() const
{
    return 1;
}
//---------------------------------------------------------------------------
unsigned int NiBoolInterpolator::GetKeyCount(unsigned short) const
{
    if (!m_spBoolData)
        return 0;
    else
        return m_spBoolData->GetNumKeys();
}
//---------------------------------------------------------------------------
NiAnimationKey::KeyType NiBoolInterpolator::GetKeyType(
    unsigned short) const
{
    if (!m_spBoolData)
        return NiAnimationKey::NOINTERP;

    unsigned int uiNumKeys;
    NiBoolKey::KeyType eType;
    unsigned char ucSize;
    m_spBoolData->GetAnim(uiNumKeys, eType, ucSize);
    return eType; 
}
//---------------------------------------------------------------------------
NiAnimationKey::KeyContent NiBoolInterpolator::GetKeyContent(
    unsigned short) const
{
    return NiAnimationKey::BOOLKEY;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiBoolInterpolator::GetKeyArray(unsigned short)
    const 
{
    if (!m_spBoolData)
        return NULL;

    unsigned int uiNumKeys;
    NiBoolKey::KeyType eType;
    unsigned char ucSize;
    NiBoolKey * pkKeys = m_spBoolData->GetAnim(uiNumKeys, eType, 
        ucSize);
    return pkKeys; 
}
//---------------------------------------------------------------------------
unsigned char NiBoolInterpolator::GetKeyStride(unsigned short)
    const 
{
    if (!m_spBoolData)
        return 0;

    unsigned int uiNumKeys;
    NiBoolKey::KeyType eType;
    unsigned char ucSize;
    m_spBoolData->GetAnim(uiNumKeys, eType, ucSize);
    return ucSize; 
}
//---------------------------------------------------------------------------
bool NiBoolInterpolator::GetChannelPosed(unsigned short) const
{
    if (m_spBoolData)
        return false;
    if (m_bBoolValue == INVALID_BOOL)
        return false;
    return true;
}
//---------------------------------------------------------------------------
void NiBoolInterpolator::Collapse()
{
    if (m_spBoolData)
    {
        unsigned int uiNumKeys;
        NiAnimationKey::KeyType eType;
        unsigned char ucSize;
        NiBoolKey* pkKeys = m_spBoolData->GetAnim(uiNumKeys, eType, ucSize);
        if (uiNumKeys == 0)
        {
            m_spBoolData = NULL;
            m_bBoolValue = INVALID_BOOL;
        }
        else
        {
            NiAnimationKey::IsPosedFunction isposed = 
                NiBoolKey::GetIsPosedFunction(eType);
            NIASSERT(isposed);
            if (isposed(pkKeys, uiNumKeys))
            {
                m_bBoolValue = pkKeys->GetKeyAt(0, ucSize)->GetBool();
                m_spBoolData = NULL;
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiBoolInterpolator::SetPoseValue(bool bPoseValue)
{
    m_spBoolData = NULL;
    m_bBoolValue = bPoseValue;
}
//---------------------------------------------------------------------------
void NiBoolInterpolator::GuaranteeTimeRange(float fStartTime, 
    float fEndTime)
{
    if (m_spBoolData)
    {
        m_spBoolData->GuaranteeKeysAtStartAndEnd(fStartTime, 
            fEndTime);
    }
}
//---------------------------------------------------------------------------
NiInterpolator* NiBoolInterpolator::GetSequenceInterpolator(float fStartTime,
    float fEndTime)
{
    NiBoolInterpolator* pkSeqInterp = (NiBoolInterpolator*)
        NiKeyBasedInterpolator::GetSequenceInterpolator(fStartTime, fEndTime);
    if (m_spBoolData)
    {
        NiBoolDataPtr spNewBoolData = m_spBoolData->GetSequenceData(
            fStartTime, fEndTime);
        pkSeqInterp->SetBoolData(spNewBoolData);
        pkSeqInterp->m_uiLastIdx = 0;
    }

    return pkSeqInterp;
}
//---------------------------------------------------------------------------
bool NiBoolInterpolator::Update(float fTime,
    NiObjectNET*, bool& bValue)
{
    if (!TimeHasChanged(fTime))
    {
        if (m_bBoolValue == INVALID_BOOL)
        {
            bValue = false;
            return false;
        }
        bValue = (m_bBoolValue != 0);
        return true;
    }

    unsigned int uiNumKeys;
    unsigned char ucSize;
    NiBoolKey::KeyType eType;
    NiBoolKey* pkKeys = GetKeys(uiNumKeys, eType, ucSize);
    if (uiNumKeys > 0)
    {
        bool bInterpValue = NiBoolKey::GenInterp(fTime, pkKeys, eType, 
            uiNumKeys, m_uiLastIdx, ucSize);
        m_bBoolValue = (bInterpValue != 0);
    }

    if (m_bBoolValue == INVALID_BOOL)
    {
        bValue = false;
        return false;
    }

    bValue = (m_bBoolValue != 0);
    m_fLastTime = fTime;
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiBoolInterpolator);
//---------------------------------------------------------------------------
void NiBoolInterpolator::CopyMembers(NiBoolInterpolator* pkDest,
    NiCloningProcess& kCloning)
{
    NiKeyBasedInterpolator::CopyMembers(pkDest, kCloning);

    pkDest->m_bBoolValue = m_bBoolValue;
    pkDest->m_spBoolData = m_spBoolData;
    pkDest->m_uiLastIdx = m_uiLastIdx;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBoolInterpolator);

//---------------------------------------------------------------------------
void NiBoolInterpolator::LoadBinary(NiStream& kStream)
{
    NiKeyBasedInterpolator::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_bBoolValue);
    m_spBoolData = (NiBoolData*) kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiBoolInterpolator::LinkObject(NiStream& kStream)
{
    NiKeyBasedInterpolator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBoolInterpolator::RegisterStreamables(NiStream& kStream)
{
    if (!NiKeyBasedInterpolator::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spBoolData)
        m_spBoolData->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiBoolInterpolator::SaveBinary(NiStream& kStream)
{
    NiKeyBasedInterpolator::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_bBoolValue);
    kStream.SaveLinkID(m_spBoolData);
}
//---------------------------------------------------------------------------
bool NiBoolInterpolator::IsEqual(NiObject* pkObject)
{
    if (!NiKeyBasedInterpolator::IsEqual(pkObject))
    {
        return false;
    }

    NiBoolInterpolator* pkDest = (NiBoolInterpolator*) pkObject;

    if (m_bBoolValue != pkDest->m_bBoolValue)
        return false;
    
    if ((m_spBoolData && !pkDest->m_spBoolData) ||
        (!m_spBoolData && pkDest->m_spBoolData) ||
        (m_spBoolData && !m_spBoolData->IsEqual(pkDest->m_spBoolData)))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiBoolInterpolator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiKeyBasedInterpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiBoolInterpolator::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_bBoolValue", m_bBoolValue));
    pkStrings->Add(NiGetViewerString("m_spBoolData", m_spBoolData));
}
//---------------------------------------------------------------------------
