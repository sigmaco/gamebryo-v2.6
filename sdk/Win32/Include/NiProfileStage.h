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

#ifndef NIPROFILESTAGE_H
#define NIPROFILESTAGE_H

#include <NiToolPipelineCloneHelper.h>

#include "NiMeshProfileProcessorLibType.h"
#include "NiMeshProfileErrorHandler.h"
#include "NiOpProfileFragment.h"
#include "NiOpStart.h"
#include "NiOpTerminator.h"

class NiMesh;
class NiOpProfile;

/// @cond EMERGENT_INTERNAL
/**
    Base class for handling the Finalize stage
*/
/// @endcond
class NIMESHPROFILEPROCESSOR_ENTRY NiProfileStage : public NiMemObject
{
public:
    typedef void (*ProgressInitCallback)(unsigned int uiSize);
    typedef void (*ProgressCallback)(NiMeshProfileErrorHandler&
        kMessageHandler);

    // Constructor / Destructor
    NiProfileStage(NiToolPipelineCloneHelper& kCloneHelper,
        NiTPointerMap<NiMesh*, NiOpProfile*>& kMeshToProfile,
        NiSystemDesc::RendererID eRenderer,
        NiMeshProfileErrorHandler& kLog,
        ProgressInitCallback pfnProgressInitCallback,
        ProgressCallback pfnProgressCallback);

    virtual ~NiProfileStage();

protected:
    void DeInterleave(NiTPrimitiveSet<NiToolPipelineCloneHelper::CloneSet*>&
        kAllCloneSets);

    void CreateFragments(NiMesh* pkMesh,
        NiOpProfile* pkMasterProfile,
        NiTObjectArray <NiOpStartPtr>& akStartOps,
        NiTObjectArray <NiOpTerminatorPtr>& akTerminatorOps,
        bool bKeepUnspecifiedStreams);
    void ResolveFragments(
        NiTPrimitiveSet<NiToolPipelineCloneHelper::CloneSet*>& kAllCloneSets);
    bool ResolveFragment(NiToolPipelineCloneHelper::CloneSetPtr& spCloneSet);

    NiToolPipelineCloneHelper* m_pkCloneHelper;
    NiTPointerMap<NiMesh*, NiOpProfile*>* m_pkMeshToProfile;
    NiTMap<NiToolPipelineCloneHelper::CloneSet*, NiOpProfileFragmentPtr>
        m_kCloneSetToProfileFragment;

    NiMeshProfileErrorHandler* m_pkLog;

    ProgressInitCallback m_pfnProgressInitCallback;
    ProgressCallback m_pfnProgressCallback;

    NiSystemDesc::RendererID m_eRenderer;
};

#endif // NIPROFILESTAGE_H
