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

#include "NiMeshProfileProcessorLibType.h"
#include <NiRefObject.h>
#include <NiMeshRequirements.h>
#include <NiMeshErrorInterface.h>

#include "NiMergeRequirements.h"
#include "NiMPPMessages.h"

typedef NiMeshRequirements::SemanticRequirement SemanticRequirement;
typedef NiMeshRequirements::SemanticRequirementPtr SemanticRequirementPtr;
typedef NiTObjectPtrSet<SemanticRequirementPtr> StreamSet;
typedef NiTObjectPtrSet<SemanticRequirementPtr> RequirementSet;

//--------------------------------------------------------------------------
// Internal Helper classes
//--------------------------------------------------------------------------
class MergeInfo
{
public:
    MergeInfo() : uiNextStreamID(0) {};
    ~MergeInfo();

    typedef NiTMap<NiUInt32, StreamSet*> IDToStreamMap;
    typedef NiTPointerMap<SemanticRequirement*, SemanticRequirement*>
        ReqToReqMap;

    void CreateStreamMap(NiTMap<NiUInt32, StreamSet*>&
        kStreamMap, RequirementSet* pkSet);
    bool Init(RequirementSet* pkProfileSet,
        RequirementSet* pkModifierSet,
        NiMeshErrorInterface* pkLog);
    void ClearIDToStreamMap(IDToStreamMap& kIDToStreamMap);

    NiTStringMap<bool> kProcessedMap;
    IDToStreamMap kProfileIDToStream;
    IDToStreamMap kModifierIDToStream;
    ReqToReqMap kProfileReqToModifierReq;
    ReqToReqMap kModifierReqToProfileReq;

    NiUInt32 uiNextStreamID;

    RequirementSet kMergedSet;
};
//---------------------------------------------------------------------------
MergeInfo::~MergeInfo()
{
    // Deallocate memory from stream maps
    ClearIDToStreamMap( kProfileIDToStream );
    ClearIDToStreamMap( kModifierIDToStream );
}
//---------------------------------------------------------------------------
void MergeInfo::ClearIDToStreamMap(IDToStreamMap& kIDToStreamMap)
{
    NiTMapIterator kIter;
    kIter = kIDToStreamMap.GetFirstPos();
    while(kIter)
    {
        NiUInt32 uiID;
        StreamSet* pkStream;
        kIDToStreamMap.GetNext(kIter, uiID, pkStream);
        NiDelete pkStream;
    }
    kIDToStreamMap.RemoveAll();
}
//---------------------------------------------------------------------------
void MergeInfo::CreateStreamMap(
    NiTMap<NiUInt32, StreamSet*>& kStreamMap, 
    RequirementSet* pkSet)
{
    for (NiUInt32 ui = 0; ui < pkSet->GetSize(); ui++)
    {
        SemanticRequirement* pkReq = pkSet->GetAt(ui);

        StreamSet* pkStream;
        if (kStreamMap.GetAt(pkReq->m_uiStreamID, pkStream) == false)
        {
            // hasn't been added yet, so add
            pkStream = NiNew StreamSet();
            kStreamMap.SetAt(pkReq->m_uiStreamID, pkStream);
        }

        pkStream->Add(pkReq);
    }
}
//---------------------------------------------------------------------------
bool MergeInfo::Init(RequirementSet* pkProfileSet,
    RequirementSet* pkModifierSet,
    NiMeshErrorInterface* pkLog)
{
    uiNextStreamID = 0;

     NiTStringMap<bool> kValidationMap;

    // [1] First we create a temporary Validation Map. This is a simple
    // mapping of every semantic found in the Profile. We ensure that 
    // duplicates are not found.
    for (NiUInt32 ui = 0; ui < pkProfileSet->GetSize(); ui++)
    {
        SemanticRequirement* pkReq = pkProfileSet->GetAt(ui);

        NIASSERT(pkReq->m_eStreamBehavior ==
            NiMeshRequirements::STRICT_INTERLEAVE ||
            pkReq->m_eStreamBehavior == NiMeshRequirements::CAN_SPLIT);

        bool bStatus;
        NiString strKey;
        strKey.Format("%s:%d", (const char*)pkReq->m_kSemantic,
            pkReq->m_uiIndex);
        if (kValidationMap.GetAt(strKey, bStatus) == true)
        {
            // This is a problem because the semantic has appeared twice.
            // Multiple occurrences of the semantic should not occur.
            NIASSERT(!"Duplicate Semantics found in requirements!");
            return false;
        }

        kValidationMap.SetAt(strKey, false);

        if (pkReq->m_uiStreamID > uiNextStreamID)
            uiNextStreamID = pkReq->m_uiStreamID;
    }

    uiNextStreamID++; // The next StreamID in case it is needed.

    // [2] Check the ModifierSet's semantic requirements. If there are any
    // present that are not in the ValidationMap, then the merge cannot
    // happen because the semantic cannot be satisfied by the Profile.
    for (NiUInt32 ui = 0; ui < pkModifierSet->GetSize(); ui++)
    {
        SemanticRequirement* pkReq = pkModifierSet->GetAt(ui);

        if (pkReq->m_eStreamBehavior == NiMeshRequirements::FLOATER)
        {
            // Force FLOATERs to a single stream.
            pkReq->m_uiStreamID = (NiUInt32)-1;
        }

        bool bStatus;
        NiString strKey;
        strKey.Format("%s:%d", (const char*)pkReq->m_kSemantic,
            pkReq->m_uiIndex);
        if (kValidationMap.GetAt(strKey, bStatus) == false)
        {
            // The semantic could not be found in the Profile, so the merge
            // must fail.

            if (pkLog)
            {
                pkLog->ReportError(NiMPPMessages::
                    ERROR_MERGE_SEMANTIC_CONFLICT_1ARG, (const char*)strKey);
            }

            return false;
        }
    }
    kValidationMap.RemoveAll(); 

    // [3] Organize semantics into "streams". This is done by grouping 
    // semantics having the same StreamID together. We store this
    // grouping within a hashmap indexiable by StreamID. This is done for 
    // both Profile and modifier.
    // Note: CreateStreamMap allocates StreamSets, so ensure it is deleted
    // on exit of this function.
    CreateStreamMap(kProfileIDToStream, pkProfileSet);
    CreateStreamMap(kModifierIDToStream, pkModifierSet);

    // [4] Constuct a mapping from every Profile Semantic to its Modifier
    // counterpart. Another mapping for convenience.
    for (NiUInt32 ui = 0; ui < pkProfileSet->GetSize(); ui++)
    {
        SemanticRequirement* pkReq1 = pkProfileSet->GetAt(ui);

        for (NiUInt32 uj=0; uj < pkModifierSet->GetSize(); uj++)
        {
            SemanticRequirement* pkReq2 = pkModifierSet->GetAt(uj);

            if (pkReq1->m_kSemantic == pkReq2->m_kSemantic &&
                pkReq1->m_uiIndex == pkReq2->m_uiIndex)
            {
                kProfileReqToModifierReq.SetAt(pkReq1, pkReq2);
                kModifierReqToProfileReq.SetAt(pkReq2, pkReq1);
                break;
            }
        }
    }

    return true;
}
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Internal Helper Functions
//--------------------------------------------------------------------------
static bool MergeStream(StreamSet* pkProfileStream, MergeInfo& kInfo,
    NiMeshErrorInterface* pkLog);
static bool MergeStream(StreamSet* pkProfileStream, NiUInt32
    uiModifierStreamID, MergeInfo& kInfo, NiMeshErrorInterface* pkLog);

bool MergeAccessFlags(NiUInt8 uiLeftAccessFlags, NiUInt8 uiRightAccessFlags,
    NiUInt8& uiMergedAccessFlags)
{
    uiMergedAccessFlags = 0;

    if (uiLeftAccessFlags & NiDataStream::ACCESS_CPU_READ ||
        uiRightAccessFlags & NiDataStream::ACCESS_CPU_READ)
    {
        uiMergedAccessFlags |= NiDataStream::ACCESS_CPU_READ;
    }
    if (uiLeftAccessFlags & NiDataStream::ACCESS_GPU_READ ||
        uiRightAccessFlags & NiDataStream::ACCESS_GPU_READ)
    {
        uiMergedAccessFlags |= NiDataStream::ACCESS_GPU_READ;
    }
    if (uiLeftAccessFlags & NiDataStream::ACCESS_GPU_WRITE ||
        uiRightAccessFlags & NiDataStream::ACCESS_GPU_WRITE)
    {
        uiMergedAccessFlags |= NiDataStream::ACCESS_GPU_WRITE;
    }
    if (uiLeftAccessFlags & NiDataStream::ACCESS_CPU_WRITE_MUTABLE ||
        uiRightAccessFlags & NiDataStream::ACCESS_CPU_WRITE_MUTABLE)
    {
        uiMergedAccessFlags |= NiDataStream::ACCESS_CPU_WRITE_MUTABLE;
    }
    else if (uiLeftAccessFlags & NiDataStream::ACCESS_CPU_WRITE_VOLATILE
        || uiRightAccessFlags & NiDataStream::ACCESS_CPU_WRITE_VOLATILE)
    {
        if (uiMergedAccessFlags & NiDataStream::ACCESS_CPU_READ)
        {
            uiMergedAccessFlags |= NiDataStream::ACCESS_CPU_WRITE_MUTABLE;
        }
        else
        {
            uiMergedAccessFlags |= NiDataStream::ACCESS_CPU_WRITE_VOLATILE;
        }
    }
    else if (uiLeftAccessFlags & NiDataStream::ACCESS_CPU_WRITE_STATIC
        || uiRightAccessFlags & NiDataStream::ACCESS_CPU_WRITE_STATIC)
    {
        uiMergedAccessFlags |= NiDataStream::ACCESS_CPU_WRITE_STATIC;
    }

    if ((uiLeftAccessFlags || uiRightAccessFlags) && 
        (uiMergedAccessFlags == 0))
    {
        return false;
    }

    // Assume Usage is anything other than USER
    return NiDataStream::IsAccessRequestValid(uiMergedAccessFlags, 
        NiDataStream::USAGE_VERTEX);
}
//---------------------------------------------------------------------------
SemanticRequirement* MergeSemantics(SemanticRequirement* pkLeft,
    SemanticRequirement* pkRight, NiUInt32 uiStreamID,
    NiMeshErrorInterface* pkLog)
{
    SemanticRequirement* pkResult = NiNew SemanticRequirement;

    NIASSERT(pkLeft->m_kSemantic == pkRight->m_kSemantic);
    pkResult->m_kSemantic = pkLeft->m_kSemantic;

    NIASSERT(pkLeft->m_uiIndex == pkRight->m_uiIndex);
    pkResult->m_uiIndex = pkLeft->m_uiIndex;

    // Usage user can be reconcile, but any other type of conflict must
    // fail
    if (pkLeft->m_kUsage == NiDataStream::USAGE_USER ||
        pkRight->m_kUsage == NiDataStream::USAGE_USER ||
        pkLeft->m_kUsage == pkRight->m_kUsage)
    {
        if (pkLeft->m_kUsage == NiDataStream::USAGE_USER)
            pkResult->m_kUsage = pkRight->m_kUsage;
        else
            pkResult->m_kUsage = pkLeft->m_kUsage;
    }
    else
    {
        if (pkLog)
        {
            pkLog->ReportError(NiMPPMessages::ERROR_MERGE_USAGE_CONFLICT_2ARGS,
                (const char*)pkLeft->m_kSemantic, pkLeft->m_uiIndex);
        }

        NiDelete pkResult;
        return NULL;
    }

    pkResult->m_uiAccessFlags = 0;
    if (MergeAccessFlags(pkLeft->m_uiAccessFlags, pkRight->m_uiAccessFlags,
        pkResult->m_uiAccessFlags) == false)
    {
        // Merging Access flags conflict

        if (pkLog)
        {
            pkLog->ReportError(NiMPPMessages::
                ERROR_MERGE_ACCESS_CONFLICT_2ARGS,
                (const char*)pkLeft->m_kSemantic, pkLeft->m_uiIndex);
        }

        NiDelete pkResult;
        return NULL;
    }

    // Formats found in both inputs end up in output
    for (NiUInt32 ui = 0; ui < pkLeft->m_kFormats.GetSize(); ui++)
    {
        NiDataStreamElement::Format eLeftFormat =
            pkLeft->m_kFormats.GetAt(ui);        

        for (NiUInt32 uj = 0; uj < pkRight->m_kFormats.GetSize(); uj++)
        {
            NiDataStreamElement::Format eRightFormat =
                pkRight->m_kFormats.GetAt(uj);
            if (eLeftFormat == NiDataStreamElement::F_UNKNOWN)
            {
                pkResult->m_kFormats.AddUnique(eRightFormat);
            }
            else if (eRightFormat == NiDataStreamElement::F_UNKNOWN)
            {
                pkResult->m_kFormats.AddUnique(eLeftFormat);
            }
            else if (eLeftFormat == eRightFormat)
            {
                pkResult->m_kFormats.AddUnique(eLeftFormat);
            }
        }
    }

    if (pkResult->m_kFormats.GetSize() == 0)
    {
        // No overlapping formats

        if (pkLog)
        {
            pkLog->ReportError(NiMPPMessages::
                ERROR_MERGE_FORMAT_CONFLICT_2ARGS,
                (const char*)pkLeft->m_kSemantic, pkLeft->m_uiIndex);
        }

        NiDelete pkResult;
        return NULL;
    }

    pkResult->m_uiStreamID = uiStreamID;

    // Profile can only specify strict or preference
    NIASSERT(pkLeft->m_eStreamBehavior == 
        NiMeshRequirements::STRICT_INTERLEAVE ||
        pkLeft->m_eStreamBehavior == NiMeshRequirements::CAN_SPLIT);

    if (pkRight->m_eStreamBehavior == NiMeshRequirements::STRICT_INTERLEAVE)
        pkResult->m_eStreamBehavior = NiMeshRequirements::STRICT_INTERLEAVE;
    else
        pkResult->m_eStreamBehavior = pkLeft->m_eStreamBehavior;

    return pkResult;
}
//---------------------------------------------------------------------------
bool MergeStream(StreamSet* pkProfileStream, MergeInfo& kInfo,
    NiMeshErrorInterface* pkLog)
{
    bool bStrictsFound = false;
    for(NiUInt32 ui=0; ui < pkProfileStream->GetSize(); ui++)
    {
        SemanticRequirement* pkReq = pkProfileStream->GetAt(ui);
        NIASSERT(pkReq);

        if (pkReq->m_eStreamBehavior ==
            NiMeshRequirements::STRICT_INTERLEAVE)
        {
            bStrictsFound = true;
        }

        SemanticRequirement* pkMatchReq = NULL;
        if (kInfo.kProfileReqToModifierReq.GetAt(pkReq, pkMatchReq) == false)
        {
            continue;
        }

        NiUInt32 uiModifierStreamID = pkMatchReq->m_uiStreamID;

        return MergeStream(pkProfileStream, uiModifierStreamID, kInfo, pkLog);
    }

    // None of the semantics were found in modifier streams, so all of the
    // semantics can be added as they are.
    for(NiUInt32 ui=0; ui < pkProfileStream->GetSize(); ui++)
    {
        SemanticRequirement* pkReq = pkProfileStream->GetAt(ui);
        SemanticRequirement* pkNewReq = NiNew SemanticRequirement();

        *pkNewReq = *pkReq;

        if (bStrictsFound == false)
        {
            // Only streams having stricts retain their original ID.
            pkNewReq->m_uiStreamID = kInfo.uiNextStreamID;
        }

        kInfo.kMergedSet.Add(pkNewReq);
    }

    if (bStrictsFound == false)
        kInfo.uiNextStreamID++;

    return true;
}
//---------------------------------------------------------------------------
bool MergeStream(StreamSet* pkProfileStream, NiUInt32
    uiModifierStreamID, MergeInfo& kInfo, NiMeshErrorInterface* pkLog)
{
    StreamSet* pkModifierStream = NULL;
    NIVERIFY(kInfo.kModifierIDToStream.GetAt(uiModifierStreamID,
        pkModifierStream));

    NIASSERT(pkProfileStream->GetSize() > 0);
    NiUInt32 uiProfileStreamID = pkProfileStream->GetAt(0)->m_uiStreamID;

    // [1] Check for StrictInterleave semantics that exist in modifier, 
    // but not in the Profile. These would only present a problem if there
    // are also Semantics in the modifier that are marked StrictInterleave
    // and are found in the Profile. In that case, we have a conflict.
    NiUInt32 uiStrictsMatched = 0;
    NiUInt32 uiStrictsNotMatched = 0;
    for(NiUInt32 ui=0; ui < pkModifierStream->GetSize(); ui++)
    {
        SemanticRequirement* pkReq = pkModifierStream->GetAt(ui);
        NIASSERT(pkReq);

        if (pkReq->m_eStreamBehavior !=
            NiMeshRequirements::STRICT_INTERLEAVE)
        {
            continue;
        }

        // Every modifier semantic should map to a Profile w/o fail.
        SemanticRequirement* pkMatched = NULL;
        NIVERIFY(kInfo.kModifierReqToProfileReq.GetAt(pkReq, pkMatched));

       if (pkMatched->m_uiStreamID != uiProfileStreamID)
           uiStrictsNotMatched++;
       else
           uiStrictsMatched++;
    }

    if (uiStrictsMatched > 0 && uiStrictsNotMatched > 0)
    {
        if (pkLog)
        {
            pkLog->ReportError(NiMPPMessages::
                ERROR_MERGE_INTERLEAVE_CONFLICT_2ARGS,
                (const char*)pkProfileStream->GetAt(0)->m_kSemantic,
                pkProfileStream->GetAt(0)->m_uiIndex);
        }
        return false;
    }

    uiStrictsMatched = 0;
    uiStrictsNotMatched = 0;
    for(NiUInt32 ui=0; ui < pkProfileStream->GetSize(); ui++)
    {
        SemanticRequirement* pkReq = pkProfileStream->GetAt(ui);
        NIASSERT(pkReq);

        if (pkReq->m_eStreamBehavior !=
            NiMeshRequirements::STRICT_INTERLEAVE)
        {
            continue;
        }

       SemanticRequirement* pkStrictMatch;
       if (kInfo.kProfileReqToModifierReq.GetAt(pkReq, pkStrictMatch) == false)
       {
           // This indicates that the semantic doesn't exist at all for
           // the modifiers. 
           uiStrictsNotMatched++;
           continue;
       }

       if (pkStrictMatch->m_uiStreamID != uiModifierStreamID)
           uiStrictsNotMatched++;
       else
           uiStrictsMatched++;
    }
    
    if (uiStrictsMatched > 0 && uiStrictsNotMatched > 0)
    {
        if (pkLog)
        {
            pkLog->ReportError(NiMPPMessages::
                ERROR_MERGE_INTERLEAVE_CONFLICT_2ARGS,
                (const char*)pkProfileStream->GetAt(0)->m_kSemantic,
                pkProfileStream->GetAt(0)->m_uiIndex);
        }
        return false;
    }

    // If there are any matching stricts in the Profile, then all the
    // semantics must exist in Modifier.

    bool bStreamIDUnassigned = true;
    NiUInt32 uiNewStreamID = (NiUInt32)-1;

    StreamSet kNonMatching;
    for(NiUInt32 ui=0; ui < pkProfileStream->GetSize(); ui++)
    {
        SemanticRequirement* pkReq = pkProfileStream->GetAt(ui);
        NIASSERT(pkReq);

       SemanticRequirement* pkMatch = NULL;

       if ((kInfo.kProfileReqToModifierReq.GetAt(pkReq, pkMatch) == false)
           || (pkMatch->m_eStreamBehavior == NiMeshRequirements::FLOATER) ||
            (pkMatch->m_uiStreamID != uiModifierStreamID))
       {
           if (pkMatch && pkMatch->m_eStreamBehavior == 
               NiMeshRequirements::FLOATER)
           {
               // The floater is a special case where exact matching is not
               // necessary.
               kInfo.kProfileReqToModifierReq.SetAt(pkReq, NULL);

               SemanticRequirement* pkNewReq = 
                   MergeSemantics(pkReq, pkMatch, pkReq->m_uiStreamID, pkLog);

               if (pkNewReq == NULL)
                   return false;

               kNonMatching.Add(pkNewReq);
           }
           else
           {
               // The semantic was not matched, so forward it
               kNonMatching.Add(pkReq);
           }
       }
       else
       {
           if (bStreamIDUnassigned)
           {
                if (uiStrictsMatched == 0) // reserve original IDs for stricts.
                    uiNewStreamID = kInfo.uiNextStreamID++;
                else
                    uiNewStreamID = uiProfileStreamID;
                bStreamIDUnassigned = false;
           }

           // Found a match... Merge the two...
           SemanticRequirement* pkNewReq = 
               MergeSemantics(pkReq, pkMatch, uiNewStreamID, pkLog);

           if (pkNewReq == NULL)
               return false;

           kInfo.kMergedSet.Add(pkNewReq);
       }
    }

    if (kNonMatching.GetSize() == 0)
        return true;

    return MergeStream(&kNonMatching, kInfo, pkLog);
}
//---------------------------------------------------------------------------
RequirementSet* MergeRequirementSetToProfile(
    RequirementSet* pkProfileSet,
    RequirementSet* pkModifierSet,
    NiMeshErrorInterface* pkLog)
{
    MergeInfo kInfo;
    if (kInfo.Init(pkProfileSet, pkModifierSet, pkLog) == false)
        return NULL;

    // All the setup is now complete. The first step is to iterate over
    // each stream found in the kProfileStreamMap.
    NiTMapIterator kIter = kInfo.kProfileIDToStream.GetFirstPos();

    while(kIter)
    {
        NiUInt32 uiProfileStreamID;
        StreamSet* pkProfileStream;
        kInfo.kProfileIDToStream.GetNext(kIter, uiProfileStreamID,
            pkProfileStream);

        if (MergeStream(pkProfileStream, kInfo, pkLog) == false)
        {
            return false;
        }
    }

    RequirementSet* pkReq = NiNew NiMeshRequirements::
        RequirementSet();

    for(NiUInt32 ui=0; ui < kInfo.kMergedSet.GetSize(); ui++)
        pkReq->Add(kInfo.kMergedSet.GetAt(ui));

    return pkReq;
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------
bool NiMergeRequirements::MergeRequirementsIntoProfile(
    NiMeshRequirements& kProfile, NiMeshRequirements& kModifier,
    NiMeshRequirements& kMerged, NiMeshErrorInterface* pkLog)
{
    kMerged.ReleaseAllSemanticRequirements();
    NiTPrimitiveSet<RequirementSet*> kMergedSets;

    if (kModifier.GetSetCount() == 0)
    {
        if (kProfile.GetSetCount() == 0)
        {
            return true;
        }

        // Trivial to meet these requirements...
        for (NiUInt32 ui = 0; ui < kProfile.GetSetCount(); ui++)
        {
            RequirementSet* pkSet = kProfile.GetSetAt(ui);
            RequirementSet* pkNewSet = NiNew RequirementSet();
            for(NiUInt32 uiJ=0; uiJ < pkSet->GetSize(); uiJ++)
                pkNewSet->Add(pkSet->GetAt(uiJ));
            kMergedSets.Add(pkNewSet);
        }
    }
    else
    {
        if (kProfile.GetSetCount() == 0)
        {
            // Trivial to meet these requirements take from the modifier.
            for (NiUInt32 ui = 0; ui < kModifier.GetSetCount(); ui++)
            {
                RequirementSet* pkSet = kModifier.GetSetAt(ui);
                RequirementSet* pkNewSet = NiNew RequirementSet();
                for(NiUInt32 uiJ=0; uiJ < pkSet->GetSize(); uiJ++)
                    pkNewSet->Add(pkSet->GetAt(uiJ));
                kMergedSets.Add(pkNewSet);
            }
        }
        else
        {
            // Work through all pairs of sets
            for (NiUInt32 ui = 0; ui < kProfile.GetSetCount(); ui++)
            {
                for (NiUInt32 uj = 0; uj < kModifier.GetSetCount();
                    uj++)
                {
                    if (pkLog)
                    {
                        pkLog->ReportAnnotation("Merging: { %s set:%d } and " \
                            "{ %s set:%d }\n", 
                            (const char*)kProfile.m_kReqName,ui,
                            (const char*)kModifier.m_kReqName, uj);
                    }

                    RequirementSet* pkNewSet = MergeRequirementSetToProfile(
                        kProfile.GetSetAt(ui), kModifier.GetSetAt(uj), pkLog);

                    if (pkNewSet)
                        kMergedSets.Add(pkNewSet);
                }
            }
        }
    }

    if (kMergedSets.GetSize() == 0)
        return false;

    // Iterate over the sets and merge the access flags for any
    // interleaved streams
    for(NiUInt32 uiSet = 0; uiSet < kMergedSets.GetSize(); uiSet++)
    {
        RequirementSet* pkSet = kMergedSets.GetAt(uiSet);

        NiUInt32 uiStartReq = 0xFFFFFFFF;
        for (NiUInt32 uiReq = 0; uiReq < pkSet->GetSize(); ++uiReq)
        {
            SemanticRequirementPtr spReq = pkSet->GetAt(uiReq);
            if (!spReq)
                continue;
            if (uiStartReq == 0xFFFFFFFF)
                uiStartReq = uiReq;

            if ((uiStartReq != uiReq) &&
                (pkSet->GetAt(uiStartReq)->m_uiStreamID !=
                spReq->m_uiStreamID))
            {
                bool bMerged = false;
                NiUInt8 uiMergedAccessFlags = 0;
                for (NiUInt32 uiP1 = uiStartReq; uiP1 < uiReq; ++uiP1)
                {
                    if (pkSet->GetAt(uiP1)->m_eStreamBehavior
                        == NiMeshRequirements::STRICT_INTERLEAVE)
                    {
                        if (MergeAccessFlags(uiMergedAccessFlags,
                            pkSet->GetAt(uiP1)->m_uiAccessFlags,
                            uiMergedAccessFlags))
                        {
                            bMerged = true;
                        }
                        else
                        {
                            bMerged = false;
                            break;
                        }
                    }               
                }

                if (bMerged)
                {
                    for (NiUInt32 uiP1 = uiStartReq; uiP1 < uiReq; ++uiP1)
                    {
                        if (pkSet->GetAt(uiP1)->m_eStreamBehavior
                            == NiMeshRequirements::STRICT_INTERLEAVE)
                        {
                            pkSet->GetAt(uiP1)->m_uiAccessFlags = 
                                uiMergedAccessFlags;
                        }               
                    }
                }

                uiStartReq = uiReq;
            }
        }
    }

    for (NiUInt32 ui = 0; ui < kMergedSets.GetSize(); ui++)
    {
        kMerged.AddRequirementSet(kMergedSets.GetAt(ui));
    }

    return true;
}
//---------------------------------------------------------------------------
