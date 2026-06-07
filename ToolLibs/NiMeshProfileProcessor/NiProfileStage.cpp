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

#include "NiProfileStage.h"
#include "NiOpProfile.h"
#include "NiMPPMessages.h"
#include "NiInterleaver.h"

typedef NiDataStreamElement NIDS;
typedef NiToolPipelineCloneHelper::CloneSet CloneSet;
typedef NiToolPipelineCloneHelper::CloneSetPtr CloneSetPtr;

//---------------------------------------------------------------------------
NiProfileStage::NiProfileStage(NiToolPipelineCloneHelper& kCloneHelper,
    NiTPointerMap<NiMesh*, NiOpProfile*>& kMeshToProfile,
    NiSystemDesc::RendererID eRenderer, NiMeshProfileErrorHandler& kLog,
    ProgressInitCallback pfnProgressInitCallback,
    ProgressCallback pfnProgressCallback)
{
    m_pkCloneHelper = &kCloneHelper;
    m_pkMeshToProfile = &kMeshToProfile;
    m_eRenderer = eRenderer;
    m_pfnProgressInitCallback = pfnProgressInitCallback;
    m_pfnProgressCallback = pfnProgressCallback;
    m_pkLog = &kLog;
    NiOpProfileFragment::ms_pkCloneHelper = m_pkCloneHelper;
}
//---------------------------------------------------------------------------
NiProfileStage::~NiProfileStage()
{
}
//---------------------------------------------------------------------------
void NiProfileStage::DeInterleave(
    NiTPrimitiveSet<NiToolPipelineCloneHelper::CloneSet*>& kAllCloneSets)
{
    NiUInt32 uiNumCloneSets = kAllCloneSets.GetSize();

    // Deinterleave all Cloneset fragments
    for (NiUInt32 ui=0; ui<uiNumCloneSets; ui++)
    {
        CloneSetPtr spCloneSet = kAllCloneSets.GetAt(ui);
        NiInterleaver::DeInterleave(spCloneSet, *m_pkCloneHelper, m_pkLog);

        if (m_pfnProgressCallback)
            m_pfnProgressCallback(*m_pkLog);
    }
}
//---------------------------------------------------------------------------
void NiProfileStage::CreateFragments(NiMesh* pkMesh,
    NiOpProfile*,
    NiTObjectArray <NiOpStartPtr>& akStartOps,
    NiTObjectArray <NiOpTerminatorPtr>& akTerminatorOps,
    bool bKeepUnspecifiedStreams)
{
    // Retrieve all the clone-sets that make up this Mesh. In the general
    // case, there will only be one, however some cloned mesh may have a
    // mix-sharing of data-streams. In this case, there will be 2 or more
    // clone-sets.
    NiTObjectSet<CloneSetPtr> kCloneSets;
    m_pkCloneHelper->GetCloneSetsThatIncludeMesh(kCloneSets, pkMesh);

    NiUInt32 uiCloneSetCnt = kCloneSets.GetSize();

    // Keep a hash table to the cloned terminators
    NiTMap<NiOpTerminator*, bool> kClonedTermMap;

    for(NiUInt32 uiCloneSet=0; uiCloneSet<uiCloneSetCnt; uiCloneSet++)
    {
        CloneSetPtr spCloneSet = kCloneSets.GetAt(uiCloneSet);

        NiOpProfileFragmentPtr spTmpFragment = NULL;
        if (m_kCloneSetToProfileFragment.GetAt(spCloneSet, spTmpFragment))
        {
            // This cloneset has already been processed while iterating
            // over another mesh.
            continue;
        }

        // Retrieve NiDataStreamRefs that are associated with this
        // cloneset. To do this, we find every NiDataStreamRef on this mesh
        // that has a data-stream which maps to the cloneset that we are
        // interested in.
        NiTPrimitiveSet<NiDataStreamRef*> kMeshStreamRefs;
        NiUInt32 uiMeshSRCnt = pkMesh->GetStreamRefCount();
        for(NiUInt32 uiMeshSR=0; uiMeshSR<uiMeshSRCnt; uiMeshSR++)
        {
            NiDataStreamRef* pkSR = pkMesh->GetStreamRefAt(uiMeshSR);
            if (!pkSR)
                continue;

            NiDataStream* pkDS = pkSR->GetDataStream();
            NIASSERT(pkDS);

            CloneSetPtr spRetrievedCloneSet = NULL;
            if (!m_pkCloneHelper->GetCloneSet(pkDS, spRetrievedCloneSet))
                continue;

            if (spRetrievedCloneSet == spCloneSet)
            {
                // Add this SR to the list of ones we need to match up
                // with startOps.
                kMeshStreamRefs.Add(pkSR);
            }
        }

        // Create a new profile fragment for this cloneset
        NiOpProfileFragment* pkProfileFragment = NiNew NiOpProfileFragment();
        m_kCloneSetToProfileFragment.SetAt(spCloneSet, pkProfileFragment);

        NiUInt32 uiSRCnt = kMeshStreamRefs.GetSize();
        NIASSERT(uiSRCnt > 0);

        // Any non-matching streams that are marked as keep will be placed
        // here.
        NiTPrimitiveSet<NiDataStreamRef*> kNonMatchingStreamRefToKeep;

        // Iterate over each of the stream ref candidates and determine
        // if it is required as an input to one or more startOp(s).
        for (NiUInt32 uiSR=0; uiSR<uiSRCnt; uiSR++)
        {
            NiDataStreamRef* pkSR = kMeshStreamRefs.GetAt(uiSR);
            NIASSERT(pkSR->GetElementDescCount() == 1);

            bool bFoundMatch = false;

            NiUInt32 uiStartSize = akStartOps.GetSize();
            for (NiUInt32 uiStart=0; uiStart<uiStartSize; uiStart++)
            {
                NiOpStart* pkStart = akStartOps.GetAt(uiStart);

                // Skip already assigned nodes
                if (pkStart->GetOutput().IsValid())
                    continue;

                if (pkStart->GetSemantic() == pkSR->GetSemanticNameAt(0) &&
                    pkStart->GetSemanticIndex() == pkSR->GetSemanticIndexAt(0))
                {
                    bFoundMatch = true;
                    pkStart->SetOutput(*pkSR);
                }
            }

            if (bFoundMatch == false && bKeepUnspecifiedStreams)
                kNonMatchingStreamRefToKeep.Add(pkSR);
        }

        // Clone terminators that can be resolved
        NiUInt32 uiHighestStreamID = 0; // Used if non-matching to keep
        NiUInt32 uiTermSize = akTerminatorOps.GetSize();
        for (NiUInt32 uiTerm=0; uiTerm<uiTermSize; uiTerm++)
        {
            NiOpTerminator* pkTerm = akTerminatorOps.GetAt(uiTerm);
            if (pkTerm->CanResolve())
            {
                NiOpTerminator* pkTermClone = (NiOpTerminator*)pkTerm->Clone();
                pkProfileFragment->m_kTerminatorNodes.Add(pkTermClone);

                if (pkTerm->GetStreamID() > uiHighestStreamID)
                    uiHighestStreamID = pkTerm->GetStreamID();

                // Record that this stream was handled.
                kClonedTermMap.SetAt(pkTerm, true);
            }
        }

        // The following code deals with streams that have been marked as to
        // keep even though no profile information was given on the stream.
        // In order for the stream to be kept, it can not have been used as
        // an input in the construction of a stream AND it cannot have the
        // same semantic and index as a stream that is already being produced.
        NiUInt32 uiNonMatchCnt = kNonMatchingStreamRefToKeep.GetSize();
        for(NiUInt32 uiNonMatch=0; uiNonMatch<uiNonMatchCnt; uiNonMatch++)
        {
            // No matches were found, but the profile says to keep it.
            // In order to give Finalize stage an opportunity to have
            // mesh modifiers merge with these streams, we create a
            // dummy operation here.

            NiDataStreamRef* pkSR = kNonMatchingStreamRefToKeep.GetAt(
                uiNonMatch);

            bool bOutputAlreadyProduced = false;
            NiUInt32 uiFragTermCnt = 
                pkProfileFragment->m_kTerminatorNodes.GetSize();
            for (NiUInt32 uiFragTerm=0; uiFragTerm<uiFragTermCnt; uiFragTerm++)
            {
                NiOpTerminator* pkTerm = 
                    pkProfileFragment->m_kTerminatorNodes.GetAt(uiFragTerm);

                NiFixedString kTermSemantic;
                NiUInt8 uiTermIndex = 0;
                NIVERIFY(pkTerm->GetSemantic(kTermSemantic, uiTermIndex));

                if (kTermSemantic == pkSR->GetSemanticNameAt(0) &&
                    uiTermIndex == pkSR->GetSemanticIndexAt(0))
                {
                    // Can not add because output with same semantic and index
                    // is already being produced.
                    bOutputAlreadyProduced = true;
                    break;
                }
            }

            if (bOutputAlreadyProduced == false)
            {
                uiHighestStreamID++;
                NiOpStart* pkStart = NiNew NiOpStart();
                pkStart->SetOutput(*pkSR);
                pkStart->SetParameters(pkSR->GetSemanticNameAt(0),
                    (NiUInt8)pkSR->GetSemanticIndexAt(0));
                NiOpTerminator* pkTerm = NiNew NiOpTerminator();
                pkTerm->SetParameters(uiHighestStreamID, 0, 
                    NiDataStream::USAGE_MAX_TYPES, 0, true);
                pkTerm->SetInput(pkStart);
                pkProfileFragment->m_kTerminatorNodes.Add(pkTerm);
            }
        }

        // reset all start-ops so that the next iteration won't have residue
        // NiDataStreamRef's.
        NiUInt32 uiStartSize = akStartOps.GetSize();
        for (NiUInt32 uiStart=0; uiStart<uiStartSize; uiStart++)
        {
            NiOpStart* pkStart = akStartOps.GetAt(uiStart);
            NiDataStreamRef kDataStreamRef;
            pkStart->SetOutput(kDataStreamRef);
        }
    }

    // Look for any "required" operations that were not resolved by the 
    // union of all the clone-sets. We marked each resolved terminator in
    // the clone map, so if not there, we have an issue.
    NiUInt32 uiTermSize = akTerminatorOps.GetSize();
    for (NiUInt32 uiTerm=0; uiTerm<uiTermSize; uiTerm++)
    {
        NiOpTerminator* pkTerm = akTerminatorOps.GetAt(uiTerm);

        if (pkTerm->IsRequired() == false)
            continue;

        bool bTmp;
        if (kClonedTermMap.GetAt(pkTerm, bTmp) == false)
        {
            // ERROR: A stream definition could not find its required input
            // stream. The required input stream was %s, %d.
            NiFixedString kSemantic;
            NiUInt8 uiIndex = 0;
            if (!pkTerm->GetSemantic(kSemantic, uiIndex))
                kSemantic = "UNKNOWN";

            m_pkLog->ReportError(NiMPPMessages::
                ERROR_REQUIRED_STREAM_FAILED_TO_FIND_INPUT_2ARGS,
                kSemantic, uiIndex);
        }
    }
}
//---------------------------------------------------------------------------
void NiProfileStage::ResolveFragments(
    NiTPrimitiveSet<NiToolPipelineCloneHelper::CloneSet*>& kAllCloneSets)
{
    NiUInt32 uiNumCloneSets = kAllCloneSets.GetSize();

    // Resolve all Cloneset fragments
    for (NiUInt32 ui=0; ui<uiNumCloneSets; ui++)
    {
        CloneSetPtr spCloneSet = kAllCloneSets.GetAt(ui);
        ResolveFragment(spCloneSet);

        if (m_pfnProgressCallback)
            m_pfnProgressCallback(*m_pkLog);
    }
}
//---------------------------------------------------------------------------
bool NiProfileStage::ResolveFragment(
    NiToolPipelineCloneHelper::CloneSetPtr& spCloneSet)
{
    NiOpProfileFragmentPtr spProfileFragment = 0;
    if (m_kCloneSetToProfileFragment.GetAt(spCloneSet, spProfileFragment) ==
        false)
    {
        return false; // No profile fragment exists.
    }

    NiUInt32 uiTermCnt = spProfileFragment->m_kTerminatorNodes.GetSize();
    NiTObjectSet<NiDataStreamRef> kStreamRefsToAdd;
    for(NiUInt32 uiTerm=0; uiTerm<uiTermCnt; uiTerm++)
    {
        NiOpTerminator* pkTerm =
            spProfileFragment->m_kTerminatorNodes.GetAt(uiTerm);

        if (pkTerm->Resolve(m_pkLog) == false)
        {
            NiFixedString kSemantic;
            NiUInt8 uiIndex = 0;
            if (!pkTerm->GetSemantic(kSemantic, uiIndex))
                kSemantic = "UNKNOWN";

            if (pkTerm->IsRequired())
            {
                m_pkLog->ReportError(NiMPPMessages::
                    ERROR_EXPECTED_REQUIRED_STREAM_FAILED_RESOLVE_2ARGS,
                    kSemantic, uiIndex);
            }
            else
            {
                // Fragments are created with the expectation that they will
                // resolve. This datastream wasn't required, but something
                // unexpected happen such that the final stream could not be
                // produced when we expected that it could be. This could 
                // happen if an operation failed. Examples: merge and swizzle
                // didn't have all its inputs, or couldn't perform the merge,
                // etc.
                m_pkLog->ReportError(NiMPPMessages::
                    ERROR_EXPECTED_STREAM_FAILED_RESOLVE_2ARGS,
                    kSemantic, uiIndex);
            }
            spProfileFragment->SetHadResolveErrors(true);
            return false;
        }
        else
        {
            NiDataStreamRef kStreamRef = pkTerm->RemoveOutput();
            NIASSERT(kStreamRef.IsValid());
            kStreamRefsToAdd.Add(kStreamRef);
        }
    }

    if (kStreamRefsToAdd.GetSize() == 0)
        return false;

    // Remove all data streams from this cloneset
    m_pkCloneHelper->ReleaseAllStreamRefs(spCloneSet);

    // Add back all the outputs of the terminators
    for (NiUInt32 uiToAdd=0; uiToAdd < kStreamRefsToAdd.GetSize(); uiToAdd++)
    {
        NiDataStreamRef kStreamRef = kStreamRefsToAdd.GetAt(uiToAdd);
        m_pkCloneHelper->AddStreamRef(spCloneSet, &kStreamRef);
    }

    return true;
}
//---------------------------------------------------------------------------

