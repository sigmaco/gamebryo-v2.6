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

#include "NiPoint3Interpolator.h"

NiImplementRTTI(NiPoint3Interpolator, NiKeyBasedInterpolator);

//---------------------------------------------------------------------------
NiPoint3Interpolator::NiPoint3Interpolator() :
    m_kPoint3Value(INVALID_POINT3), m_spPoint3Data(NULL),
    m_uiLastIdx(0)
{
}
//---------------------------------------------------------------------------
NiPoint3Interpolator::NiPoint3Interpolator(NiPosData* pkPosData) :
    m_kPoint3Value(INVALID_POINT3), m_spPoint3Data(pkPosData),
    m_uiLastIdx(0)
{
}
//---------------------------------------------------------------------------
NiPoint3Interpolator::NiPoint3Interpolator(const NiPoint3& kPoseValue) :
    m_kPoint3Value(kPoseValue), m_spPoint3Data(NULL), m_uiLastIdx(0)
{
}
//---------------------------------------------------------------------------
bool NiPoint3Interpolator::IsPoint3ValueSupported() const
{
    return true;
}
//---------------------------------------------------------------------------
unsigned short NiPoint3Interpolator::GetKeyChannelCount() const
{
    return 1;
}
//---------------------------------------------------------------------------
unsigned int NiPoint3Interpolator::GetKeyCount(unsigned short)
    const 
{
    if (!m_spPoint3Data)
        return 0;
    else
        return m_spPoint3Data->GetNumKeys();
}
//---------------------------------------------------------------------------
NiAnimationKey::KeyType NiPoint3Interpolator::GetKeyType(
    unsigned short) const
{
    if (!m_spPoint3Data)
        return NiAnimationKey::NOINTERP;

    unsigned int uiNumKeys;
    NiPosKey::KeyType eType;
    unsigned char ucSize;
    m_spPoint3Data->GetAnim(uiNumKeys, eType, ucSize);
    return eType; 
}
//---------------------------------------------------------------------------
NiAnimationKey::KeyContent NiPoint3Interpolator::GetKeyContent(
    unsigned short) const
{
    return NiAnimationKey::POSKEY;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiPoint3Interpolator::GetKeyArray(unsigned short)
    const
{
    if (!m_spPoint3Data)
        return NULL;

    unsigned int uiNumKeys;
    NiPosKey::KeyType eType;
    unsigned char ucSize;
    NiPosKey * pkKeys = m_spPoint3Data->GetAnim(uiNumKeys, eType, 
        ucSize);
    return pkKeys; 
}
//---------------------------------------------------------------------------
unsigned char NiPoint3Interpolator::GetKeyStride(unsigned short)
    const
{
    if (!m_spPoint3Data)
        return 0x00;

    unsigned int uiNumKeys;
    NiPosKey::KeyType eType;
    unsigned char ucSize;
    m_spPoint3Data->GetAnim(uiNumKeys, eType, ucSize);
    return ucSize; 
}
//---------------------------------------------------------------------------
bool NiPoint3Interpolator::GetChannelPosed(unsigned short) const
{
    if (m_spPoint3Data)
        return false;
    if (m_kPoint3Value == INVALID_POINT3)
        return false;
    return true;
}
//---------------------------------------------------------------------------
void NiPoint3Interpolator::Collapse()
{
    if (m_spPoint3Data)
    {
        unsigned int uiNumKeys;
        NiAnimationKey::KeyType eType;
        unsigned char ucSize;
        NiPosKey* pkKeys = m_spPoint3Data->GetAnim(uiNumKeys, eType, ucSize);
        if (uiNumKeys == 0)
        {
            m_spPoint3Data = NULL;
            m_kPoint3Value = INVALID_POINT3;
        }
        else
        {
            NiAnimationKey::IsPosedFunction isposed = 
                NiPosKey::GetIsPosedFunction(eType);
            NIASSERT(isposed);
            if (isposed(pkKeys, uiNumKeys))
            {
                m_kPoint3Value = pkKeys->GetKeyAt(0, ucSize)->GetPos();
                m_spPoint3Data = NULL;
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiPoint3Interpolator::SetPoseValue(const NiPoint3& kPoseValue)
{
    m_spPoint3Data = NULL;
    m_kPoint3Value = kPoseValue;
}
//---------------------------------------------------------------------------
void NiPoint3Interpolator::GuaranteeTimeRange(float fStartTime, 
    float fEndTime)
{
    if (m_spPoint3Data)
    {
        m_spPoint3Data->GuaranteeKeysAtStartAndEnd(fStartTime, 
            fEndTime);
    }
}
//---------------------------------------------------------------------------
NiInterpolator* NiPoint3Interpolator::GetSequenceInterpolator(
    float fStartTime, float fEndTime)
{
    NiPoint3Interpolator* pkSeqInterp = (NiPoint3Interpolator*)
        NiKeyBasedInterpolator::GetSequenceInterpolator(fStartTime, fEndTime);
    if (m_spPoint3Data)
    {
        NiPosDataPtr spNewPoint3Data = m_spPoint3Data->GetSequenceData(
            fStartTime, fEndTime);
        pkSeqInterp->SetPoint3Data(spNewPoint3Data);
        pkSeqInterp->m_uiLastIdx = 0;
    }

    return pkSeqInterp;
}
//---------------------------------------------------------------------------
bool NiPoint3Interpolator::Update(float fTime,
    NiObjectNET*, NiPoint3& kValue)
{
    if (!TimeHasChanged(fTime))
    {
       kValue = m_kPoint3Value;
       if (m_kPoint3Value == INVALID_POINT3)
         return false;
       return true;
    }

    unsigned int uiNumKeys;
    NiPosKey::KeyType eType;
    unsigned char ucSize;
    NiPosKey* pkKeys = GetKeys(uiNumKeys, eType, ucSize);
    if (uiNumKeys > 0)
    {
        m_kPoint3Value = NiPosKey::GenInterp(fTime, pkKeys, eType,
            uiNumKeys, m_uiLastIdx, ucSize);
    }
    
    if (m_kPoint3Value == INVALID_POINT3)
    {
        return false;
    }

    kValue = m_kPoint3Value;
    m_fLastTime = fTime;
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiPoint3Interpolator);
//---------------------------------------------------------------------------
void NiPoint3Interpolator::CopyMembers(NiPoint3Interpolator* pkDest,
    NiCloningProcess& kCloning)
{
    NiKeyBasedInterpolator::CopyMembers(pkDest, kCloning);

    pkDest->m_kPoint3Value = m_kPoint3Value;
    pkDest->m_spPoint3Data = m_spPoint3Data;
    pkDest->m_uiLastIdx = m_uiLastIdx;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPoint3Interpolator);
//---------------------------------------------------------------------------
void NiPoint3Interpolator::LoadBinary(NiStream& kStream)
{
    NiKeyBasedInterpolator::LoadBinary(kStream);

    m_kPoint3Value.LoadBinary(kStream);
    m_spPoint3Data = (NiPosData*) kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiPoint3Interpolator::LinkObject(NiStream& kStream)
{
    NiKeyBasedInterpolator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiPoint3Interpolator::RegisterStreamables(NiStream& kStream)
{
    if (!NiKeyBasedInterpolator::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spPoint3Data)
        m_spPoint3Data->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiPoint3Interpolator::SaveBinary(NiStream& kStream)
{
    NiKeyBasedInterpolator::SaveBinary(kStream);

    m_kPoint3Value.SaveBinary(kStream);
    kStream.SaveLinkID(m_spPoint3Data);
}
//---------------------------------------------------------------------------
bool NiPoint3Interpolator::IsEqual(NiObject* pkObject)
{
    if (!NiKeyBasedInterpolator::IsEqual(pkObject))
    {
        return false;
    }

    NiPoint3Interpolator* pkDest = (NiPoint3Interpolator*) pkObject;

    if (m_kPoint3Value != pkDest->m_kPoint3Value)
        return false;
    
    if ((m_spPoint3Data && !pkDest->m_spPoint3Data) ||
        (!m_spPoint3Data && pkDest->m_spPoint3Data) ||
        (m_spPoint3Data && !m_spPoint3Data->IsEqual(pkDest->m_spPoint3Data)))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiPoint3Interpolator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiKeyBasedInterpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiPoint3Interpolator::ms_RTTI.GetName()));

    pkStrings->Add(m_kPoint3Value.GetViewerString("m_kPoint3Value"));
    pkStrings->Add(NiGetViewerString("m_spPoint3Data", m_spPoint3Data));
}
//---------------------------------------------------------------------------
