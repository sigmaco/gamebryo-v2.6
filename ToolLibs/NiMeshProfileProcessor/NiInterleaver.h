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

#ifndef NIINTERLEAVER_H
#define NIINTERLEAVER_H

#include <NiTArray.h>
#include "NiInterleaveProfile.h"
#include "NiMeshErrorInterface.h"
#include "NiMesh.h"
#include "NiToolPipelineCloneHelper.h"
#include "NiMeshProfileProcessorLibType.h"

class NiDataStream;

/// @cond EMERGENT_INTERNAL
/**
    Interleaves a set of streams as specified by an NiInterleaveProfile.
*/
/// @endcond
class NIMESHPROFILEPROCESSOR_ENTRY NiInterleaver : public NiMemObject
{
public:
    // Constructor / Destructor
    NiInterleaver();
    ~NiInterleaver();

    // Entry point for cloneset conversion
    bool Interleave(NiToolPipelineCloneHelper::CloneSetPtr& spCloneSet,
        NiToolPipelineCloneHelper& kCloneHelper,
        NiInterleaveProfile* pkProfile,
        NiMeshErrorInterface* pkLog = NULL);

    static bool DeInterleave(NiToolPipelineCloneHelper::CloneSetPtr&
        spCloneSet, NiToolPipelineCloneHelper& kCloneHelper,
        NiMeshErrorInterface* pkLog = NULL);

    // Clears processed clone sets, etc.
    void Reset();

private:
    bool Initialize(); // Initializes the packer
    bool Finalize(NiMeshErrorInterface* pkLog); // Finalizes the streams
    bool Interleave(NiMeshErrorInterface* pkLog); // interleaves the streams
    bool InterleaveStreams(); // Interleaves

    // Initializes and creates adjusted layout
    bool CreateAdjustedLayout(NiMeshErrorInterface* pkLog);

    NiTObjectSet<NiDataStreamRef> m_kStreamRefs;
    NiTObjectSet<NiDataStreamRef> m_kFinishedStreamRefs;

    NiMesh* m_pkMesh;
    NiInterleaveProfile* m_pkProfile;
    NiInterleaveProfile::StreamLayoutSet m_kAdjustedLayout;
    NiToolPipelineCloneHelper::CloneSet* m_pkCloneSet;
    NiToolPipelineCloneHelper* m_pkCloneHelper;
};

#endif
