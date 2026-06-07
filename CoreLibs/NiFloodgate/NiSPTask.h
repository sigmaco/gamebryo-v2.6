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

#ifndef NISPTASK_H
#define NISPTASK_H

#include "NiFloodgateLibType.h"

#include <NiFlags.h>
#include <NiSmartPointer.h>
#include <NiSystem.h>
#include <NiTPtrSet.h>
#include <NiTPool.h>
#include <NiString.h>

#include "NiSPStream.h"
#include "NiSPTaskImpl.h"

class NiSPKernel;
class NiSPStreamPartitioner;
class NiSPWorkflow;
class NiSPTask;
NiSmartPointer(NiSPTask);

/**
    This class is the encapsulation of a Floodgate task.
    
    An NiSPTask represents one stage in an NiSPWorkflow and is used to
    parallelize the execution of a given operation on one or more streams of
    data.
 */
class NIFLOODGATE_ENTRY NiSPTask : public NiRefObject
{
    friend class NiSPWorkflow;

public:
    /// Constant value to use to obtain default behavior for the optimal
    /// block count.
    static const NiUInt32 AUTO_BLOCK_COUNT;

    /**
        Initializes object pools.

        @param uiTaskPoolSize Number of tasks to be allocated in task pool.
    */
    static void InitializePools(NiUInt32 uiTaskPoolSize);

    /// Shuts down object pools.
    static void ShutdownPools();

    /**
        Get a new task.
        
        This task comes from a pool of available tasks and is released 
        automatically when the reference count goes to zero.
        
        @param uiNumInputs A hint as to the number of input streams this
             task will have.
        @param uiNumOutputs A hint as to the number of output streams this
             task will have.
        @return A smart pointer to the new task.
    **/
    static NiSPTaskPtr GetNewTask(const NiUInt16 uiNumInputs = 1,
        const NiUInt16 uiNumOutputs = 1);

    /// Specifies the kernel to be used for the task.
    inline void SetKernel(NiSPKernel* pkKernel);

    /// Gets the kernel to be used for the task.
    inline NiSPKernel* GetKernel() const;

    /// Gets the number of input streams.
    inline NiUInt32 GetInputCount() const;

    /// Gets the number of output streams.
    inline NiUInt32 GetOutputCount() const;

    /// Gets the total number of streams.
    inline NiUInt32 GetTotalCount() const;

    /**
        Gets an input stream by index.

        @param uiIndex The index of the NiSPStream.
        @return The NiSPStream at the index specified.
    */
    inline NiSPStream* GetInputAt(NiUInt32 uiIndex) const;

    /**
        Gets an output stream by index.

        @param uiIndex The index of the NiSPStream.
        @return The NiSPStream at the index specified.
    */
    inline NiSPStream* GetOutputAt(NiUInt32 uiIndex) const;

    /// Adds an input stream to the task.
    inline void AddInput(NiSPStream* pkInputStream);

    /// Adds an output stream to the task.
    inline void AddOutput(NiSPStream* pkOutputStream);

    /// Removes an input stream from the task.
    inline void RemoveInput(NiSPStream* pkInputStream);

    /// Removes an output stream from the task.
    inline void RemoveOutput(NiSPStream* pkOutputStream);

    /// Returns whether or not the task can be cached by a workflow.
    inline bool IsCacheable() const;

    /// Specifies whether or not the task can be cached by a workflow.
    inline void SetIsCacheable(bool bIsCacheable);

    /**
        Returns whether or not the task should be compacted when it is
        initialized.

        The memory for a task comes from pools of pre-allocated objects that
        themselves contain heap allocated data, which is not by default
        re-sized to match the exact requirements of the task. If the task
        is compacted, all dynamically allocated data is re-allocated upon
        initialization to minimize memory consumption. There is no per-frame
        overhead to compacting if the size and number of input streams does
        not change.
        
        Tasks that never modify the properties of their data streams (block
        count, number of inputs and output, etc) after initial set-up should
        set compact to true. Tasks that do modify their streams might choose
        to compact if memory is tight and particularly if the number of
        input and output streams changes significantly.
    */
    inline bool IsCompacted() const;

    /// Specifies whether or not the task is compacted.
    inline void SetIsCompacted(bool bIsCompacted);

    /**
        Specifies the optimal block count for the task.
        
        The optimal block count determines the size of each chunk of data for
        one kernel invocation. The total task is broken into sub-tasks, each
        receiving this many elements to compute. The optimal block count is
        typically set to a platform-specific default found by analyzing the
        data streams for the task and taking into account the specific
        platform characteristics. Applications may choose to set it
        themselves, although care must be taken.
        
        To override the default, call this function on the task before
        the task is submitted in a workflow. To restore the default, call
        this function with the value NiSPTask::AUTO_BLOCK_COUNT.
    */
    inline void SetOptimalBlockCount(NiUInt32 uiBlockCount);

    /// Gets the current optimal block count for the task.
    inline NiUInt32 GetOptimalBlockCount();

    /// Sets the task to be decomposed prior to execution.
    inline void SetIsDataDecompositionEnabled(bool bEnable);
   
    /// Indicates if the task is to be decomposed prior to execution.
    inline bool IsDataDecompositionEnabled();

    /// @cond EMERGENT_INTERNAL 
    
    enum Status
    {
        IDLE = 0,
        PENDING,
        RUNNING,
        COMPLETED,
        ABORTED
    };
    
    /// Sets the synchronization data to the 64-bit value specified.
    void SetSyncData(NiUInt64 uiExtraData);

    /// Gets the synchronization data.
    NiUInt64 GetSyncData() const;

    /// Sets the signaling type to be used for the task.
    void SetSignalType(NiSPSignalType eSignalType);
    
    /// Gets the signaling type to be used for the task.
    NiSPSignalType GetSignalType();

    /// Gets the id for the task.
    void SetId(NiUInt32 uiId);

    /// Gets the id for the task.
    NiUInt32 GetId() const;

    /// Gets the signal id for the task.
    NiUInt32 GetSignalId() const;

    /// Sets the signal id for the task.
    void SetSignalId(NiUInt32);

    /// Sets the workflow that the task is in.
    void SetWorkflow(NiSPWorkflow* pkWorkflow);
    
    /// Gets the workflow that the task is in.
    NiSPWorkflow* GetWorkflow() const;

    /// Gets the current status of the task.
    Status GetStatus() const;

    /// Gets the stage number for the task.
    unsigned short GetStage() const;

    /// Sets the stage number for the task.
    void SetStage(unsigned short usStage);
   
    /// Gets the state of the marked flag.
    bool IsMarked() const;

    /// Sets the state of the marked flag.
    void SetIsMarked(bool bIsMarked);

    /// Indicates whether or not the task is a root.
    bool IsRoot() const;

    /// Indicates whether or not the task is a leaf.
    bool IsLeaf() const;

    /// Indicates whether or not the task is a synchronization task.
    bool IsSync() const;

    /// Indicates whether or not the task's streams contain aligned data.
    bool IsAligned() const;
    
    /// Indicates whether or not the task is currently cached.
    bool IsCached() const;

    /// Sets whether or not the task is cached.
    inline void SetIsCached(bool bCached);

    /// Indicates whether or not the task is currently enabled.
    bool IsEnabled() const;

    /// Sets whether or not the task is currently enabled.
    void SetIsEnabled(bool bIsEnabled);

    /// Gets the implementation for the task.
    NiSPTaskImpl& GetImpl();

    /**
        Prepares the task for scheduling and execution.
        
        Once this function has been called, there can be no modification of
        the input and output data stream arrays until Finalize has been
        called.
    */
    void Prepare();

    /// Releases any locked data streams and the spin-lock on input and
    /// output stream arrays.
    void Finalize();

    /// Clears the task. 
    void Clear(bool bIgnoreCaching = false);

    /// Resets the task for another run.
    void Reset(bool bForceReset = false);

#if defined(_WII)
    /** 
        @wii_begin 
        Store all input data to main memory, and invalidate the cache 
        for all output data.  Also, and store the next 32 bytes after the 
        end of output streams since it may be DMA'd in. 
        @wii_end
    */
    void PrepareDataForDMAs();
#endif
    
    /// @endcond

protected:

    /// Allow NiTPool access to protected constructors.
    friend class NiTNewInterface<NiSPTask>;

    /// NiSPTask Pool.
    static NiTObjectPool<NiSPTask>* ms_pkTaskPool; 
    
    /// Critical section for accessing the task pool.
    static NiCriticalSection ms_kTaskPoolLock;

    /// Returns a task to the pool of unused tasks.
    virtual void DeleteThis();

    typedef NiTPrimitivePtrSet<NiSPStream*> SPStreamPrimitivePtrSet;
    class StreamPtrSet : public SPStreamPrimitivePtrSet
    {
    public:
        inline void RemoveAllInput(NiSPTask* pkTask);
        inline void RemoveAllOutput(NiSPTask* pkTask);
    };

    // Methods

    /// Constructor hidden - public creation method is GetNewTask
    NiSPTask();

    /// Virtual Destructor
    virtual ~NiSPTask();

    /// Sets the current status of a task.
    void SetStatus(Status eStatus);

    /// Acquire the stream guarding lock.
    inline void LockStreams();
    
    /// Release the stream lock.
    inline void UnlockStreams();

    /// Per-instance critical section for accessing the arrays of data streams.
    NiCriticalSection m_kStreamsLock;

    /// Parent Workflow - regular pointer used to avoid circular dependency
    NiSPWorkflow* m_pkWorkflow;           
    
    /// Input streams.
    StreamPtrSet m_kInputStreams;        

    /// Output streams.
    StreamPtrSet m_kOutputStreams;       

    /// id for task.
    NiUInt32 m_uiTaskId;             

    /// id for signal.
    NiUInt32 m_uiSignalId;           

    /// Private implementation.
    NiSPTaskImpl m_kImpl;

    /// Work function proxy.
    NiSPKernel* m_pkKernel; 
    
    /// Size of slice.
    NiUInt32 m_uiSliceSize;
    
    /// Task status.
    Status  m_eStatus;              
    
    /// Synchronization data.
    NiUInt64 m_uiSyncData;           
    
    /// Optimal transfer size. 
    NiUInt32 m_uiOptimalBlockCount;  
    
    /// Task stage.
    unsigned short m_usStage; 

    NiDeclareFlags(NiUInt16);

    /// Enumeration for the various state flags.
    enum Flags {
        /// Marker for graph traversal.
        FLAG_IS_MARKED = 0x0001,

        /// Is this a root node in the DAG.
        FLAG_IS_ROOT = 0x0002,

        /// Is this a leaf node in the DAG.
        FLAG_IS_LEAF = 0x0004,

        /// Is this a synchronization task.
        FLAG_IS_SYNC = 0x0008,

        /// Is all data aligned in the streams for this task.
        FLAG_IS_ALIGNED = 0x0010,

        /// Is this task cached.
        FLAG_IS_CACHED = 0x0020,

        /// Is data decomposition enabled for this task.
        FLAG_IS_DATA_DECOMP_ENABLED = 0x0040,

        /// Is task currently enabled for execution.
        FLAG_IS_ENABLED = 0x0080,
        
        /// Does the task need to be compacted.
        FLAG_IS_COMPACTED = 0x0100,

        /// Has this object been reset and not subsequently initialized.
        FLAG_IS_RESET = 0x0200
    };    

    /// Type of signal.
    NiSPSignalType m_eSignalType;          
};

#include "NiSPTask.inl"

#endif  // #ifndef NISPTASK_H
