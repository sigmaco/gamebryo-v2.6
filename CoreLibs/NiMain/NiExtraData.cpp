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

#include "NiExtraData.h"
#include "NiBinaryExtraData.h"

NiImplementRTTI(NiExtraData,NiObject);

//---------------------------------------------------------------------------
NiExtraData::NiExtraData()
{
}
//---------------------------------------------------------------------------
NiExtraData::NiExtraData(const NiFixedString& kName)
{
    SetName(kName);
}
//---------------------------------------------------------------------------
NiExtraData::~NiExtraData ()
{
}
//---------------------------------------------------------------------------
const NiFixedString& NiExtraData::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
void NiExtraData::SetName(const NiFixedString& kName)
{
    m_kName = kName;
}
//---------------------------------------------------------------------------

// By default, extra data is both streamable and cloneable.

bool NiExtraData::IsCloneable() const
{
    return true;
}
//---------------------------------------------------------------------------
bool NiExtraData::IsStreamable() const
{
    return true;
}
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
void NiExtraData::CopyMembers(NiExtraData* pDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pDest, kCloning);

    pDest->SetName(m_kName);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiExtraData);
//---------------------------------------------------------------------------
void NiExtraData::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);
    
    if (kStream.GetFileVersion() < NiStream::GetVersion(20, 1, 0, 1))
    {
        kStream.LoadCStringAsFixedString(m_kName);
    }
    else
    {
        kStream.LoadFixedString(m_kName);
    }

}
//---------------------------------------------------------------------------
void NiExtraData::LinkObject(NiStream&)
{
}
//---------------------------------------------------------------------------
bool NiExtraData::RegisterStreamables(NiStream& kStream)
{
    if ( !NiObject::RegisterStreamables(kStream) )
        return false;

    if (!kStream.RegisterFixedString(m_kName))
        return false;

    return true;
}
//---------------------------------------------------------------------------
void NiExtraData::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);
    
    kStream.SaveFixedString(m_kName);
}
//---------------------------------------------------------------------------
bool NiExtraData::IsEqual(NiObject* pObject)
{
    if (!NiObject::IsEqual(pObject))
    {
        return false;
    }

    // assert:  pVoid is NiExtraData-derived
    NiExtraData* pkExtra = (NiExtraData*) pObject;

    if (m_kName != pkExtra->m_kName)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiExtraData::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    pkStrings->Add(NiGetViewerString(NiExtraData::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_pcName", GetName()));
}
//---------------------------------------------------------------------------
