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

#ifndef NIMERGEREQUIREMENTS_H
#define NIMERGEREQUIREMENTS_H

#include <NiMeshRequirements.h>

/// @cond EMERGENT_INTERNAL
/**
    Provides a static merge method for merging a mesh modifier requirement set
    into a profiles requirement set.
*/
/// @endcond
class NIMESHPROFILEPROCESSOR_ENTRY NiMergeRequirements
{
public:
    /**
        Merges one group of requirements into a Profile group. Each of the
        n sets of requirements in the "kModifier" object are merged with the m
        sets in the "kProfile" argument to form a potential list of n x m total
        sets of requirements. Any sets that cannot be merged are left out of
        the final merged requirements.
        
        Sets are examined pairwise, one from the kProfile object and one from
        the kModifier object. Semantic elements that appear in the kModifier
        set must exist in the kProfile set. In other words, no "new" semantics
        can be added to the Profile as it represents the total set of available
        semantics.

        Semantics elements must be reconcilable which includes: ensuring that
        the interleave restrictions are honored, ensuring that their format
        sets have a non-empty intersection, that indexes are the same,
        access flags must be mutually acceptable, etc. If the common elements
        cannot be reconciled, then this pair of sets is not moved into the
        output.

        @param kProfile One set to merge.
        @param kModifier The other set to merge.
        @param kMerged The result of the merge. Requirements are
            added to this set - any existing requirements are not removed.
        @param pkLog Pointer to the log.
        
        @return False if the merge resulted in no satisfiable requirements,
            otherwise true.
    */
    static bool MergeRequirementsIntoProfile(NiMeshRequirements& kProfile,
        NiMeshRequirements& kModifier, NiMeshRequirements& kMerged,
        NiMeshErrorInterface* pkLog = NULL);
};

#endif // #ifndef NIMERGEREQUIREMENTS_H
