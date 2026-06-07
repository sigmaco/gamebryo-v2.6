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

//---------------------------------------------------------------------------
inline float NiGeometricUtils::GetTriArea(const NiPoint3& kPoint0,
    const NiPoint3& kPoint1, const NiPoint3& kPoint2)
{
    NiPoint3 kEdge0 = kPoint1 - kPoint0;
    NiPoint3 kEdge1 = kPoint2 - kPoint0;
    NiPoint3 kCross = kEdge0.Cross(kEdge1);
    return 0.5f * kCross.Length();
}
//---------------------------------------------------------------------------
inline NiPoint3 NiGeometricUtils::GetTriCenter(const NiPoint3& kPoint0,
    const NiPoint3& kPoint1, const NiPoint3& kPoint2)
{
    NiPoint3 kCenter = kPoint0 + kPoint1 + kPoint2;
    kCenter /= 3.0f;
    return kCenter;
}
//---------------------------------------------------------------------------
inline NiPoint3 NiGeometricUtils::GetTriNormal(const NiPoint3& kPoint0,
    const NiPoint3& kPoint1, const NiPoint3& kPoint2)
{
    NiPoint3 kEdge0 = kPoint1 - kPoint0;
    NiPoint3 kEdge1 = kPoint2 - kPoint0;
    NiPoint3 kNormal = kEdge0.Cross(kEdge1);
    kNormal.Unitize();
    return kNormal;
}
//---------------------------------------------------------------------------
inline float NiGeometricUtils::GetDistToSeg(const NiPoint3& kPoint,
    const NiPoint3& kSegPoint, const NiPoint3& kSegDirection)
{
    return SegDistHelper(kPoint, kSegPoint, kSegDirection).Length();
}
//---------------------------------------------------------------------------
inline float NiGeometricUtils::GetDistSqrToSeg(const NiPoint3& kPoint,
    const NiPoint3& kSegPoint, const NiPoint3& kSegDirection)
{
    return SegDistHelper(kPoint, kSegPoint, kSegDirection).SqrLength();
}
//---------------------------------------------------------------------------
inline NiPoint3 NiGeometricUtils::SegDistHelper(const NiPoint3& kPoint,
    const NiPoint3& kSegPoint, const NiPoint3& kSegDirection)
{
    NiPoint3 kDiff = kPoint - kSegPoint;
    float fSegParam = kSegDirection.Dot(kDiff);

    if (fSegParam <= 0.0f)
    {
        fSegParam = 0.0f;
    }
    else
    {
        float fLenSqr = kSegDirection.SqrLength();
        if (fSegParam >= fLenSqr)
        {
            fSegParam = 1.0f;
            kDiff -= kSegDirection;
        }
        else
        {
            fSegParam /= fLenSqr;
            kDiff -= fSegParam * kSegDirection;
        }
    }
    return kDiff;
}
//---------------------------------------------------------------------------
