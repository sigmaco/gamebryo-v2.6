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

#include "NiSwitchStringExtraData.h"

NiImplementRTTI(NiSwitchStringExtraData,NiExtraData);

//---------------------------------------------------------------------------
NiSwitchStringExtraData::NiSwitchStringExtraData(const unsigned int uiSize,
                                                 const char** ppcValue,
                                                 const int iIndex)
{
    m_uiSize = 0;
    m_ppcValue = NULL;
    m_iIndex = -1;
    SetValue(uiSize, ppcValue, iIndex);
}
//---------------------------------------------------------------------------
void NiSwitchStringExtraData::SetValue(const unsigned int uiSize,
                                       const char** ppcValue,
                                       const int iIndex)
{
    for (unsigned int i=0; i < m_uiSize; i++)
    {
        NiFree(m_ppcValue[i]);
    }
    NiFree(m_ppcValue);

    if (ppcValue && (uiSize > 0))
    {
        m_uiSize = uiSize;

        if ((iIndex > -1) && (iIndex < (int)(uiSize)))
        {
            m_iIndex = iIndex;
        }
        else
        {
            m_iIndex = -1;
        }

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
        m_iIndex = -1;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiSwitchStringExtraData);
//---------------------------------------------------------------------------
void NiSwitchStringExtraData::CopyMembers(NiSwitchStringExtraData* pDest,
    NiCloningProcess& kCloning)
{
    NiExtraData::CopyMembers(pDest, kCloning);

    if (m_ppcValue && (m_uiSize > 0))
    {
        pDest->m_uiSize = m_uiSize;

        if ((m_iIndex > -1) && (m_iIndex < (int)(m_uiSize)))
        {
            pDest->m_iIndex = m_iIndex;
        }
        else
        {
            pDest->m_iIndex = -1;
        }

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
NiImplementCreateObject(NiSwitchStringExtraData);
//---------------------------------------------------------------------------
void NiSwitchStringExtraData::LoadBinary(NiStream& kStream)
{
    NiExtraData::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uiSize);

    if (m_uiSize > 0)
    {
        m_ppcValue = NiAlloc(char*,m_uiSize);
        for (unsigned int i=0; i < m_uiSize; i++)
        {
            kStream.LoadCString(m_ppcValue[i]);
        }
    }
    else
    {
        m_ppcValue = NULL;
    }

    NiStreamLoadBinary(kStream, m_iIndex);
}
//---------------------------------------------------------------------------
void NiSwitchStringExtraData::LinkObject(NiStream& kStream)
{
    NiExtraData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiSwitchStringExtraData::RegisterStreamables(NiStream& kStream)
{
    return NiExtraData::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiSwitchStringExtraData::SaveBinary(NiStream& kStream)
{
    NiExtraData::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uiSize);

    for (unsigned int i=0; i < m_uiSize; i++)
    {
        kStream.SaveCString(m_ppcValue[i]);
    }

    NiStreamSaveBinary(kStream, m_iIndex);
}
//---------------------------------------------------------------------------
bool NiSwitchStringExtraData::IsEqual(NiObject* pObject)
{
    if (!pObject)
    {
        return false;
    }

    if (!NiIsExactKindOf(NiSwitchStringExtraData, pObject))
        return false;

    NiSwitchStringExtraData* pExtra = (NiSwitchStringExtraData*) pObject;

    if (m_uiSize != pExtra->m_uiSize)
    {
        return false;
    }

    if (m_iIndex != pExtra->m_iIndex)
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
void NiSwitchStringExtraData::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiExtraData::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(
        NiSwitchStringExtraData::ms_RTTI.GetName()));

    pStrings->Add(NiGetViewerString("m_uiSize", m_uiSize));

    for (unsigned int i=0; i < m_uiSize; i++)
    {
        pStrings->Add(NiGetViewerString("m_ppcValue[i]", m_ppcValue[i]));
    }

    pStrings->Add(NiGetViewerString("m_iIndex", m_iIndex));
}
//---------------------------------------------------------------------------
