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

#ifndef NISQRDISTANCE_H
#define NISQRDISTANCE_H

#include "NiPoint3.h"
#include "NiSegment.h"
#include "NiTrigon.h"
#include "NiBox.h"
#include "NiParallelogram.h"

class NICOLLISION_ENTRY NiSqrDistance : public NiMemObject
{
public:
    // point - point
    static float Compute(const NiPoint3& kPnt0, const NiPoint3& kPoint1);

    // point - segment
    static float Compute(const NiPoint3& kPnt, const NiSegment& kSeg,
        float& fSegParam);

    // point - triangle
    static float Compute(const NiPoint3& kPnt, const NiTrigon& kTri,
        float& fTriParam0, float& fTriParam1);

    // point - box
    static float Compute(const NiPoint3& kPoint, const NiBox& kBox,
        float& fPpdParam0, float& fPpdParam1, float& fPpdParam2);

    // point - parallelogram
    static float Compute(const NiPoint3& kPoint,
        const NiParallelogram& kPgm, float& fPgmParam0,
        float& fPgmParam1);

    // segment - segment
    static float Compute(const NiSegment& kSeg0, const NiSegment& kSeg1,
        float& fSeg0Param, float& fSeg1Param);

    // segment - triangle
    static float Compute(const NiSegment& kSeg, const NiTrigon& kTri,
        float& fSegParam, float& fTriParam0, float& fTriParam1);

    // segment - box
    static float Compute(const NiSegment& kSeg,
        const NiBox& kBox, float& fSegParam,
        float& fBoxParam0, float& fBoxParam1, float& fBoxParam2);

    // segment - parallelogram
    static float Compute(const NiSegment& kSeg,
        const NiParallelogram& kPgm, float& fSegParam,
        float& fPgmParam0, float& fPgmParam1);

    // triangle - triangle
    static float Compute(const NiTrigon& kTri0, const NiTrigon& kTri1,
        float& fTri0Param0, float& fTri0Param1, float& fTri1Param0,
        float& fTri1Param1);

    // triangle - parallelogram
    static float Compute(const NiTrigon& kTri,
        const NiParallelogram& kPgm, float& fTriParam0, float& fTriParam1,
        float& fPgmParam0, float& fPgmParam1);
};

#include "NiSqrDistance.inl"

#endif

