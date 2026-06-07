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

#ifndef NIVISUALTRACKERLIBTYPE_H
#define NIVISUALTRACKERLIBTYPE_H

#ifdef NIVISUALTRACKER_EXPORT
    // DLL library project uses this
    #define NIVISUALTRACKER_ENTRY __declspec(dllexport)
#else
#ifdef NIVISUALTRACKER_IMPORT
    // client of DLL uses this
    #define NIVISUALTRACKER_ENTRY __declspec(dllimport)
#else
    // static library project uses this
    #define NIVISUALTRACKER_ENTRY
#endif
#endif

// Disable warning C4251.  Template classes cannot be exported for the obvious
// reason that the code is not generated until an instance of the class is
// declared.  With this warning enabled, you get thousands of complaints about
// class data members that are of template type.  For example, a member such
// as 'NiTArray<NiAVObject*> m_array' generates the warning.
//
// When maintaining the DLL code itself, you might want to enable the warning
// to check for cases where you might have failed to put NIVISUALTRACKER_ENTRY
// after the class keyword.  In particular, nested classes must have
// NIVISUALTRACKER_ENTRY.  Also, friend functions must be tagged with
// NIVISUALTRACKER_ENTRY.

#ifdef WIN32
    #pragma warning( disable : 4251 )
#endif
                                      
#endif
