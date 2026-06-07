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

#include "NiAdditionalGeometryData.h"
#include "NiBool.h"

NiImplementRTTI(NiAdditionalGeometryData,NiObject);

//---------------------------------------------------------------------------
void NiAdditionalGeometryData::NiAGDDataStream::FillInByteSizeArray(
    unsigned int* pauiDataArray, unsigned int& uiIndex, 
    unsigned int uiArraySize)
{
    NI_UNUSED_ARG(uiArraySize);
    NIASSERT(m_uiType != AGD_NITYPE_INVALID && m_uiType < 
        AGD_NITYPE_COUNT);

#ifdef NIDEBUG
    unsigned int uiStartIdx = uiIndex;
#endif
    switch(m_uiType)
    {
       // FLOAT types. The control flow allows the switch statement
       // to cascade downwards, keeping us from having to implement a for
       // loop for each type.
       case AGD_NITYPE_FLOAT4:
            NIASSERT(uiIndex < uiArraySize);
            pauiDataArray[uiIndex] = sizeof(float);
            uiIndex++;
       case AGD_NITYPE_FLOAT3:
            NIASSERT(uiIndex < uiArraySize);
            pauiDataArray[uiIndex] = sizeof(float);
            uiIndex++;
       case AGD_NITYPE_FLOAT2:
            NIASSERT(uiIndex < uiArraySize);
            pauiDataArray[uiIndex] = sizeof(float);
            uiIndex++;
       case AGD_NITYPE_FLOAT1:
            NIASSERT(uiIndex < uiArraySize);
            pauiDataArray[uiIndex] = sizeof(float);
            uiIndex++;
            break;

       // LONG and ULONG types. The control flow allows the switch statement
       // to cascade downwards, keeping us from having to implement a for
       // loop for each type.
       case AGD_NITYPE_ULONG4:
       case AGD_NITYPE_LONG4:
            NIASSERT(uiIndex < uiArraySize);
            pauiDataArray[uiIndex] = sizeof(long);
            uiIndex++;
       case AGD_NITYPE_ULONG3:
       case AGD_NITYPE_LONG3:
            NIASSERT(uiIndex < uiArraySize);
            pauiDataArray[uiIndex] = sizeof(long);
            uiIndex++;
       case AGD_NITYPE_ULONG2:
       case AGD_NITYPE_LONG2:
            NIASSERT(uiIndex < uiArraySize);
            pauiDataArray[uiIndex] = sizeof(long);
            uiIndex++;
       case AGD_NITYPE_ULONG1:
       case AGD_NITYPE_LONG1:
            NIASSERT(uiIndex < uiArraySize);
            pauiDataArray[uiIndex] = sizeof(long);
            uiIndex++;
            break;   

        // SHORT and USHORT types. The control flow allows the switch statement
        // to cascade downwards, keeping us from having to implement a for
        // loop for each type.
        case AGD_NITYPE_USHORT4: 
        case AGD_NITYPE_SHORT4: 
            NIASSERT(uiIndex < uiArraySize);
            pauiDataArray[uiIndex] = sizeof(short);
            uiIndex++;
        case AGD_NITYPE_USHORT3: 
        case AGD_NITYPE_SHORT3: 
            NIASSERT(uiIndex < uiArraySize);
            pauiDataArray[uiIndex] = sizeof(short);
            uiIndex++;
        case AGD_NITYPE_USHORT2: 
        case AGD_NITYPE_SHORT2: 
            NIASSERT(uiIndex < uiArraySize);
            pauiDataArray[uiIndex] = sizeof(short);
            uiIndex++;
        case AGD_NITYPE_USHORT1: 
        case AGD_NITYPE_SHORT1: 
            NIASSERT(uiIndex < uiArraySize);
            pauiDataArray[uiIndex] = sizeof(short);
            uiIndex++;
            break;
        
        // BYTE and UBYTE types. The control flow allows the switch statement
        // to cascade downwards, keeping us from having to implement a for
        // loop for each type.
        case AGD_NITYPE_BLEND4: 
        case AGD_NITYPE_UBYTE4: 
        case AGD_NITYPE_BYTE4:  
            NIASSERT(uiIndex < uiArraySize);
            pauiDataArray[uiIndex] = sizeof(char);
            uiIndex++;
        case AGD_NITYPE_BLEND3: 
        case AGD_NITYPE_UBYTE3: 
        case AGD_NITYPE_BYTE3:  
            NIASSERT(uiIndex < uiArraySize);
            pauiDataArray[uiIndex] = sizeof(char);
            uiIndex++;
        case AGD_NITYPE_BLEND2: 
        case AGD_NITYPE_UBYTE2: 
        case AGD_NITYPE_BYTE2:  
            NIASSERT(uiIndex < uiArraySize);
            pauiDataArray[uiIndex] = sizeof(char);
            uiIndex++;
        case AGD_NITYPE_BLEND1: 
        case AGD_NITYPE_UBYTE1: 
        case AGD_NITYPE_BYTE1:  
            NIASSERT(uiIndex < uiArraySize);
            pauiDataArray[uiIndex] = sizeof(char);
            uiIndex++;    
            break;  
    }

#ifdef NIDEBUG
    unsigned int uiSize = 0;
    for (unsigned int ui = uiStartIdx; ui < uiIndex; ui++)
    {
        uiSize += pauiDataArray[ui]; 
    }
    NIASSERT(uiSize == m_uiUnitSize);
#endif
}
//---------------------------------------------------------------------------
//  NiAGDDataStream streaming methods
//---------------------------------------------------------------------------
void NiAdditionalGeometryData::NiAGDDataStream::LoadBinary(NiStream& kStream)
{
    NiStreamLoadBinary(kStream, m_uiType);
    NiStreamLoadBinary(kStream, m_uiUnitSize);
    NiStreamLoadBinary(kStream, m_uiTotalSize);
    NiStreamLoadBinary(kStream, m_uiStride);
    NiStreamLoadBinary(kStream, m_uiBlockIndex);
    NiStreamLoadBinary(kStream, m_uiBlockOffset);
    NiStreamLoadBinary(kStream, m_uFlags);
}
//---------------------------------------------------------------------------
void NiAdditionalGeometryData::NiAGDDataStream::SaveBinary(NiStream& kStream)
{
    NiStreamSaveBinary(kStream, m_uiType);
    NiStreamSaveBinary(kStream, m_uiUnitSize);
    NiStreamSaveBinary(kStream, m_uiTotalSize);
    NiStreamSaveBinary(kStream, m_uiStride);
    NiStreamSaveBinary(kStream, m_uiBlockIndex);
    NiStreamSaveBinary(kStream, m_uiBlockOffset);
    NiStreamSaveBinary(kStream, m_uFlags);
}
//---------------------------------------------------------------------------
void NiAdditionalGeometryData::NiAGDDataStream::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    switch(m_uiType)
    {
        case AGD_NITYPE_INVALID: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_INVALID"));
            break;   
        case AGD_NITYPE_FLOAT1: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_FLOAT1"));
            break;   
        case AGD_NITYPE_FLOAT2:
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_FLOAT2"));
            break;         
        case AGD_NITYPE_FLOAT3: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_FLOAT3"));
            break;          
        case AGD_NITYPE_FLOAT4:
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_FLOAT4"));
            break;        
        case AGD_NITYPE_LONG1: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_LONG1"));
            break;          
        case AGD_NITYPE_LONG2:
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_LONG2"));
            break;          
        case AGD_NITYPE_LONG3: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_LONG3"));
            break;          
        case AGD_NITYPE_LONG4: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_LONG4"));
            break;           
        case AGD_NITYPE_ULONG1:
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_ULONG1"));
            break;          
        case AGD_NITYPE_ULONG2: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_ULONG2"));
            break;          
        case AGD_NITYPE_ULONG3: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_ULONG3"));
            break;          
        case AGD_NITYPE_ULONG4: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_ULONG4"));
            break;          
        case AGD_NITYPE_SHORT1: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_SHORT1"));
            break;          
        case AGD_NITYPE_SHORT2: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_SHORT2"));
            break;          
        case AGD_NITYPE_SHORT3: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_SHORT3"));
            break;          
        case AGD_NITYPE_SHORT4: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_SHORT4"));
            break;          
        case AGD_NITYPE_USHORT1: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_USHORT1"));
            break;         
        case AGD_NITYPE_USHORT2: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_USHORT2"));
            break;         
        case AGD_NITYPE_USHORT3: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_USHORT3"));
            break;         
        case AGD_NITYPE_USHORT4: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_USHORT4"));
            break;         
        case AGD_NITYPE_BYTE1: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_BYTE1"));
            break;           
        case AGD_NITYPE_BYTE2: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_BYTE2"));
            break;           
        case AGD_NITYPE_BYTE3: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_BYTE3"));
            break;          
        case AGD_NITYPE_BYTE4: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_BYTE4"));
            break;           
        case AGD_NITYPE_UBYTE1: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_UBYTE1"));
            break;          
        case AGD_NITYPE_UBYTE2: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_UBYTE2"));
            break;          
        case AGD_NITYPE_UBYTE3: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_UBYTE3"));
            break;          
        case AGD_NITYPE_UBYTE4: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_UBYTE4"));
            break;          
        case AGD_NITYPE_BLEND1: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_BLEND1"));
            break;          
        case AGD_NITYPE_BLEND2: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_BLEND2"));
            break;          
        case AGD_NITYPE_BLEND3: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_BLEND3"));
            break;          
        case AGD_NITYPE_BLEND4: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_BLEND4"));
            break;         
        case AGD_NITYPE_COUNT: 
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "AGD_NITYPE_COUNT"));
            break;  
        default:
            pkStrings->Add(NiGetViewerString("        m_uiType", 
                "UNKNOWN!!!"));
            break;
    }

    pkStrings->Add(NiGetViewerString("        m_uiUnitSize", m_uiUnitSize));
    pkStrings->Add(NiGetViewerString("        m_uiTotalSize", m_uiTotalSize));
    pkStrings->Add(NiGetViewerString("        m_uiStride", m_uiStride));
    pkStrings->Add(NiGetViewerString("        m_uiBlockIndex", 
        m_uiBlockIndex));
    pkStrings->Add(NiGetViewerString("        m_uiBlockOffset", 
        m_uiBlockOffset));
    pkStrings->Add(NiGetViewerString("        Keep", GetKeep()));

    switch(GetConsistency())
    {
        case NiGeometryData::MUTABLE:
            pkStrings->Add(NiGetViewerString("        Consistency", 
                "MUTABLE"));
            break;
        case NiGeometryData::STATIC:
            pkStrings->Add(NiGetViewerString("        Consistency", "STATIC"));
            break;
        case NiGeometryData::VOLATILE:
            pkStrings->Add(NiGetViewerString("        Consistency", 
                "VOLATILE"));
            break;
        default:
            pkStrings->Add(NiGetViewerString("        Consistency", 
                "UNKNOWN!!!"));
            break;
    }
}
//---------------------------------------------------------------------------
bool NiAdditionalGeometryData::NiAGDDataStream::IsEqual(
    const NiAGDDataStream* pkOther)
{
    if (m_uiType != pkOther->m_uiType ||
        m_uiUnitSize != pkOther->m_uiUnitSize ||
        m_uiTotalSize != pkOther->m_uiTotalSize ||
        m_uiStride != pkOther->m_uiStride ||
        m_uiBlockIndex != pkOther->m_uiBlockIndex ||
        m_uiBlockOffset != pkOther->m_uiBlockOffset ||
        m_uFlags != pkOther->m_uFlags)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
#define MAX_STREAM_COMP_ARRAY_SIZE 25

//---------------------------------------------------------------------------
//  NiAGDDataBlock streaming methods
//---------------------------------------------------------------------------
void NiAdditionalGeometryData::NiAGDDataBlock::LoadBinary(NiStream& kStream,
    unsigned int, NiAGDDataStream*, 
    unsigned int, unsigned short usVertexCount)
{
    NiStreamLoadBinary(kStream, m_uiDataBlockSize);
    m_pucDataBlock = NiAlloc(unsigned char, m_uiDataBlockSize);

    unsigned int auiCompArray[MAX_STREAM_COMP_ARRAY_SIZE];
    unsigned int auiBlockOffsets[MAX_STREAM_COMP_ARRAY_SIZE];
    
    // Read in the number of contiguous blocks in the array
    unsigned int uiNumBlocks;
    NiStreamLoadBinary(kStream, uiNumBlocks);

    // Read in the start/end offsets of the contiguous blocks
    // from the data array.
    NIASSERT(uiNumBlocks < MAX_STREAM_COMP_ARRAY_SIZE);
    NiStreamLoadBinary(kStream, auiBlockOffsets, uiNumBlocks);

    // For each contiguous block, read in and handle 
    // endian conversion for each of the components of 
    // the block.
    for (unsigned int um = 0; um < uiNumBlocks; um++)
    {
        unsigned int uiNumComps;
        NiStreamLoadBinary(kStream, uiNumComps);
        NIASSERT(uiNumComps < MAX_STREAM_COMP_ARRAY_SIZE);
        NiStreamLoadBinary(kStream, auiCompArray, uiNumComps);

        unsigned int uiBlockStart = auiBlockOffsets[um];
        void* pvBlockStart = m_pucDataBlock + uiBlockStart;
            
        NiStreamLoadBinary(kStream, pvBlockStart, usVertexCount,
            auiCompArray, uiNumComps);
    }
}
//---------------------------------------------------------------------------
void NiAdditionalGeometryData::NiAGDDataBlock::SaveBinary(NiStream& kStream,
    unsigned int uiBlockIdx, NiAGDDataStream* pkDataStreams, 
    unsigned int uiNumStreams, unsigned short usVertexCount)
{
    NiStreamSaveBinary(kStream, m_uiDataBlockSize);
    
    unsigned int auiCompArray[MAX_STREAM_COMP_ARRAY_SIZE];
    NiAGDDataStream* apkStreamArray[MAX_STREAM_COMP_ARRAY_SIZE];
    unsigned int auiBlockOffsetIndices[MAX_STREAM_COMP_ARRAY_SIZE];
    unsigned int auiBlockOffsets[MAX_STREAM_COMP_ARRAY_SIZE];
    unsigned int uiEndIdx = 0;

    // 1) We need to determine what NiAGDDataStreams
    // define a contiguous section of this block of data.
    for (unsigned int ui = 0; ui < uiNumStreams; ui++)
    {
        if (pkDataStreams[ui].m_uiBlockIndex == uiBlockIdx)
        {
            NIASSERT(uiEndIdx < MAX_STREAM_COMP_ARRAY_SIZE);
            apkStreamArray[uiEndIdx] = &pkDataStreams[ui];
            uiEndIdx++;
        }
    }

    // 2) Now we need to sort the NiAGDDataStreams affecting this
    // continguous block of data. 
    for (unsigned int uj = 0; uj < uiEndIdx; uj++)
    {
        NiAGDDataStream* pkCurStream = apkStreamArray[uj];
        NiAGDDataStream* pkNextStream;
        // Insertion sort by block offset
        for (unsigned int uk = 0; uk < uj; uk++)
        {
            pkNextStream = apkStreamArray[uk];
            if (pkNextStream->m_uiBlockOffset >
                pkCurStream->m_uiBlockOffset)
            {
                apkStreamArray[uk] = pkCurStream;
                pkCurStream = pkNextStream;
            }
        }
        apkStreamArray[uj] = pkCurStream;
    }

    // 3) Determine the unique contiguous blocks
    unsigned int uiNumBlocks = 1;
    auiBlockOffsetIndices[0] = 0;
    auiBlockOffsets[0] = 0;
    NIASSERT(apkStreamArray[0]->m_uiBlockOffset == 0);
    NiAGDDataStream* pkLastStreamStart = apkStreamArray[0];
    for (unsigned int ul = 1; ul < uiEndIdx; ul++)
    {
        if (apkStreamArray[ul]->m_uiBlockOffset > 
            pkLastStreamStart->m_uiTotalSize + 
            pkLastStreamStart->m_uiBlockOffset)
        {
            NIASSERT(uiNumBlocks < MAX_STREAM_COMP_ARRAY_SIZE);
            auiBlockOffsetIndices[uiNumBlocks] = ul;
            auiBlockOffsets[uiNumBlocks] = 
                apkStreamArray[ul]->m_uiBlockOffset;
            uiNumBlocks++;
            pkLastStreamStart = apkStreamArray[ul];
        }
    }
    NIASSERT(uiNumBlocks < MAX_STREAM_COMP_ARRAY_SIZE);
    auiBlockOffsetIndices[uiNumBlocks] = uiEndIdx;

    // Stream out the block offset arrays.
    // This will allow load to go MUCH faster.
    NiStreamSaveBinary(kStream, uiNumBlocks);
    NiStreamSaveBinary(kStream, auiBlockOffsets, uiNumBlocks);

    // 4) Foreach contiguous block in this NiAGDDataBlock, stream the data.
    //unsigned int uiCompIndex = 0;
    unsigned int uiCompSize = 0;
    for (unsigned int um = 0; um < uiNumBlocks; um++)
    {
        unsigned int uiCompIndex = 0;
        unsigned int uiBlockStart = auiBlockOffsetIndices[um];
        unsigned int uiBlockEnd = auiBlockOffsetIndices[um + 1];
        unsigned int uiTotalBytes = 0;
        for (unsigned int un = uiBlockStart; un < uiBlockEnd; un++)
        {
            apkStreamArray[un]->FillInByteSizeArray(auiCompArray,
                uiCompIndex, MAX_STREAM_COMP_ARRAY_SIZE);
            uiTotalBytes += apkStreamArray[un]->m_uiTotalSize;
            uiCompSize += apkStreamArray[un]->m_uiUnitSize;
        }

        unsigned int uiNumComps = uiCompIndex;
        NIASSERT(uiNumComps < MAX_STREAM_COMP_ARRAY_SIZE);
        NiStreamSaveBinary(kStream, uiNumComps);
        NiStreamSaveBinary(kStream, auiCompArray, uiNumComps);

        void* pvBlockStart = m_pucDataBlock + 
            apkStreamArray[uiBlockStart]->m_uiBlockOffset;

        NIASSERT(uiTotalBytes / uiCompSize == usVertexCount);
        NiStreamSaveBinary(kStream, pvBlockStart, usVertexCount,
            auiCompArray, uiNumComps);
    }
}
//---------------------------------------------------------------------------
void NiAdditionalGeometryData::NiAGDDataBlock::GetViewerStrings(
    NiViewerStringsArray* pkStrings)
{
    pkStrings->Add(NiGetViewerString("        m_uiDataBlockSize", 
        m_uiDataBlockSize));
    pkStrings->Add(NiGetViewerString("        m_pucDataBlock", 
        m_pucDataBlock));
}
//---------------------------------------------------------------------------
bool NiAdditionalGeometryData::NiAGDDataBlock::IsEqual(
    const NiAGDDataBlock* pkOther)
{
    if (m_uiDataBlockSize != pkOther->m_uiDataBlockSize)
        return false;

    if (0 != memcmp(m_pucDataBlock, pkOther->m_pucDataBlock, 
        m_uiDataBlockSize))
    {
        return false; 
    }

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// NiAdditionalGeometryData methods
//---------------------------------------------------------------------------
NiAdditionalGeometryData::~NiAdditionalGeometryData()
{
    //  Free all data...
    for (unsigned int i = 0; i < m_aDataBlocks.GetSize(); i++)
        RemoveDataBlock(i);

    if (!m_pkDataStreamEntries)
        return;

    //  Just delete the entries.
    NiDelete [] m_pkDataStreamEntries;
}
//---------------------------------------------------------------------------
bool NiAdditionalGeometryData::SetDataBlock(unsigned int uiIndex, 
    unsigned char* pucData, unsigned int uiTotalSize, bool bCopyData)
{
    NiAGDDataBlock* pkBlock = NULL;
    if (uiIndex < m_aDataBlocks.GetSize())
        pkBlock = m_aDataBlocks.GetAt(uiIndex);

    if (pkBlock == 0)
    {
        //  There wasn't any data there to begin with. Create one.
        pkBlock = NiNew NiAGDDataBlock();
        if (pkBlock == 0)
        {
            NIASSERT(pkBlock);
            return false;
        }
    }
    else
    {
        if (pkBlock->m_uiDataBlockSize != uiTotalSize)
        {
            NiFree(pkBlock->m_pucDataBlock);
            pkBlock->m_pucDataBlock = 0;
        }
    }

    if (pkBlock->m_pucDataBlock == 0)
    {
        if (bCopyData)
        {
            unsigned char* pucCopyData = NiAlloc(unsigned char, uiTotalSize);
            memcpy(pucCopyData, pucData, uiTotalSize);
            pucData = pucCopyData;
        }

        pkBlock->m_uiDataBlockSize = uiTotalSize;
        pkBlock->m_pucDataBlock = pucData;
        NIASSERT(pkBlock->m_pucDataBlock);
    }

    m_aDataBlocks.SetAtGrow(uiIndex, pkBlock);

    return true;
}
//---------------------------------------------------------------------------
const unsigned char* NiAdditionalGeometryData::GetDataBlock(
    unsigned int uiIndex, unsigned int& uiBlockSize)
{
    uiBlockSize = 0;

    if (uiIndex >= m_aDataBlocks.GetSize())
        return 0;

    NiAGDDataBlock* pkBlock = m_aDataBlocks.GetAt(uiIndex);
    if (pkBlock == 0)
        return 0;

    uiBlockSize = pkBlock->m_uiDataBlockSize;

    return pkBlock->m_pucDataBlock;
}
//---------------------------------------------------------------------------
bool NiAdditionalGeometryData::RemoveDataBlock(unsigned int uiIndex,
    bool bRemoveAssociatedStreams)
{
    NiAGDDataBlock* pkBlock = NULL;
    if (uiIndex < m_aDataBlocks.GetSize())
        pkBlock = m_aDataBlocks.GetAt(uiIndex);

    if (pkBlock == 0)
        return false;

    NiFree(pkBlock->m_pucDataBlock);
    NiDelete pkBlock;

    m_aDataBlocks.SetAt(uiIndex, 0);

    if (bRemoveAssociatedStreams)
    {
        for (unsigned int ui = 0; ui < m_uiDataStreamCount; ui++)
        {
            if (uiIndex == m_pkDataStreamEntries[ui].m_uiBlockIndex)
                RemoveDataStream(ui);
        }
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiAdditionalGeometryData::SetDataStream(unsigned int uiStreamIndex, 
    unsigned int uiBlockIndex, unsigned int uiBlockOffset, 
    unsigned int uiType, unsigned short usCount, 
    unsigned int uiUnitSize, unsigned int uiStride)
{
    if (usCount != m_usVertexCount)
        return false;

    //  Validity check
    if (uiStreamIndex >= m_uiDataStreamCount)
        return false;

    if (m_pkDataStreamEntries == 0)
        return false;

    if (uiBlockIndex > m_aDataBlocks.GetSize())
        return false;

    NiAGDDataStream* pkDataStream = 
        &m_pkDataStreamEntries[uiStreamIndex];

    pkDataStream->m_uiType = uiType;
    pkDataStream->m_uiBlockOffset = uiBlockOffset;
    pkDataStream->m_uiBlockIndex = uiBlockIndex;
    pkDataStream->m_uiStride = uiStride;
    pkDataStream->m_uiUnitSize = uiUnitSize;
    pkDataStream->m_uiTotalSize = uiUnitSize * usCount;
    
    return true;
}
//---------------------------------------------------------------------------
bool NiAdditionalGeometryData::GetDataStream(unsigned int uiStreamIndex, 
    unsigned char*& pucData, unsigned int& uiType, unsigned short& usCount, 
    unsigned int& uiTotalSize, unsigned int& uiUnitSize, 
    unsigned int& uiStride)
{
    //  Validity check
    if (uiStreamIndex >= m_uiDataStreamCount)
        return false;

    if (m_pkDataStreamEntries == 0)
        return false;

    NiAGDDataStream* pkDataStream = 
        &m_pkDataStreamEntries[uiStreamIndex];

    uiType = pkDataStream->m_uiType;
    
    if (uiType == AGD_NITYPE_INVALID)
        return false;

    usCount = this->m_usVertexCount;
    uiTotalSize = pkDataStream->m_uiTotalSize;
    uiUnitSize = pkDataStream->m_uiUnitSize;
    uiStride = pkDataStream->m_uiStride;

    if (pkDataStream->m_uiBlockIndex > m_aDataBlocks.GetSize())
        return false;

    NiAGDDataBlock* pkBlock = 
        m_aDataBlocks.GetAt(pkDataStream->m_uiBlockIndex);

    if (pkBlock == NULL || 
        pkBlock->m_uiDataBlockSize < pkDataStream->m_uiBlockOffset)
    {
        return false;
    }

    pucData = pkBlock->m_pucDataBlock + pkDataStream->m_uiBlockOffset;
    return true;
}
//---------------------------------------------------------------------------
bool NiAdditionalGeometryData::RemoveDataStream(unsigned int uiStreamIndex)
{
    //  Validity check
    if (uiStreamIndex >= m_uiDataStreamCount)
        return false;

    if (m_pkDataStreamEntries == 0)
        return false;

    //  See if there is already data there...
    NiAGDDataStream* pkDataStream = 
        &m_pkDataStreamEntries[uiStreamIndex];

    pkDataStream->m_uiType = AGD_NITYPE_INVALID;
    pkDataStream->m_uiTotalSize = 0;
    pkDataStream->m_uiUnitSize = 0;
    pkDataStream->m_uiStride = 0;
    pkDataStream->m_uiBlockIndex = 0;
    pkDataStream->m_uiBlockOffset = 0;
    return true;
}
//---------------------------------------------------------------------------
void NiAdditionalGeometryData::SetDataBlockCount(unsigned int uiCount)
{
    NIASSERT(uiCount >= GetDataBlockCount());
    m_aDataBlocks.SetSize(uiCount);
}
//---------------------------------------------------------------------------
unsigned int NiAdditionalGeometryData::GetDataBlockCount()
{
    return m_aDataBlocks.GetEffectiveSize();
}
//---------------------------------------------------------------------------
void NiAdditionalGeometryData::SetDataStreamCount(unsigned int uiCount)
{
    NIASSERT(uiCount >= GetDataStreamCount());
    NiAGDDataStream* pkOldEntries = m_pkDataStreamEntries;
    m_pkDataStreamEntries = NiNew NiAGDDataStream[uiCount];
    memcpy(m_pkDataStreamEntries, pkOldEntries, sizeof(NiAGDDataStream) * 
        m_uiDataStreamCount);
    m_uiDataStreamCount = uiCount;
    NiDelete [] pkOldEntries;
}
//---------------------------------------------------------------------------
unsigned int NiAdditionalGeometryData::GetDataStreamCount()
{
    if (m_pkDataStreamEntries == NULL)
        return 0;
    unsigned int uiCount = 0;
    for (unsigned int ui = 0; ui < m_uiDataStreamCount; ++ui)
    {
        if (m_pkDataStreamEntries[ui].m_uiType != AGD_NITYPE_INVALID)
            ++uiCount;
    }
    return uiCount;
}
//---------------------------------------------------------------------------
bool NiAdditionalGeometryData::SetConsistency(unsigned int uiStreamIndex, 
    NiGeometryData::Consistency eConsistency)
{
    //  Validity check
    if (uiStreamIndex >= m_uiDataStreamCount)
        return false;

    if (m_pkDataStreamEntries == 0)
        return false;

    //  See if there is already data there...
    NiAGDDataStream* pkDataStream = 
        &m_pkDataStreamEntries[uiStreamIndex];

    pkDataStream->SetConsistency(eConsistency);
    return true;
}
//---------------------------------------------------------------------------
bool NiAdditionalGeometryData::GetConsistency(unsigned int uiStreamIndex,
    NiGeometryData::Consistency& eConsistency) const
{
    //  Validity check
    if (uiStreamIndex >= m_uiDataStreamCount)
        return false;

    if (m_pkDataStreamEntries == 0)
        return false;

    //  See if there is already data there...
    NiAGDDataStream* pkDataStream = 
        &m_pkDataStreamEntries[uiStreamIndex];
    eConsistency = pkDataStream->GetConsistency();
    return true;
}
//---------------------------------------------------------------------------
bool NiAdditionalGeometryData::SetKeep(unsigned int uiStreamIndex, bool bKeep)
{
    //  Validity check
    if (uiStreamIndex >= m_uiDataStreamCount)
        return false;

    if (m_pkDataStreamEntries == 0)
        return false;

    //  See if there is already data there...
    NiAGDDataStream* pkDataStream = 
        &m_pkDataStreamEntries[uiStreamIndex];

    pkDataStream->SetKeep(bKeep);
    return true;
}
//---------------------------------------------------------------------------
bool NiAdditionalGeometryData::GetKeep(unsigned int uiStreamIndex, 
    bool& bKeep) const
{
    //  Validity check
    if (uiStreamIndex >= m_uiDataStreamCount)
        return false;

    if (m_pkDataStreamEntries == 0)
        return false;

    //  See if there is already data there...
    NiAGDDataStream* pkDataStream = 
        &m_pkDataStreamEntries[uiStreamIndex];
    bKeep = pkDataStream->GetKeep();
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiAdditionalGeometryData);
//---------------------------------------------------------------------------
void NiAdditionalGeometryData::LoadBinary(NiStream& kStream)
{
    NiObject::LoadBinary(kStream);
    NiStreamLoadBinary(kStream, m_usVertexCount);
    NiStreamLoadBinary(kStream, m_uiDataStreamCount);
    if (m_uiDataStreamCount)
    {
        m_pkDataStreamEntries = NiNew NiAGDDataStream[m_uiDataStreamCount];
        for (unsigned int ui = 0; ui < m_uiDataStreamCount; ui++)
            m_pkDataStreamEntries[ui].LoadBinary(kStream);
    }

    unsigned int uiBlockCount = 0;
    NiStreamLoadBinary(kStream, uiBlockCount);
    m_aDataBlocks.SetSize(uiBlockCount);
    for (unsigned int uj = 0; uj < uiBlockCount; uj++)
    {
        NiBool bValid;
        NiStreamLoadBinary(kStream, bValid);
        
        if (bValid)
        {
            NiAGDDataBlock* pkBlock = NiNew NiAGDDataBlock();
            pkBlock->LoadBinary(kStream, uj, m_pkDataStreamEntries, 
                m_uiDataStreamCount, m_usVertexCount);
            m_aDataBlocks.SetAt(uj, pkBlock);
        }
        else
        {
            m_aDataBlocks.SetAt(uj, NULL);
        }
    }
}
//---------------------------------------------------------------------------
void NiAdditionalGeometryData::LinkObject(NiStream& kStream)
{
    NiObject::LinkObject(kStream);
}
//---------------------------------------------------------------------------
bool NiAdditionalGeometryData::RegisterStreamables(NiStream& kStream)
{
    return NiObject::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiAdditionalGeometryData::SaveBinary(NiStream& kStream)
{
    NiObject::SaveBinary(kStream);
    NiStreamSaveBinary(kStream, m_usVertexCount);
    NiStreamSaveBinary(kStream, m_uiDataStreamCount);
    for (unsigned int ui = 0; ui < m_uiDataStreamCount; ui++)
        m_pkDataStreamEntries[ui].SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_aDataBlocks.GetSize());
    for (unsigned int uj = 0; uj < m_aDataBlocks.GetSize(); uj++)
    {
        NiAGDDataBlock* pkBlock = m_aDataBlocks.GetAt(uj);
        NiBool bValid = pkBlock != NULL;
        NiStreamSaveBinary(kStream, bValid);
        if (bValid)
        {
            pkBlock->SaveBinary(kStream, uj, m_pkDataStreamEntries,
                m_uiDataStreamCount, m_usVertexCount);
        }
    }
}
//---------------------------------------------------------------------------
bool NiAdditionalGeometryData::IsEqual(NiObject* pkObject)
{
    if (!NiObject::IsEqual(pkObject))
        return false;

    NiAdditionalGeometryData* pkData = (NiAdditionalGeometryData*) pkObject;

    if (m_usVertexCount != pkData->m_usVertexCount ||
        m_uiDataStreamCount != pkData->m_uiDataStreamCount)
    {
        return false;
    }

    for (unsigned int ui = 0; ui < m_uiDataStreamCount; ui++)
    {
        if (!m_pkDataStreamEntries[ui].IsEqual(
            &pkData->m_pkDataStreamEntries[ui]))
        {
            return false;
        }
    }

    for (unsigned int uj = 0; uj < m_aDataBlocks.GetSize(); uj++)
    {
        NiAGDDataBlock* pkBlock = m_aDataBlocks.GetAt(uj);
        NiAGDDataBlock* pkOtherBlock = pkData->m_aDataBlocks.GetAt(uj);

        if (!pkOtherBlock && pkBlock || pkOtherBlock && !pkBlock)
            return false;

        if (pkBlock && !pkBlock->IsEqual(pkOtherBlock))
            return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiAdditionalGeometryData::GetViewerStrings(NiViewerStringsArray* 
    pkStrings)
{
    NiObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(
        NiAdditionalGeometryData::ms_RTTI.GetName()));

    pkStrings->Add(NiGetViewerString("m_usVertexCount", m_usVertexCount));
    pkStrings->Add(NiGetViewerString("m_uiDataStreamCount", 
        m_uiDataStreamCount));
    
    for (unsigned int ui = 0; ui < m_uiDataStreamCount; ui++)
    {
        pkStrings->Add(NiGetViewerString("    DataStream Index", 
            ui));
        m_pkDataStreamEntries[ui].GetViewerStrings(pkStrings);
    }

    pkStrings->Add(NiGetViewerString("m_aDataBlocks.GetSize()", 
        m_aDataBlocks.GetSize()));

    for (unsigned int uj = 0; uj < m_aDataBlocks.GetSize(); uj++)
    {
        pkStrings->Add(NiGetViewerString("    DataBlock Index", 
            uj));

        NiAGDDataBlock* pkBlock = m_aDataBlocks.GetAt(uj);
        if (pkBlock)
        {
            pkBlock->GetViewerStrings(pkStrings);
        }
        else
        {
            pkStrings->Add(NiGetViewerString("        m_uiDataBlockSize", 
                0));
            pkStrings->Add(NiGetViewerString("        m_pucDataBlock", 
                "NULL"));
        }
    }
}
//---------------------------------------------------------------------------
