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

#ifndef NIOPPROFILE_H
#define NIOPPROFILE_H

#include <NiToolPipelineCloneHelper.h>
#include "NiMeshProfileProcessorLibType.h"
#include "NiObject.h"
#include "NiTArray.h"
#include "NiString.h"
#include "NiOpStart.h"
#include "NiOpTerminator.h"

class NiMesh;

#define INVALID_ACCESS_FLAG 0xff

/// @cond EMERGENT_INTERNAL
/**
    Defines the operations and interleaving profile that will be performed
    over a mesh.
*/
/// @endcond
class NIMESHPROFILEPROCESSOR_ENTRY NiOpProfile : public NiObject
{
    NiDeclareClone(NiOpProfile);

public:
    // Construction / Destruction
    NiOpProfile();
    virtual ~NiOpProfile();

    /// Performs some degree of validation over the profile.
    bool ValidateProfile();

    /// Retrieves the name of the profile
    inline const NiFixedString& GetName() const;

    /// Sets the name of the profile
    inline void SetName(const char* pkName);

    /// Retrieves the profile description
    inline const char* GetDescription() const;

    /// Sets the profile description
    inline void SetDescription(const char* pkDescription);

    /// Retrieves the keep unspecified streams setting for the Finalize stage.
    inline bool GetKeepUnspecifiedFinalizeStreams() const;

    /// Sets the keep unspecified streams setting for the Finalize stage.
    inline void SetKeepUnspecifiedFinalizeStreams(bool bValue);

    /// Retrieves the renderer for which this profile applies.
    inline NiSystemDesc::RendererID GetRenderer() const;

    /// Sets the renderer for which this profile applies.
    inline void SetRenderer(NiSystemDesc::RendererID eRenderer);

    /// Retrieves the Finalize stages start operation nodes.
    NiTObjectArray <NiOpStartPtr>& GetFinalizeStartNodes();

    /// Retrieves the Finalize stages terminator operation nodes.
    NiTObjectArray <NiOpTerminatorPtr>& GetFinalizeTerminatorNodes();

    /// Determines if two profiles are equal
    virtual bool IsEqual(NiObject* pkObject);

    /// Post resolve operations.  These are operations that need to occur
    /// after the streams have been resolved.
    enum PostResolveOp
    {
        PRO_COMPUTE_BOUNDS,
        PRO_RECOMPUTE_NORMALS,
        PRO_COUNT
    };

    inline void SetPostResolveOp(const PostResolveOp eOp, const bool bData);

    void PostResolveFinalizeOps(NiMesh* pkMesh) const;

protected:
    inline NiOpTerminator* FindFinalizeTerminatorNode(const NiFixedString& kSemantic,
        const NiUInt8 uiSemanticIndex) const;

    bool Validate(NiTObjectArray <NiOpTerminatorPtr>& kTermNodes);

    // Member data
    bool m_bKeepUnspecifiedFinalizeStreams;

    NiFixedString m_strName;
    NiString m_strDescription;
    NiSystemDesc::RendererID m_eRenderer;
    
    NiTObjectArray <NiOpStartPtr> m_kStartFinalizeNodes;
    NiTObjectArray <NiOpTerminatorPtr> m_kTerminatorFinalizeNodes;

    bool m_abPostResolveOps[PRO_COUNT];
};

NiSmartPointer(NiOpProfile);

#include "NiOpProfile.inl"

#endif  // #ifndef NIOPPROFILE_H
