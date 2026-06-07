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

#include "NiCamera.h"
#include "NiRangeLODData.h"
#include "NiMath.h"
#include "NiLODNode.h"

NiImplementRTTI(NiRangeLODData, NiLODData);

//---------------------------------------------------------------------------
NiRangeLODData::NiRangeLODData()
{
    m_kCenter = NiPoint3::ZERO;
    m_uiNumRanges = 0;
    m_pkRanges = NULL;
}
//---------------------------------------------------------------------------
NiRangeLODData::~NiRangeLODData()
{
    NiFree(m_pkRanges);
}
//---------------------------------------------------------------------------
void NiRangeLODData::UpdateWorldData(NiLODNode* pkLOD)
{
    NIASSERT(pkLOD);

    const NiTransform& kWorld = pkLOD->GetWorldTransform();

    // Update the World Center
    m_kWorldCenter = kWorld.m_Rotate * (kWorld.m_fScale * m_kCenter) + 
        kWorld.m_Translate;

    for (unsigned int i = 0; i < m_uiNumRanges; i++)
    {
        m_pkRanges[i].m_fWorldNear = kWorld.m_fScale * 
            m_pkRanges[i].m_fNear;

        m_pkRanges[i].m_fWorldFar = kWorld.m_fScale * 
            m_pkRanges[i].m_fFar;
    }
}
//---------------------------------------------------------------------------
NiLODData* NiRangeLODData::Duplicate()
{
    NiRangeLODData* pkData = NiNew NiRangeLODData;
    NIASSERT(pkData);

    pkData->SetCenter(m_kCenter);

    pkData->SetNumRanges(m_uiNumRanges);

    unsigned int uiDestSize = sizeof(m_pkRanges[0]) * m_uiNumRanges;
    NiMemcpy(pkData->m_pkRanges, m_pkRanges, uiDestSize);
   
    return (NiLODData*)pkData;
}
//---------------------------------------------------------------------------
void NiRangeLODData::SetNumRanges(unsigned int uiNumRanges)
{
    // Check to see if the ranges has changed
    if (uiNumRanges == m_uiNumRanges)
        return;

    // Check for 0 ranges;
    if (uiNumRanges == 0)
    {
        NiFree(m_pkRanges);
        m_pkRanges = NULL;
        m_uiNumRanges = 0;
        return;
    }

    // Allocate the new ranges
    Range* pkNewRanges = NiAlloc(Range, uiNumRanges);


    NIASSERT(pkNewRanges);

    // Copy over the old range Values
    if (m_pkRanges)
    {
        unsigned int uiDestSize = sizeof(pkNewRanges[0]) * NiMin(
            (int)m_uiNumRanges, (int)uiNumRanges);
        NiMemcpy(pkNewRanges, m_pkRanges, uiDestSize);
    }

    // Delete the old ranges
    NiFree(m_pkRanges);

    m_pkRanges = pkNewRanges;
    m_uiNumRanges = uiNumRanges;
}
//---------------------------------------------------------------------------
int NiRangeLODData::GetLODLevel(const NiCamera* pkCamera, 
    NiLODNode* pkLOD) const
{
    NI_UNUSED_ARG(pkLOD);
    NIASSERT(pkCamera);
    NIASSERT(pkLOD);

   
    NiPoint3 kDiff = m_kWorldCenter - pkCamera->GetWorldLocation();

    float fDist = NiAbs(pkCamera->GetWorldDirection().Dot(kDiff) * 
        pkCamera->GetLODAdjust());

    for (unsigned int uiIndex = 0; uiIndex < m_uiNumRanges; uiIndex++)
    {
        if ((fDist >= m_pkRanges[uiIndex].m_fWorldNear) &&
            (fDist < m_pkRanges[uiIndex].m_fWorldFar))
        {
            return uiIndex;
        }
    }

    return -1;
}

//---------------------------------------------------------------------------
int NiRangeLODData::GetLODIndex(int iLODLevel) const
{
    return NiClamp(iLODLevel, -1, m_uiNumRanges - 1);
}
//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiRangeLODData);
//---------------------------------------------------------------------------
void NiRangeLODData::CopyMembers(NiRangeLODData* pkDest, 
    NiCloningProcess&)
{
    NIASSERT(pkDest);

    // Set the Center use for all LOD Levels
    pkDest->m_kCenter = m_kCenter;
 
    pkDest->m_kWorldCenter = m_kWorldCenter;

    // Duplicate the Ranges
    pkDest->SetNumRanges(m_uiNumRanges);

    unsigned int uiDestSize = sizeof(m_pkRanges[0]) * m_uiNumRanges;
    NiMemcpy(pkDest->m_pkRanges, m_pkRanges, uiDestSize);

}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiRangeLODData);
//---------------------------------------------------------------------------
void NiRangeLODData::LoadBinary(NiStream& kStream)
{
    NiLODData::LoadBinary(kStream);

    m_kCenter.LoadBinary(kStream);

    unsigned int uiRanges;
    NiStreamLoadBinary(kStream, uiRanges);

    SetNumRanges(uiRanges);

    for (unsigned int i = 0; i < uiRanges; i++)
    {
        NiStreamLoadBinary(kStream, m_pkRanges[i].m_fNear);
        NiStreamLoadBinary(kStream, m_pkRanges[i].m_fFar);
    }
}
//---------------------------------------------------------------------------
void NiRangeLODData::LinkObject(NiStream& kStream)
{
    NiLODData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiRangeLODData::RegisterStreamables(NiStream& kStream)
{
    return NiLODData::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiRangeLODData::SaveBinary(NiStream& kStream)
{
    NiLODData::SaveBinary(kStream);

    m_kCenter.SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uiNumRanges);

    for (unsigned int i = 0; i < m_uiNumRanges; i++)
    {
        NiStreamSaveBinary(kStream, m_pkRanges[i].m_fNear);
        NiStreamSaveBinary(kStream, m_pkRanges[i].m_fFar);
    }
}
//---------------------------------------------------------------------------
bool NiRangeLODData::IsEqual(NiObject* pkObject)
{
    NiRangeLODData* pkLOD = (NiRangeLODData*) pkObject;

    if (m_uiNumRanges != pkLOD->m_uiNumRanges)
        return false;

    for (unsigned int i = 0; i < m_uiNumRanges; i++)
    {
        if ((pkLOD->m_pkRanges[i].m_fNear != m_pkRanges[i].m_fNear) ||
            (pkLOD->m_pkRanges[i].m_fFar != m_pkRanges[i].m_fFar))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void NiRangeLODData::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    pkStrings->Add(NiGetViewerString(NiRangeLODData::ms_RTTI.GetName()));

    pkStrings->Add(m_kCenter.GetViewerString("m_kCenter"));

    pkStrings->Add(m_kWorldCenter.GetViewerString("m_kWorldCenter"));

    for (unsigned int i = 0; i < m_uiNumRanges; i++)
    {
        char* pcString = NiAlloc(char, 128);

        NiSprintf(pcString, 128, "range[%d] = %g   %g", i, 
            m_pkRanges[i].m_fNear, m_pkRanges[i].m_fFar);
        pkStrings->Add(pcString);
    }
}
//---------------------------------------------------------------------------
