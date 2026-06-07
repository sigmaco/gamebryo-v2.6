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

#ifndef NIVIEWMATH_H
#define NIVIEWMATH_H

#include <NiPoint3.h>
#include <NiMatrix3.h>
#include <NiFrustum.h>
#include <NiBound.h>
#include <NiCamera.h>

#include "NiViewMathLibType.h"


class NIVIEWMATH_ENTRY NiViewMath
{
public:
    static const float PARALLEL_THRESHOLD;
    static const float INV_PARALLEL_THRESHOLD;

    static NiPoint3 Pan(const float fDX, const float fDY,
        const NiPoint3& kInputPoint, const NiMatrix3& kInputRotation);
    static NiMatrix3 Look(const float fDX, const float fDY,
        const NiMatrix3& kInputRotation, const NiPoint3& kUp);
    static void Orbit(const float fDX, const float fDY,
        const NiPoint3& kInputPoint, const NiMatrix3& kInputRotation,
        const NiPoint3& kCenter, const NiPoint3& kUp, NiPoint3& kReturnPoint,
        NiMatrix3& kReturnRotation);
    static NiPoint3 Dolly(const float fDZ, const NiPoint3& kInputPoint, 
        const NiMatrix3& kInputRotation);
    static NiFrustum OrthoZoom(const float fDZ,
        const NiFrustum& kInputFrustum);
    static NiMatrix3 LookAt(const NiPoint3& kFocus, const NiPoint3& kSource, 
        const NiPoint3& kUp);
    static NiPoint3 PanTo(const NiBound& kFocus, 
        const NiMatrix3& kCurrentRotation, const NiFrustum& kFrustum);
    static void MouseToRay(const float fX, const float fY, 
        const unsigned int uiAppWidth, const unsigned int uiAppHeight,
        const NiCamera* pkCamera, NiPoint3& kOrigin, NiPoint3& kDirection);
    static NiPoint3 TranslateOnAxis(const NiPoint3& kStartingPoint,
        const NiPoint3& kAxis, const NiPoint3& kInputOrigin,
        const NiPoint3& kInputDirection);
    static NiPoint3 TranslateOnPlane(const NiPoint3& kStartingPoint,
        const NiPoint3& kNormal, const NiPoint3& kInputOrigin,
        const NiPoint3& kInputDirection);
    static float RotateAboutAxis(const NiPoint3& kStartingPoint, 
        const NiPoint3& kAxis, const NiPoint3& kTangent,
        const NiPoint3& kBiTangent, const NiPoint3& kInputOrigin,
        const NiPoint3& kInputDirection);

protected:
    static float m_fOrthoZoomSpeed;
};

#endif  // #ifndef NIVIEWMATH_H
