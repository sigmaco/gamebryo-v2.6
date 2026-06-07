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

#ifndef NIPLUGINDLLINTERFACE_H
#define NIPLUGINDLLINTERFACE_H

#include <NiTPtrSet.h>
#include "NiPlugin.h"
typedef NiTPtrSet<NiPluginPtr, NiTNewInterface<NiPluginPtr> > NiPluginPtrSet;

#ifdef NIPLUGINDLL_EXPORT
    // DLL library project uses this
    #define NIPLUGINDLL_ENTRY __declspec(dllexport)
#else
    // client of DLL uses this
    #define NIPLUGINDLL_ENTRY __declspec(dllimport)
#endif

// All DLLs containing NiPlugins must implement these functions.
extern "C" NIPLUGINDLL_ENTRY void GetPlugins(
    NiPluginPtrSet& kPluginArray);
extern "C" NIPLUGINDLL_ENTRY unsigned int GetCompilerVersion(void);

typedef void (*NiGetPluginsFunction)(NiPluginPtrSet&);
typedef unsigned int (*NiGetCompilerVersionFunction)(void);

#endif  // #ifndef NIPLUGINDLLINTERFACE_H
