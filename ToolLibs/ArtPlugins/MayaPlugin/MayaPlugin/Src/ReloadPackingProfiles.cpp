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
#include "ReloadPackingProfiles.h"
#include "maya/MArgList.h"
#include <NiMeshProfileProcessor.h>
#include <NiMeshProfileXMLParser.h>

MStatus ReloadPackingProfiles::doIt( const MArgList& args )
{
    NI_UNUSED_ARG(args);

    MStatus kStat = MStatus::kSuccess;

    NiMeshProfileProcessor::DestroyMeshProfileProcessor();

    NiMeshProfileProcessor::CreateMeshProfileProcessor();
    NiMeshProfileXMLParser::RegisterAllProfilesWithProcessor();

    return kStat;

}

void* ReloadPackingProfiles::creator()
{
    return new ReloadPackingProfiles();
}