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

#ifndef NIPHYSXACCUMTRANSFORMDEST_H
#define NIPHYSXACCUMTRANSFORMDEST_H

#include "NiPhysXLibType.h"

#include "NiPhysXAccumRigidBodyDest.h"

/**
    Each NiPhysXAccumTransformDest connects a PhysX rigid body actor to the
    transformation of two Gamebryo scene graph nodes that are implementing
    animation accumulation.
    
    One target node is the accumulation root of the character,
    and the other is its non-accumulated child. Most of its functionality is
    derived from the base classes. This class only adds the final step of
    pushing the transformation state to the scene graph.

    This node is for use in controlling rag-dolls or other characters that
    have Gamebryo Animation Sequences defined. You will need this node if you
    have a physics actor controlling the "Character Root" or
    "Accumulation Root" of a character and you are NOT using an
    NiPhysXSequenceManager object to control animations. When the model is
    exported from the art tool, there will be a regular NiPhysXTransformDest
    object associated with the character root or accumulation root. The root
    node should have a child, with "NonAccum" in its name. The existing
    NiPhysXTransformDest object should be replaced by an
    NiPhysXAccumTransformDest object. Applications will have to do this in
    code.

    @note Interpolation will fail for fast rotating objects due to the
        fundamental limitations of rotation representations and discrete
        time sampling. Disable interpolation for fast moving objects.
*/

class NIPHYSX_ENTRY NiPhysXAccumTransformDest :
    public NiPhysXAccumRigidBodyDest
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareClone(NiPhysXAccumTransformDest);

    /// @endcond

public:
    /// The constructor stores its target nodes, pkAccumTarget and
    /// pkNonAccumTarget, and the other arguments are passed to the
    /// base class constructor.
    NiPhysXAccumTransformDest(NiAVObject* pkAccumTarget,
        NiAVObject* pkNonAccumTarget,
        NxActor *pkActor, NxActor* pkActorParent);
    
    
    /// Destructor
    ~NiPhysXAccumTransformDest();

    /**
        @name Access to scene graph objects
        
        These functions are accessors for the accumulated target and
        non-accumulated target. Note that strange things might happen with
        interpolated values, because the base class stores the local
        Gamebryo coordinate transformation. If the relationship of the
        target to its parent changes, then there may be a jump in the
        motion. To avoid this, call NiPhysXRigidDest::UpdateFromActors on
        this instance twice with bForce true. This overwrites the
        transformation values with new, correct, ones. 
    */
    /// @{
    
    /// Get the accumulation node target for this destination.
    NiAVObject* GetAccumTarget() const;

    /// Set the accumulation node target for this destination.
    void SetAccumTarget(NiAVObject* pkTarget);

    /// Get the non-accumulation node target for this destination.
    ///
    /// This node will typically be the scene graph child of the accum node.
    NiAVObject* GetNonAccumTarget() const;

    /// Set the non-accumulation node target for this destination.
    void SetNonAccumTarget(NiAVObject* pkTarget);
    
    /// @}

    /**
        Update function that applies transforms to the nodes.

        If the interpolation flag (defined in NiPhysXDest) is false, this
        function pushes the most recently extracted transformations into
        the target nodes. See the base class documentation for how
        this transformation is computed. If the interpolation flag is true,
        the time fT is used to interpolate or extrapolate from the two most
        recently extracted PhysX actor transforms (the work is done by the
        base class). The bForce argument is ignored.
    */
    virtual void UpdateSceneGraph(const float fT,
        const NiTransform& kRootTransform, const float fScalePToW,
        const bool bForce);
    
    /// @cond EMERGENT_INTERNAL

    virtual bool StreamCanSkip();

    virtual void ProcessClone(NiCloningProcess& kCloning);
 
    /// @endcond

protected:
    /// Constructor for use in streaming and cloning.
    NiPhysXAccumTransformDest();
    
    /// Accessor used by base classes.
    ///
    /// Returns the parent of the accum node.
    virtual NiNode* GetSceneGraphParent() const;

    /// The scene graph targets of our transforms.
    NiAVObject* m_pkAccumTarget;    
    NiAVObject* m_pkNonAccumTarget;    
};

NiSmartPointer(NiPhysXAccumTransformDest);

#include "NiPhysXAccumTransformDest.inl"

#endif  // #ifndef NIPHYSXACCUMTRANSFORMDEST_H
