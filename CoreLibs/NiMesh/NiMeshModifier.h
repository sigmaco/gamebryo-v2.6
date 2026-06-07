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

#ifndef NIMESHMODIFIER_H
#define NIMESHMODIFIER_H

#include "NiMeshLibType.h"
#include "NiMeshRequirements.h"
#include <NiFixedString.h>
#include <NiObject.h>
#include <NiSmartPointer.h>
#include <NiTArray.h>
#include <NiTSet.h>
#include <NiTList.h>
#include <NiSPWorkflowManager.h>
#include <NiSyncArgs.h>

class NiMesh;
class NiCullingProcess;
class NiMeshTracker;

/**
    This virtual class is the base for mesh modifiers. A mesh modifier is
    an object that is associated with an NiMesh object and performs dynamic
    changes to the mesh. Examples include morphing and skinning.
    
    Modifiers are designed to run in parallel with other Gamebryo processing,
    and contain information for synchronization with other engine activities.
    It is also possible to create modifiers that execute immediately.
*/
class NIMESH_ENTRY NiMeshModifier : public NiObject
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareAbstractClone(NiMeshModifier);
    NiDeclareAbstractStream;
    NiDeclareViewerStrings;
    
    /// @endcond

public:
    /// Constructor does nothing.
    NiMeshModifier();
    
    /// Destructor does nothing.
    virtual ~NiMeshModifier();
    
    /**
        @name Sync Point Functionality
        
        All modifiers maintain information about the synchronization points
        at which they need updating.
    */
    /// @{
    /**
        Add a sync point at which some work should begin.
        
        Derived classes that implement this function must always call the
        base class version, which manages storage of supported sync points.
    */
    virtual void AddSubmitSyncPoint(const NiSyncArgs::SyncPoint uiSyncPt);

    /**
        Remove a start sync point.
        
        Derived classes that implement this function must always call the
        base class version, which manages storage of supported sync points.
    */
    virtual void RemoveSubmitSyncPoint(const NiSyncArgs::SyncPoint uiSyncPt);

    /// Query if a start sync point is supported.
    inline bool SupportsSubmitSyncPoint(
        const NiSyncArgs::SyncPoint uiSyncPt) const;
    
    /// Get the number of submission points
    inline NiUInt32 GetSubmitSyncPointCount() const;
    
    /// Get a specific submit point
    inline NiSyncArgs::SyncPoint GetSubmitSyncPoint(
        const NiUInt32 uiIndex) const;

    /**
        Add a sync point at which some work should complete.
        
        Derived classes that implement this function must always call the
        base class version, which manages storage of supported sync points.
    */
    virtual void AddCompleteSyncPoint(const NiSyncArgs::SyncPoint uiSyncPt);

    /**
        Remove a completion sync point.
        
        Derived classes that implement this function must always call the
        base class version, which manages storage of supported sync points.
    */
    virtual void RemoveCompleteSyncPoint(
        const NiSyncArgs::SyncPoint uiSyncPt);

    /// Query if a completion sync point is supported.
    inline bool SupportsCompleteSyncPoint(
        const NiSyncArgs::SyncPoint uiSyncPt) const;

    /// Get the number of completion points
    inline NiUInt32 GetCompleteSyncPointCount() const;
    
    /// Get a specific completion point
    inline NiSyncArgs::SyncPoint GetCompleteSyncPoint(
        const NiUInt32 uiIndex) const;
    /// @}

    /// @name Updating and Managing Parallel Tasks
    /// @{
    /**
        Attach is called when a modifier is attached to a mesh or is
        otherwise required to do any set-up neeed for updating.
        
        Most commonly,
        the modifier will add NiStreamProcessor (Floodgate) tasks and
        attach kernels, input streams, output streams, etc. Any of these
        streams may be associated with a data stream on the mesh.

        @note AreRequirementsMet should be called prior to this method. This 
            allows Attach to be leaner in implementation. This behavior is
            enforced by NiMesh.
    */
    virtual bool Attach(NiMesh* pkMesh) = 0;
    
    /**
        Performs any work needed to remove the modifier from a mesh.
        
        The calling function is responsible for ensuring that all outstanding
        Floodgate tasks are completed. All tasks should be deleted, streams
        released, etc. Code in this function can assume that
        CompleteTasks has been performed for all sync points.
    */
    virtual bool Detach(NiMesh* pkMesh) = 0;

    /**
        This function is the entry point for an update, when a parallel
        processing modifier will submit its Floodgate tasks to the given
        workflow manager.
        
        Mesh modifiers that do not do any parallel work
        may choose to use this function as the primary work function. It will
        be called whenever a specific supported start sync point is hit.
        
        @param pkMesh The mesh to which the modifier is attached.
        @param pkArgs Information about the sync point to submit for. If the
            submit point is specific, then the pkArgs is
            guaranteed to be of a matching type. When the submit point is 
            SYNC_UPDATE or SYNC_POST_UPDATE, pkArgs is of type 
            NiUpdateSyncArgs. When the submit point is SYNC_VISIBLE, pkArgs 
            is of type NiCullingSyncArgs. No specific type is guaranteed
            at other times.
        @param pkWFManager The workflow manager object that is to be used
            for Floodgate tasks. Tasks should be submitted using the functions
            provided by the workflow manager, and a NiSPWorkflow object will
            typically be provided in return. Mesh modifiers must hold on to
            the returned workflow in a smart pointer, as it is the only
            handle they have for completing the tasks and cleaning up.
        
        @return The function should return true if any tasks are
            submitted to the workflow manager.
            
        @note NiMeshModifier-derived classes must construct a Task Group ID
            for use with the NiSPWorkflowManager class using the
            NiSyncArgs::GetTaskGroupID(SyncPoint, SyncPoint) function.
    */
    virtual bool SubmitTasks(NiMesh* pkMesh, NiSyncArgs* pkArgs,
        NiSPWorkflowManager* pkWFManager) = 0;
        
    /**
        This function is used to force tasks to complete.
        
        Tasks with
        the given start and end sync points must be forced to complete.
        The value NiAVObject::SYNC_ANY may be given for either the start
        or end sync point, in which case all the tasks starting or ending at
        that point should be completed. After completing all the tasks for
        a particular NiSPWorkflow object, any smart pointers to that workflow
        should be released.
        
        Mesh modifiers should, in this function, also do any other work
        required to complete an update step, such as computations that depend
        on the output of the Floodgate tasks. 
        
        Mesh modifiers that do not use Floodgate could use this function to
        perform any work associated with the given pair of sync points.
        
        This function will only be called with SYNC_ANY or sync points
        that have been specified as starting or ending. There is no
        guarantee that the pkArgs variable is of any specific type.

        @return The function should return true if tasks were sucessfully
            completed. False indicates an error condition.
    */
    virtual bool CompleteTasks(NiMesh* pkMesh, NiSyncArgs* pkArgs) = 0;
    /// @}

    /// @name Querying Requirements
    /// @{
    /**
        RequiresMaterialOption may be used by materials to determine 
        whether or not they are expected to do some work.
        
        A return value of
        true means that: (a) the material should implement kMaterialOption and
        (b) the mesh has the required data for implementing kMaterialOption. 
        A return value of false means that the material should not implement
        kMaterialOption.
    */
    virtual bool RequiresMaterialOption(const NiFixedString& kMaterialOption,
        bool& bResult) const;

    /**
        Validates the data stream requirements against the mesh.
        
        A caller
        may choose to remove the mesh modifier if the mesh does not satisfy
        the requirements.

        @note In some cases, NiSystemDesc::RENDERER_GENERIC is passed into 
            this method. In those scenarios, it should be treated as if any 
            renderer is being passed in. If certain requirements are renderer 
            specific allow RENDERER_GENERIC satisfy those requirements.
    */
    virtual bool AreRequirementsMet(NiMesh* pkMesh,
        NiSystemDesc::RendererID ePlatform) const;

    /// Calls the polymorphic version with the current renderer ID.
    ///
    /// This function will likely fail if a renderer has not been created.
    bool AreRequirementsMet(NiMesh* pkMesh) const;

    /// Retrieve the requirements from the mesh modifier.
    virtual void RetrieveRequirements(
        NiMeshRequirements& kRequirements) const;
    /// @}

    /// @cond EMERGENT_INTERNAL

    /// Clones a mesh modifier.
    virtual void ProcessClone(NiCloningProcess& kCloning);

    /// Post-link for streaming.
    virtual void PostLinkObject(NiStream& kStream);

    /// Selective update control.
    virtual void SetSelectiveUpdateFlags(bool& bSelectiveUpdate,
        bool& bSelectiveUpdateTransforms, bool& bRigid);
    
    /// @endcond

protected:
    NiTPrimitiveSet<NiSyncArgs::SyncPoint> m_kSubmitPoints;
    NiTPrimitiveSet<NiSyncArgs::SyncPoint> m_kCompletePoints;
        
};

// Define a NiMeshModifier smart pointer
NiSmartPointer(NiMeshModifier);

// Define an array of NiMeshModifier objects
typedef NiTPointerList<NiMeshModifierPtr> NiMeshModifierList;

#include "NiMeshModifier.inl"

#endif // #ifndef NIMESHMODIFIER_H
