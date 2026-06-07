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
#include "NiAnimationPCH.h"

#include "NiBSplineBasisData.h"

NiImplementRTTI(NiBSplineBasisData, NiObject);

//---------------------------------------------------------------------------
NiBSplineBasisData::NiBSplineBasisData()
{
}
//---------------------------------------------------------------------------
NiBSplineBasisData::~NiBSplineBasisData ()
{   
}
//---------------------------------------------------------------------------
void NiBSplineBasisData::SetBasis(const NiBSplineBasis<float, 3>* 
    pkBasisDegree3)
{
    NIASSERT(pkBasisDegree3);

    m_kBasisDegree3 = *(pkBasisDegree3);
}
//---------------------------------------------------------------------------
NiBSplineBasis<float, 3>& NiBSplineBasisData::GetDegree3Basis() 
{
    return m_kBasisDegree3;
}
//---------------------------------------------------------------------------
// Cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiBSplineBasisData);
//---------------------------------------------------------------------------
void NiBSplineBasisData::CopyMembers(NiBSplineBasisData* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);
    pkDest->m_kBasisDegree3 = m_kBasisDegree3;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiBSplineBasisData);

//---------------------------------------------------------------------------
void NiBSplineBasisData::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);
    m_kBasisDegree3.LoadBinary(kStream);
}
//---------------------------------------------------------------------------
void NiBSplineBasisData::LinkObject(NiStream& stream)
{
    NiObject::LinkObject(stream);
}
//---------------------------------------------------------------------------
bool NiBSplineBasisData::RegisterStreamables(NiStream& stream)
{
    return NiObject::RegisterStreamables(stream);
}
//---------------------------------------------------------------------------
void NiBSplineBasisData::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);   
    m_kBasisDegree3.SaveBinary(kStream);
}
//---------------------------------------------------------------------------
bool NiBSplineBasisData::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
    {
        return false;
    }

    NiBSplineBasisData* pkData = (NiBSplineBasisData*) pkObject;
    return pkData->m_kBasisDegree3.IsEqual(&m_kBasisDegree3);
}
//---------------------------------------------------------------------------
void NiBSplineBasisData::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiObject::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiBSplineBasisData::ms_RTTI.GetName()));
}
//---------------------------------------------------------------------------

