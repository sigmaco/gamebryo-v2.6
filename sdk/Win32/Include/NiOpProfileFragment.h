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

#ifndef NIOPPROFILEFRAGMENT_H
#define NIOPPROFILEFRAGMENT_H

#include <NiToolPipelineCloneHelper.h>
#include <NiMeshRequirements.h>

#include "NiMeshProfileProcessorLibType.h"
#include "NiObject.h"
#include "NiTArray.h"
#include "NiOpTerminator.h"

/// @cond EMERGENT_INTERNAL
/**
    Defines a profile fragment that applies to set of of shared data-streams.
*/
/// @endcond
class NIMESHPROFILEPROCESSOR_ENTRY NiOpProfileFragment : public NiObject
{
    NiDeclareClone(NiOpProfileFragment);

public:
    /// Default Construction
    NiOpProfileFragment();

    /// Default Destructor
    virtual ~NiOpProfileFragment();

    /// Retrieve the requirements defined by this profile fragment
    void GetRequirements(NiMeshRequirements& kProfileRequirements);

    /// Adjust profile fragment by the requirements passed in
    void AdjustFinalizeOps(NiMeshRequirements& kRequirements);

    /// Interleaves streams within the clone-set.
    bool Interleave(NiToolPipelineCloneHelper::CloneSetPtr&
        spCloneSet, NiMeshErrorInterface* pkReportedErrors) const;

    /// Specifies that resolve errors occurred. 
    void SetHadResolveErrors(bool bHadResolveErrors);

    /// Used to determine if resolve errors occurred. Used to skip
    /// interleaving.
    bool GetHadResolveErrors();

    /// Determines if two profiles are equal
    virtual bool IsEqual(NiObject* pkObject);

    /// The terminator operations that are associated with this fragment.
    NiTObjectArray <NiOpTerminatorPtr> m_kTerminatorNodes;

    /// Static pointer to the system clonehelper
    static NiToolPipelineCloneHelper* ms_pkCloneHelper;

protected:
    bool m_bHadResolveErrors;

};

NiSmartPointer(NiOpProfileFragment);

#endif  // #ifndef NIOPPROFILEFRAGMENT_H
