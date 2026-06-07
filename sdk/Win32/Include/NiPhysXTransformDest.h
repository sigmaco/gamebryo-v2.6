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

#ifndef NIPHYSXTRANSFORMDEST_H
#define NIPHYSXTRANSFORMDEST_H

#include "NiPhysXLibType.h"

#include "NiPhysXRigidBodyDest.h"

/**
    Each NiPhysXTransformDest object connects a PhysX rigid body actor to a
    Gamebryo scene graph node.
    
    Most of its functionality is derived from the base classes. This class only
    adds the final step of interpolating the PhysX results and pushing them to
    the scene graph. 

    @note Interpolation will fail for fast rotating objects due to the
        fundamental limitations of rotation representations and discrete time
        sampling. Disable interpolation for fast moving objects. 
*/
class NIPHYSX_ENTRY NiPhysXTransformDest : public NiPhysXRigidBodyDest
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareClone(NiPhysXTransformDest);
    /// @endcond

public:
    /// The constructor stores its target node, pkTarget, and the other
    /// arguments are passed to the base class constructor. 
    NiPhysXTransformDest(NiAVObject* pkTarget, NxActor *pkActor,
        NxActor* pkActorParent);

    /// Destructor does nothing.
    ~NiPhysXTransformDest();

    /// Return the scene graph node that is receiving the actor's
    /// transformation.
    NiAVObject* GetTarget() const;

    /// Set the scene graph node that is receiving the actor's transformation.
    void SetTarget(NiAVObject* pkTarget);
    
    /**
        The update function implemented from NiPhysXDest.
        
        If bInterp is false, this pushes the most recently extracted
        transformation into the target node. See the base class documentation
        for how this transformation is computed. If the interpolation flag
        (defined in NiPhysXDest) is true, the time fT is used to interpolate
        or extrapolate from the two most recently extracted transforms (the
        work is done by the base class).
        
        See the documentation for NiPhysXDest for details of the other
        arguments.
    */
    virtual void UpdateSceneGraph(const float fT,
        const NiTransform& kRootTransform, const float fScalePToW,
        const bool bForce);
            
    // *** begin Emergent internal use only ***

    virtual bool StreamCanSkip();

    virtual void ProcessClone(NiCloningProcess& kCloning);
    
    // *** end Emergent internal use only ***

protected:
    NiPhysXTransformDest();
    
    virtual NiNode* GetSceneGraphParent() const;

    NiAVObject* m_pkTarget;
};

NiSmartPointer(NiPhysXTransformDest);

#include "NiPhysXTransformDest.inl"

#endif  // #ifndef NIPHYSXTRANSFORMDEST_H
