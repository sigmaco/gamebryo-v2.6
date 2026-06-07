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

#include "NiTerrainPCH.h"
#include "NiTerrainSectorFile.h"
#include "NiTerrainSectorFileVersion1.h"

#include <NiFile.h>
#include <NiBound.h>

//---------------------------------------------------------------------------
NiTerrainSectorFile *NiTerrainSectorFile::Open(const char* kInSectorFile,
    bool bWriteAccess)
{
    NiTerrainSectorFile *pkResult = NULL;

    // Check the filename:
    if (!kInSectorFile)
        return NULL;

    // Allocate a temporary buffer for "Standardizing" the file path.
    size_t stLen = strlen(kInSectorFile);
    char* pcNonStandardSectorFile = NiStackAlloc(char, stLen+1);

    // Standardize the file path.
    NiMemcpy(pcNonStandardSectorFile, kInSectorFile, stLen+1);
    NiPath::Standardize(pcNonStandardSectorFile);

    // Set the Standardized file path and delete the temporary buffer.
    NiString kSectorFile(pcNonStandardSectorFile);
    NiStackFree(pcNonStandardSectorFile);
    
    // Decide upon the access mode:
    NiFile::OpenMode kAccessMode;
    if (bWriteAccess)    
        kAccessMode = NiFile::WRITE_ONLY;    
    else    
        kAccessMode = NiFile::READ_ONLY;    
    
    // If we are unable to access the file, return
    if (!NiFile::Access(kSectorFile, kAccessMode))
        return NULL;

    bool bPlatformLittle = NiSystemDesc::GetSystemDesc().IsLittleEndian();
    if (kAccessMode == NiFile::READ_ONLY)
    {
        // Attempt to open the file:
        NiFile *pkFile = NiFile::GetFile(kSectorFile, NiFile::READ_ONLY);
        if (!(*pkFile))
        {
            NiDelete pkFile;
            return NULL;
        }
        
        pkFile->SetEndianSwap(!bPlatformLittle);

        // Read the file header so we can inspect the version.
        FileHeader kFileHeader;
        kFileHeader.LoadBinary(*pkFile);

        // Close the file
        NiDelete pkFile;

        // Instantiate an appropriate reader for this type of file
        switch(kFileHeader.m_kVersion)
        {
        case ms_kFileVersion1:
            pkResult = 
                NiNew NiTerrainSectorFileVersion1(kSectorFile, kAccessMode);
            break;
        case ms_kFileVersion:
            pkResult = NiNew NiTerrainSectorFile(kSectorFile, kAccessMode);
            break;

        default:
            break;
        }
    }
    else
    {
        // Instantiate the latest sector parser to do the writing!
        pkResult = NiNew NiTerrainSectorFile(kSectorFile, kAccessMode);        
    }

    // Make sure we managed to load an appropriate parser
    if (pkResult == NULL)    
        return pkResult;

    // Make sure the parser initializes ok!
    if (!pkResult->Initialize())
    {
        // The parser failed to initialize properly!
        NiDelete pkResult;
        pkResult = NULL;
    }

    return pkResult;
}
//---------------------------------------------------------------------------
NiTerrainSectorFile::FileVersion NiTerrainSectorFile::GetFileVersion() const
{
    return m_kFileHeader.m_kVersion;
}
//---------------------------------------------------------------------------
bool NiTerrainSectorFile::IsReady() const
{
    return m_pkFile != 0;
}
//---------------------------------------------------------------------------
bool NiTerrainSectorFile::IsWritable() const
{
    return m_kAccessMode == NiFile::WRITE_ONLY;
}
//---------------------------------------------------------------------------
void NiTerrainSectorFile::SetBlockWidthInVerts(NiUInt32 uiVertsPerBlock)
{
    m_kFileHeader.m_uiVertsPerBlock = uiVertsPerBlock;
}
//---------------------------------------------------------------------------
void NiTerrainSectorFile::WriteFileHeader()
{
    if(!IsWritable() || !IsReady())
        return;

    // Write the file header to the file:
    m_kFileHeader.SaveBinary(*m_pkFile);
    m_ulFilePosition += sizeof(m_kFileHeader);
}
//---------------------------------------------------------------------------
void NiTerrainSectorFile::WriteBlock()
{
    if(!IsWritable() || !IsReady())
        return;

    // Update the identity information on this block!
    m_iCurrentBlockID++;
    if (m_iCurrentBlockID >= (4*m_iCurrentBlockLevelIndex + 1))
    {
        m_iCurrentBlockLevelIndex = m_iCurrentBlockID;
        m_iCurrentBlockLevel++;
    }

    // Update this block's parent's header to point to this set of children
    if ((m_iCurrentBlockID % 4) - 1 == 0)
    {
        NiUInt32 uiParentPosition = m_kPositionStack.RemoveHead();
        NiInt32 iChildOffsetOffset = (NiInt32)(
            (char*)&(m_kCurrentBlockHeader.m_ulChildOffset) - 
            (char*)&(m_kCurrentBlockHeader)); 

        // Seek to the parent and write the new child's address:
        m_pkFile->Seek(uiParentPosition, NiFile::ms_iSeekSet);
        m_pkFile->Seek(iChildOffsetOffset, NiFile::ms_iSeekCur);

        // Write the new child's address:
        NiStreamSaveBinary(*m_pkFile, m_ulFilePosition);

        // Seek back to the current block's position:
        m_pkFile->Seek(m_ulFilePosition, NiFile::ms_iSeekSet);      
    }

    // Add the position of this block to the list so it's 
    // children offset can be set later:
    m_kPositionStack.AddTail(m_ulFilePosition);

    // Finalize the information in this block's header:
    m_kCurrentBlockHeader.m_ulLength = sizeof(m_kCurrentBlockHeader);
    m_kCurrentBlockHeader.m_kPresentData.m_uiBitField = 0;
    NiUInt32 kStreamID;
    for(kStreamID = 0; kStreamID < STREAM_MAX_NUMBER; ++kStreamID)
    {
        m_kCurrentBlockHeader.m_ulLength += 
            m_akStreamHeader[kStreamID].m_ulLength;

        // Check if this stream is present:
        if(m_akStreamHeader[kStreamID].m_ulLength != 0)
        {
            m_kCurrentBlockHeader.m_kPresentData.m_uiBitField |= 
                (1 << kStreamID);
            m_kCurrentBlockHeader.m_ulLength += sizeof(DataStreamHeader);
        }
    }

    // Write the block to the file:
    // First the header:
    m_kCurrentBlockHeader.SaveBinary(*m_pkFile);
    m_ulFilePosition += sizeof(m_kCurrentBlockHeader);

    // Now write all the streams:
    for(kStreamID = 0; kStreamID < STREAM_MAX_NUMBER; ++kStreamID)
    {
        if(m_akStreamHeader[kStreamID].m_ulLength == 0)
        {
            continue;
        }

        // Write this stream to the file:
        m_akStreamHeader[kStreamID].SaveBinary(*m_pkFile);
        m_ulFilePosition += sizeof(DataStreamHeader);

        // Write the data to the file:
        // Currently, element size is assumed to be sizeof(float) for
        // endian swapping purposes.
        NiStreamSaveBinary(*m_pkFile, (float*)m_apcStreamData[kStreamID], 
            m_akStreamHeader[kStreamID].m_ulLength / sizeof(float));
        m_ulFilePosition += m_akStreamHeader[kStreamID].m_ulLength;

        // Reset this stream's info as it has now been written:
        m_akStreamHeader[kStreamID].m_ulLength = 0;
        m_apcStreamData[kStreamID] = 0;
    }

    // Reset this stream's information:
    m_kCurrentBlockHeader.m_ulChildOffset = 0;
    m_kCurrentBlockHeader.m_kPresentData.m_uiBitField = 0;
    m_kCurrentBlockHeader.m_ulLength = 0;
}
//---------------------------------------------------------------------------
NiUInt32 NiTerrainSectorFile::GetBlockWidthInVerts() const
{
    if(!IsReady()) 
    {
        return 0;
    }

    return m_kFileHeader.m_uiVertsPerBlock;
}
//---------------------------------------------------------------------------
bool NiTerrainSectorFile::NextBlock()
{
    if (!IsReady() || IsWritable())     
        return false;

    // Pop the last position on the stack as we are no longer there:
    if (m_kPositionStack.GetSize() != 0)
    {
        m_kPositionStack.RemoveTail();
    }
    // Set the current position on the stack
    m_kPositionStack.AddTail(m_ulFilePosition);

    // Reset the stream information from the previous block (keep the buffers)
    // Read in all of this block's data though!
    NiUInt32 uiStreamNumber;
    for(uiStreamNumber = 0; 
        uiStreamNumber < STREAM_MAX_NUMBER; 
        ++uiStreamNumber)
    {
        m_akStreamHeader[uiStreamNumber].m_kStreamType = STREAM_INVALID;
    }

    // Update the identity information on this block!
    m_iCurrentBlockID++;
    if (m_iCurrentBlockID >= (4*m_iCurrentBlockLevelIndex + 1))
    {
        m_iCurrentBlockLevelIndex = m_iCurrentBlockID;
        m_iCurrentBlockLevel++;
    }

    // Read in the block header:
    // No need to seek, we are already at the next block!
    m_kCurrentBlockHeader.LoadBinary(*m_pkFile);
    m_ulFilePosition += sizeof(m_kCurrentBlockHeader);

    // Read in all the available streams:
    for (uiStreamNumber = 0;
        uiStreamNumber < STREAM_MAX_NUMBER;
        ++uiStreamNumber)
    {
        if (!(m_kCurrentBlockHeader.m_kPresentData.m_uiBitField &
            (1 << uiStreamNumber)))
        {
            // This stream is not present
            continue;
        }

        // Read in the stream's header:
        DataStreamHeader kCurrentStreamHeader;
        kCurrentStreamHeader.LoadBinary(*m_pkFile);
        m_ulFilePosition += sizeof(kCurrentStreamHeader);

        // Figure out which stream this is:
        uiStreamNumber = kCurrentStreamHeader.m_kStreamType;
        if(uiStreamNumber > STREAM_MAX_NUMBER)
        {
            // Invalid stream, something wrong with the file
            return false;
        }

        // Make sure the buffer is large enough for this stream:
        if(m_akStreamHeader[uiStreamNumber].m_ulLength != 
            kCurrentStreamHeader.m_ulLength)
        {
            NiFree(m_apcStreamData[uiStreamNumber]);

            m_apcStreamData[uiStreamNumber] = 
                NiAlloc(char, kCurrentStreamHeader.m_ulLength);
        }

        // Assign this stream's header to the correct position
        m_akStreamHeader[uiStreamNumber] = 
            kCurrentStreamHeader;

        // Read in the stream's data
        // Currently, element size is assumed to be sizeof(float) for
        // endian swapping purposes.
        NiStreamLoadBinary(*m_pkFile, (float*)m_apcStreamData[uiStreamNumber], 
            kCurrentStreamHeader.m_ulLength / sizeof(float));

        m_ulFilePosition += kCurrentStreamHeader.m_ulLength;
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiTerrainSectorFile::PushBlock(int iChildID)
{
    if (!IsReady() || IsWritable())
    {
        return false;
    }

    // Check the bounds of the parameter:
    if (iChildID < 0 || iChildID > 3)
    {
        return false;
    }

    // Figure out where the children start
    NiUInt32 ulSeekOffset = m_kCurrentBlockHeader.m_ulChildOffset;
    if (ulSeekOffset == 0)
    {
        // No children for this block!
        return false;
    }

    // Update the identity information on this block!
    m_iCurrentBlockLevelIndex = 4 * m_iCurrentBlockLevelIndex + 1;
    m_iCurrentBlockLevel++;
    m_iCurrentBlockID = 4*m_iCurrentBlockID; // (add 1 in NextBlock)
    
    // Seek to the first child block:
    m_pkFile->Seek(ulSeekOffset, NiFile::ms_iSeekSet);
    m_ulFilePosition = ulSeekOffset;

    // Seek to the requested child:
    for (NiUInt32 uiChild = 0; uiChild < (NiUInt32)iChildID; ++uiChild)
    {
        // read this child's header in (to see how far we need to skip)
        m_kCurrentBlockHeader.LoadBinary(*m_pkFile);
        m_ulFilePosition += sizeof(m_kCurrentBlockHeader);

        // Seek past this block to get to its sibling:
        ulSeekOffset = 
            m_kCurrentBlockHeader.m_ulLength - sizeof(m_kCurrentBlockHeader);
        m_pkFile->Seek(ulSeekOffset, NiFile::ms_iSeekCur);
        m_ulFilePosition += ulSeekOffset;

        m_iCurrentBlockID++;
    }

    // Put this block's position on the stack:
    m_kPositionStack.AddTail(m_ulFilePosition);

    // Read in this block's details:
    bool bResult = NextBlock();
    if (!bResult)
    {
        PopBlock();
        return false;
    }
    
    return true;
}
//---------------------------------------------------------------------------
bool NiTerrainSectorFile::PopBlock()
{
    // Check that we haven't reached the root of the tree:
    if (m_kPositionStack.GetSize() == 1)
    {
        return false;
    }
    
    // Update the identity information on this block!
    m_iCurrentBlockLevelIndex = (m_iCurrentBlockLevelIndex - 1) / 4;
    m_iCurrentBlockLevel--;
    m_iCurrentBlockID = (m_iCurrentBlockID - 1) / 4 - 1;

    // The lowest item on the stack is where we currently are. 
    // Pop that off and seek to where the previous item specifies:
    m_kPositionStack.RemoveTail();

    // Seek to the previous item: (parent)
    NiUInt32 ulSeekPosition = m_kPositionStack.GetTail();
    m_pkFile->Seek(ulSeekPosition, NiFile::ms_iSeekSet);
    m_ulFilePosition = ulSeekPosition;

    // Read in the parent block's data:
    return NextBlock();
}
//---------------------------------------------------------------------------
NiBound NiTerrainSectorFile::GetBlockBound() const
{
    NiBound result;
    result.SetCenter(m_kCurrentBlockHeader.m_kBoundCenter);
    result.SetRadius(m_kCurrentBlockHeader.m_fBoundRadius);

    return result;
}
//---------------------------------------------------------------------------
NiPoint3 NiTerrainSectorFile::GetBlockSumVerts() const
{
    return m_kCurrentBlockHeader.m_kBoundSumVerts;
}
//---------------------------------------------------------------------------
NiBoxBV NiTerrainSectorFile::GetBlockBoundVolume() const
{
    NiBoxBV result;
    result.SetCenter(m_kCurrentBlockHeader.m_kVolumeCenter);
    result.SetAxis(0, m_kCurrentBlockHeader.m_kVolumeDirection1);
    result.SetAxis(1, m_kCurrentBlockHeader.m_kVolumeDirection2);
    result.SetAxis(2, m_kCurrentBlockHeader.m_kVolumeDirection3);
    result.SetExtent(0, m_kCurrentBlockHeader.m_kVolumeExtent1);
    result.SetExtent(1, m_kCurrentBlockHeader.m_kVolumeExtent2);
    result.SetExtent(2, m_kCurrentBlockHeader.m_kVolumeExtent3);

    return result;
}
//---------------------------------------------------------------------------
void NiTerrainSectorFile::SetBlockBounds(NiBound kBound, 
    NiPoint3 kSumVerts, NiBoxBV kVolume)
{
    // Bound Information:    
    m_kCurrentBlockHeader.m_kBoundCenter = kBound.GetCenter();
    m_kCurrentBlockHeader.m_fBoundRadius = kBound.GetRadius();
    m_kCurrentBlockHeader.m_kBoundSumVerts = kSumVerts;

    // Bounding Volume Information:
    m_kCurrentBlockHeader.m_kVolumeCenter = kVolume.GetCenter();
    m_kCurrentBlockHeader.m_kVolumeDirection1 = kVolume.GetAxis(0);
    m_kCurrentBlockHeader.m_kVolumeDirection2 = kVolume.GetAxis(1);
    m_kCurrentBlockHeader.m_kVolumeDirection3 = kVolume.GetAxis(2);
    m_kCurrentBlockHeader.m_kVolumeExtent1 = kVolume.GetExtent(0);
    m_kCurrentBlockHeader.m_kVolumeExtent2 = kVolume.GetExtent(1);
    m_kCurrentBlockHeader.m_kVolumeExtent3 = kVolume.GetExtent(2);

}
//---------------------------------------------------------------------------
void NiTerrainSectorFile::SetStreamData(StreamType kStreamID, 
    NiUInt32 uiObjectSize, float* pfData, NiUInt32 uiDataLength)
{
    // Imperfect sanity check of assumption that
    // element size is sizeof(float). This assumption is used for 
    // endian swapping stream data.
    NIASSERT(uiDataLength % sizeof(float) == 0);

    if (!IsReady() || !IsWritable()) 
    {
        return;
    }

    m_akStreamHeader[kStreamID].m_kStreamType = kStreamID;
    m_akStreamHeader[kStreamID].m_ulLength = uiDataLength;
    m_akStreamHeader[kStreamID].m_uiObjectSize = uiObjectSize;

    m_apcStreamData[kStreamID] = (char*)pfData;
}
//---------------------------------------------------------------------------
float* NiTerrainSectorFile::GetStreamData(StreamType kStreamID,
    NiUInt32& uiDataLength)
{
    if (!IsReady() || IsWritable()) 
    {
        return 0;
    }

    if (m_akStreamHeader[kStreamID].m_kStreamType == kStreamID)
    {
        uiDataLength = m_akStreamHeader[kStreamID].m_ulLength;
        return (float*)m_apcStreamData[kStreamID];
    }
    else
    {
        return 0;
    }
}
//---------------------------------------------------------------------------
NiTerrainSectorFile::NiTerrainSectorFile(
    const char* kSectorFile,
    NiFile::OpenMode kAccessMode):
    m_pkFile(0),
    m_ulFilePosition(0),
    m_kSectorFile(0),
    m_iCurrentBlockID(0),
    m_iCurrentBlockLevel(0)
{
    // Allocate a temporary buffer for "Standardizing" the file path.
    size_t stLen = strlen(kSectorFile);
    char* pcNonStandardSectorFile = NiStackAlloc(char, stLen+1);

    // Standardize the file path.
    NiMemcpy(pcNonStandardSectorFile, kSectorFile, stLen+1);
    NiPath::Standardize(pcNonStandardSectorFile);
    
    m_kSectorFile = pcNonStandardSectorFile;
    m_kAccessMode = kAccessMode;

    // Free the temporary buffer.
    NiStackFree(pcNonStandardSectorFile);

    // Initialize all the stream pointers to 0
    NiUInt32 uiStreamID;
    for (uiStreamID = 0; uiStreamID < STREAM_MAX_NUMBER; ++uiStreamID)
    {
        m_apcStreamData[uiStreamID] = 0;
        m_akStreamHeader[uiStreamID].m_ulLength = 0;
    }
}
//---------------------------------------------------------------------------
NiTerrainSectorFile::~NiTerrainSectorFile()
{
    NiDelete m_pkFile;

    // Release the stream buffers:
    if (!IsWritable())
    {
        NiUInt32 uiStreamID;
        for (uiStreamID = 0; uiStreamID < STREAM_MAX_NUMBER; ++uiStreamID)
        {
            if (m_apcStreamData[uiStreamID])
            {
                NiFree(m_apcStreamData[uiStreamID]);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiTerrainSectorFile::Initialize()
{
    // Attempt to gain access to the file in this mode!
    m_pkFile = NiFile::GetFile(m_kSectorFile, m_kAccessMode);

    bool bPlatformLittle = NiSystemDesc::GetSystemDesc().IsLittleEndian();
    m_pkFile->SetEndianSwap(!bPlatformLittle);

    // Return null if it was not possible
    if (!m_pkFile)
    {
        return false;
    }

    // Initialize the variables:
    m_ulFilePosition = 0;
    m_iCurrentBlockID = -1;
    m_iCurrentBlockLevel = 0;
    m_iCurrentBlockLevelIndex = 0;
    m_kPositionStack.RemoveAll();

    // If we are reading from the file, then begin setting up:
    if (m_kAccessMode == NiFile::READ_ONLY)
    {
        // Read the file header
        m_kFileHeader.LoadBinary(*m_pkFile);
        m_ulFilePosition += sizeof(m_kFileHeader);

        // Read the first block's data:
        return NextBlock();
    }
    else
    {
        m_kFileHeader.m_kVersion = ms_kFileVersion;
        m_kFileHeader.m_uiVertsPerBlock = 32; // Default
    }

    return true;
}
//---------------------------------------------------------------------------
void NiTerrainSectorFile::FileHeader::LoadBinary(NiBinaryStream& kStream)
{
    NiStreamLoadBinary(kStream, m_kVersion);
    NiStreamLoadBinary(kStream, m_uiVertsPerBlock);
}
//---------------------------------------------------------------------------
void NiTerrainSectorFile::FileHeader::SaveBinary(NiBinaryStream& kStream)
{
    NiStreamSaveBinary(kStream, m_kVersion);
    NiStreamSaveBinary(kStream, m_uiVertsPerBlock);
}
//---------------------------------------------------------------------------
void NiTerrainSectorFile::BlockHeader::LoadBinary(NiBinaryStream& kStream)
{
    NiStreamLoadBinary(kStream, m_ulLength);
    NiStreamLoadBinary(kStream, m_ulChildOffset);
    NiStreamLoadBinary(kStream, m_kPresentData);

    NiStreamLoadBinary(kStream, m_fBoundRadius);
    
    NiStreamLoadBinary(kStream, m_kBoundCenter.x);
    NiStreamLoadBinary(kStream, m_kBoundCenter.y);
    NiStreamLoadBinary(kStream, m_kBoundCenter.z);
    NiStreamLoadBinary(kStream, m_kBoundSumVerts.x);
    NiStreamLoadBinary(kStream, m_kBoundSumVerts.y);
    NiStreamLoadBinary(kStream, m_kBoundSumVerts.z);

    NiStreamLoadBinary(kStream, m_kVolumeCenter.x);
    NiStreamLoadBinary(kStream, m_kVolumeCenter.y);
    NiStreamLoadBinary(kStream, m_kVolumeCenter.z);
    NiStreamLoadBinary(kStream, m_kVolumeDirection1.x);
    NiStreamLoadBinary(kStream, m_kVolumeDirection1.y);
    NiStreamLoadBinary(kStream, m_kVolumeDirection1.z);
    NiStreamLoadBinary(kStream, m_kVolumeDirection2.x);
    NiStreamLoadBinary(kStream, m_kVolumeDirection2.y);
    NiStreamLoadBinary(kStream, m_kVolumeDirection2.z);
    NiStreamLoadBinary(kStream, m_kVolumeDirection3.x);
    NiStreamLoadBinary(kStream, m_kVolumeDirection3.y);
    NiStreamLoadBinary(kStream, m_kVolumeDirection3.z);

    NiStreamLoadBinary(kStream, m_kVolumeExtent1);
    NiStreamLoadBinary(kStream, m_kVolumeExtent2);
    NiStreamLoadBinary(kStream, m_kVolumeExtent3);
}
//---------------------------------------------------------------------------
void NiTerrainSectorFile::BlockHeader::SaveBinary(NiBinaryStream& kStream)
{
    NiStreamSaveBinary(kStream, m_ulLength);
    NiStreamSaveBinary(kStream, m_ulChildOffset);
    NiStreamSaveBinary(kStream, m_kPresentData);

    NiStreamSaveBinary(kStream, m_fBoundRadius);

    NiStreamSaveBinary(kStream, m_kBoundCenter.x);
    NiStreamSaveBinary(kStream, m_kBoundCenter.y);
    NiStreamSaveBinary(kStream, m_kBoundCenter.z);
    NiStreamSaveBinary(kStream, m_kBoundSumVerts.x);
    NiStreamSaveBinary(kStream, m_kBoundSumVerts.y);
    NiStreamSaveBinary(kStream, m_kBoundSumVerts.z);

    NiStreamSaveBinary(kStream, m_kVolumeCenter.x);
    NiStreamSaveBinary(kStream, m_kVolumeCenter.y);
    NiStreamSaveBinary(kStream, m_kVolumeCenter.z);
    NiStreamSaveBinary(kStream, m_kVolumeDirection1.x);
    NiStreamSaveBinary(kStream, m_kVolumeDirection1.y);
    NiStreamSaveBinary(kStream, m_kVolumeDirection1.z);
    NiStreamSaveBinary(kStream, m_kVolumeDirection2.x);
    NiStreamSaveBinary(kStream, m_kVolumeDirection2.y);
    NiStreamSaveBinary(kStream, m_kVolumeDirection2.z);
    NiStreamSaveBinary(kStream, m_kVolumeDirection3.x);
    NiStreamSaveBinary(kStream, m_kVolumeDirection3.y);
    NiStreamSaveBinary(kStream, m_kVolumeDirection3.z);

    NiStreamSaveBinary(kStream, m_kVolumeExtent1);
    NiStreamSaveBinary(kStream, m_kVolumeExtent2);
    NiStreamSaveBinary(kStream, m_kVolumeExtent3);
}
//---------------------------------------------------------------------------
void NiTerrainSectorFile::DataStreamHeader::LoadBinary(NiBinaryStream& kStream)
{
    NiStreamLoadBinary(kStream, m_ulLength);
    NiStreamLoadBinary(kStream, m_kStreamType);
    NiStreamLoadBinary(kStream, m_uiObjectSize);
}
//---------------------------------------------------------------------------
void NiTerrainSectorFile::DataStreamHeader::SaveBinary(NiBinaryStream& kStream)
{
    NiStreamSaveBinary(kStream, m_ulLength);
    NiStreamSaveBinary(kStream, m_kStreamType);
    NiStreamSaveBinary(kStream, m_uiObjectSize);
}
//---------------------------------------------------------------------------
NiTerrainSectorFile::FileVersion NiTerrainSectorFile::GetCurrentVersion()
{
    return ms_kFileVersion;
}
//---------------------------------------------------------------------------
