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

#include "NiColorData.h"

NiImplementRTTI(NiColorData,NiObject);

//---------------------------------------------------------------------------
NiColorData::~NiColorData ()
{
    if (m_pkKeys)
    {
        NiColorKey::DeleteFunction pfnDeleteFunc =
            NiColorKey::GetDeleteFunction(m_eType);
        NIASSERT(pfnDeleteFunc);
        pfnDeleteFunc(m_pkKeys);
    }
}
//---------------------------------------------------------------------------
void NiColorData::ReplaceAnim(NiColorKey* pkKeys,
    unsigned int uiNumKeys, NiColorKey::KeyType eType)
{
    // Delete old copies of data
    if (m_pkKeys)
    {
        NiColorKey::DeleteFunction pfnDeleteFunc =
            NiColorKey::GetDeleteFunction(m_eType);
        NIASSERT(pfnDeleteFunc);
        pfnDeleteFunc(m_pkKeys);
    }

    SetAnim(pkKeys, uiNumKeys, eType);
}
//---------------------------------------------------------------------------
void NiColorData::SetAnim(NiColorKey* pkKeys,
    unsigned int uiNumKeys, NiColorKey::KeyType eType)
{
    if (!pkKeys || uiNumKeys == 0)
    {
        m_uiNumKeys = 0;
        m_pkKeys = 0;
        m_eType = NiAnimationKey::NOINTERP;
        m_ucKeySize = 0;
        return;
    }

    m_ucKeySize = NiColorKey::GetKeySize(eType);

    m_uiNumKeys = uiNumKeys;
    m_pkKeys = pkKeys;
    m_eType = eType;
}
//---------------------------------------------------------------------------
void NiColorData::GuaranteeKeysAtStartAndEnd(float fStartTime, float fEndTime)
{
    // This should not exist without any keys
    NIASSERT(m_uiNumKeys != 0);

    NiAnimationKey* pkKeys = (NiAnimationKey*)m_pkKeys;
    NiAnimationKey::GuaranteeKeyAtStartAndEnd(NiAnimationKey::COLORKEY,
        m_eType, pkKeys, m_uiNumKeys, fStartTime, fEndTime);
    m_pkKeys = (NiColorKey*)pkKeys;
}
//---------------------------------------------------------------------------
NiColorDataPtr NiColorData::GetSequenceData(float fStartTime, float fEndTime)
{
    NiColorDataPtr spNewData = NiSmartPointerCast(NiColorData,
        CreateDeepCopy());

    if (m_uiNumKeys > 0)
    {
        NiAnimationKey* pkNewKeys = NULL;
        unsigned int uiNewNumKeys = 0;
        NiAnimationKey::CopySequence(NiAnimationKey::COLORKEY, m_eType,
            m_pkKeys, m_uiNumKeys, fStartTime, fEndTime, pkNewKeys,
            uiNewNumKeys);
        spNewData->ReplaceAnim((NiColorKey*) pkNewKeys, uiNewNumKeys,
            m_eType);
    }

    return spNewData;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiColorData);

//---------------------------------------------------------------------------
void NiColorData::LoadBinary(NiStream& stream)
{
    NiObject::LoadBinary(stream);

    // load position keys
    unsigned int uiNumKeys;
    NiStreamLoadBinary(stream,uiNumKeys);
    if ( uiNumKeys > 0 )
    {
        // load type of position
        NiColorKey::KeyType eColorType;
        NiStreamLoadEnum(stream,eColorType);
        
        // load positions
        NiColorKey::CreateFunction cf =
            NiColorKey::GetCreateFunction(eColorType);
        NIASSERT(cf);
        NiColorKey* pkColorKeys = (NiColorKey*) cf(stream, uiNumKeys);
        NIASSERT(pkColorKeys);

        ReplaceAnim(pkColorKeys, uiNumKeys, eColorType);
    }
}
//---------------------------------------------------------------------------
void NiColorData::LinkObject(NiStream& stream)
{
    NiObject::LinkObject(stream);
}
//---------------------------------------------------------------------------
bool NiColorData::RegisterStreamables(NiStream& stream)
{
    return NiObject::RegisterStreamables(stream);
}
//---------------------------------------------------------------------------
void NiColorData::SaveBinary(NiStream& stream)
{
    NiObject::SaveBinary(stream);

    // save position keys
    NiStreamSaveBinary(stream, m_uiNumKeys);
    if (m_uiNumKeys > 0)
    {
        // save type of position
        NiStreamSaveEnum(stream, m_eType);

        // save positions
        NiColorKey::SaveFunction sf = NiColorKey::GetSaveFunction(m_eType);
        NIASSERT(sf);
        sf(stream, m_pkKeys, m_uiNumKeys);
    }
}
//---------------------------------------------------------------------------
bool NiColorData::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
    {
        return false;
    }

    NiColorData* pkData = (NiColorData*) pkObject;

    if (m_uiNumKeys != pkData->m_uiNumKeys ||
        m_eType != pkData->m_eType ||
        m_ucKeySize != pkData->m_ucKeySize)
    {
        return false;
    }

    NiColorKey::EqualFunction equal = NiColorKey::GetEqualFunction(m_eType);
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
void NiColorData::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiObject::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiColorData::ms_RTTI.GetName()));

    pStrings->Add(NiGetViewerString("m_uiNumKeys",m_uiNumKeys));
}
//---------------------------------------------------------------------------
