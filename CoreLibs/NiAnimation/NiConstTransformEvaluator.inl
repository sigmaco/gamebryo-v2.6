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
inline void NiConstTransformEvaluator::SetPoseTranslate(
    const NiPoint3& kTranslate)
{
    m_kTransformValue.SetTranslate(kTranslate);
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
inline void NiConstTransformEvaluator::SetPoseRotate(
    const NiQuaternion& kRotate)
{
    m_kTransformValue.SetRotate(kRotate);
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
inline void NiConstTransformEvaluator::SetPoseScale(float fScale)
{
    m_kTransformValue.SetScale(fScale);
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
inline void NiConstTransformEvaluator::SetPoseValue(
    const NiQuatTransform& kPoseValue)
{
    if (kPoseValue.IsRotateValid())
        SetPoseRotate(kPoseValue.GetRotate());
    if (kPoseValue.IsScaleValid())
        SetPoseScale(kPoseValue.GetScale());
    if (kPoseValue.IsTranslateValid())
        SetPoseTranslate(kPoseValue.GetTranslate());
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
