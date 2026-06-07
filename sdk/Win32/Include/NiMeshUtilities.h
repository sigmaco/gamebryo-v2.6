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

#ifndef NIMESHUTILITIES_H
#define NIMESHUTILITIES_H

#include <NiBound.h>
#include "NiCommonSemantics.h"
#include "NiTStridedRandomAccessIterator.h"
#include "NiMeshVertexOperators.h"
#include "NiDataStream.h"
#include "NiMeshLibType.h"
#include "NiMesh.h"
#include "NiDataStream.h"

/**
    Utility methods for operating on an NiMesh
    or subcomponents of an NiMesh
*/
class NIMESH_ENTRY NiMeshUtilities : public NiMemObject
{
public:

    /** 
        Computes a bounding volume encompassing the specified data.

        @param pkMesh The mesh to compute bounds on.
        @param bForceToolLock Forces TOOL_LOCK_READ which may potentially be 
        slow.

    */
    static void ComputeBoundsFromMesh(NiMesh* pkMesh,
        const bool bForceToolLock = false);

    /**
        Recompute normals for the given mesh using the triangle
        connectivity information.
        
        This function assumes that there is a
        kNormSemantic stream already on the mesh, with access CPU_READ and
        CPU_WRITE_MUTABLE. The function will make use of normal sharing
        information if available on the mesh.

        @param pkMesh The mesh to compute normal sharing on.
        @param kPosnSemantic The semantic to use for position information.
        @param uiPosnIndex The semantic index for position information
        @param kNormSemantic The semantic for the normals output.
        @param uiNormIndex The semantic index for normal information
        @param uiSubMesh The sub-mesh for which to compute normals.
        @param bForceToolLock Forces TOOL_LOCK_READ which may potentially be 
        slow.
    */
    static bool RecalculateNormals(NiMesh* pkMesh,
        const NiFixedString& kPosnSemantic, const NiUInt16 uiPosnIndex,
        const NiFixedString& kNormSemantic, const NiUInt16 uiNormIndex,
        const NiUInt16 uiSubMesh = 0,
        const bool bForceToolLock = false);

    /**
        Compute normals for the given mesh using the triangle
        connectivity information.

        This function should only be called if
        there are no existing normals on the mesh. It adds a new
        normal stream with access set to the given mask.
        Normals are computed for all sub-meshes.

        @param pkMesh The mesh to compute normal sharing on.
        @param kPosnSemantic The semantic to use for position information.
        @param uiPosnIndex The semantic index for position information
        @param kNormSemantic The semantic for the normals output.
        @param uiNormIndex The semantic index for normal information
        @param uiAccessMask The access mask to apply to the new normal stream.
        @param bForceToolDSCreate Forces the data stream created to be an
        NiToolDataStream.
    */
    static bool CalculateNormals(NiMesh* pkMesh,
        const NiFixedString& kPosnSemantic = NiCommonSemantics::POSITION(),
        const NiUInt16 uiPosnIndex = 0,
        const NiFixedString& kNormSemantic = NiCommonSemantics::NORMAL(),
        const NiUInt16 uiNormIndex = 0,
        const NiUInt8 uiAccessMask =
        NiDataStream::ACCESS_GPU_READ | NiDataStream::ACCESS_CPU_WRITE_STATIC, 
        const bool bForceToolDSCreate = false);

    /**
        Recompute NDL-style NBT frames for the given mesh using the triangle
        connectivity information.
        
        This function assumes that the required semantics already exist on
        the mesh, with appropriate access. If the data does not yet exist,
        use CalculateNBTs_NDL instead, which will create the streams and fill
        them. Note however that if the mesh has been cloned, CalculateNBTs_NDL
        will only create unshared data-streams.

        @param pkMesh The mesh to recompute NBTs on.
        @param usUVSet The texture set to use.
        @param kPosnSemantic The semantic to use for position information.
        @param uiPosnIndex The semantic index for position information
        @param kNormSemantic The semantic for normal information.
        @param uiNormIndex The semantic index for normal information
        @param kBiTangentSemantic The semantic to use for the bitangent
            stream. For example, BITANGENT_BP can be used to generate
            skinning streams.
        @param kTangentSemantic The semantic to use for the tangent
            stream. For example, TANGENT_BP can be used to generate
            skinning streams.
        @param uiSubMesh The sub-mesh for which to compute NBTs.
        @param bForceToolLock Forces TOOL_LOCK_READ which may potentially be 
        slow.
        
        @return True if the operation succeeded, otherwise false. Typical
            reasons for failure include an inability to find or
            lock the necessary data streams.
    */
    static bool RecalculateNBTs_NDL(NiMesh* pkMesh, NiUInt16 usUVSet,
        const NiFixedString& kPosnSemantic, const NiUInt16 uiPosnIndex,
        const NiFixedString& kNormSemantic, const NiUInt16 uiNormIndex,
        const NiFixedString& kBiTangentSemantic,
        const NiFixedString& kTangentSemantic,
        const NiUInt16 uiSubMesh = 0,
        const bool bForceToolLock = false);

    /**
        Function to return the bitangent and the tangent for vertex 0 given 
        the position and texture coordinate data of a triangle.
    */
    static void FindBT(
        const NiPoint3& kVert0, 
        const NiPoint3& kVert1, 
        const NiPoint3& kVert2, 
        const NiPoint2& kTex0, 
        const NiPoint2& kTex1, 
        const NiPoint2& kTex2, 
        NiPoint3& kBinormal, 
        NiPoint3& kTangent);

    /**
        Validates the stream specified (multi-format version).

        @param pcWhoIsValidating Context for tracing via NILOG in case of 
            validation failure.
        @param pkMesh Mesh being validated.
        @param kSemantic Semantic of NiDataStreamRef that must exist on
            pkMesh.
        @param uiSemanticIndex Index of NiDataStreamRef semantic that must 
        exist on pkMesh.
        @param kFormats List of supported formats.
        @param uiRequiredAccessFlags Access mask requirement (if multiple
            ACCESS_CPU_WRITE flags are specified, at least one must be
            present).
        @return True if the stream with semantic kSemantic, uiSemanticIndex
            exists on pkMesh and matches the specified format and access
            flags; false otherwise (also outputs an NILOG warning).
    */
    static bool ValidateStream(const char* pcWhoIsValidating,
        NiMesh* pkMesh,
        const NiFixedString& kSemantic,
        const NiUInt32 uiSemanticIndex,
        const NiTPrimitiveArray<NiDataStreamElement::Format>& kFormats,
        NiUInt8 uiRequiredAccessFlags = 0);

    /**
        Validates the stream specified (single-format version).
        
        @param pcWhoIsValidating Context for tracing via NILOG in case of 
            validation failure.
        @param pkMesh Mesh being validated.
        @param kSemantic Semantic of NiDataStreamRef that must exist on pkMesh.
        @param uiSemanticIndex Index of NiDataStreamRef semantic that must 
            exist on pkMesh.
        @param eFormat Format requirement (ignored if 
            NiDataStreamElement::F_UNKNOWN).
        @param uiRequiredAccessFlags Access mask requirement (if multiple
            ACCESS_CPU_WRITE flags are specified, at least one must
            be present).
        @return True if the stream with semantic kSemantic, 
        uiSemanticIndex exists on pkMesh and matches the specified format 
        and access flags. False otherwise (also outputs an NILOG warning).
    */
    static bool ValidateStream(const char* pcWhoIsValidating,
        NiMesh* pkMesh,
        const NiFixedString& kSemantic,
        const NiUInt32 uiSemanticIndex = 0,
        const NiDataStreamElement::Format& eFormat =
            NiDataStreamElement::F_UNKNOWN,
        NiUInt8 uiRequiredAccessFlags = 0);
};

#include "NiMeshUtilities.inl"

#endif // NIMESHUTILITIES_H
