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

inline bool MBrushTypeCircle::isInRadii(float fActiveRadiusSqr,
    float fOuterRadiusSqr,
    NiPoint3& kOrigin,
    NiPoint3& kPoint,
    bool* pbInOuterRadius,
    float* pfDistanceSqr
    )
{
    *pfDistanceSqr = NiSqr(kOrigin.x - kPoint.x) + NiSqr(kOrigin.y - kPoint.y);
    *pbInOuterRadius = *pfDistanceSqr <= fOuterRadiusSqr;
    return *pfDistanceSqr <= fActiveRadiusSqr;
}