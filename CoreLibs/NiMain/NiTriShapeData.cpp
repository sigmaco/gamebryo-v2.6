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

#include "NiTriShapeData.h"
#include "NiBool.h"

NiImplementRTTI(NiTriShapeData,NiTriBasedGeomData);

//---------------------------------------------------------------------------
NiTriShapeData::NiTriShapeData(unsigned short usVertices,
    NiPoint3* pkVertex, NiPoint3* pkNormal, NiColorA* pkColor, 
    NiPoint2* pkTexture, unsigned short usNumTextureSets, 
    NiShaderRequirementDesc::NBTFlags eNBTMethod,
    unsigned short usTriangles, 
    unsigned short* pusTriList) :
    NiTriBasedGeomData(usVertices, pkVertex, pkNormal, pkColor, pkTexture,
        usNumTextureSets, eNBTMethod, usTriangles)
{
    NIASSERT(pusTriList);

    NIMEMASSERT(NiVerifyAddress(pusTriList));
    
    m_uiTriListLength = 3*m_usTriangles;
    m_pusTriList = pusTriList;
    m_pkSharedNormals = 0;
    m_usSharedNormalsArraySize = 0;
    m_pkSNAMemoryBlocks = 0;
}
//---------------------------------------------------------------------------
NiTriShapeData::NiTriShapeData()
{
    m_uiTriListLength = 0;
    m_pusTriList = 0;
    m_pkSharedNormals = 0;
    m_usSharedNormalsArraySize = 0;
    m_pkSNAMemoryBlocks = 0;
}
//---------------------------------------------------------------------------
NiTriShapeData::~NiTriShapeData()
{
    NiFree(m_pusTriList);

    // Will destroy all SNAMemBlocks
    NiDelete m_pkSNAMemoryBlocks;

    NiDelete[] m_pkSharedNormals;
}
//---------------------------------------------------------------------------
void NiTriShapeData::Replace(unsigned short usTriangles, 
    unsigned short* pusTriList)
{
    NIMEMASSERT(NiVerifyAddress(pusTriList));

    if (pusTriList != m_pusTriList)
        NiFree(m_pusTriList);

    SetTriangleCount(usTriangles);
    m_uiTriListLength = 3 * m_usTriangles;
    m_pusTriList = pusTriList;
}
//---------------------------------------------------------------------------
void NiTriShapeData::SetData(unsigned short usTriangles, 
    unsigned short* pusTriList)
{
    NIMEMASSERT(NiVerifyAddress(pusTriList));

    SetTriangleCount(usTriangles);
    m_uiTriListLength = 3*m_usTriangles;
    m_pusTriList = pusTriList;
}
//---------------------------------------------------------------------------
void NiTriShapeData::GetTriangleIndices(unsigned short i, unsigned short& i0,
    unsigned short& i1, unsigned short& i2) const
{
    // Call straight through to the non-virtual inline.
    GetTriangleIndices_Inline(i, i0, i1, i2);
}
//---------------------------------------------------------------------------
void NiTriShapeData::LetVerticesShareNormal(unsigned short usIndex0, 
    unsigned short usIndex1)
{
    if (usIndex0 == usIndex1)
        return;

    if (m_usSharedNormalsArraySize != m_usVertices)
    {
        NiDelete[] m_pkSharedNormals;
        NiDelete m_pkSNAMemoryBlocks;
        m_pkSharedNormals = 0;
    }

    if (m_pkSharedNormals == 0)
    {
        m_pkSharedNormals = NiNew SharedNormalArray[m_usVertices];
        m_usSharedNormalsArraySize = m_usVertices;
        m_pkSNAMemoryBlocks = NiNew SNAMemBlock(m_usSharedNormalsArraySize);
    }

    if (usIndex0 > m_usSharedNormalsArraySize || 
        usIndex1 > m_usSharedNormalsArraySize)
        return;

    // Build new arrays that contain both lists
    unsigned short usMaxCount0 = m_pkSharedNormals[usIndex0].GetCount();
    unsigned short usMaxCount1 = m_pkSharedNormals[usIndex1].GetCount();
    unsigned short* pusResultArray = NiAlloc(unsigned short, usMaxCount0 + 
        usMaxCount1 + 2);

    // Add first index's array elements
    unsigned short* pusArray0 = m_pkSharedNormals[usIndex0].GetArray();
    unsigned short usI, usCount = 0;
    for (usI = 0; usI < usMaxCount0; usI++)
    {
        // Make sure vertices do not share already
        if (pusArray0[usI] == usIndex1)
        {
            NiFree(pusResultArray);
            return;
        }
        else
            pusResultArray[usCount++] = pusArray0[usI];
    }

    // Add second index's array elements
    unsigned short* pusArray1 = m_pkSharedNormals[usIndex1].GetArray();
    for (usI = 0; usI < usMaxCount1; usI++)
    {
        // Make sure vertices do not share already
        if (pusArray1[usI] == usIndex0)
        {
            // This should never happen, since usIndex1 is in usIndex0's
            //   array iff usIndex0 is in usIndex1's array.
            NIASSERT(0);
            NiFree(pusResultArray);
            return;
        }
        else
            pusResultArray[usCount++] = pusArray1[usI];
    }

    // Add these indices
    pusResultArray[usCount++] = usIndex0;
    pusResultArray[usCount++] = usIndex1;
    NIASSERT(usCount == usMaxCount0 + usMaxCount1 + 2);
    // Set the SharedNormalArray for each member of the array
    for (usI = 0; usI < usCount; usI++)
    {
        unsigned short* pusNewArray = 0;

        // Find block with enough memory
        SNAMemBlock* pkBlock = m_pkSNAMemoryBlocks;
        while (pkBlock)
        {
            if ((unsigned int)usCount - 1 <= pkBlock->m_uiFreeBlockSize)
                break;

            pkBlock = pkBlock->m_pkNext;
        }
        // Suitable block not found - create new block
        if (pkBlock == NULL)
        {
            // Double size of previous block
            pkBlock = NiNew SNAMemBlock(
                m_pkSNAMemoryBlocks->m_uiBlockSize * 2);
            NIASSERT(pkBlock);
            pkBlock->m_pkNext = m_pkSNAMemoryBlocks;
            m_pkSNAMemoryBlocks = pkBlock;
        }

        pusNewArray = pkBlock->m_pusFreeBlock;
        pkBlock->m_uiFreeBlockSize -= usCount - 1;
        pkBlock->m_pusFreeBlock += usCount - 1;

        unsigned short usIndex = pusResultArray[usI];
        unsigned short usNewCount = 0;
        for (unsigned short usJ = 0; usJ < usCount; usJ++)
        {
            if (pusResultArray[usJ] != usIndex)
                pusNewArray[usNewCount++] = pusResultArray[usJ];
        }
        NIASSERT(usNewCount == usCount - 1);

        // Don't need to worry about deleting any existing arrays, since
        // the SharedNormalArray objects don't own the memory.
        m_pkSharedNormals[usIndex].SetArray(usNewCount, pusNewArray);
    }

    NiFree(pusResultArray);
}

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiTriShapeData);
//---------------------------------------------------------------------------
void NiTriShapeData::LoadBinary(NiStream& kStream)
{
    NiTriBasedGeomData::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uiTriListLength);

    NiBool bHasList;
    NiStreamLoadBinary(kStream, bHasList);

    if (bHasList && m_uiTriListLength > 0)
    {
        m_pusTriList = NiAlloc(unsigned short, m_uiTriListLength);
        NIASSERT(m_pusTriList);
        NiStreamLoadBinary(kStream, m_pusTriList, m_uiTriListLength);
    }

    NiStreamLoadBinary(kStream, m_usSharedNormalsArraySize);
    if (m_usSharedNormalsArraySize != 0)
    {
        m_pkSharedNormals = 
            NiNew SharedNormalArray[m_usSharedNormalsArraySize];

        // m_usSharedNormalsArraySize is first approximation of total size
        m_pkSNAMemoryBlocks = NiNew SNAMemBlock(m_usSharedNormalsArraySize);
        NIASSERT(m_pkSNAMemoryBlocks);

        for (unsigned short usI = 0; usI < m_usSharedNormalsArraySize; 
            usI++)
        {
            unsigned short* pusArray = 0;
            unsigned short usCount;
            NiStreamLoadBinary(kStream, usCount);

            if (usCount != 0)
            {
                // Find block with enough memory
                SNAMemBlock* pkBlock = m_pkSNAMemoryBlocks;
                while (pkBlock)
                {
                    if (usCount <= pkBlock->m_uiFreeBlockSize)
                        break;

                    pkBlock = pkBlock->m_pkNext;
                }

                // Suitable block not found - create new block
                if (pkBlock == NULL)
                {
                    // Double size of previous block
                    pkBlock = NiNew SNAMemBlock(
                        m_pkSNAMemoryBlocks->m_uiBlockSize * 2);
                    NIASSERT(pkBlock);
                    pkBlock->m_pkNext = m_pkSNAMemoryBlocks;
                    m_pkSNAMemoryBlocks = pkBlock;
                }

                pusArray = pkBlock->m_pusFreeBlock;
                pkBlock->m_uiFreeBlockSize -= usCount;
                pkBlock->m_pusFreeBlock += usCount;

                NiStreamLoadBinary(kStream, pusArray, usCount);
            }

            m_pkSharedNormals[usI].SetArray(usCount, pusArray);
        }
    }
}
//---------------------------------------------------------------------------
void NiTriShapeData::LinkObject(NiStream& kStream)
{
    NiTriBasedGeomData::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiTriShapeData::RegisterStreamables(NiStream& kStream)
{
    return NiTriBasedGeomData::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiTriShapeData::SaveBinary(NiStream& kStream)
{
    NiTriBasedGeomData::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uiTriListLength);

    NiBool bHasList = (m_pusTriList != NULL);
    NiStreamSaveBinary(kStream, bHasList);

    if (bHasList)
    {
        NiStreamSaveBinary(kStream, m_pusTriList, m_uiTriListLength);
    }

    NiStreamSaveBinary(kStream, m_usSharedNormalsArraySize);
    for (unsigned short usI = 0; usI < m_usSharedNormalsArraySize; usI++)
    {
        unsigned short usCount = m_pkSharedNormals[usI].GetCount();
        NiStreamSaveBinary(kStream, usCount);
        if (usCount)
        {
            NiStreamSaveBinary(kStream, m_pkSharedNormals[usI].GetArray(), 
                usCount);
        }
    }
}
//---------------------------------------------------------------------------
bool NiTriShapeData::IsEqual(NiObject* pkObject)
{
    if (!NiTriBasedGeomData::IsEqual(pkObject))
        return false;

    NiTriShapeData* pkData = (NiTriShapeData*) pkObject;

    if (m_uiTriListLength != pkData->m_uiTriListLength)
        return false;

    // if both are null, assume data has been precached,
    // and original data has been released properly
    if (!m_pusTriList && !pkData->m_pusTriList)
        return true;

    if (m_pusTriList && pkData->m_pusTriList)
    {
        unsigned int uiQuantity = m_uiTriListLength*sizeof(unsigned short);
        if (memcmp(m_pusTriList, pkData->m_pusTriList, uiQuantity) != 0)
            return false;
    }
    else
    {
        return false;
    }
    return true;
}
//---------------------------------------------------------------------------
