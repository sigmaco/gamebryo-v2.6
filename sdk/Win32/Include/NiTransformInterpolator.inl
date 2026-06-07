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
inline NiTransformData* NiTransformInterpolator::GetTransformData() const
{
    return m_spData;
}
//---------------------------------------------------------------------------
inline void NiTransformInterpolator::SetTransformData(NiTransformData* pkData)
{
    m_spData = pkData;
    m_uiLastRotIdx = 0;
    m_uiLastScaleIdx = 0;
    m_uiLastTransIdx = 0;
}
//---------------------------------------------------------------------------
inline unsigned int NiTransformInterpolator::GetNumPosKeys() const
{
    return (m_spData ? m_spData->GetNumPosKeys() : 0);
}
//---------------------------------------------------------------------------
inline NiPosKey* NiTransformInterpolator::GetPosData(
    unsigned int& uiNumKeys, NiPosKey::KeyType& eType, unsigned char& ucSize)
    const
{
    if (m_spData)
    {
        return m_spData->GetPosAnim(uiNumKeys, eType, ucSize);
    }

    uiNumKeys = 0;
    eType = NiPosKey::NOINTERP;
    ucSize = 0;
    return NULL;
}
//---------------------------------------------------------------------------
inline void NiTransformInterpolator::ReplacePosData(NiPosKey* pkKeys,
    unsigned int uiNumKeys, NiPosKey::KeyType eType)
{
    if (uiNumKeys > 0)
    {
        if (!m_spData)
        {
            m_spData = NiNew NiTransformData;
        }

        m_spData->ReplacePosAnim(pkKeys, uiNumKeys, eType);
        m_uiLastTransIdx = 0;
    }
    else if (m_spData)
    {
         m_spData->ReplacePosAnim(NULL, 0, NiAnimationKey::NOINTERP);
    }
}
//---------------------------------------------------------------------------
inline void NiTransformInterpolator::SetPoseTranslate(
    const NiPoint3& kTranslate)
{
    m_kTransformValue.SetTranslate(kTranslate);
    if (m_spData)
        m_spData->ReplacePosAnim(NULL, 0, NiAnimationKey::NOINTERP);
}
//---------------------------------------------------------------------------
inline unsigned int NiTransformInterpolator::GetNumRotKeys() const
{
    return (m_spData ? m_spData->GetNumRotKeys() : 0);
}
//---------------------------------------------------------------------------
inline NiRotKey* NiTransformInterpolator::GetRotData(unsigned int& uiNumKeys,
    NiRotKey::KeyType& eType, unsigned char& ucSize) const
{
    if (m_spData)
    {
        return m_spData->GetRotAnim(uiNumKeys, eType, ucSize);
    }

    uiNumKeys = 0;
    eType = NiRotKey::NOINTERP;
    ucSize = 0;
    return NULL;
}
//---------------------------------------------------------------------------
inline void NiTransformInterpolator::ReplaceRotData(NiRotKey* pkKeys,
    unsigned int uiNumKeys, NiRotKey::KeyType eType)
{
    if (uiNumKeys > 0)
    {
        if (!m_spData)
        {
            m_spData = NiNew NiTransformData;
        }

        m_spData->ReplaceRotAnim(pkKeys, uiNumKeys, eType);
        m_uiLastRotIdx = 0;
    }
    else if (m_spData)
    {
         m_spData->ReplaceRotAnim(NULL, 0, NiAnimationKey::NOINTERP);
    }
}
//---------------------------------------------------------------------------
inline void NiTransformInterpolator::SetPoseRotate(const NiQuaternion& kRotate)
{
    m_kTransformValue.SetRotate(kRotate);
    if (m_spData)
        m_spData->ReplaceRotAnim(NULL, 0, NiAnimationKey::NOINTERP);
}
//---------------------------------------------------------------------------
inline unsigned int NiTransformInterpolator::GetNumScaleKeys() const
{
    return (m_spData ? m_spData->GetNumScaleKeys() : 0);
}
//---------------------------------------------------------------------------
inline NiFloatKey* NiTransformInterpolator::GetScaleData(
    unsigned int& uiNumKeys, NiFloatKey::KeyType& eType, 
    unsigned char& ucSize) const
{
    if (m_spData)
    {
        return m_spData->GetScaleAnim(uiNumKeys, eType, ucSize);
    }

    uiNumKeys = 0;
    eType = NiFloatKey::NOINTERP;
    ucSize = 0;
    return NULL;
}
//---------------------------------------------------------------------------
inline void NiTransformInterpolator::ReplaceScaleData(NiFloatKey* pkKeys,
    unsigned int uiNumKeys, NiFloatKey::KeyType eType)
{
    if (uiNumKeys > 0)
    {
        if (!m_spData)
        {
            m_spData = NiNew NiTransformData;
        }

        m_spData->ReplaceScaleAnim(pkKeys, uiNumKeys, eType);
        m_uiLastScaleIdx = 0;
    }
    else if (m_spData)
    {
         m_spData->ReplaceScaleAnim(NULL, 0, NiAnimationKey::NOINTERP);
    }
}
//---------------------------------------------------------------------------
inline void NiTransformInterpolator::SetPoseScale(float fScale)
{
    m_kTransformValue.SetScale(fScale);
    if (m_spData)
        m_spData->ReplaceScaleAnim(NULL, 0, NiAnimationKey::NOINTERP);
}
//---------------------------------------------------------------------------
inline void NiTransformInterpolator::SetPoseValue(
    const NiQuatTransform& kPoseValue)
{
    if (kPoseValue.IsRotateValid())
        SetPoseRotate(kPoseValue.GetRotate());
    if (kPoseValue.IsScaleValid())
        SetPoseScale(kPoseValue.GetScale());
    if (kPoseValue.IsTranslateValid())
        SetPoseTranslate(kPoseValue.GetTranslate());
}
//---------------------------------------------------------------------------
