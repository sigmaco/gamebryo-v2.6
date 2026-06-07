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

#ifndef NIINSTANCINGUTILITIES_H
#define NIINSTANCINGUTILITIES_H

#include <NiRenderObject.h>
#include <NiMemObject.h>
#include <NiPoint4.h>
#include "NiMeshLibType.h"
#include "NiDataStreamElement.h"

class NiDataStream;
class NiDataStreamRef;
class NiMesh;

/**
    This class encapsulates the majority of the functions required to work
    with hardware instanced meshes.
    
    All methods in this class are static
    utility functions, so there is no need to instantiate an 
    NiInstancingUtilities object.
*/
class NIMESH_ENTRY NiInstancingUtilities : public NiMemObject
{
public:
    /**
        Enables mesh instancing on the provided mesh.
        
        This method will add the
        required transform data streams, attach the NiInstancingMeshModifier,
        configure the required submesh regions, and duplicate index data if
        required in order to allow the provided mesh to support hardware
        instancing.
        
        @param pkMesh The mesh to have instancing support added.
        @param uiMaxTransforms The maximum number of instances, or transforms,
            that the instanced mesh will be able to support. This value cannot
            change after instancing has been enabled on the mesh.
        @param pkTransforms A pointer to an array of NiTransform objects that
            the instance transforms should be initialized too. This array must
            contain at least a count of uiActiveTransforms valid transform
            objects. If this value is NULL all transforms will be assigned the
            identity transform.
        @param uiActiveTransforms The number of instances, or transforms, that
            will be active and visible.
        @param uiMaxIndicesPerSubmesh The maximum number of indices that will
            be allowed to be referenced per submesh. This value is used to 
            mitigate the amount of index data duplicate that is required for
            instancing support for some platforms. The number of indices that 
            can exist will be limited to this value. Decreasing this parameter
            will result in less data duplication on some platforms, but require
            more submeshes, hence additional render calls.
        @param bCullPerInstance Specifies whether per instance culling should
            be enabled for the instanced mesh.
        @param bStaticBounds Specifies whether the bounding volume that
            encapsulates all the instances should be static or dynamic.
        @param bCPURead Specifies whether the transform data stream should be
            created with CPU_READ flag. Note that some options, such as per
            instance culling, require this and will force CPU_READ regardless
            of what this flag is set too.
        @param pvIndexData Pointer to either a NiUInt32 or a NiUInt16 array
            containing the index data for the mesh. This is only required if
            the current platform requires index data duplication and the index
            data in the mesh is not flagged with CPU_READ.
        @param uiIndexCount Number of index values referenced by the 
            pvIndexData pointer. This is only required if the current platform
            requires index data duplication and the index data in the mesh is 
            not flagged with CPU_READ.
        @param eIndexFormat Specifies the format of the index data referenced
            by the pvIndexData pointer. This is only required if the current 
            platform requires index data duplication and the index data in the 
            mesh is not flagged with CPU_READ.
    */
    static bool EnableMeshInstancing(
        NiMesh* pkMesh, 
        NiUInt32 uiMaxTransforms, 
        NiTransform* pkTransforms = NULL, 
        NiUInt32 uiActiveTransforms = 0, 
        NiUInt32 uiMaxIndicesPerSubmesh = 65536, 
        bool bCullPerInstance = true, 
        bool bStaticBounds = false, 
        bool bCPURead = true,
        void* pvIndexData = NULL, 
        NiUInt32 uiIndexCount = 0,  
        NiDataStreamElement::Format eIndexFormat = 
            NiDataStreamElement::F_UINT32_1);

    /**
        Disables mesh instancing on an instanced mesh.
        
        This includes removing the transform data streams and the instancing
        mesh modifier from the mesh. Note that this will not delete any
        existing NiMeshHWInstance objects that reference this mesh.
    */
    static bool DisableMeshInstancing(NiMesh* pkMesh);

    /**
        Manually sets the transforms for instances of an instanced mesh.
        
        @param pkMesh Pointer to the instanced mesh.
        @param pkTransforms Pointer to an array of NiTransform objects that
            represent the transforms to be set.
        @param uiTransformCount Number of transforms that will be set. There
            must be at least a count of uiTransformCount transforms available
            in the array referenced by pkTransforms.
        @param uiInstanceOffset Offset, in instances, into the transform data
            stream that the pkTransforms data will be copied too. The sum of
            uiTransformCount and uiInstanceOffset can not be greater than the
            maximum number of instances.
        @return Whether or not the transforms were successfully set.
    */
    static bool SetInstanceTransformations(
        NiMesh* pkMesh, 
        NiTransform* pkTransforms, 
        NiUInt32 uiTransformCount, 
        NiUInt32 uiInstanceOffset = 0);

    /**
        Computes the world bound for an instanced mesh.
        
        This requires locking the transform data stream and iterating over
        every transform.
    */
    static bool ComputeWorldBound(NiMesh* pkMesh);

    /// Helper method to pack a NiTransform object into a NiPoint4 object in
    /// format that is easily consumable by the graphics card.
    static void PackTransform(const NiTransform& kSourceTransform,
        NiPoint4* pkDest);

    /**
        Attaches a NiMeshHWInstance object to the specified instanced mesh.

        Attaching a NiMeshHWInstance object to an instanced mesh will switch
        the mesh from working with manually provided transforms to only
        using transforms provided via NiMeshHWInstance objects.
    */
    static bool AddMeshInstance(NiMesh* pkMesh, 
        NiMeshHWInstance* pkHWInstance);

    /**
        Detaches the provided NiMeshHWInstance object from the instanced mesh.
    
        The instanced mesh will no longer reference the NiMeshHWInstance 
        object. This operation will not destroy the provided NiMeshHWInstance
        object.
    */
    static bool RemoveMeshInstance(NiMesh* pkMesh,
        NiMeshHWInstance* pkHWInstance);

    /// Returns the number of active instances on the instanced mesh.
    static const NiUInt32 GetActiveInstanceCount(const NiMesh* pkMesh);
    
    /// Returns the number of visible instances of the instanced mesh.
    static const NiUInt32 GetVisibleInstanceCount(const NiMesh* pkMesh);
    
    /// Returns the maximum number of instances that can be assigned to the 
    /// instanced mesh.
    static const NiUInt32 GetMaxInstanceCount(const NiMesh* pkMesh);
    
    /// Returns the maximum number of transforms that can be used per submesh
    /// for the provided instanced mesh.
    static const NiUInt32 GetTransformsPerSubmesh(const NiMesh* pkMesh);
    
    /// Returns the number of visible submeshes for the provided instanced 
    /// mesh.
    static const NiUInt32 GetVisibleSubmeshCount(const NiMesh* pkMesh);

    /// Gets the rate at which the indices should repeat for an 
    /// instanced mesh.
    static const NiUInt32 GetInstanceIndexFrequency(const NiMesh* pkMesh, 
        NiUInt32 uiSubmeshIdx);

    /// Gets the rate at which the indices should repeat for an 
    /// instanced submesh using the pre-specified stream reference.
    static const NiUInt32 GetInstanceIndexFrequency(const NiMesh* pkMesh, 
        NiUInt32 uiSubmeshIdx, const NiDataStreamRef* pkIndexBufferRef);

    /// Gets the total number of indices for all active instances on an 
    /// instanced submesh.
    static const NiUInt32 GetInstanceIndexCount(const NiMesh* pkMesh, 
        NiUInt32 uiSubmeshIdx);

    /// Gets the total number of indices for all active instances on an 
    /// instanced submesh using the pre-specified stream reference.
    static const NiUInt32 GetInstanceIndexCount(const NiMesh* pkMesh, 
        NiUInt32 uiSubmeshIdx, const NiDataStreamRef* pkIndexBufferRef);

    /// Sets the active instance count on the instanced mesh provided.
    static void SetActiveInstanceCount(NiMesh* pkMesh, 
        NiUInt32 uiActiveCount);

    /// Determines if the current platform requires the index data to be
    /// duplicated for instancing support.
    static bool InstancingIndexDuplicationRequired();
    
    /// @cond EMERGENT_INTERNAL

    /// Creates a GPU stream to store the results of the InstanceCullingKernel.
    static NiDataStreamRef* CreateCollectorDataStream(
        NiMesh* pkMesh, NiUInt32 uiMaxDuplications);

    /// Creates all the required submesh regions and populates their data. This
    /// is used to create and configure all the submeshes needed to render
    /// uiMaxTransforms worth of instances.
    static void SetupTransformStreamRegions(
        NiMesh* pkMesh, NiFixedString kTransformSemantic,
        NiUInt32 uiSubmeshCount, NiUInt32 uiTransformsPerSubmesh,
        NiUInt32 uiMaxTransforms);

    /// Duplicates index data. This is required by some platforms for proper
    /// instancing support.
    static bool CreateDuplicateIndexDataStream(
        NiMesh* pkMesh, 
        NiDataStreamRef* pkIndexStreamRef, 
        NiUInt32 uiTotalIndexCount,
        NiUInt32 uiTransformsPerSubmesh,
        NiUInt32 uiMaxTransforms,
        void* pvIndexData = NULL, 
        NiUInt32 uiIndexCount = 0,
        NiDataStreamElement::Format eInIndexFormat = 
        NiDataStreamElement::F_UINT32_1);

    /// Returns the total number of indices referenced by all the regions
    /// in the provided index data stream.
    static NiUInt32 GetTotalIndexCount(NiDataStream* pkIndexDataStream);

    /// Returns the total number of indices referenced by all the regions
    /// in the provided index data stream.
    static NiUInt32 GetTotalIndexCount(const NiDataStream* pkIndexDataStream);
    
    /// Used to ensure the instancing data stored in an instanced mesh will 
    /// work properly with the current platform. If the provided instancing
    /// data is not compatible with the current platform, this method will
    /// make the needed changes to the data to ensure the instanced mesh will
    /// work on the current platform.
    static void ValidateInstancingData(NiMesh* pkMesh);    
    
    /// Sets the visible instance count on the instanced mesh provided.
    static void SetVisibleInstanceCount(NiMesh* pkMesh,
        NiUInt32 uiVisibleCount);

    /// @endcond

protected:

    enum
    {
        // Maximum number of indicies there are ever allowed in a submesh 
        // for instanced meshes.
        MAX_INDICIES_PER_SUBMESH  = 524288
    };

    /// Create a transform stream for the instanced mesh and populates it
    /// with the provided transform data.
    static bool CreateInstanceTransformStream(
        NiMesh* pkMesh,
        NiUInt32 uiFrequency, 
        NiUInt32 uiMaxDuplications, 
        NiUInt32 uiActiveDuplications, 
        NiTransform* pkTransforms,
        bool bCPURead, 
        bool bCullPerInstance);

    /// Returns a pointer to an array that contains all index data for the 
    /// provided mesh.
    static void* GetAllIndexData(NiMesh* pkMesh, 
        NiUInt32 uiTotalIndexCount);

    /// Duplicates index data. This is required by some platforms for proper
    /// instancing support.
    static bool DuplicateIndexData(
        void* pvSourceData, 
        void* pvDestData, 
        const NiDataStream* kSourceDataStream, 
        NiDataStreamElement::Format eInFormat, 
        NiDataStreamElement::Format eOutFormat,
        NiUInt32 uiFrequency,
        NiUInt32 uiMaxDuplications,
        NiUInt32 ePrimitiveType);

    /// Destroys all the duplicated index data in the instanced mesh. The
    /// original index data will remain untouched. 
    static bool DestroyDuplicatedIndexData(NiMesh* pkMesh, 
        NiDataStreamRef* pkIndexStreamRef);
};

#include "NiInstancingUtilities.inl"

#endif // NIINSTANCINGUTILITIES_H
