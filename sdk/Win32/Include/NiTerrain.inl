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
inline void NiTerrain::SetSurfacePalette(
    NiSurfacePalette* spSurfacePalette)
{
    m_spCustomSurfacePalette = spSurfacePalette;
}
//---------------------------------------------------------------------------
inline NiSurfacePalette* NiTerrain::GetSurfacePalette() 
    const
{
    return m_spCustomSurfacePalette;
}
//---------------------------------------------------------------------------
inline NiSurfacePalette* NiTerrain::GetActiveSurfacePalette() 
    const
{
    if (m_spCustomSurfacePalette)
    {
        return m_spCustomSurfacePalette;
    }
    else
    {
        return GetStaticSurfacePalette();
    }
}
//---------------------------------------------------------------------------
inline NiSurfacePalette* NiTerrain::GetStaticSurfacePalette() 
{
    return NiSurfacePalette::GetInstance();
}
//---------------------------------------------------------------------------
inline const NiFixedString& NiTerrain::GetArchivePath() const
{
    return m_kArchivePath;
}
//---------------------------------------------------------------------------
inline void NiTerrain::SetArchivePath(
    const NiFixedString& kArchivePath)
{
    if (m_kArchivePath != kArchivePath)
    {
        m_kArchivePath = kArchivePath;
        SetBit(true, PROP_STORAGE_FILENAME_CHANGED);
        SetBit(true, SETTING_CHANGED);
    }
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrain::GetSectorsX() const
{
    return m_auiSectors[0];
}
//---------------------------------------------------------------------------
inline void NiTerrain::SetSectorsX(NiUInt32 uiSectorsX)
{
    if (m_auiSectors[0] != uiSectorsX)
    {
        m_auiSectors[0] = uiSectorsX;
        SetBit(true, PROP_SECTORS_CHANGED);
        SetBit(true, SETTING_CHANGED);
    }
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrain::GetSectorsY() const
{
    return m_auiSectors[1];
}
//---------------------------------------------------------------------------
inline void NiTerrain::SetSectorsY(NiUInt32 uiSectorsY)
{
    if (m_auiSectors[1] != uiSectorsY)
    {
        m_auiSectors[1] = uiSectorsY;
        SetBit(true, PROP_SECTORS_CHANGED);
        SetBit(true, SETTING_CHANGED);
    }
}
//---------------------------------------------------------------------------
inline bool NiTerrain::HasShapeChangedLastUpdate()
{
    return m_bHasShapeChangedLastUpdate;
}
//---------------------------------------------------------------------------
inline void NiTerrain::SetShapeChangedLastUpdate(bool bChanged)
{
    m_bHasShapeChangedLastUpdate = bChanged;
}
//---------------------------------------------------------------------------
inline void NiTerrain::SetLODScale(float fScale)
{
    // See NiTerrainSectorData for derivation of minimum scale
    static const float fMinimumScale = 2.0f * NiSqrt(2.0);

    if (fScale >= 1.0f)
    {
        m_fLODScale = fScale * fMinimumScale;
        m_fLODShift = 0;
    }
    else
    {
        m_fLODScale = fMinimumScale;
        m_fLODShift = (fScale - 1) * NiPow(2,(float)GetNumLOD()) * m_fLODScale
            * NiSqrt(2.0f) * float(GetBlockSize()) ;
    }

    SetBit(true, SETTING_CHANGED);
}
//---------------------------------------------------------------------------
inline float NiTerrain::GetLODScale() const
{
    return m_fLODScale;
}
//---------------------------------------------------------------------------
inline float NiTerrain::GetLODShift() const
{
    return m_fLODShift;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrain::GetLODMode() const
{
    return m_uiLODMode;
}
//---------------------------------------------------------------------------
inline bool NiTerrain::SetLODMode(NiUInt32 uiLODMode)
{
    if ((uiLODMode & (~NiTerrainSectorData::LOD_MORPH_ENABLE))
        < NiTerrainSectorData::NUM_LOD_MODES)
    {
        m_uiLODMode = uiLODMode;
        SetBit(true, SETTING_CHANGED);
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
inline NiMetaDataStore* NiTerrain::GetMetaDataStore() const
{
    return m_pkMetaDataStore;
}
//---------------------------------------------------------------------------
inline void NiTerrain::SetMetaDataStore(
    NiMetaDataStore* pkMetaDataStore
    )
{
    if (pkMetaDataStore != m_pkMetaDataStore)
    {
        m_pkMetaDataStore = pkMetaDataStore;
        SetBit(true, SETTING_CHANGED);
    }
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrain::GetBlockSize() const
{
    return m_uiBlockSize;
}
//---------------------------------------------------------------------------
inline bool NiTerrain::SetBlockSize(NiUInt32 uiBlockSize)
{
    if (uiBlockSize != m_uiBlockSize)
    {
		// Make sure it is a valid block size
		if (NiIsPowerOf2(uiBlockSize))
		{
			m_uiBlockSize = uiBlockSize;
            SetBit(true, PROP_BLOCKSIZE_CHANGED);
            SetBit(true, SETTING_CHANGED);
		}
		else
		{
			return false;
		}
    }
    return true;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrain::GetNumLOD() const
{
    return m_uiNumLOD;
}
//---------------------------------------------------------------------------
inline bool NiTerrain::SetNumLOD(NiUInt32 uiNumLOD)
{
    if (uiNumLOD != m_uiNumLOD)
    {
		m_uiNumLOD = uiNumLOD;
        SetBit(true, PROP_NUMLOD_CHANGED);
        SetBit(true, SETTING_CHANGED);
    }

    return true;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrain::GetDefaultMaskSize() const
{
    return m_uiMaskSize;
}
//---------------------------------------------------------------------------
inline bool NiTerrain::SetDefaultMaskSize(NiUInt32 uiMaskSize)
{
    if (uiMaskSize != m_uiMaskSize)
    {
        m_uiMaskSize = uiMaskSize;
        SetBit(true, SETTING_CHANGED);
    }
    return true;
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrain::GetCalcSectorSize() const
{
    return (GetBlockSize() << m_uiNumLOD) + 1;
}
//---------------------------------------------------------------------------
inline bool NiTerrain::HasSettingChanged() const
{
    return GetBit(SETTING_CHANGED);
}
//---------------------------------------------------------------------------
inline void NiTerrain::RemoveSector(
    NiTerrainSector* pkSector)
{
    NiInt16 sIndexX, sIndexY;
    pkSector->GetSectorIndex(sIndexX, sIndexY);
    NiUInt32 ulIndex = ((NiUInt32)sIndexX << 16) + (NiUInt16)sIndexY;

    m_kSectors.RemoveAt(ulIndex);

    // Detach this sector from the root node:
    DetachChild(pkSector);
}
//---------------------------------------------------------------------------
inline void NiTerrain::AddSector(NiTerrainSector* pkSector)
{
    NiInt16 sIndexX, sIndexY;
    pkSector->GetSectorIndex(sIndexX, sIndexY);
    NiUInt32 ulIndex = ((NiUInt32)sIndexX << 16) + (NiUInt16)sIndexY;

    // Disable multiple sector support
    if (ulIndex)
    {
        NIASSERT(!"Multiple sector terrains are not currently supported");
        return;
    }

    // Check for a previous sector at this location
    NiTerrainSector* pkOldSector = 0;
    m_kSectors.GetAt(ulIndex, pkOldSector);
    if (pkOldSector)
        RemoveSector(pkOldSector);

    // Assign the current sector
    m_kSectors.SetAt(ulIndex, pkSector);

    // Figure out the name of this sector:
    NiString sectorName;
    sectorName = "Sector_";
    sectorName += NiString::FromInt(sIndexX);
    sectorName += "_";
    sectorName += NiString::FromInt(sIndexY);
    pkSector->SetName(NiFixedString(sectorName));

    // Attach this sector to the root node:
    AttachChild(pkSector);

    // Push this terrain's properties to this sector
    UpdateSector(pkSector);
}
//---------------------------------------------------------------------------
inline NiTerrainSector* NiTerrain::GetSector(NiInt16 iSectorX, 
    NiInt16 iSectorY)
{
    NiUInt32 ulIndex = ((NiUInt32)iSectorX << 16) + (NiUInt16)iSectorY;
    NiTerrainSector* pkResult = NULL;
    m_kSectors.GetAt(ulIndex, pkResult);

    return pkResult;
}
//---------------------------------------------------------------------------
inline NiTerrainSector* NiTerrain::CreateSector(NiInt16 iSectorX, 
    NiInt16 iSectorY)
{
    NiTerrainSector* pkSector = NiNew NiTerrainSector(false);

    pkSector->SetSectorIndex(iSectorX, iSectorY);
    pkSector->SetDefaultMaskSize(64);
    pkSector->SetIsDeformable(false);

    // Set the transformation of this sector
    float fSectorWidth = (float)(GetBlockSize() << GetNumLOD());
    NiPoint3 kTranslation(iSectorX * fSectorWidth,iSectorY * fSectorWidth,0);
    pkSector->SetTranslate(kTranslation);
        
    // Tell this sector that it is a part of this terrain (causes this
    // terrain's data to be sent to it and is added to the list of sectors)
    pkSector->SetTerrain(this);

    return pkSector;
} 
//---------------------------------------------------------------------------
inline void NiTerrain::UseSectorList(const NiTPrimitiveSet<NiUInt32>& 
    kSectorList)
{
    for (NiUInt32 uiIndex = 0; uiIndex < kSectorList.GetSize(); 
        ++uiIndex)
    {
        NiUInt32 uiValue = kSectorList.GetAt(uiIndex);
        NiInt16 iValueX = (NiInt16)(uiValue >> 16);
        NiInt16 iValueY = (NiInt16)(uiValue & 0x0000FFFF);
        if (!GetSector(iValueX, iValueY))
        {
            CreateSector(iValueX, iValueY);
        }
    }
    if (kSectorList.GetSize() < m_kSectors.GetCount())
    {
        NiTerrainSector* pkSector = NULL;
        NiUInt32 uiIndex;
        NiTMapIterator kIterator = m_kSectors.GetFirstPos();
        while (kIterator)
        {
            m_kSectors.GetNext(kIterator, uiIndex, pkSector);
            if (kSectorList.Find(uiIndex) == -1)
            {
                RemoveSector(pkSector);
            }
        }
    }
}
//---------------------------------------------------------------------------
inline void NiTerrain::UpdateSector(NiTerrainSector *pkSector)
{
    // Update all the sectors with the latest changes to the terrain
    // Archive file
    pkSector->SetArchivePath(m_kArchivePath);

    // Metadata
    pkSector->SetMetaDataStore(m_pkMetaDataStore);

    // Surface Palette
    pkSector->SetSurfacePalette(GetActiveSurfacePalette());

    // Blocksize/NumLOD
    if (m_uiNumLOD != pkSector->GetSectorData()->GetNumLOD() ||
        m_uiBlockSize != pkSector->GetSectorData()->GetBlockSize())
    {
        // The user changed the dimensions of the terrain
        pkSector->ReformatData(m_uiBlockSize, m_uiNumLOD);
    }

    // LOD Scale
    pkSector->SetLODScale(GetLODScale(), GetLODShift());

    // LOD Mode
    pkSector->SetLODMode(GetLODMode());

    // Mask size
    pkSector->SetDefaultMaskSize(m_uiMaskSize);
}
//---------------------------------------------------------------------------
inline NiUInt32 NiTerrain::GetNumLoadedSectors() const
{
    return m_kSectors.GetCount();
}
//---------------------------------------------------------------------------
inline NiTerrainInteractor* NiTerrain::GetInteractor()
{
    return m_pkTerrainInteractor;
}
//---------------------------------------------------------------------------
