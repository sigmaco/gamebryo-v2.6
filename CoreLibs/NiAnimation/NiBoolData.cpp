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

#include "NiBoolData.h"

NiImplementRTTI(NiBoolData,NiObject);

//---------------------------------------------------------------------------
NiBoolData::~NiBoolData ()
{
    NiDelete[] m_pkKeys;
}
//---------------------------------------------------------------------------
void NiBoolData::ReplaceAnim(NiBoolKey* pkKeys, unsigned int uiNumKeys,
        NiBoolKey::KeyType eType)
{
    // Delete old copies of data
    if (m_pkKeys)
    {
        NiBoolKey::DeleteFunction pfnDeleteFunc =
            NiBoolKey::GetDeleteFunction(m_eType);
        NIASSERT(pfnDeleteFunc);
        pfnDeleteFunc(m_pkKeys);
    }

    SetAnim(pkKeys, uiNumKeys, eType);
}
//---------------------------------------------------------------------------
void NiBoolData::SetAnim(NiBoolKey* pkKeys, unsigned int uiNumKeys,
        NiBoolKey::KeyType eType)
{
    if (!pkKeys || uiNumKeys == 0)
    {
        m_uiNumKeys = 0;
        m_pkKeys = 0;
        m_eType = NiBoolKey::NOINTERP;
        m_ucKeySize = 0;
        return;
    }

    m_ucKeySize = NiBoolKey::GetKeySize(eType);

    m_uiNumKeys = uiNumKeys;
    m_pkKeys = pkKeys;
    m_eType = eType;
}
//---------------------------------------------------------------------------
void NiBoolData::GuaranteeKeysAtStartAndEnd(float fStartTime, float fEndTime)
{
    // This should not exist without any keys
    NIASSERT(m_uiNumKeys != 0);

    NiAnimationKey* pkKeys = (NiAnimationKey*) m_pkKeys;
    NiAnimationKey::GuaranteeKeyAtStartAndEnd(NiAnimationKey::BOOLKEY,
        m_eType, pkKeys, m_uiNumKeys, fStartTime, fEndTime);
    m_pkKeys = (NiBoolKey*) pkKeys;
}
//---------------------------------------------------------------------------
NiBoolDataPtr NiBoolData::GetSequenceData(float fStartTime, float fEndTime)
{
    NiBoolDataPtr spNewData = NiSmartPointerCast(NiBoolData, CreateDeepCopy());

    if (m_uiNumKeys > 0)
    {
        NiAnimationKey* pkNewKeys = NULL;
        unsigned int uiNewNumKeys = 0;
        NiAnimationKey::CopySequence(NiAnimationKey::BOOLKEY, m_eType,
            m_pkKeys, m_uiNumKeys, fStartTime, fEndTime, pkNewKeys,
            uiNewNumKeys);
        spNewData->ReplaceAnim((NiBoolKey*) pkNewKeys, uiNewNumKeys,
            m_eType);
    }

    return spNewData;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBoolData);

//---------------------------------------------------------------------------
void NiBoolData::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);

    // load visible keys
    unsigned int uiNumKeys;
    NiBoolKey::KeyType eBoolType = NiBoolKey::STEPKEY;
    NiStreamLoadBinary(kStream,uiNumKeys);

    if (uiNumKeys > 0)
    {
        if (kStream.GetFileVersion() >= NiStream::GetVersion(10, 1, 0, 104))
        {
            NiStreamLoadEnum(kStream, eBoolType);
        }

        m_ucKeySize = NiBoolKey::GetKeySize(eBoolType);

        NiBoolKey::CreateFunction pfnCreateFunc = NiBoolKey::GetCreateFunction(
            eBoolType);
        NIASSERT(pfnCreateFunc);
        NiBoolKey* pkBoolKeys = (NiBoolKey*) pfnCreateFunc(kStream, uiNumKeys);
        NIASSERT(pkBoolKeys);

        NiBoolKey::FillDerivedValsFunction pfnFillDerivedFunc =
            NiBoolKey::GetFillDerivedFunction(eBoolType);
        NIASSERT(pfnFillDerivedFunc);
        pfnFillDerivedFunc(pkBoolKeys, uiNumKeys,m_ucKeySize);
        ReplaceAnim(pkBoolKeys, uiNumKeys, eBoolType);
    }
}
//---------------------------------------------------------------------------
void NiBoolData::LinkObject(NiStream& stream)
{
    NiObject::LinkObject(stream);
}
//---------------------------------------------------------------------------
bool NiBoolData::RegisterStreamables(NiStream& stream)
{
    return NiObject::RegisterStreamables(stream);
}
//---------------------------------------------------------------------------
void NiBoolData::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uiNumKeys);
    if (m_uiNumKeys > 0)
    {
        NiStreamSaveEnum(kStream, m_eType);

        NiBoolKey::SaveFunction pfnSaveFunc = NiBoolKey::GetSaveFunction(
            m_eType);
        NIASSERT(pfnSaveFunc);
        pfnSaveFunc(kStream, m_pkKeys, m_uiNumKeys);
    }
}
//---------------------------------------------------------------------------
bool NiBoolData::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
    {
        return false;
    }

    NiBoolData* pkData = (NiBoolData*) pkObject;

    if (m_uiNumKeys != pkData->m_uiNumKeys || m_eType != pkData->m_eType ||
        m_ucKeySize != pkData->m_ucKeySize)
    {
        return false;
    }

    NiBoolKey::EqualFunction pfnEqualFunc = NiBoolKey::GetEqualFunction(
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
void NiBoolData::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiObject::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiBoolData::ms_RTTI.GetName()));

    pStrings->Add(NiGetViewerString("m_uiNumKeys",m_uiNumKeys));
}
//---------------------------------------------------------------------------
