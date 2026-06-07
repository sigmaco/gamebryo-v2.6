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
inline NiEulerRotKey::NiEulerRotKey()
{
    m_uiNumKeys[0] = 0;
    m_uiNumKeys[1] = 0;
    m_uiNumKeys[2] = 0;
    m_apkKeys[0] = 0;
    m_apkKeys[1] = 0;
    m_apkKeys[2] = 0;
    m_eType[0] = NiAnimationKey::NOINTERP;
    m_eType[1] = NiAnimationKey::NOINTERP;
    m_eType[2] = NiAnimationKey::NOINTERP;
    m_uiLastIdx[0] = 0;
    m_uiLastIdx[1] = 0;
    m_uiLastIdx[2] = 0;
    m_ucSizes[0] = 0;
    m_ucSizes[1] = 0;
    m_ucSizes[2] = 0;
}
//---------------------------------------------------------------------------
inline NiEulerRotKey::NiEulerRotKey(NiFloatKey* pkX, unsigned int uiNumX,
    NiFloatKey::KeyType eTypeX, NiFloatKey* pkY, unsigned int uiNumY,
    NiFloatKey::KeyType eTypeY, NiFloatKey* pkZ, unsigned int uiNumZ,
    NiFloatKey::KeyType eTypeZ)
{
    m_uiNumKeys[0] = uiNumX;
    m_apkKeys[0] = pkX;
    m_eType[0] = eTypeX;

    m_uiNumKeys[1] = uiNumY;
    m_apkKeys[1] = pkY;
    m_eType[1] = eTypeY;

    m_uiNumKeys[2] = uiNumZ;
    m_apkKeys[2] = pkZ;
    m_eType[2] = eTypeZ;

    m_uiLastIdx[0] = 0;
    m_uiLastIdx[1] = 0;
    m_uiLastIdx[2] = 0;

    for (int i = 0; i < 3; i++)
    {
        if (m_uiNumKeys[i] != 0)
        {
            m_ucSizes[i] = NiFloatKey::GetKeySize(m_eType[i]); 
        }
        else
        {
            m_ucSizes[i] = 0;
        }
    }

}
//---------------------------------------------------------------------------
inline void NiEulerRotKey::Initialize(NiFloatKey* pkX, unsigned int uiNumX,
    NiFloatKey::KeyType eTypeX, NiFloatKey* pkY, unsigned int uiNumY,
    NiFloatKey::KeyType eTypeY, NiFloatKey* pkZ, unsigned int uiNumZ,
    NiFloatKey::KeyType eTypeZ)
{
    CleanUp();

    m_uiNumKeys[0] = uiNumX;
    m_apkKeys[0] = pkX;
    m_eType[0] = eTypeX;

    m_uiNumKeys[1] = uiNumY;
    m_apkKeys[1] = pkY;
    m_eType[1] = eTypeY;

    m_uiNumKeys[2] = uiNumZ;
    m_apkKeys[2] = pkZ;
    m_eType[2] = eTypeZ;

    m_uiLastIdx[0] = 0;
    m_uiLastIdx[1] = 0;
    m_uiLastIdx[2] = 0;
    
    for (int i = 0; i < 3; i++)
    {
        if (m_uiNumKeys[i] != 0)
        {
            m_ucSizes[i] = NiFloatKey::GetKeySize(m_eType[i]);   
        }
        else
        {
            m_ucSizes[i] = 0;
        }
    }

}
//---------------------------------------------------------------------------
inline void NiEulerRotKey::SetKeys(unsigned char ucIndex,
    NiFloatKey* pkKeys)
{
    NIASSERT(ucIndex <= 2);
    m_apkKeys[ucIndex] = pkKeys;
    m_uiLastIdx[ucIndex] = 0;
}
//---------------------------------------------------------------------------
inline void NiEulerRotKey::SetNumKeys(unsigned char ucIndex,
    unsigned int uiKeys)
{
    NIASSERT(ucIndex <= 2);
    m_uiNumKeys[ucIndex] = uiKeys;
    m_uiLastIdx[ucIndex] = 0;
}
//---------------------------------------------------------------------------
inline void NiEulerRotKey::SetType(unsigned char ucIndex,
    NiFloatKey::KeyType eType)
{
    NIASSERT(ucIndex <= 2);
    m_eType[ucIndex] = eType;
    m_uiLastIdx[ucIndex] = 0;
    m_ucSizes[ucIndex] = NiFloatKey::GetKeySize(eType);
}
//---------------------------------------------------------------------------
inline NiFloatKey* NiEulerRotKey::GetKeys(unsigned char ucIndex) const
{
    NIASSERT(ucIndex <= 2);
    return m_apkKeys[ucIndex];
}
//---------------------------------------------------------------------------
inline unsigned char NiEulerRotKey::GetKeySize(unsigned char ucIndex) const
{
    NIASSERT(ucIndex <= 2);
    return m_ucSizes[ucIndex];
}
//---------------------------------------------------------------------------
inline unsigned int NiEulerRotKey::GetNumKeys(unsigned char ucIndex) const
{
    NIASSERT(ucIndex <= 2);
    return m_uiNumKeys[ucIndex];
}
//---------------------------------------------------------------------------
inline NiFloatKey::KeyType NiEulerRotKey::GetType(
    unsigned char ucIndex) const
{
    NIASSERT(ucIndex <= 2);
    return m_eType[ucIndex];
}
//---------------------------------------------------------------------------
inline NiEulerRotKey::~NiEulerRotKey()
{
}
//---------------------------------------------------------------------------
