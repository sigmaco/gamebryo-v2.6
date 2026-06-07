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

#include "NiFloatData.h"

NiImplementRTTI(NiFloatData,NiObject);

//---------------------------------------------------------------------------
NiFloatData::~NiFloatData ()
{
    if (m_pkKeys)
    {
        NiFloatKey::DeleteFunction pfnDeleteFunc =
            NiFloatKey::GetDeleteFunction(m_eType);
        NIASSERT(pfnDeleteFunc);
        pfnDeleteFunc(m_pkKeys);
    }
}
//---------------------------------------------------------------------------
void NiFloatData::ReplaceAnim(NiFloatKey* pkKeys, unsigned int uiNumKeys,
    NiFloatKey::KeyType eType)
{
    // Delete old copies of data
    if (m_pkKeys)
    {
        NiFloatKey::DeleteFunction pfnDeleteFunc =
            NiFloatKey::GetDeleteFunction(m_eType);
        NIASSERT(pfnDeleteFunc);
        pfnDeleteFunc(m_pkKeys);
    }

    SetAnim(pkKeys, uiNumKeys, eType);
}
//---------------------------------------------------------------------------
void NiFloatData::SetAnim(NiFloatKey* pkKeys, unsigned int uiNumKeys,
    NiFloatKey::KeyType eType)
{
    if ( !pkKeys || uiNumKeys == 0 || eType == NiAnimationKey::NOINTERP)
    {
        m_uiNumKeys = 0;
        m_pkKeys = 0;
        m_ucKeySize = 0;
        m_eType = NiAnimationKey::NOINTERP;
        return;
    }

    m_ucKeySize = NiFloatKey::GetKeySize(eType);
    
    m_uiNumKeys = uiNumKeys;
    m_pkKeys = pkKeys;
    m_eType = eType;
}
//---------------------------------------------------------------------------
void NiFloatData::GuaranteeKeysAtStartAndEnd(float fStartTime, float fEndTime)
{
    // This should not exist without any keys
    NIASSERT(m_uiNumKeys != 0);

    NiAnimationKey* pkKeys = (NiAnimationKey*)m_pkKeys;
    NiAnimationKey::GuaranteeKeyAtStartAndEnd(NiAnimationKey::FLOATKEY,
        m_eType, pkKeys, m_uiNumKeys, fStartTime, fEndTime);
    m_pkKeys = (NiFloatKey*)pkKeys;
}
//---------------------------------------------------------------------------
NiFloatDataPtr NiFloatData::GetSequenceData(float fStartTime, float fEndTime)
{
    NiFloatDataPtr spNewData = NiSmartPointerCast(NiFloatData,
        CreateDeepCopy());

    if (m_uiNumKeys > 0)
    {
        NiAnimationKey* pkNewKeys = NULL;
        unsigned int uiNewNumKeys = 0;
        NiAnimationKey::CopySequence(NiAnimationKey::FLOATKEY, m_eType,
            m_pkKeys, m_uiNumKeys, fStartTime, fEndTime, pkNewKeys,
            uiNewNumKeys);
        spNewData->ReplaceAnim((NiFloatKey*) pkNewKeys, uiNewNumKeys,
            m_eType);
    }

    return spNewData;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiFloatData);
//---------------------------------------------------------------------------
void NiFloatData::LoadBinary(NiStream& stream)
{
    NiObject::LoadBinary(stream);

    unsigned int uiNumKeys;
    NiStreamLoadBinary(stream,uiNumKeys);

    if ( uiNumKeys > 0 )
    {
        // load type of float keys
        NiFloatKey::KeyType eType;
        NiStreamLoadEnum(stream,eType);
        m_ucKeySize = NiFloatKey::GetKeySize(eType);
                
        // load keys
        NiFloatKey::CreateFunction cf = NiFloatKey::GetCreateFunction(eType);
        NiFloatKey* pkKeys = (NiFloatKey*) cf(stream, uiNumKeys);
        NIASSERT(pkKeys);

        NiFloatKey::FillDerivedValsFunction derived =
            NiFloatKey::GetFillDerivedFunction(eType);
        NIASSERT(derived);
        derived(pkKeys, uiNumKeys, m_ucKeySize);

        ReplaceAnim(pkKeys, uiNumKeys, eType);
    }
}
//---------------------------------------------------------------------------
void NiFloatData::LinkObject(NiStream& stream)
{
    NiObject::LinkObject(stream);
}
//---------------------------------------------------------------------------
bool NiFloatData::RegisterStreamables(NiStream& stream)
{
    return NiObject::RegisterStreamables(stream);
}
//---------------------------------------------------------------------------
void NiFloatData::SaveBinary(NiStream& stream)
{
    NiObject::SaveBinary(stream);

    NiStreamSaveBinary(stream, m_uiNumKeys);

    if (m_uiNumKeys > 0)
    {
        // save type of interpolation keys
        NiStreamSaveEnum(stream, m_eType);

        // save keys
        NiFloatKey::SaveFunction sf = NiFloatKey::GetSaveFunction(m_eType);
        NIASSERT(sf);
        sf(stream, m_pkKeys, m_uiNumKeys);
    }
}
//---------------------------------------------------------------------------
bool NiFloatData::IsEqual(NiObject* pkObject)
{
    unsigned int i;
    
    if (!NiObject::IsEqual(pkObject))
    {
        return false;
    }

    NiFloatData* pkData = (NiFloatData*) pkObject;

    if (m_uiNumKeys != pkData->m_uiNumKeys ||
        m_eType != pkData->m_eType ||
        m_ucKeySize != pkData->m_ucKeySize)
    {
        return false;
    }

    NiFloatKey::EqualFunction equal = NiFloatKey::GetEqualFunction(m_eType);
    NIASSERT(equal);

    for (i = 0; i < m_uiNumKeys; i++)
    {
        if (!equal(*m_pkKeys->GetKeyAt(i, m_ucKeySize), 
            *pkData->m_pkKeys->GetKeyAt(i, m_ucKeySize)))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiFloatData::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiObject::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiFloatData::ms_RTTI.GetName()));

    pStrings->Add(NiGetViewerString("m_uiNumKeys",m_uiNumKeys));
}
//---------------------------------------------------------------------------
