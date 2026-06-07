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

#include "NiStringExtraData.h"

NiImplementRTTI(NiStringExtraData,NiExtraData);

//---------------------------------------------------------------------------
NiStringExtraData::NiStringExtraData(const NiFixedString& kString)
{
    SetValue(kString);
}
//---------------------------------------------------------------------------
void NiStringExtraData::SetValue(const NiFixedString& kString)
{
    m_kString = kString;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiStringExtraData);
//---------------------------------------------------------------------------
void NiStringExtraData::CopyMembers(NiStringExtraData* pDest,
    NiCloningProcess& kCloning)
{
    NiExtraData::CopyMembers(pDest, kCloning);
    pDest->m_kString = m_kString;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiStringExtraData);
//---------------------------------------------------------------------------
void NiStringExtraData::LoadBinary(NiStream& stream)
{
    NiExtraData::LoadBinary(stream);
    
    if (stream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 1))
    {
        stream.LoadCStringAsFixedString(m_kString);
    }
    else
    {
        stream.LoadFixedString(m_kString);
    }
}
//---------------------------------------------------------------------------
void NiStringExtraData::LinkObject(NiStream& stream)
{
    NiExtraData::LinkObject(stream);
}
//---------------------------------------------------------------------------
bool NiStringExtraData::RegisterStreamables(NiStream& stream)
{
    if (!NiExtraData::RegisterStreamables(stream))
        return false;

    stream.RegisterFixedString(m_kString);

    return true;
}
//---------------------------------------------------------------------------
void NiStringExtraData::SaveBinary(NiStream& stream)
{
    NiExtraData::SaveBinary(stream);
    stream.SaveFixedString(m_kString);
}
//---------------------------------------------------------------------------
bool NiStringExtraData::IsEqual(NiObject* pObject)
{
    if ( !pObject )
        return false;

    if (!NiIsExactKindOf(NiStringExtraData, pObject))
        return false;

    NiStringExtraData* pExtra = (NiStringExtraData*) pObject;

    if ( m_kString != pExtra->m_kString )
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiStringExtraData::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiExtraData::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiStringExtraData::ms_RTTI.GetName()));

    pStrings->Add(NiGetViewerString("m_kString", (const char*) m_kString));
}
//---------------------------------------------------------------------------
