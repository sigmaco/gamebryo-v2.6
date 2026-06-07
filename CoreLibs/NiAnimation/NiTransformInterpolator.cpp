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

#include "NiTransformInterpolator.h"
#include "NiEulerRotKey.h"

NiImplementRTTI(NiTransformInterpolator, NiKeyBasedInterpolator);

//---------------------------------------------------------------------------
NiTransformInterpolator::NiTransformInterpolator() :
    m_spData(NULL), m_uiLastTransIdx(0), m_uiLastRotIdx(0),
    m_uiLastScaleIdx(0)
{
}
//---------------------------------------------------------------------------
NiTransformInterpolator::NiTransformInterpolator(NiTransformData* pkData) :
    m_spData(pkData), m_uiLastTransIdx(0), m_uiLastRotIdx(0),
    m_uiLastScaleIdx(0)
{
}
//---------------------------------------------------------------------------
NiTransformInterpolator::NiTransformInterpolator(
    const NiQuatTransform& kPoseValue) :
    m_kTransformValue(kPoseValue), m_spData(NULL), 
    m_uiLastTransIdx(0), m_uiLastRotIdx(0), m_uiLastScaleIdx(0)
{
}
//---------------------------------------------------------------------------
bool NiTransformInterpolator::IsTransformValueSupported() const
{
    return true;
}
//---------------------------------------------------------------------------
unsigned short NiTransformInterpolator::GetKeyChannelCount() const
{
    return 3;
}
//---------------------------------------------------------------------------
unsigned int NiTransformInterpolator::GetKeyCount(unsigned short usChannel)
    const
{
    if (usChannel == POSITION)
    {
        if (!m_spData)
            return 0;
        else
            return m_spData->GetNumPosKeys();
    }
    else if (usChannel == ROTATION)
    {
        if (! m_spData)
            return 0;
        else
            return  m_spData->GetNumRotKeys();
    }
    else if (usChannel == SCALE)
    {
        if (! m_spData)
            return 0;
        else
            return  m_spData->GetNumScaleKeys();
    }
    else
    {
        NIASSERT(usChannel < 3);
        return 0;
    }
}
//---------------------------------------------------------------------------
NiAnimationKey::KeyType NiTransformInterpolator::GetKeyType(
    unsigned short usChannel) const
{
    if (usChannel == POSITION)
    {
        if (!m_spData)
            return NiAnimationKey::NOINTERP;

        unsigned int uiNumKeys;
        NiPosKey::KeyType eType;
        unsigned char ucSize;
        m_spData->GetPosAnim(uiNumKeys, eType, ucSize);
        return eType; 
    }
    else if (usChannel == ROTATION)
    {
        if (!m_spData)
            return NiAnimationKey::NOINTERP;

        unsigned int uiNumKeys;
        NiRotKey::KeyType eType;
        unsigned char ucSize;
        m_spData->GetRotAnim(uiNumKeys, eType, ucSize);
        return eType;     
    }
    else if (usChannel == SCALE)
    {
        if (!m_spData)
            return NiAnimationKey::NOINTERP;

        unsigned int uiNumKeys;
        NiFloatKey::KeyType eType;
        unsigned char ucSize;
        m_spData->GetScaleAnim(uiNumKeys, eType, ucSize);
        return eType;     
    }
    else
    {
        NIASSERT(usChannel < 3);
        return NiAnimationKey::NOINTERP;
    }
    
}
//---------------------------------------------------------------------------
NiAnimationKey::KeyContent NiTransformInterpolator::GetKeyContent(
    unsigned short usChannel) const
{
    if (usChannel == POSITION)
        return NiAnimationKey::POSKEY;
    else if (usChannel == ROTATION)
        return NiAnimationKey::ROTKEY;
    else if (usChannel == SCALE)
        return NiAnimationKey::FLOATKEY;
    
    NIASSERT(usChannel < 3);
    return NiAnimationKey::NUMKEYCONTENTS;
}
//---------------------------------------------------------------------------
NiAnimationKey* NiTransformInterpolator::GetKeyArray(unsigned short usChannel)
    const
{
    if (usChannel == POSITION)
    {       
        if (!m_spData)
            return NULL;

        unsigned int uiNumKeys;
        NiPosKey::KeyType eType;
        unsigned char ucSize;
        NiPosKey * pkKeys = m_spData->GetPosAnim(uiNumKeys, eType, 
            ucSize);
        return pkKeys; 
    }
    else if (usChannel == ROTATION)
    {
        if (!m_spData)
            return NULL;
    
        unsigned int uiNumKeys;
        NiRotKey::KeyType eType;
        unsigned char ucSize;
        NiRotKey * pkKeys = m_spData->GetRotAnim(uiNumKeys, eType, 
            ucSize);
        return pkKeys; 
    }
    else if (usChannel == SCALE)
    {
        if (!m_spData)
            return NULL;
    
        unsigned int uiNumKeys;
        NiFloatKey::KeyType eType;
        unsigned char ucSize;
        NiFloatKey * pkKeys = m_spData->GetScaleAnim(uiNumKeys, eType, 
            ucSize);
        return pkKeys; 
    }
    else
    {
        NIASSERT(usChannel < 3);
        return NULL;
    }
}
//---------------------------------------------------------------------------
unsigned char NiTransformInterpolator::GetKeyStride(unsigned short usChannel)
    const
{
    if (usChannel == POSITION)
    {       
        if (!m_spData)
            return 0;

        unsigned int uiNumKeys;
        NiPosKey::KeyType eType;
        unsigned char ucSize;
        m_spData->GetPosAnim(uiNumKeys, eType, ucSize);
        return ucSize; 
    }
    else if (usChannel == ROTATION)
    {
        if (!m_spData)
            return 0;
    
        unsigned int uiNumKeys;
        NiRotKey::KeyType eType;
        unsigned char ucSize;
        m_spData->GetRotAnim(uiNumKeys, eType, ucSize);
        return ucSize; 
    }
    else if (usChannel == SCALE)
    {
        if (!m_spData)
            return 0;
    
        unsigned int uiNumKeys;
        NiFloatKey::KeyType eType;
        unsigned char ucSize;
        m_spData->GetScaleAnim(uiNumKeys, eType, ucSize);
        return ucSize; 
    }
    else
    {
        NIASSERT(usChannel < 3);
        return 0;
    }
}
//---------------------------------------------------------------------------
void NiTransformInterpolator::GetActiveTimeRange(float& fBeginKeyTime,
    float& fEndKeyTime) const
{
    fBeginKeyTime = NI_INFINITY;
    fEndKeyTime = -NI_INFINITY;
    bool bKeys = false;

    unsigned int uiNumKeys;
    NiPosKey::KeyType eTransType;
    unsigned char ucSize;
    NiPosKey* pkTransKeys = GetPosData(uiNumKeys, eTransType, ucSize);
    if (uiNumKeys > 0)
    {
        float fKeyTime = pkTransKeys->GetKeyAt(0, ucSize)->GetTime();
        if (fKeyTime < fBeginKeyTime)
        {
            fBeginKeyTime = fKeyTime;
        }
        fKeyTime = pkTransKeys->GetKeyAt(uiNumKeys - 1, ucSize)->GetTime();
        if (fKeyTime > fEndKeyTime)
        {
            fEndKeyTime = fKeyTime;
        }
        bKeys = true;
    }

    NiRotKey::KeyType eRotType;
    NiRotKey* pkRotKeys = GetRotData(uiNumKeys, eRotType, ucSize);
    if (uiNumKeys > 0)
    {
        if (eRotType == NiRotKey::EULERKEY)
        {
            NiEulerRotKey* pkEulerKey = (NiEulerRotKey*) pkRotKeys->GetKeyAt(
                0, ucSize);

            for (unsigned char uc = 0; uc < 3; uc++)
            {
                unsigned int uiNumFloatKeys = pkEulerKey->GetNumKeys(uc);
                if (uiNumFloatKeys > 0)
                {
                    NiFloatKey* pkFloatKeys = pkEulerKey->GetKeys(uc);
                    unsigned char ucFloatKeySize = pkEulerKey->GetKeySize(uc);
                    float fKeyTime = pkFloatKeys->GetKeyAt(0, 
                        ucFloatKeySize)->GetTime();
                    if (fKeyTime < fBeginKeyTime)
                    {
                        fBeginKeyTime = fKeyTime;
                    }
                    fKeyTime = pkFloatKeys->GetKeyAt(uiNumFloatKeys - 1, 
                        ucFloatKeySize)->GetTime();
                    if (fKeyTime > fEndKeyTime)
                    {
                        fEndKeyTime = fKeyTime;
                    }
                    bKeys = true;
                }
            }
        }
        else
        {
            float fKeyTime = pkRotKeys->GetKeyAt(0, ucSize)->GetTime();
            if (fKeyTime < fBeginKeyTime)
            {
                fBeginKeyTime = fKeyTime;
            }
            fKeyTime = pkRotKeys->GetKeyAt(uiNumKeys - 1, ucSize)->GetTime();
            if (fKeyTime > fEndKeyTime)
            {
                fEndKeyTime = fKeyTime;
            }
            bKeys = true;
        }
    }

    NiFloatKey::KeyType eScaleType;
    NiFloatKey* pkScaleKeys = GetScaleData(uiNumKeys, eScaleType, ucSize);
    if (uiNumKeys > 0)
    {
        float fKeyTime = pkScaleKeys->GetKeyAt(0, ucSize)->GetTime();
        if (fKeyTime < fBeginKeyTime)
        {
            fBeginKeyTime = fKeyTime;
        }
        fKeyTime = pkScaleKeys->GetKeyAt(uiNumKeys - 1, ucSize)->GetTime();
        if (fKeyTime > fEndKeyTime)
        {
            fEndKeyTime = fKeyTime;
        }
        bKeys = true;
    }

    if (!bKeys)
    {
        fBeginKeyTime = 0.0f;
        fEndKeyTime = 0.0f;
    }
}
//---------------------------------------------------------------------------
void NiTransformInterpolator::GuaranteeTimeRange(float fStartTime, 
    float fEndTime)
{
    if (m_spData)
    {
        m_spData->GuaranteeKeysAtStartAndEnd(fStartTime, 
            fEndTime);
    }
}
//---------------------------------------------------------------------------
NiInterpolator* NiTransformInterpolator::GetSequenceInterpolator(
    float fStartTime, float fEndTime)
{
    NiTransformInterpolator* pkSeqInterp = (NiTransformInterpolator*)
        NiKeyBasedInterpolator::GetSequenceInterpolator(fStartTime, fEndTime);

    if (m_spData)
    {
        NiTransformDataPtr spNewData = m_spData->GetSequenceData(fStartTime,
            fEndTime);
        pkSeqInterp->SetTransformData(spNewData);
        pkSeqInterp->m_uiLastTransIdx = 0;
        pkSeqInterp->m_uiLastRotIdx = 0;
        pkSeqInterp->m_uiLastScaleIdx = 0;
    }

    return pkSeqInterp;
}
//---------------------------------------------------------------------------
bool NiTransformInterpolator::Update(float fTime, NiObjectNET*,
    NiQuatTransform& kValue)
{
    if (!TimeHasChanged(fTime))
    {
        kValue = m_kTransformValue;
            
        if (m_kTransformValue.IsTransformInvalid())
            return false;
        return true;
    }

    // Compute translation value.
    unsigned int uiNumKeys;
    NiPosKey::KeyType eTransType;
    unsigned char ucSize;
    NiPosKey* pkTransKeys = GetPosData(uiNumKeys, eTransType, ucSize);
    if (uiNumKeys > 0)
    {
        m_kTransformValue.SetTranslate(NiPosKey::GenInterp(fTime, pkTransKeys,
            eTransType, uiNumKeys, m_uiLastTransIdx, ucSize));
    }

    // Compute rotation value.
    NiRotKey::KeyType eRotType;
    NiRotKey* pkRotKeys = GetRotData(uiNumKeys, eRotType, ucSize);
    if (uiNumKeys > 0)
    {
        NiRotKey::GenInterp(fTime, pkRotKeys, eRotType, uiNumKeys, 
            m_uiLastRotIdx, ucSize, m_kTransformValue.GetRotatePointer());
    }

    // Compute scale value.
    NiFloatKey::KeyType eScaleType;
    NiFloatKey* pkScaleKeys = GetScaleData(uiNumKeys, eScaleType, ucSize);
    if (uiNumKeys > 0)
    {
        m_kTransformValue.SetScale(NiFloatKey::GenInterp(fTime, pkScaleKeys,
            eScaleType, uiNumKeys, m_uiLastScaleIdx, ucSize));
    }
    
    kValue = m_kTransformValue;
    if (m_kTransformValue.IsTransformInvalid())
    {
        return false;
    }

    m_fLastTime = fTime;
    return true;
}
//---------------------------------------------------------------------------
bool NiTransformInterpolator::GetChannelPosed(unsigned short usChannel) const
{
    switch(usChannel)
    {
        case POSITION:
            if (m_spData && GetKeyCount(usChannel) > 0)
                return false;
            else 
                return m_kTransformValue.IsTranslateValid();
        case ROTATION:
            if (m_spData && GetKeyCount(usChannel) > 0)
                return false;
            else 
                return m_kTransformValue.IsRotateValid();
        case SCALE:
            if (m_spData && GetKeyCount(usChannel) > 0)
                return false;
            else 
                return m_kTransformValue.IsScaleValid();
        default:
            return false;
    }
}
//---------------------------------------------------------------------------
void NiTransformInterpolator::Collapse()
{
    if (!m_spData)
    {
        return;
    }

    NiQuatTransform kCollapsedTransform;

    unsigned int uiNumKeys;
    NiAnimationKey::KeyType eType;
    unsigned char ucSize;

    // Collapse position keys.
    NiPosKey* pkPosKeys = m_spData->GetPosAnim(uiNumKeys, eType, ucSize);
    if (uiNumKeys == 0)
    {
        m_spData->ReplacePosAnim(NULL, 0, NiAnimationKey::NOINTERP);
        if (m_kTransformValue.IsTranslateValid())
        {
            kCollapsedTransform.SetTranslate(
                m_kTransformValue.GetTranslate());
        }
    }
    else
    {
        NiAnimationKey::IsPosedFunction isposed = 
            NiPosKey::GetIsPosedFunction(eType);
        NIASSERT(isposed);
        if (isposed(pkPosKeys, uiNumKeys))
        {
            NiPoint3 kValue = pkPosKeys->GetKeyAt(0, ucSize)->GetPos();
            kCollapsedTransform.SetTranslate(kValue);
            m_spData->ReplacePosAnim(NULL, 0, NiAnimationKey::NOINTERP);
        }
        else if (m_kTransformValue.IsTranslateValid())
        {
            kCollapsedTransform.SetTranslate(
                m_kTransformValue.GetTranslate());
        }
    }

    // Collapse rotation keys.
    NiRotKey* pkRotKeys = m_spData->GetRotAnim(uiNumKeys, eType, ucSize);
    if (uiNumKeys == 0)
    {
        m_spData->ReplaceRotAnim(NULL, 0, NiAnimationKey::NOINTERP);
        if (m_kTransformValue.IsRotateValid())
        {
            kCollapsedTransform.SetRotate(m_kTransformValue.GetRotate());
        }
    }
    else
    {
        NiAnimationKey::IsPosedFunction isposed = 
            NiRotKey::GetIsPosedFunction(eType);
        NIASSERT(isposed);
        if (isposed(pkRotKeys, uiNumKeys))
        {
            NiQuaternion kValue;
            if (eType == NiRotKey::EULERKEY)
            {
                NiRotKey::InterpFunction interp = 
                    NiRotKey::GetInterpFunction(eType);
                NIASSERT(interp);
                interp(0.0f, pkRotKeys->GetKeyAt(0, ucSize), 0, &kValue);
            }
            else
            {
                kValue = pkRotKeys->GetKeyAt(0, ucSize)->GetQuaternion();
            }
            kCollapsedTransform.SetRotate(kValue);
            m_spData->ReplaceRotAnim(NULL, 0, NiAnimationKey::NOINTERP);
        }
        else if (m_kTransformValue.IsRotateValid())
        {
            kCollapsedTransform.SetRotate(m_kTransformValue.GetRotate());
        }
    }

    // Collapse scale keys.
    NiFloatKey* pkScaleKeys = m_spData->GetScaleAnim(uiNumKeys, eType, ucSize);
    if (uiNumKeys == 0)
    {
        m_spData->ReplaceScaleAnim(NULL, 0, NiAnimationKey::NOINTERP);
        if (m_kTransformValue.IsScaleValid())
        {
            kCollapsedTransform.SetScale(m_kTransformValue.GetScale());
        }
    }
    else
    {
        NiAnimationKey::IsPosedFunction isposed = 
            NiFloatKey::GetIsPosedFunction(eType);
        NIASSERT(isposed);
        if (isposed(pkScaleKeys, uiNumKeys))
        {
            float fValue = pkScaleKeys->GetKeyAt(0, ucSize)->GetValue();
            kCollapsedTransform.SetScale(fValue);
            m_spData->ReplaceScaleAnim(NULL, 0, NiAnimationKey::NOINTERP);
        }
        else if (m_kTransformValue.IsScaleValid())
        {
            kCollapsedTransform.SetScale(m_kTransformValue.GetScale());
        }
    }

    if (m_spData->GetNumPosKeys() == 0 && m_spData->GetNumRotKeys() == 0 &&
        m_spData->GetNumScaleKeys() == 0)
    {
        m_spData = NULL;
    }
    m_kTransformValue = kCollapsedTransform;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiTransformInterpolator);
//---------------------------------------------------------------------------
void NiTransformInterpolator::CopyMembers(NiTransformInterpolator* pkDest,
    NiCloningProcess& kCloning)
{
    NiKeyBasedInterpolator::CopyMembers(pkDest, kCloning);

    pkDest->m_kTransformValue = m_kTransformValue;

    pkDest->m_spData = m_spData;
    pkDest->m_uiLastTransIdx = m_uiLastTransIdx;
    pkDest->m_uiLastRotIdx = m_uiLastRotIdx;
    pkDest->m_uiLastScaleIdx = m_uiLastScaleIdx;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiTransformInterpolator);
//---------------------------------------------------------------------------
void NiTransformInterpolator::LoadBinary(NiStream& kStream)
{
    NiKeyBasedInterpolator::LoadBinary(kStream);

    m_kTransformValue.LoadBinary(kStream);

    m_spData = (NiTransformData*) kStream.ResolveLinkID();
}
//---------------------------------------------------------------------------
void NiTransformInterpolator::LinkObject(NiStream& kStream)
{
    NiKeyBasedInterpolator::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiTransformInterpolator::RegisterStreamables(NiStream& kStream)
{
    if (!NiKeyBasedInterpolator::RegisterStreamables(kStream))
    {
        return false;
    }

    if (m_spData)
        m_spData->RegisterStreamables(kStream);

    return true;
}
//---------------------------------------------------------------------------
void NiTransformInterpolator::SaveBinary(NiStream& kStream)
{
    NiKeyBasedInterpolator::SaveBinary(kStream);

    m_kTransformValue.SaveBinary(kStream);
    kStream.SaveLinkID(m_spData);
}
//---------------------------------------------------------------------------
bool NiTransformInterpolator::IsEqual(NiObject* pkObject)
{
    if (!NiKeyBasedInterpolator::IsEqual(pkObject))
    {
        return false;
    }

    NiTransformInterpolator* pkDest = (NiTransformInterpolator*) pkObject;

    if (m_kTransformValue != pkDest->m_kTransformValue)
    {
        return false;
    }

    if ((m_spData && !pkDest->m_spData) ||
        (!m_spData && pkDest->m_spData) ||
        (m_spData && !m_spData->IsEqual(pkDest->m_spData)))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Viewer strings
//---------------------------------------------------------------------------
void NiTransformInterpolator::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiKeyBasedInterpolator::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiTransformInterpolator::ms_RTTI
        .GetName()));

    m_kTransformValue.GetViewerStrings(pkStrings);
    
    if (m_spData)
        m_spData->GetViewerStrings(pkStrings);
}
//---------------------------------------------------------------------------
