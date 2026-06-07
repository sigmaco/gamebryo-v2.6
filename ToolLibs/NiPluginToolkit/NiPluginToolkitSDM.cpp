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

#include "NiPluginToolkitSDM.h"
#include <NiSDMMacros.h>
#include "NiPluginHelpers.h"

NiImplementSDMConstructor(NiPluginToolkit,
    "NiMesh NiFloodgate NiMain");
NiImplementDllMain(NiPluginToolkit);

//---------------------------------------------------------------------------
void NiPluginToolkitSDM::Init()
{
    NiImplementSDMInitCheck();
}
//---------------------------------------------------------------------------
void NiPluginToolkitSDM::Shutdown()
{
    NiImplementSDMShutdownCheck();

    NiPluginHelpers::_SDMShutdown();
}
//---------------------------------------------------------------------------
