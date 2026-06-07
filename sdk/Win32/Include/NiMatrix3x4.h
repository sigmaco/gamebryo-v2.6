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


#ifndef NIMATRIX3X4_H
#define NIMATRIX3X4_H

#include <NiMeshLibType.h>
#include <NiMemObject.h>
#include <NiTransform.h>
#include <NiPoint2.h>
#include <NiPoint3.h>
#include <NiPoint4.h>

#if defined (_PS3)
    #include <vectormath/cpp/vectormath_aos.h>
#endif

/**
    Encodes a 4x4 matrix as 12 floats in row major order with the 
    last row assumed to be (0,0,0,1).
    
    The purpose of using this class is to minimize data passed to hardware
    when performing skinning operations.
*/
class NIMESH_ENTRY NiMatrix3x4 : public NiMemObject
{
public:

    /// Matrix data.
    NiPoint4 m_kEntry[3];

    /// Empty constructor - does not initialize data.
    inline NiMatrix3x4();

    /// Constructor for NiTransform.
    NiMatrix3x4(NiTransform& kTransform);

    /// Assign this 3x4 transformation matrix from another.
    inline NiMatrix3x4& operator =(const NiMatrix3x4& kMatrix);

    /// Assign this 3x4 transformation from an NiTransform.
    inline NiMatrix3x4& operator =(const NiTransform& kTransform);

    /// Elementwise add 3x4 transformation matrix to this matrix.
    NiMatrix3x4& operator +=(const NiMatrix3x4& kMatrix);

    /// Elementwise multiplication by a floating point scalar.
    NiMatrix3x4 operator*(float fScalar) const;

    /// Transform a 3-D point by this 3x4 transformation matrix.
    NiPoint3 operator*(const NiPoint3& pt) const;

    /// Transform a 3-D point by the 3x3 portion of the transformation matrix.
    inline NiPoint3 ScaleAndRotate(const NiPoint3& pt) const;

    /// Transform a 3-D point by the 2x4 portion of the transformation matrix.
    inline NiPoint2 CalculateST(const NiPoint3& pt) const;

    /// Transform a 4-D point by this 3x4 transformation matrix.
    inline NiPoint4 operator*(const NiPoint4& pt) const;

    /// Return the identity 3x4 transformation matrix.
    inline static const NiMatrix3x4 Identity();
    
    /// Make this an identity 3x4 transformation matrix.
    inline void MakeIdentity();

#if defined (_PS3)
    /// Conversion to a PS3 native Matrix4 for use on SPU.
    inline Vectormath::Aos::Matrix4 toM4() const;
#endif

};

#include "NiMatrix3x4.inl"

#endif
