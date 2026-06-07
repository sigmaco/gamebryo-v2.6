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

#include <NiMesh.h>
#include <NiMeshUtilities.h>
#include <NiMeshRequirements.h>
#include <NiSkinningMeshModifier.h>

#include "NiMeshProfileProcessor.h"
#include "NiMeshErrorInterface.h"
#include "NiComponentInput.h"
#include "NiOpProfile.h"
#include "NiOpStart.h"
#include "NiOpTerminator.h"
#include "NiOpCastConvert.h"
#include "NiOpMergeAndSwizzle.h"
#include "NiMPPMessages.h"

NiImplementCreateClone(NiOpProfile);

typedef NiInterleaveProfile::StreamLayout StreamLayout;
typedef NiInterleaveProfile::StreamLayoutSet StreamLayoutSet;
typedef NiInterleaveProfile::Element Element;
typedef NiTPointerMap<NiUInt32, StreamLayout*> StreamGroupStreamLayoutMap;

//---------------------------------------------------------------------------
void AddToStreamGroup(StreamGroupStreamLayoutMap& kStreamGroups,
    NiUInt32 uiGrp, NiFixedString& kSemantic, NiUInt8 uiSemanticIndex,
    bool bStrictInterleave, bool bCoalesceOnSplit)
{
    StreamLayout* pkStreamLayout;
    if (kStreamGroups.GetAt(uiGrp, pkStreamLayout))
    {
        // share group already exist
        pkStreamLayout->Add(Element(kSemantic, uiSemanticIndex,
            bStrictInterleave, bCoalesceOnSplit));
    }
    else
    {
        pkStreamLayout = NiNew StreamLayout();
        pkStreamLayout->Add(Element(kSemantic, uiSemanticIndex,
            bStrictInterleave, bCoalesceOnSplit));
        kStreamGroups.SetAt(uiGrp, pkStreamLayout);
    }
}
//---------------------------------------------------------------------------
void RetrieveAvailableStreams(NiMesh* pkMesh, 
    NiTPrimitiveArray<NiDataStreamRef*>& kAvailableStreams)
{
    NiUInt32 uiStreamRefCnt = pkMesh->GetStreamRefCount();
    for(NiUInt32 ui=0; ui<uiStreamRefCnt; ui++)
    {
        NiDataStreamRef* pkStreamRef = pkMesh->GetStreamRefAt(ui);
        if (!pkStreamRef)
            continue;
        kAvailableStreams.Add(pkStreamRef);
    }
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
NiOpProfile::NiOpProfile()
{
    m_strName = "Default";
    m_strDescription = "None";
    m_eRenderer = NiSystemDesc::RENDERER_GENERIC;
    m_bKeepUnspecifiedFinalizeStreams = false;
    memset(m_abPostResolveOps, 0, sizeof(m_abPostResolveOps));
}
//---------------------------------------------------------------------------
NiOpProfile::~NiOpProfile()
{
    m_kStartFinalizeNodes.RemoveAll();
    m_kTerminatorFinalizeNodes.RemoveAll();
}
//---------------------------------------------------------------------------
void NiOpProfile::PostResolveFinalizeOps(NiMesh* pkMesh) const
{
    for (NiUInt32 ui = 0; ui < PRO_COUNT; ui++)
    {
        if (m_abPostResolveOps[ui])
        {
            switch (ui)
            {
                case PRO_COMPUTE_BOUNDS:
                    pkMesh->RecomputeBounds();
                    break;
                case PRO_RECOMPUTE_NORMALS:
                    if (NiGetModifier(NiSkinningMeshModifier, pkMesh) != 0)
                    {
                        for (NiUInt32 ui = 0; ui < pkMesh->GetSubmeshCount();
                            ui++)
                        {
                            NiMeshUtilities::RecalculateNormals(pkMesh,
                                NiCommonSemantics::POSITION_BP(), 0,
                                NiCommonSemantics::NORMAL_BP(), 0,
                                (NiUInt16)(ui), true);
                        }
                    }
                    else
                    {
                        for (NiUInt32 ui = 0; ui < pkMesh->GetSubmeshCount();
                            ui++)
                        {
                            NiMeshUtilities::RecalculateNormals(pkMesh,
                                NiCommonSemantics::POSITION(), 0,
                                NiCommonSemantics::NORMAL(), 0,
                                (NiUInt16)(ui), true);
                        }
                    }
                    break;
                default:
                    NIASSERT(0 && "Invalid post resolve operation.");
            }
        }
    }
}
//---------------------------------------------------------------------------
bool NiOpProfile::Validate(NiTObjectArray <NiOpTerminatorPtr>
    &kTermNodes)
{
    // Check Flag Validity 
    for(NiUInt32 ui=0; ui< kTermNodes.GetSize(); ui++)
    {
        NiOpTerminator* pkTerm = kTermNodes.GetAt(ui);

        if (pkTerm->GetUsage() == NiDataStream::USAGE_MAX_TYPES ||
            pkTerm->GetAccessFlags() == 0)
        {
            // These represent don't care's, so can't validate.
            continue;
        }

        if (NiDataStream::IsAccessRequestValid(pkTerm->GetAccessFlags(),
            pkTerm->GetUsage()) == false)
        {
            return false;
        }
    }

    // Check Duplicate SemanticName, Index pairs
    bool bFoundDuplicates = false;
    NiTStringPointerMap<NiTPrimitiveSet<NiUInt32>* > kSemanticMap;
    for(NiUInt32 ui=0; ui< kTermNodes.GetSize(); ui++)
    {
        NiOpTerminator* pkTerm = kTermNodes.GetAt(ui);
        NiFixedString kSemantic;
        NiUInt8 uiIndex;
        if (pkTerm->GetSemantic(kSemantic, uiIndex) == false)
            continue;

        NiTPrimitiveSet<NiUInt32>* pkIndexSet = NULL;
        if (!kSemanticMap.GetAt(kSemantic, pkIndexSet))
        {
            // First time encounterd this semantic. Add the indexset
            // with uiIndex as the first entry.
            pkIndexSet = NiNew NiTPrimitiveSet<NiUInt32>();
            pkIndexSet->Add(uiIndex);
            kSemanticMap.SetAt(kSemantic, pkIndexSet);
            continue;
        }

        // if uiIndex is already in the index-set, there are 
        // duplicate semantic-index pairs.
        NiUInt32 uiLoc = pkIndexSet->Find(uiIndex);

        if (uiLoc != -1)
        {
            bFoundDuplicates = true;
            break;
        }

        pkIndexSet->Add(uiIndex);
    }

    // free up memory.
    NiTMapIterator kIter = kSemanticMap.GetFirstPos();
    while(kIter)
    {
        const char* pkString = NULL;
        NiTPrimitiveSet<NiUInt32>* pkIndexSet = NULL;
        kSemanticMap.GetNext(kIter, pkString, pkIndexSet);
        pkIndexSet->RemoveAll();
        NiDelete pkIndexSet;
    }
    kSemanticMap.RemoveAll();

    if (bFoundDuplicates)
        return false;

    // Check for internal inconsistencies
    // These would be streams with same StreamID and having StrictInterleave,
    // but having different values for access flags, usage, strictAccess,
    // etc.

    // This first loop is only to organize the StreamGroups
    NiTMap<NiUInt32, NiTPrimitiveSet<NiOpTerminator*>* > kStreamIDToTermsMap;
    for(NiUInt32 ui=0; ui< kTermNodes.GetSize(); ui++)
    {
        NiOpTerminator* pkTerm = kTermNodes.GetAt(ui);

        // Filter out only the ones marked for StrictInterleave
        if (!pkTerm->IsStrictInterleave())
                continue;

        NiUInt32 uiStreamID = pkTerm->GetStreamID();

        NiTPrimitiveSet<NiOpTerminator*>* pkOpTermSet = NULL;
        if (kStreamIDToTermsMap.GetAt(uiStreamID, pkOpTermSet))
        {
            pkOpTermSet->Add(pkTerm);
            continue;
        }
        else
        {
            pkOpTermSet = NiNew NiTPrimitiveSet<NiOpTerminator*>();
            pkOpTermSet->Add(pkTerm);
            kStreamIDToTermsMap.SetAt(uiStreamID, pkOpTermSet);
        }
    }

    // Now that the StreamGroups are organized, test
    kIter = kStreamIDToTermsMap.GetFirstPos();
    bool bInconsistentAccess = false;
    bool bInconsistentUsage = false;

    while(kIter)
    {
        NiTPrimitiveSet<NiOpTerminator*>* pkOpTermSet = NULL;
        NiUInt32 uiStreamID = 0;
        kStreamIDToTermsMap.GetNext(kIter, uiStreamID, pkOpTermSet);

        NiUInt32 uiTermSetCount = pkOpTermSet->GetSize();
        NiOpTerminator* pkTerm = pkOpTermSet->GetAt(0);
        NiUInt8 uiFinalAccess = pkTerm->GetAccessFlags();
        NiDataStream::Usage eFinalUsage = pkTerm->GetUsage();

        for(NiUInt32 uiTermSet=1; uiTermSet<uiTermSetCount; uiTermSet++)
        {
            pkTerm = pkOpTermSet->GetAt(uiTermSet);
            if (uiFinalAccess != pkTerm->GetAccessFlags())
                bInconsistentAccess = true;

            if (eFinalUsage != pkTerm->GetUsage())
                bInconsistentUsage = true;
        }
    }

    kIter = kStreamIDToTermsMap.GetFirstPos();
    while(kIter)
    {
        NiTPrimitiveSet<NiOpTerminator*>* pkOpTermSet = NULL;
        NiUInt32 uiStreamID = 0;
        kStreamIDToTermsMap.GetNext(kIter, uiStreamID, pkOpTermSet);
        NiDelete pkOpTermSet;
    }

    if (bInconsistentAccess || bInconsistentUsage)
        return false;

    return true;
}
//---------------------------------------------------------------------------
bool NiOpProfile::ValidateProfile()
{
    if (Validate(m_kTerminatorFinalizeNodes) == false)
        return false;

    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
void NiOpProfile::CopyMembers(NiOpProfile* pkDest, NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    pkDest->m_strName = m_strName;
    pkDest->m_strDescription = m_strDescription;
    pkDest->m_eRenderer = m_eRenderer;

    NiTPrimitiveArray<NiOpNode*> kStartPackNodes;
    kStartPackNodes.SetSize(m_kStartFinalizeNodes.GetSize());
    pkDest->m_kStartFinalizeNodes.SetSize(m_kStartFinalizeNodes.GetSize());
    for (NiUInt32 ui = 0; ui < m_kStartFinalizeNodes.GetSize(); ui++)
    {
        NiOpStart* pkClone = 
            (NiOpStart*)m_kStartFinalizeNodes[ui]->Clone(kCloning);
        if (pkClone)
        {
            pkDest->m_kStartFinalizeNodes.Add(pkClone);
            kStartPackNodes.Add(pkClone);
        }
    }

    pkDest->m_kTerminatorFinalizeNodes.SetSize(
        m_kTerminatorFinalizeNodes.GetSize());
    for (NiUInt32 ui = 0; ui < m_kTerminatorFinalizeNodes.GetSize(); ui++)
    {
        NiOpTerminator* pkClone = 
            (NiOpTerminator*)m_kTerminatorFinalizeNodes[ui]->Clone(kCloning);
        if (pkClone)
        {
            pkDest->m_kTerminatorFinalizeNodes.Add(pkClone);
        }
    }
}
//---------------------------------------------------------------------------
bool NiOpProfile::IsEqual(NiObject* pkOther)
{
    if (!NiObject::IsEqual(pkOther))
        return false;

    NiOpProfile* pkOtherProfile = NiDynamicCast(NiOpProfile, pkOther);
    if (!pkOtherProfile)
        return false;

    if (m_strName != pkOtherProfile->m_strName)
        return false;

    if (m_strDescription != pkOtherProfile->m_strDescription)
        return false;

    if (m_eRenderer != pkOtherProfile->m_eRenderer)
        return false;

    if (m_kStartFinalizeNodes.GetSize() != 
        pkOtherProfile->m_kStartFinalizeNodes.GetSize())
    {
        return false;
    }

    for (NiUInt32 ui = 0; ui < m_kStartFinalizeNodes.GetSize(); ui++)
    {
        if (!m_kStartFinalizeNodes.GetAt(ui)->IsEqual(
            pkOtherProfile->m_kStartFinalizeNodes.GetAt(ui)))
        {
            return false;
        }
    }

    if (m_kTerminatorFinalizeNodes.GetSize() != 
        pkOtherProfile->m_kTerminatorFinalizeNodes.GetSize())
    {
        return false;
    }

    for (NiUInt32 ui = 0; ui < m_kTerminatorFinalizeNodes.GetSize(); ui++)
    {
        if (!m_kTerminatorFinalizeNodes.GetAt(ui)->IsEqual(
            pkOtherProfile->m_kTerminatorFinalizeNodes.GetAt(ui)))
        {
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
