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

#ifndef NIPHYSXPOSEBUFFERDEST_H
#define NIPHYSXPOSEBUFFERDEST_H

#include "NiPhysXLibType.h"

#include "NiPhysXRigidBodyDest.h"
#include "NiPhysXSequenceManager.h"

/**
    Each NiPhysXPoseBufferDest connects a PhysX rigid body actor to 
    one rotation and one translation data item within the NiPoseBuffer 
    owned by the active animation sequence. The location of this data
    matches the output of a given sequence evaluator. These data 
    values form a transform which, after blending with pose buffer 
    data from other active sequences, ultimately drive a scene graph 
    transform node.
    
    Most of its functionality is derived from the base classes. This class 
    only adds the final step of interpolating the PhysX results and pushing 
    them into the pose buffer.

    This class should be used in conjunction with a NiPhysXSequenceManager
    object, in which case applications will never need to create one and will
    never need to explicitly call any of its functions. The code that adds a
    sequence to the NiPhysXSequenceManager creates these objects as part of
    its setup and continues to manage them.

    @note Fast Spinning Objects: Interpolation will fail for fast rotating
        objects due to the fundamental limitations of rotation representations
        and discrete time sampling. Disable interpolation for fast moving
        objects. 
*/
class NIPHYSX_ENTRY NiPhysXPoseBufferDest : public NiPhysXRigidBodyDest
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareClone(NiPhysXPoseBufferDest);
    /// @endcond

public:

    /**
        The constructor stores the evaluator index, the scene graph parent 
        of the node that evaluator is controlling, the source sequence data,
        and the rotation and translation data locations within the pose buffer.
        
        The other arguments are passed to the base class constructor. 
    */
    NiPhysXPoseBufferDest(NxActor *pkActor, NxActor* pkActorParent, 
        NiNode* pkSceneParent, NiPhysXSequenceManager::SequenceData* pkSeqData,
        NiUInt32 uiEvalIndex, NiPoseBufferHandle kPBHandleTranslate, 
        NiPoseBufferHandle kPBHandleRotate);

    /// Destructor sets all held pointers to NULL.
    ~NiPhysXPoseBufferDest();

    /// Returns the associated sequence data.
    NiPhysXSequenceManager::SequenceData* GetSequenceData() const;

    /// Returns the evaluator index within the source sequence data.
    NiUInt32 GetEvaluatorIndex() const;

    /// Sets the activity flag, disables the associated evaluator channels, 
    /// and validates the associated pose buffer locations.
    virtual void SetActive(const bool bActive);

    /**
        Push the transform value into the pose buffer.

        If the interpolation flag (defined in NiPhysXDest) is false, 
        this pushes the most recently extracted transformation into 
        the target pose buffer locations. See the base class 
        documentation for how this transformation is computed. 
        If the interpolation flag is true, the time fT is used 
        to interpolate or extrapolate from the two most recently 
        extracted transforms (the work is done by the base class).
    */
    virtual void UpdateSceneGraph(const float fT, 
        const NiTransform& kRootTransform, const float fScalePToW,
        const bool bForce);

    /// @cond EMERGENT_INTERNAL

    /// Updates internal pointers to the rotation and translation data items 
    /// within the specified pose buffer.
    void UpdatePoseBufferDataPointers(const NiPoseBuffer* pkPoseBuffer);

    virtual bool StreamCanSkip();

    /// @endcond
    
protected:
    NiPhysXPoseBufferDest();

    /// Accessor used by base classes.
    virtual NiNode* GetSceneGraphParent() const;
    
    NiNode* m_pkSceneParent;
    NiPhysXSequenceManager::SequenceData* m_pkSeqData;
    NiUInt32 m_uiEvalIndex;
    NiPoseBufferHandle m_kPBHandleTranslate;
    NiPoseBufferHandle m_kPBHandleRotate;
    NiPoint3* m_pkPBTranslate;
    NiQuaternion* m_pkPBRotate;
    float m_fLastUpdateTime;
};

NiSmartPointer(NiPhysXPoseBufferDest);

#include "NiPhysXPoseBufferDest.inl"

#endif  // #ifndef NIPHYSXPOSEBUFFERDEST_H
