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
//---------------------------------------------------------------------------

#include "MaxImmerse.h"
#include <NiMAX.h>
#include "NiMAXNBTConverter.h"
#include <NiNBTGenerator.h>
#include <NiSystem.h>

#include <NiMesh.h>

// MAX method

//---------------------------------------------------------------------------
NiMAXNBTConverter::NiMAXNBTConverter()
{
}
//---------------------------------------------------------------------------
NiMAXNBTConverter::~NiMAXNBTConverter()
{
}
//---------------------------------------------------------------------------
bool NiMAXNBTConverter::SetupBinormalTangentData(
    NiMesh* pkMesh,
    unsigned short usUVSet,
    NiShaderRequirementDesc::NBTFlags eNBTMethod)
{
    if (!NiIsExactKindOf(NiMesh, pkMesh))
        return false;

    // Make sure we need to make NBTs
    if (eNBTMethod == NiShaderRequirementDesc::NBT_METHOD_NONE)
        return true;

    if (eNBTMethod == NiShaderRequirementDesc::NBT_METHOD_DCC)
    {
        //position stream
        NiDataStreamElementLock kPositionLock(
            pkMesh, 
            NiCommonSemantics::POSITION(), 
            0, 
            NiDataStreamElement::F_FLOAT32_3,
            NiDataStream::LOCK_TOOL_READ | 
            NiDataStream::LOCK_TOOL_WRITE); 

        // If the lock is not valid, then vertex 
        // positions were not present on the mesh.
        if(!kPositionLock.IsLocked())
            return false;

        NiTStridedRandomAccessIterator<NiPoint3> kPositionIterator =
            kPositionLock.begin<NiPoint3>(0); 

        //normal stream
        NiDataStreamElementLock kNormalLock(
            pkMesh,
            NiCommonSemantics::NORMAL(),
            0, 
            NiDataStreamElement::F_FLOAT32_3,
            NiDataStream::LOCK_TOOL_READ | 
            NiDataStream::LOCK_TOOL_WRITE); 

        // If the lock is not valid, then vertex 
        // normals were not present on the mesh.
        if(!kNormalLock.IsLocked())
        {
            kPositionLock.Unlock();
            return false;
        }

        NiTStridedRandomAccessIterator<NiPoint3> kNormalIterator = 
            kNormalLock.begin<NiPoint3>(0); 

        // texcoord stream
        NiDataStreamElementLock kTexCoordLock(
            pkMesh,
            NiCommonSemantics::TEXCOORD(),
            usUVSet, 
            NiDataStreamElement::F_FLOAT32_2,
            NiDataStream::LOCK_TOOL_READ | 
            NiDataStream::LOCK_TOOL_WRITE); 

        // If the lock is not valid, then texcoords
        // are not present on the mesh.
        if(!kTexCoordLock.IsLocked())
        {
            kPositionLock.Unlock();
            kNormalLock.Unlock();
            return false;
        }

        NiTStridedRandomAccessIterator<NiPoint2> kTexCoordIterator = 
            kTexCoordLock.begin<NiPoint2>(0); 

        //indexes
        NiDataStreamElementLock kIndexLock(
            pkMesh, 
            NiCommonSemantics::INDEX(), 
            0, 
            NiDataStreamElement::F_UINT32_1, 
            NiDataStream::LOCK_TOOL_READ | 
            NiDataStream::LOCK_TOOL_WRITE); 

        // If the lock is not valid, then index stream was not present on the
        // mesh.
        if(!kIndexLock.IsLocked())
        {
            kPositionLock.Unlock();
            kNormalLock.Unlock();
            kTexCoordLock.Unlock();
            return false;
        }

        NiTStridedRandomAccessIterator<unsigned int> kIndexIterator =
            kIndexLock.begin<unsigned int>(0); 

        if(!CreateBTStreams(pkMesh, usUVSet))
            return false;

        // Get the Binormal iterator
        NiDataStreamElementLock kBiNormalLock(pkMesh,
            NiCommonSemantics::BINORMAL(), usUVSet, 
            NiDataStreamElement::F_FLOAT32_3, 
            NiDataStream::LOCK_TOOL_WRITE | NiDataStream::LOCK_TOOL_READ);

        if (!kBiNormalLock.IsLocked())
        {
            kPositionLock.Unlock();
            kNormalLock.Unlock();
            kTexCoordLock.Unlock();
            kIndexLock.Unlock();
            return false;
        }

        NiTStridedRandomAccessIterator<NiPoint3> kBiNormalIter = 
            kBiNormalLock.begin<NiPoint3>();

        // Get the Tangent iterator
        NiDataStreamElementLock kTangentLock(pkMesh,
            NiCommonSemantics::TANGENT(), usUVSet, 
            NiDataStreamElement::F_FLOAT32_3, 
            NiDataStream::LOCK_TOOL_WRITE | NiDataStream::LOCK_TOOL_READ);

        if (!kTangentLock.IsLocked())
        {
            kPositionLock.Unlock();
            kNormalLock.Unlock();
            kTexCoordLock.Unlock();
            kIndexLock.Unlock();
            kBiNormalLock.Unlock();
            return false;
        }

        NiTStridedRandomAccessIterator<NiPoint3> kTangentIter = 
            kTangentLock.begin<NiPoint3>();

        unsigned int uiNumVertices = pkMesh->GetVertexCount();

        bool bReturn = false;
        bReturn = CreateBasisMatrices_MAX(
            uiNumVertices,
            kPositionIterator,
            kNormalIterator,
            kTexCoordIterator,
            kTangentIter,
            kBiNormalIter,
            kIndexLock.count(),
            kIndexIterator);

        kPositionLock.Unlock();
        kNormalLock.Unlock();
        kTexCoordLock.Unlock();
        kIndexLock.Unlock();
        kTangentLock.Unlock();
        kBiNormalLock.Unlock();

        return bReturn;
    }
    else
    {
        // finally, call into the NBT generation ToolLib
        // this won't do anything if the NBT method is set to NONE
        return NiNBTGenerator::SetupBinormalTangentData(pkMesh, usUVSet,
            eNBTMethod);
    }

    //return false;
}
//---------------------------------------------------------------------------
bool NiMAXNBTConverter::CreateBasisMatrices_MAX(
        unsigned int uiNumVertices, 
        NiTStridedRandomAccessIterator<NiPoint3>& kVertices,
        NiTStridedRandomAccessIterator<NiPoint3>& kNormals,
        NiTStridedRandomAccessIterator<NiPoint2>& kTextures, 
        NiTStridedRandomAccessIterator<NiPoint3>& kBiNormals,
        NiTStridedRandomAccessIterator<NiPoint3>& kTangents,
        unsigned int uiNumIndices, 
        NiTStridedRandomAccessIterator<unsigned int>& kIndices)
{
    // The vertices and UVs of a given triangle in Max's data format
    Point3 kTriUVs[3];
    Point3 kTriXYZs[3];
    // the return value for Max's Binorm Tangent calculation
    Point3 kTangBinorm[2];

    // The number of calculated NBTs
    unsigned int uiNumberOfCalcs;  
    uiNumberOfCalcs = uiNumIndices / 3;

    NIASSERT(uiNumberOfCalcs < USHRT_MAX);

    for (unsigned int uiCalc = 0; uiCalc < uiNumberOfCalcs; uiCalc++)
    {
        unsigned int uiIndex = uiCalc * 3;
        unsigned short usIndex1 = (unsigned short)kIndices[uiIndex];
        unsigned short usIndex2 = (unsigned short)kIndices[uiIndex + 1];
        unsigned short usIndex3 = (unsigned short)kIndices[uiIndex + 2];

        // store Gamebryo vertex data in Max data structure
        kTriXYZs[0].x = kVertices[usIndex1].x;
        kTriXYZs[0].y = kVertices[usIndex1].y;
        kTriXYZs[0].z = kVertices[usIndex1].z;
        kTriXYZs[1].x = kVertices[usIndex2].x;
        kTriXYZs[1].y = kVertices[usIndex2].y;
        kTriXYZs[1].z = kVertices[usIndex2].z;
        kTriXYZs[2].x = kVertices[usIndex3].x;
        kTriXYZs[2].y = kVertices[usIndex3].y;
        kTriXYZs[2].z = kVertices[usIndex3].z;

        // store Gamebryo texture coordinate data in max data structure
        kTriUVs[0].x = kTextures[usIndex1].x;
        kTriUVs[0].y = kTextures[usIndex1].y;
        kTriUVs[1].x = kTextures[usIndex2].x;
        kTriUVs[1].y = kTextures[usIndex2].y;
        kTriUVs[2].x = kTextures[usIndex3].x;
        kTriUVs[2].y = kTextures[usIndex3].y;

        // compute NBT frame
        ComputeTangentAndBinormal(kTriUVs, kTriXYZs, kTangBinorm);

        // if a triangle's UVs are CCW, we must flip the Binormals
        // first get the triangle's UV edge vectors
        NiPoint3 kP1P2Vector;
        NiPoint3 kP1P3Vector;
        kP1P2Vector.x = kTextures[usIndex2].x - kTextures[usIndex1].x;
        kP1P2Vector.y = kTextures[usIndex2].y - kTextures[usIndex1].y;
        kP1P2Vector.z = 0.0f;
        kP1P3Vector.x = kTextures[usIndex3].x - kTextures[usIndex1].x;
        kP1P3Vector.y = kTextures[usIndex3].y - kTextures[usIndex1].y;
        kP1P3Vector.z = 0.0f;
        // take the cross product of the triangle edges
        NiPoint3 kCrossResult = kP1P2Vector.Cross(kP1P3Vector);
        // a positive z component indicates Counter-Clockwise
        // Counter-clockwise triangles need to have their Binormal flipped
        float fCCW = (kCrossResult.z > 0.0f) ? 1.0f : -1.0f;

        // vert 0 of triangle
        kBiNormals[usIndex1].x += fCCW * kTangBinorm[1].x;
        kBiNormals[usIndex1].y += fCCW * kTangBinorm[1].y;
        kBiNormals[usIndex1].z += fCCW * kTangBinorm[1].z;
        kTangents[usIndex1].x += kTangBinorm[0].x;
        kTangents[usIndex1].y += kTangBinorm[0].y;
        kTangents[usIndex1].z += kTangBinorm[0].z;

        // vert 1 of triangle
        kBiNormals[usIndex2].x += fCCW * kTangBinorm[1].x;
        kBiNormals[usIndex2].y += fCCW * kTangBinorm[1].y;
        kBiNormals[usIndex2].z += fCCW * kTangBinorm[1].z;
        kTangents[usIndex2].x += kTangBinorm[0].x;
        kTangents[usIndex2].y += kTangBinorm[0].y;
        kTangents[usIndex2].z += kTangBinorm[0].z;

        // vert 2 of triangle
        kBiNormals[usIndex3].x += fCCW * kTangBinorm[1].x;
        kBiNormals[usIndex3].y += fCCW * kTangBinorm[1].y;
        kBiNormals[usIndex3].z += fCCW * kTangBinorm[1].z;
        kTangents[usIndex3].x += kTangBinorm[0].x;
        kTangents[usIndex3].y += kTangBinorm[0].y;
        kTangents[usIndex3].z += kTangBinorm[0].z;
    }

    // Unitize all normals, binormals, and tangents...
    for(NiUInt32 ui=0; ui < uiNumVertices; ui++)
    {
        NiPoint3::UnitizeVector(kNormals[ui]);
        NiPoint3::UnitizeVector(kBiNormals[ui]);
        NiPoint3::UnitizeVector(kTangents[ui]);
    }

    return true;
}
//----------------------------------------------------------------------------
bool NiMAXNBTConverter::CreateBTStreams(NiMesh* pkMesh, NiUInt16 usUVSet)
{
    // We don't expect the Binormal and Tangent streams to exist yet
    // Use them if they exist, but be ready to create them if not

    // Look for a UV stream
    NiDataStreamRef* pkRef;
    NiDataStreamElement kElement;
    bool bFound = pkMesh->FindStreamRefAndElementBySemantic(
        NiCommonSemantics::TEXCOORD(), usUVSet, 
        NiDataStreamElement::F_UNKNOWN, pkRef, kElement);
    if (!bFound)
    {
        NILOG("NBTCalculation: Could not find the texture coordinate %d.\n",
            usUVSet);
        return false;
    }

    // If we've found a UV stream, get the element count
    NiDataStream* pkUVStream;
    pkUVStream = pkRef->GetDataStream();
    if (!pkUVStream)
    {
        NILOG("NBTCalculation: Invalid data stream"
            "for texture coordinate %d.\n", usUVSet);
        return false;
    }

    NiUInt32 uiCount = pkUVStream->GetTotalCount();

    // Look for an existing binormal stream that matches the UV index
    bFound = pkMesh->FindStreamRefAndElementBySemantic(
        NiCommonSemantics::BINORMAL(), usUVSet, 
        NiDataStreamElement::F_UNKNOWN, pkRef, kElement);
    if (!bFound)
    {
        // if we didn't find a matching stream, we need to create it
        pkMesh->AddStream(NiCommonSemantics::BINORMAL(), usUVSet,
            NiDataStreamElement::F_FLOAT32_3, uiCount, 
            NiDataStream::ACCESS_GPU_READ | 
            NiDataStream::ACCESS_CPU_WRITE_STATIC, NiDataStream::USAGE_VERTEX,
            NULL, true);
    }

    // Look for an existing tangent stream that matches the UV index
    bFound = pkMesh->FindStreamRefAndElementBySemantic(
        NiCommonSemantics::TANGENT(), usUVSet, 
        NiDataStreamElement::F_UNKNOWN, pkRef, kElement);
    if (!bFound)
    {
        // if we didn't find a matching stream, we need to create it
        pkMesh->AddStream(NiCommonSemantics::TANGENT(), usUVSet,
            NiDataStreamElement::F_FLOAT32_3, uiCount, 
            NiDataStream::ACCESS_GPU_READ | 
            NiDataStream::ACCESS_CPU_WRITE_STATIC, NiDataStream::USAGE_VERTEX,
            NULL, true);
    }
    return true;
}
//---------------------------------------------------------------------------
