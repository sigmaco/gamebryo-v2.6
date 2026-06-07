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

#ifndef NIPHYSXACCUMINTERPOLATORDEST_H
#define NIPHYSXACCUMINTERPOLATORDEST_H

#include "NiPhysXLibType.h"

#include <NiTransformInterpolator.h>

#include "NiPhysXAccumRigidBodyDest.h"

class NiControllerManager;

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    the NiPhysXAccumPoseBufferDest class.
*/

class NIPHYSX_ENTRY NiPhysXAccumInterpolatorDest :
    public NiPhysXAccumRigidBodyDest
{
    /// @cond EMERGENT_INTERNAL

    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareClone(NiPhysXAccumInterpolatorDest);

    /// @endcond

public:
    /**
        The constructor stores its target interpolators, pkAccumTarget and
        pkNonAccumTarget; the character being animated, pkControllerManager;
        and the scene graph parent of the accumulation node that interpolator 
        is controlling, pkSceneParent.
        
        The other arguments are passed to the base class constructor.
    */
    NiPhysXAccumInterpolatorDest(NiInterpolator* pkAccumTarget,
        NiInterpolator* pkNonAccumTarget,
        NiControllerManager* pkControllerManager,
        NiNode* pkSceneParent, NxActor *pkActor, NxActor* pkActorParent);
    
    /// Destructor.
    ~NiPhysXAccumInterpolatorDest();

    /**
        @name Access functions for animation system objects.
        
        In order to completely transfer state, this class must hold the
        interpolator associated with the accumulation node as well as 
        the interpolator associated with the non-accum node. See the 
        documentation on animation accumulation for a discussion of 
        these nodes.
        
        The interpolators must be of type NiTransformInterpolator or
        NiBSplineTransformInterpolator or a derived class.
        
        The scene graph parent of the node the target is controlling must
        also be provided to enable the computation of local transformations.
        That is, the parent node for the accumulation root node.
    */
    /// @{

    /// Get the current interpolator for the accum node.
    NiInterpolator* GetAccumTarget() const;
    
    /// Set the interpolator for the accumulation node and the 
    /// scene graph node that is the accumulation node's parent.
    void SetAccumTarget(NiInterpolator* pkTarget, NiNode* pkParent);
    
    /// Get the non-accumulated target interpolator.
    NiInterpolator* GetNonAccumTarget() const;

    /// Set the non-accumulated target interpolator.
    void SetNonAccumTarget(NiInterpolator* pkTarget);

    /// Get the associated controller manager (i.e. the character driven 
    /// by the physics sequence).
    NiControllerManager* GetControllerManager() const;

    /// Set the associated controller manager (i.e. the character driven 
    /// by the physics sequence).
    void SetControllerManager(NiControllerManager* pkControllerManager);

    /// Get the associated active animation sequence. NULL will be returned 
    /// when the destination is not active.
    virtual NiControllerSequence* GetActiveAnimSequence() const;

    /// Set the associated active animation sequence.
    virtual void SetActiveAnimSequence(NiControllerSequence* pkActiveAnimSeq);

    /// @}

    /**
        Update function that applies transforms to the interpolators.

        If the interpolation flag (defined in NiPhysXDest) is false, this
        function pushes the most recently extracted transformations into
        the target interpolators. See the base class documentation for how
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
    virtual void PostLinkObject(NiStream& kStream);

    /// @endcond

protected:
    /// Constructor for use in streaming and cloning.
    NiPhysXAccumInterpolatorDest();
    
    /// Accessor used by base classes.
    virtual NiNode* GetSceneGraphParent() const;
    
    /// Animation system objects
    NiInterpolator* m_pkAccumTarget;
    NiInterpolator* m_pkNonAccumTarget;
    NiControllerManager* m_pkControllerManager;
    NiNode* m_pkSceneParent;
    NiControllerSequence* m_pkActiveAnimSeq;

    /// The converter class needs access to change the scene parent node.
    friend class NiPhysXConverter;
};

NiSmartPointer(NiPhysXAccumInterpolatorDest);

#include "NiPhysXAccumInterpolatorDest.inl"

/// @endcond

#endif  // #ifndef NIPHYSXACCUMINTERPOLATORDEST_H
