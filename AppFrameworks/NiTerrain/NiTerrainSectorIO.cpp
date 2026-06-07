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

#include <NiTPointerList.h>
#include <NiDOMTool.h>
#include <NiImageConverter.h>
#include <NiFilename.h>

#include "NiTerrainSector.h"
#include "NiSurfaceMask.h"
#include "NiTerrainSectorFile.h"

//---------------------------------------------------------------------------
bool NiTerrainSector::Save(const char* pcArchive, NiUInt32* puiErrorCode)
{    
    NiOutputDebugString("- - - - - - - - - - - - - - - - - - -\n");

    bool bRes = true;
    bRes &= SaveToFile(pcArchive, puiErrorCode);
    bRes &= SaveSurfaceMasks(pcArchive, puiErrorCode);
    
    NiOutputDebugString("- - - - - - - - - - - - - - - - - - -\n");

    return bRes;
}
//---------------------------------------------------------------------------
bool NiTerrainSector::SaveToFile(const char* pcArchive, NiUInt32* puiErrorCode)
{
    NiUInt32 uiDummyErrorCode = 0;
    if (puiErrorCode == 0)
        puiErrorCode = &uiDummyErrorCode;

    // Save to the sector's own file format!
    // --- Generate sector's path/filename  

    NiString kSectorPath = GetSectorPath(pcArchive);
    NiString kFileName = NiFixedString(kSectorPath +
        NiString("\\quadtree.dof"));

    if (!NiFile::DirectoryExists(kSectorPath))
    {
        NiFile::CreateDirectoryRecursive(kSectorPath);
    }

    // --- Open the sector file:
    NiTerrainSectorFilePtr spSectorFile = 
        NiTerrainSectorFile::Open(kFileName, true);
    if (!spSectorFile)
    {
        *puiErrorCode |= EC_DOF_INVALID_FILE; 
        return false;
    }

    // Figure out the dimensions of the buffers:
    NiUInt32 uiWidthInVerts = m_pkSectorData->GetBlockWidthInVerts();
    NiUInt32 uiVertsPerBlock = uiWidthInVerts * uiWidthInVerts;

    // Iterators for each of the data streams:
    NiTerrainPositionRandomAccessIterator kHeights;
    NiTerrainNormalRandomAccessIterator kNormals;
    NiTerrainTangentRandomAccessIterator kTangents;

    // Write the header information:
    spSectorFile->SetBlockWidthInVerts(uiWidthInVerts);
    spSectorFile->WriteFileHeader();

    NiTerrainConfiguration kConfig = GetConfiguration();
    // Prepare some buffers for the data:
    float *pfHeightData = NiAlloc(float, uiVertsPerBlock * 1);
    float *pfNormalData = NiAlloc(float, uiVertsPerBlock * 2);

    float *pfTangentData = 0;
    if (kConfig.IsTangentDataEnabled())
        pfTangentData = NiAlloc(float, uiVertsPerBlock * 2);

    float *pfMorphHeightData = 0;
    float *pfMorphNormalData = 0;
    float *pfMorphTangentData = 0;

    if (kConfig.IsMorphingDataEnabled())
    {
        pfMorphHeightData = NiAlloc(float, uiVertsPerBlock * 1);
        pfMorphNormalData = NiAlloc(float, uiVertsPerBlock * 2);

        if (GetConfiguration().IsTangentDataEnabled())
            pfMorphTangentData = NiAlloc(float, uiVertsPerBlock * 2);
    }

    // Loop through the list of blocks (starting with lowest LOD)
    NiInt32 iNumLeaves = m_kLeafArray.GetSize();
    for (NiInt32 iLeafID = 0; iLeafID < iNumLeaves; ++iLeafID)
    {
        // Figure out which leaf we wish to retrieve:
        // Need to convert from QuadTree space to LOD space
        NiTerrainDataLeaf* pkLeaf = GetLeafByRegion(iLeafID);
        
        NiUInt32 uiMaxX = uiWidthInVerts;
        NiUInt32 uiMaxY = uiWidthInVerts;
        
        pkLeaf->GetPositionIterator(kHeights);
        pkLeaf->GetNormalIterator(kNormals);
        pkLeaf->GetTangentIterator(kTangents);

        for (NiUInt32 y = 0; y < uiMaxY; ++y)
        {
            for (NiUInt32 x = 0; x < uiMaxX; ++x)
            {         
                NiUInt32 uiIndex = y * uiMaxX + x;

                NiPoint3 kPosition;
                kHeights.GetHighDetail(uiIndex, kPosition);
                NiPoint2 kNormal;
                kNormals.GetHighDetail(uiIndex, kNormal);

                NiPoint2 kTangent;
                kTangent.x = 0.0f;
                kTangent.y = 0.0f;
                if (GetConfiguration().IsTangentDataEnabled())
                    kTangents.GetHighDetail(uiIndex, kTangent);

                // Get this leaf's height data:
                pfHeightData[uiIndex] = kPosition.z;

                // Get this leaf's normal data:
                pfNormalData[uiIndex * 2 + 0] = kNormal.x;
                pfNormalData[uiIndex * 2 + 1] = kNormal.y;

                // Get this leaf's tangent data:
                if (pfTangentData)
                {
                    pfTangentData[uiIndex * 2 + 0] = kTangent.x;
                    pfTangentData[uiIndex * 2 + 1] = kTangent.y;
                }

                // Get this leaf's morph height data:
                if (pfMorphHeightData)
                {
                    pfMorphHeightData[uiIndex] = kHeights.GetComponent(
                        uiIndex, NiTerrainPositionRandomAccessIterator::W);

                    // Get this leaf's morph normal data:
                    pfMorphNormalData[uiIndex * 2 + 0] = kNormals.GetComponent(
                        uiIndex, NiTerrainNormalRandomAccessIterator::Z);
                    pfMorphNormalData[uiIndex * 2 + 1] = kNormals.GetComponent(
                        uiIndex, NiTerrainNormalRandomAccessIterator::W);

                    // Get this leaf's morph tangent data:
                    if (pfMorphTangentData)
                    {
                        pfMorphTangentData[uiIndex * 2 + 0] = 
                            kTangents.GetComponent(
                            uiIndex, NiTerrainTangentRandomAccessIterator::Z);
                        pfMorphTangentData[uiIndex * 2 + 1] = 
                            kTangents.GetComponent(
                            uiIndex, NiTerrainTangentRandomAccessIterator::W);
                    }
                }
            }
        }

        // Push this block to the file!
        // Assign all the data streams:
        spSectorFile->SetHeightData(pfHeightData, 
            uiVertsPerBlock * sizeof(float));
        spSectorFile->SetNormalData(pfNormalData, 
            uiVertsPerBlock * 2 * sizeof(float));
        if (pfTangentData)
        {
            spSectorFile->SetTangentData(pfTangentData, 
                uiVertsPerBlock * 2 * sizeof(float));
        }
        if (pfMorphHeightData)
        {
            spSectorFile->SetMorphHeightData(pfMorphHeightData, 
                uiVertsPerBlock * 1 * sizeof(float));
        }
        if (pfMorphNormalData)
        {
            spSectorFile->SetMorphNormalData(pfMorphNormalData, 
                uiVertsPerBlock * 2 * sizeof(float));
        }
        if (pfMorphTangentData)
        {
            spSectorFile->SetMorphTangentData(pfMorphTangentData, 
                uiVertsPerBlock * 2 * sizeof(float));
        }

        // Set the block physics information
        spSectorFile->SetBlockBounds(
            pkLeaf->GetLocalBound(), 
            pkLeaf->GetSumOfVertexPositions(),
            pkLeaf->GetLocalBoxBound());

        spSectorFile->WriteBlock();
    }

    NiFree(pfHeightData);
    NiFree(pfNormalData);
    if (pfTangentData)
        NiFree(pfTangentData);
    if (pfMorphHeightData)
        NiFree(pfMorphHeightData);
    if (pfMorphNormalData)
        NiFree(pfMorphNormalData);
    if (pfMorphTangentData)
        NiFree(pfMorphTangentData);

    *puiErrorCode |= EC_DOF_SAVED; 
    return true;
}
//---------------------------------------------------------------------------
void NiTerrainSector::Unload()
{
    UnloadLODData();
    DestroySupportingData();
}
//---------------------------------------------------------------------------
bool NiTerrainSector::Load(const char* pcArchive, NiUInt32* puiErrorCode)
{
    return LoadLODData(NiTerrainUtils::ms_uiMAX_LOD, puiErrorCode, pcArchive);
}
//---------------------------------------------------------------------------
bool NiTerrainSector::LoadFromFile(const NiFixedString &kArchive, 
    NiUInt32 uiMaxDetailLevel, NiUInt32* puiErrorCode)
{
    NiUInt32 uiDummyErrorCode = 0;
    if (puiErrorCode == 0)
        puiErrorCode = &uiDummyErrorCode;

    NiFixedString kSectorPath = GetSectorPath(kArchive);
    NiFixedString kFileName = NiFixedString(kSectorPath +
        NiString("\\quadtree.dof"));

    // Attempt to open the file:
    NiTerrainSectorFilePtr spSectorFile = NiTerrainSectorFile::Open(kFileName);
    if(!spSectorFile)
    {
        *puiErrorCode |= NiTerrainSector::EC_DOF_INVALID_FILE;
        return false;
    }

    // Check if the file is of the latest version
    if (spSectorFile->GetFileVersion() != 
        NiTerrainSectorFile::GetCurrentVersion())
    {
        *puiErrorCode |= NiTerrainSector::EC_DOF_OUTOFDATE;
    }

    // Prevent any updates or draws while we are processing
    m_bUpdating = true;

    bool bResult = LoadFromFile(spSectorFile, uiMaxDetailLevel, puiErrorCode);

    m_bUpdating = false;

    return bResult;   
}
//---------------------------------------------------------------------------
bool NiTerrainSector::LoadFromFile(NiTerrainSectorFile *pkSectorFile,
    NiUInt32 uiMaxDetailLevel, NiUInt32* puiErrorCode)
{
    NiUInt32 uiDummyErrorCode = 0;
    if (puiErrorCode == 0)
        puiErrorCode = &uiDummyErrorCode;

    // Check that the file is ready:
    if(!pkSectorFile)
    {
        *puiErrorCode |= NiTerrainSector::EC_DOF_INVALID_FILE;
        return false;
    }    

    // Check the width of the blocks:
    NiUInt32 uiBlockWidth = m_pkSectorData->GetBlockWidthInVerts();
    if (pkSectorFile->GetBlockWidthInVerts() != uiBlockWidth)
    {
        m_bUpdating = false;
        *puiErrorCode |= NiTerrainSector::EC_DOF_INVALID_SECTOR_SIZE;
        return false;
    }

    // Allocate memory to store the data
    NiOutputDebugString("Allocating Memory\n");
    bool bCreatedStreams = CreateStreams(uiMaxDetailLevel);
    if (!bCreatedStreams)
    {
        NIASSERT(0 && "Failed to create data streams");
        m_bUpdating = false;
        *puiErrorCode |= NiTerrainSector::EC_DOF_STREAM_CREATION_ERROR;
        return false;
    }

    // Build the NiTerrainDataLeaf tree
    BuildQuadTree(uiMaxDetailLevel);

    // Now build the meshes from the data streams
    NiOutputDebugString("Creating meshes\n");
    BuildMesh(uiMaxDetailLevel);

    // Declare pointers to the streams from the files for each block:
    float* pfHeightData = 0;
    float* pfNormalData = 0;
    float* pfTangentData = 0;
    float* pfMorphHeightData = 0;
    float* pfMorphNormalData = 0;
    float* pfMorphTangentData = 0;

    // Get an iterator to the position stream we are about to populate
    NiTerrainPositionRandomAccessIterator kPositions;
    NiTerrainNormalRandomAccessIterator kNormals;
    NiTerrainTangentRandomAccessIterator kTangents;

    // Get the terrain configuration
    NiTerrainConfiguration kConfig = GetConfiguration();

    // Prepare to populate the position stream
    NiUInt32 uiNumLeaves = m_kLeafArray.GetSize();
    NiTerrainDataLeaf* pkLeaf = 0;
    NiIndex kBottomLeft;
    NiIndex kIndex;
    float fPositionOffset = float(m_pkSectorData->GetSectorSize()) * -0.5f;

    // Cycle through each leaf and populate its portion of the position stream

    // Jump to the LOD level that we need to begin loading from:
    NiInt32 iNextLevel = GetSectorData()->GetHighestLoadedLOD() + 1;
    for (; iNextLevel > 0; --iNextLevel)
    {
        pkSectorFile->PushBlock(0);
    }
    NiUInt32 iLeafID = pkSectorFile->GetBlockID();

    float fBaseBoundExtent = pkSectorFile->GetBlockBoundVolume().GetExtent(0);
    
    bool bUpdateBounds = (fBaseBoundExtent > fPositionOffset * -2.0f);
    if (fBaseBoundExtent < fPositionOffset * -1.0f)
    {
        *puiErrorCode |= NiTerrainSector::EC_DOF_MISSING_DATA;
        return false;
    }
    for (; iLeafID < uiNumLeaves; ++iLeafID)
    {
        pkLeaf = GetLeafByRegion(iLeafID);
        pkLeaf->GetBottomLeftIndex(kBottomLeft);
        NiUInt32 uiIndexIncrement = 1 << pkLeaf->GetNumSubDivisions();

        // Get iterators into the streams for this block:
        pkLeaf->GetPositionIterator(kPositions);
        pkLeaf->GetNormalIterator(kNormals);
        pkLeaf->GetTangentIterator(kTangents);

        // Get the buffers for each of these streams:
        NiUInt32 uiDataLength;
        pfHeightData = pkSectorFile->GetHeightData(uiDataLength);
        pfNormalData = pkSectorFile->GetNormalData(uiDataLength);

        if (kConfig.IsTangentDataEnabled())
            pfTangentData = pkSectorFile->GetTangentData(uiDataLength);

        if (kConfig.IsMorphingDataEnabled())
        {
            pfMorphHeightData = pkSectorFile->GetMorphHeightData(uiDataLength);
            pfMorphNormalData = pkSectorFile->GetMorphNormalData(uiDataLength);
            pfMorphTangentData = 
                pkSectorFile->GetMorphTangentData(uiDataLength);
        }

        // Check that the required streams are here:
        if(pfHeightData == 0)
        {
            *puiErrorCode |= NiTerrainSector::EC_DOF_MISSING_DATA;
            return false;
        }
        
        // Make sure that tangent/normal data will be built if needed
        if(pfNormalData == 0 || (pfTangentData == 0 && 
            kConfig.IsTangentDataEnabled()))
        {
            pkLeaf->GetQuadMesh()->RequestLightingRebuild();
        }
        else
        {
            pkLeaf->GetQuadMesh()->MarkLightingBuilt();
        }

        // Make sure morphing data will be built if needed
        if ((pfMorphHeightData == 0 || 
            pfMorphNormalData == 0 || 
            pfMorphTangentData == 0) &&
            kConfig.IsMorphingDataEnabled())
        {
            pkLeaf->GetQuadMesh()->RequestMorphRebuild();
        }
        else
        {
            pkLeaf->GetQuadMesh()->MarkMorphBuilt();
        }

        for (NiUInt32 y = 0; y < uiBlockWidth; y++)
        {
            NiUInt32 uiIndexY = kBottomLeft.y + y * uiIndexIncrement;
            for (NiUInt32 x = 0; x < uiBlockWidth; x++)
            {                               
                NiUInt32 uiIndexX = kBottomLeft.x + x * uiIndexIncrement;

                // Calculate the index of the vert to modify:
                NiUInt32 uiStreamIndex = x + y * uiBlockWidth;

                // Set the position of this vertex:
                kPositions.SetComponent(uiStreamIndex,
                    NiTerrainPositionRandomAccessIterator::X,
                    float(uiIndexX) + fPositionOffset);
                kPositions.SetComponent(uiStreamIndex,
                    NiTerrainPositionRandomAccessIterator::Y,
                    float(uiIndexY) + fPositionOffset);
                if (pfHeightData)
                {
                    kPositions.SetComponent(uiStreamIndex,
                        NiTerrainPositionRandomAccessIterator::Z,
                        pfHeightData[uiStreamIndex]);
                }

                // Set the normal of this vertex:
                NiPoint2 kNormal = NiPoint2::ZERO;
                if (pfNormalData)
                {
                    kNormal.x = pfNormalData[uiStreamIndex * 2 + 0];
                    kNormal.y = pfNormalData[uiStreamIndex * 2 + 1];                    
                }
                kNormals.SetHighDetail(uiStreamIndex, kNormal);

                // Set the tangent of this vertex:
                NiPoint2 kTangent;
                if (pfTangentData)
                {
                    kTangent.x = pfTangentData[uiStreamIndex * 2 + 0];
                    kTangent.y = pfTangentData[uiStreamIndex * 2 + 1];
                    kTangents.SetHighDetail(uiStreamIndex, kTangent);
                }

                // Set the morph height data of this vertex:
                if (pfMorphHeightData)
                {
                    kPositions.SetComponent(uiStreamIndex,
                        NiTerrainPositionRandomAccessIterator::W,
                        pfMorphHeightData[uiStreamIndex]);
                }

                // Set the morph normal data of this vertex:
                if (pfMorphNormalData)
                {
                    kNormals.SetComponent(uiStreamIndex,
                        NiTerrainNormalRandomAccessIterator::Z,
                        pfMorphNormalData[uiStreamIndex * 2 + 0]);
                    kNormals.SetComponent(uiStreamIndex,
                        NiTerrainNormalRandomAccessIterator::W,
                        pfMorphNormalData[uiStreamIndex * 2 + 1]);
                }

                // Set the morph tangent data of this vertex:
                if (pfMorphTangentData && 
                    GetConfiguration().IsTangentDataEnabled())
                {
                    kTangents.SetComponent(uiStreamIndex,
                        NiTerrainTangentRandomAccessIterator::Z,
                        pfMorphTangentData[uiStreamIndex * 2 + 0]);
                    kTangents.SetComponent(uiStreamIndex,
                        NiTerrainTangentRandomAccessIterator::W,
                        pfMorphTangentData[uiStreamIndex * 2 + 1]);
                }
            }
        }
        // Setup the bounds of this block if the bounds are correct
        NiBound kBound = pkSectorFile->GetBlockBound();
        NiPoint3 kCenter = kBound.GetCenter();
        float fRadius = kBound.GetRadius();
        pkLeaf->SetBoundData(
            kCenter, 
            fRadius, 
            pkSectorFile->GetBlockSumVerts());
        pkLeaf->SetBoundVolumeBox(
            pkSectorFile->GetBlockBoundVolume());
        
        if (bUpdateBounds)
        {
            pkLeaf->RequestUpdate();
            *puiErrorCode |= NiTerrainSector::EC_DOF_INVALID_BOUNDS;
        }
        
        // Move onto the next block in the file:
        if((iLeafID != uiNumLeaves - 1) && !pkSectorFile->NextBlock())
        {
            break;
        }
    }
    
    // Check that we loaded all the expected leaves:
    if (iLeafID != uiNumLeaves)
    {   
        *puiErrorCode |= NiTerrainSector::EC_DOF_MISSING_DATA;
        m_bUpdating = false;
        return false;
    }

    if (bUpdateBounds)
    {
        BuildBounds();
        m_pkQuadData->Update();
    }

    // Build the normals, tangents, lighting that are needed.
    NiOutputDebugString("Building data\n");
    BuildData(uiMaxDetailLevel);

    m_bUpdating = false;

    *puiErrorCode |= EC_DOF_LOADED;
    return true;   
}
//----------------------------------------------------------------------------
bool NiTerrainSector::GetHeightMapValues(float*& pfMap, size_t stBufferSize, 
    NiUInt32&, float& fMinVal, float& fMaxVal)
{
    // The map must be allocated
    NIASSERT(pfMap);
    
    fMinVal = NI_INFINITY;
    fMaxVal = -NI_INFINITY;

    NiUInt32 uiNumLeaves = 1 << m_pkSectorData->GetNumLOD();
    uiNumLeaves *= uiNumLeaves;

    NiUInt32 uiBlockSize = m_pkSectorData->GetBlockSize();
    NiUInt32 uiBlockWidth = m_pkSectorData->GetBlockWidthInVerts();
    NiUInt32 uiRowSize = m_pkSectorData->GetSectorWidthInVerts();
    NiUInt32 uiSectorSize = m_pkSectorData->GetSectorSize();

    NiIndex kBottomLeft;
    NiTerrainPositionRandomAccessIterator kPositions;

    size_t stBytesRequired = m_pkSectorData->GetSectorWidthInVerts() * 
        m_pkSectorData->GetSectorWidthInVerts() * sizeof(float);
    if (stBytesRequired != stBufferSize)
        return false;

    for (NiUInt32 uiLeafID = 0; uiLeafID < uiNumLeaves; ++uiLeafID)
    {
        NiUInt32 uiMaxX = uiBlockSize;
        NiUInt32 uiMaxY = uiBlockWidth;
        NiUInt32 uiMinY = 1;
        NiTerrainDataLeaf* pkLeaf = GetLeaf(
            GetLeafOffset(GetSectorData()->GetNumLOD()) + uiLeafID);
        pkLeaf->GetBottomLeftIndex(kBottomLeft);

        if (kBottomLeft.y == 0)
            uiMinY = 0;
        if (kBottomLeft.x + uiBlockSize == m_pkSectorData->GetSectorSize())
            uiMaxX += 1;

        NiUInt32 uiOffset = (uiSectorSize - (kBottomLeft.y + uiBlockSize)) * 
            uiRowSize + kBottomLeft.x;
        pkLeaf->GetPositionIterator(kPositions);

        for (NiUInt32 y = uiMinY; y < uiMaxY; ++y)
        {
            for (NiUInt32 x = 0; x < uiMaxX; ++x)
            {
                float fHeight = kPositions.GetComponent(y * uiBlockWidth + x,
                    NiTerrainPositionRandomAccessIterator::Z);

                if(fMinVal > fHeight)
                    fMinVal = fHeight;
                if(fMaxVal < fHeight)
                    fMaxVal = fHeight;

                pfMap[(uiMaxY - y - 1) * uiRowSize + uiOffset + x] = fHeight;
            }
        }
    }
    
    return true;
}
//----------------------------------------------------------------------------
bool NiTerrainSector::SetHeightFromMap(float* pfMap, size_t stBufferSize,
    NiUInt32 uiNumVal, bool)
{
    // Check that the buffer is the correct size
    NiUInt32 uiRequiredSize = m_pkSectorData->GetSectorWidthInVerts() * 
        m_pkSectorData->GetSectorWidthInVerts();
    if (uiRequiredSize != uiNumVal)
        return false;
    if (uiRequiredSize * sizeof(float) != stBufferSize)
        return false;

    // Prevent any updates or draws while we are processing
    m_bUpdating = true;

    // Prepare to populate the position stream
    NiUInt32 uiBlockWidth = m_pkSectorData->GetBlockWidthInVerts();
    NiUInt32 uiNumLeaves = m_kLeafArray.GetSize();
    NiTerrainVertex kVertex;
    NiIndex kBottomLeft;
    NiIndex kLocalIndex;

    // Cycle through each leaf and populate its portion of the position stream
    for (NiUInt32 uiLeafID = 0; uiLeafID < uiNumLeaves; ++uiLeafID)
    {
        NiTerrainDataLeaf* pkLeaf = m_kLeafArray[uiLeafID];
        NIVERIFY(pkLeaf->GetQuadMesh() != NULL);

        pkLeaf->GetBottomLeftIndex(kBottomLeft);
        NiUInt32 uiIndexIncrement = 1 << pkLeaf->GetNumSubDivisions();

        for (NiUInt32 y = 0; y < uiBlockWidth; y++)
        {
            NiUInt32 uiIndexY = kBottomLeft.y + y * uiIndexIncrement;

            NiUInt32 yOffset = (m_pkSectorData->GetSectorSize() - uiIndexY) * 
                m_pkSectorData->GetSectorWidthInVerts();

            for (NiUInt32 x = 0; x < uiBlockWidth; x++)
            {
                NiUInt32 uiIndexX = kBottomLeft.x + x * uiIndexIncrement;

                NiUInt32 uiBufferIndex = uiIndexX + yOffset;

                pkLeaf->IndexToLocalFast(NiIndex(uiIndexX, uiIndexY),
                    kLocalIndex);
                pkLeaf->GetVertexAt(kVertex, kLocalIndex);

                NiTerrainSector* pkSector = pkLeaf->GetContainingSector();
                pkSector->ModifyVertexHeightFrom(
                    kVertex, pkLeaf, 
                    pfMap[uiBufferIndex],
                    true);
            }
        }
    }

    // Rebuild all the lighting
    RebuildAllLighting();

    m_bUpdating = false;

    return true;
}
// --------------------------------------------------------------------------
void NiTerrainSector::SmoothLowerLOD()
{
    NIASSERT(m_puiLeafOffsets);

    NiUInt32 uiWidthInVerts = m_pkSectorData->GetBlockWidthInVerts();
    NiUInt32 uiSize = m_pkSectorData->GetBlockSize();

    // Average each individual block
    for (NiUInt32 uiLevel = m_pkSectorData->GetNumLOD(); uiLevel > 0; 
        --uiLevel)
    {
        NiUInt32 uiNumLeaves = 1 << uiLevel;
        uiNumLeaves *= uiNumLeaves;

        for (NiUInt32 uiLeafID = 0; uiLeafID < uiNumLeaves; ++uiLeafID)
        {
            NiTerrainDataLeaf* pkLeaf = 
                GetLeaf(m_puiLeafOffsets[uiLevel] + uiLeafID);

            // Dont blur when a custom INDEX stream is in use
            if (pkLeaf->GetQuadMesh()->HasDynamicIndexStream())
                continue;

            NiTerrainPositionRandomAccessIterator kPositions;
            pkLeaf->GetPositionIterator(kPositions);
            NiUInt32 uiRowOffset;
            NiUInt32 uiPos;

            // These arrays will not be completely filled
            NiTerrainPositionRandomAccessIterator kAdjacentPositions[9];
            const NiTerrainDataLeaf* apkAdjacents[9];

            for (NiUInt8 uc = 0; uc < 4; uc += 1)
            {
                uiPos = 1 << uc;
                apkAdjacents[uiPos] = pkLeaf->GetAdjacent(uiPos);
                if (apkAdjacents[uiPos])
                {
                    apkAdjacents[uiPos]->
                        GetPositionIterator(kAdjacentPositions[uiPos]);
                }
            }

            for (NiUInt32 y = 0; y < uiWidthInVerts; ++y)
            {
                uiRowOffset = y * uiWidthInVerts;

                for (NiUInt32 x = 0; x < uiWidthInVerts; ++x)
                {
                    // Only smooth verts which join 8 triangles, not 4
                    if (y % 2 != x % 2)
                        continue;

                    uiPos = uiRowOffset + x;

                    // center
                    float fSum = kPositions.GetComponent(uiPos,
                        NiTerrainPositionRandomAccessIterator::Z);
                    float fNum = 5.0f;
                    
                    // left
                    if (x == 0) 
                    {
                        if (apkAdjacents[NiTerrainDataLeaf::BORDER_LEFT])
                        {
                            fSum += kAdjacentPositions
                                [NiTerrainDataLeaf::BORDER_LEFT].GetComponent(
                                uiRowOffset + uiSize, 
                                NiTerrainPositionRandomAccessIterator::Z);
                        }
                        else
                        {
                            fNum -= 1.0f;
                        }
                    } 
                    else
                    {
                        fSum += kPositions.GetComponent(uiPos - 1,
                            NiTerrainPositionRandomAccessIterator::Z); 
                    }

                    // right
                    if (x == uiSize) 
                    {
                        if (apkAdjacents[NiTerrainDataLeaf::BORDER_RIGHT]) 
                        {
                            fSum += kAdjacentPositions[NiTerrainDataLeaf::
                                BORDER_RIGHT].GetComponent(
                                uiRowOffset,
                                NiTerrainPositionRandomAccessIterator::Z);
                        }
                        else
                        {
                            fNum -= 1.0f;
                        }
                    } 
                    else
                    {
                        fSum += kPositions.GetComponent(uiPos + 1,
                            NiTerrainPositionRandomAccessIterator::Z);
                    }

                    // bottom
                    if (y == 0) 
                    {
                        if (apkAdjacents[NiTerrainDataLeaf::BORDER_BOTTOM])
                        {
                            fSum += kAdjacentPositions
                                [NiTerrainDataLeaf::BORDER_BOTTOM].GetComponent
                                (uiSize * uiWidthInVerts + x, 
                                NiTerrainPositionRandomAccessIterator::Z);
                        }
                        else
                        {
                            fNum -= 1.0f;
                        }
                    } 
                    else
                    {
                        fSum += kPositions.GetComponent(
                            uiPos - uiWidthInVerts, 
                            NiTerrainPositionRandomAccessIterator::Z);
                    }

                    // top
                    if (y == uiSize) 
                    {
                        if (apkAdjacents[NiTerrainDataLeaf::BORDER_TOP])
                        {
                            fSum += kAdjacentPositions
                                [NiTerrainDataLeaf::BORDER_TOP].GetComponent(
                                x, NiTerrainPositionRandomAccessIterator::Z);
                        }
                        else
                        {
                            fNum -= 1.0f;
                        }
                    } 
                    else
                    {
                        fSum += kPositions.GetComponent(uiPos + uiWidthInVerts,
                            NiTerrainPositionRandomAccessIterator::Z);
                    }
                    
                    // Since we only modify the '8' verts and only read the '4'
                    // verts, we can modify the stream directly.
                    kPositions.SetComponent(uiPos,
                        NiTerrainPositionRandomAccessIterator::Z, fSum / fNum);
                }
            }
        }

        // Now we need to average the borders.

        NiUInt32 uiBlockSpacing = m_pkSectorData->GetBlockSize() << 
            (m_pkSectorData->GetNumLOD() - uiLevel);
        NiUInt32 uiMaxIndex = m_pkSectorData->GetSectorSize() - uiBlockSpacing;
        NiUInt32 uiBlockSize = m_pkSectorData->GetBlockSize();

        // Y borders
        NiTerrainDataLeaf* pkLeft;
        NiTerrainDataLeaf* pkRight;        
        NiTerrainPositionRandomAccessIterator kPositionsLeft;
        NiTerrainPositionRandomAccessIterator kPositionsRight;
        NiUInt32 uiRowSize = m_pkSectorData->GetBlockWidthInVerts();
        NiUInt32 uiLeftOffset = uiBlockSize;
        NiUInt32 uiBotOffset = uiBlockSize * uiRowSize;

        for (NiUInt32 uiX = 0; uiX < uiMaxIndex; uiX += uiBlockSpacing)
        {
            for (NiUInt32 uiY = 0; uiY <= uiMaxIndex; uiY += uiBlockSpacing)
            {
                pkLeft = GetLeafContaining(NiIndex(uiX, uiY),
                    (NiUInt32)uiLevel);
                pkRight = pkLeft->GetAdjacent(NiTerrainDataLeaf::BORDER_RIGHT);
                pkLeft->GetPositionIterator(kPositionsLeft);
                pkRight->GetPositionIterator(kPositionsRight);

                float fAverage;
                NiUInt32 uiRowOffset = 0;
                for (NiUInt32 ui = 1; ui < uiBlockSize; ++ui)
                {
                    fAverage = kPositionsLeft.GetComponent(
                        uiLeftOffset + uiRowOffset,
                        NiTerrainPositionRandomAccessIterator::Z);
                    fAverage += kPositionsRight.GetComponent(uiRowOffset,
                        NiTerrainPositionRandomAccessIterator::Z);
                    fAverage *= 0.5f;

                    kPositionsLeft.SetComponent(uiLeftOffset + uiRowOffset,
                        NiTerrainPositionRandomAccessIterator::Z, fAverage);
                    kPositionsRight.SetComponent(uiRowOffset,
                        NiTerrainPositionRandomAccessIterator::Z, fAverage);

                    uiRowOffset += uiRowSize ;
                }
            }
        }
        
        // X borders
        NiTerrainDataLeaf* pkBottom;
        NiTerrainDataLeaf* pkTop;
        NiTerrainPositionRandomAccessIterator kPositionsBottom;
        NiTerrainPositionRandomAccessIterator kPositionsTop;

        for (NiUInt32 uiX = 0; uiX <= uiMaxIndex; uiX += uiBlockSpacing)
        {
            for (NiUInt32 uiY = 0; uiY < uiMaxIndex; uiY += uiBlockSpacing)
            {
                pkBottom = GetLeafContaining(NiIndex(uiX, uiY),
                    (NiUInt32)uiLevel);
                pkTop = pkBottom->GetAdjacent(NiTerrainDataLeaf::BORDER_TOP);
                pkBottom->GetPositionIterator(kPositionsBottom);
                pkTop->GetPositionIterator(kPositionsTop);

                float fAverage;
                NiUInt32 uiRowOffset = 0;
                for (NiUInt32 ui = 1; ui < uiBlockSize; ++ui)
                {                    
                    fAverage = kPositionsBottom.GetComponent(uiBotOffset + ui,
                        NiTerrainPositionRandomAccessIterator::Z);
                    fAverage += kPositionsTop.GetComponent(ui,
                        NiTerrainPositionRandomAccessIterator::Z);
                    fAverage *= 0.5f;

                    kPositionsBottom.SetComponent(uiBotOffset + ui, 
                        NiTerrainPositionRandomAccessIterator::Z, fAverage);
                    kPositionsTop.SetComponent(ui,
                        NiTerrainPositionRandomAccessIterator::Z, fAverage);

                    uiRowOffset += uiRowSize ;
                }
            }
        }

        // Corners        
        const NiInt32 auiLeafOffsetsX[] = {-1, 0, -1, 0};
        const NiInt32 auiLeafOffsetsY[] = {0, 0, -1, -1};
        NiUInt32 uiSpacing = 1 << (m_pkSectorData->GetNumLOD() - uiLevel);

        for (NiUInt32 uiX = 0; uiX <= uiMaxIndex; uiX += uiBlockSpacing)
        {
            for (NiUInt32 uiY = 0; uiY <= uiMaxIndex; uiY += uiBlockSpacing)
            {
                // These arrays will not be completely filled
                const NiTerrainDataLeaf* apkLeaves[4];
                NiTerrainPositionRandomAccessIterator kPositions[4];
                float fSum = 0.0f;
                float fCount = 0.0f;

                // First pass, collect average
                for (NiUInt8 uc = 0; uc < 4; uc += 1)
                {
                    NiInt32 iOffsetX = uiX + 
                        auiLeafOffsetsX[uc] * uiSpacing;
                    NiInt32 iOffsetY = uiY + 
                        auiLeafOffsetsY[uc] * uiSpacing;

                    if (iOffsetX < 0 || iOffsetY < 0)
                    {
                        apkLeaves[uc] = 0;
                        continue;
                    }

                    apkLeaves[uc] = GetLeafContaining(
                        NiIndex((NiUInt16)iOffsetX, (NiUInt16)iOffsetY),
                        uiLevel);

                    if (apkLeaves[uc])
                    {
                        apkLeaves[uc]->GetPositionIterator(kPositions[uc]);
                        
                        fSum += kPositions[uc].GetComponent(
                            (abs(auiLeafOffsetsX[uc]) * uiBlockSize) + 
                            (abs(auiLeafOffsetsY[uc]) * uiBlockSize) * 
                            uiRowSize, 
                            NiTerrainPositionRandomAccessIterator::Z);

                        fCount += 1.0f;
                    }
                }
                
                // Second pass, apply average
                for (NiUInt8 uc = 0; uc < 4; uc += 1)
                {
                    if (apkLeaves[uc])
                    {
                        NiIndex kIndex(
                            (NiUInt16)abs(auiLeafOffsetsX[uc]) * uiBlockSize,
                            (NiUInt16)abs(auiLeafOffsetsY[uc]) * uiBlockSize);

                        kPositions[uc].SetComponent(
                            kIndex.x + kIndex.y * uiRowSize,
                            NiTerrainPositionRandomAccessIterator::Z, 
                            fSum / fCount);
                    }
                }
            }
        }
    }
}
// --------------------------------------------------------------------------
bool NiTerrainSector::CreateBlankGeometry()
{
    // Destroy the sector block to reinitialise the sector
    if (m_pkQuadMesh)
    {
        NiOutputDebugString("Using existing quad mesh\n");
        return true;
    }

    // Allocate memory to store the data
    NiOutputDebugString("Allocating Memory\n");
    bool bCreatedStreams = CreateStreams();
    if (!bCreatedStreams)
    {
        NIASSERT(!"Failed to create data streams");
        return false;
    }

    // Build the NiTerrainDataLeaf tree
    if (m_kLeafArray.GetEffectiveSize() == 0)
        BuildQuadTree();

    // Now build the meshes from the data
    NiOutputDebugString("Creating meshes\n");
    BuildMesh();
    
    NiUInt32 uiBlockWidth = m_pkSectorData->GetBlockWidthInVerts();
    
    // Get an iterator to the position stream we are about to populate
    NiTerrainPositionRandomAccessIterator kCurrentIterator;
    NiTerrainNormalRandomAccessIterator kCurrentNormal;
    NiTerrainTangentRandomAccessIterator kCurrentTangent;

    // Prepare to populate the position stream
    NiUInt32 uiNumLeaves = m_kLeafArray.GetSize();
    NiTerrainDataLeaf* pkLeaf = 0;
    NiIndex kBottomLeft;
    float fPositionOffset = float(m_pkSectorData->GetSectorSize()) * -0.5f;

    // Cycle through each leaf and populate its portion of the position stream
    for (NiUInt32 uiLeafID = 0; uiLeafID < uiNumLeaves; ++uiLeafID)
    {
        pkLeaf = m_kLeafArray[uiLeafID];
        pkLeaf->GetPositionIterator(kCurrentIterator);
        pkLeaf->GetNormalIterator(kCurrentNormal);
        pkLeaf->GetTangentIterator(kCurrentTangent);
        NIASSERT(kCurrentIterator.Exists());

        NiUInt32 uiIndexIncrement = 1 << pkLeaf->GetNumSubDivisions();
        pkLeaf->GetBottomLeftIndex(kBottomLeft);

        for (NiUInt32 y = 0; y < uiBlockWidth; y++)
        {
            NiUInt32 uiIndexY = kBottomLeft.y + y * uiIndexIncrement;
            for (NiUInt32 x = 0; x < uiBlockWidth; x++)
            {
                NiUInt32 uiIndexX = kBottomLeft.x + x * uiIndexIncrement;
                               
                // Offset in this leaves position stream region, of this vertex
                NiPoint3 kVertex;                
                kVertex.x = float(uiIndexX) + fPositionOffset;
                kVertex.y = float(uiIndexY) + fPositionOffset;
                kVertex.z = 0.0f;

                kCurrentIterator.SetHighDetail(x + y * uiBlockWidth, kVertex);
                kCurrentIterator.SetLowDetail(x + y * uiBlockWidth, kVertex);

                // Set the normals and tangents
                NiPoint2 kNormal = NiPoint2::ZERO;
                kCurrentNormal.SetHighDetail(x + y * uiBlockWidth, kNormal);
                kCurrentNormal.SetLowDetail(x + y * uiBlockWidth, kNormal);

                if (GetConfiguration().IsTangentDataEnabled())
                {
                    NiPoint2 kTangent = NiPoint2::UNIT_X; 
                    kCurrentTangent.SetHighDetail(x + y * uiBlockWidth, kTangent);
                    kCurrentTangent.SetLowDetail(x + y * uiBlockWidth, kTangent);
                }
            }
        }
        pkLeaf->GetQuadMesh()->MarkLightingBuilt();
        pkLeaf->GetQuadMesh()->MarkMorphBuilt();
    }

    // Build the normals, tangents, lighting etc.
    NiOutputDebugString("Building lighting\n");
    BuildData();

    // Create some cache data in the leaves
    NiOutputDebugString("Creating bounds\n");
    BuildBounds();

    // Create a mask stack
    if (m_uiSurfaceMaskStackSize == 0)
        InitMaskStack(4);

    m_pkSectorData->SetHighestLoadedLOD(m_pkSectorData->GetNumLOD());

    return true;
}
// --------------------------------------------------------------------------
void NiTerrainSector::LoadSurfaces(const NiSurfacePalettePtr& spSurfacePalette,
    const char* pcArchive, NiUInt32 uiDetailLevel, NiUInt32* puiErrorCode)
{
    // The surface directory should be relative to the base terrain file
    NiFixedString kSectorPath = GetSectorPath(pcArchive);
    NiFixedString kSurfacePath = GetSurfacePath(pcArchive);

    InitMaskStack(NiTerrainMaterial::MAX_LAYERS_PER_BLOCK);
    NiDOMTool kDom;
    LoadSurfacesRecursive(spSurfacePalette, kSectorPath, kSurfacePath, 
        m_pkQuadData, kDom, uiDetailLevel, puiErrorCode);
}
// --------------------------------------------------------------------------
void NiTerrainSector::LoadSurfacesRecursive(
    const NiSurfacePalettePtr& spSurfacePalette, const char* pcCurrentDir,
    const char* pcArchive, NiTerrainDataLeaf* pkCurrentLeaf, NiDOMTool& kDom,
    NiUInt32 uiDetailLevel, NiUInt32* puiErrorCode)
{
    NiUInt32 uiDummyErrorCode;
    if (!puiErrorCode)
        puiErrorCode = &uiDummyErrorCode;
        
    NiOutputDebugString("Loading surface masks\n");

    NiString kFileName = pcCurrentDir;
    kFileName += "\\surfaces.xml";

    // Make a temporary copy of the file name to standardize the path.
    char* pcPathToStandardize = kFileName.MakeExternalCopy();
    NiPath::Standardize(pcPathToStandardize);

    kFileName = pcPathToStandardize;

    // Destroy the temporary copy of the file name.
    NiFree(pcPathToStandardize);

    if ((NiInt32)pkCurrentLeaf->GetLevel() > 
        m_pkSectorData->GetHighestLoadedLOD()) 
    {
        kDom.Init((const char*)kFileName);
        if (NIBOOL_IS_TRUE(kDom.LoadFile())) 
        {
            TiXmlElement* pkCurElem;
            pkCurElem = kDom.ResetSectionTo("Block");
            pkCurElem = kDom.SetSectionTo("SurfaceMask");
    
            const char* pcPackage = 0;
            const char* pcName = 0;
            const char* pcLayerNum = 0;
            const NiSurfacePackage* pkPackage = 0;
            NiSurfaceMask* pkMask = 0;
            NiDevImageConverter kImageConverter;
            NiPixelData* pkPixelData = 0;
            NiSurface* pkSurface = 0;
            int iPriority = 0;
            NiUInt32 uiMaxLayerNum = 0;
    
            // Keep a track of surfaces, in their requested positions
            NiTPointerMap<int, NiSurface*> kSurfaces(7);
    
            while (pkCurElem)
            {
                if (kDom.IsCurrentSectionValid())
                {
                    pcPackage = kDom.GetAttributeFromCurrent("package");
                    if (pcPackage == 0)
                        pcPackage = "";
    
                    pcName = kDom.GetAttributeFromCurrent("name");
                    if (pcName == 0)
                        pcName = "";
    
                    pcLayerNum = kDom.GetAttributeFromCurrent("position");
                    if (pcLayerNum == 0)
                        pcLayerNum = "";
    
                    
                    NiString kPackagePath(pcPackage);
                    
                    // Make sure the surface palette knows about this package
                    if (NiPath::IsRelative(pcPackage))
                    {
                        kPackagePath = NiString(pcArchive) + "\\" + 
                            kPackagePath;
                    }
                    pkPackage = spSurfacePalette->LoadPackage(kPackagePath);
    
                    if (pkPackage == NULL)
                    {                        
                        char acBuf[1024];
                        NiSprintf(acBuf, 1024, "Could not find material package " \
                            "\"%s\" in directory \"%s\".", pcPackage,                          
                            (const char*)kPackagePath);
                        NiOutputDebugString(acBuf);
                        *puiErrorCode |= EC_SURFACE_INVALID_PACKAGE;
                    }
                    else if (pkPackage->GetName() == NULL || pkPackage->GetName()
                        == "")
                    {
                        NiOutputDebugString("Error with the Terrain data\n"
                            " file: The package name is empty");
                        *puiErrorCode |= EC_SURFACE_EMPTY_PACKAGENAME;                    
                    }
                    else
                    {
                        // Get the surface
                        pkSurface = spSurfacePalette->GetSurface(
                            pkPackage->GetName(), NiFixedString(pcName));
                    }
    
                    if (!pkSurface)
                    {
                        NiString kMsg;
                        kMsg.Format("Failed to load surface: Name=%s", pcName);
                        NiOutputDebugString(kMsg);
                        *puiErrorCode |= EC_SURFACE_FAILED_LOADSURFACE;    
                    }
    
                    // Make sure the layer number is valid
                    if (!NiString(pcLayerNum).ToInt(iPriority))
                    {
                        // Ignore this surface, it has an invalid position!
                        char msg[256];
                        NiSprintf(msg, 256, "Invalid position, expected "
                            "integer! (position=\"%s\")", pcLayerNum);
                        NiOutputDebugString(msg);
                        *puiErrorCode |= EC_SURFACE_INVALID_INDEX; 

                        pkCurElem = kDom.SetSectionToNextSibling();
                        continue;
                    }
    
                    uiMaxLayerNum = NiMax(uiMaxLayerNum, iPriority);
    
                    NiSurface* pkDummySurface;
                    if (kSurfaces.GetAt(iPriority, pkDummySurface))
                    {
                        // Ignore this surface, it is a duplicate!
                        char msg[256];
                        NiSprintf(msg, 256, "Duplicate surface position found! "
                            "(position=\"%c\")", pcLayerNum);
                        NiOutputDebugString(msg);
                        *puiErrorCode |= EC_SURFACE_INVALID_INDEX; 

                        pkCurElem = kDom.SetSectionToNextSibling();
                        continue;
                    }
    
                    kSurfaces.SetAt(iPriority, pkSurface);
                }
    
                pkCurElem = kDom.SetSectionToNextSibling();
            }

            iPriority = 0;
            for (NiUInt32 ui = 0; ui <= uiMaxLayerNum; ++ui)
            {
                if (!kSurfaces.GetAt(ui, pkSurface))
                    continue;

                // Load the mask
                NiString kPath;
                kPath.Format("%s\\%d.tga", (const char*)pcCurrentDir, ui);
                if (!kImageConverter.CanReadImageFile(kPath))
                {
                    NiOutputDebugString(NiString("Failed to read mask: ") + 
                        NiString(kPath));
                    *puiErrorCode |= EC_SURFACE_FAILED_READ_MASK; 
                    continue;
                }

                pkPixelData = kImageConverter.ReadImageFile(kPath, 0);

                // Dynamic or static?
                pkMask = CreateMask(pkPixelData);
                NiDelete pkPixelData;

                if (!pkSurface)
                {
                    NiOutputDebugString(NiString("Failed to load surface:") + 
                        NiString(pcName));
                    *puiErrorCode |= EC_SURFACE_FAILED_LOADSURFACE; 
                    continue;
                }
                else
                {
                    // Add this surface to the leaf, and make an alpha layer
                    pkSurface->CompileSurface();
                    pkCurrentLeaf->AddSurfaceMask(pkSurface, pkMask, 
                        (NiUInt32)iPriority);

                    ++iPriority;
                }
            }
            
            kDom.EndSection();
            kDom.EndSection();
        }
        else
        {
            *puiErrorCode |= EC_SURFACE_FAILED_LOADSURFACE;
        }

        kDom.Flush();
    }

    // Attempt to recurse into children
    NiString kChildDir;
    const NiUInt8 ucQuadrantX[] = {0, 1, 1, 0};
    const NiUInt8 ucQuadrantY[] = {0, 0, 1, 1};

    for (NiUInt8 uc = 0; uc < 4; ++uc)
    {
        kChildDir.Format("%s\\Block_%d_%d", 
            (const char*)pcCurrentDir, ucQuadrantX[uc], ucQuadrantY[uc]);
        NiTerrainDataLeaf* pkChild = pkCurrentLeaf->GetChildAt(uc);
        if (NiFile::DirectoryExists(kChildDir) && pkChild)
        {
            LoadSurfacesRecursive(spSurfacePalette, (const char*)kChildDir,
                pcArchive, pkChild, kDom, uiDetailLevel, puiErrorCode);
        }
    }
}
// --------------------------------------------------------------------------
bool NiTerrainSector::SaveSurfaceMasks(const char* pcArchive, 
    NiUInt32* puiErrorCode)
{
    NiUInt32 uiDummyErrorCode;
    if (!puiErrorCode)
        puiErrorCode = &uiDummyErrorCode;

    NiOutputDebugString("- Saving Surface Masks\n");

    // Check that we are ready to go:
    if(m_pkQuadData == 0)
    {
        *puiErrorCode |= EC_SURFACE_INVALID_TERRAIN_DATA;
        return false;
    }

    NiFixedString kSectorPath = GetSectorPath(pcArchive);
    if (!NiFile::DirectoryExists(kSectorPath))
    {
        NiFile::CreateDirectoryRecursive(kSectorPath);
    }

    if (!NiFile::DirectoryExists(kSectorPath))
    {
        *puiErrorCode |= EC_INVALID_ARCHIVE_PATH;
        return false;
    }

    NiDOMTool kDom;
    NiTerrainDataLeaf* pkData;

    for (NiUInt32 uiLeafID = 0; uiLeafID < m_kLeafArray.GetSize(); ++uiLeafID)
    {
        pkData = m_kLeafArray.GetAt(uiLeafID);
        NiTerrainDataLeaf* pkParent = pkData->GetParent();

        // Only stream out the top-level node in the quadtree.
        if (pkParent != NULL)
            continue;

        // Build the blocks path
        // We go anti clockwise, starting at the bottom left.
        const NiUInt8 ucQuadrantX[] = {0, 1, 1, 0};
        const NiUInt8 ucQuadrantY[] = {0, 0, 1, 1};

        NiString kBlockDir(int(
            10 * m_pkSectorData->GetNumLOD() - pkData->GetNumSubDivisions()));
        kBlockDir.Format("");


        while (pkParent)
        {
            NiUInt32 uiIndex = 0;
            for (NiUInt32 ui = 0; ui < 4; ui++)
            {
                if (pkParent->GetChildAt(ui) == pkData)
                {
                    uiIndex = ui;
                    break;
                }
            }

            kBlockDir.Format("\\Block_%d_%d\\%s", 
                ucQuadrantX[uiIndex], ucQuadrantY[uiIndex], 
                (const char*)kBlockDir);

            pkParent = pkParent->GetParent();
        }

        kBlockDir = kSectorPath + NiString("\\") + kBlockDir;

        // Does the new directory exist?
        if (!NiFile::DirectoryExists(kBlockDir))
        {
            if (!NiFile::CreateDirectoryRecursive(kBlockDir))
            {
                NIASSERT(false);
                *puiErrorCode |= EC_SURFACE_FAILED_DIRECTORY;
                return false;
            }
        }

        // Create an XML file, specifying which surfaces the masks refer to
        kDom.Init(kBlockDir + (char*)("\\surfaces.xml"));
        kDom.WriteHeader();
        kDom.BeginSection("Block");

        NiUInt32 uiSurfaceCount = pkData->GetSurfaceCount();
        const NiSurface* pkSurface = 0;
        const NiSurfacePackage* pkPackage = 0;
        for (NiUInt32 ui = 0; ui < uiSurfaceCount; ++ui)
        {
            pkSurface = pkData->GetSurface((NiUInt32)ui);
            if (!pkSurface)
                continue;

            kDom.BeginSection("SurfaceMask");

            pkPackage = pkSurface->GetPackage();
                 
            //Convert package file path to relative WTR surfacePath
            char* pcPackageFileRelative = NiAlloc(char, 512);
            NiString kPackageFile = pkPackage->m_kFileSrc;
            if (NiPath::ConvertToRelative(
                pcPackageFileRelative, 512, kPackageFile, pcArchive))
            {
                kDom.AssignAttribute("package", pcPackageFileRelative);
            }
            else
            {
                kDom.AssignAttribute("package", kPackageFile);
            }
            kDom.AssignAttribute("name", pkSurface->GetName());
            kDom.AssignAttribute("position", NiString::FromInt(int(ui)));

            NiFree(pcPackageFileRelative);

            kDom.EndSection();
        }
        
        kDom.EndSection();

        NiBool bResult = kDom.SaveFile();
        kDom.Flush();

        if (!bResult)
        {
            *puiErrorCode |= EC_SURFACE_FAILED_SAVE;
            return false;
        }

        // Save the actual data to TGA files
        const NiSurfaceMask *pkMask = NULL;
        for (NiUInt32 ui = 0; ui < uiSurfaceCount; ++ui)
        {
            pkMask = pkData->GetSurfaceMask(ui);
            if (!pkMask)
                continue;

            // Create the file:
            NiFile* pkFile = NiFile::GetFile(kBlockDir + (char*)("\\")
                + NiString::FromInt(ui) + (char*)(".tga"),
                NiFile::WRITE_ONLY);

            NiUInt8 ucIDLength = 0; // No Image ID Data
            NiUInt8 ucColorMapType = 0; // No colour map data
            NiUInt8 ucImageType = 3 | 0x08; // or 11 if greyscale AND RLE
            NiUInt8 aucColorMapSpec[5] = {0};
            NiUInt8 aucImageSpec[10] = {0};
            NiUInt16* pusWidth = (NiUInt16*)&aucImageSpec[4];
            NiUInt16* pusHeight = (NiUInt16*)&aucImageSpec[6];

            *pusWidth = (NiUInt16)pkMask->GetWidth();
            *pusHeight = (NiUInt16)pkMask->GetWidth();

            aucImageSpec[8] = 8; // 8 bits per pixel
            aucImageSpec[9] = 0; // 0 bits for alpha, Bottom Left ordering

            NiUInt64 ulExtAreaOffset = 0;
            NiUInt64 ulDeveloperDirOffset = 0;
            NiUInt8 aucSignature[] = {"TRUEVISION-XFILE."};

            // Write File Header
            pkFile->Seek(0);
            pkFile->Write(&ucIDLength, sizeof(ucIDLength));
            pkFile->Write(&ucColorMapType, sizeof(ucColorMapType));
            pkFile->Write(&ucImageType, sizeof(ucImageType));
            pkFile->Write(&aucColorMapSpec, sizeof(aucColorMapSpec));
            pkFile->Write(&aucImageSpec, sizeof(aucImageSpec));
            
            // Write image data (RLE)
            const NiUInt8* pucCurPixel = pkMask->m_pucMask;
            for (NiUInt32 uiPixelID = 0; uiPixelID < 
                (NiUInt32)(*pusWidth) * (*pusWidth);
                ++uiPixelID)
            {
                NiUInt8 ucPixCount = 0;
                bool bRaw;
                if (uiPixelID%(*pusWidth) && // Less than end of scan line
                    pucCurPixel[uiPixelID] == pucCurPixel[uiPixelID + 1])
                {
                    bRaw = false; // Should be RLE

                    // Count how many of these values there are:
                    ucPixCount++;
                    
                    //Less than end of scan line
                    while ((uiPixelID+ucPixCount)%(*pusWidth) && 
                        pucCurPixel[uiPixelID + ucPixCount] == 
                        pucCurPixel[uiPixelID] && ucPixCount < 128)
                    {
                        ucPixCount++;
                    }
                }
                else
                {
                    // Should be a raw packet
                    bRaw = true;
                    //Count how many of these values there are:
                    ucPixCount++;

                    //Less than end of scan line
                    while ((uiPixelID+ucPixCount)%(*pusWidth)&& 
                        pucCurPixel[uiPixelID + ucPixCount] != 
                        pucCurPixel[uiPixelID + ucPixCount + 1] &&
                        ucPixCount < 128)
                    {   
                        ucPixCount++;
                    }
                }

                // We should be at the end of the RLE Packet now
                ucPixCount--;
                if (bRaw)
                {
                    pkFile->Write(&ucPixCount, sizeof(ucPixCount));
                    pkFile->Write(&pucCurPixel[uiPixelID], ucPixCount + 1);
                }
                else
                {
                    ucPixCount |= 0x80; // Mark as non-raw
                    pkFile->Write(&ucPixCount, sizeof(ucPixCount));
                    pkFile->Write(&pucCurPixel[uiPixelID], 1);
                }
                uiPixelID += ucPixCount & 0x7F;
            }
             
            // Write Image footer
            pkFile->Write(&ulExtAreaOffset, sizeof(ulExtAreaOffset));
            pkFile->Write(&ulDeveloperDirOffset, sizeof(ulDeveloperDirOffset));
            pkFile->Write(&aucSignature, sizeof(aucSignature));

            NiDelete pkFile;
        }

        kDom.Flush();
    }
    

    return true;
}
