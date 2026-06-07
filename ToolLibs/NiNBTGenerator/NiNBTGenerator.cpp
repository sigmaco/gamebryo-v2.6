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
#include "NiNBTGenerator.h"

#include <NiSystem.h>
#include <NiMesh.h>
#include <NiToolDataStream.h>
#include <NiDataStreamElementLock.h>
#include <NiDataStreamPrimitiveLock.h>
#include <NiMeshAlgorithms.h>
#include <NiSkinningMeshModifier.h>
#include <NiMorphMeshModifier.h>
#include <NiStringExtraData.h>
#include <NiMeshUtilities.h>
#include <NiPoint2.h>

// ATi method
#include "ATi/NmFileIO.h"

//----------------------------------------------------------------------------
class ATIInitDataLoop
{
public:
    ATIInitDataLoop(NiVertIter& kVertIter, NiNormalIter& kNormalIter,
        NiTexCoordIter& kTexCoordIter, NmRawTriangle* pkTris) :
        m_kVertIter(kVertIter), m_kNormalIter(kNormalIter), 
        m_kTexCoordIter(kTexCoordIter), m_pkTris(pkTris) {};

    inline bool operator()(const NiUInt32* pIndices, NiUInt32,
        NiUInt32 uiTri, NiUInt32)
    {
        NmRawTriangle* pkTri = &m_pkTris[uiTri];

        NiUInt32 ui0 = pIndices[0];
        NiUInt32 ui1 = pIndices[1];
        NiUInt32 ui2 = pIndices[2];

        pkTri->vert[0].v1.x = m_kVertIter[ui0].x;
        pkTri->vert[0].v1.y = m_kVertIter[ui0].y;
        pkTri->vert[0].v1.z = m_kVertIter[ui0].z;
        pkTri->norm[0].v1.x = m_kNormalIter[ui0].x;
        pkTri->norm[0].v1.y = m_kNormalIter[ui0].y;
        pkTri->norm[0].v1.z = m_kNormalIter[ui0].z;
        pkTri->texCoord[0].v1.u = m_kTexCoordIter[ui0].x;
        pkTri->texCoord[0].v1.v = m_kTexCoordIter[ui0].y;

        pkTri->vert[1].v1.x = m_kVertIter[ui1].x;
        pkTri->vert[1].v1.y = m_kVertIter[ui1].y;
        pkTri->vert[1].v1.z = m_kVertIter[ui1].z;
        pkTri->norm[1].v1.x = m_kNormalIter[ui1].x;
        pkTri->norm[1].v1.y = m_kNormalIter[ui1].y;
        pkTri->norm[1].v1.z = m_kNormalIter[ui1].z;
        pkTri->texCoord[1].v1.u = m_kTexCoordIter[ui1].x;
        pkTri->texCoord[1].v1.v = m_kTexCoordIter[ui1].y;

        pkTri->vert[2].v1.x = m_kVertIter[ui2].x;
        pkTri->vert[2].v1.y = m_kVertIter[ui2].y;
        pkTri->vert[2].v1.z = m_kVertIter[ui2].z;
        pkTri->norm[2].v1.x = m_kNormalIter[ui2].x;
        pkTri->norm[2].v1.y = m_kNormalIter[ui2].y;
        pkTri->norm[2].v1.z = m_kNormalIter[ui2].z;
        pkTri->texCoord[2].v1.u = m_kTexCoordIter[ui2].x;
        pkTri->texCoord[2].v1.v = m_kTexCoordIter[ui2].y;

        return true;
    }

    NiVertIter& m_kVertIter;
    NiNormalIter& m_kNormalIter;
    NiTexCoordIter& m_kTexCoordIter;
    NmRawTriangle* m_pkTris;

private:
    ATIInitDataLoop & operator=( const ATIInitDataLoop & );
};
//----------------------------------------------------------------------------
class ATIAssignStreamLoop
{
public:
    ATIAssignStreamLoop(NiBiNormalIter& kBiNormalIter,
        NiTangentIter& kTangentIter, NmRawTangentSpaceD* pkTSData) :
        m_kBiNormalIter(kBiNormalIter), m_kTangentIter(kTangentIter),
        m_pkTSData(pkTSData) {};

    inline bool operator()(const NiUInt32* pIndices, NiUInt32,
        NiUInt32 uiTri, NiUInt32)
    {
        NmRawTangentSpaceD* pkTri = &m_pkTSData[uiTri];

        NiUInt32 ui0 = pIndices[0];
        NiUInt32 ui1 = pIndices[1];
        NiUInt32 ui2 = pIndices[2];

        m_kTangentIter[ui0].x = (float)pkTri->tangent[0].v1.x;
        m_kTangentIter[ui0].y = (float)pkTri->tangent[0].v1.y;
        m_kTangentIter[ui0].z = (float)pkTri->tangent[0].v1.z;
        m_kBiNormalIter[ui0].x = (float)pkTri->binormal[0].v1.x;
        m_kBiNormalIter[ui0].y = (float)pkTri->binormal[0].v1.y;
        m_kBiNormalIter[ui0].z = (float)pkTri->binormal[0].v1.z;

        m_kTangentIter[ui1].x = (float)pkTri->tangent[1].v1.x;
        m_kTangentIter[ui1].y = (float)pkTri->tangent[1].v1.y;
        m_kTangentIter[ui1].z = (float)pkTri->tangent[1].v1.z;
        m_kBiNormalIter[ui1].x = (float)pkTri->binormal[1].v1.x;
        m_kBiNormalIter[ui1].y = (float)pkTri->binormal[1].v1.y;
        m_kBiNormalIter[ui1].z = (float)pkTri->binormal[1].v1.z;

        m_kTangentIter[ui2].x = (float)pkTri->tangent[2].v1.x;
        m_kTangentIter[ui2].y = (float)pkTri->tangent[2].v1.y;
        m_kTangentIter[ui2].z = (float)pkTri->tangent[2].v1.z;
        m_kBiNormalIter[ui2].x = (float)pkTri->binormal[2].v1.x;
        m_kBiNormalIter[ui2].y = (float)pkTri->binormal[2].v1.y;
        m_kBiNormalIter[ui2].z = (float)pkTri->binormal[2].v1.z;

        return true;
    }

    NiBiNormalIter& m_kBiNormalIter;
    NiTangentIter& m_kTangentIter;
    NmRawTangentSpaceD* m_pkTSData;

private:
    ATIAssignStreamLoop & operator=( const ATIAssignStreamLoop & );
};
//---------------------------------------------------------------------------
NiNBTGenerator::NiNBTGenerator()
{
}
//---------------------------------------------------------------------------
NiNBTGenerator::~NiNBTGenerator()
{
}
//---------------------------------------------------------------------------
bool NiNBTGenerator::SetupBinormalTangentData(NiToolPipelineCloneHelper&
    kCloneHelper, NiToolPipelineCloneHelper::CloneSetPtr& spCloneSet,
    NiUInt32 uiUVSet, NiShaderRequirementDesc::NBTFlags eNBTMethod)
{
    // Assume all mesh in cloneset has same requirements for NBT generation
    NiMesh* pkMesh = spCloneSet->GetAt(0);

    // This class does not handle MAX NBT generation
    if ((eNBTMethod == NiShaderRequirementDesc::NBT_METHOD_NONE) ||
        (eNBTMethod == NiShaderRequirementDesc::NBT_METHOD_DCC))
        return true;

    // The only primitive type we consider for NBTs is triangle meshes
    NIASSERT(pkMesh->GetPrimitiveType() == 
        NiPrimitiveType::PRIMITIVE_TRIANGLES);

    CreateTangentSpace_Mesh(kCloneHelper, spCloneSet, eNBTMethod, uiUVSet);

    // Reset the scene modifiers to account for the new BTs.
    NiUInt32 uiSetCnt = spCloneSet->GetSize();
    for(NiUInt32 ui=0; ui < uiSetCnt; ui++)
    {
        pkMesh = spCloneSet->GetAt(ui);
        NiMesh::ResetSceneModifiers(pkMesh);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiNBTGenerator::SetupBinormalTangentData(NiMesh* pkMesh,
    NiUInt32 uiUVSet, NiShaderRequirementDesc::NBTFlags eNBTMethod)
{
    NiToolPipelineCloneHelper kCloneHelper(pkMesh);
    NiTObjectSet<NiToolPipelineCloneHelper::CloneSetPtr> kCloneSets;
    kCloneHelper.GetAllCloneSets(kCloneSets);

    return SetupBinormalTangentData(kCloneHelper, kCloneSets.GetAt(0), uiUVSet,
        eNBTMethod);
}
//----------------------------------------------------------------------------
bool NiNBTGenerator::CreateBindPoses(NiToolPipelineCloneHelper&
        kCloneHelper, NiToolPipelineCloneHelper::CloneSetPtr& spCloneSet,
        NiUInt32 uiUVSet, NiBiNormalIter& kBiNormIter,
        NiTangentIter& kTangentIter)
{
    // Assume all mesh in cloneset has same requirements for NBT generation
    NiMesh* pkMesh = spCloneSet->GetAt(0);

    // in the case that the mesh is skinned, we need to have bind pose data
    // for binormals & tangents
    NiSkinningMeshModifier* pkSkin = NiGetModifier(NiSkinningMeshModifier,
        pkMesh);
    if (pkSkin == NULL)
        return false;

    NiMorphMeshModifier* pkMorph = NiGetModifier(NiMorphMeshModifier,
        pkMesh);

    // prepare the appropriate access flags for bind pose streams
    NiUInt8 ucAccessMask = (NiUInt8)((pkMorph) ? 
        NiDataStream::ACCESS_CPU_WRITE_MUTABLE :
        NiDataStream::ACCESS_CPU_WRITE_STATIC);
    if (pkSkin->GetSoftwareSkinned())
        ucAccessMask |= NiDataStream::ACCESS_CPU_READ;
    else
        ucAccessMask |= NiDataStream::ACCESS_GPU_READ;

    NiUInt32 uiElementCount = pkMesh->GetVertexCount();

    CreateBTStreams(kCloneHelper, spCloneSet, uiUVSet,
        NiCommonSemantics::BINORMAL_BP(), NiCommonSemantics::TANGENT_BP(),
        NiDataStreamElement::F_FLOAT32_3, ucAccessMask, true);

    NiDataStreamElementLock kBLock = NiDataStreamElementLock(pkMesh, 
        NiCommonSemantics::BINORMAL_BP(), uiUVSet, 
        NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_TOOL_WRITE);
    NiDataStreamElementLock kTLock = NiDataStreamElementLock(pkMesh, 
        NiCommonSemantics::TANGENT_BP(), uiUVSet, 
        NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_TOOL_WRITE);

    // fill in the data in the new streams
    if (kBLock.IsLocked() && kTLock.IsLocked())
    {
        NiTStridedRandomAccessIterator<NiPoint3> kBPoseIter = 
            kBLock.begin<NiPoint3>();
        NiTStridedRandomAccessIterator<NiPoint3> kTPoseIter = 
            kTLock.begin<NiPoint3>();
        for (NiUInt32 ui = 0; ui < uiElementCount; ui++)
        {
            kBPoseIter[ui] = kBiNormIter[ui];
            kTPoseIter[ui] = kTangentIter[ui];
        }
    }
    return true;
}
//----------------------------------------------------------------------------
bool NiNBTGenerator::CreateTangentSpace_Mesh(NiToolPipelineCloneHelper&
        kCloneHelper, NiToolPipelineCloneHelper::CloneSetPtr& spCloneSet,
         NiShaderRequirementDesc::NBTFlags eNBTMethod, NiUInt32 uiUVSet)
{
    // Assume all mesh in cloneset has same requirements for NBT generation
    NiMesh* pkMeshShape = spCloneSet->GetAt(0);

    // This function assumes pkMeshShape consist of a single submesh and that
    // the data streams are made up of a single continuous region.
    // There is no assumption about whether or not indices exist.

    // Create the Binormal and Tangent streams if they don't already exist
    CreateBTStreams(kCloneHelper, spCloneSet, uiUVSet,
        NiCommonSemantics::BINORMAL(), NiCommonSemantics::TANGENT(),
        NiDataStreamElement::F_FLOAT32_3,
        NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_CPU_WRITE_STATIC,
        true);

    bool bReturn = false;
    if (eNBTMethod == NiShaderRequirementDesc::NBT_METHOD_NDL)
    {
        bReturn = NiMeshUtilities::RecalculateNBTs_NDL(pkMeshShape,
            (NiUInt16)(uiUVSet), NiCommonSemantics::POSITION(), 0,
            NiCommonSemantics::NORMAL(), 0, NiCommonSemantics::BINORMAL(),
            NiCommonSemantics::TANGENT(), 0, true);
    }
    else if (eNBTMethod == NiShaderRequirementDesc::NBT_METHOD_ATI)
    {
        bReturn = CreateBasisMatrices_ATI(kCloneHelper, spCloneSet, uiUVSet);
    }
    else
    {
        NIASSERT(0 && "Invalid method passed into CreateTangentSpace_Mesh");
        return false;
    }

    // if the mesh is skinned, it should have binormal & tangent bind poses
    NiSkinningMeshModifier* pkSkin = 
        NiGetModifier(NiSkinningMeshModifier, pkMeshShape);
    if (bReturn && pkSkin)
    {
        // Get the Binormal iterator
        NiDataStreamElementLock kLockBiNormals(pkMeshShape,
            NiCommonSemantics::BINORMAL(), uiUVSet, 
            NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_TOOL_READ);

        NIASSERT(kLockBiNormals.IsLocked());

        // Get the Tangent iterator
        NiDataStreamElementLock kLockTangents(pkMeshShape,
            NiCommonSemantics::TANGENT(), uiUVSet, 
            NiDataStreamElement::F_FLOAT32_3, NiDataStream::LOCK_TOOL_READ);

        NIASSERT(kLockTangents.IsLocked());

        // Change the access flags for the tangents and bi normals if we are
        // cpu skinning.
        if (pkSkin->GetSoftwareSkinned())
        {
            NiToolDataStream* pkTDS = NiDynamicCast(NiToolDataStream, 
                kLockBiNormals.GetDataStream());

            NiUInt8 uiAccessMask = pkTDS->GetAccessMask();
            uiAccessMask &= ~NiDataStream::ACCESS_CPU_WRITE_ANY;
            uiAccessMask |= NiDataStream::ACCESS_CPU_WRITE_VOLATILE;
            pkTDS->SetAccessMask(uiAccessMask);

            pkTDS = NiDynamicCast(NiToolDataStream, 
                kLockTangents.GetDataStream());

            uiAccessMask = pkTDS->GetAccessMask();
            uiAccessMask &= ~NiDataStream::ACCESS_CPU_WRITE_ANY;
            uiAccessMask |= NiDataStream::ACCESS_CPU_WRITE_VOLATILE;
            pkTDS->SetAccessMask(uiAccessMask);
        }

        NiTangentIter kTangentIter = kLockTangents.begin<NiPoint3>();
        NiBiNormalIter kBiNormalIter = kLockBiNormals.begin<NiPoint3>();

        CreateBindPoses(kCloneHelper, spCloneSet, uiUVSet, kBiNormalIter,
            kTangentIter);
    }

    return bReturn;
}
//----------------------------------------------------------------------------
bool NiNBTGenerator::CreateBasisMatrices_ATI(NiToolPipelineCloneHelper&,
        NiToolPipelineCloneHelper::CloneSetPtr& spCloneSet,
        NiUInt32 uiUVSet)
{
    // Assume all mesh in cloneset has same requirements for NBT generation
    NiMesh* pkMesh = spCloneSet->GetAt(0);

    // Attempt to lock the UVSet
    NiDataStreamElementLock kLockTexCoords(pkMesh, 
        NiCommonSemantics::TEXCOORD(), uiUVSet,
        NiDataStreamElement::F_FLOAT32_2,  
        NiDataStream::LOCK_TOOL_READ);
    if (!kLockTexCoords.IsLocked())
        return false;

    // Get the verts iterator
    // Assume that verts are in the format of 3 floats
    NiDataStreamElementLock kLockVerts(pkMesh,
        NiCommonSemantics::POSITION(), 0, NiDataStreamElement::F_FLOAT32_3,
        NiDataStream::LOCK_TOOL_READ);
    if (!kLockVerts.IsLocked())
        return false;

    NiUInt32 uiVertCount = kLockVerts.count();
    NiVertIter kVertIter = kLockVerts.begin<NiPoint3>();

    // Get the texture coord iterator
    NiTexCoordIter kTexCoordIter = kLockTexCoords.begin<NiPoint2>();
    NIASSERT(kLockTexCoords.count() == uiVertCount);

    // Get the normal iterator
    NiDataStreamElementLock kLockNormals(pkMesh,
        NiCommonSemantics::NORMAL(), 0, NiDataStreamElement::F_FLOAT32_3,
        NiDataStream::LOCK_TOOL_WRITE);

    if (!kLockNormals.IsLocked())
        return false;

    NiNormalIter kNormalIter = kLockNormals.begin<NiPoint3>();

    // Get the Binormal iterator
    NiDataStreamElementLock kLockBiNormals(pkMesh,
        NiCommonSemantics::BINORMAL(), uiUVSet, 
        NiDataStreamElement::F_FLOAT32_3, 
        NiDataStream::LOCK_TOOL_WRITE | NiDataStream::LOCK_TOOL_READ);

    if (!kLockBiNormals.IsLocked())
        return false;

    NiBiNormalIter kBiNormalIter = kLockBiNormals.begin<NiPoint3>();

    // Get the Tangent iterator
    NiDataStreamElementLock kLockTangents(pkMesh,
        NiCommonSemantics::TANGENT(), uiUVSet, 
        NiDataStreamElement::F_FLOAT32_3, 
        NiDataStream::LOCK_TOOL_WRITE | NiDataStream::LOCK_TOOL_READ);

    if (!kLockTangents.IsLocked())
        return false;

    NiTangentIter kTangentIter = kLockTangents.begin<NiPoint3>();

    // Clear out binormals and tangents
    for (NiUInt32 ui = 0; ui < uiVertCount; ui++)
        kBiNormalIter[ui] = kTangentIter[ui] = NiPoint3::ZERO;

    // Put data into ATI's data structures
    NiUInt32 uiNumTris = pkMesh->GetTotalPrimitiveCount();
    NmRawTriangle* pkTris = NiExternalNew NmRawTriangle[uiNumTris];

    ATIInitDataLoop kATIInitDataLoop(kVertIter, kNormalIter, kTexCoordIter,
        pkTris);
    NiMeshAlgorithms::ForEachPrimitiveAllSubmeshes(pkMesh, kATIInitDataLoop,
        NiDataStream::LOCK_TOOL_READ);

    NmRawTangentSpaceD* pkTSData = NULL;

    NIVERIFY(NmComputeTangentsD(uiNumTris, pkTris, &pkTSData));

    ATIAssignStreamLoop kATIAssignStreamLoop(kBiNormalIter, kTangentIter,
        pkTSData);
    NiMeshAlgorithms::ForEachPrimitiveAllSubmeshes(pkMesh,
        kATIAssignStreamLoop, NiDataStream::LOCK_TOOL_READ);

    NiExternalDelete [] pkTris;
    NiExternalDelete [] pkTSData;

    return true;
}
//----------------------------------------------------------------------------
void AddStream(NiToolPipelineCloneHelper&
    kCloneHelper, NiToolPipelineCloneHelper::CloneSetPtr& spCloneSet,
    const NiFixedString& kSemantic, NiUInt32 uiSemanticIndex,
    NiDataStreamElement::Format eFormat, NiUInt32 uiCount,
    NiUInt8 uiAccessMask, NiDataStream::Usage eUsage, const void*, 
    bool bForceToolDSCreate, bool bCreateDefaultRegion)
{
    NiDataStream* pkDataStream = 
        NiDataStream::CreateSingleElementDataStream(eFormat, uiCount, 
        uiAccessMask, eUsage, NULL, bCreateDefaultRegion, bForceToolDSCreate);
    NIASSERT(pkDataStream);

    NiDataStreamRef* pkRef = NiNew NiDataStreamRef();
    pkRef->SetDataStream(pkDataStream);
    NIASSERT(pkDataStream && pkDataStream->GetElementDescCount() == 1);
    pkRef->BindSemanticToElementDescAt(0, kSemantic, uiSemanticIndex);
    pkRef->SetPerInstance(false);
    if (bCreateDefaultRegion)
    {
        NIASSERT(pkDataStream->GetRegionCount() == 1);
        pkRef->BindRegionToSubmesh(0, 0);
    }

    NIVERIFY(kCloneHelper.AddStreamRef(spCloneSet, pkRef));

    // AddStreamRef allocates a new stream ref so we need to delete this one.
    NiDelete pkRef;
    pkRef = NULL;
}
//----------------------------------------------------------------------------
bool NiNBTGenerator::CreateBTStreams(NiToolPipelineCloneHelper&
    kCloneHelper, NiToolPipelineCloneHelper::CloneSetPtr& spCloneSet,
    NiUInt32 uiUVSet, const NiFixedString& kBiTangentSemantic,
    const NiFixedString& kTangentSemantic,
    NiDataStreamElement::Format eFormat, NiUInt8 uiAccessMask, 
    bool bForceToolDSCreate, bool bCreateDefaultRegion)
{
    // Assume all mesh in cloneset has same requirements for NBT generation
    NiMesh* pkMesh = spCloneSet->GetAt(0);

    // We don't expect the Binormal and Tangent streams to exist yet
    // Use them if they exist, but be ready to create them if not

    // Look for a UV stream
    NiDataStreamRef* pkRef;
    NiDataStreamElement kElement;
    bool bFound = pkMesh->FindStreamRefAndElementBySemantic(
        NiCommonSemantics::TEXCOORD(), uiUVSet, 
        NiDataStreamElement::F_UNKNOWN, pkRef, kElement);
    if (!bFound)
    {
        NILOG("Could not find the texture coordinate %d.\n", uiUVSet);
        return false;
    }

    // If we've found a UV stream, get the element count
    NiDataStream* pkUVStream;
    pkUVStream = pkRef->GetDataStream();
    if (!pkUVStream)
    {
        NILOG("Invalid data stream for texture coordinate %d.\n", uiUVSet);
        return false;
    }

    NiUInt32 uiCount = pkUVStream->GetTotalCount();

    // Look for an existing binormal stream that matches the UV index
    bFound = pkMesh->FindStreamRefAndElementBySemantic(
        kBiTangentSemantic, uiUVSet, 
        NiDataStreamElement::F_UNKNOWN, pkRef, kElement);
    if (!bFound)
    {
        // if we didn't find a matching stream, we need to create it
        AddStream(kCloneHelper, spCloneSet, kBiTangentSemantic, uiUVSet,
            eFormat, uiCount, uiAccessMask, NiDataStream::USAGE_VERTEX,
            NULL, bForceToolDSCreate, bCreateDefaultRegion);
    }

    // Look for an existing tangent stream that matches the UV index
    bFound = pkMesh->FindStreamRefAndElementBySemantic(
        kTangentSemantic, uiUVSet, 
        NiDataStreamElement::F_UNKNOWN, pkRef, kElement);
    if (!bFound)
    {
        // if we didn't find a matching stream, we need to create it
        AddStream(kCloneHelper, spCloneSet, kTangentSemantic, uiUVSet,
            eFormat, uiCount, uiAccessMask, NiDataStream::USAGE_VERTEX,
            NULL, bForceToolDSCreate, bCreateDefaultRegion);
    }
    return true;
}
//---------------------------------------------------------------------------

