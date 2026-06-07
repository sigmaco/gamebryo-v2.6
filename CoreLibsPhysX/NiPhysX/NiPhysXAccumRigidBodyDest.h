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

#ifndef NIPHYSXACCUMRIGIDBODYDEST_H
#define NIPHYSXACCUMRIGIDBODYDEST_H

#include "NiPhysXLibType.h"
#include "NiPhysXRigidBodyDest.h"

#if defined(WIN32) || defined(_XENON)
#pragma warning(push)
#pragma warning(disable: 4100) // unreferenced formal parameter
#pragma warning(disable: 4512) // assignment operator could not be generated
#pragma warning(disable: 4244) // conversion from 'type' to 'type', possible loss of data
#pragma warning(disable: 4245) // conversion from 'type' to 'type', signed/unsigned mismatch
#elif defined(_PS3)
#pragma GCC system_header
#endif
#include <NxPhysics.h>
#if defined(WIN32) || defined(_XENON)
#pragma warning( pop )
#endif

/**
    A destination object for transferring state from a PhysX rigid body actor
    to Gamebryo nodes associated with animation accumulation.
    
    A destination in the Gamebryo-PhysX package is a link between a PhysX
    actor that is producing state, and a Gamebryo scene graph object that
    is being driven by the physics. NiPhysXAccumRigidBodyDest is an abstract
    base class for destinations that extract and set rigid body state
    (rotation and translation) on two nodes that are doing transform
    accumulation in the Gamebryo animation system.

    Beyond functionality of its base class, this class adds flags to store
    the accumulation settings and a function to take a total transformation
    and break it into accumulated and non-accumulated portions. The latter
    is a protected member function available to sub-classes.

    @note This class is abstract because it does not implement
        NiPhysXDest::UpdateSceneGraph nor
        NiPhysXRigidBodyDest::GetSceneGraphParent().
*/
class NIPHYSX_ENTRY NiPhysXAccumRigidBodyDest : public NiPhysXRigidBodyDest
{

    /// @cond EMERGENT_INTERNAL
    NiDeclareRTTI;
    NiDeclareStream;
    NiDeclareAbstractClone(NiPhysXAccumRigidBodyDest);
    /// @endcond
    
public:
    /// Constructor takes the actor that we are extractig state from
    /// and the actor that corresponds to the scene graph parent of the
    /// accumulation node we are controlling.
    NiPhysXAccumRigidBodyDest(NxActor* pkActor, NxActor* pkActorParent);
    
    /// Destructor.
    ~NiPhysXAccumRigidBodyDest();

    /**
        @name Accumulation control flags

        These functions allow you to get and set the flags that control
        how the accumulation is performed. Any flag that is true causes that
        portion of the transformation to be sent to the accumulation node,
        while false leaves the transformation component on the non-accumulated
        node. For rotation you must specify either none, one or all of the
        flags. The front facing flags inform the system of the front facing
        direction for the character. The ucAxis is 0 for X, 1 for Y and 2 for
        Z. However, only X and Y are respected by the system.
    */
    /// @{
    
    /// Set flags controlling the axes along which to accumulate translation.
    void SetAccumTranslation(
        const bool bAccumX, const bool bAccumY, const bool bAccumZ);

    /// Get flags controlling the axes along which to accumulate translation.
    void GetAccumTranslation(
        bool& bAccumX, bool& bAccumY, bool& bAccumZ) const;

    /// Set flags controlling the axes about which to accumulate rotation.
    ///
    /// Either none, one or all of the flags must be true. 
    void SetAccumRotation(
        const bool bAccumX, const bool bAccumY, const bool bAccumZ);

    /// Get flags controlling the axes about which to accumulate rotation.
    void GetAccumRotation(
        bool& bAccumX, bool& bAccumY, bool& bAccumZ) const;

    /**
        Set the axis to consider front facing for the character.

        X is 0, Y is 1, Z is 2 and bIsNegative reverses direction.
        However, only X and Y are respected by the system.
    */
    void SetAccumFrontFacing(const unsigned char ucAxis,
        const bool bIsNegative);

    /// Get the axis to consider front facing for the character.
    void GetAccumFrontFacing(unsigned char& ucAxis, bool& bIsNegative) const;

    /// @}

protected:
    /// Protected constructor for streaming and cloning.
    NiPhysXAccumRigidBodyDest();

    /// Function provided for derived classes to obtain the accumulated
    /// transforms given the time and the PhysX root transform.
    void GetAccumTransforms(float fTime, NiQuaternion& kAccumRotation,
        NiPoint3& kAccumTranslation, NiQuaternion& kNonAccumRotation,
        NiPoint3& kNonAccumTranslation, const NiTransform& kRootTransform);
    
    /// Functions to break a transformation into accumulated and
    /// non-accumulated pieces.
    void FactorRot(const NiMatrix3& kFullRot,
        NiMatrix3& kAccumRot, NiMatrix3& kNonAccumRot);
    void FactorTrans(const NiPoint3& kFullTrans,
        NiPoint3& kAccumTrans, NiPoint3& kNonAccumTrans);
    
    /// Flags for storing the accumulation method.
    enum {
        // Translations that are accumulated
        ACCUM_X_TRANS   = 0x0001,
        ACCUM_Y_TRANS   = 0x0002,
        ACCUM_Z_TRANS   = 0x0004,
        
        // Rotation that is accumulated. Either one, none or all
        // of these may be specified.
        ACCUM_X_ROT     = 0x0008,
        ACCUM_Y_ROT     = 0x0010,
        ACCUM_Z_ROT     = 0x0020,

        // Front facing axis. Default is positive X.
        ACCUM_X_FRONT   = 0x0040,
        ACCUM_Y_FRONT   = 0x0080,
        ACCUM_Z_FRONT   = 0x0100,
        ACCUM_NEG_FRONT = 0x0200
    };
    NiDeclareFlags(NiUInt32);
};

NiSmartPointer(NiPhysXAccumRigidBodyDest);

#include "NiPhysXAccumRigidBodyDest.inl"

#endif  // #ifndef NIPHYSXACCUMRIGIDBODYDEST_H
