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

#include "NiEulerRotKey.h"
#include "NiTransformData.h"

NiImplementRTTI(NiTransformData,NiObject);

//---------------------------------------------------------------------------
NiTransformData::NiTransformData()
{
    m_uiNumRotKeys = 0;
    m_pkRotKeys = 0;
    m_eRotType = NiAnimationKey::NOINTERP;
    m_ucRotSize = 0;

    m_uiNumPosKeys = 0;
    m_pkPosKeys = 0;
    m_ePosType = NiAnimationKey::NOINTERP;
    m_ucPosSize = 0;

    m_uiNumScaleKeys = 0;
    m_pkScaleKeys = 0;
    m_eScaleType = NiAnimationKey::NOINTERP;
    m_ucScaleSize = 0;
}
//---------------------------------------------------------------------------
NiTransformData::~NiTransformData ()
{
    CleanRotAnim(m_pkRotKeys, m_uiNumRotKeys, m_eRotType);

    if (m_pkPosKeys)
    {
        NiPosKey::DeleteFunction pfnPosDeleteFunc =
            NiPosKey::GetDeleteFunction(m_ePosType);
        NIASSERT(pfnPosDeleteFunc);
        pfnPosDeleteFunc(m_pkPosKeys);
    }

    if (m_pkScaleKeys)
    {
        NiFloatKey::DeleteFunction pfnScaleDeleteFunc =
            NiFloatKey::GetDeleteFunction(m_eScaleType);
        NIASSERT(pfnScaleDeleteFunc);
        pfnScaleDeleteFunc(m_pkScaleKeys);
    }
}
//---------------------------------------------------------------------------
void NiTransformData::ReplaceRotAnim(NiRotKey* pkKeys,
    unsigned int uiNumKeys, NiRotKey::KeyType eType)
{
    // Delete old copies of data
    CleanRotAnim(m_pkRotKeys, m_uiNumRotKeys, m_eRotType);

    SetRotAnim(pkKeys, uiNumKeys, eType);
}
//---------------------------------------------------------------------------
void NiTransformData::ReplacePosAnim(NiPosKey* pkKeys,
    unsigned int uiNumKeys, NiPosKey::KeyType eType)
{
    // Delete old copies of data
    if (m_pkPosKeys)
    {
        NiPosKey::DeleteFunction pfnDeleteFunc =
            NiPosKey::GetDeleteFunction(m_ePosType);
        NIASSERT(pfnDeleteFunc);
        pfnDeleteFunc(m_pkPosKeys);
    }

    SetPosAnim(pkKeys, uiNumKeys, eType);
}
//---------------------------------------------------------------------------
void NiTransformData::ReplaceScaleAnim(NiFloatKey* pkKeys,
    unsigned int uiNumKeys, NiFloatKey::KeyType eType)
{
    // Delete old copies of data
    if (m_pkScaleKeys)
    {
        NiFloatKey::DeleteFunction pfnDeleteFunc =
            NiFloatKey::GetDeleteFunction(m_eScaleType);
        NIASSERT(pfnDeleteFunc);
        pfnDeleteFunc(m_pkScaleKeys);
    }

    SetScaleAnim(pkKeys, uiNumKeys, eType);
}
//---------------------------------------------------------------------------
void NiTransformData::SetRotAnim(NiRotKey* pkKeys,
    unsigned int uiNumKeys, NiRotKey::KeyType eType)
{
    if (!pkKeys || uiNumKeys == 0)
    {
        m_uiNumRotKeys = 0;
        m_pkRotKeys = 0;
        m_eRotType = NiAnimationKey::NOINTERP;
        m_ucRotSize = 0;
        return;
    }

    m_uiNumRotKeys = uiNumKeys;
    m_pkRotKeys = pkKeys;
    m_eRotType = eType;

    m_ucRotSize = NiRotKey::GetKeySize(eType);
    
}
//---------------------------------------------------------------------------
void NiTransformData::SetPosAnim(NiPosKey* pkKeys,
    unsigned int uiNumKeys, NiPosKey::KeyType eType)
{
    if (!pkKeys || uiNumKeys == 0)
    {
        m_uiNumPosKeys = 0;
        m_pkPosKeys = 0;
        m_ePosType = NiAnimationKey::NOINTERP;
        m_ucPosSize = 0;
        return;
    }

    m_uiNumPosKeys = uiNumKeys;
    m_pkPosKeys = pkKeys;
    m_ePosType = eType;

    m_ucPosSize = NiPosKey::GetKeySize(eType);
    
}
//---------------------------------------------------------------------------
void NiTransformData::SetScaleAnim(NiFloatKey* pkKeys,
    unsigned int uiNumKeys, NiFloatKey::KeyType eType)
{
    if (!pkKeys || uiNumKeys == 0)
    {
        m_uiNumScaleKeys = 0;
        m_pkScaleKeys = 0;
        m_ucScaleSize = 0;
        m_eScaleType = NiAnimationKey::NOINTERP;
        return;
    }

    m_uiNumScaleKeys = uiNumKeys;
    m_pkScaleKeys = pkKeys;
    m_eScaleType = eType;    
    
    m_ucScaleSize = NiFloatKey::GetKeySize(eType);
    

}
//---------------------------------------------------------------------------
void NiTransformData::CleanRotAnim(NiRotKey* pkRotKeys,
    unsigned int, NiRotKey::KeyType eType)
{
    if (pkRotKeys && eType == NiRotKey::EULERKEY)
    {
        ((NiEulerRotKey*) pkRotKeys->GetKeyAt(0, m_ucRotSize))->CleanUp();
    }

    if (pkRotKeys)
    {
        NiRotKey::DeleteFunction pfnDeleteFunc =
            NiRotKey::GetDeleteFunction(m_eRotType);
        NIASSERT(pfnDeleteFunc);
        pfnDeleteFunc(pkRotKeys);
    }
}
//---------------------------------------------------------------------------
void NiTransformData::GuaranteeKeysAtStartAndEnd(float fStartTime,
    float fEndTime)
{
    // rotation keys 
    NiAnimationKey* pkKeys;
    if (m_uiNumRotKeys != 0)
    {
        pkKeys = (NiAnimationKey*)m_pkRotKeys;
        NiAnimationKey::GuaranteeKeyAtStartAndEnd(NiAnimationKey::ROTKEY,
            m_eRotType, pkKeys, m_uiNumRotKeys, fStartTime, fEndTime);
        m_pkRotKeys = (NiRotKey*)pkKeys;
    }

    // position keys
    if (m_uiNumPosKeys != 0)
    {
        pkKeys = (NiAnimationKey*)m_pkPosKeys;
        NiAnimationKey::GuaranteeKeyAtStartAndEnd(NiAnimationKey::POSKEY,
            m_ePosType, pkKeys, m_uiNumPosKeys, fStartTime, fEndTime);
        m_pkPosKeys = (NiPosKey*)pkKeys;
    }

    // scale keys
    if (m_uiNumScaleKeys != 0)
    {
        pkKeys = (NiAnimationKey*)m_pkScaleKeys;
        NiAnimationKey::GuaranteeKeyAtStartAndEnd(NiAnimationKey::FLOATKEY,
            m_eScaleType, pkKeys, m_uiNumScaleKeys, fStartTime, fEndTime);
        m_pkScaleKeys = (NiFloatKey*)pkKeys;
    }
}
//---------------------------------------------------------------------------
NiTransformDataPtr NiTransformData::GetSequenceData(float fStartTime,
    float fEndTime)
{
    NiTransformDataPtr spNewData = NiSmartPointerCast(NiTransformData,
        CreateDeepCopy());

    NiAnimationKey* pkNewKeys = NULL;
    unsigned int uiNewNumKeys = 0;

    if (m_uiNumRotKeys > 0)
    {
        NiAnimationKey::CopySequence(NiAnimationKey::ROTKEY, m_eRotType,
            m_pkRotKeys, m_uiNumRotKeys, fStartTime, fEndTime, pkNewKeys,
            uiNewNumKeys);
        spNewData->ReplaceRotAnim((NiRotKey*) pkNewKeys, uiNewNumKeys,
            m_eRotType);
    }

    if (m_uiNumPosKeys > 0)
    {
        NiAnimationKey::CopySequence(NiAnimationKey::POSKEY, m_ePosType,
            m_pkPosKeys, m_uiNumPosKeys, fStartTime, fEndTime, pkNewKeys,
            uiNewNumKeys);
        spNewData->ReplacePosAnim((NiPosKey*) pkNewKeys, uiNewNumKeys,
            m_ePosType);
    }

    if (m_uiNumScaleKeys > 0)
    {
        NiAnimationKey::CopySequence(NiAnimationKey::FLOATKEY, m_eScaleType,
            m_pkScaleKeys, m_uiNumScaleKeys, fStartTime, fEndTime, pkNewKeys,
            uiNewNumKeys);
        spNewData->ReplaceScaleAnim((NiFloatKey*) pkNewKeys, uiNewNumKeys,
            m_eScaleType);
    }

    return spNewData;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiTransformData);
//---------------------------------------------------------------------------
void NiTransformData::LoadBinary(NiStream& stream)
{
    NiObject::LoadBinary(stream);

    // load rotation keys
    unsigned int uiNumKeys;
    NiStreamLoadBinary(stream, uiNumKeys);
    if ( uiNumKeys > 0 )
    {
        // load type of rotation
        NiRotKey::KeyType eRotType;
        NiStreamLoadEnum(stream, eRotType);
        unsigned char ucSize = NiRotKey::GetKeySize(eRotType);
        
        // load rotations
        NiRotKey::CreateFunction cf = NiRotKey::GetCreateFunction(eRotType);
        NIASSERT(cf);
        NiRotKey* pkRotKeys = (NiRotKey*) cf(stream, uiNumKeys);
        NIASSERT(pkRotKeys);

        NiRotKey::FillDerivedValsFunction derived =
            NiRotKey::GetFillDerivedFunction(eRotType);
        NIASSERT(derived);
        derived(pkRotKeys, uiNumKeys,ucSize);
        ReplaceRotAnim(pkRotKeys, uiNumKeys, eRotType);
    }
    
    // load position keys
    NiStreamLoadBinary(stream, uiNumKeys);
    if ( uiNumKeys > 0 )
    {
        // load type of position
        NiPosKey::KeyType ePosType;
        NiStreamLoadEnum(stream, ePosType);
        unsigned char ucSize = NiPosKey::GetKeySize(ePosType);

        // load positions
        NiPosKey::CreateFunction cf = NiPosKey::GetCreateFunction(ePosType);
        NIASSERT(cf);
        NiPosKey* pkPosKeys = (NiPosKey*) cf(stream, uiNumKeys);
        NIASSERT(pkPosKeys);

        NiPosKey::FillDerivedValsFunction derived =
            NiPosKey::GetFillDerivedFunction(ePosType);
        NIASSERT(derived);
        derived(pkPosKeys, uiNumKeys, ucSize);
        ReplacePosAnim(pkPosKeys, uiNumKeys, ePosType);
    }
    
    // load scale keys
    NiStreamLoadBinary(stream, uiNumKeys);
    if ( uiNumKeys > 0 )
    {
        // load type of scale
        NiFloatKey::KeyType eScaleType;
        NiStreamLoadEnum(stream, eScaleType);
        unsigned char ucSize = NiFloatKey::GetKeySize(eScaleType);

        // load scale
        NiFloatKey::CreateFunction cf =
            NiFloatKey::GetCreateFunction(eScaleType);
        NIASSERT(cf);
        NiFloatKey* pkScaleKeys = (NiFloatKey*) cf(stream, uiNumKeys);
        NIASSERT(pkScaleKeys);

        NiFloatKey::FillDerivedValsFunction derived =
            NiFloatKey::GetFillDerivedFunction(eScaleType);
        NIASSERT(derived);
        derived(pkScaleKeys, uiNumKeys,ucSize);
        ReplaceScaleAnim(pkScaleKeys, uiNumKeys, eScaleType);
    }
}
//---------------------------------------------------------------------------
void NiTransformData::LinkObject(NiStream& stream)
{
    NiObject::LinkObject(stream);
}
//---------------------------------------------------------------------------
bool NiTransformData::RegisterStreamables(NiStream& stream)
{
    return NiObject::RegisterStreamables(stream);
}
//---------------------------------------------------------------------------
void NiTransformData::SaveBinary(NiStream& stream)
{
    NiObject::SaveBinary(stream);

    // if we have an Euler rotation it must have only one key
    NIASSERT(m_eRotType != NiRotKey::EULERKEY || m_uiNumRotKeys == 1);

    // save rotation keys
    NiStreamSaveBinary(stream, m_uiNumRotKeys);
    if (m_uiNumRotKeys > 0)
    {
        // save type of rotation
        NiStreamSaveEnum(stream, m_eRotType);

        // save rotations
        NiRotKey::SaveFunction sf = NiRotKey::GetSaveFunction(m_eRotType);
        NIASSERT(sf);
        sf(stream, m_pkRotKeys, m_uiNumRotKeys);
    }

    // save position keys
    NiStreamSaveBinary(stream, m_uiNumPosKeys);
    if (m_uiNumPosKeys > 0)
    {
        // save type of position
        NiStreamSaveEnum(stream, m_ePosType);

        // save positions
        NiPosKey::SaveFunction sf = NiPosKey::GetSaveFunction(m_ePosType);
        NIASSERT(sf);
        sf(stream, m_pkPosKeys, m_uiNumPosKeys);
    }

    // save scale keys
    NiStreamSaveBinary(stream, m_uiNumScaleKeys);
    if (m_uiNumScaleKeys > 0)
    {
        // save type of scale
        NiStreamSaveEnum(stream, m_eScaleType);

        // save scales
        NiFloatKey::SaveFunction sf =
            NiFloatKey::GetSaveFunction(m_eScaleType);
        NIASSERT(sf);
        sf(stream, m_pkScaleKeys, m_uiNumScaleKeys);
    }
}
//---------------------------------------------------------------------------
bool NiTransformData::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
    {
        return false;
    }

    // if we have an euler rotation it must have only one key
    NIASSERT(m_eRotType != NiRotKey::EULERKEY || m_uiNumRotKeys == 1);

    NiTransformData* pkData = (NiTransformData*) pkObject;

    if (m_uiNumRotKeys != pkData->m_uiNumRotKeys ||
        m_eRotType != pkData->m_eRotType ||
        m_uiNumPosKeys != pkData->m_uiNumPosKeys ||
        m_ePosType != pkData->m_ePosType ||
        m_uiNumScaleKeys != pkData->m_uiNumScaleKeys ||
        m_eScaleType != pkData->m_eScaleType ||
        m_ucRotSize != pkData->m_ucRotSize ||
        m_ucPosSize != pkData->m_ucPosSize ||
        m_ucScaleSize != pkData->m_ucScaleSize)
    {
        return false;
    }

    NiRotKey::EqualFunction rotEqual =
        NiRotKey::GetEqualFunction(m_eRotType);
    NIASSERT(rotEqual);
    unsigned int uiI;
    for (uiI = 0; uiI < m_uiNumRotKeys; uiI++)
    {
        if (!rotEqual(*m_pkRotKeys->GetKeyAt(uiI, m_ucRotSize),
            *pkData->m_pkRotKeys->GetKeyAt(uiI, m_ucRotSize)))
        {
            return false;
        }
    }

    NiPosKey::EqualFunction posEqual =
        NiPosKey::GetEqualFunction(m_ePosType);
    NIASSERT(posEqual);
    for (uiI = 0; uiI < m_uiNumPosKeys; uiI++)
    {
        if (!posEqual(*m_pkPosKeys->GetKeyAt(uiI, m_ucPosSize),
            *pkData->m_pkPosKeys->GetKeyAt(uiI, m_ucPosSize)))
        {
            return false;
        }
    }

    NiFloatKey::EqualFunction fltEqual =
        NiFloatKey::GetEqualFunction(m_eScaleType);
    NIASSERT(fltEqual);
    for (uiI = 0; uiI < m_uiNumScaleKeys; uiI++)
    {
        if (!fltEqual(*m_pkScaleKeys->GetKeyAt(uiI, m_ucScaleSize),
            *pkData->m_pkScaleKeys->GetKeyAt(uiI, m_ucScaleSize)))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiTransformData::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiObject::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiTransformData::ms_RTTI.GetName()));

    pStrings->Add(NiGetViewerString("m_uiNumRotKeys",m_uiNumRotKeys));
    pStrings->Add(NiGetViewerString("m_uiNumPosKeys",m_uiNumPosKeys));
    pStrings->Add(NiGetViewerString("m_uiNumScaleKeys",m_uiNumScaleKeys));
}
//---------------------------------------------------------------------------
