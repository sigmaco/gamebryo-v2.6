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

#ifndef NITOOLDATASTREAM_H
#define NITOOLDATASTREAM_H

#include "NiDataStream.h"

/**
    Represents a basic NiDataStream that can be used
    within the tool pipeline to deal with generic streams of data
    before they have been converted to a platform specific format
*/
class NIMESH_ENTRY NiToolDataStream : public NiDataStream
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;

    /// @endcond

public:

    /// Custom constructor. 
    /// This methods allocates space for data
    NiToolDataStream(
        const NiDataStreamElementSet& kElements, 
        NiUInt32 uiCount, 
        NiUInt8 uiAccessMask, 
        Usage eUsage);
    
    /// Custom constructor. 
    /// This method does not allocate space for data
    NiToolDataStream(NiUInt8 uiAccessMask, Usage eUsage);

    /// Destructor.
    virtual ~NiToolDataStream();

    /// Resizes the buffer
    virtual void Resize(unsigned int uiSize);

    /** 
        Determines if the locking request is valid. For NiToolDataStream
        objects, this will always return true.
        Applications querying the validity of the lock flags on a tool 
        data stream for the purposes of validating a platform-specific data 
        stream can call the NiDataStream::IsLockRequestValid function 
        explicitly.
        e.g., pkDataStream->NiDataStream::IsLockRequestValid(uiLockMask);
    */
    virtual bool IsLockRequestValid(NiUInt8 uiLockMask) const;

    /// Set access mode as requested if IsValidAccessRequest is true.
    void SetAccessMask(NiUInt8 uiAccessMask);    

    /// Set usage mode as requested.
    void SetUsage(NiDataStream::Usage eUsage);

    /// Set access mode to volatile, or, if read access is set, to mutable.
    void MakeWritable();

    /// Set access mode to readable, and, if write access is set, to mutable.
    void MakeReadable();

protected:

    /// Allocates the buffer
    virtual void Allocate();
    
    /// Platform specific locking mechanism
    virtual void* LockImpl(NiUInt8 uiLockMask);

    /// Platform specific unlocking mechanism
    virtual void UnlockImpl(NiUInt8 uiLockMask);

    /// Internal pointer to underlying memory 
    unsigned char* m_pucMemory;
};

NiSmartPointer(NiToolDataStream);

#endif // NITOOLDATASTREAM_H

