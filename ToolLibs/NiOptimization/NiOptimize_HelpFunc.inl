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

#include <NiTexturingProperty.h>
#include <NiVertexColorProperty.h>

//---------------------------------------------------------------------------
// Helper functions.
//---------------------------------------------------------------------------
inline bool NiOptimize::CloseTo(float fA, float fB, float fEpsilon)
{
    // Determines if float fA is within fEpsilon of float fB.

    // Returns: true, if fA is within fEpsilon of fB.
    //          false, otherwise.

    return ((float) fabs(fA - fB) <= fEpsilon);
}
//---------------------------------------------------------------------------
inline bool NiOptimize::Point3CloseTo(const NiPoint3& kP1,
    const NiPoint3& kP2, float fEpsilon)
{
    // Determines if point kP1 is within fEpsilon of point kP2.

    // Returns: true, if kP1 is within fEpsilon of kP2.
    //          false, otherwise.

    if (!CloseTo(kP1.x, kP2.x, fEpsilon))
    {
        return false;
    }
    if (!CloseTo(kP1.y, kP2.y, fEpsilon))
    {
        return false;
    }
    if (!CloseTo(kP1.z, kP2.z, fEpsilon))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
inline bool NiOptimize::Matrix3CloseTo(const NiMatrix3& kM1,
    const NiMatrix3& kM2, float fEpsilon)
{
    // Determines if matrix kM1 is within fEpsilon of matrix kM2.

    // Returns: true, if kM1 is within fEpsilon of kM2.
    //          false, otherwise.

    NiPoint3 kM1_row0, kM1_row1, kM1_row2;
    NiPoint3 kM2_row0, kM2_row1, kM2_row2;

    kM1.GetRow(0, kM1_row0);
    kM1.GetRow(1, kM1_row1);
    kM1.GetRow(2, kM1_row2);
    kM2.GetRow(0, kM2_row0);
    kM2.GetRow(1, kM2_row1);
    kM2.GetRow(2, kM2_row2);

    if (Point3CloseTo(kM1_row0, kM2_row0, fEpsilon) &&
        Point3CloseTo(kM1_row1, kM2_row1, fEpsilon) &&
        Point3CloseTo(kM1_row2, kM2_row2, fEpsilon))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
inline bool NiOptimize::QuatCloseTo(const NiQuaternion& kQ1,
    const NiQuaternion& kQ2, float fEpsilon)
{
    // Determines if quaternion kQ1 is within fEpsilon of quaternion kQ2.

    // Returns: true, if kQ1 is within fEpsilon of kQ2.
    //          false, otherwise.

    if (!CloseTo(kQ1.GetW(), kQ2.GetW(), fEpsilon))
    {
        return false;
    }
    if (!CloseTo(kQ1.GetX(), kQ2.GetX(), fEpsilon))
    {
        return false;
    }
    if (!CloseTo(kQ1.GetY(), kQ2.GetY(), fEpsilon))
    {
        return false;
    }
    if (!CloseTo(kQ1.GetZ(), kQ2.GetZ(), fEpsilon))
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
inline bool NiOptimize::IsPowerOf2(unsigned int uiNum)
{
    // Determines if the provided integer is a power of two.

    // Returns: true, if uiNum is a power of two.
    //          false, otherwise.

    float fNum = (float) uiNum;
    while (fNum > 1.0f)
        fNum = fNum / 2.0f;

    if (fNum == 1.0f)
        return true;

    return false;
}
//---------------------------------------------------------------------------
inline bool NiOptimize::CheckTextureSize(NiTexture* pkTexture)
{
    // Checks whether or not the texture dimensions are powers of two.

    // Returns: true, if the texture dimensions are powers of two.
    //          false, otherwise.

    NIASSERT(pkTexture);
    unsigned int uiWidth = pkTexture->GetWidth();
    unsigned int uiHeight = pkTexture->GetHeight();
    if (uiWidth && uiHeight && IsPowerOf2(uiWidth) &&
        IsPowerOf2(uiHeight))
    {
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
