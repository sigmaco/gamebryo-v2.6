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

#include "stdafx.h"
#include "DeprecatedImportPlugin.h"
#include "DeprecatedProcessPlugin.h"
#include "DeprecatedExportPlugin.h"
#include "DeprecatedViewerPlugin.h"

//---------------------------------------------------------------------------
NIPLUGINDLL_ENTRY void GetPlugins(NiPluginPtrSet& kPluginArray)
{
    kPluginArray.Add(NiNew DeprecatedProcessPlugin("NBT Generator Plug-in"));
    kPluginArray.Add(NiNew DeprecatedProcessPlugin("Texture Swizzle"));
    kPluginArray.Add(NiNew DeprecatedProcessPlugin("Mesh Prepare"));
    kPluginArray.Add(NiNew DeprecatedViewerPlugin("PS2 Viewer"));
    kPluginArray.Add(NiNew DeprecatedViewerPlugin("Xbox Viewer"));
}
//---------------------------------------------------------------------------
