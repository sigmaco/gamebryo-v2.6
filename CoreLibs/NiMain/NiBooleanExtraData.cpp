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

#include "NiBooleanExtraData.h"
#include "NiBool.h"

NiImplementRTTI(NiBooleanExtraData,NiExtraData);


//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiBooleanExtraData);
//---------------------------------------------------------------------------
void NiBooleanExtraData::CopyMembers(NiBooleanExtraData* pDest,
    NiCloningProcess& kCloning)
{
    NiExtraData::CopyMembers(pDest, kCloning);

    pDest->m_bValue = m_bValue;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBooleanExtraData);
//---------------------------------------------------------------------------
void NiBooleanExtraData::LoadBinary(NiStream& kStream)
{
    NiExtraData::LoadBinary(kStream);
    NiBool bValue;
    NiStreamLoadBinary(kStream, bValue);
    m_bValue = bValue ? true : false;
}
//---------------------------------------------------------------------------
void NiBooleanExtraData::LinkObject(NiStream& kStream)
{
    NiExtraData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiBooleanExtraData::RegisterStreamables(NiStream& kStream)
{
    return NiExtraData::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiBooleanExtraData::SaveBinary(NiStream& kStream)
{
    NiExtraData::SaveBinary(kStream);
    NiBool bValue = m_bValue;
    NiStreamSaveBinary(kStream, bValue);
}
//---------------------------------------------------------------------------
bool NiBooleanExtraData::IsEqual(NiObject* pObject)
{
    if ( !pObject )
        return false;

    if (!NiIsExactKindOf(NiBooleanExtraData, pObject))
        return false;

    NiBooleanExtraData* pExtra = (NiBooleanExtraData*) pObject;

    if (m_bValue != pExtra->m_bValue)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiBooleanExtraData::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiExtraData::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiBooleanExtraData::ms_RTTI.GetName()));

    pStrings->Add(NiGetViewerString("m_bValue", m_bValue));
}
//---------------------------------------------------------------------------
