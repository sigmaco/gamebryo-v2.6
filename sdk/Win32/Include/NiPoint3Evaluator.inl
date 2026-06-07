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
inline NiPosData* NiPoint3Evaluator::GetPoint3Data() const
{
    return m_spPoint3Data;
}
//---------------------------------------------------------------------------
inline void NiPoint3Evaluator::SetPoint3Data(NiPosData* pkPosData)
{
    m_spPoint3Data = pkPosData;
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
inline NiPosKey* NiPoint3Evaluator::GetKeys(unsigned int& uiNumKeys,
    NiPosKey::KeyType& eType, unsigned char& ucSize) const
{
    if (m_spPoint3Data)
    {
        return m_spPoint3Data->GetAnim(uiNumKeys, eType, ucSize);
    }

    uiNumKeys = 0;
    eType = NiPosKey::NOINTERP;
    ucSize = 0;
    return NULL;
}
//---------------------------------------------------------------------------
inline void NiPoint3Evaluator::ReplaceKeys(NiPosKey* pkKeys,
    unsigned int uiNumKeys, NiPosKey::KeyType eType)
{
    if (uiNumKeys > 0)
    {
        if (!m_spPoint3Data)
        {
            m_spPoint3Data = NiNew NiPosData;
        }

        m_spPoint3Data->ReplaceAnim(pkKeys, uiNumKeys, eType);
    }
    else if (m_spPoint3Data)
    {
        m_spPoint3Data->ReplaceAnim(NULL, 0, NiAnimationKey::NOINTERP);
    }
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
inline void NiPoint3Evaluator::SetKeys(NiPosKey* pkKeys,
    unsigned int uiNumKeys, NiPosKey::KeyType eType)
{
    if (uiNumKeys > 0)
    {
        if (!m_spPoint3Data)
        {
            m_spPoint3Data = NiNew NiPosData;
        }

        m_spPoint3Data->SetAnim(pkKeys, uiNumKeys, eType);
    }
    else if (m_spPoint3Data)
    {
        m_spPoint3Data->SetAnim(NULL, 0, NiAnimationKey::NOINTERP);
    }
    SetEvalChannelTypes();
}
//---------------------------------------------------------------------------
