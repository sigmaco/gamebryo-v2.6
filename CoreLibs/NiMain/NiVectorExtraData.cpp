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

#include "NiVectorExtraData.h"

NiImplementRTTI(NiVectorExtraData,NiExtraData);

//---------------------------------------------------------------------------
NiVectorExtraData::NiVectorExtraData()
{
    m_afVector[0] = m_afVector[1] = m_afVector[2] = m_afVector[3] = 0.0f;
}
//---------------------------------------------------------------------------
NiVectorExtraData::NiVectorExtraData(const float* pfVector4)
{
    SetValue(pfVector4);
}
//---------------------------------------------------------------------------
NiVectorExtraData::NiVectorExtraData(const NiPoint3 kVector)
{
    SetValue(kVector);
    m_afVector[3] = 0.0f;
}
//---------------------------------------------------------------------------
void NiVectorExtraData::SetValue(const float* pfVector4)
{
    m_afVector[0] = pfVector4[0];
    m_afVector[1] = pfVector4[1];
    m_afVector[2] = pfVector4[2];
    m_afVector[3] = pfVector4[3];
}
//---------------------------------------------------------------------------
void NiVectorExtraData::SetValue(const NiPoint3 kVector3)
{
    m_afVector[0] = kVector3[0];
    m_afVector[1] = kVector3[1];
    m_afVector[2] = kVector3[2];
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiVectorExtraData);

//---------------------------------------------------------------------------
void NiVectorExtraData::CopyMembers(NiVectorExtraData* pDest,
    NiCloningProcess& kCloning)
{
    NiExtraData::CopyMembers(pDest, kCloning);

    pDest->m_afVector[0] = m_afVector[0];
    pDest->m_afVector[1] = m_afVector[1];
    pDest->m_afVector[2] = m_afVector[2];
    pDest->m_afVector[3] = m_afVector[3];
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiVectorExtraData);

//---------------------------------------------------------------------------
void NiVectorExtraData::LoadBinary(NiStream& kStream)
{
    NiExtraData::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_afVector[0]);
    NiStreamLoadBinary(kStream, m_afVector[1]);
    NiStreamLoadBinary(kStream, m_afVector[2]);
    NiStreamLoadBinary(kStream, m_afVector[3]);
}
//---------------------------------------------------------------------------
void NiVectorExtraData::LinkObject(NiStream& kStream)
{
    NiExtraData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiVectorExtraData::RegisterStreamables(NiStream& kStream)
{
    return NiExtraData::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiVectorExtraData::SaveBinary(NiStream& kStream)
{
    NiExtraData::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_afVector[0]);
    NiStreamSaveBinary(kStream, m_afVector[1]);
    NiStreamSaveBinary(kStream, m_afVector[2]);
    NiStreamSaveBinary(kStream, m_afVector[3]);
}
//---------------------------------------------------------------------------
bool NiVectorExtraData::IsEqual(NiObject* pObject)
{
    if (!pObject)
    {
        return false;
    }

    if (!NiIsExactKindOf(NiVectorExtraData, pObject))
        return false;

    NiVectorExtraData* pExtra = (NiVectorExtraData*) pObject;

    if ((m_afVector[0] != pExtra->m_afVector[0])
        || (m_afVector[1] != pExtra->m_afVector[1])
        || (m_afVector[2] != pExtra->m_afVector[2])
        || (m_afVector[3] != pExtra->m_afVector[3]))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiVectorExtraData::GetViewerStrings(NiViewerStringsArray* pStrings)
{
    NiExtraData::GetViewerStrings(pStrings);

    pStrings->Add(NiGetViewerString(NiVectorExtraData::ms_RTTI.GetName()));

    char* pcVectorString = NiAlloc(char,55);
    NiSprintf(pcVectorString, 55, "Vector = (%5.3f,%5.3f,%5.3f,%5.3f)",
            m_afVector[0], m_afVector[1], m_afVector[2], m_afVector[3]);
    pStrings->Add(pcVectorString);
}
//---------------------------------------------------------------------------
