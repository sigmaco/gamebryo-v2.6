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

#ifndef NIMESH_H
#define NIMESH_H

// NOTE: This is NOT the top-level header file for the NiMesh library.
// Use NiMeshLib.h.

#include "NiMeshSDM.h"

#include <NiCommonSemantics.h>
#include <NiPrimitiveType.h>
#include <NiRenderObject.h>
#include <NiUpdateProcess.h>

#include "NiDataStreamElementLock.h"
#include "NiDataStreamRef.h"
#include "NiInstancingUtilities.h"
#include "NiMeshModifier.h"

/**
    NiMesh is the base class for all vertex-based geometry in Gamebryo. 

    The essential components of NiMesh are the array of NiDataStreamRef
    objects that contain all varying attributes of the mesh (vertices, 
    indices, etc), the list of modifiers that procedurally manipulate 
    the mesh, and bounds management for the mesh.
*/
class NIMESH_ENTRY NiMesh : public NiRenderObject
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareClone(NiMesh);
    NiDeclareStream;
    NiDeclareViewerStrings;
    
    /// @endcond

public:
    /// Default constructor.
    NiMesh();

    /// Virtual destructor.
    virtual ~NiMesh();

    /// Returns the sum over all submeshes in the first USAGE_VERTEX
    /// data stream with per-vertex (not per-instance) data.
    ///
    /// @note Multiple references to the same NiDataStream Region 
    /// by multiple submeshes are ignored.
    NiUInt32 GetVertexCount() const;

    /// Returns the count for the specified submesh in
    /// the first USAGE_VERTEX data stream with per-vertex
    /// (not per-instance) data.
    NiUInt32 GetVertexCount(NiUInt32 uiSubMesh) const;

    /**
        @name Public Instancing Helper Methods
        Helper methods to assist the renderers with instanced meshes.
    */
    //@{
    /// Determine whether or not this mesh is instanced.
    inline const bool GetInstanced() const;
   
    /// Get the reference for the base instance stream.
    inline NiDataStreamRef* GetBaseInstanceStream() const;    

    /// Get the reference for the visible instance stream.
    inline NiDataStreamRef* GetVisibleInstanceStream() const; 

    /** 
        Returns true if the given mesh satisfies the requirements to be an
        instance.
        
        The function compares a wide range of data including data streams,
        materials, properties. All data on one mesh must exist on the other
        mesh and be equal on both meshes.

        @param pkTestMesh The mesh to test against.
    */
    bool IsInstancable(const NiMesh* pkTestMesh) const;

    //@} 

    /// @name Bounds Management
    //@{
    /// Set the model-space bound for this mesh.
    inline void SetModelBound(const NiBound& kBound);

    /// Get the model-space bound for this mesh.
    inline const NiBound& GetModelBound() const;

    /**
        Compute the bounds from the stream containing the POSITION semantic.
        @note The POSITION stream must have CPU_READ access.
    */
    void RecomputeBounds();

    /// update the world bounds.
    virtual void UpdateWorldBound();
    //@}

    /**
        Sets the primitive type for the NiMesh to the value specified in 
        ePrimitiveType.
        
        Values are defined by the PrimitiveType enumeration.
    */
    inline void SetPrimitiveType(NiPrimitiveType::Type ePrimitiveType);

    /// Gets the primitive type of the NiMesh.
    ///
    /// Values are defined by the PrimitiveType enumeration.
    NiPrimitiveType::Type GetPrimitiveType() const;

    /// Primitive type enumeration value returned as a string.
    const char* GetPrimitiveTypeString();

    /// Render the object immediately.
    virtual void RenderImmediate(NiRenderer* pkRenderer);

    /**
        Method to determine whether or not this mesh requires a material
        to implement specific functionality.

        RequiresMaterialOption may be used by materials to determine 
        whether or not they are expected to do some work. kMaterialOption 
        is a fixed string representing the work in question. It may be
        one of the standard Gamebryo material option strings (see 
        NiRenderObjectMaterialOption) or a custom option string.
        
        A return value of true means that: 
           - the material should implement kMaterialOption, and
           - the mesh has the required data for implementing kMaterialOption. 

        A return value of false means that:
           - the material should not implement kMaterialOption.
    */
    virtual bool RequiresMaterialOption(const NiFixedString& kMaterialOption)
        const;
     
    /**
        @name Public NiMeshModifier Helper Methods
        Methods to interact with the NiMeshModifiers attached to this NiMesh.
    */
    //@{
    /// Gets the number of modifiers on this mesh.
    inline NiUInt32 GetModifierCount() const;

    /// Get the modifier at the specified index.
    inline NiMeshModifier* GetModifierAt(NiUInt32 uiModifierIndex) const;

    /// Get the index of the specified modifier.
    /// Returns (NiUInt32)(-1) if the modifier is not present.
    NiUInt32 GetModifierIndex(NiMeshModifier* pkModifier) const;

    /// Gets the first modifier with a given RTTI class.
    NiMeshModifier* GetModifierByType(const NiRTTI* pkRTTI) const;

    /// Adds an NiMeshModifier to this NiMesh.
    ///
    /// If bAttach is false, the modifier is not Attached (see below).
    inline bool AddModifier(NiMeshModifier* pkModifier, bool bAttach = true);

    /**
        Adds an NiMeshModifier to this NiMesh at the index specified, which
        must be less than GetModifierCount.
    
        If bAttach is false, the modifier is not Attached (see below).
    */
    bool AddModifierAt(NiUInt32 uiInsertIndex, NiMeshModifier* pkModifier, 
        bool bAttach = true);

    /// Remove an NiMeshModifier from this NiMesh.
    bool RemoveModifier(NiMeshModifier* pkModifier);

    /// Removes an NiMeshModifier from this NiMesh using its index in the 
    /// array of modifiers.
    bool RemoveModifierAt(NiUInt32 uiRemoveIndex);

    /// Returns true if the modifier at the specified index has been attached
    bool GetModifierAttachedAt(NiUInt32 uiIndex) const;

    /**
        Attaches an NiMeshModifier on this NiMesh.
        
        This action causes the modifier to associate itself with data
        streams on the mesh and do other work required to execute.
    */
    bool AttachModifier(NiMeshModifier* pkModifier);

    /// Detaches an NiMeshModifier from this NiMesh.
    ///
    /// This function undoes anything done during Attach for the same
    /// modifier.
    bool DetachModifier(NiMeshModifier* pkModifier);
    
    /**
        A convenience function to call AttachModifier for all the modifiers
        on the mesh.
        
        Returns false if any one AttachModifier returns false, otherwise
        returns true.
    */
    bool AttachAllModifiers();

    /**
        A convenience function to call DetachModifier for all the modifiers
        on the mesh.
        
        Returns false if any one DetachModifier returns false, otherwise
        returns true.
    */
    bool DetachAllModifiers();

    /**
        A static convenience function to call AttachModifier for all the
        modifiers on all of the meshes in a scene graph.
        
        Returns false if any one AttachModifier returns false, otherwise
        returns true.
    */
    static bool AttachSceneModifiers(NiAVObject* pkScene);

    /**
        A static convenience function to call DetachModifier for all the
        modifiers on all of the meshes in a scene graph.
        
        Returns false if any one DetachModifier returns false, otherwise
        returns true.
    */
    static bool DetachSceneModifiers(NiAVObject* pkScene);

    /** 
        Called to rebind any NiDataStreams referenced by modifiers on this 
        mesh. 
        
        This method should be called after datastreams are added or 
        removed in order to make sure that the mesh modifers do not reference
        invalid or stale datastreams. Internally, this method waits for any
        pending tasks, calls Detach on all modifiers, and then calls Attach
        on all modifiers.
        
        @return False if there was any error, otherwise true. If there
            is an error resetting one modifier, the function continues
            to process the others, and then returns false;

        @note This method is moderately expensive.
    **/
    bool ResetModifiers();

    /**
        A static convenience function to call ResetModifiers for all the
        modifiers on all of the meshes in a scene graph.
        
        Returns false if any one ResetModifiers returns false, otherwise
        returns true.
    */
    static bool ResetSceneModifiers(NiAVObject* pkObject);

    /// Submit all modifiers that match the sync points in the argument.
    void SubmitModifiers(NiSyncArgs* pkArgs,
        NiSPWorkflowManager* pkWFManager);

    /// Complete all modifiers that match the sync points in the argument.
    void CompleteModifiers(NiSyncArgs* pkArgs);

    /**
        Traverse the scene and submit mesh modifiers (static function).

        @param pkScene Root of scene graph.
        @param pkArgs Synchronization points to submit for.
        @param pkWorkflowManager The workflow manager used to submit
            the modifiers.
    **/
    static void SubmitSceneModifiers(NiAVObject* pkScene, NiSyncArgs* pkArgs,
        NiSPWorkflowManager* pkWorkflowManager);
    
    /** Traverse the scene and complete currently executing mesh modifiers
        (static function).
    
        @param pkScene Root of scene graph.
        @param pkArgs Synchronization points to complete. If this argument is
           0, all modifiers are completed regardless of their syncronization
           points.
    **/
    static void CompleteSceneModifiers(NiAVObject* pkScene,
        NiSyncArgs* pkArgs = 0);
    //@}

    /// Returns true if the specified semantic exists on a datastream attached
    /// to this NiMesh.
    virtual bool ContainsData(const NiFixedString& kSemantic, 
        NiUInt32 uiSemanticIndex = 0) const;

    /// Returns the number of elements with the specified semantic across all
    /// semantic indices.
    virtual NiUInt32 GetSemanticCount(const NiFixedString& kSemantic) const;

    /// Gets the number of sub-meshes contained in this mesh.
    inline NiUInt32 GetSubmeshCount() const;

    /// Sets the slot index count for all StreamRefs, and resets all
    /// slot indices to the value uiInitialRegionIndexValue, which defaults 
    /// to 0.
    inline void SetSubmeshCount(NiUInt32 uiSubmeshCount,
        NiUInt16 uiInitialRegionIndexValue = 0);

    /**
        Gets the data stream reference using the criteria specified 
         (const-safe).

        @param kSemantic Semantic to match.
        @param uiSemanticIndex Semantic index to match.
        @param eFormat NiDataStreamElement::Format to match. Ignored if 
            eFormat = NiDataStreamElement::F_UNKNOWN (the default value).
        @return A const data stream reference matching the criteria specified,
            or NULL if none is found.
    */
    inline const NiDataStreamRef* FindStreamRef(
        const NiFixedString& kSemantic, 
        NiUInt32 uiSemanticIndex = 0, 
        NiDataStreamElement::Format eFormat = NiDataStreamElement::F_UNKNOWN) 
            const;

    /**
        Gets the data stream reference using the criteria specified.

        @param kSemantic Semantic to match.
        @param uiSemanticIndex Semantic index to match.
        @param eFormat NiDataStreamElement::Format to match. Ignored if 
            eFormat = NiDataStreamElement::F_UNKNOWN (the default value).
        @return A data stream reference matching the criteria specified, or
            NULL if none is found.
    */
    inline NiDataStreamRef* FindStreamRef(const NiFixedString& kSemantic,
        NiUInt32 uiSemanticIndex = 0, 
        NiDataStreamElement::Format eFormat = NiDataStreamElement::F_UNKNOWN);

    /// Gets the number of streamrefs attached to this mesh.
    inline NiUInt32 GetStreamRefCount() const;

    /// Get the stream ref at the specified index (const-safe).
    inline const NiDataStreamRef* GetStreamRefAt(
        NiUInt32 uiStreamRefIdx) const;

    /// Get the stream ref at the specified index.
    inline NiDataStreamRef* GetStreamRefAt(NiUInt32 uiStreamRefIdx);

    /**
        Method to add an existing datastream to the internal output streamref 
        array while also filling in common streamref data.  Each material can
        have a separate set of output streams, so the material that this 
        output stream is used with must be specified here.

        @note This will bind all submesh region references to the first region
            in the datastream.
        @param pkMaterial Material that will output data to this stream.
        @param pkOutputStream NiDataStream to receive the output data.
        @param kBindName Name applied to this data stream. This name will be
            used by the material to specify which stream it is writing to. 
        @param kSemanticName Semantic to bind to the first element desc.
        @param uiSemanticIndex Semantic index to bind to the first element 
            desc.
    */
    NiDataStreamRef* AddOutputStream(
        const NiMaterial* pkMaterial,
        NiDataStream* pkOutputStream,
        const NiFixedString& kBindName,
        const NiFixedString& kSemanticName, 
        NiUInt32 uiSemanticIndex = 0);

    /**
        Static method to convert an NiOutputStreamDescriptor::VertexFormatEntry
        object's DataType field to the equivalent value in 
        NiDataStreamElement::Format.
    */
    static NiDataStreamElement::Format VertexFormatEntryToDataStreamFormat(
        const NiOutputStreamDescriptor::VertexFormatEntry& kEntry);

    /** 
        Removes the output stream refs, for the specified material,
        from this mesh.
    */
    inline void RemoveOutputStreamsForMaterial(const NiMaterial* pkMaterial);

    /// Remove all output streams associated with this NiMesh.
    inline void RemoveOutputStreamsForAllMaterials();

    /// Gets the number of output stream refs attached to this mesh.
    NiUInt32 GetCurrentMaterialOutputStreamRefCount();

    /// Get the stream ref at the specified index.
    NiDataStreamRef* GetCurrentMaterialOutputStreamRefAt(
        NiUInt32 uiOutputStreamIdx);
    const NiFixedString& GetCurrentMaterialOutputStreamRefNameAt(
        NiUInt32 uiOutputStreamIdx);

    /**
        Finds an stream reference index and element reference index 
        for the semantic and semantic index specified (const-safe). 

        If eFormat is something other than F_UNKNOWN then the type
        will be checked as well.
        This method returns true if the semantic was located, false otherwise.
    */
    inline bool FindStreamRefAndElementBySemantic(
        const NiFixedString& kSemantic,
        NiUInt32 uiSemanticIndex, 
        NiDataStreamElement::Format eFormat, // Ignored if F_UNKNOWN
        const NiDataStreamRef*& pkRef, 
        NiDataStreamElement& kElement) const;

    /**
        Finds an stream reference index and element reference index 
        for the semantic and semantic index specified. 

        If eFormat is something other than F_UNKNOWN then the type
        will be checked as well.
        This method returns true if the semantic was located, false otherwise.
    */
    inline bool FindStreamRefAndElementBySemantic(
        const NiFixedString& kSemantic,
        NiUInt32 uiSemanticIndex, 
        NiDataStreamElement::Format eFormat, // Ignored if F_UNKNOWN
        NiDataStreamRef*& pkRef, 
        NiDataStreamElement& kElement);


    /**
        Returns the first StreamRef matching the NiDataStream::Usage and 
        NiDataStream::Access flags specified (const-safe).

        @param eUsage Single NiDataStream::Usage to match.
        @param uiAccessRequiredMask Combination of NiDataStream::Access values
            or'ed together. Defaults to allow any Access.
        @return First StreamRef matching input parameters.
    */
    inline const NiDataStreamRef* GetFirstStreamRef(
        NiDataStream::Usage eUsage, 
        NiUInt32 uiAccessRequiredMask = ~0) const;

    /**
        Returns the first StreamRef matching the NiDataStream::Usage and 
        NiDataStream::Access flags specified.

        @param eUsage Single NiDataStream::Usage to match.
        @param uiAccessRequiredMask Combination of NiDataStream::Access values
            or'ed together. Defaults to allow any Access.
        @return First StreamRef matching input parameters.
    */
    inline NiDataStreamRef* GetFirstStreamRef(NiDataStream::Usage eUsage, 
        NiUInt32 uiAccessRequiredMask = ~0);

    /// Returns the first StreamRef that matching USAGE_VERTEX and containing
    /// per-vertex (as opposed to per-instance) data.
    const NiDataStreamRef* GetFirstUsageVertexPerVertexStreamRef() const;

    
    /**
        Returns the first StreamRef that matching USAGE_VERTEX and containing
        per-vertex (as opposed to per-instance) data.
    
        Non-const version.
    */
    NiDataStreamRef* GetFirstUsageVertexPerVertexStreamRef();

    /**
        Method to add an existing datastream to the internal streamref array
        while also filling in common streamref data.

        @note This will bind all submesh region references to the first region
            in the datastream.
        @param pkStream Existing datastream
        @param kSemanticName Semantic to bind to the first element desc
        @param uiSemanticIndex Semantic index to bind to the first element 
            desc.
        @param bPerInstanceStream Determine if this is a source of instancing
            data
    */
    inline NiDataStreamRef* AddStreamRef(
        NiDataStream* pkStream,
        const NiFixedString& kSemanticName, 
        NiUInt32 uiSemanticIndex = 0,
        bool bPerInstanceStream = false);

    /**
        Method to add a blank streamref to the mesh.
        
        @note In order to be useful, this streamref needs to be filled out 
            with information.
    */
    inline NiDataStreamRef* AddStreamRef();

    /**
        Method to add a copy of the input streamref to the mesh.
        
        @note A copy is made of the streamref. No pointer sharing occurs.
    */
    inline NiDataStreamRef* AddStreamRef(const NiDataStreamRef* pkRef);

    /**
        Instantiates and adds a new, single-element stream to the mesh.
        
        The ownership of the pvSource pointer remains the responsibility of
        the caller and is not managed by the new stream. The data contained in
        pvSource (if not null) will be copied into the newly created stream.
        
        @param kSemantic The semantic string for the newly instantiated
            stream.
        @param uiSemanticIndex The semantic index for the newly instantiated
            stream.
        @param eFormat The format of the single element in the stream.
        @param uiCount The number of instances of that element in the stream.
        @param uiAccessMask The access specifiers for this stream.
        @param eUsage The usage specifiers for this stream.
        @param pvSource Optional, initial source data for this stream. The
            size and type of this buffer is expected to match the size and
            type of the stream that is being created. If static streams do not
            write their data at this time, a subsequent write lock can be
            obtained to write the data.
        @param bForceToolDSCreate This forces the factory to create an 
            NiToolDataStream.
        @param bCreateDefaultRegion This creates a single region that is the
            size of the entire buffer.
        @return NiDataStreamRef The pointer to the new stream.
    */
    NiDataStreamRef* AddStream(
        const NiFixedString& kSemantic, 
        NiUInt32 uiSemanticIndex, 
        NiDataStreamElement::Format eFormat,
        NiUInt32 uiCount, 
        NiUInt8 uiAccessMask, 
        NiDataStream::Usage eUsage, 
        const void* pvSource = NULL, 
        bool bForceToolDSCreate = false,
        bool bCreateDefaultRegion = true);

    /// Same as AddStream, but return a locked NiDataStreamElementLock 
    /// for the newly added NiDataStream object.
    inline NiDataStreamElementLock AddStreamGetLock(
        const NiFixedString& kSemantic, 
        NiUInt32 uiSemanticIndex, 
        NiDataStreamElement::Format eFormat,
        NiUInt32 uiCount, 
        NiUInt8 uiAccessMask, 
        NiDataStream::Usage eUsage, 
        bool bForceToolDSCreate = false,
        bool bCreateDefaultRegion = true,
        NiUInt8 uiLockMask = 
            NiDataStream::LOCK_READ |
            NiDataStream::LOCK_WRITE | 
            NiDataStream::LOCK_TOOL_READ |
            NiDataStream::LOCK_TOOL_WRITE);


    /**
        Removes the specified stream ref.
        
        This will invalidate all outstanding NiDataStreamRef pointers.
    */
    inline void RemoveStreamRef(const NiDataStreamRef* pkRef);

    /// Remove all stream ref's associated with this NiMesh.
    inline void RemoveAllStreamRefs();

    /// Gets the number of elements in the stream with the semantic
    /// specified for the provided submesh.
    inline NiUInt32 GetCount(NiUInt32 uiSubmesh,
        const NiFixedString& kSemantic, NiUInt32 uiSemanticIndex = 0) const;
    
    /// Gets the number of elements in the stream with the semantic
    /// specified for all submesh.
    inline NiUInt32 GetTotalCount(const NiFixedString&
        kSemantic, NiUInt32 uiSemanticIndex = 0) const;

    /// Gets the largest semantic index matching the input semantic name.
    /// @return -1 if not found.
    inline NiInt32 GetMaxIndexBySemantic(
        const NiFixedString& kSemantic) const;

    /**
        Gets the number of primitives for a given mesh. 
        
        If the index buffer exists, then it will be used with the primitive 
        type to determine the number of primitives it contains. If the index 
        buffer is not present the position buffer will be used with the 
        primitive type to determine the number of primitives. A value of 0 
        will be returned in all other cases.
    */
    inline NiUInt32 GetPrimitiveCount(NiUInt32 uiSubmeshIndex) const;
    
    /// Calls GetPrimitiveCount for all submesh and returns the total.
    inline NiUInt32 GetTotalPrimitiveCount() const;

    /**
        Update the cached primitive count for the submesh at index 0 on the
        object.
        
        Applications only need to call this for meshes modified
        programmatically. The function is automatically called during cloning
        and streaming.
    */
    inline void UpdateCachedPrimitiveCount();

    /**
        Adds the NiMesh object to a set of objects that is used when preparing
        and finalizing the mesh.
        
        The default implementation of this function just adds "this" to the
        set. Derived classes may add additional embedded NiMesh objects to the
        set.
    */
    virtual void RetrieveMeshSet(NiTPrimitiveSet<NiMesh*>& kMeshSet);

    /** 
        Returns true if the mesh is valid for consumption by the renderers 
        and NiMeshAlgorithms.
    
        Checks if each NiDataStreamRef is valid and contains the same
        number of submeshes as the mesh.

        @param pkInvalidDescription If provided, set to cause of failure.
    */
    bool IsValid(NiString* pkInvalidDescription = NULL) const;

    //@{
    /**
        Accesses a flag that controls whether or not NiMesh objects are 
        precached as they are encountered during streaming. 

        The default value for the flag is to precache.
    */
    static void SetUsePreloading(bool bPreload);
    static bool GetUsePreloading();
    //@}

    //@{
    /**
        Accesses a flag that controls whether or not the CPU memory buffers
        are released when NiMesh objects are precached during streaming. 
        
        This will only be done for static buffers that do not have the 
        CPU_WRITE flag enabled. The default value the flag is to release the 
        memory.
    */
    static void SetDestroyAppDataFlag(bool bDestroy);
    static bool GetDestroyAppDataFlag();
    //@}

    /// @cond EMERGENT_INTERNAL

    /// Submit any modifiers that should be submitted as part of an update
    /// step.
    void SubmitUpdateModifiers(NiUpdateProcess& kUpdate);

    /**
        Complete all modifiers that should complete at SYNC_UPDATE or
        SYNC_POST_UPDATE.
    
        Called as the first stage of updating an object.
    */
    void CompleteUpdateModifiers();

    /// Copy the stream refs from this mesh
    void CloneStreamRefs(NiMesh* pkOriginal, 
        NiCloningProcess* pkCloning = NULL);

    /**
        Gets the number of primitives given an element count.
    
        This count will generally be the number of elements
        in a slot on a data stream, the the entire length of the stream.
    */
    NiUInt32 GetPrimitiveCountFromElementCount(NiUInt32 uiElementCount) const;

    /**
        Configures this mesh to draw the results of pkSourceMesh's 
        Stream Output.  pkSourceMesh must have the desired stream-out 
        material active, and the callee (this) mesh must have the material
        designed to draw the results active, when this function is called.
        After the call, however, you switch away from this material and come 
        back to it without any problems.
    */
    bool BindToStreamOutputOf(NiMesh* pkSourceMesh);

    /**
        The application can call this function to tell the renderer to request 
        a one-time clear on the output streams, for this mesh, associated with 
        the specified material.
    */
    void ClearStreamOutBuffers(const char* pcMaterialName);

    /**
        The application can call this function to tell the renderer to request 
        a one-time clear on ALL output streams for ALL materials on the mesh.
    */
    void ClearAllStreamOutBuffers();

    /**
        Renderer calls this to check if the active material wants its
        SO buffers to be cleared.
    */
    bool CheckIfActiveMaterialStreamOutBuffersNeedCleared();

    /**
        Renderer should call this when it clears the SO buffers for the
        active material, so we can reset the m_bNeedsCleared flag.
    */
    void OnActiveMaterialStreamOutBuffersCleared();

    /// Miscellaneous functions to support Stream Output.
    inline bool GetInputDataIsFromStreamOut() const;
    inline void SetInputDataIsFromStreamOut(bool bInputDataIsFromStreamOut);

    /// Culling (separate from display).
    virtual void OnVisible(NiCullingProcess& kCuller);

    /// Update methods.
    virtual void UpdateDownwardPass(NiUpdateProcess& kUpdate);
    virtual void UpdateSelectedDownwardPass(NiUpdateProcess& kUpdate);
    virtual void UpdateRigidDownwardPass(NiUpdateProcess& kUpdate);
    virtual void UpdatePropertiesDownward(NiPropertyState* pkParentState);
    virtual void UpdateEffectsDownward(NiDynamicEffectState* pkParentState);

    /// Cloning methods.
    virtual void ProcessClone(NiCloningProcess& kCloning);

    /// set selective update flags.
    virtual void SetSelectiveUpdateFlags(
        bool& bSelectiveUpdate, 
        bool bSelectiveUpdateTransforms, 
        bool& bRigid);

    /// post-link.
    virtual void PostLinkObject(NiStream& kStream);

    /// Instancing helper methods.
    void SetInstanced(bool bInstanced);    
    void SetBaseInstanceStream(NiDataStreamRef* pkStream);
    void SetVisibleInstanceStream(NiDataStreamRef* pkStream);

    /// @endcond

protected:
    /// Disabled copy constructor.
    NiMesh(const NiMesh&);

    /// Find datastream for the specified semantic
    inline bool FindStreamAndElementIndex(
        const NiFixedString& kSemanticName,
        NiUInt32 uiSemanticIdx, 
        NiUInt32& uiStreamIdx, 
        NiUInt32& uiElementIdx) const;

    void StreamRefsLoadBinary(NiStream& kStream);
    bool StreamRefsRegisterStreamables(NiStream& kStream);
    void StreamRefsSaveBinary(NiStream& kStream);
    bool StreamRefsIsEqual(const NiMesh* pkOther) const;

    void SetModifierAttachedAt(NiUInt32 uiIndex, bool bAttached);

    /// Helper function to improve inlining of GetPrimitiveCount.
    inline NiUInt32 GetPrimitiveCountHelper(NiUInt32 uiSubmeshIndex) const;

    /**
        Method to add an existing NiDataStreamRef to the internal output 
        streamref array. The Ref must already have the semantic names/indices 
        set up in it. Each material can have a separate set of output streams, 
        so the material that this output stream is used with must be specified 
        here.

        @note This will bind all submesh region references to the first region
            in the datastream.
        @note The NiMesh assumes control of the passed-in pointer. The pointer
            may not be shared with any other object.

        @param pkMaterial Material that will output data to this stream.
        @param pkRef NiDataStreamRef to receive the output data.
        @param kBindName Name applied to this data stream. This name will be
            used by the material to specify which stream it is writing to. 
    */
    NiDataStreamRef* AddOutputStreamRef(
        const NiMaterial* pkMaterial,
        NiDataStreamRef* pkRef,
        const NiFixedString& kBindName);

    /// Update internal index pointing to the current material's set of 
    /// output data streams.
    void UpdateCurrentMaterialOutputDataStreamIndex();

    /**
        Configures this mesh to use Stream Output with the current material.  
        Adds a set of 'output' data streams on the mesh, so that when the mesh
        is drawn, the results will be saved into these streams.  
        Note that if you switch away from this material and come back, 
        the output streams will still be there and ready.
    */
    bool CreateStreamOutputStreamsForActiveMaterial();

    enum
    {
        /// The maximum number of mesh modifiers that can be associated 
        /// with a mesh.
        MAX_MESH_MODIFIER_COUNT = 7,

        /// The number of bytes needed to represent MAX_MESH_MODIFIER_COUNT
        /// where a single bit represents a single modifier entry.
        MAX_MESH_MODIFIER_BITARRAY_COUNT = 1
    };

    /// model data.
    NiBound m_kBound;

    class OutputStream : public NiMemObject
    {
    public:
        OutputStream();
        virtual ~OutputStream();

        // Not true NiObject cloning, but similar
        static OutputStream* CloneOutputStream(
            const OutputStream* pkStream, 
            NiCloningProcess& kCloning);

        bool IsEqual(const OutputStream* pkOther) const;

        class Entry : public NiMemObject
        {
        public:
            Entry();
            virtual ~Entry();

            // Not true NiObject cloning, but similar
            static Entry* CloneEntry(
                const Entry* pkEntry, 
                NiCloningProcess& kCloning);



            bool IsEqual(const Entry* pkOther) const;

            NiDataStreamRef* m_pkStreamRef; // for stream output
            NiFixedString m_kName;    // for binding by name
        };

        NiTPrimitiveSet<Entry*> m_kStreams; // for stream output
        bool m_bNeedsCleared;
        const NiMaterial* m_pkMaterial; // each material has its own set of *output* StreamRefs
    };

    // DataStream groups
    NiTPrimitiveSet<NiDataStreamRef*> m_kDataStreams;
    
    NiTPrimitiveSet<OutputStream*> m_kOutputDataStreamsByMaterial;
    unsigned int m_uiCurrentMaterialOutputDataStreamIndex;

    /// Array of NiMeshModifier objects that have been 
    /// added to this mesh.
    NiMeshModifierPtr m_aspMeshModifiers[MAX_MESH_MODIFIER_COUNT];

    /// @name Hardware Instancing Parameters
    /// @note Not all platforms may support hardware instancing.
    //@{
    NiDataStreamRef* m_pkBaseInstanceStream;
    NiDataStreamRef* m_pkVisibleInstanceStream;
    bool m_bInstancingEnabled;
    //@}

    /// Array of bits that signify whether or not the mesh modifier at the 
    /// same index in the m_aspMeshModifiers array has been properly attached
    /// to the mesh. If it has not, it will not be executed at runtime.
    NiUInt8 m_auiModifierAttached[MAX_MESH_MODIFIER_BITARRAY_COUNT];

    /// The total number of modifiers added to this mesh.
    NiUInt8 m_uiNumModifiers;

    /// Primitive type for the NiMesh.
    NiPrimitiveType::Type m_eMeshPrimitiveType;

    /// The primitive count of the first submesh cached as an optimization for
    /// the common case of meshes with only 1 submesh.
    NiUInt32 m_uiFirstSubmeshPrimitiveCount;

    /// The number of submeshes on this mesh.
    NiUInt16 m_uiSubmeshCount;

    /// If true, there are no indices, and the # of vertices is probably 
    /// unknown.
    bool m_bInputDataIsFromStreamOut;

    static bool ms_bPreload;
    static bool ms_bDestroyAppData;
};

NiSmartPointer(NiMesh);

#include "NiMesh.inl"

#endif /// #ifndef NIMESH_H
