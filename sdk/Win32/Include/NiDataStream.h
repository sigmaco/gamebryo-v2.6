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

#ifndef NIDATASTREAM_H
#define NIDATASTREAM_H

#include "NiTSet.h"
#include "NiDataStreamElement.h"
#include "NiCloningProcess.h"
#include "NiSPDataStream.h"
#include "NiString.h"

class NiDataStreamFactory;

/**
    Thread safe abstraction for any data needed by an NiRenderObject.

    Each NiRenderer-derived class will implement its own renderer-specific
    versions of this class. Objects of this class may also be registered
    with a NiFloodgate task as an input or output and will be locked and
    unlocked as appropriate during the course of the task's execution.
*/
// Do not change the ordering of superclasses - doing so will cause problems
// on some platforms.
class NIMESH_ENTRY NiDataStream : public NiObject, public NiSPDataStream
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareClone(NiDataStream);
    NiDeclareViewerStrings;

    /// @endcond

public:

    /**
       Enumerates how a data stream may be locked during its lifetime.

       Locking calls are passed LockType enumeration values that are checked
       against these Access flags that were set at data stream construction.

       There are three types of write settings: Static, Mutable, and Volatile.

       These have significant impact on the performance and memory
       overheads of data streams.
    */ 
    enum Access
    {
        /// Access for reading by CPU.
        ACCESS_CPU_READ = 0x01,

        /**
            Access for writing once by CPU.

            Subsequent write locks will fail. 
            Reading from the data stream is thus inherently thread-safe 
            with no enforcement needed because the contents will not change 
            after the initial write.
        */
        ACCESS_CPU_WRITE_STATIC = 0x02,

        /**            
            Access for multiple writes by CPU infrequently.

            - Contents are preserved from lock to lock. 
            - Contents may be written sparsely/incompletely. 
        */
        ACCESS_CPU_WRITE_MUTABLE = 0x04,

        /**
            Access for multiple writes by CPU frequently, such as writing
            every frame or every second.  

            - Data is not preserved from lock to lock.
            - Entire contents must written upon locking. 
            - Incompatible with ACCESS_CPU_READ.
        */
        ACCESS_CPU_WRITE_VOLATILE = 0x08,

        /**
            Access for reading by GPU, such as vertex buffers, index buffers,
            etc for GPU consumption.
        */
        ACCESS_GPU_READ = 0x10,

        /**
            Access for writing by GPU, such as vertex buffers, index buffers,
            etc for GPU output.
        */
        ACCESS_GPU_WRITE = 0x20,

        /// Mask used to see if access flags allow any form of CPU writing.
        ACCESS_CPU_WRITE_ANY = ACCESS_CPU_WRITE_MUTABLE | 
            ACCESS_CPU_WRITE_STATIC | 
            ACCESS_CPU_WRITE_VOLATILE,

        /// @cond EMERGENT_INTERNAL

        /// Indicates the static buffer has already been initialized.
        ACCESS_CPU_WRITE_STATIC_INITIALIZED = 0x40

        /// @endcond
    };

    /// Enumerates the contents of this data stream.
    enum Usage
    {
        /// Index buffer.
        USAGE_VERTEX_INDEX,

        /// Vertex buffer.
        USAGE_VERTEX,

        /// Emergent internal use only. Array of shader constants. 
        USAGE_SHADERCONSTANT,

        /// Arbitrary user-defined data.
        USAGE_USER,

        /// Display list buffer.
        USAGE_DISPLAYLIST,

        /// Maximum number of Usage types.
        USAGE_MAX_TYPES
    };

    /// Used by Lock/Unlock methods to indicate how data will be accessed.
    /// The enumerations can be OR'ed together.
    enum LockType
    {
        /// Lock the data for CPU read. ACCESS_CPU_READ must have been 
        /// specified for this to succeed.
        LOCK_READ  = NiSPDataStream::LOCK_READ,

        /// Lock the data for CPU write. ACCESS_CPU_WRITE must have been 
        /// specified for this to succeed.
        LOCK_WRITE = NiSPDataStream::LOCK_WRITE,

        /// Lock the data for CPU read regardless of Access flags.
        /// This is a "back door" internal use only path only meant for use in
        /// the tools.
        LOCK_TOOL_READ  = 0x04,

        /// Lock the data for CPU write regardless of Access flags.
        /// This is a "back door" internal use only path only meant for use in
        /// the tools.
        LOCK_TOOL_WRITE  = 0x08
    };

    /**
        Represents a subset of space within a data stream.

        Regions are logical only and do not actively contain data. They simply
        provide meta information about ranges of elements within the buffer.
        They can be created or removed and may overlap if needed. Adding or 
        removing Regions has no impact whatsoever on the underlying data 
        stream.
    */
    class NIMESH_ENTRY Region : public NiMemObject
    {
    public:
        /// Default constructor.
        Region();

        /**
            Parameterized constructor.

            @param uiStartIndex Start index of the region.
            @param uiRange Number of instances contained within the region.
        */
        Region(NiUInt32 uiStartIndex, NiUInt32 uiRange);

        /// Sets the start index of the region. This assumes the range has
        /// already been set.
        inline void SetStartIndex(NiUInt32 uiStartIndex);

        /// Sets the start index of the region and the range. This function 
        /// provides a mechanism for setting the start index and the range. If
        /// only the start index is set without specifying the range the 
        /// primitive count will most likely be wrong resulting in an overflow
        /// of the buffer.
        void SetStartIndex(NiUInt32 uiStartIndex, NiUInt32 uiRange);

        /// Gets the start index of the region.
        inline NiUInt32 GetStartIndex() const;

        /// Sets the number of instances contained within the region.
        inline void SetRange(NiUInt32 uiRange);

        /// Gets the number of instances contained within the region.
        inline NiUInt32 GetRange() const;

        /** 
            Compute the starting point of this region given an input buffer.

            @param pDataStreamStart Pointer from a DataStream Lock without 
                specifying a Region.
            @param uiStride Stride of an individual instance within the 
                region.
            @param uiElementOffsetInBytes Initial offset of the elements, used
                to obtain the start value for iterating over one of the
                elements in the region.
            @return Appropriately offset buffer location.
        */
        inline void* ComputeStart(
            void* pDataStreamStart, 
            NiUInt32 uiStride,
            NiUInt32 uiElementOffsetInBytes = 0) const;

        /** 
            Compute the end point of this region given an input buffer.

            @param pDataStreamStart Pointer from a DataStream Lock without 
                specifying a Region.
            @param uiStride Stride of an individual instance within the 
                region.
            @param uiElementOffsetInBytes Initial offset of the elements, used
                to obtain the end value for iterating over one of the
                elements in the region.
            @return Appropriately offset buffer location.
        */
        inline void* ComputeEnd(
            void* pDataStreamStart, 
            NiUInt32 uiStride,
            NiUInt32 uiElementOffsetInBytes = 0) const;

        /// Given a stride, compute the initial offset in bytes to the first
        /// element in this region.
        inline NiUInt32 ComputeOffsetInBytes(NiUInt32 uiStride) const;

        /// Given a stride, compute the size of the entire region in bytes. 
        inline NiUInt32 ComputeSizeInBytes(NiUInt32 uiStride) const;

        /// @cond EMERGENT_INTERNAL

        /// Determine if one region is equal to another.
        inline bool IsEqual(const Region &kRhs) const;

        /// @endcond

    protected:

        /// Start index of the region.
        NiUInt32 m_uiStartIndex;

        /// Number of instances contained within the region.
        NiUInt32 m_uiRange;
    };

    /** 
        @name Static Creation Methods
        Methods to assist in creating an NiDataStream.
    */
    //@{
    /**
        Helper function to create a data stream with only one element.

        @param eFormat Format of the single element.
        @param uiCount Number of instances of the element.
        @param uiAccessMask Access flag for this stream.
        @param eUsage Intended Usage for this stream.
        @param pvSource Optional buffer to read initial state from.
        @param bCreateRegion0 Should a region be created containing the
            initial count of elements?
        @param bForceCreateToolDS Should this data stream be instantiated as a
            tool data stream even if a renderer is present?
        @return Instantiated data stream.
    */
    static NiDataStream* CreateSingleElementDataStream(
        NiDataStreamElement::Format eFormat, 
        NiUInt32 uiCount, 
        NiUInt8 uiAccessMask,
        Usage eUsage, 
        const void* pvSource = NULL, 
        bool bCreateRegion0 = true, 
        bool bForceCreateToolDS = false);

     /**
        Create a data stream containing a set of elements.

        @param kElements Array of elements contained within the stream.
        @param uiCount Number of instances of the element set.
        @param uiAccessMask Access flag for this stream.
        @param eUsage Intended Usage for this stream.
        @param bForceCreateToolDS Should this data stream be instantiated as a
            tool data stream even if a renderer is present?
        @return Instantiated data stream or NULL if unable to create the 
            instance.
    */
    static NiDataStream* CreateDataStream(
        const NiDataStreamElementSet& kElements, 
        NiUInt32 uiCount, 
        NiUInt8 uiAccessMask, 
        Usage eUsage,
        bool bForceCreateToolDS = false);

    /// Create an exact duplicate of the NiDataStream object, ignoring any
    /// cloning flags and forcing an explicit duplication of all members.
    NiDataStream* CreateFullCopy();

    /// Create a duplicate of the NiDataStream object, but override the
    /// initial creation parameters.
    NiDataStream* CreateFullCopyEx(NiUInt8 uiAccessMask,
        Usage eUsage, bool bCopyRegions, bool bCopyBuffer);
    //@}

    /// Set the NiDataStreamFactory used by CreateDataStream.
    static void SetFactory(NiDataStreamFactory* pkFactory);

    /// Get the NiDataStreamFactory used by CreateDataStream.
    static NiDataStreamFactory* GetFactory();

    /// Default virtual destructor. The data stream must be unlocked before
    /// destruction.
    virtual ~NiDataStream();

    /// Get the stride of the data buffer.
    inline NiUInt32 GetStride() const;

    /// Get the size of the buffer in bytes.
    inline NiUInt32 GetSize() const;

    /// Gets the total number of items in the stream.
    inline NiUInt32 GetTotalCount() const;

    /// Gets the number of items in the given region.
    inline NiUInt32 GetCount(NiUInt32 uiRegionIdx) const;

    /// Get the access rights on the stream.
    inline NiUInt8 GetAccessMask() const;

    /// Gets the usage of the stream (index, vertex, constant).
    Usage GetUsage() const;

    /// Determine whether or not the buffer is locked by the CPU.
    inline bool GetLocked() const;
    
    /** 
        @name Lock/Unlock Methods
        Methods to lock and unlock an NiDataStream object's data.
    */
    //@{
    /**
        Locks the stream for manipulating the underlying buffer.

        @note Only one lock is permitted until Unlock is called. 

        @note After a failed call to Lock(), GetLocked() will return false.

        @param uiLockMask Bitwise combination of NiDataStream::LockType.
        @return Pointer to the data to read/write.        
    */
    void* Lock(NiUInt8 uiLockMask);

    /**
        Locks the stream for manipulating the underlying buffer.

        @note Only one lock is permitted until Unlock is called. 

        @note After a failed call to LockRegion(), GetLocked() will return 
            false.

        @param uiRegion Region of the buffer to lock.
        @param uiLockMask Bitwise combination of NiDataStream::LockType flags.
        @return Pointer to the data to read/write.
    */
    void* LockRegion(NiUInt32 uiRegion, NiUInt8 uiLockMask);

    /// Const-safe helper method to lock the data stream for read.
    inline const void* LockRead() const;

    /// Helper method to lock the data stream for write.
    inline void* LockWrite();
    
    /**
        Releases a locked stream.

        @note Safe to call after Lock(), even if Lock() fails.

        @param uiLockMask Mask used when Locking the stream.
    */
    inline void Unlock(NiUInt8 uiLockMask);

    /// Const-safe helper method to unlock for read.
    inline void UnlockRead() const;

    /// Helper method to unlock for write.
    inline void UnlockWrite();

    //@}

    /** 
        @name Floodgate Methods (from NiSPDataStream)
        Methods to hook up NiDataStream objects to NiFloodgate task
        inputs/outputs. The GetStride member function is also used by
        Floodgate.
    */
    //@{
    /// Locks a Floodgate data source.
    virtual const void* LockSource(NiUInt32 uiRegionIdx);
    
    /// Unlocks a Floodgate data source.
    virtual void UnlockSource();
    
    /// Determine the Floodgate block count for this stream.
    virtual NiUInt32 GetBlockCount(NiUInt32 uiRegionIdx) const;

    /// Cache that this stream should be locked for writing.
    virtual void EnableWriteFlag();

    /// Cache that this stream should be locked for reading.
    virtual void EnableReadFlag();
    
    /// Return the cached lock flags.
    virtual NiUInt32 GetLockFlags() const;
    
    /// Clear the cached lock flags.
    virtual void ClearLockFlags();
    //@}

    /// Adjusts the size of the stream. Contents of stream are undefined 
    /// afterwards.
    virtual void Resize(NiUInt32 uiSize) = 0;

    /// Gets the number of Regions in the stream.
    inline NiUInt32 GetRegionCount() const;

    /// Gets the Region at the index specified (const-safe).
    inline const Region& GetRegion(NiUInt32 uiRegion) const;

    /// Gets the Region at the index specified.
    Region& GetRegion(NiUInt32 uiRegion);

    /// Sets the Region at the index specified.
    inline void SetRegion(const Region& kRegion, NiUInt32 uiRegion);

    /// Add a region to this datastream. Returns the index at which the
    /// region was added.
    inline NiUInt32 AddRegion(const Region& kRegion);

    /// Remove a region from this data stream.
    inline void RemoveRegion(NiUInt32 uiIndex);

    /// Remove all regions from this data stream.
    inline void RemoveAllRegions();

    /// Gets the number of elements in the element set.
    inline NiUInt32 GetElementDescCount() const;

    /// Get the element desc at the specified index
    inline const NiDataStreamElement& GetElementDescAt(
        NiUInt32 uiDescIdx) const;

    /// Sets the cloning method.
    inline void SetCloningBehavior(
        NiObject::CloningBehavior eCloningBehavior);
    
    /// Gets the cloning method.
    inline NiObject::CloningBehavior GetCloningBehavior() const;

    /** 
        Sets the cloning method to the default value. If access is
        ACCESS_CPU_WRITE_MUTABLE, CLONE_COPY is set. If access 
        is ACCESS_CPU_WRITE_VOLATILE, CLONE_BLANK_COPY is set. Otherwise,
        the cloning method is set to CLONE_SHARE.
    */
    void SetDefaultCloningBehavior();

    /// Determines if the locking request is valid with respect to the access
    /// mask on the stream.
    virtual bool IsLockRequestValid(NiUInt8 uiLockMask) const;

    /// Determines if the access request is valid for the given usage type.
    static bool IsAccessRequestValid(NiUInt8 uiAccessMask, Usage eUsage);

    /// Whether or not the data stream should be saved to disk when streamed.
    inline bool GetStreamable() const;

    /// Specify whether or not the data stream should be saved to disk when
    /// streamed.
    inline void SetStreamable(bool bStreamable);

    /// Converts the given Access flags to a string. Uses NiString to enforce
    /// ownership of the concatenated string.
    static NiString AccessFlagsToString(NiUInt8 uiFlags);

    /// Converts the given Usage to a string. 
    static const char* UsageToString(Usage eUsage);

    /// Set whether the vertex buffer contains only a single entry intended
    // to be read repeatedly by the GPU as varying input.
    void SetGPUConstantSingleEntry(bool bSingleEntry);
    bool GetGPUConstantSingleEntry() const;

    /// @cond EMERGENT_INTERNAL

    /**
        Allows us to override the serialization system and
        specify the streamable classname as "NiDataStream" for all 
        derived classes.

        @param acName Buffer to store the streamable RTTI name.
        @param uiMaxSize Maximum size of the input buffer.
        @return Whether or not the buffer was successfully copied
    */
    virtual bool GetStreamableRTTIName(char* acName, 
        unsigned int uiMaxSize) const;

    /** 
        In the platform-specific subclasses, only the templated lock policy
        can determine when the class is locked. It must have a public
        interface to set that back on the data stream. Otherwise, we'd have
        to declare each template specialization as a friend which would be
        unwieldy.
    */
    void SetLocked(bool bLocked);

    void SaveLinkID(NiStream& kStream);
    
    /// Copy internal data members into the destination stream. If the source
    /// data stream does not have the ACCESS_CPU_READ access flag set, or the
    /// destination data stream does not have an appropriate 
    /// ACCESS_CPU_WRITE_* flag set, this function can be exceedingly slow.
    void CopyData(NiDataStream* pkDest, bool bCopyRegions, 
        bool bCopyBuffer);
    /// @endcond

protected:

    /// Hidden Constructor.
    NiDataStream(
        const NiDataStreamElementSet& kElements, 
        NiUInt32 uiCount, 
        NiUInt8 uiAccessMask, 
        Usage eUsage);
    
    /// Hidden Constructor.
    NiDataStream(NiUInt8 uiAccessMask, Usage eUsage);

    /// Create function for streaming.
    static NiDataStream* CreateDataStream( 
        NiUInt8 uiAccessMask, 
        Usage eUsage,
        bool bForceCreateToolDS = false,
        bool bCanOverrideAccessMask = true);

    /// Grant protected access to the NiDataStreamFactory.
    friend class NiDataStreamFactory;

    /// Allocates the data for a data stream. Subclasses must override.
    virtual void Allocate() = 0;

    /// Platform specific locking mechanism.
    virtual void* LockImpl(NiUInt8 uiLockMask) = 0;

    /// Platform specific unlocking mechanism.
    virtual void UnlockImpl(NiUInt8 uiLockMask) = 0;

    /// Validates that the data is properly aligned for the current platform.
    virtual bool ValidateDataAlignment(unsigned int* pauiComponentSizes,
        unsigned int uiNumComponents) const;

    /// Get the sizes of each element component.
    bool GetSizeInfo(NiUInt32* auiComponentSizes, 
        const NiUInt32 uiMaxComponentCount, 
        NiUInt32& uiActualComponentCount);

    /// Static factory for creating data streams.
    static NiDataStreamFactory* ms_pkFactory;

    /// Cloning behavior of this data stream.
    NiObject::CloningBehavior m_eCloningBehavior;

    /// Set of regions in this data stream.
    NiTObjectSet<Region> m_kRegions;

    /// Element descriptions for elements contained within this stream.
    NiDataStreamElementSet m_kElements;

    /// Size in bytes of this data stream.
    NiUInt32 m_uiSize;

    /// Usage mask of this data stream.
    Usage m_eUsage;      

    /// Access flags for this data stream.
    NiUInt8 m_uiAccessMask; 

    /// Aggregate lock flags to support NiSPDataStream locking.
    NiUInt8 m_uiLockFlags;

    /// Data stream currently locked?
    bool m_bLocked;

    /// Should the the data stream be streamed?
    bool m_bStreamable;

    /// Should this stream be interpreted as a single entry?
    bool m_bGPUConstantSingleEntry;
};

NiSmartPointer(NiDataStream);

// Do these two stream regions exactly match?
bool  NIMESH_ENTRY operator==(const NiDataStream::Region& r1, 
    const NiDataStream::Region& r2);
bool  NIMESH_ENTRY operator!=(const NiDataStream::Region& r1, 
    const NiDataStream::Region& r2);

#include "NiDataStream.inl"

#endif // #ifndef NIDATASTREAM_H
