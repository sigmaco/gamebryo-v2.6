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

#include "NiBinaryExtraData.h"

NiImplementRTTI(NiBinaryExtraData,NiExtraData);


//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiBinaryExtraData);
//---------------------------------------------------------------------------
void NiBinaryExtraData::CopyMembers(NiBinaryExtraData* pDest,
    NiCloningProcess& kCloning)
{
    NiExtraData::CopyMembers(pDest, kCloning);

    if (m_uiSize > 0)
    {
        pDest->m_pcBinaryData = NiAlloc(char, m_uiSize);
        unsigned int uiDestSize = m_uiSize * sizeof(char);
        NiMemcpy(pDest->m_pcBinaryData, m_pcBinaryData, uiDestSize);
        pDest->m_uiSize = m_uiSize;
    }
    else
    {
        pDest->m_uiSize = 0;
        pDest->m_pcBinaryData = NULL;
    }
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBinaryExtraData);
//---------------------------------------------------------------------------
void NiBinaryExtraData::LoadBinary(NiStream& kStream)
{
    NiExtraData::LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_uiSize);
    if (m_uiSize > 0)
    {
        m_pcBinaryData = NiAlloc(char, m_uiSize);
        NIASSERT(m_pcBinaryData);
        NiStreamLoadBinary(kStream, m_pcBinaryData, m_uiSize);
    }
}
//---------------------------------------------------------------------------
void NiBinaryExtraData::LinkObject(NiStream& kStream)
{
    NiExtraData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBinaryExtraData::RegisterStreamables(NiStream& kStream)
{
    return NiExtraData::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiBinaryExtraData::SaveBinary(NiStream& kStream)
{
    NiExtraData::SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_uiSize);
    if (m_uiSize > 0)
    {
        NiStreamSaveBinary(kStream, m_pcBinaryData, m_uiSize);
    }
}
//---------------------------------------------------------------------------
bool NiBinaryExtraData::IsEqual(NiObject* pObject)
{
    if ( !pObject )
        return false;

    if (!NiIsExactKindOf(NiBinaryExtraData, pObject))
        return false;

    NiBinaryExtraData* pExtra = (NiBinaryExtraData*) pObject;

    if (m_uiSize != pExtra->m_uiSize)
    {
        return false;
    }

    if (memcmp(m_pcBinaryData, pExtra->m_pcBinaryData, m_uiSize) != 0)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiBinaryExtraData::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiExtraData::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiBinaryExtraData::ms_RTTI.GetName()));

    pStrings->Add(NiGetViewerString("m_uiSize", m_uiSize));
}
//---------------------------------------------------------------------------
