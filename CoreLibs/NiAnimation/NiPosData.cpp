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

#include "NiPosData.h"

NiImplementRTTI(NiPosData,NiObject);

//---------------------------------------------------------------------------
NiPosData::~NiPosData ()
{
    if (m_pkKeys)
    {
        NiPosKey::DeleteFunction pfnDeleteFunc =
            NiPosKey::GetDeleteFunction(m_eType);
        NIASSERT(pfnDeleteFunc);
        pfnDeleteFunc(m_pkKeys);
    }
}
//---------------------------------------------------------------------------
void NiPosData::ReplaceAnim(NiPosKey* pkKeys,
    unsigned int uiNumKeys, NiPosKey::KeyType eType)
{
    // Delete old copies of data
    if (m_pkKeys)
    {
        NiPosKey::DeleteFunction pfnDeleteFunc =
            NiPosKey::GetDeleteFunction(m_eType);
        NIASSERT(pfnDeleteFunc);
        pfnDeleteFunc(m_pkKeys);
    }

    SetAnim(pkKeys, uiNumKeys, eType);
}
//---------------------------------------------------------------------------
void NiPosData::SetAnim(NiPosKey* pkKeys,
    unsigned int uiNumKeys, NiPosKey::KeyType eType)
{
    if (!pkKeys || uiNumKeys == 0)
    {
        m_uiNumKeys = 0;
        m_pkKeys = 0;
        m_eType = NiAnimationKey::NOINTERP;
        m_ucKeySize = 0;
        return;
    }

    m_ucKeySize = NiPosKey::GetKeySize(eType);

    m_uiNumKeys = uiNumKeys;
    m_pkKeys = pkKeys;
    m_eType = eType;
}
//---------------------------------------------------------------------------
void NiPosData::GuaranteeKeysAtStartAndEnd(float fStartTime, float fEndTime)
{
    // This should not exist without any keys
    NIASSERT(m_uiNumKeys != 0);

    NiAnimationKey* pkKeys = (NiAnimationKey*)m_pkKeys;
    NiAnimationKey::GuaranteeKeyAtStartAndEnd(NiAnimationKey::POSKEY,
        m_eType, pkKeys, m_uiNumKeys, fStartTime, fEndTime);
    m_pkKeys = (NiPosKey*)pkKeys;
}
//---------------------------------------------------------------------------
NiPosDataPtr NiPosData::GetSequenceData(float fStartTime, float fEndTime)
{
    NiPosDataPtr spNewData = NiSmartPointerCast(NiPosData, CreateDeepCopy());

    if (m_uiNumKeys > 0)
    {
        NiAnimationKey* pkNewKeys = NULL;
        unsigned int uiNewNumKeys = 0;
        NiAnimationKey::CopySequence(NiAnimationKey::POSKEY, m_eType,
            m_pkKeys, m_uiNumKeys, fStartTime, fEndTime, pkNewKeys,
            uiNewNumKeys);
        spNewData->ReplaceAnim((NiPosKey*) pkNewKeys, uiNewNumKeys,
            m_eType);
    }

    return spNewData;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPosData);
//---------------------------------------------------------------------------
void NiPosData::LoadBinary(NiStream& stream)
{
    NiObject::LoadBinary(stream);

    // load position keys
    unsigned int uiNumKeys;
    NiStreamLoadBinary(stream, uiNumKeys);
    if ( uiNumKeys > 0 )
    {
        // load type of position
        NiPosKey::KeyType ePosType;
        NiStreamLoadEnum(stream,ePosType);
        m_ucKeySize = NiPosKey::GetKeySize(ePosType);

        // load positions
        NiPosKey::CreateFunction cf = NiPosKey::GetCreateFunction(ePosType);
        NIASSERT(cf);
        NiPosKey* pkPosKeys = (NiPosKey*) cf(stream, uiNumKeys);
        NIASSERT(pkPosKeys);

        NiPosKey::FillDerivedValsFunction derived =
            NiPosKey::GetFillDerivedFunction(ePosType);
        NIASSERT(derived);
        derived(pkPosKeys, uiNumKeys, m_ucKeySize);
        ReplaceAnim(pkPosKeys, uiNumKeys, ePosType);
    }
}
//---------------------------------------------------------------------------
void NiPosData::LinkObject(NiStream& stream)
{
    NiObject::LinkObject(stream);
}
//---------------------------------------------------------------------------
bool NiPosData::RegisterStreamables(NiStream& stream)
{
    return NiObject::RegisterStreamables(stream);
}
//---------------------------------------------------------------------------
void NiPosData::SaveBinary(NiStream& stream)
{
    NiObject::SaveBinary(stream);

    // save position keys
    NiStreamSaveBinary(stream, m_uiNumKeys);
    if ( m_uiNumKeys > 0 )
    {
        // save type of position
        NiStreamSaveEnum(stream, m_eType);

        // save positions
        NiPosKey::SaveFunction sf = NiPosKey::GetSaveFunction(m_eType);
        NIASSERT(sf);
        sf(stream, m_pkKeys, m_uiNumKeys);
    }
}
//---------------------------------------------------------------------------
bool NiPosData::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
    {
        return false;
    }

    NiPosData* pkData = (NiPosData*) pkObject;

    if (m_uiNumKeys != pkData->m_uiNumKeys || m_eType != pkData->m_eType ||
        m_ucKeySize != pkData->m_ucKeySize)
    {
        return false;
    }

    NiPosKey::EqualFunction equal = NiPosKey::GetEqualFunction(m_eType);
    NIASSERT(equal);
    for (unsigned int uiI = 0; uiI < m_uiNumKeys; uiI++)
    {
        if (!equal(*m_pkKeys->GetKeyAt(uiI, m_ucKeySize),
            *pkData->m_pkKeys->GetKeyAt(uiI, m_ucKeySize)))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiPosData::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiObject::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiPosData::ms_RTTI.GetName()));

    pStrings->Add(NiGetViewerString("m_uiNumKeys",m_uiNumKeys));
}
//---------------------------------------------------------------------------
