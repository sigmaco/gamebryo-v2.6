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

#ifndef NIPHYSXTYPES_H
#define NIPHYSXTYPES_H

#include "NiPhysXLibType.h"


#include <NiMainPCH.h>
#include <NiColor.h>
#include <NiQuaternion.h>

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
    This class provides static member functions for converting between
    Gamebryo classes and PhysX classes.
    
    @note Conversions involving quaternions may produce unexpected results
        due to the fact that there are two valid quaternion representations
        for any 3D rotation, and all the conversions guarantee is that the
        rotations are equivalent. For example, a call to NiQuaternionToNxMat33
        followed by NxMat33ToNiQuaternion on the result will not always give
        the quaternion that you started with - it may produce the one with
        all the values negated. This is the same quaternion mathematically
        because it produces the same rotation, but some tests, such as
        Gamebryo's NiQuaternion equality operator, will say they are different.
*/
class NIPHYSX_ENTRY NiPhysXTypes
{
public:

    /// Convert a NxVec3 object to a NiPoint3 object.
    static void NxVec3ToNiPoint3(const NxVec3& kNxVSrc, NiPoint3& kNiPDest);
    
    /// Convert a NiPoint3 object to a NxVec3 object.
    static void NiPoint3ToNxVec3(const NiPoint3& kNiPSrc, NxVec3& kNxVDest);

    /// Convert a NxQuat object to a NiQuaternion object.
    static void NxQuatToNiQuaternion(const NxQuat& kNxQSrc,
        NiQuaternion& kNiQDest);

    /// Convert a NiQuaternion object to a NxQuat object.
    static void NiQuaternionToNxQuat(const NiQuaternion& kNiQSrc,
        NxQuat& kNxQDest);

    /// Convert a NxMat33 object to a NiMatrix3 object.
    static void NxMat33ToNiMatrix3(const NxMat33& kNxMSrc, 
        NiMatrix3& kNiMDest);

    /// Convert a NiMatrix3 object to a NxMat33 object.
    static void NiMatrix3ToNxMat33(const NiMatrix3& kNiMSrc, 
        NxMat33& kNxMDest);

    /**
        Convert a NxMat33 representing a rotation into a NiQuaternion
        representing the same rotation.
        
        Uses PhysX's NxMat33.toQuat function.
    */
    static void NxMat33ToNiQuaternion(const NxMat33& kNxMSrc,
        NiQuaternion& kNiQDest);

    /**
        Convert a NiQuaternion representing a rotation into a NxMat33
        representing the same rotation.
        
        Uses PhysX's NxMat33.fromQuat function.
    */
    static void NiQuaternionToNxMat33(const NiQuaternion& kNiQSrc,
        NxMat33& kNxMDest);

    /// Convert a PhysX pose matrix to the components of Gamebryo's
    /// NiTransform representation.
    static void NxMat34ToNiTransform(const NxMat34& kNxMSrc,
        NiMatrix3& kNiMDest, NiPoint3& kNiPDest);

    /// Convert the components of Gamebryo's NiTransform representation to
    /// a PhysX pose matrix.
    static void NiTransformToNxMat34(const NiMatrix3& kNiMSrc,
        const NiPoint3& kNiPSrc, NxMat34& kNxMDest);

    /// Convert from a PhysX pose matrix to the components
    /// of Gamebryo's NiQuatTransform representation.
    static void NxMat34ToNiQuatTransform(const NxMat34& kNxMSrc,
        NiQuaternion& kNiQDest, NiPoint3& kNiPDest);

    /// Convert from the components of Gamebryo's NiQuatTransform
    /// representation to a PhysX pose matrix.
    static void NiQuatTransformToNxMat34(const NiQuaternion& kNiQSrc,
        const NiPoint3& kNiPSrc, NxMat34& kNxMDest);

    /// Converts from the color representation used in PhysX's debug
    /// rendering to a Gamebryo color.
    static void NxColorToNiColorA(const NxU32 kNxSrc, NiColorA& kNiDest);

    /// Converts from the ARGB color representation used in PhysX's debug
    /// rendering to an RGBA color.
    static void NxColorToNiRGBA(const NxU32 kNxSrc, NiRGBA& uiNiDest);

    /// A zero vector.
    static const NxVec3 NXVEC3_ZERO;

    /// An identity matrix.
    static const NxMat33 NXMAT33_ID;

    /// An identity pose.
    static const NxMat34 NXMAT34_ID;

    /// @cond EMERGENT_INTERNAL
    
    static void NxVec3SaveBinary(NiStream& kStream, const NxVec3& kNxVec);
    static void NxVec3LoadBinary(NiStream& kStream, NxVec3& kNxVec);

    static void NxMat34SaveBinary(NiStream& kStream, const NxMat34& kNxMat);
    static void NxMat34LoadBinary(NiStream& kStream, NxMat34& kNxMat);
    
    static void NxQuatSaveBinary(NiStream& kStream, const NxQuat& kNxQuat);
    static void NxQuatLoadBinary(NiStream& kStream, NxQuat& kNxQuat);

    static const NxPhysicsSDKDesc NXPHYSICSSDKDESC_DEFAULT;

    /// @endcond

protected:
   
};

#include "NiPhysXTypes.inl"

#endif  // #ifndef NIPHYSXTYPES_H

