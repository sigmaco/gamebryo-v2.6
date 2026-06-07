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

inline bool MBrushType::isInRadii(float fActiveRadiusSqr, 
    float fOuterRadiusSqr, 
    NiPoint3& kOrigin, 
    NiPoint3& kPoint, 
    bool* pbInOuterRadius, 
    float* pfDistanceSqr
    ) 
{
    NI_UNUSED_ARG(fActiveRadiusSqr);
    NI_UNUSED_ARG(fOuterRadiusSqr);
    NI_UNUSED_ARG(kOrigin);
    NI_UNUSED_ARG(kPoint);
    NI_UNUSED_ARG(pbInOuterRadius);
    NI_UNUSED_ARG(pfDistanceSqr);
    return false;
}
