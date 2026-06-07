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

#include "NiColorExtraData.h"

NiImplementRTTI(NiColorExtraData,NiExtraData);


//---------------------------------------------------------------------------
NiColorExtraData::NiColorExtraData(const NiColorA kColorA)
{
    SetValue(kColorA);
}
//---------------------------------------------------------------------------
NiColorExtraData::NiColorExtraData(const NiColor kColor)
{
    SetValue(kColor);
    m_kColorA.a = 1.0f;
}
//---------------------------------------------------------------------------
void NiColorExtraData::SetValue(const NiColorA kColorA)
{
    m_kColorA = kColorA;
}
//---------------------------------------------------------------------------
void NiColorExtraData::SetValue(const NiColor kColor)
{
    m_kColorA.r = kColor.r;
    m_kColorA.g = kColor.g;
    m_kColorA.b = kColor.b;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiColorExtraData);
//---------------------------------------------------------------------------
void NiColorExtraData::CopyMembers(NiColorExtraData* pDest,
    NiCloningProcess& kCloning)
{
    NiExtraData::CopyMembers(pDest, kCloning);

    pDest->m_kColorA = m_kColorA;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiColorExtraData);
//---------------------------------------------------------------------------
void NiColorExtraData::LoadBinary(NiStream& kStream)
{
    NiExtraData::LoadBinary(kStream);

    m_kColorA.LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiColorExtraData::LinkObject(NiStream& kStream)
{
    NiExtraData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiColorExtraData::RegisterStreamables(NiStream& kStream)
{
    return NiExtraData::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiColorExtraData::SaveBinary(NiStream& kStream)
{
    NiExtraData::SaveBinary(kStream);
    m_kColorA.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiColorExtraData::IsEqual(NiObject* pObject)
{
    if (!pObject)
    {
        return false;
    }

    if (!NiIsExactKindOf(NiColorExtraData, pObject))
        return false;

    NiColorExtraData* pExtra = (NiColorExtraData*) pObject;

    if (m_kColorA != pExtra->m_kColorA)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiColorExtraData::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiExtraData::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiColorExtraData::ms_RTTI.GetName()));
    pStrings->Add(m_kColorA.GetViewerString("Color = "));
}
//---------------------------------------------------------------------------
