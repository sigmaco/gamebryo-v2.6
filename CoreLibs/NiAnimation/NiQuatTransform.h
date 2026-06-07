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

#ifndef NIQUATTRANSFORM_H
#define NIQUATTRANSFORM_H

#include "NiAnimationLibType.h"
#include <NiPoint3.h>
#include <NiQuaternion.h>
#include "NiFlags.h"

class NiStream;

class NIANIMATION_ENTRY NiQuatTransform : public NiMemObject
{
public:
    NiQuatTransform();
    NiQuatTransform(const NiPoint3& kTranslate, const NiQuaternion& kRotate,
        float fScale);

    inline const NiPoint3& GetTranslate() const;
    inline void SetTranslate(const NiPoint3& kTranslate);

    inline const NiQuaternion& GetRotate() const;
    inline void SetRotate(const NiQuaternion& kRotate);

    // This SetRotate function converts the NiMatrix3 to an NiQuaternion
    // before storing the value. It is provided as a convenience function.
    inline void SetRotate(const NiMatrix3& kRotate);

    inline float GetScale() const;
    inline void SetScale(float fScale);

    inline bool IsTranslateValid() const;
    inline bool IsRotateValid() const;
    inline bool IsScaleValid() const;
    inline bool IsTransformInvalid() const;

    inline bool operator==(const NiQuatTransform& kTransform) const;
    inline bool operator!=(const NiQuatTransform& kTransform) const;
    NiQuatTransform operator*(const NiQuatTransform& kTransform) const;

    void Invert(NiQuatTransform& kDest) const;
    
    void HierInvert(NiQuatTransform& kDest) const;
    NiQuatTransform HierApply(const NiQuatTransform& kTransform) const;

    inline void MakeInvalid();

    // *** begin Emergent internal use only ***
    NiPoint3* GetTranslatePointer();
    NiQuaternion* GetRotatePointer();
    float* GetScalePointer();
    void LoadBinary(NiStream& kStream);
    void SaveBinary(NiStream& kStream);
    void GetViewerStrings(NiViewerStringsArray* pkStrings);
    // *** end Emergent internal use only ***

protected:
    NiPoint3 m_kTranslate;
    NiQuaternion m_kRotate;
    float m_fScale;

    inline void SetTranslateValid(bool bValid);
    inline void SetRotateValid(bool bValid);
    inline void SetScaleValid(bool bValid);

    static const NiPoint3 INVALID_TRANSLATE;
    static const NiQuaternion INVALID_ROTATE;
    static const float INVALID_SCALE;
};

#include "NiQuatTransform.inl"

#endif   // #ifndef NIQUATTRANSFORM_H
