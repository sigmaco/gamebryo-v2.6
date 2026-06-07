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

#ifndef NITERRAINPHYSXLIBTYPE_H
#define NITERRAINPHYSXLIBTYPE_H

#ifndef __SPU__

    #if defined(NITERRAINPHYSX_EXPORT)
        // building DLL library uses this
        #define NITERRAINPHYSX_ENTRY __declspec(dllexport)
    #elif defined(NITERRAINPHYSX_IMPORT)
        // something importing a DLL uses this
        #define NITERRAINPHYSX_ENTRY __declspec(dllimport)
    #else
        // static library project uses this
        #define NITERRAINPHYSX_ENTRY
    #endif
#else
    #define NITERRAINPHYSX_ENTRY 
#endif // __SPU__

#endif // NITERRAINPHYSXLIBTYPE_H