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

#include "NiTextKeyExtraData.h"

NiImplementRTTI(NiTextKeyExtraData,NiExtraData);

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiTextKeyExtraData);
//---------------------------------------------------------------------------
void NiTextKeyExtraData::CopyMembers(NiTextKeyExtraData* pDest,
    NiCloningProcess& kCloning)
{
    NiExtraData::CopyMembers(pDest, kCloning);

    pDest->m_uiNumKeys = m_uiNumKeys;

    if ( m_uiNumKeys > 0 && NiIsExactKindOf(NiTextKeyExtraData,this) )
    {
        pDest->m_pKeys = NiNew NiTextKey[m_uiNumKeys];
        for (unsigned int uiI = 0; uiI < m_uiNumKeys; uiI++)
        {
            pDest->m_pKeys[uiI].SetTime(m_pKeys[uiI].GetTime());
            pDest->m_pKeys[uiI].SetText(m_pKeys[uiI].GetText());
        }
    }
    else
    {
        pDest->m_pKeys = 0;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiTextKeyExtraData);
//---------------------------------------------------------------------------
void NiTextKeyExtraData::LoadBinary(NiStream& stream)
{
    NiExtraData::LoadBinary(stream);

    unsigned int uiCount;
    NiStreamLoadBinary(stream, uiCount);
    if ( uiCount > 0 )
    {
        NiTextKey* pTextKeys = NiNew NiTextKey[uiCount];
        NIASSERT(pTextKeys);

        for (unsigned int uiI = 0; uiI < uiCount; uiI++)
            pTextKeys[uiI].LoadBinary(stream);

        SetKeys(pTextKeys,uiCount);
    }
}
//---------------------------------------------------------------------------
void NiTextKeyExtraData::LinkObject(NiStream& stream)
{
    NiExtraData::LinkObject(stream);
}
//---------------------------------------------------------------------------
bool NiTextKeyExtraData::RegisterStreamables(NiStream& stream)
{
    if (!NiExtraData::RegisterStreamables(stream))
        return false;

    for (unsigned int uiI = 0; uiI < m_uiNumKeys; uiI++)
        m_pKeys[uiI].RegisterStreamables(stream);

    return true;
}
//---------------------------------------------------------------------------
void NiTextKeyExtraData::SaveBinary(NiStream& stream)
{
    NiExtraData::SaveBinary(stream);

    NiStreamSaveBinary(stream,m_uiNumKeys);
    for (unsigned int uiI = 0; uiI < m_uiNumKeys; uiI++)
        m_pKeys[uiI].SaveBinary(stream);
}
//---------------------------------------------------------------------------
bool NiTextKeyExtraData::IsEqual(NiObject* pObject)
{
    if (!pObject)
        return false;

    NiTextKeyExtraData* pExtra = (NiTextKeyExtraData*) pObject;

    // Don't go through NiExtraData::IsEqual because memcmp assumes non-null
    //   pointers, and this class does not use those pointers
    if (!NiObject::IsEqual(pObject))
        return false;

    if ( m_uiNumKeys != pExtra->m_uiNumKeys )
        return false;

    for (unsigned int uiI = 0; uiI < m_uiNumKeys; uiI++)
    {
        if ( m_pKeys[uiI] != pExtra->m_pKeys[uiI] )
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiTextKeyExtraData::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiExtraData::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiTextKeyExtraData::ms_RTTI.GetName()));

    pStrings->Add(NiGetViewerString("m_uiNumKeys", m_uiNumKeys));

    for(unsigned int uiLoop = 0; uiLoop < m_uiNumKeys; uiLoop++)
    {
        // Create the intermediate strings
        char* pcTime = NiGetViewerString("Time", m_pKeys[uiLoop].GetTime());
        char* pcText = NiGetViewerString("Text", m_pKeys[uiLoop].GetText());

        size_t uiLen = strlen(pcTime) + strlen(pcText) + 4;
        char* pcTextString = NiAlloc(char, uiLen);
        NiSprintf(pcTextString, uiLen, "%s : %s", pcTime, pcText);

        pStrings->Add(NiGetViewerString("TextKey", pcTextString));

        // free the intermediate strings
        NiFree(pcTime);
        NiFree(pcText);
        NiFree(pcTextString);
    }

}
//---------------------------------------------------------------------------
