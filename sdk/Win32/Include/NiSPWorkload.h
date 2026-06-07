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

#ifndef NISPWORKLOAD_H
#define NISPWORKLOAD_H

// The documentation for this class is disabled to work around a problem with
// Doxygen when multiple header files with the same class name exist.

/// @cond

#include <NiSystem.h>
#include "NiMemObject.h"
#include "NiFloodgateLibType.h"

typedef NiUInt8 NiSPSignalType;

class NiSPKernel;

/**
    Stores data associated with a single block of Floodgate work.
    
    This includes the kernel and information about all of the streams that the
    kernel is to operate on. A single task may get broken into many workloads
    in order to meet restrictions on DMA transfers and the like.
*/
class NIFLOODGATE_ENTRY NiSPWorkload : public NiMemObject
{
public:
    friend class NiSPStreamPartitioner;

    /// @name Signal Types
    /// Static constants defining an enumeration for NiSPSignal type.
    //@{
    static const NiUInt32 SIGNAL_NONE = 0;
    static const NiUInt32 SIGNAL_COMPLETION = 1;
    static const NiUInt32 SIGNAL_GPU_COMPLETION = 2; 
    static const NiUInt32 SIGNAL_PPU_TASK_NOTIFY = 3;
    //@}

    /// Header to manage workload meta information.
    struct Header                       
    {
        /// Base address of the data stream.
        void* m_pvBaseAddress;   

        /// Offset in bytes from the base address to the section of the data
        /// the active kernel is working with.
        NiUInt32 m_uiDataOffset;  

        /// The number of blocks the active kernel is operating on.
        NiUInt32 m_uiBlockCount;

        /// The size in bytes of each block.
        NiUInt32 m_uiStride;
    };                                      

    /// Constructor: no memory is allocated for input and output streams.
    NiSPWorkload();
    
    /// Destructor frees memory allocated for inputs and outputs.
    ~NiSPWorkload();
    
    /// Gets an input stream by index including the data offset.
    template <class T> T* GetInput(NiUInt16 uiInputIndex);

    /// Gets an input stream by index NOT including the data offset.
    template <class T> T* GetInputBase(NiUInt16 uiInputIndex);

    /// Gets an input stream's stride.
    NiUInt32 GetInputStride(NiUInt16 uiInputIndex);

    /// Gets an output stream by index including the data offset.
    template <class T> T* GetOutput(NiUInt16 uiOutputIndex);

    /// Gets an output stream by index NOT including the data offset.
    template <class T> T* GetOutputBase(NiUInt16 uiOutputIndex);

    /// Gets an output stream's stride.
    NiUInt32 GetOutputStride(NiUInt16 uiOutputIndex);

    /// Gets the number of chunks to process.
    NiUInt32 GetBlockCount();

    /// Gets the number of inputs.
    NiUInt16 GetInputCount();

    /// Gets the number of output streams.
    NiUInt16 GetOutputCount();

    /// Gets the kernel for this workload.
    NiSPKernel* GetKernel();

    /// Sets the kernel for this workload.
    void SetKernel(NiSPKernel* pKernel);

    /**
        Clears the workload.
        
        This frees memory.
    */
    void Clear();
    
    /**
        Resets the workload and makes sure it has the required number of
        input and output stream headers.
        
        If told to compact, it reallocates internal memory to minimize
        comsumption.
    */
    void Reset(NiUInt32 uiInputCount, NiUInt32 uiOutputCount,
        bool bCompact = false);

    /// Attempt to prime the cache with the entire workload.
    /// This is a no-op on Win32.
    void Preload();

protected:
    /**
        Information about each input and output stream.
        
        Inputs come first, followed by outputs.
    */
    Header* m_pkInputsOutputs;

    /// The number of available inputs and outputs.
    NiUInt32 m_uiInOutCount;

    /// The number of input streams.
    NiUInt16 m_uiInputCount;

    /// The number of output streams.
    NiUInt16 m_uiOutputCount;

    /// The number of blocks of data.
    NiUInt32 m_uiBlockCount;

    /// The start offset for this workload within the total task.
    NiUInt32 m_uiRangeStart;

    /// The kernel.
    NiSPKernel* m_pkKernel;
};

#include "NiSPWorkload.inl"

/// @endcond

#endif  // #ifndef NISPWORKLOAD_H
