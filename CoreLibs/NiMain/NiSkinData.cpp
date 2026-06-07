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

#include "NiSkinData.h"
#include "NiNode.h"
#include "NiTSet.h"
#include "NiBool.h"

NiImplementRTTI(NiSkinData, NiObject);

//---------------------------------------------------------------------------
// NiSkinData::BoneData member functions
//---------------------------------------------------------------------------
NiSkinData::BoneData::BoneData() : m_pkBoneVertData(NULL), m_usVerts(0)
{
}
//---------------------------------------------------------------------------
void NiSkinData::BoneData::SortAndMerge()
{
    // Sort the array of vertices which are influenced by this bone and merge
    // duplicate entries. The sort is worthwhile even if there are no
    // duplicates, since it should improve the locality of memory references.

    NIASSERT(m_usVerts > 0);

    qsort(m_pkBoneVertData, m_usVerts, sizeof(m_pkBoneVertData[0]),
        NiSkinData::BoneVertData::Compare);

    unsigned int i, uiUniqueVerts = 1;

    for (i = 1; i < m_usVerts; i++)
    {
        NIASSERT(m_pkBoneVertData[i - 1].m_usVert <=
            m_pkBoneVertData[i].m_usVert);

        if (m_pkBoneVertData[i - 1].m_usVert != m_pkBoneVertData[i].m_usVert)
        {
            uiUniqueVerts++;
        }
    }

    if (uiUniqueVerts != m_usVerts)
    {
        BoneVertData* pkNewBoneVertData = NiNew BoneVertData[uiUniqueVerts];
        unsigned int j;

        for (i = j = 0; i < uiUniqueVerts; i++)
        {
            unsigned short usVert = m_pkBoneVertData[j].m_usVert;
            float fWeight = 0.0f;

            do
            {
                fWeight += m_pkBoneVertData[j++].m_fWeight;
            } while (j < m_usVerts && m_pkBoneVertData[j].m_usVert == usVert);

            pkNewBoneVertData[i].m_usVert = usVert;
            pkNewBoneVertData[i].m_fWeight = fWeight;
        }

        NIASSERT(j == m_usVerts);

        NiDelete[] m_pkBoneVertData;
        m_pkBoneVertData = pkNewBoneVertData;
        m_usVerts = (unsigned short)uiUniqueVerts;
    }
}

//---------------------------------------------------------------------------
// NiSkinData::BoneVertData member functions
//---------------------------------------------------------------------------
NiSkinData::BoneVertData::BoneVertData() : m_usVert(0)
{
}
//---------------------------------------------------------------------------
int NiSkinData::BoneVertData::Compare(const void* pv0, const void* pv1)
{
    // A pointer to this comparison function can be passed to qsort to sort
    // an array of NiSkinData::BoneVertData objects according to their
    // indices.

    unsigned short usVert0 = ((NiSkinData::BoneVertData*) pv0)->m_usVert;
    unsigned short usVert1 = ((NiSkinData::BoneVertData*) pv1)->m_usVert;

    if (usVert0 < usVert1)
        return -1;
    if (usVert0 > usVert1)
        return 1;
    return 0;
}

//---------------------------------------------------------------------------
// NiSkinData member functions
//---------------------------------------------------------------------------
NiSkinData::NiSkinData(unsigned int uiBones, BoneData* pkBoneData,
    const NiTransform& kRootParentToSkin, const NiPoint3* pkVerts)
    : m_kRootParentToSkin(kRootParentToSkin), m_pkBoneData(pkBoneData), 
    m_uiBones(uiBones)
{
    ComputeBoneBounds(pkVerts);
}

//---------------------------------------------------------------------------
NiSkinData::~NiSkinData()
{
    FreeBoneVertData();
    NiDelete[] m_pkBoneData;
}

//---------------------------------------------------------------------------
void NiSkinData::FreeBoneVertData()
{
    // Free the data that is only used for software deformation.
    // If NULL, return.  This implies that a streaming operation was
    // cancelled since m_pkBoneData is set in LoadBinary
    if (m_pkBoneData == NULL)
        return;

    for (unsigned int i = 0; i < m_uiBones; i++)
    {
        NiDelete[] m_pkBoneData[i].m_pkBoneVertData;
        m_pkBoneData[i].m_pkBoneVertData = NULL;
    }
}

//---------------------------------------------------------------------------
// In this routine we compute a static bounding volume for each bone. These
// static volumes will be combined at runtime to efficiently derive a loose
// bounding volume for the skin
void NiSkinData::ComputeBoneBounds(const NiPoint3* pkVerts)
{
    NiPoint3Set verts;

    for (unsigned int i = 0; i < m_uiBones; i++)
    {
        for (unsigned int j = 0; j < m_pkBoneData[i].m_usVerts; j++)
        {
            verts.Add(pkVerts[m_pkBoneData[i].m_pkBoneVertData[j].m_usVert]);
        }

        NIASSERT(verts.GetSize() == m_pkBoneData[i].m_usVerts);
        m_pkBoneData[i].m_kBound.ComputeFromData(verts.GetSize(), 
            verts.GetBase());
        m_pkBoneData[i].m_kBound.Update(m_pkBoneData[i].m_kBound, 
            m_pkBoneData[i].m_kSkinToBone);
        verts.RemoveAll();
    }
}

//---------------------------------------------------------------------------
void NiSkinData::RemoveUnusedVertices(const int* piOldToNew)
{
    unsigned int i, j, k;

    for (i = 0; i < m_uiBones; i++)
    {
        unsigned int uiVerts = 0;

        // Count how many of the vertices influenced by this bone are still
        // active.

        BoneVertData* pkBoneVertData = m_pkBoneData[i].m_pkBoneVertData;

        for (j = 0; j < m_pkBoneData[i].m_usVerts; j++)
        {
            int iNewVert = piOldToNew[pkBoneVertData[j].m_usVert];

            if (iNewVert >= 0)
            {
                uiVerts++;
            }
        }

        // Since this routine is used when coinicident vertices are being
        // merged, no bone should be reduced to influencing 0 vertices.

        NIASSERT(uiVerts > 0);

        // Create a new BoneVertData array that only contains the vertices
        // that are still active.

        BoneVertData* pkNewBoneVertData = NiNew BoneVertData[uiVerts];
        NIASSERT(pkNewBoneVertData != NULL);
        
        for (j = k = 0; j < m_pkBoneData[i].m_usVerts; j++)
        {
            int iNewVert = piOldToNew[pkBoneVertData[j].m_usVert];
            
            if (iNewVert >= 0)
            {
                pkNewBoneVertData[k].m_usVert = (unsigned short)iNewVert;
                pkNewBoneVertData[k].m_fWeight = pkBoneVertData[j].m_fWeight;
                k++;
            }
        }
        
        NiDelete[] pkBoneVertData;
        m_pkBoneData[i].m_pkBoneVertData = pkNewBoneVertData;
        m_pkBoneData[i].m_usVerts = (unsigned short)uiVerts;
    }
}

//---------------------------------------------------------------------------
void NiSkinData::SortAndMergeBoneData()
{
    
    for (unsigned int i = 0; i < m_uiBones; i++)
    {
        m_pkBoneData[i].SortAndMerge();
    }
}

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiSkinData);
//---------------------------------------------------------------------------
void NiSkinData::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);
    
    m_kRootParentToSkin.LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_uiBones);

    m_pkBoneData = NiNew BoneData[m_uiBones];
    
    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 101))
    {
        kStream.ReadLinkID();   // m_spSkinPartition
    }

    NiBool bBoneVertData;
    NiStreamLoadBinary(kStream, bBoneVertData);

    for (unsigned int i = 0; i < m_uiBones; i++)
    {
        BoneData* pkBoneData = &m_pkBoneData[i];
        pkBoneData->m_kSkinToBone.LoadBinary(kStream);
        pkBoneData->m_kBound.LoadBinary(kStream);
        NiStreamLoadBinary(kStream, pkBoneData->m_usVerts);

        if (bBoneVertData)
        {
            pkBoneData->m_pkBoneVertData = 
                NiNew BoneVertData[pkBoneData->m_usVerts];
            
            for (unsigned int v = 0; v < pkBoneData->m_usVerts; v++)
            {
                BoneVertData* pkBoneVertData = 
                    &pkBoneData->m_pkBoneVertData[v];
                NiStreamLoadBinary(kStream, pkBoneVertData->m_usVert);
                NiStreamLoadBinary(kStream, pkBoneVertData->m_fWeight);
            }
        }
        else
        {
            pkBoneData->m_pkBoneVertData = NULL;
        }

    }
}

//---------------------------------------------------------------------------
void NiSkinData::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
    if (kStream.GetFileVersion() < NiStream::GetVersion(10, 1, 0, 101))
    {
        m_spSkinPartition = (NiSkinPartition*) kStream.GetObjectFromLinkID();
    }
}

//---------------------------------------------------------------------------
bool NiSkinData::RegisterStreamables(NiStream& kStream)
{
    if (!NiObject::RegisterStreamables(kStream))
        return false;

    return true;
}

//---------------------------------------------------------------------------
void NiSkinData::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);

    m_kRootParentToSkin.SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_uiBones);

    NiBool bBoneVertData = m_pkBoneData[0].m_pkBoneVertData != NULL;
    NiStreamSaveBinary(kStream, bBoneVertData);

    for (unsigned int i = 0; i < m_uiBones; i++)
    {
        BoneData* pkBoneData = &m_pkBoneData[i];
        pkBoneData->m_kSkinToBone.SaveBinary(kStream);
        pkBoneData->m_kBound.SaveBinary(kStream);
        NiStreamSaveBinary(kStream, pkBoneData->m_usVerts);

        if (bBoneVertData)
        {
            for (unsigned int v = 0; v < pkBoneData->m_usVerts; v++)
            {
                BoneVertData* pkBoneVertData = 
                    &pkBoneData->m_pkBoneVertData[v];

                NiStreamSaveBinary(kStream, pkBoneVertData->m_usVert);
                NiStreamSaveBinary(kStream, pkBoneVertData->m_fWeight);
            }
        }
    }
}

//---------------------------------------------------------------------------
bool NiSkinData::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
        return false;

    NiSkinData* pkSkinData = (NiSkinData*) pkObject;

    if (m_kRootParentToSkin != pkSkinData->m_kRootParentToSkin)
        return false;

    if (m_uiBones != pkSkinData->m_uiBones)
        return false;

    for (unsigned int i = 0; i < m_uiBones; i++)
    {
        BoneData* pkBoneData[2];
        
        pkBoneData[0] = &m_pkBoneData[i];
        pkBoneData[1] = &pkSkinData->m_pkBoneData[i];

        if (pkBoneData[0]->m_kSkinToBone != pkBoneData[1]->m_kSkinToBone)
            return false;

        if (pkBoneData[0]->m_kBound != pkBoneData[1]->m_kBound)
            return false;

        if (pkBoneData[0]->m_usVerts != pkBoneData[1]->m_usVerts)
            return false;

        for (unsigned int v = 0; v < pkBoneData[0]->m_usVerts; v++)
        {
            BoneVertData* pkBoneVertData[2];
            
            pkBoneVertData[0] = &pkBoneData[0]->m_pkBoneVertData[v];
            pkBoneVertData[1] = &pkBoneData[1]->m_pkBoneVertData[v];

            if (pkBoneVertData[0]->m_usVert != pkBoneVertData[1]->m_usVert)
                return false;

            if (pkBoneVertData[0]->m_fWeight != pkBoneVertData[1]->m_fWeight)
                return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------

