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

#include "MayaPluginPCH.h"
#include "GetShadowTechniquesCommand.h"
#include "MyiInitPlugin.h"
#include "NiShadowManager.h"

MStatus GetShadowTechniques::doIt( const MArgList& args )
{
    NI_UNUSED_ARG(&args);
    MStringArray kTechniqueNames;

    NiShadowManager::Initialize();

    for (int iTechniqueIndex = 0;
         iTechniqueIndex < NiShadowManager::MAX_KNOWN_SHADOWTECHNIQUES; 
         iTechniqueIndex++)
    {
         if (MyiInitPlugin::GetShadowTechniqueName(iTechniqueIndex))
         {
            MString kName(
                MyiInitPlugin::GetShadowTechniqueName(iTechniqueIndex));
            kTechniqueNames.append(kName);
         }
    }

    clearResult(); 
    setResult(kTechniqueNames);  

    return MStatus::kSuccess;
}

void* GetShadowTechniques::creator()
{
    return new GetShadowTechniques();
}