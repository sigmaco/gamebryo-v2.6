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

#ifndef NIPHYSXSHAREDDATALIBTYPE_H
#define NIPHYSXSHAREDDATALIBTYPE_H

#ifdef NIPHYSXSHAREDDATA_EXPORT
    // DLL library project uses this
    #define NIPHYSXSHAREDDATA_ENTRY __declspec(dllexport)
#else
    // client of DLL uses this
    #define NIPHYSXSHAREDDATA_ENTRY __declspec(dllimport)
#endif

#endif  // #ifndef NIPHYSXSHAREDDATALIBTYPE_H
