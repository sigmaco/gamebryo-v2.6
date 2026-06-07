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

#ifndef NITRANSFORM_H
#define NITRANSFORM_H

#include "NiMatrix3.h"
#include "NiPoint3.h"

class NIMAIN_ENTRY NiTransform : public NiMemObject
{
public:
    // NOTE: Variable declaration order effects assembly language code.
    // Do not change.
    NiMatrix3 m_Rotate;
    NiPoint3 m_Translate;
    float m_fScale;

    void MakeIdentity();
    bool operator!=(const NiTransform &xform) const;
    NiTransform operator*(const NiTransform &xform) const;
    void Invert(NiTransform& kDest) const;
    inline NiPoint3 operator*(const NiPoint3 &kPoint) const;
    static void Interpolate(
        float fAmount, 
        const NiTransform& kStart,
        const NiTransform& kFinish,
        NiTransform& kOutput);

    // *** begin Emergent internal use only ***
#ifndef __SPU__
    // streaming
    void LoadBinary(NiStream& stream);
    void SaveBinary(NiStream& stream);
#endif
    // *** end Emergent internal use only ***
};

#include "NiTransform.inl"

#endif // NITRANSFORM_H
