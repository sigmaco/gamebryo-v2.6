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

#ifndef NISPSTREAM_H
#define NISPSTREAM_H

#include "NiFloodgateLibType.h"
#include "NiSPDataStream.h"
#include <NiSmartPointer.h>
#include <NiMemObject.h>
#include <NiTArray.h>
#include <NiSystem.h>
#include <NiTSet.h>
#include <NiFlags.h>

class NiSPTask;
typedef NiTPrimitiveArray<NiSPTask*> NiSPTaskArray;

/**
    This class represents an encapsulation of a data buffer for use with
    Floodgate.

    Users of this class are encouraged to use the templated versions
    NiTSPStream and NiTSPFixedInput for ease of stream creation.
*/
class NIFLOODGATE_ENTRY NiSPStream : public NiMemObject
{
    NiDeclareFlags(NiUInt8);

public:
    /**
        Constructs a new NiSPStream from the parameters specified.

        @param pvData Data to be referenced by the NiSPStream.
        @param uiStride The stride of the data.
        @param uiBlockCount Number of stride-sized elements in the NiSPStream.
        @param bIsFixedInput If false, data should be decomposed; otherwise it
            is fixed.
    */
    NiSPStream(void* pvData = NULL, NiUInt32 uiStride = 0, 
        NiUInt32 uiBlockCount = 0, bool bIsFixedInput = false);
    
    /// Destructor.
    virtual ~NiSPStream();

    /// Returns a pointer to the data contained within the stream.
    inline void* GetData();
    
    /// Sets the pointer to the data contained within the stream.
    inline void SetData(void* pvData);
    
    /**
        Sets the data source (as opposed to the RAW pointer).
        
        The stride of this stream will always be taken from the data source
        and is assumed not to change after this call has been made.
        
        The data source pointer is not owned by the NiSPStream object. It is
        the responsibility of the calling code to manage the pointer. The
        data source pointer must remain valid (not deleted, for instance)
        until this NiSPStream object is deleted or another, possibly NULL,
        source is set.

        @param pkDataStreamSource Pointer to an object that implements the
            NiSPDataStream interface.
        @param bAutoSetBlockCount Indicates if the block count should be
            determined from the NiSPDataStream or if it will be set manually.
        @param uiRegionIdx Region to use within the data source.
        @param uiElementOffset Offset in the stream at which the 
            element being worked on starts.
    */
    inline void SetDataSource(NiSPDataStream* pkDataStreamSource, 
        bool bAutoSetBlockCount = false, NiUInt32 uiRegionIdx = 0,
        NiUInt32 uiElementOffset = 0);

    /// Gets the data source (as opposed to the RAW pointer).
    inline NiSPDataStream* GetDataSource() const;

    /// Indicates if the block count should be automatically set based
    /// on the data source whenever it is locked.
    inline bool GetAutoSetBlockCount() const;
    
    /// Sets the flag indicating whether or not the block count should be 
    /// automatically set from the NiSPDataStream whenever it is locked.
    inline void SetAutoSetBlockCount(bool bAutoSetBlockCount);

    /// Gets region index to use with the data source.
    inline NiUInt32 GetRegionIdx() const;
    
    /// Sets region index to use with the data source.
    inline void SetRegionIdx(NiUInt32 uiRegionIdx);

    /// Gets the element offset to use with the data source.
    inline NiUInt32 GetElementOffset() const;

    /// Sets the element offset to use with the data source.
    inline void SetElementOffset(NiUInt32 uiElementOffset);

    /**
        Clears the internal arrays of input and output tasks.
        
        For thread safety, functions internal to this class set entries in
        these arrays to zero rather than remove them. This function actually
        removes the entries.
    */
    inline void ClearTaskArrays();

    /// @cond EMERGENT_INTERNAL 
    
    /// Gets pointer to the data pre-aligned to the platform alignment.
    void* GetAlignedData();
    
    /// Sets pointer to the data pre-aligned to the platform alignment.
    void SetAlignedData(void*);
    
    /// Indicates if the data is already aligned to a 128 byte boundary.
    bool IsDataAligned();

    /// Gets the size of the data.
    NiUInt32 GetDataSize();
    
    /// Indicates if the stream contains data for a fixed input. Fixed input
    /// data is not partitioned.
    bool IsFixedInput();

    /// Gets the stride of the data.
    NiUInt16 GetStride();
    
    /// Sets the stride of the data.
    void SetStride(NiUInt16 uiStride);
    
    /// Gets the number of elements in the stream.
    NiUInt32 GetBlockCount();
    
    /// Sets the number of elements in the stream.
    void SetBlockCount(NiUInt32 uiBlockCount);
    
    /**
        Adds a task for which this stream will be an input.
        
        Should only be called by NiSPTask.
    */
    void AddInput(NiSPTask* pkTask);
    
    /**
        Removes a task for which this stream has been an input.
        
        Should only be called by NiSPTask.
    */
    void RemoveInput(NiSPTask* pkTask);

    /**
        Gets a task for which this stream will be an input.
        
        Return value can be NULL.
    */
    NiSPTask* GetInputAt(NiUInt32 uiIndex) const;
    
    /**
        Gets the size of the array used to hold tasks for which this stream is
        an input.
        
        Some entries in this array can be NULL.
    */
    NiUInt32 GetInputSize() const;

    /// Gets the number of tasks for which this stream is an input.
    inline NiUInt32 GetEffectiveInputSize() const;

    /**
        Adds a task for which this stream will be an output.
        
        Should only be called by NiSPTask.
    */
    void AddOutput(NiSPTask* pkTask);

    /**
        Removes a task for which this stream has been an output.
        
        Should only be called by NiSPTask.
    */
    void RemoveOutput(NiSPTask* pkTask);

    /**
        Gets a task for which this stream will be an output.
        
        Return value can be NULL.
    */
    NiSPTask* GetOutputAt(NiUInt32 uiIndex) const;

    /**
        Gets the size of the array used to hold tasks for which this stream is
        an output.
        
        Some entries in this array can be NULL.
    */
    NiUInt32 GetOutputSize() const;

    /// Gets the number of tasks for which this stream is an output.
    inline NiUInt32 GetEffectiveOutputSize() const;

    /// Gets the pointer to the block at the index specified.
    void* GetDataForBlockIndex(NiUInt32 uiBlockIndex);

    /// Gets an aligned pointer to the block at the index specified.
    void* GetAlignedDataForBlockIndex(NiUInt32 uiBlockIndex);

    /// Gets the data offset in bytes for the block index specified.
    NiUInt32 GetDataOffsetForBlockIndex(NiUInt32 uiBlockIndex);

    /// Prepares the stream for partitioning.
    void Prepare();
    
    /// Locks this stream's underlying data source.
    void Lock();
    
    /// Unlocks this stream's underlying data source.
    void Unlock();

    /// @endcond 

protected:
    /// Enumeration of flags.
    enum Flags {
        /// Is the stream for a fixed input?
        FLAG_ISFIXEDINPUT = 0x01,

        /// Take block count from source?
        FLAG_AUTOSETBLOCKCOUNT = 0x02
    };
    
    /// Sets the block count.
    inline void Do_SetBlockCount(NiUInt32 uiBlockCount);

    /// Resets the cached state of tasks dependent on this stream.
    void NotifyDependentTasks();

    /**
        Completely removes this stream from all tasks using it.
        
        Called in destructor.
    */
    void RemoveFromDependentTasks();

    /**
        Indicates whether or not the stream is already locked.
        
        This member variable is placed here to pack in with the flags member.
    */
    bool m_bIsLocked;
    
    /**
        The stride of the data stream.
        
        This member variable is placed here to pack in with the flags member.
    */
    NiUInt16 m_uiStride;             

    /**
        A collection of tasks for which this stream is an input.
        
        This array may contain NULL elements.
    */
    NiSPTaskArray m_kInputs;              
   
    /**
        A collection of tasks for which this stream is an output.
        
        This array may contain NULL elements.
    */
    NiSPTaskArray m_kOutputs;             
    
    /// Pointer to data buffer.
    void* m_pvData;               
    
    /// Size of data buffer.
    NiUInt32 m_uiDataSize;    
    
    /// The underlying data source, if there is one.
    NiSPDataStream* m_pkDataStreamSource;
    
    /// The current region index, used only if m_pkDataStreamSource is
    /// non-NULL.
    NiUInt32 m_uiRegionIdx;

    /// The offset into m_pvData where the element being worked on
    /// can be found.
    NiUInt32 m_uiElementOffset;

    /// Number of blocks in buffer.
    NiUInt32 m_uiBlockCount;         
};
    
NiSmartPointer(NiSPStream);

#include "NiSPStream.inl"

///    Helper class for constructing streams from known data types.
template <class T>
class NiTSPStream : public NiSPStream
{
public:
    /// Constructor: sets the internal data to NULL.
    NiTSPStream() : NiSPStream(NULL, sizeof(T), 0){}

    /// Constructor: takes a pointer to the data and the number of elements.
    NiTSPStream(T* pData, NiUInt32 uiBlockCount) : 
      NiSPStream(pData, sizeof(T), uiBlockCount){}

    /// Virtual destructor.
    inline virtual ~NiTSPStream(){}
};

///    Helper class for constructing fixed streams of a known data types.
template <class T>
class NiTSPFixedInput: public NiSPStream
{
public:
    /// Constructor: sets the internal data to NULL.
    NiTSPFixedInput() : NiSPStream(NULL, sizeof(T), 0, true){}

    /// Constructor: takes a pointer to the data and the number of elements.
    NiTSPFixedInput(T* pData, NiUInt32 uiBlockCount) : 
      NiSPStream(pData, sizeof(T), uiBlockCount, true){}

    /// Virtual destructor.
    inline virtual ~NiTSPFixedInput(){}
};

#endif  // #ifndef NISPSTREAM_H
