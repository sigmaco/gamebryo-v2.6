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

#include "NiTerrainPCH.h"

#include "NiTerrain.h"
#include "NiTerrainSectorComponent.h"
#include "NiSurfaceMaskPixel.h"
#include "NiTerrainInteractor.h"
#include "NiTerrainConfiguration.h"

#include <NiFixedString.h>
#include <NiFactories.h>

NiFogPropertyPtr NiTerrain::ms_spFogProperty = NULL;
NiTerrainConfiguration NiTerrain::ms_kDefaultConfiguration;

NiImplementRTTI(NiTerrain, NiNode);
//---------------------------------------------------------------------------
NiTerrain::NiTerrain(NiMetaDataStore* pkMetaDataStore) :
    m_pkMetaDataStore(pkMetaDataStore),
    m_spCustomSurfacePalette(0),
    m_bHasShapeChangedLastUpdate(false),
    m_kArchivePath(),
    m_uiBlockSize(32),
    m_uiNumLOD(5),    
    m_fLODScale(NiSqrt(2.0f) * 2.0f),
    m_fLODShift(0.0f),
    m_uiLODMode(NiTerrainSectorData::LOD_MODE_2D |
        NiTerrainSectorData::LOD_MORPH_ENABLE),
    m_pkTerrainInteractor(0),
    m_kSectors(31)
{
    // Initial sector map size of 31, leaning toward optimal situation with 3^2
    // loaded sector. ie, 2 sectors in all directions around the active sector

    m_auiSectors[0] = 1;
    m_auiSectors[1] = 1;

    NiMetaDataStore::GetInstance();

    Initialize();
}
//---------------------------------------------------------------------------
void NiTerrain::Initialize()
{
    if (m_pkMetaDataStore == 0) 
    {
        m_pkMetaDataStore = NiMetaDataStore::GetInstance();
    }

    // Initial pool size
    m_pkTerrainInteractor = NiNew NiTerrainInteractor(2000);
    m_pkTerrainInteractor->SetTerrain(this);
}
//---------------------------------------------------------------------------
NiTerrain::~NiTerrain() 
{
    m_spCustomSurfacePalette = 0;
    m_kSectors.RemoveAll();

    NiDelete m_pkTerrainInteractor;
}
//---------------------------------------------------------------------------
void NiTerrain::_SDMInit()
{
    ms_spFogProperty = NiNew NiFogProperty();    
    ms_spFogProperty->SetFogFunction(NiFogProperty::FOG_RANGE_SQ);
    ms_spFogProperty->SetFogColor(NiColor(1.0f, 1.0f, 1.0f));
    ms_spFogProperty->SetFog(true);
    ms_spFogProperty->SetDepth(0.95f);
}
//---------------------------------------------------------------------------
void NiTerrain::_SDMShutdown()
{
    ms_spFogProperty = NULL;
}
//---------------------------------------------------------------------------
NiFogProperty* NiTerrain::GetGlobalFogProperty()
{
    return ms_spFogProperty;
}
//---------------------------------------------------------------------------
void NiTerrain::SetGlobalFogProperty(NiFogProperty* pkFogProp)
{
    NIASSERT(pkFogProp);
    ms_spFogProperty = pkFogProp;
}
//---------------------------------------------------------------------------
const NiFixedString NiTerrain::CreateSurfacePath(
    const NiFixedString& kIndexPath)
{
    NiString kTerrainPath = kIndexPath;
    NiFixedString kSurfaceRoot;

#ifndef _PS3
    kSurfaceRoot = kTerrainPath + "\\..\\";
#else
    kSurfaceRoot = kTerrainPath + "/../";
#endif
    
    if (NiPath::IsRelative(kSurfaceRoot))
    {
        char* pcAbsolutePath = NiAlloc(char, 512);
        NiPath::ConvertToAbsolute(
            pcAbsolutePath, 512,
            kSurfaceRoot, 0);

        kSurfaceRoot = NiFixedString(pcAbsolutePath);
        NiFree(pcAbsolutePath);
    }

    return kSurfaceRoot;
}
//---------------------------------------------------------------------------
bool NiTerrain::Save(const char* pcFileName, NiUInt32* puiErrorCode)
{
    bool bRes = true;
    
    NiTListIterator kIterator = m_kSectors.GetFirstPos();
    while (kIterator)
    {
        NiTerrainSector* pkSector = NULL;
        NiUInt32 ulIndex;
        m_kSectors.GetNext(kIterator, ulIndex, pkSector);

        // Attempt to save all sectors, regardless of the other ones failures
        bRes &= pkSector->Save(pcFileName, puiErrorCode);
    }

    return bRes;
}
//---------------------------------------------------------------------------
bool NiTerrain::Load(const char* pcFileName, NiUInt32* puiErrorCode)
{
    bool bRes = true;
    
    NiTListIterator kIterator = m_kSectors.GetFirstPos();
    while (kIterator)
    {
        NiTerrainSector* pkSector = NULL;
        NiUInt32 ulIndex;
        m_kSectors.GetNext(kIterator, ulIndex, pkSector);

        // Attempt to save all sectors, regardless of the other ones failures
        bRes &= pkSector->Load(pcFileName, puiErrorCode);
    }

    return bRes;
}
//---------------------------------------------------------------------------
void NiTerrain::Unload()
{
    NiTMapIterator kIterator = m_kSectors.GetFirstPos();
    while (kIterator)
    {
        NiTerrainSector* pkSector = NULL;
        NiUInt32 ulIndex;
        m_kSectors.GetNext(kIterator, ulIndex, pkSector);

        pkSector->Unload();
    }
}
//---------------------------------------------------------------------------
void NiTerrain::OnVisible(NiCullingProcess& kCuller)
{
    NiTerrainSector* pkSector = NULL;
    NiUInt32 ulIndex;

    // Perform lodding
    NiTListIterator kIterator = m_kSectors.GetFirstPos();
    while (kIterator)
    {
        m_kSectors.GetNext(kIterator, ulIndex, pkSector);

        if (pkSector->ProcessLOD(kCuller))
        {
            pkSector->ProcessDecals(kCuller);
        }
    }

    // Perform stitching of sectors
    kIterator = m_kSectors.GetFirstPos();
    while (kIterator)
    {
        m_kSectors.GetNext(kIterator, ulIndex, pkSector);
        pkSector->ProcessBorders();
    }
}
//---------------------------------------------------------------------------
void NiTerrain::UpdateDownwardPass(NiUpdateProcess& kUpdate)
{
    DoUpdate();
    NiNode::UpdateDownwardPass(kUpdate);
}
//---------------------------------------------------------------------------
void NiTerrain::UpdateSelectedDownwardPass(NiUpdateProcess& kUpdate)
{
    if (GetSelectiveUpdateTransforms())
    {
        DoUpdate();
    }
    NiNode::UpdateSelectedDownwardPass(kUpdate);
}
//---------------------------------------------------------------------------
void NiTerrain::UpdateRigidDownwardPass(NiUpdateProcess& kUpdate)
{
    if (GetSelectiveUpdateTransforms())
    {
        DoUpdate();
    }
    NiNode::UpdateRigidDownwardPass(kUpdate);
}
//---------------------------------------------------------------------------
void NiTerrain::DoUpdate(bool bUpdateWorldData) 
{
    // Set shape changed to false now (If a sector has changed it will
    // set it to true after this when we propagate the update)
    SetShapeChangedLastUpdate(false);

    // Update any sectors that may require updating
    if (HasSettingChanged())
    {       
        NiTListIterator kIterator = m_kSectors.GetFirstPos();
        while (kIterator)
        {
            NiTerrainSector* pkSector = NULL;
            NiUInt32 ulIndex;
            m_kSectors.GetNext(kIterator, ulIndex, pkSector);

            UpdateSector(pkSector);
            // Sector Update is called by the NiNode::Update code
            // DO NOT propagate the Update to children here! This
            // would create multiple update calls and break the update
            // order when using the NiEntity/Component system.
        }

        SetBit(false, SETTING_CHANGED);
    }

    if (bUpdateWorldData)
    {
        NiAVObject::UpdateWorldData();
    }
}
//---------------------------------------------------------------------------
void NiTerrain::ModifyVertexHeightFrom(NiTerrainSector* pkSector, 
        const NiTerrainVertex& kVertex, float fNewHeight, 
        bool bModifyParentLOD)
{
    pkSector->ModifyVertexHeightFrom(
        kVertex, kVertex.GetFirstContainingLeaf(), 
        fNewHeight,
        bModifyParentLOD);

    // Check if we are at the border of a sector:
    NiIndex kIndex = kVertex.GetIndex();
    NiUInt32 uiSectorSize = pkSector->GetSectorData()->GetSectorSize();
    if (kIndex.x == 0 || kIndex.x == uiSectorSize ||
        kIndex.y == 0 || kIndex.y == uiSectorSize)
    {
        NiTerrainSector* pkAdjacentSector = NULL;
        NiTerrainVertex kAdjacentVertex;
        NiIndex kAdjacentIndex;

        // Mark adjacent blocks in other sectors as changed too
        if (kIndex.x == 0)
        {
            // Modify sector's block on the Left border
            pkAdjacentSector = pkSector->GetAdjacentSector(
                NiTerrainDataLeaf::BORDER_LEFT);
            kAdjacentIndex = kIndex;
            kAdjacentIndex.x = uiSectorSize;
        } 
        else if (kIndex.x == uiSectorSize)
        {
            // Modify sector's block on the Right border
            pkAdjacentSector = pkSector->GetAdjacentSector(
                NiTerrainDataLeaf::BORDER_RIGHT);
            kAdjacentIndex = kIndex;
            kAdjacentIndex.x = 0;
        }
        if (pkAdjacentSector)
        {
            pkAdjacentSector->GetVertexAt(kAdjacentVertex, kAdjacentIndex);
            pkAdjacentSector->ModifyVertexHeightFrom(
                kAdjacentVertex, kAdjacentVertex.GetFirstContainingLeaf(), 
                fNewHeight,
                bModifyParentLOD);
            pkAdjacentSector = 0;
        }

        if (kIndex.y == 0)
        {
            // Modify sector's block on the Bottom border
            pkAdjacentSector = pkSector->GetAdjacentSector(
                NiTerrainDataLeaf::BORDER_BOTTOM);
            kAdjacentIndex = kIndex;
            kAdjacentIndex.y = uiSectorSize;
        }
        else if (kIndex.y == uiSectorSize)
        {
            // Modify sector's block on the Top border
            pkAdjacentSector = pkSector->GetAdjacentSector(
                NiTerrainDataLeaf::BORDER_TOP);
            kAdjacentIndex = kIndex;
            kAdjacentIndex.y = 0;
        }
        if (pkAdjacentSector)
        {
            pkAdjacentSector->GetVertexAt(kAdjacentVertex, kAdjacentIndex);
            pkAdjacentSector->ModifyVertexHeightFrom(
                kAdjacentVertex, kAdjacentVertex.GetFirstContainingLeaf(), 
                fNewHeight,
                bModifyParentLOD);
            pkAdjacentSector = 0;
        }

        // Corner Cases:
        if (kIndex.x == 0)
        {
            kAdjacentIndex.x = uiSectorSize;
            if (kIndex.y == 0)
            {
                // Modify sector's block on the LEFT - BOTTOM
                pkAdjacentSector = pkSector->GetAdjacentSector(
                    NiTerrainDataLeaf::BORDER_LEFT |
                    NiTerrainDataLeaf::BORDER_BOTTOM);
                kAdjacentIndex.y = uiSectorSize;
            }
            else if (kIndex.y == uiSectorSize)
            {
                // Modify sector's block on the LEFT - TOP
                pkAdjacentSector = pkSector->GetAdjacentSector(
                    NiTerrainDataLeaf::BORDER_LEFT |
                    NiTerrainDataLeaf::BORDER_TOP);              
                kAdjacentIndex.y = 0;
            }
        }else if (kIndex.x == uiSectorSize)
        {
            kAdjacentIndex.x = 0;
            if (kIndex.y == 0)
            {
                // Modify sector's block on the RIGHT - BOTTOM
                pkAdjacentSector = pkSector->GetAdjacentSector(
                    NiTerrainDataLeaf::BORDER_RIGHT |
                    NiTerrainDataLeaf::BORDER_BOTTOM);              
                kAdjacentIndex.y = uiSectorSize;
            }
            else if (kIndex.y == uiSectorSize)
            {
                // Modify sector's block on the RIGHT - TOP
                pkAdjacentSector = pkSector->GetAdjacentSector(
                    NiTerrainDataLeaf::BORDER_RIGHT |
                    NiTerrainDataLeaf::BORDER_TOP);              
                kAdjacentIndex.y = 0;
            }
        }
        if (pkAdjacentSector)
        {
            pkAdjacentSector->GetVertexAt(kAdjacentVertex, kAdjacentIndex);
            pkAdjacentSector->ModifyVertexHeightFrom(
                kAdjacentVertex, kAdjacentVertex.GetFirstContainingLeaf(), 
                fNewHeight,
                bModifyParentLOD);
            pkAdjacentSector = 0;
        }
    }
}
//---------------------------------------------------------------------------
const NiTerrainConfiguration NiTerrain::GetDefaultConfiguration()
{
    return ms_kDefaultConfiguration;
}
//---------------------------------------------------------------------------
void NiTerrain::SetDefaultConfiguration(NiTerrainConfiguration kConfig)
{
    bool bValid = true;

    // We currently only support one valid configuration for the other platforms
    // which has all the flags set to true.
#ifndef _WII
    if (!kConfig.IsMorphingDataEnabled() || 
        !kConfig.IsLightingDataCompressionEnabled() ||
        !kConfig.IsTangentDataEnabled())
    {
        bValid = false;
    }
#endif

    if (!kConfig.ValidateConfiguration())
        bValid = false;

    if (bValid)
        ms_kDefaultConfiguration = kConfig;
}
//---------------------------------------------------------------------------
const NiTerrainConfiguration NiTerrain::GetConfiguration() const
{
    return ms_kDefaultConfiguration;
}
//---------------------------------------------------------------------------