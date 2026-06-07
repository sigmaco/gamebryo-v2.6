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

#ifndef NIPHYSXTOOLSLIBTYPE_H
#define NIPHYSXTOOLSLIBTYPE_H

#ifdef NIPHYSXTOOLS_EXPORT
    // DLL library project uses this
    #define NIPHYSXTOOLS_ENTRY __declspec(dllexport)
#else
#ifdef NIPHYSXTOOLS_IMPORT
    // client of DLL uses this
    #define NIPHYSXTOOLS_ENTRY __declspec(dllimport)
#else
    // static library project uses this
    #define NIPHYSXTOOLS_ENTRY
#endif
#endif

#endif  // #ifndef NIPHYSXTOOLSLIBTYPE_H
