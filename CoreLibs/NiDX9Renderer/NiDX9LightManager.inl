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
inline NiDX9LightManager::LightEntry::LightEntry(const NiLight& kLight, 
    int iIndex) :
    m_uiRevID(0), // rev ID's start at 1, so this will always cause an update
    m_iIndex(iIndex),
    m_bNeedsRemoval(false),
    m_bActive(false)
{
    bool bUpdated = Update(kLight);
    NI_UNUSED_ARG(bUpdated);
    NIASSERT(bUpdated);
}
//---------------------------------------------------------------------------
inline D3DLIGHT9* NiDX9LightManager::LightEntry::GetD3DLight()
{
    return &m_kD3DLight;
}
//---------------------------------------------------------------------------
inline void NiDX9LightManager::LightEntry::SetIndex(int iIndex)
{
    m_iIndex = iIndex;
}
//---------------------------------------------------------------------------
inline int NiDX9LightManager::LightEntry::GetIndex() const
{
    return m_iIndex;
}
//---------------------------------------------------------------------------
inline void NiDX9LightManager::LightEntry::SetRemoval(bool bRemove)
{
    m_bNeedsRemoval = bRemove;
}
//---------------------------------------------------------------------------
inline bool NiDX9LightManager::LightEntry::GetRemoval() const
{
    return m_bNeedsRemoval;
}
//---------------------------------------------------------------------------
inline void NiDX9LightManager::LightEntry::SetActive(bool bActive)
{
    m_bActive = bActive;
}
//---------------------------------------------------------------------------
inline bool NiDX9LightManager::LightEntry::GetActive() const
{
    return m_bActive;
}
//---------------------------------------------------------------------------
inline void NiDX9LightManager::MarkIndexAsAvailable(unsigned int uiIndex)
{
    const unsigned int uiSize = 8 * sizeof(unsigned int);
    NIASSERT(uiIndex < uiSize * LIGHTINDEXARRAYSIZE);

    unsigned int uiArrayIndex = uiIndex / uiSize;
    unsigned int uiBitPosition = uiIndex % uiSize;
    // Set the bit to indicate it is free
    m_auiValidIndices[uiArrayIndex] &= ~(1 << uiBitPosition);
}
//---------------------------------------------------------------------------
