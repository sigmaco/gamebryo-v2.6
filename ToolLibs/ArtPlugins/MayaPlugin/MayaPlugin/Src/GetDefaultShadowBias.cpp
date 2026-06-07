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
#include "GetDefaultShadowBias.h"
#include "MyiInitPlugin.h"
#include "maya/MArgList.h"
#include "NiShadowManager.h"
#include <NiSystem.h>

MStatus GetDefaultShadowBias::doIt( const MArgList& args )
{
    MStatus kStat = MStatus::kSuccess;

    int iTechniqueIndex = args.asInt(0, &kStat);
    
    if(kStat != MStatus::kSuccess)
    {
        NIASSERT(0);

        displayError("Could not parse technique index argument");

        return MStatus::kFailure;
    }

    if(iTechniqueIndex >= NiShadowManager::MAX_KNOWN_SHADOWTECHNIQUES)
    {
        NIASSERT(0);

        displayError("Shadow Technique Index Out of Range!");

        return MStatus::kFailure;
    }

    int iLightType = args.asInt(1, &kStat);

    if(kStat != MStatus::kSuccess)
    {
        NIASSERT(0);

        displayError("Could not parse light type argument");

        return MStatus::kFailure;
    }

    if(iLightType >= (int)NiStandardMaterial::LIGHT_MAX)
    {
        NIASSERT(0);

        displayError("Invalid Light Type");

        return MStatus::kFailure;
    }

    clearResult(); 
    setResult(
        MyiInitPlugin::GetShadowTechniqueBias(iTechniqueIndex, iLightType));

    return MStatus::kSuccess;
}

void* GetDefaultShadowBias::creator()
{
    return new GetDefaultShadowBias();
}