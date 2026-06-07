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

#include "NiFloatExtraData.h"

NiImplementRTTI(NiFloatExtraData,NiExtraData);


//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiFloatExtraData);
//---------------------------------------------------------------------------
void NiFloatExtraData::CopyMembers(NiFloatExtraData* pDest,
    NiCloningProcess& kCloning)
{
    NiExtraData::CopyMembers(pDest, kCloning);

    pDest->m_fValue = m_fValue;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiFloatExtraData);
//---------------------------------------------------------------------------
void NiFloatExtraData::LoadBinary(NiStream& kStream)
{
    NiExtraData::LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_fValue);
}
//---------------------------------------------------------------------------
void NiFloatExtraData::LinkObject(NiStream& kStream)
{
    NiExtraData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiFloatExtraData::RegisterStreamables(NiStream& kStream)
{
    return NiExtraData::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiFloatExtraData::SaveBinary(NiStream& kStream)
{
    NiExtraData::SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_fValue);
}
//---------------------------------------------------------------------------
bool NiFloatExtraData::IsEqual(NiObject* pObject)
{
    if (!pObject)
        return false;

    if (!NiIsExactKindOf(NiFloatExtraData, pObject))
        return false;

    NiFloatExtraData* pExtra = (NiFloatExtraData*) pObject;

    if (m_fValue != pExtra->m_fValue)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiFloatExtraData::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiExtraData::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiFloatExtraData::ms_RTTI.GetName()));

    pStrings->Add(NiGetViewerString("m_fValue", m_fValue));
}
//---------------------------------------------------------------------------
