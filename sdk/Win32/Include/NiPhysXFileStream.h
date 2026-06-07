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

#ifndef NIPHYSXFILESTREAM_H
#define NIPHYSXFILESTREAM_H

#include "NiPhysXLibType.h"

#include <NiSystem.h>

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4512) // assignment operator could not be generated
#pragma warning(disable: 4244) // conversion from 'type' to 'type', possible loss of data
#pragma warning(disable: 4245) // conversion from 'type' to 'type', signed/unsigned mismatch
#elif defined(_PS3)
#pragma GCC system_header
#endif
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
    Gamebryo NiFile stream.
    
    All the interface calls are passed directly to the file stream.
    Gamebryo-PhysX uses this class to stream in external cooked mesh files. 
*/
class NIPHYSX_ENTRY NiPhysXFileStream : public NxStream, public NiMemObject
{
public:
    /// The constructor passes its arguments directly to the constructor for
    /// its internal NiFile object. 
    NiPhysXFileStream(const char *pcName, NiFile::OpenMode eMode,
        NiUInt32 uiBufferSize = 32768);

    /// The destructor closes any open files.
    virtual ~NiPhysXFileStream();
    
    /**
        Returns true if the object succeeded in creating its file and is ready
        for reading or writing.
        
        Applications should check this before using the stream. 
    */
    virtual bool Ready() const;

    /**
        @name Functions inherited from NxStream

        See the PhysX documentation for NxStream for a description of these
        member functions.
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
    NiFile* m_pkFile;
};

#if defined (WIN32)
#pragma warning(pop)
#endif


#endif // #ifndef NIPHYSXFILESTREAM_H
