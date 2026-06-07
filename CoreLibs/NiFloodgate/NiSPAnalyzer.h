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

#ifndef NISPANALYZER_H
#define NISPANALYZER_H

#include "NiFloodgateLibType.h"

#include <NiSystem.h>
#include <NiTPtrSet.h>

class NiSPTask;
class NiSPWorkflow;

/**
    This class handles the analysis phase of Floodgate workflow submission.
    
    Analysis is the process of determining an execution order of tasks within
    a workflow such that the dependencies between the inputs and outputs of
    the tasks are satisfied. Applications do not need to call this function -
    analysis is automatically managed by the stream processor.
*/
class NIFLOODGATE_ENTRY NiSPAnalyzer : public NiMemObject
{
public:

    /// Constructor.
    NiSPAnalyzer();
    
    /// Destructor.
    ~NiSPAnalyzer();

    /**
        Analyzes a workflow to determine the order in which tasks should be
        executed.

        @param pkWorkflow The workflow to analyze.
    */
    void Analyze(NiSPWorkflow* pkWorkflow);

    /// Max number of stages in a workflow
    static const NiUInt16 MAX_STAGE = 999;

    /// Max number of recursions used to check for pathologic directed 
    /// acyclic graphs.
    static const NiUInt16 MAX_RECURSIONS = 256;

protected:
    /**
        Performs analysis on a single NiSPTask.

        @param pkTask Task upon which analysis should be performed.
        @param uiRecursionDepth Current depth of the recursive analysis.
    */
    void Analyze(NiSPTask* pkTask, NiUInt16 uiRecursionDepth);
};

#endif  // #ifndef NISPANALYZER_H
