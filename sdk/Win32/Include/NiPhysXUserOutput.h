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

#ifndef NIPHYSXUSEROUTPUT_H
#define NIPHYSXUSEROUTPUT_H

#include "NiPhysxLibType.h"

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
#include <NxPhysics.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

#if defined(WIN32)
#pragma warning(push)
#pragma warning(disable: 4275) // DLL export warning due to PhysX
#endif

/**
    This class implements the PhysX's NxUserOutputStream interface with
    Gamebryo's various debug output and assertion macros.

    If no alternative is explicitly passed to NiPhysXManager::Initialize
    then an instance of this class is created and passed to the PhysX SDK
    initialization function. Users wishing to override this behavior should
    create their own class that implements NxUserOutputStream and pass it to
    NiPhysXManager::Initialize. 
*/
class NIPHYSX_ENTRY NiPhysXUserOutput :
    public NxUserOutputStream, public NiMemObject
{
public:

    /// Constructs a string from the given information and passes it to
    /// NiOutputDebugString. 
    virtual void reportError(NxErrorCode code, const char *message,
        const char *file, int line); 

    /**
        Constructs a string from the given information and passes it to
        NiOutputDebugString, and then asserts.
        
        This behavior mimics the PhysX default output stream. 
    */
    virtual NxAssertResponse reportAssertViolation (const char *message,
        const char *file, int line);

    /// Passes the message directly to NiOutputDebugString. 
    virtual void print (const char *message);    
};

#if defined (WIN32)
#pragma warning(pop)
#endif

#endif  // #ifndef NIPHYSXUSEROUTPUT_H

