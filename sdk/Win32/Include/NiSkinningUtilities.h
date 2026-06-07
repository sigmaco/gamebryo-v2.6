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

#ifndef NISKINNINGUTILITIES_H
#define NISKINNINGUTILITIES_H

#include <NiMemObject.h>
#include <NiMeshToolsLibType.h>

#include "NiToolPipelineCloneHelper.h"

class NiMesh;

/**
    Helper class for creating skinned meshes that are GPU-friendly by breaking 
    up the mesh into submeshes that use a predefined number of bones.
*/
class NIMESHTOOLS_ENTRY NiSkinningUtilities : public NiMemObject
{
public:

    // *** begin Emergent internal use only ***
    // See NiMeshTools wrapper method for documentation.
    static bool MakeSubmeshesForGPUSkinning(NiToolPipelineCloneHelper&
        kCloneHelper, NiToolPipelineCloneHelper::CloneSetPtr& spCloneSet, 
        unsigned char ucBonesPerSubmesh);

    // A method for placing bones before the skinned meshes in the scene
    // hierarchy.
    static bool ReorderBonesForSkinnedMeshes(NiNode* pkRootNode);
    // *** end Emergent internal use only ***
};


#endif // NISKINNINGUTILITIES_H
