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
inline NiTerrain* NiTerrainComponent::GetTerrain()
{   
    return m_spTerrain;
}
//---------------------------------------------------------------------------
inline const NiTerrain* NiTerrainComponent::GetTerrain() const
{
    return m_spTerrain;
}
//---------------------------------------------------------------------------
inline NiSurfacePalette* NiTerrainComponent::GetActiveSurfacePalette() const
{
    return m_spTerrain->GetActiveSurfacePalette();
}
//---------------------------------------------------------------------------
inline const NiTPointerList<NiTerrainComponent*>* 
    NiTerrainComponent::GetTerrainComponents()
{
    return ms_pkTerrainComponents;
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiTerrainComponent::GetPropStorageFilename() const
{
    if (m_spMasterComponent && !GetBit(PROP_STORAGE_FILENAME_CHANGED))
    {
        return m_spMasterComponent->GetPropStorageFilename();
    }
    else
    {
        return m_kPropStorageFileName;
    }
}
//---------------------------------------------------------------------------
inline void NiTerrainComponent::SetPropStorageFilename(
    const NiFixedString& kFilename)
{
    if (GetPropStorageFilename() != kFilename)
    {
        m_kPropStorageFileName = kFilename;
        SetBit(true, PROP_STORAGE_FILENAME_CHANGED);
        SetBit(true, PROP_SETTING_CHANGED);
    }
}
//---------------------------------------------------------------------------
inline const NiTPrimitiveSet<NiUInt32>& NiTerrainComponent::
    GetPropSectorList() const
{
    return m_kSectorList;
}
//---------------------------------------------------------------------------
inline void NiTerrainComponent::SetPropSectorList(
    const NiTPrimitiveSet<NiUInt32>& kSectorList)
{
    m_kSectorList.RemoveAll();
    for (NiUInt32 uiIndex = 0; uiIndex < kSectorList.GetSize(); ++uiIndex)
    {
        m_kSectorList.Add(kSectorList.GetAt(uiIndex));
    }
    SetBit(true, PROP_SECTORS_CHANGED);
    SetBit(true, PROP_SETTING_CHANGED);
}
//---------------------------------------------------------------------------
inline bool NiTerrainComponent::SetPropSectorList(NiUInt32 uiIndex,
    const NiPoint2& kValue)
{
    // Make sure that this value is integral
    NiPoint2 kIndex;
    kIndex.x = NiFloor(kValue.x);
    kIndex.y = NiFloor(kValue.y);
    NiUInt32 uiValue = (((NiUInt32)kIndex.x) << 16) + (NiUInt16)kIndex.y;

    // Check that this value does not already exist
    if (m_kSectorList.Find(uiValue) != -1)
        return true;

    NiUInt32 uiOldSize = m_kSectorList.GetSize();
    if (uiIndex >= uiOldSize)
    {
        m_kSectorList.Realloc(uiIndex + 1);
        
        for (NiUInt32 uiNewIndex = uiOldSize; uiNewIndex < uiIndex + 1; 
            ++uiNewIndex)
        {
            m_kSectorList.Add(0);
        }
    }
    m_kSectorList.ReplaceAt(uiIndex, uiValue);
        
    SetBit(true, PROP_SECTORS_CHANGED);
    SetBit(true, PROP_SETTING_CHANGED);

    return true;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainComponent::GetPropSectorsX() const
{
    if (m_spMasterComponent && !GetBit(PROP_SECTORS_CHANGED))
    {
        return m_spMasterComponent->GetPropSectorsX();
    }
    else
    {
        return m_auiPropSectors[0];
    }
}
//---------------------------------------------------------------------------
inline void NiTerrainComponent::SetPropSectorsX(NiUInt32 uiSectorsX)
{
    if (m_auiPropSectors[0] != uiSectorsX)
    {
        m_auiPropSectors[0] = uiSectorsX;
        SetBit(true, PROP_SECTORS_CHANGED);
        SetBit(true, PROP_SETTING_CHANGED);
    }
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainComponent::GetPropSectorsY() const
{
    if (m_spMasterComponent && !GetBit(PROP_SECTORS_CHANGED))
    {
        return m_spMasterComponent->GetPropSectorsY();
    }
    else
    {
        return m_auiPropSectors[1];
    }
}
//---------------------------------------------------------------------------
inline void NiTerrainComponent::SetPropSectorsY(NiUInt32 uiSectorsY)
{
    if (m_auiPropSectors[1] != uiSectorsY)
    {
        m_auiPropSectors[1] = uiSectorsY;
        SetBit(true, PROP_SECTORS_CHANGED);
        SetBit(true, PROP_SETTING_CHANGED);
    }
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainComponent::GetPropBlockSize() const
{
    return m_uiPropBlockSize;
}
//---------------------------------------------------------------------------
inline bool NiTerrainComponent::SetPropBlockSize(NiUInt32 uiBlockSize)
{
    if (uiBlockSize != m_uiPropBlockSize)
    {
        // If we have a valid terrain do not allow the user to set the
        // block size.
        if (GetNumLoadedSectors() > 0)
        {
            NiMessageBox("Can not modify the block size after the terrain "
                "has been initialized.", 
                "Failed to set block size");
            return false;
        }

        // Make sure it is a valid block size
        if (NiIsPowerOf2(uiBlockSize))
        {
            m_uiPropBlockSize = uiBlockSize;
            SetBit(true, PROP_BLOCKSIZE_CHANGED);
            SetBit(true, PROP_SETTING_CHANGED);
        }
        else
        {
            NiMessageBox("Failed to set block size. Block size has to be a"
                "power of 2.", 
                "Failed to set block size");
            return false;
        }
    }
    return true;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainComponent::GetPropNumLOD() const
{
    return m_uiPropNumLOD;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainComponent::GetPropMaskSize() const
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
inline NiInt32 NiTerrainComponent::GetPropTerrainMinHeight() const
{
    if (m_spMasterComponent && !GetBit(PROP_MIN_ELEVATION_CHANGED))
        return m_spMasterComponent->GetPropTerrainMinHeight();
    else
        return m_iTerrainMinHeight;
         
}
//---------------------------------------------------------------------------
inline NiInt32 NiTerrainComponent::GetPropTerrainMaxHeight() const
{
    if (m_spMasterComponent && !GetBit(PROP_MAX_ELEVATION_CHANGED))
        return m_spMasterComponent->GetPropTerrainMaxHeight();
    else
        return m_iTerrainMaxHeight;
}
//---------------------------------------------------------------------------
inline bool NiTerrainComponent::SetPropNumLOD(NiUInt32 uiNumLOD)
{    
    if (uiNumLOD != m_uiPropNumLOD)
    {
        // If we have a valid terrain do not allow the user to set the
        // number of LODs.
        if (GetNumLoadedSectors() > 0)
        {
            NiMessageBox("Can not modify the number of LODs after the terrain "
                "has been initialized.", 
                "NiTerrainComponent::SetPropNumLOD");
            return false;
        }

        m_uiPropNumLOD = uiNumLOD;
    }

    return true;
}
//---------------------------------------------------------------------------
inline bool NiTerrainComponent::SetPropMaskSize(NiUInt32 uiSize)
{
    if (!NiIsPowerOf2(uiSize))
        return false;

    if (GetPropMaskSize() != uiSize)
    {
        m_uiPropMaskSize = uiSize;

        SetBit(true, PROP_MASK_SIZE_CHANGED);
        SetBit(true, PROP_SETTING_CHANGED);
    }
    
    return true;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainComponent::GetPropCalcSectorSize() const
{
    return (GetPropBlockSize() << m_uiPropNumLOD) + 1;
}
//---------------------------------------------------------------------------
inline bool NiTerrainComponent::HasSettingChanged() const
{
    return GetBit(PROP_SETTING_CHANGED);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrainComponent::GetNumLoadedSectors() const
{
    return m_spTerrain->GetNumLoadedSectors();
}
//---------------------------------------------------------------------------
inline bool NiTerrainComponent::GetPropCookPhysXData() const
{
    return m_bCookPhysXData;
}
//---------------------------------------------------------------------------
inline bool NiTerrainComponent::SetPropCookPhysXData(bool bNewValue)
{
    m_bCookPhysXData = bNewValue;
    SetBit(true, PROP_COOKPHYSX_CHANGED);
    SetBit(true, PROP_SETTING_CHANGED);
    return true;
}
//---------------------------------------------------------------------------
