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
inline NiTextureTransform::NiTextureTransform() :
    m_kTranslate(NiPoint2::ZERO), m_fRotate(0.0f),
        m_kScale(NiPoint2(1.0f, 1.0f)), m_kCenter(NiPoint2(0.5f, 0.5f)),
        m_bMatrixDirty(true), 
        m_eMethod(NiTextureTransform::MAYA_TRANSFORM)
{
}
//---------------------------------------------------------------------------
inline const NiMatrix3* NiTextureTransform::GetMatrix() const
{
    // Recompute the Matrix
    if (m_bMatrixDirty)
    {
        UpdateMatrix();
    }

    return &m_kMatrix;
}
//---------------------------------------------------------------------------
inline NiPoint2 NiTextureTransform::GetTranslate() const
{
    return m_kTranslate;
}
//---------------------------------------------------------------------------
inline void NiTextureTransform::SetTranslate(const NiPoint2& kTranslate)
{
    m_bMatrixDirty = m_bMatrixDirty || (m_kTranslate != kTranslate);
    m_kTranslate = kTranslate;
}
//---------------------------------------------------------------------------
inline float NiTextureTransform::GetRotate() const
{
    return m_fRotate;
}
//---------------------------------------------------------------------------
inline void NiTextureTransform::SetRotate(float fRotate)
{
    m_bMatrixDirty = m_bMatrixDirty || (m_fRotate != fRotate);
    m_fRotate = fRotate;
}
//---------------------------------------------------------------------------
inline NiPoint2 NiTextureTransform::GetScale() const
{
    return m_kScale;
}
//---------------------------------------------------------------------------
inline void NiTextureTransform::SetScale(const NiPoint2& kScale)
{
    m_bMatrixDirty = m_bMatrixDirty || (m_kScale != kScale);
    m_kScale = kScale;
}
//---------------------------------------------------------------------------
inline NiTextureTransform::TransformMethod 
    NiTextureTransform::GetTransformMethod() const
{
    return m_eMethod;
}
//---------------------------------------------------------------------------
inline void NiTextureTransform::SetTransformMethod(TransformMethod eMethod)
{
    m_bMatrixDirty = m_bMatrixDirty || (m_eMethod != eMethod);
    m_eMethod = eMethod;
}
//---------------------------------------------------------------------------
inline NiPoint2 NiTextureTransform::GetCenter() const
{
    return m_kCenter;
}
//---------------------------------------------------------------------------
inline void NiTextureTransform::SetCenter(const NiPoint2& kCenter)
{
    m_bMatrixDirty = m_bMatrixDirty || (m_kCenter != kCenter);
    m_kCenter = kCenter;
}
//---------------------------------------------------------------------------
