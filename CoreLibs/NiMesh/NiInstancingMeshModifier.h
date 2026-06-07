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

#ifndef NIINSTANCINGMESHMODIFIER_H
#define NIINSTANCINGMESHMODIFIER_H
        
#include <NiStreamProcessor.h>
#include <NiTPool.h>
#include <NiTPtrSet.h>
#include <NiMeshHWInstance.h>

#include "NiMeshLibType.h"
#include "NiMeshModifier.h"
#include "NiFrustumPlanes.h"
#include "NiBound.h"
#include "NiPoint4.h"

class NiInstanceCullingKernel;

/**
    This mesh modifier provides per frame instancing capabilities in
    Gamebryo.
    
    Specificity, this mesh modifier is responsible for updating the 
    transform data stream of the instanced mesh, updating the bounding volume
    of the instanced mesh, and performing per instance culling of individual
    instances if requested.
*/
class NIMESH_ENTRY NiInstancingMeshModifier : public NiMeshModifier
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiInstancingMeshModifier);
    NiDeclareStream;
    NiDeclareViewerStrings;
    
    /// @endcond

public:
    /// Constructor.
    NiInstancingMeshModifier();

    /// Destructor.
    virtual ~NiInstancingMeshModifier();

    /// Returns whether per-instance culling is enabled.
    inline bool GetPerInstaceCulling();
    
    /// Sets the mesh modifier to use per-instance culling (false by default).
    ///
    /// This enables culling of individual instances.
    inline void SetPerInstanceCulling(bool bPerInstanceCulling);    
    
    /// Returns whether the bounds for the instanced mesh referenced by the
    /// mesh modifier is static.
    inline bool GetStaticBounds();
    
    /**
        Sets the bounding volume for the instanced mesh referenced by the mesh
        modifier to static (false by default).
    
        This eliminates the need for the mesh modifier to recalculate the
        bounds when a transforms change.
    */
    inline void SetStaticBounds(bool bStaticBounds);

    /// Returns whether instance nodes, NiMeshHWInstance, objects are being
    /// used to represent the transforms for the instances of the instanced
    /// mesh.
    inline bool AreInstanceNodesUsed();

    /**
        Flags the instancing node, NiMeshHWInstance, transforms as dirty.
    
        This will force the mesh modifier to update the transforms data
        stream with the transforms of the NiMeshHWInstance objects.
    */
    inline void MarkInstanceNodeTranformsDirty();
    
    /// Returns whether the instance node, NiMeshHWInstance, transforms are
    /// dirty.
    inline bool AreInstanceNodeTranformsDirty(); 

    /// Adds an NiMeshHWInstance object to the list of NiMeshHWInstance objects
    /// that are used to represent the transforms for the instances of the 
    /// instanced mesh.
    void AddMeshInstance(NiMeshHWInstance* pkHWInstance);
    
    /// Removes an NiMeshHWInstance object to the list of NiMeshHWInstance 
    /// objects that are used to represent the transforms for the instances of 
    /// the instanced mesh.
    void RemoveMeshInstance(NiMeshHWInstance* pkHWInstance);

    /**
        Manually sets the scale for the instances' bounding volumes. This scale
        represents the scale applied in an instance's transform. Since the
        scale is encoded in the transformation matrix the instance culling
        kernel needs to be explicitly provided the scale for each instance. 
        This scale will then be applied to the bounding volume when per
        instance culling is used. These scales will be automatically extracted
        if NiMeshHWInstance nodes are used. Otherwise they need to be manually
        set via this method. If they are not set a default scale of 1.0 will be
        used.

        @param pkBoundScales Pointer to an array of floats that represent the 
        scales to be set.
        @param uiScaleCount Number of scales that will be set. There
        must be at least a count of uiScaleCount scales available
        in the array referenced by pkBoundScales.
        @param uiInstanceOffset Offset, in instances, into the scale data
        stream that the pkBoundScales data will be copied too. The sum of
        uiScaleCount and uiInstanceOffset can not be greater than the
        maximum number of instances.
        @return Whether or not the scale were successfully set.
    */
    bool SetInstanceBoundScale(
        float* pkBoundScales, 
        NiUInt32 uiScaleCount, 
        NiUInt32 uiInstanceOffset = 0);

    /// This method checks if the mesh is suitable for instancing.
    ///
    /// If it returns false, the modifier may be removed from the mesh.
    virtual bool AreRequirementsMet(NiMesh* pkMesh, 
        NiSystemDesc::RendererID ePlatform) const;

    /// Retrieves the requirements from the mesh modifier.
    virtual void RetrieveRequirements(NiMeshRequirements& kRequirements) const;

    /**
        RequiresMaterialOption may be used by materials to determine 
        whether or not they are expected to do some work. 
        
        @return True if the kMaterialOption is
            NiRenderObjectMaterialOption::TRANSFORM_INSTANCED(), otherwise
            false.
    */
    virtual bool RequiresMaterialOption(const NiFixedString& kMaterialOption, 
        bool& bResult) const;

    /// Returns the mesh affected by the modifier.
    inline NiMesh* GetAffectedMesh();

    /// @cond EMERGENT_INTERNAL

    /// @name Functions for updating and managing parallel tasks.
    /// @{
    /**
        Creates Floodgate tasks required for instance culling.
        Streams from the mesh are used as the source of Floodgate streams.
    */
    virtual bool Attach(NiMesh* pkMesh);
    
    /**
        Removes all Floodgate tasks and references to streams on the mesh.
    */
    virtual bool Detach(NiMesh* pkMesh);

    /**
        Performs per-frame modifier work.
        
        This function gathers the instancing transforms and computes the
        bound, if required. If per-instance culling is enabled, it adds
        Floodgate tasks to support instancing.
    */
    virtual bool SubmitTasks(NiMesh* pkMesh, NiSyncArgs* pkArgs,
        NiSPWorkflowManager* pkWFManager);
        
    /**
        Forces completion of outstanding Floodgate tasks that match the
        requested sync points.
    */
    virtual bool CompleteTasks(NiMesh* pkMesh, NiSyncArgs* pkArgs);
    /// @}
    
    /// Selective update control.
    virtual void SetSelectiveUpdateFlags(bool& bSelectiveUpdate,
        bool& bSelectiveUpdateTransforms, bool& bRigid);

    /// Cloning methods.
    virtual void ProcessClone(NiCloningProcess& kCloning);

    /// Post-link method
    virtual void PostLinkObject(NiStream& kStream);

    /// @endcond

protected:
    /// Updates the transform data stream with the transforms of the referenced
    /// NiMeshHWInstance objects.
    void GatherInstanceNodeTransforms(NiMesh* pkMesh, 
        bool bUpdateWorldBounds);

    /// Sets the base instance stream and visible instance stream cache on
    /// the mesh.
    void SetMeshInstanceCache();

    /// The mesh affected by the mesh modifier.
    NiMesh* m_pkAffectedMesh;

    /// Array of all the NiMeshHWInstance objects that are used to represent
    /// the transforms for the instances of the instanced mesh.
    NiTLargePrimitiveArray<NiMeshHWInstance*> m_kInstanceNodes;

    /// The instance culling kernel used to perform per instance culling via
    /// Floodgate.
    NiInstanceCullingKernel* m_pkKernel;
    
    /// The individual task used to execute per instance culling with the 
    /// NiInstanceCullingKernel.
    NiSPTaskPtr m_spTask;
    
    /// Workflow that contains the entire per instance culling operation.
    NiSPWorkflowPtr m_spWorkflow;
    
    /// This data stream represents the primary transforms list. It contains 
    /// the transforms for all of the active instances, when per instance 
    /// culling is enabled.
    NiTSPStream<NiPoint4> m_kInSPStream;

    /// This data stream provides the scale for bounding volume of each
    /// instance to the instance culling kernel. 
    NiTSPStream<NiPoint4> m_kInScaleSPStream;
    
    /// This is the data stream that will contain the resulting instance 
    /// transforms to be used by the GPU, when per instance culling is enabled.
    NiTSPStream<NiPoint4> m_kOutSPStream; 
#ifndef  _PS3
    /// A dummy data stream to be used as a temporary storage location for 
    /// kernels to store the visible instances before the results are copied to
    /// the final output data stream.
    NiTSPStream<NiPoint4> m_kDummyOutSPStream;
#endif
    
    /// World transform input.
    NiTSPFixedInput<NiTransform> m_kWorldTransformStream;
    
    /// Local bounds input.
    NiTSPFixedInput<NiBound> m_kLocalBoundStream;
    
    /// Frustum planes input.
    NiTSPFixedInput<NiFrustumPlanes> m_kFrustumPlanesStream;
    
    /// Output effective address.
    NiTSPFixedInput<NiUInt32> m_kOutputStreamEA;
    
    /// Used by the kernel to allocate a section of the output stream.    
    NiTSPFixedInput<NiUInt32> m_kVisibleArrayIndexStream;

    /// Dummy output data used as a temporary storage location for kernels to 
    /// store the visible instances before the results are copied to the final 
    /// output data stream.
    NiPoint4* m_pkDummyOutData;

    /// Pointer to an float array used to represent the bounding volume scale
    /// for each instance. 
    float* m_afScale;
    
    /// Number of elements in the dummy output data.
    NiUInt32 m_pkDummyOutElementCount;
   
    /// Number of instances reported visible by the NiInstanceCullingKernel.
    NiUInt32 m_uiVisibleOutputIndex;

    /// The planes of the active view frustum.       
    NiPlane m_kPlanes[6];
    
    /// Bounding volume of the original mesh.
    NiBound m_kBound;   

    /// Indicates instanced nodes, NiMeshHWInstance object, are used.
    bool m_bInstanceNodes;
    
    /// Indicates the instance nodes, NiMeshHWInstance, are dirty.
    bool m_bDirtyNodeTransforms;
    
    /// Indicates per instance culling is to be used.
    bool m_bPerInstanceCulling;
    
    /// Indicates that the bounding volume of the instanced mesh is static.
    bool m_bStaticBounds;
};

#include "NiInstancingMeshModifier.inl"

#endif // #ifndef NIINSTANCINGMESHMODIFIER_H
