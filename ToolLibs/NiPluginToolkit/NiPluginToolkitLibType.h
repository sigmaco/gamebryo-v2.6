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

#ifndef NIPLUGINTOOLKITLIBTYPE_H
#define NIPLUGINTOOLKITLIBTYPE_H

#ifdef NIPLUGINTOOLKIT_EXPORT
    // DLL library project uses this
    #define NIPLUGINTOOLKIT_ENTRY __declspec(dllexport)
#else
    // client of DLL uses this
    #define NIPLUGINTOOLKIT_ENTRY __declspec(dllimport)
#endif

#ifdef WIN32
    #pragma warning( disable : 4786 )
#endif            

#endif  // #ifndef NIPLUGINTOOLKITLIBTYPE_H
