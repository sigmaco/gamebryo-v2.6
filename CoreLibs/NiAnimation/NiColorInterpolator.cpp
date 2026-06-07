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

#include "NiColorInterpolator.h"

NiImplementRTTI(NiColorInterpolator, NiKeyBasedInterpolator);

//---------------------------------------------------------------------------
NiColorInterpolator::NiColorInterpolator() :
    m_kColorValue(INVALID_COLORA), m_spColorData(NULL),
    m_uiLastIdx(0)
{
}
//---------------------------------------------------------------------------
NiColorInterpolator::NiColorInterpolator(NiColorData* pkColorData) :
    m_kColorValue(INVALID_COLORA), m_spColorData(pkColorData),
    m_uiLastIdx(0)
{
}
//---------------------------------------------------------------------------
NiColorInterpolator::NiColorInterpolator(const NiColorA& kPoseValue) :
    m_kColorValue(kPoseValue), m_spColorData(NULL),
    m_uiLastIdx(0)
{
}
//---------------------------------------------------------------------------
bool NiColorInterpolator::IsColorAValueSupported() const
{
    return true;
}
//---------------------------------------------------------------------------
unsigned short NiColorInterpolator::GetKeyChannelCount() const 
{
    return 1;
}
//---------------------------------------------------------------------------
unsigned int NiColorInterpolator::GetKeyCount(unsigned short) const
{
    if (!m_spColorData)
        return 0;
    else
        return m_spColorData->GetNumKeys();
}
//---------------------------------------------------------------------------
NiAnimationKey::KeyType NiColorInterpolator::GetKeyType(
    unsigned short) const
{
    if (!m_spColorData)
        return NiAnimationKey::NOINTERP;

    unsigned int uiNumKeys;
    NiColorKey::KeyType eType;
    unsigned char ucSize;
    m_spColorData->GetAnim(uiNumKeys, eType, ucSize);
    return eType; 
}
//---------------------------------------------------------------------------
NiAnimationKey::KeyContent NiColorInterpolator::GetKeyContent(
    unsigned short) const
{
    return NiAnimationKey::COLORKEY;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiColorInterpolator::GetKeyArray(unsigned short)
    const 
{
    if (!m_spColorData)
        return NULL;

    unsigned int uiNumKeys;
    NiColorKey::KeyType eType;
    unsigned char ucSize;
    NiColorKey * pkKeys = m_spColorData->GetAnim(uiNumKeys, eType, 
        ucSize);
    return pkKeys; 
}
//---------------------------------------------------------------------------
unsigned char NiColorInterpolator::GetKeyStride(unsigned short)
    const 
{
    if (!m_spColorData)
        return 0;

    unsigned int uiNumKeys;
    NiColorKey::KeyType eType;
    unsigned char ucSize;
    m_spColorData->GetAnim(uiNumKeys, eType, ucSize);
    return ucSize; 
}
//---------------------------------------------------------------------------
bool NiColorInterpolator::GetChannelPosed(unsigned short) const
{
    if (m_spColorData)
        return false;
    if (m_kColorValue == INVALID_COLORA)
        return false;
    return true;
}
//---------------------------------------------------------------------------
void NiColorInterpolator::Collapse()
{
    if (m_spColorData)
    {
        unsigned int uiNumKeys;
        NiAnimationKey::KeyType eType;
        unsigned char ucSize;
        NiColorKey* pkKeys = m_spColorData->GetAnim(uiNumKeys, eType, ucSize);
        if (uiNumKeys == 0)
        {
            m_spColorData = NULL;
            m_kColorValue = INVALID_COLORA;
        }
        else
        {
            NiAnimationKey::IsPosedFunction isposed = 
                NiColorKey::GetIsPosedFunction(eType);
            NIASSERT(isposed);
            if (isposed(pkKeys, uiNumKeys))
            {
                m_kColorValue = pkKeys->GetKeyAt(0, ucSize)->GetColor();
                m_spColorData = NULL;
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiColorInterpolator::SetPoseValue(const NiColorA& kPoseValue)
{
    m_spColorData = NULL;
    m_kColorValue = kPoseValue;
}
//---------------------------------------------------------------------------
void NiColorInterpolator::GuaranteeTimeRange(float fStartTime, 
    float fEndTime)
{
    if (m_spColorData)
    {
        m_spColorData->GuaranteeKeysAtStartAndEnd(fStartTime, 
            fEndTime);
    }
}
//---------------------------------------------------------------------------
NiInterpolator* NiColorInterpolator::GetSequenceInterpolator(float fStartTime,
    float fEndTime)
{
    NiColorInterpolator* pkSeqInterp = (NiColorInterpolator*)
        NiKeyBasedInterpolator::GetSequenceInterpolator(fStartTime, fEndTime);
    if (m_spColorData)
    {
        NiColorDataPtr spNewColorData = m_spColorData->GetSequenceData(
            fStartTime, fEndTime);
        pkSeqInterp->SetColorData(spNewColorData);
        pkSeqInterp->m_uiLastIdx = 0;
    }

    return pkSeqInterp;
}
//---------------------------------------------------------------------------
bool NiColorInterpolator::Update(float fTime,
    NiObjectNET*, NiColorA& kValue)
{
    if (!TimeHasChanged(fTime))
    {
        kValue = m_kColorValue;  
        if (m_kColorValue == INVALID_COLORA)
            return false;
        return true;
    }

    unsigned int uiNumKeys;
    NiColorKey::KeyType eType;
    unsigned char ucSize;
    NiColorKey* pkKeys = GetKeys(uiNumKeys, eType, ucSize);
    if (uiNumKeys > 0)
    {
        m_kColorValue = NiColorKey::GenInterp(fTime, pkKeys, eType,
            uiNumKeys, m_uiLastIdx, ucSize);
    }
    if (m_kColorValue == INVALID_COLORA)
    {
        return false;
    }

    kValue = m_kColorValue;  
    m_fLastTime = fTime;
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiColorInterpolator);
//---------------------------------------------------------------------------
void NiColorInterpolator::CopyMembers(NiColorInterpolator* pkDest,
    NiCloningProcess& kCloning)
{
    NiKeyBasedInterpolator::CopyMembers(pkDest, kCloning);

    pkDest->m_kColorValue = m_kColorValue;
    pkDest->m_spColorData = m_spColorData;
    pkDest->m_uiLastIdx = m_uiLastIdx;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiColorInterpolator);

//---------------------------------------------------------------------------
void NiColorInterpolator::LoadBinary(NiStream& kStream)
{
    NiKeyBasedInterpolator::LoadBinary(kStream);

    m_kColorValue.LoadBinary(kStream);
    m_spColorData = (NiColorData*) kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiColorInterpolator::LinkObject(NiStream& kStream)
{
    NiKeyBasedInterpolator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiColorInterpolator::RegisterStreamables(NiStream& kStream)
{
    if (!NiKeyBasedInterpolator::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spColorData)
        m_spColorData->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiColorInterpolator::SaveBinary(NiStream& kStream)
{
    NiKeyBasedInterpolator::SaveBinary(kStream);

    m_kColorValue.SaveBinary(kStream);
    kStream.SaveLinkID(m_spColorData);
}
//---------------------------------------------------------------------------
bool NiColorInterpolator::IsEqual(NiObject* pkObject)
{
    if (!NiKeyBasedInterpolator::IsEqual(pkObject))
    {
        return false;
    }

    NiColorInterpolator* pkDest = (NiColorInterpolator*) pkObject;

    if (m_kColorValue != pkDest->m_kColorValue)
        return false;
    
    if ((m_spColorData && !pkDest->m_spColorData) ||
        (!m_spColorData && pkDest->m_spColorData) ||
        (m_spColorData && !m_spColorData->IsEqual(pkDest->m_spColorData)))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiColorInterpolator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiKeyBasedInterpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiColorInterpolator::ms_RTTI.GetName()));

    pkStrings->Add(m_kColorValue.GetViewerString("m_kColorValue"));
    pkStrings->Add(NiGetViewerString("m_spColorData", m_spColorData));
}
//---------------------------------------------------------------------------
