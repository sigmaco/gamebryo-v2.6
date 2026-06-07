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

#ifndef NIMESHUPDATEPROCESS_H
#define NIMESHUPDATEPROCESS_H

#include <NiUpdateProcess.h>
#include "NiMeshLibType.h"

#include <NiSPWorkflowManager.h>

/**
    Objects of the NiMeshUpdateProcess class hold data required for
    NiAVObject::Update, and provide callback functions to be called pre-
    and post-update. NiMeshUpdateProcess contains an NiSPWorkflowManager
    object for use in submitting mesh modifier tasks, and should be used
    instead of NiUpdateProcess itself.
*/
class NIMESH_ENTRY NiMeshUpdateProcess : public NiUpdateProcess
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    
    /// @endcond
    
public:
    /** Construct an NiMeshUpdateProcess object. If the pkWorkflowManager
        argument is NULL, the constructor allocates a local object. The
        workflow manager is used for all mesh modifier Floodgate tasks
        that are submitted as part of an update with this object.
    */
    NiMeshUpdateProcess(NiSPWorkflowManager* pkWorkflowManager = NULL);
    
    /// The destructor deletes the NiSPWorkflowManager object only if it was
    /// allocated in the constructor.
    virtual ~NiMeshUpdateProcess();

    /// Obtain a pointer to the workflow manager that the process is using.
    inline NiSPWorkflowManager* GetWorkflowManager() const;

    /// This function is called by NiAVObject::Update before any updating
    /// begins. It does nothing in this class.
    virtual void PreUpdate(NiAVObject* pkObject);

    /** This function is called by NiAVObject::Update after all the objects
        in a scene graph have been updated. The pkObject argument is the
        object on which NiAVObject::Update was called. In this class, the
        PostUpdate function flushes all Floodgate tasks from the
        NiSPWorkflowManager object.
    */
    virtual void PostUpdate(NiAVObject* pkObject);

protected:
    /// Boolean indicating if the culling process owns or references the
    /// workflow manager.
    bool m_bOwnsManager;
};

NiSmartPointer(NiMeshUpdateProcess);

#include "NiMeshUpdateProcess.inl"

#endif

