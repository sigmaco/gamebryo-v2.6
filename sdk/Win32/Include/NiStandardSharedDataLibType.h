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

#ifndef NISTANDARDSHAREDDATALIBTYPE_H
#define NISTANDARDSHAREDDATALIBTYPE_H

#ifdef NISTANDARDSHAREDDATA_EXPORT
    // DLL library project uses this
    #define NISTANDARDSHAREDDATA_ENTRY __declspec(dllexport)
#else
    // client of DLL uses this
    #define NISTANDARDSHAREDDATA_ENTRY __declspec(dllimport)
#endif

#endif  // #ifndef NISTANDARDSHAREDDATALIBTYPE_H
