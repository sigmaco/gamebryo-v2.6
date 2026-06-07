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

// Precompiled Header
#include "NiMainPCH.h"

#include "NiMatrix3.h"
#include <NiRTLib.h>

const NiMatrix3 NiMatrix3::ZERO(
    NiPoint3(0.0f, 0.0f, 0.0f),
    NiPoint3(0.0f, 0.0f, 0.0f),
    NiPoint3(0.0f, 0.0f, 0.0f));

const NiMatrix3 NiMatrix3::IDENTITY(
    NiPoint3(1.0f, 0.0f, 0.0f),
    NiPoint3(0.0f, 1.0f, 0.0f),
    NiPoint3(0.0f, 0.0f, 1.0f));

//---------------------------------------------------------------------------
NiMatrix3::NiMatrix3 (const NiPoint3& col0, const NiPoint3& col1, 
    const NiPoint3& col2 )
{
    SetCol( 0, col0 );
    SetCol( 1, col1 );
    SetCol( 2, col2 );
}
//---------------------------------------------------------------------------
void NiMatrix3::MakeIdentity ()
{
    m_pEntry[0][0] = 1.0f;
    m_pEntry[0][1] = 0.0f;
    m_pEntry[0][2] = 0.0f;
    m_pEntry[1][0] = 0.0f;
    m_pEntry[1][1] = 1.0f;
    m_pEntry[1][2] = 0.0f;
    m_pEntry[2][0] = 0.0f;
    m_pEntry[2][1] = 0.0f;
    m_pEntry[2][2] = 1.0f;
}
//---------------------------------------------------------------------------
void NiMatrix3::MakeDiagonal (float fDiag0, float fDiag1, float fDiag2)
{
    m_pEntry[0][0] = fDiag0;
    m_pEntry[0][1] = 0.0f;
    m_pEntry[0][2] = 0.0f;
    m_pEntry[1][0] = 0.0f;
    m_pEntry[1][1] = fDiag1;
    m_pEntry[1][2] = 0.0f;
    m_pEntry[2][0] = 0.0f;
    m_pEntry[2][1] = 0.0f;
    m_pEntry[2][2] = fDiag2;
}
//---------------------------------------------------------------------------
void NiMatrix3::MakeXRotation (float fAngle)
{
    float sn, cs;
    NiSinCos(fAngle, sn, cs);
    
    m_pEntry[0][0] = 1.0f;
    m_pEntry[0][1] = 0.0f;
    m_pEntry[0][2] = 0.0f;
    m_pEntry[1][0] = 0.0f;
    m_pEntry[1][1] = cs;
    m_pEntry[1][2] = sn;
    m_pEntry[2][0] = 0.0f;
    m_pEntry[2][1] = -sn;
    m_pEntry[2][2] = cs;
}
//---------------------------------------------------------------------------
void NiMatrix3::MakeYRotation (float fAngle)
{
    float sn, cs;
    NiSinCos(fAngle, sn, cs);
    
    m_pEntry[0][0] = cs;
    m_pEntry[0][1] = 0.0f;
    m_pEntry[0][2] = -sn;
    m_pEntry[1][0] = 0.0f;
    m_pEntry[1][1] = 1.0f;
    m_pEntry[1][2] = 0.0f;
    m_pEntry[2][0] = sn;
    m_pEntry[2][1] = 0.0f;
    m_pEntry[2][2] = cs;
}
//---------------------------------------------------------------------------
void NiMatrix3::MakeZRotation (float fAngle)
{
    float sn, cs;
    NiSinCos(fAngle, sn, cs);

    m_pEntry[0][0] = cs;
    m_pEntry[0][1] = sn;
    m_pEntry[0][2] = 0.0f;
    m_pEntry[1][0] = -sn;
    m_pEntry[1][1] = cs;
    m_pEntry[1][2] = 0.0f;
    m_pEntry[2][0] = 0.0f;
    m_pEntry[2][1] = 0.0f;
    m_pEntry[2][2] = 1.0f;
}
//---------------------------------------------------------------------------
void NiMatrix3::MakeRotation (float fAngle, float x, float y, float z)
{
    float sn, cs;
    NiSinCos(fAngle, sn, cs);

    float omcs = 1.0f-cs;
    float x2 = x*x;
    float y2 = y*y;
    float z2 = z*z;
    float xym = x*y*omcs;
    float xzm = x*z*omcs;
    float yzm = y*z*omcs;
    float xsin = x*sn;
    float ysin = y*sn;
    float zsin = z*sn;
    
    m_pEntry[0][0] = x2*omcs+cs;
    m_pEntry[0][1] = xym+zsin;
    m_pEntry[0][2] = xzm-ysin;
    m_pEntry[1][0] = xym-zsin;
    m_pEntry[1][1] = y2*omcs+cs;
    m_pEntry[1][2] = yzm+xsin;
    m_pEntry[2][0] = xzm+ysin;
    m_pEntry[2][1] = yzm-xsin;
    m_pEntry[2][2] = z2*omcs+cs;
}
//---------------------------------------------------------------------------
bool NiMatrix3::operator== (const NiMatrix3& mat) const
{
    return
        (m_pEntry[0][0] == mat.m_pEntry[0][0]) &&
        (m_pEntry[0][1] == mat.m_pEntry[0][1]) &&
        (m_pEntry[0][2] == mat.m_pEntry[0][2]) &&
        (m_pEntry[1][0] == mat.m_pEntry[1][0]) &&
        (m_pEntry[1][1] == mat.m_pEntry[1][1]) &&
        (m_pEntry[1][2] == mat.m_pEntry[1][2]) &&
        (m_pEntry[2][0] == mat.m_pEntry[2][0]) &&
        (m_pEntry[2][1] == mat.m_pEntry[2][1]) &&
        (m_pEntry[2][2] == mat.m_pEntry[2][2]);
}
//---------------------------------------------------------------------------
NiMatrix3 NiMatrix3::operator+ (const NiMatrix3& mat) const
{
    NiMatrix3 result = *this;
    result.m_pEntry[0][0] += mat.m_pEntry[0][0];
    result.m_pEntry[0][1] += mat.m_pEntry[0][1];
    result.m_pEntry[0][2] += mat.m_pEntry[0][2];
    result.m_pEntry[1][0] += mat.m_pEntry[1][0];
    result.m_pEntry[1][1] += mat.m_pEntry[1][1];
    result.m_pEntry[1][2] += mat.m_pEntry[1][2];
    result.m_pEntry[2][0] += mat.m_pEntry[2][0];
    result.m_pEntry[2][1] += mat.m_pEntry[2][1];
    result.m_pEntry[2][2] += mat.m_pEntry[2][2];
    return result;
}
//---------------------------------------------------------------------------
NiMatrix3 NiMatrix3::operator- (const NiMatrix3& mat) const
{
    NiMatrix3 result = *this;
    result.m_pEntry[0][0] -= mat.m_pEntry[0][0];
    result.m_pEntry[0][1] -= mat.m_pEntry[0][1];
    result.m_pEntry[0][2] -= mat.m_pEntry[0][2];
    result.m_pEntry[1][0] -= mat.m_pEntry[1][0];
    result.m_pEntry[1][1] -= mat.m_pEntry[1][1];
    result.m_pEntry[1][2] -= mat.m_pEntry[1][2];
    result.m_pEntry[2][0] -= mat.m_pEntry[2][0];
    result.m_pEntry[2][1] -= mat.m_pEntry[2][1];
    result.m_pEntry[2][2] -= mat.m_pEntry[2][2];
    return result;
}
//---------------------------------------------------------------------------
NiPoint3 operator* (const NiPoint3& pt, const NiMatrix3& mat)
{
    return NiPoint3
    (
        pt.x*mat.m_pEntry[0][0]+pt.y*mat.m_pEntry[1][0]+
            pt.z*mat.m_pEntry[2][0],
        pt.x*mat.m_pEntry[0][1]+pt.y*mat.m_pEntry[1][1]+
            pt.z*mat.m_pEntry[2][1],
        pt.x*mat.m_pEntry[0][2]+pt.y*mat.m_pEntry[1][2]+
            pt.z*mat.m_pEntry[2][2]
    );
}
//---------------------------------------------------------------------------
bool NiMatrix3::Inverse (NiMatrix3& inv) const
{
    // Invert a 3x3 using cofactors.  This is about 8 times faster than
    // the Numerical Recipes code which uses Gaussian elimination.

    inv.m_pEntry[0][0] = m_pEntry[1][1]*m_pEntry[2][2]-
        m_pEntry[1][2]*m_pEntry[2][1];
    inv.m_pEntry[0][1] = m_pEntry[0][2]*m_pEntry[2][1]-
        m_pEntry[0][1]*m_pEntry[2][2];
    inv.m_pEntry[0][2] = m_pEntry[0][1]*m_pEntry[1][2]-
        m_pEntry[0][2]*m_pEntry[1][1];
    inv.m_pEntry[1][0] = m_pEntry[1][2]*m_pEntry[2][0]-
        m_pEntry[1][0]*m_pEntry[2][2];
    inv.m_pEntry[1][1] = m_pEntry[0][0]*m_pEntry[2][2]-
        m_pEntry[0][2]*m_pEntry[2][0];
    inv.m_pEntry[1][2] = m_pEntry[0][2]*m_pEntry[1][0]-
        m_pEntry[0][0]*m_pEntry[1][2];
    inv.m_pEntry[2][0] = m_pEntry[1][0]*m_pEntry[2][1]-
        m_pEntry[1][1]*m_pEntry[2][0];
    inv.m_pEntry[2][1] = m_pEntry[0][1]*m_pEntry[2][0]-
        m_pEntry[0][0]*m_pEntry[2][1];
    inv.m_pEntry[2][2] = m_pEntry[0][0]*m_pEntry[1][1]-
        m_pEntry[0][1]*m_pEntry[1][0];

    float fDet = m_pEntry[0][0]*inv.m_pEntry[0][0]+
        m_pEntry[0][1]*inv.m_pEntry[1][0]+
        m_pEntry[0][2]*inv.m_pEntry[2][0];
    if ( NiAbs(fDet) <= 1e-06f )
        return false;

    float fInvdet = 1.0f/fDet;
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
        {
            inv.m_pEntry[row][col] *= fInvdet;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
NiMatrix3 NiMatrix3::Inverse () const
{
    NiMatrix3 inv;
    
    if ( Inverse(inv) == false )
        inv.MakeZero( );

    return inv;
}
//---------------------------------------------------------------------------
NiMatrix3 NiMatrix3::Transpose () const
{
    NiPoint3 row[3];

    GetRow( 0, row[0] );
    GetRow( 1, row[1] );
    GetRow( 2, row[2] );

    return NiMatrix3( row[0], row[1], row[2] );
}
//---------------------------------------------------------------------------
void NiMatrix3::ExtractAngleAndAxis (float& fAngle, float& fX, float& fY,
    float& fZ) const
{
    // Let (x,y,z) be the unit-length axis and let A be an angle of rotation.
    // The rotation matrix is R = I + sin(A)*P + (1-cos(A))*P^2 where
    // I is the identity and
    //
    //       +-        -+
    //   P = |  0 +z -y |
    //       | -z  0 +x |
    //       | +y -x  0 |
    //       +-        -+
    //
    // Some algebra will show that
    //
    //   cos(A) = (trace(R)-1)/2  and  R - R^t = 2*sin(A)*P

    float fTrace = m_pEntry[0][0]+m_pEntry[1][1]+m_pEntry[2][2];
    fAngle = NiACos(0.5f*(fTrace-1.0f));

    fX = m_pEntry[1][2]-m_pEntry[2][1];
    fY = m_pEntry[2][0]-m_pEntry[0][2];
    fZ = m_pEntry[0][1]-m_pEntry[1][0];
    float fLength = NiSqrt(fX*fX+fY*fY+fZ*fZ);
    const float fEpsilon = 1e-06f;
    if ( fLength > fEpsilon )
    {
        float fInvLength = 1.0f/fLength;
        fX *= fInvLength;
        fY *= fInvLength;
        fZ *= fInvLength;
    }
    else  // angle is 0 or pi
    {
        if ( fAngle > 1.0f )  // any number strictly between 0 and pi works
        {
            // angle must be pi
            fX = NiSqrt(0.5f*(1.0f+m_pEntry[0][0]));
            fY = NiSqrt(0.5f*(1.0f+m_pEntry[1][1]));
            fZ = NiSqrt(0.5f*(1.0f+m_pEntry[2][2]));

            // determine signs of axis components
            float tx, ty, tz;
            tx = m_pEntry[0][0]*fX+m_pEntry[0][1]*fY+m_pEntry[0][2]*fZ-fX;
            ty = m_pEntry[1][0]*fX+m_pEntry[1][1]*fY+m_pEntry[1][2]*fZ-fY;
            tz = m_pEntry[2][0]*fX+m_pEntry[2][1]*fY+m_pEntry[2][2]*fZ-fZ;
            fLength = tx*tx+ty*ty+tz*tz;
            if ( fLength < fEpsilon )
                return;

            fZ = -fZ;
            tx = m_pEntry[0][0]*fX+m_pEntry[0][1]*fY+m_pEntry[0][2]*fZ-fX;
            ty = m_pEntry[1][0]*fX+m_pEntry[1][1]*fY+m_pEntry[1][2]*fZ-fY;
            tz = m_pEntry[2][0]*fX+m_pEntry[2][1]*fY+m_pEntry[2][2]*fZ-fZ;
            fLength = tx*tx+ty*ty+tz*tz;
            if ( fLength < fEpsilon )
                return;

            fY = -fY;
            tx = m_pEntry[0][0]*fX+m_pEntry[0][1]*fY+m_pEntry[0][2]*fZ-fX;
            ty = m_pEntry[1][0]*fX+m_pEntry[1][1]*fY+m_pEntry[1][2]*fZ-fY;
            tz = m_pEntry[2][0]*fX+m_pEntry[2][1]*fY+m_pEntry[2][2]*fZ-fZ;
            fLength = tx*tx+ty*ty+tz*tz;
            if ( fLength < fEpsilon )
                return;
        }
        else
        {
            // Angle is zero, matrix is the identity, no unique axis, so
            // return (1,0,0) for as good a guess as any.
            fX = 1.0f;
            fY = 0.0f;
            fZ = 0.0f;
        }
    }
}
//---------------------------------------------------------------------------
bool NiMatrix3::Reorthogonalize ()
{
    // Factor M = QR where Q is orthogonal and R is upper triangular.
    // Algorithm uses Gram-Schmidt orthogonalization (the QR algorithm).
    //
    // If M = [ m0 | m1 | m2 ] and Q = [ q0 | q1 | q2 ], then
    //
    //   q0 = m0/|m0|
    //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
    //   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
    //
    // where |V| indicates length of vector V and A*B indicates dot
    // product of vectors A and B.  The matrix R has entries
    //
    //   r00 = q0*m0  r01 = q0*m1  r02 = q0*m2
    //   r10 = 0      r11 = q1*m1  r12 = q1*m2
    //   r20 = 0      r21 = 0      r22 = q2*m2
    //
    // The reorthogonalization replaces current matrix by computed Q.

    const float fEpsilon = 1e-05f;

    // unitize column 0
    float fLength = NiSqrt(m_pEntry[0][0]*m_pEntry[0][0] +
        m_pEntry[1][0]*m_pEntry[1][0] + m_pEntry[2][0]*m_pEntry[2][0]);
    if ( fLength < fEpsilon )
        return false;
    float fInvLength = 1.0f/fLength;
    m_pEntry[0][0] *= fInvLength;
    m_pEntry[1][0] *= fInvLength;
    m_pEntry[2][0] *= fInvLength;

    // project out column 0 from column 1
    float fDot = m_pEntry[0][0]*m_pEntry[0][1] + m_pEntry[1][0]*m_pEntry[1][1]
        + m_pEntry[2][0]*m_pEntry[2][1];
    m_pEntry[0][1] -= fDot*m_pEntry[0][0];
    m_pEntry[1][1] -= fDot*m_pEntry[1][0];
    m_pEntry[2][1] -= fDot*m_pEntry[2][0];

    // unitize column 1
    fLength = NiSqrt(m_pEntry[0][1]*m_pEntry[0][1] +
        m_pEntry[1][1]*m_pEntry[1][1] + m_pEntry[2][1]*m_pEntry[2][1]);
    if ( fLength < fEpsilon )
        return false;
    fInvLength = 1.0f/fLength;
    m_pEntry[0][1] *= fInvLength;
    m_pEntry[1][1] *= fInvLength;
    m_pEntry[2][1] *= fInvLength;

    // project out column 0 from column 2
    fDot = m_pEntry[0][0]*m_pEntry[0][2] + m_pEntry[1][0]*m_pEntry[1][2] +
        m_pEntry[2][0]*m_pEntry[2][2];
    m_pEntry[0][2] -= fDot*m_pEntry[0][0];
    m_pEntry[1][2] -= fDot*m_pEntry[1][0];
    m_pEntry[2][2] -= fDot*m_pEntry[2][0];

    // project out column 1 from column 2
    fDot = m_pEntry[0][1]*m_pEntry[0][2] + m_pEntry[1][1]*m_pEntry[1][2] +
        m_pEntry[2][1]*m_pEntry[2][2];
    m_pEntry[0][2] -= fDot*m_pEntry[0][1];
    m_pEntry[1][2] -= fDot*m_pEntry[1][1];
    m_pEntry[2][2] -= fDot*m_pEntry[2][1];

    // unitize column 2
    fLength = NiSqrt(m_pEntry[0][2]*m_pEntry[0][2] +
        m_pEntry[1][2]*m_pEntry[1][2] + m_pEntry[2][2]*m_pEntry[2][2]);
    if ( fLength < fEpsilon )
        return false;
    fInvLength = 1.0f/fLength;
    m_pEntry[0][2] *= fInvLength;
    m_pEntry[1][2] *= fInvLength;
    m_pEntry[2][2] *= fInvLength;

    return true;
}
//---------------------------------------------------------------------------
NiMatrix3 NiMatrix3::TransposeTimes (const NiMatrix3& mat) const
{
    NiMatrix3 prd;

    prd.m_pEntry[0][0] =
        m_pEntry[0][0]*mat.m_pEntry[0][0]+
        m_pEntry[1][0]*mat.m_pEntry[1][0]+
        m_pEntry[2][0]*mat.m_pEntry[2][0];
    prd.m_pEntry[1][0] =
        m_pEntry[0][1]*mat.m_pEntry[0][0]+
        m_pEntry[1][1]*mat.m_pEntry[1][0]+
        m_pEntry[2][1]*mat.m_pEntry[2][0];
    prd.m_pEntry[2][0] =
        m_pEntry[0][2]*mat.m_pEntry[0][0]+
        m_pEntry[1][2]*mat.m_pEntry[1][0]+
        m_pEntry[2][2]*mat.m_pEntry[2][0];
    prd.m_pEntry[0][1] =
        m_pEntry[0][0]*mat.m_pEntry[0][1]+
        m_pEntry[1][0]*mat.m_pEntry[1][1]+
        m_pEntry[2][0]*mat.m_pEntry[2][1];
    prd.m_pEntry[1][1] =
        m_pEntry[0][1]*mat.m_pEntry[0][1]+
        m_pEntry[1][1]*mat.m_pEntry[1][1]+
        m_pEntry[2][1]*mat.m_pEntry[2][1];
    prd.m_pEntry[2][1] =
        m_pEntry[0][2]*mat.m_pEntry[0][1]+
        m_pEntry[1][2]*mat.m_pEntry[1][1]+
        m_pEntry[2][2]*mat.m_pEntry[2][1];
    prd.m_pEntry[0][2] =
        m_pEntry[0][0]*mat.m_pEntry[0][2]+
        m_pEntry[1][0]*mat.m_pEntry[1][2]+
        m_pEntry[2][0]*mat.m_pEntry[2][2];
    prd.m_pEntry[1][2] =
        m_pEntry[0][1]*mat.m_pEntry[0][2]+
        m_pEntry[1][1]*mat.m_pEntry[1][2]+
        m_pEntry[2][1]*mat.m_pEntry[2][2];
    prd.m_pEntry[2][2] =
        m_pEntry[0][2]*mat.m_pEntry[0][2]+
        m_pEntry[1][2]*mat.m_pEntry[1][2]+
        m_pEntry[2][2]*mat.m_pEntry[2][2];

    return prd;
}
//---------------------------------------------------------------------------
NiMatrix3 NiMatrix3::Congruence (const NiMatrix3& rot) const
{
    NiMatrix3 prod;

    int iRow, iCol, iMid;
    for (iRow = 0; iRow < 3; iRow++)
    {
        for (iCol = 0; iCol < 3; iCol++)
        {
            prod.m_pEntry[iRow][iCol] = 0.0f;
            for (iMid = 0; iMid < 3; iMid++)
            {
                prod.m_pEntry[iRow][iCol] +=
                    rot.m_pEntry[iRow][iMid]*m_pEntry[iMid][iCol];
            }
        }
    }

    NiMatrix3 cong;
    for (iRow = 0; iRow < 3; iRow++)
    {
        for (iCol = 0; iCol < 3; iCol++)
        {
            cong.m_pEntry[iRow][iCol] = 0.0f;
            for (iMid = 0; iMid < 3; iMid++)
            {
                cong.m_pEntry[iRow][iCol] +=
                    prod.m_pEntry[iRow][iMid]*rot.m_pEntry[iCol][iMid];
            }
        }
    }

    return cong;
}
//---------------------------------------------------------------------------
void NiMatrix3::TransformVertices (const NiMatrix3& rot, const NiPoint3& trn,
    unsigned int uiVertexCount, const NiPoint3* pInVertex,
    NiPoint3* pOutVertex)
{
    // out = rot*in + trn
    for (unsigned int i = 0; i < uiVertexCount; i++)
    {
        pOutVertex[i].x = trn.x +
            rot.m_pEntry[0][0]*pInVertex[i].x +
            rot.m_pEntry[0][1]*pInVertex[i].y +
            rot.m_pEntry[0][2]*pInVertex[i].z;
        pOutVertex[i].y = trn.y +
            rot.m_pEntry[1][0]*pInVertex[i].x +
            rot.m_pEntry[1][1]*pInVertex[i].y +
            rot.m_pEntry[1][2]*pInVertex[i].z;
        pOutVertex[i].z = trn.z +
            rot.m_pEntry[2][0]*pInVertex[i].x +
            rot.m_pEntry[2][1]*pInVertex[i].y +
            rot.m_pEntry[2][2]*pInVertex[i].z;
    }
}
//---------------------------------------------------------------------------
void NiMatrix3::TransformNormals (const NiMatrix3& rot,
    unsigned int uiNormalCount, const NiPoint3* pInNormal,
    NiPoint3* pOutNormal)
{
    // out = transpose(rot)*in
    for (unsigned int i = 0; i < uiNormalCount; i++)
    {
        pOutNormal[i].x =
            rot.m_pEntry[0][0]*pInNormal[i].x +
            rot.m_pEntry[1][0]*pInNormal[i].y +
            rot.m_pEntry[2][0]*pInNormal[i].z;
        pOutNormal[i].y =
            rot.m_pEntry[0][1]*pInNormal[i].x +
            rot.m_pEntry[1][1]*pInNormal[i].y +
            rot.m_pEntry[2][1]*pInNormal[i].z;
        pOutNormal[i].z =
            rot.m_pEntry[0][2]*pInNormal[i].x +
            rot.m_pEntry[1][2]*pInNormal[i].y +
            rot.m_pEntry[2][2]*pInNormal[i].z;
    }
}
//---------------------------------------------------------------------------
void NiMatrix3::TransformVerticesAndNormals (const NiMatrix3& rot,
    const NiPoint3& trn, unsigned int uiCount, const NiPoint3* pInVertex,
    NiPoint3* pOutVertex, const NiPoint3* pInNormal,
    NiPoint3* pOutNormal)
{
    // vOut = rot*vIn + trn
    // nOut = transpose(rot)*nIn
    for (unsigned int i = 0; i < uiCount; i++)
    {
        pOutVertex[i].x = trn.x +
            rot.m_pEntry[0][0]*pInVertex[i].x +
            rot.m_pEntry[0][1]*pInVertex[i].y +
            rot.m_pEntry[0][2]*pInVertex[i].z;
        pOutVertex[i].y = trn.y +
            rot.m_pEntry[1][0]*pInVertex[i].x +
            rot.m_pEntry[1][1]*pInVertex[i].y +
            rot.m_pEntry[1][2]*pInVertex[i].z;
        pOutVertex[i].z = trn.z +
            rot.m_pEntry[2][0]*pInVertex[i].x +
            rot.m_pEntry[2][1]*pInVertex[i].y +
            rot.m_pEntry[2][2]*pInVertex[i].z;

        pOutNormal[i].x =
            rot.m_pEntry[0][0]*pInNormal[i].x +
            rot.m_pEntry[1][0]*pInNormal[i].y +
            rot.m_pEntry[2][0]*pInNormal[i].z;
        pOutNormal[i].y =
            rot.m_pEntry[0][1]*pInNormal[i].x +
            rot.m_pEntry[1][1]*pInNormal[i].y +
            rot.m_pEntry[2][1]*pInNormal[i].z;
        pOutNormal[i].z =
            rot.m_pEntry[0][2]*pInNormal[i].x +
            rot.m_pEntry[1][2]*pInNormal[i].y +
            rot.m_pEntry[2][2]*pInNormal[i].z;
    }
}
//---------------------------------------------------------------------------
void NiMatrix3::EigenSolveSymmetric (float afEigenvalue[3],
    NiPoint3 akEigenvector[3])
{
    NiMatrix3 kMat = *this;
    float afSubd[2];
    bool bReflection = kMat.Tridiagonal(afEigenvalue, afSubd);

    bool bConverged = kMat.QLAlgorithm(afEigenvalue, afSubd);
    NI_UNUSED_ARG(bConverged);
    NIASSERT(bConverged);

    // The columns of the matrix are the eigenvectors.  The tridiagonal
    // algorithm produces a reflection matrix, and the QL algorithm
    // postmultiplies by rotations.  The end result is a reflection matrix.
    // The last column has its signs changed to produce a rotation matrix,
    // therefore allowing an application to use the columns for a right-hand
    // coordinate system.
    akEigenvector[0].x = kMat.m_pEntry[0][0];
    akEigenvector[0].y = kMat.m_pEntry[1][0];
    akEigenvector[0].z = kMat.m_pEntry[2][0];
    akEigenvector[1].x = kMat.m_pEntry[0][1];
    akEigenvector[1].y = kMat.m_pEntry[1][1];
    akEigenvector[1].z = kMat.m_pEntry[2][1];
    if (bReflection)
    {
        akEigenvector[2].x = -kMat.m_pEntry[0][2];
        akEigenvector[2].y = -kMat.m_pEntry[1][2];
        akEigenvector[2].z = -kMat.m_pEntry[2][2];
    }
    else
    {
        akEigenvector[2].x = kMat.m_pEntry[0][2];
        akEigenvector[2].y = kMat.m_pEntry[1][2];
        akEigenvector[2].z = kMat.m_pEntry[2][2];
    }
}
//---------------------------------------------------------------------------
bool NiMatrix3::Tridiagonal (float afDiag[3], float afSubd[2])
{
    // Householder reduction T = Q^t M Q
    //   Input:   
    //     mat, symmetric 3x3 matrix M
    //   Output:  
    //     mat, orthogonal matrix Q
    //     diag, diagonal entries of T
    //     subd, subdiagonal entries of T (T is symmetric)

    float fM00 = m_pEntry[0][0];
    float fM01 = m_pEntry[0][1];
    float fM02 = m_pEntry[0][2];
    float fM11 = m_pEntry[1][1];
    float fM12 = m_pEntry[1][2];
    float fM22 = m_pEntry[2][2];

    afDiag[0] = fM00;

    const float fEpsilon = 1e-08f;
    if (NiAbs(fM02) >= fEpsilon)
    {
        afSubd[0] = NiSqrt(fM01 * fM01 + fM02 * fM02);
        float fInvLength = 1.0f / afSubd[0];
        fM01 *= fInvLength;
        fM02 *= fInvLength;
        float fTmp = 2.0f * fM01 * fM12 + fM02 * (fM22 - fM11);
        afDiag[1] = fM11 + fM02 * fTmp;
        afDiag[2] = fM22 - fM02 * fTmp;
        afSubd[1] = fM12 - fM01 * fTmp;
        m_pEntry[0][0] = 1.0f;
        m_pEntry[0][1] = 0.0f;
        m_pEntry[0][2] = 0.0f;
        m_pEntry[1][0] = 0.0f;
        m_pEntry[1][1] = fM01;
        m_pEntry[1][2] = fM02;
        m_pEntry[2][0] = 0.0f;
        m_pEntry[2][1] = fM02;
        m_pEntry[2][2] = -fM01;
        return true;
    }
    else
    {
        afDiag[1] = fM11;
        afDiag[2] = fM22;
        afSubd[0] = fM01;
        afSubd[1] = fM12;
        m_pEntry[0][0] = 1.0f;
        m_pEntry[0][1] = 0.0f;
        m_pEntry[0][2] = 0.0f;
        m_pEntry[1][0] = 0.0f;
        m_pEntry[1][1] = 1.0f;
        m_pEntry[1][2] = 0.0f;
        m_pEntry[2][0] = 0.0f;
        m_pEntry[2][1] = 0.0f;
        m_pEntry[2][2] = 1.0f;
        return false;
    }
}
//---------------------------------------------------------------------------
bool NiMatrix3::QLAlgorithm (float afDiag[3], float afSubd[2])
{
    const int iMax = 32;
    for (int i = 0; i < iMax; i++)
    {
        float fSum, fDiff, fDiscr, fEValue0, fEValue1, fCos, fSin, fTmp;
        int iRow;

        fSum = NiAbs(afDiag[0]) + NiAbs(afDiag[1]);
        if (NiAbs(afSubd[0]) + fSum == fSum)
        {
            // The matrix is effectively
            //       +-        -+
            //   M = | d0  0  0 |
            //       | 0  d1 s1 |
            //       | 0  s1 d2 |
            //       +-        -+

            afSubd[0] = 0.0f;

            // Test if the matrix is already diagonal.
            fSum = NiAbs(afDiag[1]) + NiAbs(afDiag[2]);
            if (NiAbs(afSubd[1]) + fSum == fSum)
            {
                // The current orthogonal matrix and the diagonal array do
                // not need to be updated. 
                afSubd[1] = 0.0f;
                return true;
            }

            // Compute the eigenvalues as roots of a quadratic equation.
            fSum = afDiag[1] + afDiag[2];
            fDiff = afDiag[1] - afDiag[2];
            fDiscr = NiSqrt(fDiff * fDiff + 4.0f * afSubd[1] * afSubd[1]);
            fEValue0 = 0.5f * (fSum - fDiscr);
            fEValue1 = 0.5f * (fSum + fDiscr);

            // Compute the Givens rotation.
            if (fDiff >= 0.0f)
            {
                fCos = afSubd[1];
                fSin = afDiag[1] - fEValue0;
            }
            else
            {
                fCos = afDiag[2] - fEValue0;
                fSin = afSubd[1];
            }
            fTmp = 1.0f / NiSqrt(fCos * fCos + fSin * fSin);
            fCos *= fTmp;
            fSin *= fTmp;

            // Postmultiply the current orthogonal matrix with the Givens
            // rotation.
            for (iRow = 0; iRow < 3; iRow++)
            {
                fTmp = m_pEntry[iRow][2];
                m_pEntry[iRow][2] = fSin * m_pEntry[iRow][1] + fCos * fTmp;
                m_pEntry[iRow][1] = fCos * m_pEntry[iRow][1] - fSin * fTmp;
            }

            // Update the tridiagonal matrix.
            afDiag[1] = fEValue0;
            afDiag[2] = fEValue1;
            afSubd[1] = 0.0f;
            return true;
        }

        fSum = NiAbs(afDiag[1]) + NiAbs(afDiag[2]);
        if (NiAbs(afSubd[1]) + fSum == fSum)
        {
            // The matrix is effectively
            //       +-         -+
            //   M = | d0  s0  0 |
            //       | s0  d1  0 |
            //       | 0   0  d2 |
            //       +-         -+

            afSubd[1] = 0.0f;

            // Test if the matrix is already diagonal.
            fSum = NiAbs(afDiag[0]) + NiAbs(afDiag[1]);
            if (NiAbs(afSubd[0]) + fSum == fSum)
            {
                // The current orthogonal matrix and the diagonal array do
                // not need to be updated. 
                afSubd[0] = 0.0f;
                return true;
            }

            // Compute the eigenvalues as roots of a quadratic equation.
            fSum = afDiag[0] + afDiag[1];
            fDiff = afDiag[0] - afDiag[1];
            fDiscr = NiSqrt(fDiff * fDiff + 4.0f * afSubd[0] * afSubd[0]);
            fEValue0 = 0.5f * (fSum - fDiscr);
            fEValue1 = 0.5f * (fSum + fDiscr);

            // Compute the Givens rotation.
            if (fDiff >= 0.0f)
            {
                fCos = afSubd[0];
                fSin = afDiag[0] - fEValue0;
            }
            else
            {
                fCos = afDiag[1] - fEValue0;
                fSin = afSubd[0];
            }
            fTmp = 1.0f / NiSqrt(fCos * fCos + fSin * fSin);
            fCos *= fTmp;
            fSin *= fTmp;

            // Postmultiply the current orthogonal matrix with the Givens
            // rotation.
            for (iRow = 0; iRow < 3; iRow++)
            {
                fTmp = m_pEntry[iRow][1];
                m_pEntry[iRow][1] = fSin * m_pEntry[iRow][0] + fCos * fTmp;
                m_pEntry[iRow][0] = fCos * m_pEntry[iRow][0] - fSin * fTmp;
            }

            // Update the tridiagonal matrix.
            afDiag[0] = fEValue0;
            afDiag[1] = fEValue1;
            afSubd[0] = 0.0f;
            return true;
        }

        // The matrix is
        //       +-        -+
        //   M = | d0 s0  0 |
        //       | s0 d1 s1 |
        //       | 0  s1 d2 |
        //       +-        -+

        // Set up the parameters for the first pass of the QL step.  The
        // value for A is the difference between diagonal term D[2] and the
        // implicit shift suggested by Wilkinson.
        float fRatio = (afDiag[1] - afDiag[0]) / (2.0f * afSubd[0]);
        float fRoot = NiSqrt(1.0f + fRatio * fRatio);
        float fB = afSubd[1];
        float fA = afDiag[2] - afDiag[0];
        if (fRatio >= 0.0f)
            fA += afSubd[0] / (fRatio + fRoot);
        else
            fA += afSubd[0] / (fRatio - fRoot);

        // Compute the Givens rotation for the first pass.
        if (NiAbs(fB) >= NiAbs(fA))
        {
            fRatio = fA / fB;
            fSin = 1.0f / NiSqrt(1.0f + fRatio * fRatio);
            fCos = fRatio * fSin;
        }
        else
        {
            fRatio = fB / fA;
            fCos = 1.0f / NiSqrt(1.0f + fRatio * fRatio);
            fSin = fRatio * fCos;
        }

        // Postmultiply the current orthogonal matrix with the Givens
        // rotation.
        for (iRow = 0; iRow < 3; iRow++)
        {
            fTmp = m_pEntry[iRow][2];
            m_pEntry[iRow][2] = fSin * m_pEntry[iRow][1] + fCos * fTmp;
            m_pEntry[iRow][1] = fCos * m_pEntry[iRow][1] - fSin * fTmp;
        }

        // Set up the parameters for the second pass of the QL step.  The
        // values tmp0 and tmp1 are required to fully update the tridiagonal
        // matrix at the end of the second pass.
        float fTmp0 = (afDiag[1] - afDiag[2]) * fSin +
            2.0f * afSubd[1] * fCos;
        float fTmp1 = fCos * afSubd[0];
        fB = fSin * afSubd[0];
        fA = fCos * fTmp0 - afSubd[1];
        fTmp0 *= fSin;

        // Compute the Givens rotation for the second pass.  The subdiagonal
        // term S[1] in the tridiagonal matrix is updated at this time.
        if (NiAbs(fB) >= NiAbs(fA))
        {
            fRatio = fA / fB;
            fRoot = NiSqrt(1.0f + fRatio * fRatio);
            afSubd[1] = fB * fRoot;
            fSin = 1.0f / fRoot;
            fCos = fRatio * fSin;
        }
        else
        {
            fRatio = fB / fA;
            fRoot = NiSqrt(1.0f + fRatio * fRatio);
            afSubd[1] = fA * fRoot;
            fCos = 1.0f / fRoot;
            fSin = fRatio * fCos;
        }

        // Postmultiply the current orthogonal matrix with the Givens
        // rotation.
        for (iRow = 0; iRow < 3; iRow++)
        {
            fTmp = m_pEntry[iRow][1];
            m_pEntry[iRow][1] = fSin * m_pEntry[iRow][0] + fCos * fTmp;
            m_pEntry[iRow][0] = fCos * m_pEntry[iRow][0] - fSin * fTmp;
        }

        // Complete the update of the tridiagonal matrix.
        float fTmp2 = afDiag[1] - fTmp0;
        afDiag[2] += fTmp0;
        fTmp0 = (afDiag[0] - fTmp2) * fSin + 2.0f * fTmp1 * fCos;
        afSubd[0] = fCos * fTmp0 - fTmp1;
        fTmp0 *= fSin;
        afDiag[1] = fTmp2 + fTmp0;
        afDiag[0] -= fTmp0;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiMatrix3::ToEulerAnglesXYZ (float& rfXAngle, float& rfYAngle,
    float& rfZAngle) const
{
    rfYAngle = -NiASin(m_pEntry[0][2]);
    if ( rfYAngle < NI_HALF_PI )
    {
        if ( rfYAngle > -NI_HALF_PI )
        {
            rfXAngle = -NiFastATan2(-m_pEntry[1][2],m_pEntry[2][2]);
            rfZAngle = -NiFastATan2(-m_pEntry[0][1],m_pEntry[0][0]);
            return true;
        }
        else
        {
            // WARNING.  Not a unique solution.
            float fRmY = NiFastATan2(m_pEntry[1][0],m_pEntry[1][1]);
            rfZAngle = 0.0f;  // any angle works
            rfXAngle = fRmY - rfZAngle;
            return false;
        }
    }
    else
    {
        // WARNING.  Not a unique solution.
        float fRpY = NiFastATan2(m_pEntry[1][0],m_pEntry[1][1]);
        rfZAngle = 0.0f;  // any angle works
        rfXAngle = rfZAngle - fRpY;
        return false;
    }
}
//---------------------------------------------------------------------------
bool NiMatrix3::ToEulerAnglesXZY (float& rfXAngle, float& rfZAngle,
    float& rfYAngle) const
{
    rfZAngle = NiASin(m_pEntry[0][1]);
    if ( rfZAngle < NI_HALF_PI )
    {
        if ( rfZAngle > -NI_HALF_PI )
        {
            rfXAngle = -NiFastATan2(m_pEntry[2][1],m_pEntry[1][1]);
            rfYAngle = -NiFastATan2(m_pEntry[0][2],m_pEntry[0][0]);
            return true;
        }
        else
        {
            // WARNING.  Not a unique solution.
            float fRmY = NiFastATan2(-m_pEntry[2][0],m_pEntry[2][2]);
            rfYAngle = 0.0f;  // any angle works
            rfXAngle = fRmY - rfYAngle;
            return false;
        }
    }
    else
    {
        // WARNING.  Not a unique solution.
        float fRpY = NiFastATan2(-m_pEntry[2][0],m_pEntry[2][2]);
        rfYAngle = 0.0f;  // any angle works
        rfXAngle = rfYAngle - fRpY;
        return false;
    }
}
//---------------------------------------------------------------------------
bool NiMatrix3::ToEulerAnglesYXZ (float& rfYAngle, float& rfXAngle,
    float& rfZAngle) const
{
    rfXAngle = NiASin(m_pEntry[1][2]);
    if ( rfXAngle < NI_HALF_PI )
    {
        if ( rfXAngle > -NI_HALF_PI )
        {
            rfYAngle = -NiFastATan2(m_pEntry[0][2],m_pEntry[2][2]);
            rfZAngle = -NiFastATan2(m_pEntry[1][0],m_pEntry[1][1]);
            return true;
        }
        else
        {
            // WARNING.  Not a unique solution.
            float fRmY = NiFastATan2(-m_pEntry[0][1],m_pEntry[0][0]);
            rfZAngle = 0.0f;  // any angle works
            rfYAngle = fRmY - rfZAngle;
            return false;
        }
    }
    else
    {
        // WARNING.  Not a unique solution.
        float fRpY = NiFastATan2(-m_pEntry[0][1],m_pEntry[0][0]);
        rfZAngle = 0.0f;  // any angle works
        rfYAngle = rfZAngle - fRpY;
        return false;
    }
}
//---------------------------------------------------------------------------
bool NiMatrix3::ToEulerAnglesYZX (float& rfYAngle, float& rfZAngle,
    float& rfXAngle) const
{
    rfZAngle = -NiASin(m_pEntry[1][0]);
    if ( rfZAngle < NI_HALF_PI )
    {
        if ( rfZAngle > -NI_HALF_PI )
        {
            rfYAngle = -NiFastATan2(-m_pEntry[2][0],m_pEntry[0][0]);
            rfXAngle = -NiFastATan2(-m_pEntry[1][2],m_pEntry[1][1]);
            return true;
        }
        else
        {
            // WARNING.  Not a unique solution.
            float fRmY = NiFastATan2(m_pEntry[2][1],m_pEntry[2][2]);
            rfXAngle = 0.0f;  // any angle works
            rfYAngle = fRmY - rfXAngle;
            return false;
        }
    }
    else
    {
        // WARNING.  Not a unique solution.
        float fRpY = NiFastATan2(m_pEntry[2][1],m_pEntry[2][2]);
        rfXAngle = 0.0f;  // any angle works
        rfYAngle = rfXAngle - fRpY;
        return false;
    }
}
//---------------------------------------------------------------------------
bool NiMatrix3::ToEulerAnglesZXY (float& rfZAngle, float& rfXAngle,
    float& rfYAngle) const
{
    rfXAngle = -NiASin(m_pEntry[2][1]);
    if ( rfXAngle < NI_HALF_PI )
    {
        if ( rfXAngle > -NI_HALF_PI )
        {
            rfZAngle = -NiFastATan2(-m_pEntry[0][1],m_pEntry[1][1]);
            rfYAngle = -NiFastATan2(-m_pEntry[2][0],m_pEntry[2][2]);
            return true;
        }
        else
        {
            // WARNING.  Not a unique solution.
            float fRmY = NiFastATan2(m_pEntry[0][2],m_pEntry[0][0]);
            rfYAngle = 0.0f;  // any angle works
            rfZAngle = fRmY - rfYAngle;
            return false;
        }
    }
    else
    {
        // WARNING.  Not a unique solution.
        float fRpY = NiFastATan2(m_pEntry[0][2],m_pEntry[0][0]);
        rfYAngle = 0.0f;  // any angle works
        rfZAngle = rfYAngle - fRpY;
        return false;
    }
}
//---------------------------------------------------------------------------
bool NiMatrix3::ToEulerAnglesZYX (float& rfZAngle, float& rfYAngle,
    float& rfXAngle) const
{
    rfYAngle = NiASin(m_pEntry[2][0]);
    if ( rfYAngle < NI_HALF_PI )
    {
        if ( rfYAngle > -NI_HALF_PI )
        {
            rfZAngle = -NiFastATan2(m_pEntry[1][0],m_pEntry[0][0]);
            rfXAngle = -NiFastATan2(m_pEntry[2][1],m_pEntry[2][2]);
            return true;
        }
        else
        {
            // WARNING.  Not a unique solution.
            float fRmY = NiFastATan2(-m_pEntry[0][1],m_pEntry[0][2]);
            rfXAngle = 0.0f;  // any angle works
            rfZAngle = fRmY - rfXAngle;
            return false;
        }
    }
    else
    {
        // WARNING.  Not a unique solution.
        float fRpY = NiFastATan2(-m_pEntry[0][1],m_pEntry[0][2]);
        rfXAngle = 0.0f;  // any angle works
        rfZAngle = rfXAngle - fRpY;
        return false;
    }
}
//---------------------------------------------------------------------------
void NiMatrix3::FromEulerAnglesXYZ (float fXAngle, float fYAngle,
    float fZAngle)
{
    NiMatrix3 kXRot, kYRot, kZRot;
    kXRot.MakeXRotation(fXAngle);
    kYRot.MakeYRotation(fYAngle);
    kZRot.MakeZRotation(fZAngle);
    *this = kXRot*(kYRot*kZRot);
}
//---------------------------------------------------------------------------
void NiMatrix3::FromEulerAnglesXZY (float fXAngle, float fZAngle,
    float fYAngle)
{
    NiMatrix3 kXRot, kYRot, kZRot;
    kXRot.MakeXRotation(fXAngle);
    kYRot.MakeYRotation(fYAngle);
    kZRot.MakeZRotation(fZAngle);
    *this = kXRot*(kZRot*kYRot);
}
//---------------------------------------------------------------------------
void NiMatrix3::FromEulerAnglesYXZ (float fYAngle, float fXAngle,
    float fZAngle)
{
    NiMatrix3 kXRot, kYRot, kZRot;
    kXRot.MakeXRotation(fXAngle);
    kYRot.MakeYRotation(fYAngle);
    kZRot.MakeZRotation(fZAngle);
    *this = kYRot*(kXRot*kZRot);
}
//---------------------------------------------------------------------------
void NiMatrix3::FromEulerAnglesYZX (float fYAngle, float fZAngle,
    float fXAngle)
{
    NiMatrix3 kXRot, kYRot, kZRot;
    kXRot.MakeXRotation(fXAngle);
    kYRot.MakeYRotation(fYAngle);
    kZRot.MakeZRotation(fZAngle);
    *this = kYRot*(kZRot*kXRot);
}
//---------------------------------------------------------------------------
void NiMatrix3::FromEulerAnglesZXY (float fZAngle, float fXAngle,
    float fYAngle)
{
    NiMatrix3 kXRot, kYRot, kZRot;
    kXRot.MakeXRotation(fXAngle);
    kYRot.MakeYRotation(fYAngle);
    kZRot.MakeZRotation(fZAngle);
    *this = kZRot*(kXRot*kYRot);
}
//---------------------------------------------------------------------------
void NiMatrix3::FromEulerAnglesZYX (float fZAngle, float fYAngle,
    float fXAngle)
{
    NiMatrix3 kXRot, kYRot, kZRot;
    kXRot.MakeXRotation(fXAngle);
    kYRot.MakeYRotation(fYAngle);
    kZRot.MakeZRotation(fZAngle);
    *this = kZRot*(kYRot*kXRot);
}
//---------------------------------------------------------------------------
void NiMatrix3::Snap()
{
    const float epsilon = 1e-08f;
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
        {
            if (NiAbs(m_pEntry[row][col]) <= epsilon && 
                m_pEntry[row][col] != 0.0f)
            {
                //NIASSERT(NiAbs(m_pEntry[row][col]) > 1e-32f);
                m_pEntry[row][col] = 0.0f;
            }
        }
    }
}

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
void NiMatrix3::LoadBinary (NiStream& stream)
{
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
            NiStreamLoadBinary(stream,m_pEntry[row][col]);
    }
}
//---------------------------------------------------------------------------
void NiMatrix3::SaveBinary (NiStream& stream)
{
    Snap();
    for (int row = 0; row < 3; row++)
    {
        for (int col = 0; col < 3; col++)
            NiStreamSaveBinary(stream,m_pEntry[row][col]);
    }
}
//---------------------------------------------------------------------------
char* NiMatrix3::GetViewerString (const char* pPrefix) const
{
    size_t stLen = strlen(pPrefix) + 128;
    char* pString = NiAlloc(char, stLen);
    NiSprintf(pString, stLen, "%s = ((%g,%g,%g),(%g,%g,%g),(%g,%g,%g))",
        pPrefix, 
        m_pEntry[0][0], m_pEntry[0][1], m_pEntry[0][2],
        m_pEntry[1][0],m_pEntry[1][1],m_pEntry[1][2],
        m_pEntry[2][0],m_pEntry[2][1],m_pEntry[2][2]);
    return pString;
}
//---------------------------------------------------------------------------
