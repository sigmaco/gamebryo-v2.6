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

#include <NiSystem.h>

//---------------------------------------------------------------------------
// The following copyright notice may not be removed.
static char EmergentCopyright[] NI_UNUSED =
    "Copyright (c) 1996-2008 Emergent Game Technologies.";
//---------------------------------------------------------------------------

#include <NiVersion.h>
static char acGamebryoVersion[] NI_UNUSED =
    GAMEBRYO_MODULE_VERSION_STRING(NiOptimization);

#include "NiOptimize.h"
const float NiOptimize::ms_fEpsilon = 0.0001f;

