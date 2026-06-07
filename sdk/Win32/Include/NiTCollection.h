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

#ifndef NITCOLLECTION_H
#define NITCOLLECTION_H

#include "NiRTLib.h"
#include <NiMemObject.h>

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4714)
#endif

template <class T> class NiTMallocInterface 
{
public:
    inline static T* Allocate(unsigned int uiNumElements)
        {return NiAlloc(T, uiNumElements);}
    inline static void Deallocate(T* pArray)
        {NiFree(pArray);}
};

template <class T> class NiTNewInterface 
{
public:
    inline static T* Allocate(unsigned int uiNumElements)
        {return NiNew T[uiNumElements];}
    inline static void Deallocate(T* pArray)
        {NiDelete [] pArray;}
};

template <class T> class NiTExternalNewInterface 
{
public:
    inline static T* Allocate(unsigned int uiNumElements)
        {return NiExternalNew T[uiNumElements];}
    inline static void Deallocate(T* pArray)
        {NiExternalDelete [] pArray;}
};

#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

#endif // NITCOLLECTION_H
