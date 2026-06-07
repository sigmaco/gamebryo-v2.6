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

#ifndef NIMESHCULLINGPROCESS_H
#define NIMESHCULLINGPROCESS_H

#include "NiCullingProcess.h"
#include "NiMeshLibType.h"
#include "NiVisibleArray.h"

class NiSPWorkflowManager;

/**
    The default culling process for NiMesh objects.

    NiMeshCullingProcess is the only subclass which can derive from 
    NiCullingProcess. Applications should use this class for culling and 
    for derived subclasses as it internally handles the details of executing
    mesh modifiers via Floodgate.
**/
class NIMESH_ENTRY NiMeshCullingProcess : public NiCullingProcess
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    
    /// @endcond
    
public:
    /**
        The default public constructor. 
        @param pkVisibleSet Pointer to the visible set to use with this 
            culling process. NULL may be passed if the application intends to
            provide a visible set to the call to Cull.
        @param pkWorkflowManager The workflow manager for this culling process.
            Applications may pass NULL in for the workflow manager. If so,
            NiMeshCullingProcess will create one from the heap.
    **/
    NiMeshCullingProcess(NiVisibleArray* pkVisibleSet,
        NiSPWorkflowManager* pkWorkflowManager);

    /// Destructor which deletes the workflow manager only if this object
    /// created it.
    virtual ~NiMeshCullingProcess();
    
    /// Obtain a pointer to the workflow manager that the process is using.
    NiSPWorkflowManager* GetWorkflowManager() const;

protected:

    /**
        This protected constructor is designed to allow only subclasses to 
        change the value of m_bUseVirtualAppend declared in NiCullingProcess.
        See documentation on that class for discussion of the argument.
    **/
    NiMeshCullingProcess(NiVisibleArray* pkVisibleSet, 
        NiSPWorkflowManager* pkWorkflowManager, bool bUseVirtualAppend);

    /**
        The overridden protected processing function from NiCullingProcess.
        Note that previous versions of Gamebryo declared Process as a 
        public virtual. The current version of Gamebryo uses the non-virtual 
        method, NiCullingProcess::Cull, as the entry point into the culling 
        process. 
        @param pkCamera The camera for this culling operation.
        @param pkScene The scene root for this operation.
        @param pkVisibleSet The visible set for results returned from this
            operation. NULL can be passed if the object was constructed with
            a non-null visible set.
    **/
    virtual void Process(const NiCamera* pkCamera, NiAVObject* pkScene,
        NiVisibleArray* pkVisibleSet);

    /// @cond EMERGENT_INTERNAL

    /**
        The following methods are used internally to handle flushing of mesh
        modifiers and other activities after the call to Process has completed
        inside Cull. It is not recommended that they be overridden. They are
        provided as pure virtual functions to avoid circular dependencies with
        NiFloodgate and NiMesh.
    */
    /// @name Helper Processing Functions
    //@{
    virtual void PreProcess();
    virtual void PostProcess();
    //@}

    /// @endcond

    /// Boolean indicating if the culling process owns or references the
    /// workflow manager.
    bool m_bOwnsManager;

private:
    // Disable warning C4512
    NiMeshCullingProcess & operator=( const NiMeshCullingProcess & );
};

NiSmartPointer(NiMeshCullingProcess);

#endif
