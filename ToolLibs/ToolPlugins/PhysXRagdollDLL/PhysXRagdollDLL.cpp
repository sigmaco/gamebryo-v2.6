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

#include "StdAfx.h"
#include "NiPhysXRagdollPlugin.h"


//---------------------------------------------------------------------------
NIPLUGINDLL_ENTRY void GetPlugins(NiPluginPtrSet& kPluginArray)
{
    // This method is the entry point for the plug-in DLL system. We add the
    // plug-ins that this DLL supports to the global list of plug-ins through
    // the NiPtrSet. If this method is not implemented, the plug-in DLL will
    // fail to load.

    kPluginArray.Add(NiNew NiPhysXRagdollPlugin);
}
//---------------------------------------------------------------------------
NIPLUGINDLL_ENTRY unsigned int GetCompilerVersion(void)
{
     return (_MSC_VER);
}
//--------------------------------------------------------------------------- 