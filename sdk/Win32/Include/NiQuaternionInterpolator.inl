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
inline NiRotData* NiQuaternionInterpolator::GetQuaternionData() const
{
    return m_spQuaternionData;
}
//---------------------------------------------------------------------------
inline void NiQuaternionInterpolator::SetQuaternionData(
    NiRotData* pkRotData)
{
    m_spQuaternionData = pkRotData;
}
//---------------------------------------------------------------------------
inline NiRotKey* NiQuaternionInterpolator::GetKeys(unsigned int& uiNumKeys,
    NiRotKey::KeyType& eType, unsigned char& ucSize) const
{
    if (m_spQuaternionData)
    {
        return m_spQuaternionData->GetAnim(uiNumKeys, eType, ucSize);
    }

    uiNumKeys = 0;
    eType = NiRotKey::NOINTERP;
    ucSize = 0;
    return NULL;
}
//---------------------------------------------------------------------------
inline void NiQuaternionInterpolator::ReplaceKeys(NiRotKey* pkKeys,
    unsigned int uiNumKeys, NiRotKey::KeyType eType)
{
    if (uiNumKeys > 0)
    {
        if (!m_spQuaternionData)
        {
            m_spQuaternionData = NiNew NiRotData;
        }

        m_spQuaternionData->ReplaceAnim(pkKeys, uiNumKeys, eType);
        m_uiLastIdx = 0;
    }
    else if (m_spQuaternionData)
    {
        m_spQuaternionData->ReplaceAnim(NULL, 0, NiAnimationKey::NOINTERP);
    }
}
//---------------------------------------------------------------------------
inline void NiQuaternionInterpolator::SetKeys(NiRotKey* pkKeys,
    unsigned int uiNumKeys, NiRotKey::KeyType eType)
{
    if (uiNumKeys > 0)
    {
        if (!m_spQuaternionData)
        {
            m_spQuaternionData = NiNew NiRotData;
        }

        m_spQuaternionData->SetAnim(pkKeys, uiNumKeys, eType);
        m_uiLastIdx = 0;
    }
    else if (m_spQuaternionData)
    {
        m_spQuaternionData->SetAnim(NULL, 0, NiAnimationKey::NOINTERP);
    }
}
//---------------------------------------------------------------------------
