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
inline NiColorData* NiColorEvaluator::GetColorData() const
{
    return m_spColorData;
}
//---------------------------------------------------------------------------
inline void NiColorEvaluator::SetColorData(NiColorData* pkColorData)
{
    m_spColorData = pkColorData;
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
inline NiColorKey* NiColorEvaluator::GetKeys(unsigned int& uiNumKeys,
    NiColorKey::KeyType& eType, unsigned char& ucSize) const
{
    if (m_spColorData)
    {
        return m_spColorData->GetAnim(uiNumKeys, eType, ucSize);
    }

    uiNumKeys = 0;
    eType = NiColorKey::NOINTERP;
    ucSize = 0;
    return NULL;
}
//---------------------------------------------------------------------------
inline void NiColorEvaluator::ReplaceKeys(NiColorKey* pkKeys,
    unsigned int uiNumKeys, NiColorKey::KeyType eType)
{
    if (uiNumKeys > 0)
    {
        if (!m_spColorData)
        {
            m_spColorData = NiNew NiColorData;
        }

        m_spColorData->ReplaceAnim(pkKeys, uiNumKeys, eType);
    }
    else if (m_spColorData)
    {
        m_spColorData->ReplaceAnim(NULL, 0, NiAnimationKey::NOINTERP);
    }
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
inline void NiColorEvaluator::SetKeys(NiColorKey* pkKeys,
    unsigned int uiNumKeys, NiColorKey::KeyType eType)
{
    if (uiNumKeys > 0)
    {
        if (!m_spColorData)
        {
            m_spColorData = NiNew NiColorData;
        }

        m_spColorData->SetAnim(pkKeys, uiNumKeys, eType);
    }
    else if (m_spColorData)
    {
        m_spColorData->SetAnim(NULL, 0, NiAnimationKey::NOINTERP);
    }
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
