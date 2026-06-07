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
inline NiFloatData* NiFloatEvaluator::GetFloatData() const
{
    return m_spFloatData;
}
//---------------------------------------------------------------------------
inline void NiFloatEvaluator::SetFloatData(NiFloatData* pkFloatData)
{
    m_spFloatData = pkFloatData;
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
inline NiFloatKey* NiFloatEvaluator::GetKeys(unsigned int& uiNumKeys,
    NiFloatKey::KeyType& eType, unsigned char& ucSize) const
{
    if (m_spFloatData)
    {
        return m_spFloatData->GetAnim(uiNumKeys, eType, ucSize);
    }

    uiNumKeys = 0;
    eType = NiFloatKey::NOINTERP;
    ucSize = 0;
    return NULL;
}
//---------------------------------------------------------------------------
inline void NiFloatEvaluator::ReplaceKeys(NiFloatKey* pkKeys,
    unsigned int uiNumKeys, NiFloatKey::KeyType eType)
{
    if (uiNumKeys > 0)
    {
        if (!m_spFloatData)
        {
            m_spFloatData = NiNew NiFloatData;
        }

        m_spFloatData->ReplaceAnim(pkKeys, uiNumKeys, eType);
    }
    else if (m_spFloatData)
    {
        m_spFloatData->ReplaceAnim(NULL, 0, NiAnimationKey::NOINTERP);
    }
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
inline void NiFloatEvaluator::SetKeys(NiFloatKey* pkKeys,
    unsigned int uiNumKeys, NiFloatKey::KeyType eType)
{
    if (uiNumKeys > 0)
    {
        if (!m_spFloatData)
        {
            m_spFloatData = NiNew NiFloatData;
        }

        m_spFloatData->SetAnim(pkKeys, uiNumKeys, eType);
    }
    else if (m_spFloatData)
    {
        m_spFloatData->SetAnim(NULL, 0, NiAnimationKey::NOINTERP);
    }
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
