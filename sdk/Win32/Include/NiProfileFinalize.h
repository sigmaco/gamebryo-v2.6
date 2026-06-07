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
//---------------------------------------------------------------------------

#ifndef NIPROFILEFINALIZE_H
#define NIPROFILEFINALIZE_H

#include <NiToolPipelineCloneHelper.h>

#include "NiMeshProfileProcessorLibType.h"
#include "NiProfileStage.h"

/// @cond EMERGENT_INTERNAL
/**
    Performs the Finalize stage.
*/
/// @endcond
class NIMESHPROFILEPROCESSOR_ENTRY NiProfileFinalize : public NiProfileStage
{
public:
    // Constructor / Destructor
    NiProfileFinalize(NiToolPipelineCloneHelper& kCloneHelper,
        NiTPointerMap<NiMesh*, NiOpProfile*>& kMeshToProfile,
        NiSystemDesc::RendererID eRenderer,
        NiMeshProfileErrorHandler& kLog,
        ProgressInitCallback pfnProgressInitCallback,
        ProgressCallback pfnProgressCallback);

    ~NiProfileFinalize(){};

    void PerformStage();

protected:
    void CreateFinalizeFragments(NiTPrimitiveSet<NiMesh*>& kAllMesh);
    void AdjustFinalizeFragments(NiTPrimitiveSet<NiMesh*>& kAllMesh);
    bool MergeModifierRequirementsIntoProfileFragment(NiMesh* pkMesh);
    void InterleaveFinalizeFragment(
        NiTPrimitiveSet<NiToolPipelineCloneHelper::CloneSet*>& kAllCloneSets);
    void PostFinalizeStep(NiTPrimitiveSet<NiMesh*>& kAllMesh);
    void ValidateMeshModifiers(NiMesh* pkMesh);
};

#endif // NIPROFILEFINALIZE_H
