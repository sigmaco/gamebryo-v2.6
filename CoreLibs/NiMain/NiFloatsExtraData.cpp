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

#include "NiFloatsExtraData.h"

NiImplementRTTI(NiFloatsExtraData, NiExtraData);


//---------------------------------------------------------------------------
NiFloatsExtraData::NiFloatsExtraData(const unsigned int uiSize,
                                     const float* pfValue)
{
    m_pfValue = NULL;
    SetArray(uiSize, pfValue);
}
//---------------------------------------------------------------------------
void NiFloatsExtraData::SetArray(const unsigned int uiSize,
                                 const float* pfValue)
{
    NiFree(m_pfValue);

    if (pfValue && (uiSize > 0))
    {
        m_uiSize = uiSize;

        m_pfValue = NiAlloc(float, uiSize);

        NIASSERT(m_pfValue);

        for (unsigned int i=0; i < uiSize; i++)
        {
            m_pfValue[i] = pfValue[i];
        }
    }
    else
    {
        m_uiSize = 0;
        m_pfValue = NULL;
    }
}
//---------------------------------------------------------------------------

// Set a specific entry in the array to a new value.  Note that this member
//    function does not grow the array (to avoid memory fragmentation issues).

bool NiFloatsExtraData::SetValue(const unsigned int uiIndex,
                                        float fValue)
{
    if (uiIndex < m_uiSize)
    {
        m_pfValue[uiIndex] = fValue;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
float NiFloatsExtraData::GetValue(const unsigned int uiIndex) const
{
    if (uiIndex < m_uiSize)
    {
        return (m_pfValue[uiIndex]);
    }
    else
    {
        return (0.0f);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiFloatsExtraData);
//---------------------------------------------------------------------------
void NiFloatsExtraData::CopyMembers(NiFloatsExtraData* pDest,
    NiCloningProcess& kCloning)
{
    NiExtraData::CopyMembers(pDest, kCloning);

    if (m_pfValue && (m_uiSize > 0))
    {
        pDest->m_uiSize = m_uiSize;

        pDest->m_pfValue = NiAlloc(float, m_uiSize);

        NIASSERT(pDest->m_pfValue);

        for (unsigned int i=0; i < m_uiSize; i++)
        {
            pDest->m_pfValue[i] = m_pfValue[i];
        }
    }
    else
    {
        pDest->m_pfValue = NULL;
        pDest->m_uiSize = 0;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiFloatsExtraData);
//---------------------------------------------------------------------------
void NiFloatsExtraData::LoadBinary(NiStream& kStream)
{
    NiExtraData::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uiSize);
    if (m_uiSize > 0)
    {
        m_pfValue = NiAlloc(float, m_uiSize);
        NiStreamLoadBinary(kStream, m_pfValue, m_uiSize);
    }
    else
    {
        m_pfValue = NULL;
    }
}
//---------------------------------------------------------------------------
void NiFloatsExtraData::LinkObject(NiStream& kStream)
{
    NiExtraData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiFloatsExtraData::RegisterStreamables(NiStream& kStream)
{
    return NiExtraData::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiFloatsExtraData::SaveBinary(NiStream& kStream)
{
    NiExtraData::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uiSize);
    NiStreamSaveBinary(kStream, m_pfValue, m_uiSize);
}
//---------------------------------------------------------------------------
bool NiFloatsExtraData::IsEqual(NiObject* pObject)
{
    if (!pObject)
    {
        return false;
    }

    if (!NiIsExactKindOf(NiFloatsExtraData, pObject))
        return false;

    NiFloatsExtraData* pExtra = (NiFloatsExtraData*)(pObject);

    if (m_uiSize != pExtra->m_uiSize)
    {
        return false;
    }

    if ((m_pfValue && !pExtra->m_pfValue)
     || (!m_pfValue && pExtra->m_pfValue))
    {
        return false;
    }

    if (m_pfValue)
    {
        for (unsigned int i=0; i < m_uiSize; i++)
        {
            if (m_pfValue[i] != pExtra->m_pfValue[i])
            {
                return false;
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiFloatsExtraData::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiExtraData::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiFloatsExtraData::ms_RTTI.GetName()));

    pStrings->Add(NiGetViewerString("m_uiSize", m_uiSize));

    for (unsigned int i=0; i < m_uiSize; i++)
    {
        pStrings->Add(NiGetViewerString("m_pfValue[i]", m_pfValue[i]));
    }
}
//---------------------------------------------------------------------------
