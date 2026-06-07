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

#ifndef NIPHYSXSRC_H
#define NIPHYSXSRC_H

#include "NiPhysXLibType.h"

#include <NiObject.h>
#include <NiTransform.h>

/**
    A source in the Gamebryo-PhysX package is a link between a Gamebryo node
    that is being driven by animation and a PhysX actor that is intended to
    in some way mirror the behaviors in the physics simulation.
    
    NiPhysXSrc is an abstract base class from which all such objects are
    derived.
    
    In addition to decalring virtual update functions, the class stores a flag
    indicating activity and another controlling interpolation of state.
    Derived classes should only transfer state if the source is active (unless
    forced), and should implement interpolation in an appropriate form.
*/
class NIPHYSX_ENTRY NiPhysXSrc : public NiObject
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareAbstractClone(NiPhysXSrc);
    /// @endcond

public:
    /// Constructor.
    ///
    /// Sets the source as active and interpolation off.
    NiPhysXSrc();

    /// Destructor does nothing.
    ~NiPhysXSrc();

    /// Return the activity flag.
    bool GetActive() const;

    /// Set the activity flag.
    void SetActive(const bool bActive);
   
    /// Get the interpolation flag.
    bool GetInterpolate() const;

    /// Set the interpolation flag.
    void SetInterpolate(const bool bActive);
    
    /**
        @name Update Functions

        These functions work together to transfer state from a scene graph
        node to a PhysX actor.
        If this instance is not active, then they do nothing unless forced.
        They are split into two functions because the second, UpdateToActors
        requires access to the PhysX SDK lock, while the first does not.
        Both might touch the scene graph.
    */

    /// @{

    /**
        Perform any update work to extract state from the Gamebryo scene graph.
        
        The function is typically called via NiPhysXScene::UpdateSources.
        It must not do work that requires access to the PhysX SDK lock.
        
        @param fT The application time, which can be used to tag the extracted
            state.
        @param kInvRootTransform The inverse of global transformation applied
            to the PhysX scene. This transform converts Gamebryo world
            coordinates to PhysX global coordinates.
        @param fScaleWToP All positions and velocities are scaled by
            fScalePToW after they are transformed by kRootTransform.
        @param bForce If true, derived classes should do the update regardless
            of internal state. 
    */
    virtual void UpdateFromSceneGraph(const float fT,
        const NiTransform& kInvRootTransform, const float fScaleWToP,
        const bool bForce = false) = 0;

    /**
        This function is called on all sources immediately before a
        simulation step is started.
        
        It must push any state into the PhysX actors that are being
        controlled by the source. A lock is held on the PhysX SDK by
        the NiPhysXScene that calls this function, so derived classes
        should not attempt to gain the lock in this function. 
        
        @param fTBegin The application time at the beginning of the
            simulation step.
        @param fTEnd The time that PhysX is stepping to.
    */
    virtual void UpdateToActors(const float fTBegin, const float fTEnd) = 0;

    /// @}

protected:
    bool m_bActive;
    bool m_bInterp;
};

NiSmartPointer(NiPhysXSrc);

#include "NiPhysXSrc.inl"

#endif  // #ifndef NIPHYSXSRC_H
