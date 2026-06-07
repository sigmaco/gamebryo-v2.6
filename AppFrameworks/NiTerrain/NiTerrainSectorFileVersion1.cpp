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

NiTerrainSectorFileVersion1::NiTerrainSectorFileVersion1(
    const char* kSectorFile,
    NiFile::OpenMode kAccessMode): NiTerrainSectorFile(kSectorFile, kAccessMode)
{
}
//---------------------------------------------------------------------------
NiTerrainSectorFileVersion1::~NiTerrainSectorFileVersion1()
{
    NiFree(m_puiLODOffsets);
    NiFree(m_puiLookUpTable);
}
//---------------------------------------------------------------------------
bool NiTerrainSectorFileVersion1::Initialize()
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
    m_uiNumBlocks = 0;
    m_kPositionStack.RemoveAll();
    m_auiChildOffsetsX[0] = 0;
    m_auiChildOffsetsX[1] = 1;
    m_auiChildOffsetsX[2] = 1;
    m_auiChildOffsetsX[3] = 0;
    m_auiChildOffsetsY[0] = 0;
    m_auiChildOffsetsY[1] = 0;
    m_auiChildOffsetsY[2] = 1;
    m_auiChildOffsetsY[3] = 1;

    // If we are reading from the file, then begin setting up:
    if (m_kAccessMode == NiFile::READ_ONLY)
    {
        // Read the file header
        m_kFileHeader.LoadBinary(*m_pkFile);
        m_ulFilePosition += sizeof(m_kFileHeader);

        // Parse the data to find out the number of blocks:       
        while (m_ulFilePosition < m_pkFile->GetFileSize())
        {
            OldNextBlock();
            m_uiNumBlocks++;
        }

        // Work out the number of LoD
        NiUInt32 uiLoDAddition = 0;
        m_uiNumLOD = 1;
        while (uiLoDAddition < m_uiNumBlocks - 1)
        {
            uiLoDAddition += 1 << (m_uiNumLOD * 2);
            ++m_uiNumLOD;
        }

        CreateHierarchyLookUpTable(0);

        // Reinitialise the file to the start
        m_ulFilePosition = 0;
        m_iCurrentBlockID = -1;
        m_iCurrentBlockLevel = 0;
        m_iCurrentBlockLevelIndex = 0;
        m_kPositionStack.RemoveAll();
        m_ulFilePosition += sizeof(m_kFileHeader);

        return NextBlock();
    }
    else
    {
        // We are not allowed to write this version of the file
        return false;
    }
}
//---------------------------------------------------------------------------
void NiTerrainSectorFileVersion1::CreateHierarchyLookUpTable(
    NiUInt32 uiNewBlockID)
{
    m_puiLODOffsets = NiAlloc(NiUInt32, m_uiNumLOD);
    m_puiLookUpTable = NiAlloc(NiUInt32, m_uiNumBlocks);

    //Initialize the lod offset table
    NiUInt32 uiPreviousOffset = 0;
    for (NiInt32 i = m_uiNumLOD - 1; i >= 0; --i)
    {
        m_puiLODOffsets[i] = uiPreviousOffset;
        uiPreviousOffset += 1 << (i * 2);
    }

    AddBlockToLookUp(uiNewBlockID, 0, 0, 0);
}
//---------------------------------------------------------------------------
void NiTerrainSectorFileVersion1::AddBlockToLookUp(
    NiUInt32 uiNewBlockID, 
    NiUInt32 uiCurrentLOD, 
    NiUInt32 uiBlockX, 
    NiUInt32 uiBlockY)
{
    // Add the block to the look up table
    m_puiLookUpTable[uiNewBlockID] = (m_uiNumBlocks - 1) - (uiBlockX + 
        uiBlockY * (1 << uiCurrentLOD) + m_puiLODOffsets[uiCurrentLOD]);

    ++uiCurrentLOD;

    // Parse the children of this block if there are any
    if (uiCurrentLOD < m_uiNumLOD)
    {
        for (NiUInt32 i = 0; i < 4; ++i)
        {
            NiUInt32 uiChildID = uiNewBlockID * 4 + 1 + i;
            NiUInt32 uiChildBlockX = uiBlockX * 2 + m_auiChildOffsetsX[i];
            NiUInt32 uiChildBlockY = uiBlockY * 2 + m_auiChildOffsetsY[i];

            AddBlockToLookUp(uiChildID, uiCurrentLOD, 
                uiChildBlockX, uiChildBlockY);
        }
    }
}
//---------------------------------------------------------------------------
void NiTerrainSectorFileVersion1::ReadFromBlockIDConverted(NiUInt32 uiBlockID)
{
    // Convert block id to index into the file
    NiUInt32 uiFileIndex = m_puiLookUpTable[uiBlockID];
    m_iCurrentBlockID = -1;
    m_iCurrentBlockLevel = 0;
    m_iCurrentBlockLevelIndex = 0;
    
    // We now parse the file to find the appropriate position in the file
    m_ulFilePosition = 0;
    m_ulFilePosition += sizeof(m_kFileHeader);
    m_pkFile->Seek(m_ulFilePosition, NiFile::ms_iSeekSet);
    for (NiUInt32 i = 0; i <= uiFileIndex; ++i)
        OldNextBlock();
    
    m_iCurrentBlockID = uiBlockID;
}
//---------------------------------------------------------------------------
bool NiTerrainSectorFileVersion1::NextBlock()
{
    NiInt32 iIndex = m_iCurrentBlockID + 1;
    ReadFromBlockIDConverted(iIndex);
   
        
    return true;
}
//---------------------------------------------------------------------------
bool NiTerrainSectorFileVersion1::PushBlock(NiInt32 iChildID)
{
    NiInt32 iIndex = m_iCurrentBlockID * 4 + 1 + iChildID;
    ReadFromBlockIDConverted(iIndex);

    return true;
}
//---------------------------------------------------------------------------
bool NiTerrainSectorFileVersion1::PopBlock()
{
    NiInt32 iIndex = (m_iCurrentBlockID - 1) / 4;
    ReadFromBlockIDConverted(iIndex);

    return true;
}  
//---------------------------------------------------------------------------
bool NiTerrainSectorFileVersion1::OldNextBlock()
{
    return NiTerrainSectorFile::NextBlock();
}
//---------------------------------------------------------------------------
bool NiTerrainSectorFileVersion1::OldPopBlock()
{
    return NiTerrainSectorFile::PopBlock();
}
//---------------------------------------------------------------------------
bool NiTerrainSectorFileVersion1::OldPushBlock(NiUInt32 uiLOD)
{
    return NiTerrainSectorFile::PushBlock(uiLOD);
}
//---------------------------------------------------------------------------
