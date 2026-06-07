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

#ifndef NITEXTURETRANSFORM_H
#define NITEXTURETRANSFORM_H

#include "NiMatrix3.h"
#include "NiPoint2.h"
#include <NiMemObject.h>

class NIMAIN_ENTRY NiTextureTransform : public NiMemObject
{
public:
    enum TransformMethod
    {
        MAYA_DEPRECATED_TRANSFORM = 0,
        MAX_TRANSFORM,
        MAYA_TRANSFORM
    };

    NiTextureTransform();
    NiTextureTransform(const NiPoint2& kTranslate, float fRotate,
        const NiPoint2& kScale, const NiPoint2& kCenter, 
        TransformMethod eMethod);

    // Return the 3x3 Matrix representation of this transform. This is 
    // for the renderers so they won't need to construct the matrix for
    // every object every frame.
    inline const NiMatrix3* GetMatrix() const;

    // Convert this transform into an equivelent identity transform.
    void MakeIdentity();

    // UV translation
    inline NiPoint2 GetTranslate() const;
    inline void SetTranslate(const NiPoint2& kTranslate);

    // W axis rotation in texture space
    inline float GetRotate() const;
    inline void SetRotate(float fRotate);

    // UV Scale
    inline NiPoint2 GetScale() const;
    inline void SetScale(const NiPoint2& kScale);

    // Depending on the originating you expect to scale before the 
    // rotation or after the rotation. 3DS Max Scales first while Maya 
    // scales after.
    TransformMethod GetTransformMethod() const;
    inline void SetTransformMethod(TransformMethod eMethod);

    // The Center point defines where the texture rotates around
    inline NiPoint2 GetCenter() const;
    inline void SetCenter(const NiPoint2& kCenter);

    bool operator==(const NiTextureTransform &xform) const;
    bool operator!=(const NiTextureTransform &xform) const;

    static const NiTextureTransform IDENTITY;

    // *** begin Emergent internal use only ***

    // streaming
    void LoadBinary(NiStream& stream);
    void SaveBinary(NiStream& stream);

    // *** end Emergent internal use only ***

protected:

    NiPoint2 m_kTranslate;
    float m_fRotate;
    NiPoint2 m_kScale;
    NiPoint2 m_kCenter;

    // Cached matrix for the renderers. So building the Matrix only needs
    // to happen once and not every object every frame.
    mutable bool m_bMatrixDirty;
    mutable NiMatrix3 m_kMatrix;

    // Depending on the originating you expect to scale before the 
    // rotation or after the rotation. 3DS Max Scales first while Maya 
    // scales after.
    TransformMethod m_eMethod;

    // Rebuild the Cached matrix.  This can be called on a const instance
    // because it will need to be called when the renderers use it.  
    void UpdateMatrix() const;
};


#include "NiTextureTransform.inl"

#endif // NITEXTURETRANSFORM_H
