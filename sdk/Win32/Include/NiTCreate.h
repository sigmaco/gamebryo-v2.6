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

// NifPropertyWindow.h

#ifndef NITCREATE_H
#define NITCREATE_H

#include <NiMemoryDefines.h>

// In some cases, it may be necessary to use these templated create
// functions to allocate an object instead of calling NiNew directly.
// This is especially true in MFC, where "new" is overridden
// by a macro to mean DEBUG_NEW. Since NiNew is a substitution for
// new, we run into a problem. Calling NiTCreate<object>() will
// allow a client to allocate through Gamebryo in such a circumstance.

template<class TYPE> 
inline TYPE* NiTCreate()
{
    return NiNew TYPE();
}

template<class TYPE, class ARG1> 
inline TYPE* NiTCreate1(ARG1 a)
{
    return NiNew TYPE(a);
}

template<class TYPE, class ARG1, class ARG2> 
inline TYPE* NiTCreate2(ARG1 a, ARG2 b)
{
    return NiNew TYPE(a, b);
}

template<class TYPE, class ARG1, class ARG2, class ARG3> 
inline TYPE* NiTCreate3(ARG1 a, ARG2 b, ARG3 c)
{
    return NiNew TYPE(a, b, c);
}

template<class TYPE, class ARG1, class ARG2, class ARG3, class ARG4> 
inline TYPE* NiTCreate4(ARG1 a, ARG2 b, ARG3 c, ARG4 d)
{
    return NiNew TYPE(a, b, c, d);
}

#endif  // #ifndef NITCREATE_H
