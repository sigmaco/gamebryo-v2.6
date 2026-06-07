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
inline const NiFixedString& NiTerrainSectorComponent::GetPropTerrainID() const
{
    if (m_spMasterComponent && !GetBit(PROP_TERRAIN_ID_CHANGED))
    {
        return m_spMasterComponent->GetPropTerrainID();
    }
    else
    {
        return m_kPropTerrainID;
    }
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainSectorComponent::GetPropMaskSize() const
{
    if (m_spMasterComponent && !GetBit(PROP_MASK_SIZE_CHANGED))
    {
        return m_spMasterComponent->GetPropMaskSize();
    }
    else
    {
        return m_uiPropMaskSize;
    }
}
//---------------------------------------------------------------------------
inline bool NiTerrainSectorComponent::GetPropDeformable() const
{
    if (m_spMasterComponent && !GetBit(PROP_DEFORMABLE_CHANGED))
    {
        return m_spMasterComponent->GetPropDeformable();
    }
    else
    {
        return m_bPropDeformable;
    }
}
//---------------------------------------------------------------------------
inline NiInt16 NiTerrainSectorComponent::GetPropSectorIndexX() const
{
    return m_iPropSectorIndex[0];
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorComponent::SetPropSectorIndexX(
    NiInt16 iIndexX)
{
    if (m_iPropSectorIndex[0] != iIndexX)
    {
        m_iPropSectorIndex[0] = iIndexX;
        MarkSettingChanged();
        m_spSector->SetSectorIndex(m_iPropSectorIndex[0], 
            m_iPropSectorIndex[1]);
    }
}
//---------------------------------------------------------------------------
inline NiInt16 NiTerrainSectorComponent::GetPropSectorIndexY() const
{
    return m_iPropSectorIndex[1];
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorComponent::SetPropSectorIndexY(
    NiInt16 iIndexY)
{
    if (m_iPropSectorIndex[1] != iIndexY)
    {
        m_iPropSectorIndex[1] = iIndexY;
        MarkSettingChanged();
        m_spSector->SetSectorIndex(m_iPropSectorIndex[0], 
            m_iPropSectorIndex[1]);
    }
}
//---------------------------------------------------------------------------
inline bool NiTerrainSectorComponent::HasSettingChanged() const
{
    return GetBit(PROP_SETTING_CHANGED);
}
//---------------------------------------------------------------------------
inline void NiTerrainSectorComponent::MarkSettingChanged()
{
    SetBit(true, PROP_SETTING_CHANGED);
}
