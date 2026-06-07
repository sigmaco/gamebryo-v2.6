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

#ifndef NITLIST_H
#define NITLIST_H

#include "NiRTLib.h"

// While the template lists work best for pointer types T, other types
// certainly will work when they satisfy the conditions below.  The template
// class assumes that type T has the following:
//   1.  A "zero" element (i.e., T var; var = 0; is supported)
//       which is considered to be a null array element.
//   2.  The default constructor for T must exist and create the
//       "zero" element.  The constructor must also handle all necessary
//       actions for constructing elements.  That is, the template array
//       class cannot make any post-construction member calls that are
//       specific to class T.
//   3.  Copy constructor T::T(const T&) must work properly.  The class T is
//       responsible for implementing this if need be.
//   4.  The destructor must handle all necessary actions for destroying
//       elements.  That is, the template array class cannot make any
//       pre-destruction member calls that are specific to class T.
//   5.  bool operator== (const T&);
//   6.  bool operator!= (const T&);
//   7.  T& operator= (const T&);
//
// For true objects, the NiTObjectList may be a better choice
//
// Example of iteration from head to tail:
//
//   NiTList<T> kList;
//   NiTListIterator pos = kList.GetHeadPos();
//   while (pos)
//   {
//       T element = kList.GetNext(pos);
//       <process element here>;
//   }

#include <NiTPointerListBase.h>
#include <NiTDefaultAllocator.h>

template <class T> class NiTList : public 
    NiTPointerListBase<NiTDefaultAllocator<T>, T>{};

#endif // NITLIST_H
