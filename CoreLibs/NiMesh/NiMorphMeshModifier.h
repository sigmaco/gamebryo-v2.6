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

#ifndef NIMORPHMESHMODIFIER_H
#define NIMORPHMESHMODIFIER_H

#include <NiStreamProcessor.h>
#include <NiSPStream.h>

#include "NiMeshLibType.h"
#include "NiMeshModifier.h"
#include "NiMorphingKernelData.h"

class NiMorphingKernel;

/**
    The NiMorphMeshModifier class provides the ability to morph streams of
    data from one data set to another. Specifically, it performs
    linear-weighted blending between a set of target data streams. The current
    implementation assumes that the data stream format is F_FLOAT32_3 or 
    F_FLOAT16_4.

    If multiple semantics are to be morphed, it is assumed that all semantics
    have the same number of morph targets and that the same weights are
    used for all semantics.

    The target data are assumed to be present as streams on the mesh that
    this modifier is attached to, named as determined by 
    MorphElementSemantic(). The target vertices can be interpreted as either 
    a set of independent targets or as a set of offsets from a base target. 
    The first target stream is always assumed to be the base mesh for all of 
    the offset targets. The target data streams must have ACCESS_CPU_READ.

    The destination data stream(s) must be present and have CPU_WRITE_VOLATILE
    or CPU_WRITE_MUTABLE access.
    
    The weights are taken from another stream on the mesh, with semantic
    MORPHWEIGHTS. This stream must be present for the modifier to successfully
    attach to the mesh. There must be at least as many weights as there are
    morph targets. Typically, the mesh will have an NiMorphWeightsController
    object attached to control the weights. That class is found in the
    NiAnimation library.

    In addition, the following assumptions are made: The data format must be 
    F_FLOAT32_3 or F_FLOAT16_4; data streams must have only one region.

    @ps3_begin On the PS3, each of the morphed elements must be in its
    own single-element data stream. Since output streams are not DMA'd to
    the SPUs by Floodgate, interleaved data would otherwise be lost.@ps3_end
*/
class NIMESH_ENTRY NiMorphMeshModifier : public NiMeshModifier
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiMorphMeshModifier);
    NiDeclareStream;
    NiDeclareViewerStrings;
    NiDeclareFlags(NiUInt8);

    /// @endcond

public:
    /// Constructor. The number of morph targets can be specified. If not
    /// given, it must be set before the modifier is attached to a mesh.
    NiMorphMeshModifier(NiUInt16 uiNumTargets = 0);

    /// Destructor.
    virtual ~NiMorphMeshModifier();
    
    /// Get and set the number of morph targets.
    //@{
    inline NiUInt16 GetNumTargets() const;
    inline void SetNumTargets(const NiUInt16 uiNumTargets);
    //@}
    
    /// Specify a semantic to be morphed.
    NiUInt32 AddMorphedElement(const NiFixedString& kSemantic, 
        NiUInt32 uiSemanticIdx);

    /**
        Determine if an element will be morphed.
        
        @param kSemantic The semantic sought.
        @param uiSemanticIdx The semantic index for the sought semantic.
        @param uiIndexReturn On return, holds the internal index for the
            semantic, which is the index to be used with GetMorphedElement
            to retrieve this semantic.
    */
    bool IsMorphedElement(const NiFixedString& kSemantic, 
        NiUInt32 uiSemanticIdx, NiUInt32& uiIndexReturn) const;

    /// Get the element that will be morphed at the specified index of 
    /// internal morph targets.
    bool GetMorphedElement(const NiUInt32 uiIndex, NiFixedString& kSemantic, 
        NiUInt32& uiSemanticIdx);
   
    /// This method checks, after finalization, if the mesh is suitable for
    /// morphing. If it returns false, the modifier may be removed
    /// from the mesh.
    virtual bool AreRequirementsMet(NiMesh* pkMesh,
        NiSystemDesc::RendererID eRenderer) const;

    /// Returns NiDataStreamRef semantic to use for morph targets for the given
    /// element.
    static NiFixedString MorphElementSemantic(const NiFixedString& kSemantic);

    /// This method is called during the mesh data stream packing process.
    /// The requirements are used as a guide only - the packer is not
    /// guaranteed to achieve them.
    virtual void RetrieveRequirements(NiMeshRequirements& kRequirements) const;

    /// @name Functions for updating and managing parallel tasks.
    /// @{
    /**
        Creates Floodgate tasks required for morphing. Streams from the
        mesh are used as the source of Floodgate streams.
    */
    virtual bool Attach(NiMesh* pkMesh);
    
    /**
        Removes all Floodgate tasks and references to streams on the mesh.
    */
    virtual bool Detach(NiMesh* pkMesh);

    /**
        Submits Floodgate tasks to support morphing, unless the morph is
        inactive.
    */
    virtual bool SubmitTasks(NiMesh* pkMesh, NiSyncArgs* pkArgs,
        NiSPWorkflowManager* pkWFManager);
        
    /**
        Forces completion of outstanding Floodgate tasks that match the
        requested sync points. Also does normal recalculation if required.
    */
    virtual bool CompleteTasks(NiMesh* pkMesh, NiSyncArgs* pkArgs);
    /// @}

    /// Calculate morph bound by combining bounds of all targets. Returns
    /// true if the bound could be computed, false otherwise. MUST be called
    /// explicitly by tools or the application.
    bool CalculateMorphBound(NiMesh* pkMesh);

    /// Get the flag controlling always update. If always update is
    /// set, the morph will always be computed even if the object is not
    /// visible or the morph weights have not changed.
    inline bool GetAlwaysUpdate() const;

    /// Set the flag controlling always update.
    inline void SetAlwaysUpdate(const bool bValue);

    /**
        Get the flag controlling the interpretation of target
        vertices. If true, the first morph target is considered a base
        mesh with remaining targets providing incremental offsets from the
        base. If false, all targets are absolute.
    **/
    inline bool GetRelativeTargets() const;

    /// Set the flag controlling the interpretation of target vertices.
    inline void SetRelativeTargets(const bool bValue);

    /// Get the flag controlling normal updating. If this flag is
    /// set, new normals will be computed based on the morphed vertex data.
    inline bool GetUpdateNormals() const;

    // Set the flag controlling normal updating.
    inline void SetUpdateNormals(const bool bValue);

    /// Get the flag controlling updating. The morph will only be
    /// updated if this flag is set, and after submitting tasks to do update
    /// the flag is set back to false.
    inline bool GetNeedsUpdate() const;

    /// Set the flag controlling updating.
    inline void SetNeedsUpdate();

    /**
        Get the flag indicating whether or not the mesh we are attached
        to is also skinned. This is only needed to determine which streams
        to use for normal re-calculation, if normal re-calculation is enabled.
    */
    inline bool GetSkinned() const;

    /// Set the flag indicating whether or not the mesh we are attached
    /// to is also skinned.
    inline void SetSkinned(const bool bValue);

    /**
        Get the flag indicating whether or not the mesh the modifier
        is attached to is also software skinned.
        
        This flag is used to determine the
        sync points for the modifier, and must be set correctly before the
        Attach function is called.
    */
    inline bool GetSWSkinned() const;

    /// Set the flag indicating whether or not the mesh the modifier
    /// is attached to is also software skinned.
    inline void SetSWSkinned(const bool bValue);

    /**
        Determine if the modifier supports a specific material option.
        
        Returns true, and sets bResult to true, only if the option
        is NiRenderObjectMaterialOption::MORPHING(). Otherwise, returns false.
    */
    virtual bool RequiresMaterialOption(const NiFixedString& kMaterialOption,
        bool& bResult) const;

    /// @cond EMERGENT_INTERNAL

    /// Selective update control
    virtual void SetSelectiveUpdateFlags(bool& bSelectiveUpdate,
        bool& bSelectiveUpdateTransforms, bool& bRigid);

    /// Clones the modifier
    virtual void ProcessClone(NiCloningProcess& kCloning);
    
    /// Reset sync points. Should be done when no tasks are outstanding.
    virtual bool ResetSyncPoints(NiMesh* pkMesh);

    /// @endcond

protected:
    /// Class to hold data about each morphed element
    class ElementData : public NiMemObject
    {
    public:
        /// The semantic information about the element.
        NiFixedString m_kSemantic;
        NiUInt32 m_uiSemanticIdx;

        /// Values to control normalization of each element. If non-zero,
        /// the morphed points will re normalized.
        NiUInt32 m_uiNormalizeFlag;
        
        /// The morph target SP streams. Pointers owned by this object.
        NiTPrimitiveArray<NiSPStream*> m_kTargetSPStreams;
        
        /// The output SP stream, owned by this class.
        NiSPStream m_kOutputSPStream;

        /// Extra data used by the kernel for morphing
        NiMorphingKernelData m_kMorphingKernelData;

        /// MorphingKernel extra data input stream
        NiTSPFixedInput<NiMorphingKernelData> m_kMorphingKernelDataStream;

        /// Floodgate kernel
        NiMorphingKernel* m_pkMorphingKernel;
        
        /// Floodgate task
        NiSPTaskPtr m_spTask;

        /// Floodgate workflow
        NiSPWorkflowPtr m_spWorkflow;
    };

    // flags
    enum MorphFlags
    {
        FLAG_RELATIVETARGETS = 0x01,
        FLAG_UPDATENORMALS   = 0x02,
        FLAG_NEEDSUPDATE     = 0x04,
        FLAG_ALWAYSUPDATE    = 0x08,
        FLAG_NEEDSCOMPLETION = 0x10,
        FLAG_SKINNED         = 0x20,
        FLAG_SWSKINNED       = 0x40,
    };
    
    /// Set the flag indicating whether or not we need to update. This is
    /// protected so objects outside of the modifier can not set this flag
    /// to false.
    inline void SetNeedsUpdate(const bool bValue);

    /// Get and set the flag indicating whether or not there is an outstanding
    /// task that needs completion.
    inline bool GetNeedsCompletion() const;
    inline void SetNeedsCompletion(const bool bValue);

    /// These are elements of the output mesh that should be computed through
    /// morphing. See assumptions at top of this file.
    NiTPrimitiveArray<ElementData*> m_kElementData;

    /// The weights that control the morph. This stream has a single
    /// element of type float, and there are as many components as there
    /// are morph targets.
    NiSPStream* m_pkWeightsSPStream;
    
    /// The number of morph targets.
    NiUInt16 m_uiNumTargets;
};

NiSmartPointer(NiMorphMeshModifier);

#include "NiMorphMeshModifier.inl"

#endif // #ifndef NIMORPHMESHMODIFIER_H
