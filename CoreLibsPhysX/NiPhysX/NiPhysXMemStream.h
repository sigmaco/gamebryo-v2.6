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

#ifndef NIPHYSXMEMSTREAM_H
#define NIPHYSXMEMSTREAM_H

#include "NiPhysXLibType.h"

#include <NiUniversalTypes.h>

#include <NiMemStream.h>

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4512) // assignment operator could not be generated
#pragma warning(disable: 4244) // conversion from 'type' to 'type', possible loss of data
#pragma warning(disable: 4245) // conversion from 'type' to 'type', signed/unsigned mismatch
#elif defined(_PS3)
#pragma GCC system_header
#endif
#include <NxPhysics.h>
#include <NxStream.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

#if defined(WIN32)
#pragma warning(push)
#pragma warning(disable: 4275) // DLL export warning due to PhysX
#endif

/**
    This class implements the PhysX's NxStream streaming interface with a
    Gamebryo NiMemStream stream.
    
    All the interface calls are passed directly to the memory stream.
    Gamebryo-PhysX uses this class in its tools as an intermediary in
    streaming cooked meshes to NIF files. It is provided an a convenience
    class to applications, primarily for use when cooking PhysX meshes. 
*/
class NIPHYSX_ENTRY NiPhysXMemStream : public NxStream, public NiMemObject
{
public:

    /// Default constructor initializes to an empty buffer.
    NiPhysXMemStream();
    
    /**
        This constructor passes the arguments directly to the constructor for
        its internal NiMemStream object.
        
        It is intended for read streams.
    */
    NiPhysXMemStream(const void* pBuffer, NiUInt32 uiSize);
    
    /// Destructor frees the buffer's memory if the application has not yet
    /// called GetBuffer.
    virtual ~NiPhysXMemStream();
    
    /// Returns the current size of the memory buffer.
    virtual NiUInt32 GetSize() const;

    /**
        Returns a pointer to the internal memory buffer.
        
        Once this function has been called, the buffer cannot be written to
        and the application is responsible for freeing the memory (using
        NiFree). 
    */
    virtual void* GetBuffer();

    /**
        Reset the internal position pointer to the first byte.
        
        Use this function when writing to and then immediately reading from
        the same stream. Call it after the writing is done. 
    */
    virtual void Reset();

    /**
        @name Methods from NxStream
        
        See the documentation for NxStream for a description of the remaining
        member functions that this class implements. 
    */
    /// @{

    virtual NxU8 readByte() const;

    virtual NxU16 readWord() const;

    virtual NxU32 readDword() const;

    virtual NxF32 readFloat() const;

    virtual NxF64 readDouble() const;

    virtual void readBuffer(void* buffer, NxU32 size) const;

    virtual NxStream& storeByte(NxU8 b);

    virtual NxStream& storeWord(NxU16 w);

    virtual NxStream& storeDword(NxU32 d);

    virtual NxStream& storeFloat(NxF32 f);

    virtual NxStream& storeDouble(NxF64 f);

    virtual NxStream& storeBuffer(const void* buffer, NxU32 size);

    /// @}

protected:
    NiMemStream* m_pkStream;
};

#if defined (WIN32)
#pragma warning(pop)
#endif

#endif // #ifndef NIPHYSXMEMSTREAM_H
