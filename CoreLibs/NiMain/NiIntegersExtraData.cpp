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

#include "NiIntegersExtraData.h"

NiImplementRTTI(NiIntegersExtraData, NiExtraData);

//---------------------------------------------------------------------------
NiIntegersExtraData::NiIntegersExtraData(const unsigned int uiSize,
                                         const int* piValue)
{
    m_piValue = NULL;
    SetArray(uiSize, piValue);
}
//---------------------------------------------------------------------------
void NiIntegersExtraData::SetArray(const unsigned int uiSize,
                                   const int* piValue)
{
    NiFree(m_piValue);

    if (piValue && (uiSize > 0))
    {
        m_uiSize = uiSize;

        m_piValue = NiAlloc(int, uiSize);

        NIASSERT(m_piValue);

        for (unsigned int i=0; i < uiSize; i++)
        {
            m_piValue[i] = piValue[i];
        }
    }
    else
    {
        m_uiSize = 0;
        m_piValue = NULL;
    }
}
//---------------------------------------------------------------------------

// Set a specific entry in the array to a new value.  Note that this member
//    function does not grow the array (to avoid memory fragmentation issues).

bool NiIntegersExtraData::SetValue(const unsigned int uiIndex,
                                   int iValue)
{
    if (uiIndex < m_uiSize)
    {
        m_piValue[uiIndex] = iValue;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
int NiIntegersExtraData::GetValue(const unsigned int uiIndex) const
{
    if (uiIndex < m_uiSize)
    {
        return (m_piValue[uiIndex]);
    }
    else
    {
        return (0);
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiIntegersExtraData);
//---------------------------------------------------------------------------
void NiIntegersExtraData::CopyMembers(NiIntegersExtraData* pDest,
    NiCloningProcess& kCloning)
{
    NiExtraData::CopyMembers(pDest, kCloning);

    if (m_piValue && (m_uiSize > 0))
    {
        pDest->m_uiSize = m_uiSize;

        pDest->m_piValue = NiAlloc(int, m_uiSize);

        NIASSERT(pDest->m_piValue);

        for (unsigned int i=0; i < m_uiSize; i++)
        {
            pDest->m_piValue[i] = m_piValue[i];
        }
    }
    else
    {
        pDest->m_piValue = NULL;
        pDest->m_uiSize = 0;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiIntegersExtraData);
//---------------------------------------------------------------------------
void NiIntegersExtraData::LoadBinary(NiStream& kStream)
{
    NiExtraData::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uiSize);
    if (m_uiSize > 0)
    {
        m_piValue = NiAlloc(int, m_uiSize);
        NiStreamLoadBinary(kStream, m_piValue, m_uiSize);
    }
    else
    {
        m_piValue = NULL;
    }
}
//---------------------------------------------------------------------------
void NiIntegersExtraData::LinkObject(NiStream& kStream)
{
    NiExtraData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiIntegersExtraData::RegisterStreamables(NiStream& kStream)
{
    return NiExtraData::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiIntegersExtraData::SaveBinary(NiStream& kStream)
{
    NiExtraData::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uiSize);
    NiStreamSaveBinary(kStream, m_piValue, m_uiSize);
}
//---------------------------------------------------------------------------
bool NiIntegersExtraData::IsEqual(NiObject* pObject)
{
    if (!pObject)
    {
        return false;
    }

    if (!NiIsExactKindOf(NiIntegersExtraData, pObject))
        return false;

    NiIntegersExtraData* pExtra = (NiIntegersExtraData*)(pObject);

    if (m_uiSize != pExtra->m_uiSize)
    {
        return false;
    }

    if ((m_piValue && !pExtra->m_piValue)
     || (!m_piValue && pExtra->m_piValue))
    {
        return false;
    }

    if (m_piValue)
    {
        for (unsigned int i=0; i < m_uiSize; i++)
        {
            if (m_piValue[i] != pExtra->m_piValue[i])
            {
                return false;
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiIntegersExtraData::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiExtraData::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiIntegersExtraData::ms_RTTI.GetName()));

    pStrings->Add(NiGetViewerString("m_uiSize", m_uiSize));

    for (unsigned int i=0; i < m_uiSize; i++)
    {
        pStrings->Add(NiGetViewerString("m_piValue[i]", m_piValue[i]));
    }
}
//---------------------------------------------------------------------------
