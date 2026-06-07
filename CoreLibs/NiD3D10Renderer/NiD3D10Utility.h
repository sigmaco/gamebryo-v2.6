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

#ifndef NID3D10UTILITY_H
#define NID3D10UTILITY_H

#include "NiD3D10RendererLibType.h"
#include "NiD3D10Headers.h"

class NiTransform;
class NiMatrix3;
class NiPoint3;

class NID3D10RENDERER_ENTRY NiD3D10Utility
{
public:
    // Matrix conversion
    static void GetD3DFromNi(D3DXMATRIX& kD3D, const NiTransform& kNi);
    static void GetD3DFromNi(D3DXMATRIX& kD3D, const NiMatrix3& kNiRot,
        const NiPoint3& kNiTrans, float fNiScale);
    static void GetD3DTransposeFromNi(D3DXMATRIX& kD3D,
        const NiTransform& kNi);
    static void GetD3DTransposeFromNi(D3DXMATRIX& kD3D,
        const NiMatrix3& kNiRot, const NiPoint3& kNiTrans, float fNiScale);

    // Fast float to int
    static int FastFloatToInt(float fValue);
};

//#include "NiD3D10Utility.inl"

#endif  //NID3D10UTILITY_H
