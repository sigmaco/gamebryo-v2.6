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

#include "NiRotData.h"

NiImplementRTTI(NiRotData, NiObject);

//---------------------------------------------------------------------------
NiRotData::NiRotData() : m_uiNumKeys(0), m_pkKeys(NULL),
    m_eType(NiRotKey::NOINTERP), m_ucKeySize(0)
{
}
//---------------------------------------------------------------------------
NiRotData::~NiRotData()
{
    if (m_pkKeys)
    {
        NiRotKey::DeleteFunction pfnDeleteFunc = NiRotKey::GetDeleteFunction(
            m_eType);
        NIASSERT(pfnDeleteFunc);
        pfnDeleteFunc(m_pkKeys);
    }
}
//---------------------------------------------------------------------------
void NiRotData::ReplaceAnim(NiRotKey* pkKeys, unsigned int uiNumKeys,
    NiRotKey::KeyType eType)
{
    // Delete old copies of data
    if (m_pkKeys)
    {
        NiRotKey::DeleteFunction pfnDeleteFunc = NiRotKey::GetDeleteFunction(
            m_eType);
        NIASSERT(pfnDeleteFunc);
        pfnDeleteFunc(m_pkKeys);
    }

    SetAnim(pkKeys, uiNumKeys, eType);
}
//---------------------------------------------------------------------------
void NiRotData::SetAnim(NiRotKey* pkKeys, unsigned int uiNumKeys,
    NiRotKey::KeyType eType)
{
    if (!pkKeys || uiNumKeys == 0)
    {
        m_uiNumKeys = 0;
        m_pkKeys = NULL;
        m_eType = NiRotKey::NOINTERP;
        m_ucKeySize = 0;
        return;
    }

    m_ucKeySize = NiRotKey::GetKeySize(eType);

    m_uiNumKeys = uiNumKeys;
    m_pkKeys = pkKeys;
    m_eType = eType;
}
//---------------------------------------------------------------------------
void NiRotData::GuaranteeKeysAtStartAndEnd(float fStartTime, float fEndTime)
{
    // This should not exist without any keys
    NIASSERT(m_uiNumKeys != 0);

    NiAnimationKey* pkKeys = (NiAnimationKey*) m_pkKeys;
    NiAnimationKey::GuaranteeKeyAtStartAndEnd(NiAnimationKey::POSKEY,
        m_eType, pkKeys, m_uiNumKeys, fStartTime, fEndTime);
    m_pkKeys = (NiRotKey*) pkKeys;
}
//---------------------------------------------------------------------------
NiRotDataPtr NiRotData::GetSequenceData(float fStartTime, float fEndTime)
{
    NiRotDataPtr spNewData = NiSmartPointerCast(NiRotData, CreateDeepCopy());

    if (m_uiNumKeys > 0)
    {
        NiAnimationKey* pkNewKeys = NULL;
        unsigned int uiNewNumKeys = 0;
        NiAnimationKey::CopySequence(NiAnimationKey::ROTKEY, m_eType,
            m_pkKeys, m_uiNumKeys, fStartTime, fEndTime, pkNewKeys,
            uiNewNumKeys);
        spNewData->ReplaceAnim((NiRotKey*) pkNewKeys, uiNewNumKeys,
            m_eType);
    }

    return spNewData;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiRotData);
//---------------------------------------------------------------------------
void NiRotData::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    unsigned int uiNumKeys;
    NiStreamLoadBinary(kStream, uiNumKeys);
    if (uiNumKeys > 0)
    {
        NiRotKey::KeyType eRotType;
        NiStreamLoadEnum(kStream, eRotType);
        m_ucKeySize = NiRotKey::GetKeySize(eRotType);
        
        NiRotKey::CreateFunction pfnCreateFunc = NiRotKey::GetCreateFunction(
            eRotType);
        NIASSERT(pfnCreateFunc);
        NiRotKey* pkRotKeys = (NiRotKey*) pfnCreateFunc(kStream, uiNumKeys);
        NIASSERT(pkRotKeys);

        NiRotKey::FillDerivedValsFunction pfnFillDerivedFunc =
            NiRotKey::GetFillDerivedFunction(eRotType);
        NIASSERT(pfnFillDerivedFunc);
        pfnFillDerivedFunc(pkRotKeys, uiNumKeys,m_ucKeySize);
        ReplaceAnim(pkRotKeys, uiNumKeys, eRotType);
    }
}
//---------------------------------------------------------------------------
void NiRotData::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiRotData::RegisterStreamables(NiStream& kStream)
{
    return NiObject::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiRotData::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uiNumKeys);
    if (m_uiNumKeys > 0)
    {
        NiStreamSaveEnum(kStream, m_eType);

        NiRotKey::SaveFunction pfnSaveFunc = NiRotKey::GetSaveFunction(
            m_eType);
        NIASSERT(pfnSaveFunc);
        pfnSaveFunc(kStream, m_pkKeys, m_uiNumKeys);
    }
}
//---------------------------------------------------------------------------
bool NiRotData::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
    {
        return false;
    }

    NiRotData* pkData = (NiRotData*) pkObject;

    if (m_uiNumKeys != pkData->m_uiNumKeys || m_eType != pkData->m_eType ||
        m_ucKeySize != pkData->m_ucKeySize)
    {
        return false;
    }

    NiRotKey::EqualFunction pfnEqualFunc = NiRotKey::GetEqualFunction(
        m_eType);
    NIASSERT(pfnEqualFunc);
    for (unsigned int ui = 0; ui < m_uiNumKeys; ui++)
    {
        if (!pfnEqualFunc(*m_pkKeys->GetKeyAt(ui, m_ucKeySize),
            *pkData->m_pkKeys->GetKeyAt(ui, m_ucKeySize)))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiRotData::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiRotData::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_uiNumKeys", m_uiNumKeys));
}
//---------------------------------------------------------------------------
