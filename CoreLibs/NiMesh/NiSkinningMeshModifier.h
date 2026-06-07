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

#ifndef NISKINNINGMESHMODIFIER_H
#define NISKINNINGMESHMODIFIER_H

#include "NiMeshModifier.h"
#include "NiMatrix3x4.h"
#include <NiTPtrSet.h>
#include <NiTransform.h>
#include <NiSPStream.h>
#include <NiAVObject.h>
#include <NiDataStream.h>
#include <NiSPTask.h>
#include <NiBound.h>
#include <NiBool.h>

#if defined(_WII)
#include "NiWiiSkinningKernelData.h"
#else
#include "NiSkinningKernelData.h"
#endif

class NiCalculateBoneMatricesKernel;
class NiMesh;
struct NiCBMKernelData;

/**
    This mesh modifer provides the functionality needed for both software
    (CPU/SPU) skinning and hardware (GPU) skinning. 

    This mesh modifier uses two kernels for skinning: 
    CalculateBoneMatrices and Deform. CalculateBoneMatrices gathers the 
    transforms for bones defined by other NiAVObjects in the scene graph, 
    determines the delta transform from the pose the mesh was bound to, and 
    then transforms the results by the current mesh transform. The output 
    matrices from this process are then either fed to the GPU as shader 
    constants in the hardware case or used as arguments to the Deform kernel
    in the software case. Deform will take the mesh vertices and the computed 
    bone transforms and will "skin" the mesh. Deform is only invoked in the
    software skinning case.

    @note Since shader constants are limited on GPU hardware, the number of  
    bones influencing a mesh in a given draw call is also limited. Please use
    the methods in NiSkinningUtilities to break the mesh into sub-meshes that 
    are more GPU-friendly.
    
    For an overview of skinning, please see the 
    "Programming Skinnned Objects" section in "General Topics" of the "Programmer's Guide".    
*/
class NIMESH_ENTRY NiSkinningMeshModifier : public NiMeshModifier
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiSkinningMeshModifier);
    NiDeclareStream;
    NiDeclareViewerStrings;
    NiDeclareFlags(unsigned short);

    /// @endcond

public:

    /// Constructs a new NiSkinningMeshModifier for the given number of bones
    NiSkinningMeshModifier(NiUInt32 uiBoneCount);

    /// Destructor.
    virtual ~NiSkinningMeshModifier();

    /// Returns the number of bones influencing the mesh.
    inline NiUInt32 GetBoneCount() const;

    /// Gets skinning mode (hardware / software)
    inline bool GetSoftwareSkinned() const;
    
    /**
        Sets the skinning mode (hardware / software)
    
        @param bUseSoftwareSkinning Indicates if software skinning should be
            used. Pass true to use software (CPU/SPU) skinning; false to
            use GPU skinning.

        @note If modified after Attach() has been called, the mesh modifier 
        will need to be reset.
    */
    inline void SetSoftwareSkinned(bool bUseSoftwareSkinning);

    /// Gets the world-space bounds for each bone. This method returns NULL if
    /// model bound is static.
    inline NiBound* GetBoneBounds() const;

    /// Enable dynamic bounds updating (does not immediately initialize bone 
    /// bounds).
    void EnableDynamicBounds();
    
    /// Disable dynamic bounds updating (does not immediately initialize bone 
    /// bounds)
    void DisableDynamicBounds();

    /// Set the root bone parent node
    inline void SetRootBoneParent(NiAVObject* pkRootBone);
    
    /// Gets the root bone parent node
    inline NiAVObject* GetRootBoneParent() const;

    /// Sets the root bone parent to skin transform
    inline void SetRootBoneParentToSkinTransform(const NiTransform& pkRoot);
    
    /// Gets the root bone parent to skin transform
    inline NiTransform& GetRootBoneParentToSkinTransform();

    /// Gets the world to skin transform
    inline void GetWorldToSkinTransform(NiTransform& kWorldToSkin) const;

    /// Gets the root bones
    void GetRootBones(NiTPointerList<NiAVObject*>& kRootBones) const;

    /// Gets the set of bones
    inline NiAVObject** GetBones() const;

    /// Gets the set of skin to bone transforms
    inline NiTransform* GetSkinToBoneTransforms() const;

    /// Gets the bone matrices (returns NULL if called before Attach())
    inline NiMatrix3x4* GetBoneMatrices() const;

    /**
        Deforms a single vertex. This method is useful when needing to skin a 
        small set of vertices in a frame. It requires that the BLENDWEIGHTS,
        BLENDINDICES, BONES, POSITION, and NORMAL data streams are able to
        be locked for CPU_READ access.

        @param kVertex The final skinned position in world space.
        @param kNormal The final skinned normal in world space.
        @param pkMesh The mesh containing the skinned data streams and also
            has this modifier attached.
        @param uiWhichSubmesh The sub-mesh to use when skinning.
        @param uiWhichVertex The index of the vertex to skin within the 
            specified submesh.
    */
    void DeformVertex(
        NiPoint3& kVertex,
        NiPoint3& kNormal,
        NiMesh* pkMesh,
        NiUInt32 uiWhichSubmesh,
        NiUInt32 uiWhichVertex);

    /// This method checks, after finalization, if the mesh is suitable for
    /// skinning. If it returns false, the modifier may be removed
    /// from the mesh.
    virtual bool AreRequirementsMet(NiMesh* pkMesh,
        NiSystemDesc::RendererID ePlatform) const;

    /// This method is called during the mesh data stream packing process.
    /// The requirements are used as a guide only - the packer is not
    /// guaranteed to achieve them.
    virtual void RetrieveRequirements(NiMeshRequirements& kRequirements) const;

    /// @name Functions for updating and managing parallel tasks.
    /// @{

    /// Creates Floodgate tasks required for skinning (exactly which tasks
    /// depend on CPU vs GPU skinning). Streams from the mesh are used
    /// as the source of Floodgate streams.
    virtual bool Attach(NiMesh* pkMesh);
    
    /// Removes all Floodgate tasks and references to streams on the mesh.
    virtual bool Detach(NiMesh* pkMesh);

    /// Adds Floodgate tasks to support skinning. A task to update bone
    /// matrices is always added, while CPU skinning also adds a task to
    /// perform the skinning.
    virtual bool SubmitTasks(NiMesh* pkMesh, NiSyncArgs* pkArgs,
        NiSPWorkflowManager* pkWFManager);

    /// Forces completion of outstanding Floodgate tasks that match the
    /// requested sync points.
    virtual bool CompleteTasks(NiMesh* pkMesh, NiSyncArgs* pkArgs);
    /// @}

    /**
        Determine if the modifier supports a specific material option.

        Returns true only if the option is 
        NiRenderObjectMaterialOption::TRANSFORM_SKINNED(). If the option 
        matches, bResult is set to indicate whether material should perform 
        the skinning deformation. If hardware skinning is enabled, bResult is
        set to true; if software skinning is enabled, bResult is set to false.
    */
    virtual bool RequiresMaterialOption(const NiFixedString& kMaterialOption,
        bool& bResult) const;

    /// @cond EMERGENT_INTERNAL

    /// Selective update control
    virtual void SetSelectiveUpdateFlags(bool& bSelectiveUpdate,
        bool& bSelectiveUpdateTransforms, bool& bRigid);

    /// Should not be called after Attach() is called, does not preserve
    /// the contents of the bones or transforms arrays
    void Resize(NiUInt32 uiNewBoneCount);

    /// cloning
    virtual void ProcessClone(NiCloningProcess& kCloning);

    /// Get the world matrices of the bones
    const NiTransform& GetWorldBoneMatrix(NiUInt32 uiBone) const;

    /// @endcond

protected:
    // flags
    enum
    {
        /// Software skinning will be used to deform the mesh
        USE_SOFTWARE_SKINNING  = 0x0001,
        /// Mesh bounds will be recompute after deformation
        RECOMPUTE_BOUNDS = 0x0002
    };

    /// Number of bones
    NiUInt32 m_uiBoneCount;

    /// The NiAVObject that represents the root bone of the skinned model
    /// Bone pointers are regular pointers rather than smart pointers to avoid
    /// reference cycles that arise when skin is a descendant of the root bone.
    NiAVObject* m_pkRootBoneParent;

    /// Root bone to skin space transform
    NiTransform m_kRootBoneParentToSkinTransform;

    /**
        The NiAVObjects's Transforms that represent the world position of each 
        bone affecting the skinned object. Bone pointers are regular pointers
        rather than smart pointers to avoid reference cycles that arise when a 
        skin is a descendant of the root bone.
    */
    NiAVObject** m_apkBones;
    
    /// The NiAVObject's transforms that represent the world position of each
    /// bone affecting the skinned object
    NiTransform** m_ppkBoneWorlds;

    /// The SkinToBone transforms for each bone
    NiTransform* m_pkSkinToBoneTransforms;

    /// Array of bone matrices
    NiMatrix3x4* m_pkBoneMatrices;

    /// World-space bounds per bone
    NiBound* m_pkBoneBounds;

    /*
        Track whether bones are dirty so that software deformation doesn't
        happen if an object is rendered again without any bone updates
        (for instance, if the object is rendered once to a shadow map prior
        to being rendered to the main scene).
    */
    bool m_bBonesDirty;

    /// Skinning deformation task
    NiSPTaskPtr m_spDeformTask;

    /// Skinning deformation task
    NiSPWorkflowPtr m_spDeformWorkflow;

    /// Bone Matrix fixed input stream
    NiTSPFixedInput<NiMatrix3x4> m_kBoneMatrixStream;

#if defined(_WII)
    /// Extra data used by the kernel for skinning
    NiWiiSkinningKernelData m_kWiiSkinningKernelData;

    /// SkinningKernel extra data input stream
    NiTSPFixedInput<NiWiiSkinningKernelData> 
        m_kWiiSkinningKernelDataStream;

    /// Bind pose input streams for (P); (PN); (P),(N); (PNBT); & (P),(NBT)
    NiSPStream m_kBindPoseStreams[2];

    /// Blend weights input stream
    NiSPStream m_kBlendWeightStream;
    
    /// Blend indices input stream
    NiSPStream m_kBlendIndiciesStream;

    /// Output streams for (P); (PN); (P),(N); (PNBT); & (P),(NBT)
    NiSPStream m_kOutputStreams[2];

#else
    /// Extra data used by the kernel for skinning
    NiSkinningKernelData m_kSkinningKernelData;

    /// SkinningKernel extra data input stream
    NiTSPFixedInput<NiSkinningKernelData> m_kSkinningKernelDataStream;

    /// Bind pose positions input stream
    NiSPStream m_kBPPositionsStream;
    
    /// Normals input stream
    NiSPStream m_kBPNormalsStream;
    
    /// Binormals input stream
    NiSPStream m_kBPBinormalsStream;
    
    /// Tangents input stream
    NiSPStream m_kBPTangentsStream;
    
    /// Blend weights input stream
    NiSPStream m_kBlendWeightStream;
    
    /// Blend indices input stream
    NiSPStream m_kBlendIndiciesStream;
    
    /// Positions output stream
    NiSPStream m_kPositionsStream;
    
    /// Normals output stream
    NiSPStream m_kNormalsStream;
    
    /// Binormals output stream
    NiSPStream m_kBinormalsStream;
    
    /// Tangents output stream
    NiSPStream m_kTangentsStream;
#endif

    /// Calculate bone matrices kernel
    NiCalculateBoneMatricesKernel* m_pkCBMKernel;

    /// Calculate bone matrices task
    NiSPTaskPtr m_spCBMTask;

    /// Calculate bone matrices workflow
    NiSPWorkflowPtr m_spCBMWorkflow;

    /// Extra data used by the kernel for computing bone matrices
    NiCBMKernelData* m_pkCBMKernelData;

    /// CBMKernel extra data input stream
    NiTSPFixedInput<NiCBMKernelData> m_kCBMKernelDataStream;
    
    /// Bone to World transforms input stream
    NiTSPStream<NiTransform**> m_kBoneWorldStream;
    
    /// Skin to Bone transforms input stream
    NiTSPStream<NiTransform> m_kSkinToBoneTransformsStream;
    
    /// Bone matrix output streams
    NiTSPStream<NiMatrix3x4> m_kBoneMatrixOutputStream;
    
    /// Stores the most recent time for which SubmitTasks was called to
    /// do SYNC_UPDATE or SYNC_POST_UPDATE task submission.
    float m_fUpdateTime;

    /// Constructor. for use with streaming and cloning.
    NiSkinningMeshModifier();

    /// Allocates memory for bones, transforms, etc...
    void Initialize(NiUInt32 uiBoneCount);

    /// Updates the model bounds
    void UpdateModelBounds(NiMesh* pkMesh);

    /// Prepares the software deform task
    bool AttachDeformTask(NiMesh* pkMesh);

    /// Prepares the calculate bone matrices task
    bool AttachCalcBoneMatricesTask(NiMesh* pkMesh);
};

#include "NiSkinningMeshModifier.inl"

#endif // #ifndef NISKINNINGMESHMODIFIER_H
