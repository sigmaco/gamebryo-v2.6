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

#ifndef NIAUDIOLIBTYPE_H
#define NIAUDIOLIBTYPE_H

#include <NiOS.h>

#ifdef NIAUDIO_EXPORT
    // DLL library project uses this
    #define NIAUDIO_ENTRY __declspec(dllexport)
#else
#ifdef NIAUDIO_IMPORT
    // client of DLL uses this
    #define NIAUDIO_ENTRY __declspec(dllimport)
#else
    // static library project uses this
    #define NIAUDIO_ENTRY
#endif
#endif

// Disable warning C4251.  Template classes cannot be exported for the obvious
// reason that the code is not generated until an instance of the class is
// declared.  With this warning enabled, you get thousands of complaints about
// class data members that are of template type.  For example, a member such
// as 'NiTPrimitiveArray<NiAVObject*> m_array' generates the warning.
#ifndef _PS3
#pragma warning( disable : 4251 )
#endif
                                      
#endif
