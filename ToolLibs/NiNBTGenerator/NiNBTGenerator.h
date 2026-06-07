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

#ifndef NINBTGENERATOR_H
#define NINBTGENERATOR_H

#include <NiSystem.h>
#include <NiTStridedRandomAccessIterator.h>
#include <NiMeshTools.h>
#include <NiShaderRequirementDesc.h>
#include "NiNBTGeneratorLibType.h"

class NiBinaryStream;
class NiPoint3;
class NiPoint2;
class NiColorA;
class NiMesh;

typedef NiTStridedRandomAccessIterator<NiPoint3> NiVertIter;
typedef NiTStridedRandomAccessIterator<NiColorA> NiColorIter;
typedef NiTStridedRandomAccessIterator<NiPoint3> NiNormalIter;
typedef NiTStridedRandomAccessIterator<NiPoint3> NiBiNormalIter;
typedef NiTStridedRandomAccessIterator<NiPoint3> NiTangentIter;
typedef NiTStridedRandomAccessIterator<NiPoint2> NiTexCoordIter;

class NINBTGENERATOR_ENTRY NiNBTGenerator
{
public:
    NiNBTGenerator();
    ~NiNBTGenerator();

    static bool SetupBinormalTangentData(NiToolPipelineCloneHelper& 
        kCloneHelper, NiToolPipelineCloneHelper::CloneSetPtr& spCloneSet,
        NiUInt32 uiUVSet, NiShaderRequirementDesc::NBTFlags eNBTMethod);

    static bool SetupBinormalTangentData(NiMesh* pkMesh,
        NiUInt32 uiUVSet, NiShaderRequirementDesc::NBTFlags eNBTMethod);

    /**
        Create default bitangent and tangent streams on a mesh.
        
        If the requested data streams already exist, then the function
        returns without creating new streams and it does not check for
        the requested access.
        
        @param kCloneHelper The reference to the clone helper being used.
        @param spCloneSet The reference to the cloneset consisting of the 
            mesh(s) that will share the bitangent and tangent streams.
        @param usIVSet The texture set to use. The newly created streams
            will have this semantic index.
        @param kBiTangentSemantic The semantic to use for the bitangent
            stream. For example, BITANGENT_BP can be used to generate
            skinning streams.
        @param kTangentSemantic The semantic to use for the tangent
            stream. For example, TANGENT_BP can be used to generate
            skinning streams.
        @param eFormat The data format to use for the new streams. Gamebryo
            code for computing and recomputing NBT frames requires the
            format of these streams be F_FLOAT32_3.
        @param uiAccessMask The access mask to use for the new streams.
        @param bForceToolDSCreate Whether or not to create tool data streams.
        @param bCreateDefaultRegion If true, create a default single region
            for the stream. If false, an application must explicitly create
            the regions.
            
        @return False if the requested texture set does not exist or has
            an invalid data stream, true otherwise.
    */
    static bool CreateBTStreams(NiToolPipelineCloneHelper&
        kCloneHelper, NiToolPipelineCloneHelper::CloneSetPtr& spCloneSet,
        NiUInt32 uiUVSet, const NiFixedString& kBiTangentSemantic,
        const NiFixedString& kTangentSemantic,
        NiDataStreamElement::Format eFormat, NiUInt8 uiAccessMask, 
        bool bForceToolDSCreate = false, bool bCreateDefaultRegion = true);

protected:
    static bool CreateBindPoses(NiToolPipelineCloneHelper&
        kCloneHelper, NiToolPipelineCloneHelper::CloneSetPtr& spCloneSet,
        NiUInt32 uiUVSet, NiBiNormalIter& kBiNormalIter,
        NiTangentIter& kTangentIter);

    static bool CreateTangentSpace_Mesh(NiToolPipelineCloneHelper&
        kCloneHelper, NiToolPipelineCloneHelper::CloneSetPtr& spCloneSet,
        NiShaderRequirementDesc::NBTFlags eNBTMethod, NiUInt32 uiUVSet);

    static bool CreateBasisMatrices_ATI(NiToolPipelineCloneHelper&
        kCloneHelper, NiToolPipelineCloneHelper::CloneSetPtr& spCloneSet,
        NiUInt32 uiUVSet);
};

#endif  //NINBTGENERATOR_H
