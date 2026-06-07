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
#include "GetProfileDescription.h"
#include "GetPackingProfiles.h"
#include "maya/MArgList.h"
#include <NiMeshProfileProcessor.h>
#include <NiMeshProfileXMLParser.h>

MStatus GetProfileDescription::doIt( const MArgList& args )
{
    MStatus kStat = MStatus::kSuccess;

    MString kProfileName;
    kStat = args.get(0, kProfileName);
    CHECK_MSTATUS(kStat);

    // Convert to the correct name
    kProfileName = GetPackingProfiles::GetProfileName(kProfileName);

    const char* pcProfileName = kProfileName.asChar();

    const char* pcDescription = NULL;
    const NiOpProfile* kNiOpProfile = NULL;
    for(NiUInt32 uiPlatformIndex = 0; 
        uiPlatformIndex < NiSystemDesc::RENDERER_NUM; uiPlatformIndex++)
    {
        kNiOpProfile = NiMeshProfileProcessor::GetProfile(
            pcProfileName, (NiSystemDesc::RendererID)uiPlatformIndex);

        if(kNiOpProfile != NULL)
        {
            pcDescription = kNiOpProfile->GetDescription();
        }
    }
    
    if(pcDescription != NULL)
    {
        MString kDescription(pcDescription);

        clearResult(); 
        setResult(kDescription); 
        kStat = MStatus::kSuccess;
    }
    else
    {
        clearResult(); 
        MString kErrorString("Could not find profile named ");
        kErrorString += kProfileName;
        displayError(kErrorString); 
        kStat = MStatus::kFailure;
    }
 
    return kStat;
}

void* GetProfileDescription::creator()
{
    return new GetProfileDescription();
}