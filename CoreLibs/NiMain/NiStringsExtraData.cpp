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
#include "NiMainPCH.h"

#include "NiStringsExtraData.h"

NiImplementRTTI(NiStringsExtraData,NiExtraData);

//---------------------------------------------------------------------------
NiStringsExtraData::NiStringsExtraData(const unsigned int uiSize,
                                       const char** ppcValue)
{
    m_ppcValue = NULL;
    m_uiSize = 0;
    SetArray(uiSize, ppcValue);
}
//---------------------------------------------------------------------------
void NiStringsExtraData::SetArray(const unsigned int uiSize,
                                  const char** ppcValue)
{
    for (unsigned int i=0; i < m_uiSize; i++)
    {
        NiFree(m_ppcValue[i]);
    }
    NiFree(m_ppcValue);

    if (ppcValue && (uiSize > 0))
    {
        m_uiSize = uiSize;

        m_ppcValue = NiAlloc(char*,uiSize);

        NIASSERT(m_ppcValue);

        for (unsigned int i=0; i < uiSize; i++)
        {
            if (ppcValue[i])
            {
                size_t stLen = strlen(ppcValue[i]) + 1;
                m_ppcValue[i] = NiAlloc(char,stLen);
                NIASSERT(m_ppcValue[i]);
                NiStrcpy(m_ppcValue[i], stLen, ppcValue[i]);
            }
            else
            {
                m_ppcValue[i] = NULL;
            }
        }
    }
    else
    {
        m_uiSize = 0;
        m_ppcValue = NULL;
    }
}
//---------------------------------------------------------------------------

// Set a specific entry in the array to a new value.  Note that this member
//    function does not grow the array (to avoid memory fragmentation issues).

bool NiStringsExtraData::SetValue(const unsigned int uiIndex,
                                  char* pcValue)
{
    if (uiIndex < m_uiSize)
    {
        if (m_ppcValue[uiIndex])   // First, blow away any existing string.
        {
            NiFree(m_ppcValue[uiIndex]);
            m_ppcValue[uiIndex] = NULL;
        }

        if (pcValue && (strlen(pcValue) > 0)) // Then, set the string, if any.
        {
            size_t stLen = strlen(pcValue) + 1;
            m_ppcValue[uiIndex] = NiAlloc(char,stLen);
            NiStrcpy(m_ppcValue[uiIndex], stLen, pcValue);
        }

        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
char* NiStringsExtraData::GetValue(const unsigned int uiIndex) const
{
    if (uiIndex < m_uiSize)
    {
        return (m_ppcValue[uiIndex]);
    }
    else
    {
        return NULL;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiStringsExtraData);
//---------------------------------------------------------------------------
void NiStringsExtraData::CopyMembers(NiStringsExtraData* pDest,
    NiCloningProcess& kCloning)
{
    NiExtraData::CopyMembers(pDest, kCloning);

    if (m_ppcValue && (m_uiSize > 0))
    {
        pDest->m_uiSize = m_uiSize;

        pDest->m_ppcValue = NiAlloc(char*,m_uiSize);

        NIASSERT(pDest->m_ppcValue);

        for (unsigned int i=0; i < m_uiSize; i++)
        {
            if (m_ppcValue[i])
            {
                size_t stLen = strlen(m_ppcValue[i]) + 1;
                pDest->m_ppcValue[i] = NiAlloc(char,stLen);
                NiStrcpy(pDest->m_ppcValue[i], stLen, m_ppcValue[i]);
            }
            else
            {
                pDest->m_ppcValue[i] = NULL;
            }
        }
    }
    else
    {
        pDest->m_ppcValue = NULL;
        pDest->m_uiSize = 0;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiStringsExtraData);

//---------------------------------------------------------------------------
void NiStringsExtraData::LoadBinary(NiStream& kStream)
{
    NiExtraData::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uiSize);

    if (m_uiSize > 0)
    {
        m_ppcValue = NiAlloc(char*,m_uiSize);
        for (unsigned int i=0; i < m_uiSize; i++)
        {
            m_ppcValue[i] = 0;
            kStream.LoadCString(m_ppcValue[i]);
        }
    }
    else
    {
        m_ppcValue = NULL;
    }
}
//---------------------------------------------------------------------------
void NiStringsExtraData::LinkObject(NiStream& kStream)
{
    NiExtraData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiStringsExtraData::RegisterStreamables(NiStream& kStream)
{
    return NiExtraData::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiStringsExtraData::SaveBinary(NiStream& kStream)
{
    NiExtraData::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uiSize);

    for (unsigned int i=0; i < m_uiSize; i++)
    {
        kStream.SaveCString(m_ppcValue[i]);
    }
}
//---------------------------------------------------------------------------
bool NiStringsExtraData::IsEqual(NiObject* pObject)
{
    if (!pObject)
    {
        return false;
    }

    if (!NiIsExactKindOf(NiStringsExtraData, pObject))
        return false;

    NiStringsExtraData* pExtra = (NiStringsExtraData*) pObject;

    if (m_uiSize != pExtra->m_uiSize)
    {
        return false;
    }

    if ((m_ppcValue && !pExtra->m_ppcValue)
     || (!m_ppcValue && pExtra->m_ppcValue))
    {
        return false;
    }

    if (m_ppcValue)
    {
        for (unsigned int i=0; i < m_uiSize; i++)
        {
            if ((m_ppcValue[i] && !pExtra->m_ppcValue[i])
             || (!m_ppcValue[i] && pExtra->m_ppcValue[i]))
            {
                return false;
            }

            if (m_ppcValue[i])
            {
                if (strcmp(m_ppcValue[i], pExtra->m_ppcValue[i]) != 0)
                {
                    return false;
                }
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiStringsExtraData::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiExtraData::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiStringsExtraData::ms_RTTI.GetName()));

    pStrings->Add(NiGetViewerString("m_uiSize", m_uiSize));

    for (unsigned int i=0; i < m_uiSize; i++)
    {
        pStrings->Add(NiGetViewerString("m_ppcValue[i]", m_ppcValue[i]));
    }
}
//---------------------------------------------------------------------------
