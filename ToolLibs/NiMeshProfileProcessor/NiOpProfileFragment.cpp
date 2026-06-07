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
#include <NiMeshRequirements.h>

#include "NiMeshProfileProcessor.h"
#include "NiMeshErrorInterface.h"
#include "NiOpProfileFragment.h"
#include "NiOpTerminator.h"
#include "NiOpCastConvert.h"
#include "NiInterleaveProfile.h"

NiImplementCreateClone(NiOpProfileFragment);

typedef NiInterleaveProfile::StreamLayout StreamLayout;
typedef NiInterleaveProfile::StreamLayoutSet StreamLayoutSet;
typedef NiInterleaveProfile::Element Element;
typedef NiTPointerMap<NiUInt32, StreamLayout*> StreamGroupStreamLayoutMap;

NiToolPipelineCloneHelper* NiOpProfileFragment::ms_pkCloneHelper = NULL;

//---------------------------------------------------------------------------
// NiOpProfileFragment Helper functions
//---------------------------------------------------------------------------
void AddToThisStreamGroup(StreamGroupStreamLayoutMap& kStreamGroups,
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

//---------------------------------------------------------------------------
NiOpProfileFragment::NiOpProfileFragment() : m_bHadResolveErrors(false)
{
}
//---------------------------------------------------------------------------
NiOpProfileFragment::~NiOpProfileFragment()
{
    //m_kStartNodes.RemoveAll();
    m_kTerminatorNodes.RemoveAll();
}
//---------------------------------------------------------------------------
void NiOpProfileFragment::SetHadResolveErrors(bool bHadResolveErrors)
{
    m_bHadResolveErrors = bHadResolveErrors;
}
//---------------------------------------------------------------------------
bool NiOpProfileFragment::GetHadResolveErrors()
{
    return m_bHadResolveErrors;
}
//---------------------------------------------------------------------------
void NiOpProfileFragment::GetRequirements(NiMeshRequirements&
    kProfileRequirements)
{
    // The purpose of GetRequirements is to translate the ProfileFragment into
    // terms of NiMeshRequirements. The requirements will not only reflect
    // how the streams are expected to turn out, but also will express how the
    // fragment can be modified (AdjustFinalizeOps). The requirements that
    // are obtained from this method can be merged with other fragments and
    // mesh modifiers to select how the data should ultimately get processed.

    // Important note: Fragments are assumed to have been initialized.

    NiUInt32 uiSet = (NiUInt32)-1;

    if (kProfileRequirements.GetSetCount())
    {
        NIASSERT(kProfileRequirements.GetSetCount() == 1);
        uiSet = 0;
    }

    for(NiUInt32 ui=0; ui < m_kTerminatorNodes.GetSize(); ui++)
    {
        // for each terminator node...
        NiOpTerminator* pkOp = m_kTerminatorNodes.GetAt(ui);
        NIASSERT(pkOp);

        NiFixedString kSemantic;
        NiUInt8 uiSemanticIndex;
        if (!pkOp->GetSemantic(kSemantic, uiSemanticIndex))
        {
            NIASSERT(!"Could not determine semantic or index!");
            continue; 
        }

        NiDataStreamElement::Format eExpectedFinalFormat = 
            NiDataStreamElement::F_UNKNOWN;
        if ((pkOp->RetrieveExpectedFinalFormat(eExpectedFinalFormat) 
            == false) || (eExpectedFinalFormat ==
            NiDataStreamElement::F_UNKNOWN))
        {
            // The stream may have been optional and not found.
            NIASSERT(pkOp->IsRequired() == false);
            continue;
        }

        NiTPrimitiveSet<NiDataStreamElement::Format> kFormats;

        if (pkOp->IsStrictFormat() == false)
        {
            // If the profile is not strict, then their may be some
            // flexibility allowed for modifying the final format. This
            // flexiblity is not blanketed, so adding a F_UNKNOWN to allow
            // the requirements to merge with any format that mesh modifiers
            // specify would be inappropriate. Instead, only formats that
            // can be cast to using CastConvert are allowed. CastConvert
            // must retain the same component count.
            
            NiUInt8 uiExpectedCompCnt = 
                NiDataStreamElement::GetComponentCount(eExpectedFinalFormat);
            bool bNorm = false;

            for(NiUInt32 uiBool=0; uiBool<2; uiBool++)
            {
                if (uiBool==1) // switch bNorm the second time around
                    bNorm = true;

                kFormats.Add(NiDataStreamElement::GetPredefinedFormat(
                    NiDataStreamElement::T_INT8,uiExpectedCompCnt,bNorm));
                kFormats.Add(NiDataStreamElement::GetPredefinedFormat(
                    NiDataStreamElement::T_UINT8, uiExpectedCompCnt,bNorm));
                kFormats.Add(NiDataStreamElement::GetPredefinedFormat(
                    NiDataStreamElement::T_INT16, uiExpectedCompCnt,bNorm));
                kFormats.Add(NiDataStreamElement::GetPredefinedFormat(
                    NiDataStreamElement::T_UINT16, uiExpectedCompCnt,bNorm));
                kFormats.Add(NiDataStreamElement::GetPredefinedFormat(
                    NiDataStreamElement::T_INT32, uiExpectedCompCnt,bNorm));
                kFormats.Add(NiDataStreamElement::GetPredefinedFormat(
                    NiDataStreamElement::T_UINT32, uiExpectedCompCnt,bNorm));
                kFormats.Add(NiDataStreamElement::GetPredefinedFormat(
                    NiDataStreamElement::T_FLOAT16,uiExpectedCompCnt,bNorm));
                kFormats.Add(NiDataStreamElement::GetPredefinedFormat(
                    NiDataStreamElement::T_FLOAT32,uiExpectedCompCnt,bNorm));
            }
        }
        
        // AddUnique in case already added
        kFormats.AddUnique(eExpectedFinalFormat);

        // Access is never strict, though there is a guarantee that
        // "less access" will never be given. Access comes from the inputs
        // or by the profile.
        // Usage is always strict (NiMeshRequirements doesn't know how to
        // interpret 'dont cares'). Usage will either be specified by the
        // profile or by the inputs.
        NiUInt8 uiAccessFlags = 0;
        NiDataStream::Usage eUsage = NiDataStream::USAGE_MAX_TYPES;
        NIVERIFY(pkOp->RetrieveExpectedFinalAccessAndUsage(uiAccessFlags,
            eUsage));
        
        NIASSERT(uiAccessFlags != 0);
        NIASSERT(eUsage != NiDataStream::USAGE_MAX_TYPES);

        NiMeshRequirements::SemanticRequirement* pkSemanticReq = 
            NiNew NiMeshRequirements::SemanticRequirement();
        pkSemanticReq->m_kSemantic = kSemantic;
        pkSemanticReq->m_uiIndex = uiSemanticIndex;

        if (pkOp->IsStrictInterleave())
        {
            pkSemanticReq->m_eStreamBehavior =
                NiMeshRequirements::STRICT_INTERLEAVE;
        }
        else
        {
            pkSemanticReq->m_eStreamBehavior = NiMeshRequirements::CAN_SPLIT;
        }
        //pkSemanticReq->m_bStrictInterleave = pkOp->IsStrictInterleave();
        pkSemanticReq->m_uiStreamID = pkOp->GetStreamID();
        pkSemanticReq->m_kUsage = eUsage;
        pkSemanticReq->m_uiAccessFlags = uiAccessFlags;
        for (NiUInt32 uiF=0; uiF<kFormats.GetSize(); uiF++)
            pkSemanticReq->m_kFormats.Add(kFormats.GetAt(uiF));

        // Create the set
        if (uiSet == (NiUInt32)-1)
            uiSet = kProfileRequirements.CreateNewRequirementSet();

        kProfileRequirements.AddRequirement(uiSet, pkSemanticReq);
    }
}
//---------------------------------------------------------------------------
void NiOpProfileFragment::AdjustFinalizeOps(NiMeshRequirements& kRequirements)
{
    // Any of the requirements should be valid - the selection of the first
    // is a convenient choice.
    if (kRequirements.GetSetCount() < 1)
        return; // no sets
    
    NiUInt32 uiSet = 0;

    // Create a mapping from the semantic/index to terminator so that it isn't
    // necessary to iterate over every terminator for each requirement.
    NiTStringMap<NiOpTerminator*> kSemanticToTerminator;
    for (NiUInt32 ui = 0; ui < m_kTerminatorNodes.GetSize(); ui++)
    {
        NiFixedString kSemantic;
        NiUInt8 uiSemanticIndex;
        if (m_kTerminatorNodes[ui]->GetSemantic(kSemantic, uiSemanticIndex) ==
            false)
        {
            continue;
        }

        NiString strKey;
        strKey.Format("%s:%d", (const char*)kSemantic, uiSemanticIndex);
        kSemanticToTerminator.SetAt(strKey,
            (NiOpTerminator*)(m_kTerminatorNodes[ui]));
    }

    for (NiUInt32 ui = 0; ui < kRequirements.GetRequirementCount(uiSet); ui++)
    {
        NiMeshRequirements::SemanticRequirement* pkRequirement =
            kRequirements.GetRequirement(uiSet, ui);

        if (!pkRequirement)
            continue;

        NiOpTerminator* pkTerminator = NULL;
        NiString strKey;
        strKey.Format("%s:%d", (const char*)pkRequirement->m_kSemantic,
            pkRequirement->m_uiIndex);
        kSemanticToTerminator.GetAt(strKey, pkTerminator);

        if (!pkTerminator)
            continue;

        // Adjust the terminator node
        NiUInt32 uiStreamID = pkRequirement->m_uiStreamID;
        NiUInt32 uiAccessFlags = pkRequirement->m_uiAccessFlags;
        if (!uiAccessFlags)
        {
            // Grab from the DCCT stream or profile in the event that it is 0
            NiUInt8 uiExpectedAccess = 0;
            NiDataStream::Usage eExpectedUsage;
            pkTerminator->RetrieveExpectedFinalAccessAndUsage(
                uiExpectedAccess, eExpectedUsage);
            uiAccessFlags = uiExpectedAccess;
        }

        NIASSERT(uiAccessFlags);
        NiDataStream::Usage eUsage = pkRequirement->m_kUsage;

        NIASSERT(NiDataStream::IsAccessRequestValid((NiUInt8)uiAccessFlags, eUsage));

        NiUInt8 uiStrictFlags = pkTerminator->GetStrictFlags();
        // only strict flag that might change is interleave
        if (pkRequirement->m_eStreamBehavior ==
            NiMeshRequirements::STRICT_INTERLEAVE)
        {
            uiStrictFlags |= NiMeshProfileProcessor::STRICT_INTERLEAVE;
        }

        pkTerminator->SetParameters(uiStreamID, (NiUInt8)uiAccessFlags, eUsage,
            uiStrictFlags, pkTerminator->IsRequired());

        // Adjust the format if we need to
        NiDataStreamElement::Format eFormat =
            NiDataStreamElement::F_UNKNOWN;

        NiDataStreamElement::Format eFormatCheck = 
            NiDataStreamElement::F_UNKNOWN;
        NIVERIFY(pkTerminator->RetrieveExpectedFinalFormat(
            eFormatCheck));

        // Set the format to the first entry... this will be used
        // as a backup.
        if (pkRequirement->m_kFormats.GetSize() > 0)
            eFormat = pkRequirement->m_kFormats.GetAt(0);

        // Attempt to satisfy the profiles original expectation
        if (eFormatCheck != NiDataStreamElement::F_UNKNOWN)
        {
            NiUInt32 uiPossibleFCnt = pkRequirement->m_kFormats.GetSize();
            for(NiUInt32 uiFIndex=0; uiFIndex < uiPossibleFCnt; uiFIndex++)
            {
                if (pkRequirement->m_kFormats.GetAt(uiFIndex) ==
                    eFormatCheck)
                {
                    eFormat = eFormatCheck;
                    break;
                }
            }
        }

        if (eFormat == NiDataStreamElement::F_UNKNOWN)
            continue; // Leave as is.

        // No work needed
        if (eFormatCheck == eFormat)
            continue;

        // Mesh Modifiers require changing the format. They should *not*
        // request special formats or unknown ones, so verify that.
        NIASSERT(NiDataStreamElement::IndexOf(eFormat) <= 
            NiDataStreamElement::IndexOf(NiDataStreamElement::F_FLOAT32_4));

        // We should not expect original format to be of special types
        NIASSERT(NiDataStreamElement::IndexOf(eFormatCheck) <=
            NiDataStreamElement::IndexOf(NiDataStreamElement::F_FLOAT32_4));

        NiDataStreamElement::Type eType = 
            NiDataStreamElement::GetType(eFormat);

        NiMeshProfileOperations::CastConvertType eConvertType =
            NiMeshProfileOperations::CT_TYPECLAMP;

        if (NiDataStreamElement::IsNormalized(eFormat))
            eConvertType = NiMeshProfileOperations::CT_NORMALIZE;

        // There are currently three operations that may cause a format
        // change: CastConvert, MergeAndSwizzle, and Reinterpret. Here, the
        // last format altering operation is found and if it happens to be
        // a CastConvert, it is simply modified. Otherwise, the code passes
        // through and a "new" CastConvert operation is created as the final
        // operation.
        NiOpNode* pkLastFormatAlteringNode =
            pkTerminator->RetrieveFinalFormatAlteringNode();
        if (pkLastFormatAlteringNode && pkLastFormatAlteringNode->
            GetOperationName() == "CastConvert")
        {
            // modify in place...
            ((NiOpCastConvert*)pkLastFormatAlteringNode)->SetParameters(eType,
                eConvertType);
            continue;
        }

        NiOpNode* pkInput = pkTerminator->GetInput();
        NIASSERT(pkInput);

        // Since the cast convert node does not exist, one must be added
        NiOpCastConvert* pkCastConvert = NiNew NiOpCastConvert();
        pkCastConvert->SetParameters(eType, eConvertType);

        // Insert it into the terminator node
        pkCastConvert->SetInput(pkInput);
        pkTerminator->SetInput(pkCastConvert);
    }
}
//---------------------------------------------------------------------------
bool NiOpProfileFragment::Interleave(NiToolPipelineCloneHelper::CloneSetPtr&
    spCloneSet, NiMeshErrorInterface* pkReportedErrors) const
{
    // Grab streamrefs from mesh and assign to terminator nodes.
    NIASSERT(spCloneSet->GetSize() > 0);

    NiUInt32 uiNumTerminators = m_kTerminatorNodes.GetSize();

    // If no terminator pack nodes specified, nothing to do...
    if (uiNumTerminators == 0)
        return false;

    // Find the desired StreamGroup / layout
    StreamGroupStreamLayoutMap kStreamGroups;

    // Use the first mesh to grab the datastreams.
    NiMesh* pkMesh = spCloneSet->GetAt(0); 

    for (NiUInt32 ui=0; ui < uiNumTerminators; ui++)
    {
        NiOpTerminator* pkOp = m_kTerminatorNodes.GetAt(ui);
        NIASSERT(pkOp);
        if (!pkOp)
            continue;

        // Grab the StreamGroup, Semantic, and SemanticIndex from the
        // terminator node.
        NiUInt32 uiStreamID = pkOp->GetStreamID();
        NiFixedString kSemantic;
        NiUInt8 uiSemanticIndex;
        pkOp->GetSemantic(kSemantic, uiSemanticIndex);

        NiDataStreamRef* pkStreamRef;
        NiDataStreamElement kElement;
        if (pkMesh->FindStreamRefAndElementBySemantic(
            kSemantic, uiSemanticIndex, NiDataStreamElement::F_UNKNOWN,
            pkStreamRef, kElement)
            == false)
        {
            // The failure should have been detected prior to Interleave.
            NIASSERT(pkOp->IsRequired() == false);
            NIASSERT(pkOp->IsStrictInterleave() == false);
            continue;
        }

        // Coalesce options not supported. Coalescing is performed by default.
        AddToThisStreamGroup(kStreamGroups, uiStreamID, kSemantic,
            uiSemanticIndex, pkOp->IsStrictInterleave(), true);
    }

    NiInterleaveProfile kProfile;

    // Using StreamGroups mapping, build the profile
    NiTMapIterator kIter = kStreamGroups.GetFirstPos();
    while(kIter)
    {
        NiUInt32 uiStreamID;
        StreamLayout* pkStreamLayout;
        kStreamGroups.GetNext(kIter, uiStreamID, pkStreamLayout);
        kProfile.GetStreamLayoutSet().Add(*pkStreamLayout);
        NiDelete pkStreamLayout;
    }
    kStreamGroups.RemoveAll();

    NiInterleaver kInterleaver;
    bool bResult;
    NIASSERT(ms_pkCloneHelper);
    bResult = kInterleaver.Interleave(spCloneSet, *ms_pkCloneHelper, &kProfile,
        pkReportedErrors);
    return bResult;
}
//---------------------------------------------------------------------------



//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
void NiOpProfileFragment::CopyMembers(NiOpProfileFragment* pkDest,
    NiCloningProcess& kCloning)
{
    NiObject::CopyMembers(pkDest, kCloning);

    pkDest->m_kTerminatorNodes.SetSize(m_kTerminatorNodes.GetSize());
    for (NiUInt32 ui = 0; ui < m_kTerminatorNodes.GetSize(); ui++)
    {
        NiOpTerminator* pkClone = 
            (NiOpTerminator*)m_kTerminatorNodes[ui]->Clone(kCloning);
        if (pkClone)
        {
            pkDest->m_kTerminatorNodes.Add(pkClone);
        }
    }

    m_bHadResolveErrors = pkDest->GetHadResolveErrors();
}
//---------------------------------------------------------------------------
bool NiOpProfileFragment::IsEqual(NiObject* pkOther)
{
    if (!NiObject::IsEqual(pkOther))
        return false;

    NiOpProfileFragment* pkOtherProfile =
        NiDynamicCast(NiOpProfileFragment, pkOther);

    if (!pkOtherProfile)
        return false;

    if (m_kTerminatorNodes.GetSize() != 
        pkOtherProfile->m_kTerminatorNodes.GetSize())
    {
        return false;
    }

    for (NiUInt32 ui = 0; ui < m_kTerminatorNodes.GetSize(); ui++)
    {
        if (!m_kTerminatorNodes.GetAt(ui)->IsEqual(
            pkOtherProfile->m_kTerminatorNodes.GetAt(ui)))
        {
            return false;
        }
    }

    if (m_bHadResolveErrors != pkOtherProfile->GetHadResolveErrors())
        return false;

    return true;
}
//---------------------------------------------------------------------------
