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

//---------------------------------------------------------------------------
#ifndef NIBATCHEDUPDATEPROCESS_H
#define NIBATCHEDUPDATEPROCESS_H
//---------------------------------------------------------------------------

#include "NiMeshUpdateProcess.h"

//---------------------------------------------------------------------------

/**
    Update process for use when batch updating multiple independent scene
    graphs. Unlike NiMeshUpdateProcess, this class will not flush accumulated
    task groups at the end of a call to Update. Instead, all task groups will
    be flushed when an explicit call to Flush() is made. Hence, the use of
    this update process may result in fewer, larger workflow submissions to
    Floodgate, which may be more efficient.
*/
class NIMESH_ENTRY NiBatchedUpdateProcess : public NiMeshUpdateProcess
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;

    /// @endcond

public:
    /**
        Constructs a new NiBatchedUpdateProcess object.
       
        If no workflow manager
        is specified, it will create an internal workflow manager with default
        parameters that will be owned by this object. Otherwise, this object
        will use the passed-in workflow manager, but will not own it.
    **/
    NiBatchedUpdateProcess(NiSPWorkflowManager* pkWorkflowManager = NULL);

    /// Destructor.
    virtual ~NiBatchedUpdateProcess();

    /// Perform work before an object is updated.
    ///
    /// This implementation intentionally does no work in PreUpdate.
    virtual void PreUpdate(NiAVObject* pkObject);

    /// Perform work after an object is updated.
    ///
    /// This implementation intentionally does no work in PostUpdate.
    virtual void PostUpdate(NiAVObject* pkObject);

    /**
        Flushes accumulated task groups to the associated workflow manager.

        This function should be called immediately after all of the scene
        graph nodes have been updated, but before moving on to other work,
        such as culling or rendering, to maximize latency hiding.
    **/
    void Flush();
};

//---------------------------------------------------------------------------
#endif
//---------------------------------------------------------------------------
