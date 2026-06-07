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

#ifndef NIPHYSXDEST_H
#define NIPHYSXDEST_H

#include "NiPhysXLibType.h"

#include <NiObject.h>
#include <NiAVObject.h>

class NiControllerSequence;
class NiPhysXScene;

/**
    A destination in the Gamebryo-PhysX package is a link between a
    PhysX actor that is producing state, and a Gamebryo scene graph object
    that is being driven by the physics.
    
    NiPhysXDest is an abstract base class from which all such objects are
    derived.
*/
class NIPHYSX_ENTRY NiPhysXDest : public NiObject
{
    /// @cond EMERGENT_INTERNAL
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareAbstractClone(NiPhysXDest);
    /// @endcond

public:
    /**
        Constructor.
        
        This class contains pure virtual functions and cannot be instantiated.
        For reference purposes, the constructor sets the interpolation flag
        to false and the activity flag to true.
    */
    NiPhysXDest();

    /// Destructor does nothing.
    ~NiPhysXDest();

    /**
        Get the activity flag.

        State is only extracted from PhysX and passed to Gamebryo if both this
        flag is active and updating of destinations is active for the
        NiPhysXScene that this destination is controlled by. 
    */
    bool GetActive() const;

    /// Set the activity flag (default true).
    virtual void SetActive(const bool bActive);

    /**
        Get the interpolation flag.

        This flag control whether or not this destination interpolates its
        state when the timing of a Gamebryo and PhysX scene are mismatched,
        typically due to using fixed timesteps for the physics. Derived
        classes must interpret this flag in an appropriate way. 
    */
    bool GetInterpolate() const;

    /// Set the interpolation flag (default false).
    void SetInterpolate(const bool bActive);

    /**
        @name Update Functions

        These functions work together to update destinations from PhysX actors.
        If this instance is not active, then they do nothing unless forced.
        They are split into two functions because the first, UpdateFromActors
        requires access to the PhysX SDK lock, while the second does not.
        Both might touch the scene graph.
    */

    /// @{

    /**
        Perform any update work that requires access to the PhysX scene and
        which needs to know the current PhysX simulation time.
        
        This function is called by NiPhysXScene::FetchResults when the
        simulation step completes, or by NiPhysXScene::UpdateDestinations if
        that is called with the bForce argument true. It presumes the scene
        is locked when called. 
        
        @param fT The simulation time - the time that NiPhysXScene was most
            recently stepped to.
        @param kRootTransform The global transformation applied to the PhysX
            scene.
        @param fScalePToW All positions and velocities are scaled by
            fScalePToW before they are transformed by kRootTransform.
        @param bForce If true, derived classes should do the update regardless
            of internal state. 
    */
    virtual void UpdateFromActors(const float fT, 
        const NiTransform& kRootTransform, const float fScalePToW,
        const bool bForce) = 0;

    /**
        Perform the remainder of the update step.
        
        This function is called by NiPhysXScene::UpdateDestinations and should
        push any data into the scene graph it is controlling.
        
        @param fT The time is that desired by the application (typically the
            update time, but maybe different) in accumulated application time.
            It need not be the same time as used for UpdateFromActors.
        @param kRootTransform The global transformation applied to the PhysX
            scene.
        @param fScalePToW All positions and velocities are scaled by
            fScalePToW before they are transformed by kRootTransform.
        @param bForce If true, derived classes should do the update regardless
            of internal state. 
    */
    virtual void UpdateSceneGraph(const float fT,
        const NiTransform& kRootTransform, const float fScalePToW,
        const bool bForce) = 0;
    
    /// @}

protected:
    bool m_bActive;
    bool m_bInterp;
};

NiSmartPointer(NiPhysXDest);

#include "NiPhysXDest.inl"

#endif  // #ifndef NIPHYSXDEST_H
