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

#ifndef NISPTASKIMPL_H
#define NISPTASKIMPL_H

// The documentation for this class is disabled to work around a problem with
// Doxygen when multiple header files with the same class name exist.

/// @cond

#include "NiFloodgateLibType.h"
#include "NiSPStreamPartitioner.h"
#include "NiSPJob.h"

class NiSPTask;

/// Encapsulates the platform-specific implementation of a task.
class NIFLOODGATE_ENTRY NiSPTaskImpl : public NiSPJob 
{
public: 
    /// The maximum number of inputs allowed for a task on this platform.
    static const NiUInt32 MAX_INPUT_COUNT;

    /// The maximum data size of any fixed input stream to a task.
    static const NiUInt32 MAX_FIXED_INPUT_SIZE;

    /// Gets the signalling kernel for this task.
    static NiSPKernel* GetSignalKernel();

    /// Constructor.
    NiSPTaskImpl();
    
    /// Destructor.
    ~NiSPTaskImpl();

    /// Gets the stream partitioner for this task.
    NiSPStreamPartitioner* GetStreamPartitioner() const;

    /// Clears the task, making it ready to be reused.
    void Clear();

    /// Initializes the task.
    virtual void Initialize(NiSPTask* pkTask);

    /// @cond EMERGENT_INTERNAL 

    /// Initializes pools.
    static void InitializePools(NiUInt32 uiPartitionPoolSize);
    
    /// Shuts down pools.
    static void ShutdownPools();

    /// @endcond

protected:
    typedef NiTObjectPool<NiSPStreamPartitioner> StreamPartPool;

    /// Object pool for stream partitioners.
    static StreamPartPool* ms_pkStreamPartPool;  
    
    static NiCriticalSection ms_kStreamPartPoolLock;     

    /// Signaling kernel.
    static NiSPKernel* ms_pkSignalKernel;    

    /// Partioner used for data decomposition.
    NiSPStreamPartitioner* m_pkPartitioner;    
};

#include "NiSPTaskImpl.inl"

/// @endcond

#endif  // #ifndef NISPTASKIMPL_H
