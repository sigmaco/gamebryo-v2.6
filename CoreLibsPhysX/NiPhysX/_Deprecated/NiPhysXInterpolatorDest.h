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

#ifndef NIPHYSXINTERPOLATORDEST_H
#define NIPHYSXINTERPOLATORDEST_H

#include "NiPhysXLibType.h"

#include <NiTransformInterpolator.h>

#include "NiPhysXRigidBodyDest.h"

/// @cond DEPRECATED_CLASS

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    the NiPhysXPoseBufferDest class.
*/

class NIPHYSX_ENTRY NiPhysXInterpolatorDest : public NiPhysXRigidBodyDest
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareClone(NiPhysXInterpolatorDest);
    /// @endcond

public:

    /**
        The constructor stores its target interpolator, pkTarget, and the
        scene graph parent of the node that interpolator is controlling,
        pkActorParent.
        
        The other arguments are passed to the base class constructor. 
    */
    NiPhysXInterpolatorDest(NiInterpolator* pkTarget,
        NiNode* pkSceneParent, NxActor *pkActor, NxActor* pkActorParent);

    /// Destructor does nothing.
    ~NiPhysXInterpolatorDest();

    /// Get the target interpolator.
    NiInterpolator* GetTarget() const;
        
    /**
        Set the target interpolator.

        The argument must be of type NiTransformInterpolator or
        NiBSplineTransformInterpolator or a derived class. The scene graph
        parent of the node the target is controlling must also be provided
        to enable the computation of local transformations.
    */
    void SetTarget(NiInterpolator* pkTarget, NiNode* pkParent);
    
    /// Get the associated active animation sequence. NULL will be returned 
    /// when the destination is not active.
    virtual NiControllerSequence* GetActiveAnimSequence() const;

    /// Set the associated active animation sequence.
    virtual void SetActiveAnimSequence(NiControllerSequence* pkActiveAnimSeq);

    /**
        Push the state into the Gamebryo scene graph objects.

        If bInterp is false, this pushes the most recently extracted
        transformation into the target interpolator. See the base class
        documentation for how this transformation is computed. If the
        interpolation flag (defined in NiPhysXDest) is true, the time fT
        is used to interpolate or extrapolate from the two most recently
        extracted transforms (the work is done by the base class).
    */
    virtual void UpdateSceneGraph(const float fT,
        const NiTransform& kRootTransform, const float fScalePToW,
        const bool bForce);

    /// @cond EMERGENT_INTERNAL

    virtual bool StreamCanSkip();

    virtual void ProcessClone(NiCloningProcess& kCloning);
    
    /// @endcond
    
protected:
    NiPhysXInterpolatorDest();
    
    virtual NiNode* GetSceneGraphParent() const;
    
    NiInterpolator* m_pkTarget;
    NiNode* m_pkSceneParent;
    NiControllerSequence* m_pkActiveAnimSeq;

    friend class NiPhysXConverter;
};

NiSmartPointer(NiPhysXInterpolatorDest);

#include "NiPhysXInterpolatorDest.inl"

/// @endcond

#endif  // #ifndef NIPHYSXINTERPOLATORDEST_H
