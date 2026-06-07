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

#ifndef NITTHREADLOCAL_H
#define NITTHREADLOCAL_H

#include "NiUniversalTypes.h"
#include "NiSystemLibType.h"

#if defined(_PS3)
#include <pthread.h>
#else
#include "NiRTLib.h"
#endif

template <class T> class NiTThreadLocal
{
public:
    NiTThreadLocal();
    NiTThreadLocal(const T& kObject);
    ~NiTThreadLocal();

    inline bool IsValid() const;

    // implicit conversions
    operator T() const;

    // assignment
    NiTThreadLocal& operator=(const T& kObject);

private:

#if defined(WIN32) || defined(_XENON)
    DWORD m_kTLSHandle;
    typedef void* ThreadLocalReturnType;
#elif defined (_PS3)
    pthread_key_t m_kTLSHandle;
    static const pthread_key_t INVALID_KEY = 0xFFFFFFFF;
    typedef void* ThreadLocalReturnType;
#elif defined(_WII)
    NiUInt32 m_kTLSHandle;
    typedef void* ThreadLocalReturnType;
#endif  //#if defined(WIN32)

    union InternalTypeConverter
    {
        ThreadLocalReturnType m_kInternal;
        T m_kTyped;
    };

};

#include "NiTThreadLocal.inl"

#endif  //#ifndef NITTHREADLOCAL_H
