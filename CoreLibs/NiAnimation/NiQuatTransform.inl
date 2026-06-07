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
inline NiQuatTransform::NiQuatTransform() : m_kTranslate(INVALID_TRANSLATE),
    m_kRotate(INVALID_ROTATE), m_fScale(INVALID_SCALE)
{
    //No need to set the translate,rotate, or scale valid flags here
    //they are all invalid and the initializer list did it for us already
    //by setting the values to INVALID_*
}
//---------------------------------------------------------------------------
inline NiQuatTransform::NiQuatTransform(const NiPoint3& kTranslate,
    const NiQuaternion& kRotate, float fScale) : m_kTranslate(kTranslate),
    m_kRotate(kRotate), m_fScale(fScale)
{
    //No need to set the translate,rotate, or scale valid flags here
    //they are all invalid and the initializer list did it for us already
    //by setting the values to INVALID_* if the user made them invalid
}
//---------------------------------------------------------------------------
inline const NiPoint3& NiQuatTransform::GetTranslate() const
{
    return m_kTranslate;
}
//---------------------------------------------------------------------------
inline NiPoint3* NiQuatTransform::GetTranslatePointer()
{
    return &m_kTranslate;
}
//---------------------------------------------------------------------------
inline void NiQuatTransform::SetTranslate(const NiPoint3& kTranslate)
{
    m_kTranslate = kTranslate;
    SetTranslateValid(true);
}
//---------------------------------------------------------------------------
inline const NiQuaternion& NiQuatTransform::GetRotate() const
{
    return m_kRotate;
}
//---------------------------------------------------------------------------
inline NiQuaternion* NiQuatTransform::GetRotatePointer()
{
    return &m_kRotate;
}
//---------------------------------------------------------------------------
inline void NiQuatTransform::SetRotate(const NiQuaternion& kRotate)
{
    m_kRotate = kRotate;
    SetRotateValid(true);
}
//---------------------------------------------------------------------------
inline void NiQuatTransform::SetRotate(const NiMatrix3& kRotate)
{
    NiQuaternion kRotateQuat;
    kRotateQuat.FromRotation(kRotate);
    SetRotate(kRotateQuat);
}
//---------------------------------------------------------------------------
inline float NiQuatTransform::GetScale() const
{
    return m_fScale;
}
//---------------------------------------------------------------------------
inline float* NiQuatTransform::GetScalePointer()
{
    return &m_fScale;
}
//---------------------------------------------------------------------------
inline void NiQuatTransform::SetScale(float fScale)
{
    m_fScale = fScale;
    SetScaleValid(true);
}
//---------------------------------------------------------------------------
inline bool NiQuatTransform::IsTranslateValid() const
{
    return m_kTranslate.x != -NI_INFINITY;
}
//---------------------------------------------------------------------------
inline bool NiQuatTransform::IsRotateValid() const
{
    return m_kRotate.GetX() != -NI_INFINITY;
}
//---------------------------------------------------------------------------
inline bool NiQuatTransform::IsScaleValid() const
{
    return m_fScale != -NI_INFINITY;
}
//---------------------------------------------------------------------------
inline bool NiQuatTransform::IsTransformInvalid() const
{
    return !(IsScaleValid() || IsRotateValid() || IsTranslateValid());
}
//---------------------------------------------------------------------------
inline void NiQuatTransform::SetTranslateValid(bool bValid)
{
    if (!bValid)
        m_kTranslate.x = -NI_INFINITY;
}
//---------------------------------------------------------------------------
inline void NiQuatTransform::SetRotateValid(bool bValid)
{
    if (!bValid)
        m_kRotate.SetX(-NI_INFINITY);
}
//---------------------------------------------------------------------------
inline void NiQuatTransform::SetScaleValid(bool bValid)
{
    if (!bValid)
        m_fScale = -NI_INFINITY;
}
//---------------------------------------------------------------------------
inline bool NiQuatTransform::operator==(const NiQuatTransform& kTransform)
    const
{
    return (m_kTranslate == kTransform.m_kTranslate &&
        m_kRotate == kTransform.m_kRotate &&
        m_fScale == kTransform.m_fScale);
}
//---------------------------------------------------------------------------
inline bool NiQuatTransform::operator!=(const NiQuatTransform& kTransform)
    const
{
    return !(*this == kTransform);
}
//---------------------------------------------------------------------------
inline void NiQuatTransform::MakeInvalid()
{
    SetTranslateValid(false);
    SetRotateValid(false);
    SetScaleValid(false);
}
//---------------------------------------------------------------------------
