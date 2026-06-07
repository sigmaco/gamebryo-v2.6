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

#ifndef NISPSTREAMPARTITIONER_H
#define NISPSTREAMPARTITIONER_H

// The documentation for this class is disabled to work around a problem with
// Doxygen when multiple header files with the same class name exist.

/// @cond

#include <NiSystem.h>
#include <NiMath.h>
#include "NiSPWorkload.h"

class NiSPJobList;
class NiSPTask;
class NiSPStream;

/// Base class for implementing data decomposition policies.
class NIFLOODGATE_ENTRY NiSPStreamPartitioner : public NiMemObject
{
public:
    /// Determines the total block for a task.
    static NiUInt32 ComputeTotalBlockCount(NiSPTask* pkTask);

    /// Constructor.
    NiSPStreamPartitioner();

    /// Destructor.
    virtual ~NiSPStreamPartitioner(){}

    /// Gets the optimal number of blocks to send at one time.
    NiUInt32 GetOptimalBlockCount();

    /// Indicates if the set has more chunks to transfer.
    bool HasMoreBlocks();

    /// Resets the set back to the start of its streams.
    void Reset();

    /// Gets the current chunk index.
    NiInt32 GetBlockIndex();

    /// Gets the number of blocks to transfer.
    NiInt32 GetTransferBlockCount();

    /// Gets the size of a chunk across all streams in bytes.
    NiUInt32 GetTransferSize();

    /// Sets the task to partition.
    void SetTask(NiSPTask* pkTask);

    /// Increases the chunk index by transfer chunk count.
    void AdvanceBlockIndex();

    /// Initializes the stream partitioner.
    virtual void Initialize(NiSPTask* pkTask);

    /**
        Drives the execution of data partitioning (decomposition) by
        calling the appropriate subclass's routines for partitioning
        the data in each NiSPStream for the current NiSPTask.

        @param pkJobList Current NiSPJobList instance.
        @param uiJobIndex Index of the NiSPJob within the list.
        @param uiInputSize Total size of input data.
        @param uiOutputSize Total size of output data.
    */
    virtual void Partition(NiSPJobList* pkJobList, NiUInt32 uiJobIndex,
        NiUInt32& uiInputSize, NiUInt32& uiOutputSize);

    /**
        Partitions an input stream for the current task.

        @param pkJobList Current NiSPJobList instance.
        @param uiJobIndex Index of the NiSPJob within the list.
        @param pkStream Stream to partition.
        @param kWorkloadHeader Header to fill out with job related info.
        @return Size in bytes of the input data added to the job.
    */
    virtual NiUInt32 PartitionInput(NiSPJobList* pkJobList,
        NiUInt32 uiJobIndex, NiSPStream* pkStream,
        NiSPWorkload::Header& kWorkloadHeader);

    /**
        Partitions an output stream for the current task.

        @param pkJobList Current NiSPJobList instance.
        @param uiJobIndex Index of the NiSPJob within the list.
        @param pkStream Stream to partition.
        @param kWorkloadHeader Header to fill out with job related info.
        @return Size in bytes of the output data added to the job.
    */
    virtual NiUInt32 PartitionOutput(NiSPJobList* pkJobList,
        NiUInt32 uiJobIndex, NiSPStream* pkStream,
        NiSPWorkload::Header& kWorkloadHeader);

protected:
    /// Computes an estimate for the optimal block count based on the task.
    void EstimateOptimalBlockCount();

    /// Number of blocks to transfer at a time.
    NiUInt32 m_uiOptimalBlockCount;

    /// Pointer to current block to transfer.
    NiUInt32 m_uiCurrentBlockIndex;
    
    /// Number of blocks per stream.
    NiUInt32 m_uiTotalBlockCount;    

    /// Size of one block in bytes.
    NiUInt32 m_uiTransferSize;       

    /// Task containing streams.
    NiSPTask* m_pkTask;               

};

#include "NiSPStreamPartitioner.inl"

/// @endcond

#endif  // #ifndef NISPSTREAMPARTITIONER_H
