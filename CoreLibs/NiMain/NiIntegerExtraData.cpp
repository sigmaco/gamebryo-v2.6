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

#include "NiIntegerExtraData.h"

NiImplementRTTI(NiIntegerExtraData,NiExtraData);

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiIntegerExtraData);
//---------------------------------------------------------------------------
void NiIntegerExtraData::CopyMembers(NiIntegerExtraData* pDest,
    NiCloningProcess& kCloning)
{
    NiExtraData::CopyMembers(pDest, kCloning);

    pDest->m_iValue = m_iValue;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiIntegerExtraData);
//---------------------------------------------------------------------------
void NiIntegerExtraData::LoadBinary(NiStream& kStream)
{
    NiExtraData::LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_iValue);
}
//---------------------------------------------------------------------------
void NiIntegerExtraData::LinkObject(NiStream& kStream)
{
    NiExtraData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiIntegerExtraData::RegisterStreamables(NiStream& kStream)
{
    return NiExtraData::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiIntegerExtraData::SaveBinary(NiStream& kStream)
{
    NiExtraData::SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_iValue);
}
//---------------------------------------------------------------------------
bool NiIntegerExtraData::IsEqual(NiObject* pObject)
{
    if (!pObject)
    {
        return false;
    }

    if (!NiIsExactKindOf(NiIntegerExtraData, pObject))
        return false;

    NiIntegerExtraData* pExtra = (NiIntegerExtraData*) pObject;

    if (m_iValue != pExtra->m_iValue)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiIntegerExtraData::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiExtraData::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiIntegerExtraData::ms_RTTI.GetName()));

    pStrings->Add(NiGetViewerString("m_iValue", m_iValue));
}
//---------------------------------------------------------------------------
