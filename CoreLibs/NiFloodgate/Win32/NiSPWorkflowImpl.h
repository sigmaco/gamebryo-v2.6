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

#ifndef NISPWORKFLOWIMPL_H
#define NISPWORKFLOWIMPL_H

// The documentation for this class is disabled to work around a problem with
// Doxygen when multiple header files with the same class name exist.

/// @cond

#include "NiFloodgateLibType.h"

class NiSPWorkflow;

/// Encapsulates the platform-specific implementation of a workflow.
class NIFLOODGATE_ENTRY NiSPWorkflowImpl : public NiMemObject
{
public:
    /// Constructor.
    NiSPWorkflowImpl();
    
    /// Destructor.
    ~NiSPWorkflowImpl();

    /// Initializes the workflow implementation.
    bool Initialize();

    /// Executes the workflow.
    void Execute(NiSPWorkflow* pkWorkflow);

    /// Terminates a workflow.
    void Terminate();

    /// Prepares the workflow implementation for execution.
    bool Prepare(){ return true; }
    
    /// Releases any shared resources after execution.
    void Release(){}
};

/// @endcond

#endif  // #ifndef NISPWORKFLOWIMPL_H
