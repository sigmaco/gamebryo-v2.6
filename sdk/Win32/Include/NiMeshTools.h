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

#ifndef NIMESHTOOLS_H
#define NIMESHTOOLS_H

#include "NiMeshToolsLibType.h"
#include "NiToolPipelineCloneHelper.h"

#include <NiBound.h>
#include <NiTStridedRandomAccessIterator.h>
#include <NiMeshVertexOperators.h>
#include <NiDataStream.h>

class NiMesh;
class NiSkinningMeshModifier;
class NiDataStream;

/**
    Utility methods for operating on an NiMesh
    or subcomponents of an NiMesh
*/
class NIMESHTOOLS_ENTRY NiMeshTools : public NiMemObject
{
public:

    /// Internal helper class for managing NiMesh vertex operations
    class MeshElementHelper
    {
    public:
        NiMeshVertexOperators::EQUALS_FUNCTION m_pFunc;
        NiUInt8 m_uiCount;
        NiUInt32 m_uiOffsetInBytes;

        MeshElementHelper(const NiDataStreamElement& kElement);

        bool Compare(void* pVertex1, void* pVertex2);
    };

    /// Internal helper class for managing NiMesh functionality
    class MeshHelper : public NiMemObject
    {
    public:

        NiUInt8 m_ucLockMask;
        NiTPrimitiveSet<MeshElementHelper> m_kElementFuncs;    
        NiDataStreamPtr m_spDS;
        NiUInt32 m_uiStride;
        NiUInt32 m_uiActiveRegion;    
        void* m_pDSStart;
        void* m_pRegionStart;

        MeshHelper(NiDataStream* pkStream, char ucLockMask);

        MeshHelper(MeshHelper* pkFrom);

        ~MeshHelper();

        void Init(NiDataStream* pkStream, char ucLockMask);

        bool IsValid();

        float* GetVertex(NiUInt32 uiVertexIdx);

        bool CompareVertex(NiUInt32 uiA, NiUInt32 uiB);

        void RemoveVertex(NiUInt32 uiVertexIdx);

        void SetVertex(NiUInt32 uiVertexIdx, float* pfVertex);

        void Reset(NiUInt32 uiRegion);

        NiUInt32 GetRange(NiUInt32 uiRegion);
    };

    /**
        Interpolate an attribute of the mesh using barycentric coordinates
        along the surface of the triangle. 
        http://mathworld.wolfram.com/BarycentricCoordinates.html
        Any point in the plane V1V2V3 can be represented by the barycentric 
        coordinate (U,V). The parameter U controls how much V2 gets weighted 
        into the result, and the parameter V controls how much V3 gets weighted
        into the result. Lastly, the value of [1 - (U + V)] controls how much 
        V1 gets weighted into the result.
    */
    template <typename T>
    inline static T InterpolateTriangle(
        float fBarycentricU,
        float fBarycentricV, 
        unsigned int uiV1, 
        unsigned int uiV2,
        unsigned int uiV3, 
        NiTStridedRandomAccessIterator<T>& kIter);

    /**
        Interpolate an attribute of the mesh using barycentric coordinates
        along the surface of the triangle. 
        http://mathworld.wolfram.com/BarycentricCoordinates.html
        Any point in the plane V1V2V3 can be represented by the barycentric 
        coordinate (U,V). The parameter U controls how much V2 gets weighted 
        into the result, and the parameter V controls how much V3 gets weighted
        into the result. Lastly, the value of [1 - (U + V)] controls how much 
        V1 gets weighted into the result.
    */
    template <typename T>
    inline static T InterpolateTriangle(
        float fBarycentricU,
        float fBarycentricV, 
        const T& kV1,
        const T& kV2,
        const T& kV3);

    /// Returns true if any streams are shared between the two NiMesh objects.
    static bool SharesDataStreams(NiMesh* pkMeshA, NiMesh* pkMeshB);

    /**
        Welds all the vertices in the specified mesh if it can be welded. A 
        vertex is defined by a slice through all of the vertex data streams and
        can be welded if they are all equal. The index buffer on the mesh is 
        adjusted accordingly.
    */
    static void WeldVertices(NiMesh* pkMesh);

    /// Looks through all submeshes and removes any data from all vertex 
    /// streams that are not referenced by the index buffer. 
    /// @note This is a no-op for non-indexed mesh.
    static void RemoveUnusedVertices(NiMesh* pkMesh);

    /**
        Sorts the index buffers to optimize the mesh in a generic view
        independent way for better intra-object Z-buffer rejection using 
        three assumptions. First, larger triangles are more likely to occlude 
        smaller triangles and should come first in the mesh. Second, outward 
        facing triangles from the center of the mesh are more likely to 
        provide occlusion on concave meshes. Third, triangles further from 
        the center are more likely to occlude than those near the center. 
        Note, this only works on triangle lists.
     */
    static void ViewIndependentOptimization(NiMesh* pkMesh, 
        const float fSizeModifier, const float fNormalModifier,
        const float fPositionModifier);

    /// Rearranges the vertices to match the access order defined in the
    /// index buffer.  Note, this only works on idexed triangle lists.
    static void VertexCacheOptimization(NiMesh* pkMesh,
        const NiUInt32 uiVertexCacheSize);

    // Handles the sorting for pixel optimization.
    class WeightSort : public NiMemObject
    {
    public:

        /// Default constructor.
        WeightSort(const float fWeight = 0.0f, 
            const NiUInt32 uiIndex = 0xffffffff); 

        /// Destructor.
        inline virtual ~WeightSort() {}

        // Sorts the weights
        static void Sort(WeightSort* pkData, const NiUInt32 uiSize);

        float m_fWeight;
        NiUInt32 m_uiIndex;

    protected:
        static int CompareWeights(const void* pv0, const void* pv1);
    };

    /// Class to handle sorting for vertex cache optimization
    class VertexWeightData : public NiMemObject
    {  
    public:

        VertexWeightData();

        inline virtual ~VertexWeightData() {}

        NiUInt32 m_uiCacheIndex;
        NiUInt32 m_uiRefCount;
        NiUInt32 m_uiOldIndex;
        NiUInt32 m_uiNewIndex;
        float m_fWeight;
    };

    /// Class to handle sorting for vertex cache optimization
    class PrimitiveWeightData : public NiMemObject
    {  
    public:

        PrimitiveWeightData();

        inline virtual ~PrimitiveWeightData() {}

        inline float GetTotalWeight();

        bool m_bIsAdded;
        VertexWeightData* m_apVerts[3];
    };

    /// Converts all streams capture in the kCloneHelper to 
    /// the current platforms format.
    /// @note This method assumes a renderer exists
    static void ConvertStreamsToPlatformSpecific(
        NiToolPipelineCloneHelper& kCloneHelper);

    /// Converts a single stream to the current platforms format.
    /// The stream must be registered with the kCloneHelper.
    /// @note This method assumes a renderer exists
    static bool ConvertStreamToPlatformSpecific(
        NiToolPipelineCloneHelper& kCloneHelper, NiDataStream* pkDS);

    /**
        Compute normal sharing information for a mesh.
        
        This function finds
        vertices on the mesh that have the same normal, and places them in
        the same normal sharing group. Data streams are added to the mesh for
        the contents of each group (semantic NORMALSHAREGROUP, index 0) and
        the list of which group each vertex belongs to (semantic
        NORMALSHAREINDEX, index 0), unless the streams already exist. The
        streams are created with access CPU_READ and CPU_WRITE_STATIC, usage
        USAGE_USER and cloning set to share.

        Note that this function does not attempt to share the NORMALSHAREGROUP
        and NORMALSHAREINDEX streams among already existing clones of the
        mesh.

        Normal sharing is computed for all sub-meshes. Vertices used in
        different sub-meshes will not be found to share a normal vector, even
        if they share a common location and normal direction.

        Code that re-arranges vertices must-re-call this funtion to compute
        new normal sharing information, because the NORMALSHAREINDEX stream
        depends on the vertex ordering. Any NiDataStreamRef pointers
        for the mesh will be invalid when this function is called for
        the second or subsequent times. These streams could also be
        updated directly if the mapping from old to new vertex locations are
        available.
        
        The i'th entry in the NORMALSHAREINDEX stream is the location in
        the NORMALSHAREGROUP stream to look for information about the share
        group for the i'th vertex.
        
        The NORMALSHAREGROUP stream always has a 0 in the first location,
        which is used for vertices that do not share their normal with any
        other vertices. After that, there is a sequence of <count, vertex1,
        vertex2, ...> sets, each consisting of a count of the number of
        vertices that are in a group, followed by the indices for those
        vertices, followed by another count, followed by more indices, etc.
        
        @param pkMesh The mesh to compute normal sharing on.
        @param kPosnSemantic The semantic to use for position information.
        @param uiPosnIndex The semantic index for position information
        @param kNormSemantic The semantic to use for normal information.
        @param uiNormIndex The semantic index for normal information
    */
    static bool GenerateNormalSharing(NiMesh* pkMesh,
        const NiFixedString& kPosnSemantic, const NiUInt32 uiPosnIndex,
        const NiFixedString& kNormSemantic, const NiUInt32 uiNormIndex);

    /**
        Constructs a GPU-ready skinned mesh.

        Takes an existing mesh and breaks it into submeshes based on the input 
        number of bones per submesh. If any of the below assumptions are not 
        valid, the return value will be false.
        
        @param kCloneHelper The NiToolPipelineCloneHelper used for the scene.
        @param spCloneSet The set of mesh clones to partition.
        @param ucBonesPerSubmesh The maximum number of bones fitting inside a 
            single submesh partition.
        @return Whether or not the partitioning was completed.

        @note The number of bones per vertex will be 4.
        @note The NiMesh should exist and should have an NiSkinningMeshModifier
            on it.
        @note The NiMesh should be triangles, not tristrips or any other 
            variety of primitive.
        @note All NiDataStreams on the mesh must be lockable for TOOL_READ.
    */      
    static bool MakeSubmeshesForGPUSkinning(NiToolPipelineCloneHelper&
        kCloneHelper, NiToolPipelineCloneHelper::CloneSetPtr& spCloneSet, 
        unsigned char ucBonesPerSubmesh);

    /**
        Resizes an NiSkinningMeshModifier, mapping existing bones and 
        transforms to new indices.

        @param pkSkinMod The NiSkinningMeshModifier to remap.
        @param uiNewBoneCount The new number of bones.
        @param puiIndices An mapping of new indices to old indices.
            puiIndices should be of size uiNewBoneCount.

        @return Whether or not the remap was successful.  It could fail
            if puiIndices has values outside the range of 
            0..pkSkinMod->GetBoneCount()-1.
    */
    static bool RemapSkinningMeshModifier(NiSkinningMeshModifier* pkSkinMod,
        NiUInt32 uiNewBoneCount, NiUInt32* puiIndices);

protected:
    /// Function to return the weight per this vertex
    static float GetVertexCacheWeight(
        const VertexWeightData* pkData, 
        const NiUInt32 uiVertexCacheSize);

    /// Initializes the vertex cache
    static void InitVertexCache(
        const NiUInt32 uiPrimitiveCount,
        const PrimitiveWeightData* pkPrimitiveWeights,
        const NiUInt32 uiVertexCacheSize,
        NiTPrimitiveArray<VertexWeightData*>& kVertexCache);

    /// This function adds the primitive or triangle that has the best weight
    /// value associated with it. This primitive is the next one that will help
    /// reduce cache misses.
    static bool AddBestPrimitive(
        const NiUInt32 uiVertexCacheSize,
        const NiUInt32 uiVertexCount,
        const NiUInt32 uiPrimitiveCount,
        NiTPrimitiveArray<VertexWeightData*>& kVertexCache,
        VertexWeightData* pkVertexWeights,
        PrimitiveWeightData* pkPrimitiveWeights,
        NiTStridedRandomAccessIterator<NiUInt32>& kIndexIter,
        NiUInt32& uiVertexCounter,
        NiUInt32& uiPrimitiveCounter);
};

#include "NiMeshTools.inl"

#endif // NIMESHTOOLS_H
