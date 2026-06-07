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

#include "NiTerrainComponent.h"
#include "NiTerrainSectorComponent.h"
#include "NiSurfaceMaskPixel.h"
#include "NiTerrainInteractor.h"

#include <NiFixedString.h>
#include <NiFactories.h>
#include <NiDefaultErrorHandler.h>

// Streaming
NiFactoryDeclarePropIntf(NiTerrainComponent);

// Class naming
NiFixedString NiTerrainComponent::ms_kClassName;
NiFixedString NiTerrainComponent::ms_kComponentName;

// Properties
NiFixedString NiTerrainComponent::ms_kPropStorageFileName;
NiFixedString NiTerrainComponent::ms_kPropSectors;
NiFixedString NiTerrainComponent::ms_kPropSectorList;
NiFixedString NiTerrainComponent::ms_kPropBlockSize;
NiFixedString NiTerrainComponent::ms_kPropNumLOD;
NiFixedString NiTerrainComponent::ms_kPropCalcSectorSize;
NiFixedString NiTerrainComponent::ms_kPropInteractor;
NiFixedString NiTerrainComponent::ms_kPropSceneRoot;
NiFixedString NiTerrainComponent::ms_kPropTerrainMinHeight;
NiFixedString NiTerrainComponent::ms_kPropTerrainMaxHeight;
NiFixedString NiTerrainComponent::ms_kPropMaskSize;
NiFixedString NiTerrainComponent::ms_kPropCookPhysXData;

// Sibling properties (Transformation)
NiFixedString NiTerrainComponent::ms_kPropTranslation;
NiFixedString NiTerrainComponent::ms_kPropRotation;
NiFixedString NiTerrainComponent::ms_kPropScale;

// Descriptions
NiFixedString NiTerrainComponent::ms_kDescStorageFileName;
NiFixedString NiTerrainComponent::ms_kDescSectorList;
NiFixedString NiTerrainComponent::ms_kDescBlockSize;
NiFixedString NiTerrainComponent::ms_kDescNumLOD;
NiFixedString NiTerrainComponent::ms_kDescCalcSectorSize;
NiFixedString NiTerrainComponent::ms_kDescTerrainMinHeight;
NiFixedString NiTerrainComponent::ms_kDescTerrainMaxHeight;
NiFixedString NiTerrainComponent::ms_kDescMaskSize;
NiFixedString NiTerrainComponent::ms_kDescCookPhysXData;

// Component Management
NiTPointerList<NiTerrainComponent*>* 
    NiTerrainComponent::ms_pkTerrainComponents = 0;

//---------------------------------------------------------------------------
NiTerrainComponent::NiTerrainComponent(NiMetaDataStore* pkMetaDataStore) :
    m_uFlags(0),
    m_spTerrain(0),
    m_spMasterComponent(0),
    m_kPropStorageFileName(),
    m_uiPropBlockSize(32),
    m_uiPropNumLOD(5),  
    m_uiPropMaskSize(64),
    m_kSectorList(0),
    m_iTerrainMinHeight(0),
    m_iTerrainMaxHeight(300),
    m_kUniqueTerrainID(),
    m_bCookPhysXData(false)
{
    m_spTerrain = NiNew NiTerrain(pkMetaDataStore);

    m_auiPropSectors[0] = 1;
    m_auiPropSectors[1] = 1;

    NiMetaDataStore::GetInstance();
    m_kSectorList.Add(0);

    Initialize();
}
//---------------------------------------------------------------------------
NiTerrainComponent::NiTerrainComponent(NiTerrainComponent* pkTemplateComponent,
    NiMetaDataStore* pkMetaDataStore) :
    m_uFlags(0),
    m_spTerrain(0),
    m_spMasterComponent(pkTemplateComponent),
    m_kPropStorageFileName(pkTemplateComponent->m_kPropStorageFileName),
    m_uiPropBlockSize(pkTemplateComponent->m_uiPropBlockSize),
    m_uiPropNumLOD(pkTemplateComponent->m_uiPropNumLOD),
    m_uiPropMaskSize(pkTemplateComponent->m_uiPropMaskSize),
    m_kSectorList(0),
    m_iTerrainMinHeight(pkTemplateComponent->m_iTerrainMinHeight),
    m_iTerrainMaxHeight(pkTemplateComponent->m_iTerrainMaxHeight),
    m_kUniqueTerrainID(),
    m_bCookPhysXData(false)
{
    m_spTerrain = NiNew NiTerrain(pkMetaDataStore);

    m_auiPropSectors[0] = pkTemplateComponent->GetPropSectorsX();
    m_auiPropSectors[1] = pkTemplateComponent->GetPropSectorsY();

    SetPropSectorList(pkTemplateComponent->m_kSectorList);

    Initialize();
}
//---------------------------------------------------------------------------
void NiTerrainComponent::Initialize()
{
    ms_pkTerrainComponents->AddTail(this);
}
//---------------------------------------------------------------------------
NiTerrainComponent::~NiTerrainComponent() 
{
    if (ms_pkTerrainComponents)
    {
        // In the scene designer, SDM_Shutdown gets called before all the 
        // components are deleted.
        ms_pkTerrainComponents->Remove(this);
    }    
}
//---------------------------------------------------------------------------
bool NiTerrainComponent::LoadTerrain(NiScene* pkScene, 
    NiExternalAssetManager* pkAssetManager,
    NiDefaultErrorHandler* pkError)
{
    bool bResult = false;

    unsigned int uiCount = pkScene->GetEntityCount();
    for(unsigned int uiEntity = 0; uiEntity < uiCount; uiEntity++)
    {
        NiEntityInterface* pkEntity = pkScene->GetEntityAt(uiEntity);

        NiTerrainComponent* pkComponent = 
            NiTerrainComponent::FindTerrainComponent(pkEntity);
        if (pkComponent)
        {
            if (!pkComponent->Load(NULL, pkError))
            {
                if (pkError)
                {
                    pkError->ReportError("Failed to load terrain "
                        "archive. Ensure archive location exists.",
                        "NiTerrainComponent::LoadTerrain", 
                        pkComponent->GetName(), NULL);
                }
                continue;
            }
            bResult = true;
        }
    }

    // Make sure we found a terrain component.
    if (!bResult)
    {
        return false;
    }

    // Get the global fog property from the terrain and tweak its values.
    NiFogProperty* pkTerrainFog = NiTerrain::GetGlobalFogProperty();
    NI_UNUSED_ARG(pkTerrainFog);
    NIASSERT(pkTerrainFog);

    // Apply a fog property to all entities.
    for (NiUInt32 ui = 0; ui < uiCount; ui++)
    {
        NiEntityInterface* pkEntity = pkScene->GetEntityAt(ui);
        NIASSERT(pkEntity);

        NiBool bPropDataResult;
        NiObject* pkObj = NULL;

        bPropDataResult = pkEntity->GetPropertyData("Scene Root Pointer",
            pkObj);
        if (NIBOOL_IS_TRUE(bPropDataResult))
        {
            NiAVObject* pkAVObj = NiDynamicCast(NiAVObject, pkObj);
            if (pkAVObj)            
            {
                NiFogProperty* pkFogProp = NiDynamicCast(NiFogProperty,
                    pkAVObj->GetProperty(NiFogProperty::GetType()));

                if (!pkFogProp)                
                {
                    pkAVObj->AttachProperty(
                        NiTerrain::GetGlobalFogProperty());
                }

                pkAVObj->UpdateProperties();
            }
        }
    }

    // Update the scene after we load to make sure any entities that depended
    // on the terrain are now up to date.
    for(unsigned int uiEntity=0; uiEntity< uiCount; uiEntity++)
    {
        NiEntityInterface* pkEntity = pkScene->GetEntityAt(uiEntity);
        pkEntity->Update(NULL, 0.0, pkError, pkAssetManager);
    }

    return bResult;
}
//---------------------------------------------------------------------------
void NiTerrainComponent::Remove(NiTerrainComponent* pkTerrainComponent)
{
    ms_pkTerrainComponents->Remove(pkTerrainComponent);
}
//---------------------------------------------------------------------------
void NiTerrainComponent::_SDMInit()
{
    ms_kClassName = "NiTerrainComponent";
    ms_kComponentName = "Terrain";

    // Properties
    ms_kPropStorageFileName = "Terrain Archive";
    ms_kPropSectors = "Number of Sectors";
    ms_kPropSectorList = "Sector List";
    ms_kPropBlockSize = "Block Side Length";
    ms_kPropNumLOD = "Number of block sub-divisions";
    ms_kPropCalcSectorSize = "Resulting sector size";
    ms_kPropInteractor = "Interactor";
    ms_kPropSceneRoot = "Scene Root Pointer";
    ms_kPropTerrainMinHeight = "Min. Elevation";
    ms_kPropTerrainMaxHeight = "Max. Elevation";
    ms_kPropMaskSize = "Default Surface Mask Size";
    ms_kPropCookPhysXData = "Cook PhysX Data";

    // Sibling properties (Transformation)
    ms_kPropTranslation = "Translation";
    ms_kPropRotation = "Rotation";
    ms_kPropScale = "Scale";

    // Descriptions
    ms_kDescStorageFileName = "The terrain archive to load the sector data "
        "from, relative to GSA file location.";
    ms_kDescSectorList = "The list of sector indices that this terrain controls. "
        "Each index must be integral and must be unique.";
    ms_kDescBlockSize = "The size of each block within the sector. MUST be a "
        "power of 2. Can not be modified after the terrain is initialized.";
    ms_kDescNumLOD = "The number of levels of detail, in addition to the base "
        "level. Can not be modified after the terrain is initialized.";
    ms_kDescCalcSectorSize = "The resulting side length, in vertices, of each "
        "sector according to the specified number of detail levels and size "
        "of each block";
    ms_kDescTerrainMinHeight = "Minimum height of the terrain.";
    ms_kDescTerrainMaxHeight = "Maximum height of the terrain.";
    ms_kDescMaskSize = "The side length (in pixels) of the opacity mask that "
        "will be created for each painted surface in this sector. Note, that "
        "changing this value will not affect existing masks on this sector.";
    ms_kDescCookPhysXData = "If enabled, and the Scene Designer PhysX plugin "
        "is installed, the terrain's geometry and metadata information will "
        "be exported to a file in the terrain's archive path for faster load "
        "times.";

    // Component Management
    ms_pkTerrainComponents = NiNew NiTPointerList<NiTerrainComponent*>;
}
//---------------------------------------------------------------------------
void NiTerrainComponent::_SDMShutdown()
{
    ms_kClassName = NULL;
    ms_kComponentName = NULL;

    // Properties
    ms_kPropStorageFileName = NULL;
    ms_kPropSectors = NULL;
    ms_kPropSectorList = NULL;
    ms_kPropBlockSize = NULL;
    ms_kPropNumLOD = NULL;
    ms_kPropCalcSectorSize = NULL;
    ms_kPropInteractor = NULL;
    ms_kPropSceneRoot = NULL;
    ms_kPropTerrainMinHeight = NULL;
    ms_kPropTerrainMaxHeight = NULL;
    ms_kPropMaskSize = NULL;
    ms_kPropCookPhysXData = NULL;

    // Sibling properties (Transformation)
    ms_kPropTranslation = NULL;
    ms_kPropRotation = NULL;
    ms_kPropScale = NULL;    

    // Descriptions
    ms_kDescStorageFileName = NULL;
    ms_kDescSectorList = NULL;
    ms_kDescBlockSize = NULL;
    ms_kDescNumLOD = NULL;
    ms_kDescCalcSectorSize = NULL;
    ms_kDescTerrainMinHeight = NULL;
    ms_kDescTerrainMaxHeight = NULL;
    ms_kDescMaskSize = NULL;
    ms_kDescCookPhysXData = NULL;

    // Component Management
    ms_pkTerrainComponents->RemoveAll();
    NiDelete(ms_pkTerrainComponents);
}
//---------------------------------------------------------------------------
void NiTerrainComponent::UnloadData()
{
    m_spTerrain->Unload();
}
//---------------------------------------------------------------------------
NiTerrainComponent* NiTerrainComponent::FindTerrainComponent(
    const NiEntityInterface* pkEntity)
{
    if (!pkEntity)
    {
        return NULL;
    }

    NiUInt32 uiComponents = pkEntity->GetComponentCount();

    for(NiUInt32 uiComp=0; uiComp < uiComponents; uiComp++)
    {
        NiEntityComponentInterface* pkComp = pkEntity->GetComponentAt(
            uiComp);

        if (pkComp == NULL)
            continue;

        NiFixedString kClassName = pkComp->GetClassName();
        if (pkComp->GetClassName() == NiTerrainComponent::ms_kClassName)
        {
            return (NiTerrainComponent*)pkComp;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
void NiTerrainComponent::GenerateErrorMessages(NiUInt32 uiErrorCode,
    NiEntityErrorInterface* pkError,
    bool bSaving)
{
    if (!pkError)
        return;

    NiFixedString kProperty;
    if (bSaving)
        kProperty = "Saving File";
    else
        kProperty = "Load from file";

    if (uiErrorCode & NiTerrainSector::EC_DOF_INVALID_FILE ||
        uiErrorCode & NiTerrainSector::EC_INVALID_ARCHIVE_PATH||
        uiErrorCode & NiTerrainSector::EC_DOF_MISSING_DATA)
    {
        if (!bSaving)
        {
            pkError->ReportError("The sector file is not valid.", 
                "Please make sure the quadtree.dof file exists and that the "
                "terrain properties (lod and block size) correspond to the "
                "file you are attempting to load.", "NiTerrain", 
                kProperty );
        }
        else
        {
            pkError->ReportError("Failed to open the file for writing.", 
                "Make sure the file is not read only", "NiTerrain", 
                kProperty );
        }
    }

    if (uiErrorCode & NiTerrainSector::EC_DOF_STREAM_CREATION_ERROR)
    {
        pkError->ReportError("Stream creation error.", 
            "Failed to create the terrain streams.", "NiTerrain", 
            kProperty);
    }

    if (uiErrorCode & NiTerrainSector::EC_DOF_INVALID_SECTOR_SIZE)
    {
        pkError->ReportError("Invalid sector", "The sector size does not "
            "correspond to the size defined in the file.", "NiTerrain", 
            kProperty );
    }

    if (uiErrorCode & NiTerrainSector::EC_DOF_INVALID_BOUNDS)
    {
        if (NiTerrainInteractor::InToolMode())
        {
            pkError->ReportError("Invalid bounds", "The sector file you are "
                "attempting to load does not correspond to the created "
                "terrain. The bounds for each block were invalid and have "
                "been rebuilt.", "NiTerrain", kProperty);
        }
        else
        {
            NiOutputDebugString("WARNING:\nThe quadtree.dof file represents a "
                "terrain different from the created terrain.\nThe bounds "
                "loaded from the file are therefore incorrect and have been "
                "recalculated.\nTo fix this issue, save the terrain again.\n");
        }
    }

    if (uiErrorCode & NiTerrainSector::EC_SURFACE_INVALID_PACKAGE ||
        uiErrorCode & NiTerrainSector::EC_SURFACE_EMPTY_PACKAGENAME ||
        uiErrorCode & NiTerrainSector::EC_SURFACE_FAILED_LOADSURFACE)
    {
        pkError->ReportError("Failed to load material mask", "The "
            "material file surface.xml has invalid content or could not be"
            "found. Check values point to the correct packages", "NiTerrain", 
            kProperty );
    }

    if (uiErrorCode & NiTerrainSector::EC_SURFACE_INVALID_INDEX ||
        uiErrorCode & NiTerrainSector::EC_SURFACE_FAILED_READ_MASK)
    {
        if (NiTerrainInteractor::InToolMode())
        {
            pkError->ReportError("Failed to load a material.", 
                "Make sure indexes are correct in the surfaces.xml file "
                "and the associated material masks exist.",
                "NiTerrain", kProperty);
        }
        else
        {
            NiOutputDebugString("WARNING:\nFailed to load a material.\n" 
                "Make sure indexes are correct in the surfaces.xml file "
                "and the associated material masks exist.\n");
        }
    }

    if (uiErrorCode & NiTerrainSector::EC_DOF_OUTOFDATE)
    {
        if (NiTerrainInteractor::InToolMode())
        {
            pkError->ReportError("The quadtree.dof file version is out of " 
                "date", "Save the file again to make sure the latest "
                "version is in use", "NiTerrain", kProperty );
        }
        else
        {
            NiOutputDebugString("WARNING:\nThe quadtree.dof file version "
                "is out of date.\nSave the file again to make sure the "
                "latest version is in use.\n");
        }
    }    

    if (uiErrorCode & NiTerrainSector::EC_SURFACE_INVALID_TERRAIN_DATA)
    {
        pkError->ReportError("Invalid terrain", 
            "The terrain being saved does not have any data.", "NiTerrain", 
            kProperty );
    }

    if (uiErrorCode & NiTerrainSector::EC_SURFACE_FAILED_DIRECTORY)
    {
        pkError->ReportError("Saving materials failed", 
            "Failed to create the directory hierarchy", "NiTerrain", 
            kProperty );
    }

    if (uiErrorCode & NiTerrainSector::EC_SURFACE_FAILED_SAVE)
    {
        pkError->ReportError("Material file failed to save", 
            "The material file surfaces.xml could not be saved. Make sure the "
            "file exist and is not read only.", 
            "NiTerrain", kProperty );
    }
}
//---------------------------------------------------------------------------
bool NiTerrainComponent::Save(const char* pcArchive, 
    NiEntityErrorInterface* pkError)
{
    NiUInt32 uiErrorCode = 0; 
    bool result = m_spTerrain->Save(pcArchive, &uiErrorCode);    

    if (pkError != 0)
    {
        GenerateErrorMessages(uiErrorCode, pkError, true);
    }

    return result;
}
//---------------------------------------------------------------------------
bool NiTerrainComponent::Load(const char* pcArchive, 
    NiEntityErrorInterface* pkError)
{
    NiUInt32 uiErrorCode = 0; 
    bool result = m_spTerrain->Load(pcArchive, &uiErrorCode);    

    if (pkError != 0)
    {
       GenerateErrorMessages(uiErrorCode, pkError);
    }

    return result;
}
//---------------------------------------------------------------------------
bool NiTerrainComponent::IsTemplate()
{
    return m_spMasterComponent == 0;
}
//---------------------------------------------------------------------------
NiFixedString NiTerrainComponent::GetUniqueComponentName()
{
    return m_kUniqueTerrainID;
}
//---------------------------------------------------------------------------
void NiTerrainComponent::Update(NiEntityPropertyInterface* pkParentEntity,
    float fTime, NiEntityErrorInterface*,
    NiExternalAssetManager*) 
{
    // Propagate translation details.
    NiPoint3 kTranslate;
    NiMatrix3 kRotate;
    float fScale;
    if (pkParentEntity->GetPropertyData(ms_kPropTranslation, kTranslate, 0))
    {
        pkParentEntity->GetPropertyData(ms_kPropRotation, kRotate, 0);
        pkParentEntity->GetPropertyData(ms_kPropScale, fScale, 0);

        m_spTerrain->SetTranslate(kTranslate);
        m_spTerrain->SetRotate(kRotate);
        m_spTerrain->SetScale(fScale);
    }

    // Unique name
    m_kUniqueTerrainID = pkParentEntity->GetName();
    m_spTerrain->SetName(m_kUniqueTerrainID);

    if (HasSettingChanged())
    {        
        // Push the changed settings to terrain        
        // Archive file name
        m_spTerrain->SetArchivePath(GetPropStorageFilename());

        // Blocksize/NumLOD
        m_spTerrain->SetBlockSize(GetPropBlockSize());
        m_spTerrain->SetNumLOD(GetPropNumLOD());

        // Check the sector list
        m_spTerrain->UseSectorList(m_kSectorList);

        m_spTerrain->SetDefaultMaskSize(m_uiPropMaskSize);

        SetBit(false, PROP_SETTING_CHANGED);
    }

    // Call update on the terrain
    m_spTerrain->Update(fTime);
}
//---------------------------------------------------------------------------
void NiTerrainComponent::BuildVisibleSet(
    NiEntityRenderingContext* pkRenderingContext,
    NiEntityErrorInterface*) 
{
    NiCullingProcess* pkCuller = 
	    pkRenderingContext->m_pkCullingProcess;	

    // Cull this sector's section of the scene graph:
    NiCullScene(pkRenderingContext->m_pkCamera,
	    m_spTerrain,
	    *pkCuller,
	    *pkCuller->GetVisibleSet(),
        false);
}
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiTerrainComponent::Clone(bool bInheritProperties)
{
    NiTerrainComponent* pkClone = 
        NiNew NiTerrainComponent(this, m_spTerrain->GetMetaDataStore());
    
    if (!bInheritProperties)
    {
        pkClone->SetMasterComponent(0);
        pkClone->m_uFlags = m_uFlags;
    }

    pkClone->SetBit(true, PROP_SETTING_CHANGED);
        
    return pkClone;
}
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiTerrainComponent::GetMasterComponent() const
{
    return m_spMasterComponent;
}
//---------------------------------------------------------------------------
void NiTerrainComponent::SetMasterComponent(
    NiEntityComponentInterface* pkMasterComponent)
{
    if (pkMasterComponent)
    {
        // We were given a master component
        NIASSERT(pkMasterComponent->GetClassName() == GetClassName());
        m_spMasterComponent = (NiTerrainComponent*)pkMasterComponent;
    }
    else
    {
        // We were told not to have a master component any more; ensure all our
        // properties are unique
        NiTObjectSet<NiFixedString> kPropertyNames(5);
        GetPropertyNames(kPropertyNames);
        NiUInt32 uiSize = kPropertyNames.GetSize();
        for (NiUInt32 ui = 0; ui < uiSize; ui++)
        {
            bool bMadeUnique;
            NIVERIFY(
                MakePropertyUnique(kPropertyNames.GetAt(ui), bMadeUnique));
        }
        m_spMasterComponent = NULL;
    }
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::IsAnimated() const 
{
    return false;
}
//---------------------------------------------------------------------------
void NiTerrainComponent::GetDependentPropertyNames(
    NiTObjectSet<NiFixedString>&) 
{
    // No dependant properties
    return;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::SetTemplateID(const NiUniqueID&) 
{
    // Not supported for custom components
    return false;
}
//---------------------------------------------------------------------------
NiUniqueID NiTerrainComponent::GetTemplateID() 
{
    static const NiUniqueID kUniqueID = 
        NiUniqueID(0x18,0x9F,0x24,0x14,0x50,0x8A,0x37,0x44,
        0xAB,0x18,0x27,0x2,0xA7,0x2A,0x4B,0xFD);
    return kUniqueID;
}
//---------------------------------------------------------------------------
void NiTerrainComponent::AddReference() 
{
    IncRefCount();
}
//---------------------------------------------------------------------------
void NiTerrainComponent::RemoveReference() 
{
    DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiTerrainComponent::GetClassName() const 
{
    return ms_kClassName;
}
//---------------------------------------------------------------------------
NiFixedString NiTerrainComponent::GetName() const 
{
    return ms_kComponentName;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::SetName(const NiFixedString&) 
{
    // This component does not allow its name to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::GetDisplayName(const NiFixedString& kPropertyName,
    NiFixedString& kDisplayName) const 
{
    if (kPropertyName == ms_kPropStorageFileName ||
        kPropertyName == ms_kPropBlockSize ||
        kPropertyName == ms_kPropNumLOD ||
        kPropertyName == ms_kPropCalcSectorSize ||
        kPropertyName == ms_kPropTerrainMinHeight ||
        kPropertyName == ms_kPropTerrainMaxHeight ||
        kPropertyName == ms_kPropMaskSize ||
        kPropertyName == ms_kPropCookPhysXData)
    {
        kDisplayName = kPropertyName;
    }
    else if (kPropertyName == ms_kPropInteractor ||
        kPropertyName == ms_kPropSceneRoot ||
        kPropertyName == ms_kPropSectorList ||
        kPropertyName == ms_kPropSectors)
    {
        kDisplayName = NiFixedString(0);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::SetDisplayName(const NiFixedString&,
    const NiFixedString&) 
{
    // This component does not allow the display name to be set.
    return false;
}
//---------------------------------------------------------------------------
void NiTerrainComponent::GetPropertyNames(
    NiTObjectSet<NiFixedString>& kPropertyNames) const 
{
    kPropertyNames.Add(ms_kPropStorageFileName);
    kPropertyNames.Add(ms_kPropSectorList);    
    kPropertyNames.Add(ms_kPropBlockSize);
    kPropertyNames.Add(ms_kPropNumLOD);
    kPropertyNames.Add(ms_kPropCalcSectorSize);
    kPropertyNames.Add(ms_kPropSceneRoot);    
    kPropertyNames.Add(ms_kPropTerrainMinHeight);
    kPropertyNames.Add(ms_kPropTerrainMaxHeight);
    kPropertyNames.Add(ms_kPropMaskSize);
    kPropertyNames.Add(ms_kPropCookPhysXData);
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::CanResetProperty(
    const NiFixedString& kPropertyName, bool& bCanReset) const 
{
    if (kPropertyName == ms_kPropStorageFileName)
    {
        bCanReset = m_spMasterComponent && GetBit(
            PROP_STORAGE_FILENAME_CHANGED);
    }
    else if (kPropertyName == ms_kPropSectors)
    {
        bCanReset = m_spMasterComponent && GetBit(PROP_SECTORS_CHANGED);
    }
    else if (kPropertyName == ms_kPropSectorList)
    {
        bCanReset = m_spMasterComponent && GetBit(PROP_SECTORS_CHANGED);
    }
    else if (kPropertyName == ms_kPropBlockSize ||
        kPropertyName == ms_kPropNumLOD)
    {
        // Do not inherit these properties as they will invalidate any 
        // data already loaded for the current terrain
        bCanReset = false;
    }    
    else if (kPropertyName == ms_kPropTerrainMinHeight)
    {
        bCanReset = m_spMasterComponent && GetBit(PROP_MIN_ELEVATION_CHANGED);
    }
    else if (kPropertyName == ms_kPropTerrainMaxHeight)
    {
        bCanReset = m_spMasterComponent && GetBit(PROP_MAX_ELEVATION_CHANGED);
    }
    else if (kPropertyName == ms_kPropSceneRoot ||
        kPropertyName == ms_kPropCalcSectorSize)
    {
        bCanReset = false;
    }
    else if (kPropertyName == ms_kPropMaskSize)
    {
        bCanReset = m_spMasterComponent && GetBit(PROP_MASK_SIZE_CHANGED);
    }
    else if (kPropertyName == ms_kPropCookPhysXData)
    {
        bCanReset = m_spMasterComponent && GetBit(PROP_COOKPHYSX_CHANGED);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::ResetProperty(const NiFixedString& kPropertyName) 
{
    bool bCanReset;
    if (!CanResetProperty(kPropertyName, bCanReset) || !bCanReset)
    {
        return false;
    }

    if (kPropertyName == ms_kPropStorageFileName)
    {
        SetPropStorageFilename(m_spMasterComponent->GetPropStorageFilename());
        SetBit(false, PROP_STORAGE_FILENAME_CHANGED);
    }
    else if (kPropertyName == ms_kPropSectors)
    {
        SetPropSectorsX(m_spMasterComponent->GetPropSectorsX());
        SetPropSectorsY(m_spMasterComponent->GetPropSectorsY());        
        SetBit(false, PROP_SECTORS_CHANGED);
    }
    else if (kPropertyName == ms_kPropSectorList)
    {
        SetPropSectorList(m_spMasterComponent->GetPropSectorList());
        SetBit(false, PROP_SECTORS_CHANGED);
}
    else if (kPropertyName == ms_kPropTerrainMinHeight)
    {
        SetPropTerrainMinHeight(
            m_spMasterComponent->GetPropTerrainMinHeight());
        SetBit(false, PROP_MIN_ELEVATION_CHANGED);
    }
    else if (kPropertyName == ms_kPropTerrainMaxHeight)
    {
        SetPropTerrainMaxHeight(
            m_spMasterComponent->GetPropTerrainMaxHeight());

        SetBit(false, PROP_MAX_ELEVATION_CHANGED);
    }
    else if (kPropertyName == ms_kPropMaskSize)
    {
        SetPropMaskSize(
            m_spMasterComponent->GetPropMaskSize());
        SetBit(false, PROP_MASK_SIZE_CHANGED);
    }
    else if (kPropertyName == ms_kPropCookPhysXData)
    {
        SetPropCookPhysXData(m_spMasterComponent->GetPropCookPhysXData());
        SetBit(false, PROP_COOKPHYSX_CHANGED);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::MakePropertyUnique(
    const NiFixedString& kPropertyName, bool& bMadeUnique) 
{
    bool bCanReset;
    if (!CanResetProperty(kPropertyName, bCanReset))
    {
        return false;
    }

    if (!bCanReset && m_spMasterComponent)
    {
        bMadeUnique = true;
        if (kPropertyName == ms_kPropStorageFileName)
        {
            SetPropStorageFilename(GetPropStorageFilename());
        }
        else if (kPropertyName == ms_kPropSectors)
        {
            SetPropSectorsX(GetPropSectorsX());
            SetPropSectorsY(GetPropSectorsY());
        }
        else if (kPropertyName == ms_kPropSectorList)
        {
            // Do not follow the same pattern here because the list
            // would be deleted. 
            SetBit(true, PROP_SECTORS_CHANGED);
        }
        else if (kPropertyName == ms_kPropTerrainMinHeight)
        {
            SetPropTerrainMinHeight(GetPropTerrainMinHeight());
        }
        else if (kPropertyName == ms_kPropTerrainMaxHeight)
        {
            SetPropTerrainMaxHeight(GetPropTerrainMaxHeight());
        }
        else if (kPropertyName == ms_kPropMaskSize)
        {
            SetPropMaskSize(GetPropMaskSize());
        }
        else if (kPropertyName == ms_kPropCookPhysXData)
        {   
            SetPropCookPhysXData(GetPropCookPhysXData());
        }
        else
        {
            bMadeUnique = false;
        }
    }
    else
    {
        bMadeUnique = false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::GetPrimitiveType(const NiFixedString& kPropertyName,
    NiFixedString& kPrimitiveType) const 
{
    if (kPropertyName == ms_kPropStorageFileName)
    {
        kPrimitiveType = PT_STRING;
    }
    else if (kPropertyName == ms_kPropSectors ||
        kPropertyName == ms_kPropBlockSize ||
        kPropertyName == ms_kPropNumLOD ||
        kPropertyName == ms_kPropCalcSectorSize ||
        kPropertyName == ms_kPropMaskSize)
    {
        kPrimitiveType = PT_UINT;
    }
    else if (kPropertyName == ms_kPropSectorList)
    {
        kPrimitiveType = PT_POINT2;
    }
    else if (kPropertyName == ms_kPropSceneRoot)
	{
		kPrimitiveType = PT_NIOBJECTPOINTER;
	}
    else if (kPropertyName == ms_kPropTerrainMinHeight)
    {
        kPrimitiveType = PT_INT;
    }
    else if (kPropertyName == ms_kPropTerrainMaxHeight)
    {
        kPrimitiveType = PT_INT;
    }
    else if (kPropertyName == ms_kPropCookPhysXData)
    {
        kPrimitiveType = PT_BOOL;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::SetPrimitiveType(const NiFixedString&,
    const NiFixedString&) 
{    
    // Do not allow the setting of primitive types
    return false;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::GetSemanticType(const NiFixedString& kPropertyName,
    NiFixedString& kSemanticType) const 
{
    if (kPropertyName == ms_kPropStorageFileName)
    {
        kSemanticType = "Directory Path";
    }
    else if (kPropertyName == ms_kPropSectors ||
        kPropertyName == ms_kPropBlockSize ||
        kPropertyName == ms_kPropNumLOD ||
        kPropertyName == ms_kPropCalcSectorSize ||
        kPropertyName == ms_kPropMaskSize)
    {
        kSemanticType = PT_UINT;
    }
    else if (kPropertyName == ms_kPropSectorList)
    {
        kSemanticType = PT_POINT2;
    }    
    else if (kPropertyName == ms_kPropSceneRoot)
	{
		kSemanticType = PT_NIOBJECTPOINTER;
	}
    else if (kPropertyName == ms_kPropTerrainMinHeight)
    {
        kSemanticType = PT_INT;
    }
    else if (kPropertyName == ms_kPropTerrainMaxHeight)
    {
        kSemanticType = PT_INT;
    }
    else if (kPropertyName == ms_kPropCookPhysXData)
    {
        kSemanticType = PT_BOOL;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::SetSemanticType(const NiFixedString&,
    const NiFixedString&) 
{
    // Do not allow the setting of semantic types
    return false;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::GetDescription(const NiFixedString& kPropertyName,
    NiFixedString& kDescription) const 
{
    if (kPropertyName == ms_kPropStorageFileName)
    {
        kDescription = ms_kDescStorageFileName;
    }
    else if (kPropertyName == ms_kPropSectorList)
	{
		kDescription = ms_kDescSectorList;
	}
    else if (kPropertyName == ms_kPropBlockSize)
    {
        kDescription = ms_kDescBlockSize;
    }
    else if (kPropertyName == ms_kPropNumLOD)
    {
        kDescription = ms_kDescNumLOD;
    }
    else if (kPropertyName == ms_kPropCalcSectorSize)
    {
        kDescription = ms_kDescCalcSectorSize;
    }
    else if (kPropertyName == ms_kPropSceneRoot)
	{
        kDescription = ms_kPropSceneRoot;
	}
    else if (kPropertyName == ms_kPropTerrainMinHeight)
    {
        kDescription = ms_kDescTerrainMinHeight;
    }
    else if (kPropertyName == ms_kPropTerrainMaxHeight)
    {
        kDescription = ms_kDescTerrainMaxHeight;
    }
    else if (kPropertyName == ms_kPropMaskSize)
    {
        kDescription = ms_kDescMaskSize;
    }
    else if (kPropertyName == ms_kPropCookPhysXData)
    {
        kDescription = ms_kDescCookPhysXData;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::SetDescription(const NiFixedString&,
    const NiFixedString&) 
{
    // Do not allow the setting of descriptions
    return false;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::GetCategory(const NiFixedString& kPropertyName,
    NiFixedString& kCategory) const 
{
    if (kPropertyName == ms_kPropStorageFileName ||
        kPropertyName == ms_kPropSectors ||
        kPropertyName == ms_kPropSectorList ||
        kPropertyName == ms_kPropBlockSize ||
        kPropertyName == ms_kPropNumLOD ||
        kPropertyName == ms_kPropCalcSectorSize ||
        kPropertyName == ms_kPropTerrainMinHeight ||
        kPropertyName == ms_kPropTerrainMaxHeight ||
        kPropertyName == ms_kPropMaskSize ||
        kPropertyName == ms_kPropCookPhysXData)
    {
        kCategory = ms_kComponentName;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::IsPropertyReadOnly(
    const NiFixedString& kPropertyName, bool& bIsReadOnly) 
{
    if (kPropertyName == ms_kPropStorageFileName ||
        kPropertyName == ms_kPropSectors ||
        kPropertyName == ms_kPropBlockSize ||
        kPropertyName == ms_kPropNumLOD ||
        kPropertyName == ms_kPropTerrainMinHeight ||
        kPropertyName == ms_kPropTerrainMaxHeight ||
        kPropertyName == ms_kPropMaskSize ||
        kPropertyName == ms_kPropCookPhysXData)
    {
        bIsReadOnly = false;
    }
    else if(kPropertyName == ms_kPropInteractor ||
        kPropertyName == ms_kPropCalcSectorSize ||
        kPropertyName == ms_kPropSectorList ||
        kPropertyName == ms_kPropSceneRoot)
    {
        bIsReadOnly = true;
    }
    else 
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::IsPropertyUnique(const NiFixedString& kPropertyName,
    bool& bIsUnique) 
{
    if (kPropertyName == ms_kPropStorageFileName ||
        kPropertyName == ms_kPropSectors ||
        kPropertyName == ms_kPropSectorList ||
        kPropertyName == ms_kPropTerrainMinHeight ||
        kPropertyName == ms_kPropTerrainMaxHeight ||
        kPropertyName == ms_kPropMaskSize ||
        kPropertyName == ms_kPropCookPhysXData)
    {
        if (m_spMasterComponent)
        {
            NIVERIFY(CanResetProperty(kPropertyName, bIsUnique));
        }
        else
        {
            bIsUnique = true;
        }
    }
    else if(kPropertyName == ms_kPropCalcSectorSize ||
        kPropertyName ==  ms_kPropNumLOD ||
        kPropertyName == ms_kPropBlockSize ||
        kPropertyName == ms_kPropSceneRoot)
    {
        bIsUnique = true;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::IsPropertySerializable(
    const NiFixedString& kPropertyName, bool& bIsSerializable) 
{    
    if (kPropertyName == ms_kPropStorageFileName ||
        kPropertyName == ms_kPropSectorList ||        
        kPropertyName == ms_kPropBlockSize ||
        kPropertyName == ms_kPropNumLOD ||
        kPropertyName == ms_kPropTerrainMinHeight ||
        kPropertyName == ms_kPropTerrainMaxHeight ||
        kPropertyName == ms_kPropMaskSize ||
        kPropertyName == ms_kPropCookPhysXData)
    {
        bIsSerializable = true;
    }
    else if (kPropertyName == ms_kPropInteractor ||
        kPropertyName == ms_kPropCalcSectorSize ||
        kPropertyName == ms_kPropSceneRoot ||
        kPropertyName == ms_kPropSectors)
    {
        bIsSerializable = false;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::IsPropertyInheritable(
    const NiFixedString& kPropertyName, bool& bIsInheritable) 
{    
    if (kPropertyName == ms_kPropStorageFileName ||
        kPropertyName == ms_kPropSectors ||
        kPropertyName == ms_kPropSectorList ||        
        kPropertyName == ms_kPropTerrainMinHeight ||
        kPropertyName == ms_kPropTerrainMaxHeight ||
        kPropertyName == ms_kPropMaskSize ||
        kPropertyName == ms_kPropCookPhysXData)
    {
        bIsInheritable = true;
    }
    else if (kPropertyName == ms_kPropInteractor ||
        kPropertyName == ms_kPropCalcSectorSize ||
        kPropertyName == ms_kPropBlockSize ||
        kPropertyName == ms_kPropNumLOD ||
        kPropertyName == ms_kPropSceneRoot)
    {
        bIsInheritable = false;
    }
    else 
    {
        return false;
    }

    return true;    
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::IsExternalAssetPath(
    const NiFixedString& kPropertyName, NiUInt32 uiIndex, 
    bool& bIsExternalAssetPath) const 
{
    
    if (kPropertyName == ms_kPropSectors ||
        kPropertyName == ms_kPropSectorList ||
        kPropertyName == ms_kPropSceneRoot ||        
        kPropertyName == ms_kPropInteractor ||
        kPropertyName == ms_kPropBlockSize ||
        kPropertyName == ms_kPropNumLOD ||
        kPropertyName == ms_kPropCalcSectorSize ||
        kPropertyName == ms_kPropTerrainMinHeight ||
        kPropertyName == ms_kPropTerrainMaxHeight ||
        kPropertyName == ms_kPropMaskSize ||
        kPropertyName == ms_kPropCookPhysXData)
    {
        bIsExternalAssetPath = false;
    } 
    else if (kPropertyName == ms_kPropStorageFileName && uiIndex == 0) 
    {
        bIsExternalAssetPath = true;
    } 
    else 
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::SetElementCount(const NiFixedString& kPropertyName,
    NiUInt32 uiCount, bool &bCountSet)
{
    if (kPropertyName == ms_kPropStorageFileName ||
        kPropertyName == ms_kPropSectors ||
        kPropertyName == ms_kPropSceneRoot ||        
        kPropertyName == ms_kPropInteractor ||
        kPropertyName == ms_kPropBlockSize ||
        kPropertyName == ms_kPropNumLOD ||
        kPropertyName == ms_kPropCalcSectorSize ||
        kPropertyName == ms_kPropTerrainMinHeight ||
        kPropertyName == ms_kPropTerrainMaxHeight ||
        kPropertyName == ms_kPropMaskSize ||
        kPropertyName == ms_kPropCookPhysXData)
    {
        bCountSet = false;
    }
    else if (kPropertyName == ms_kPropSectorList)
    {
        const NiInt32 uiOrigCount = m_kSectorList.GetSize();
        for (NiInt32 ui = uiOrigCount - 1; ui >= (NiInt32)uiCount; --ui)
        {
            m_kSectorList.RemoveAt(ui);
        }
        for (NiInt32 ui = uiOrigCount; ui < (NiInt32)uiCount; ++ui)
        {
            m_kSectorList.Add(0);
        }
        bCountSet = true;
        SetBit(true, PROP_SECTORS_CHANGED);
        SetBit(true, PROP_SETTING_CHANGED);
    } 
    else 
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::GetElementCount(const NiFixedString& kPropertyName,
    NiUInt32& uiCount) const 
{
    if (kPropertyName == ms_kPropStorageFileName ||
        kPropertyName == ms_kPropInteractor ||
        kPropertyName == ms_kPropSceneRoot ||        
        kPropertyName == ms_kPropBlockSize ||
        kPropertyName == ms_kPropNumLOD ||
        kPropertyName == ms_kPropCalcSectorSize ||
        kPropertyName == ms_kPropTerrainMinHeight ||
        kPropertyName == ms_kPropTerrainMaxHeight ||
        kPropertyName == ms_kPropMaskSize ||
        kPropertyName == ms_kPropCookPhysXData) 
    {
        uiCount = 1;
    }
    else if (kPropertyName == ms_kPropSectors) 
    {
        uiCount = 2;
    }
    else if (kPropertyName == ms_kPropSectorList)
    {
        uiCount = m_kSectorList.GetSize();
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::IsCollection(const NiFixedString& kPropertyName,
    bool& bIsCollection) const 
{
    if (kPropertyName == ms_kPropStorageFileName ||
        kPropertyName == ms_kPropInteractor ||
        kPropertyName == ms_kPropBlockSize ||
        kPropertyName == ms_kPropNumLOD ||
        kPropertyName == ms_kPropSceneRoot ||
        kPropertyName == ms_kPropCalcSectorSize ||
        kPropertyName == ms_kPropTerrainMinHeight ||
        kPropertyName == ms_kPropTerrainMaxHeight ||
        kPropertyName == ms_kPropMaskSize ||
        kPropertyName == ms_kPropCookPhysXData)
    {
        bIsCollection = false;
    } 
    else if (kPropertyName == ms_kPropSectors ||
        kPropertyName == ms_kPropSectorList)          
    {
        bIsCollection = true;
    }
    else 
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::GetPropertyData(const NiFixedString& kPropertyName,
    NiFixedString& kData, NiUInt32 uiIndex) const
{
    if (uiIndex != 0) 
    {
        return false;
    }
    else if (kPropertyName == ms_kPropStorageFileName)
    {
        kData = GetPropStorageFilename();
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::SetPropertyData(const NiFixedString& kPropertyName,
    const NiFixedString& kData, NiUInt32 uiIndex) 
{
    if (uiIndex != 0) 
    {
        return false;
    }
    else if (kPropertyName == ms_kPropStorageFileName)
    {
        if (!kData.Exists() || kData.GetLength() == 0)
            SetPropStorageFilename(0);
        else
        {
            // Check that the path is absolute
            if (NiPath::IsRelative(kData))
                return false;

            SetPropStorageFilename(kData);
        }
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::GetPropertyData(const NiFixedString& kPropertyName,
    NiUInt32& uiData, NiUInt32 uiIndex) const
{
    if (uiIndex > 0 && kPropertyName != ms_kPropSectors)
        return false;

    if (kPropertyName == ms_kPropSectors)
    {
        if (uiIndex == 0) 
        {
            uiData = GetPropSectorsX();
        }
        else if (uiIndex == 1) 
        {
            uiData = GetPropSectorsY();
        }
        else
        {
            return false;
        }
    }
    else if (kPropertyName == ms_kPropBlockSize)
    {
        uiData = GetPropBlockSize();
    }
    else if (kPropertyName == ms_kPropNumLOD)
    {
        uiData = GetPropNumLOD();
    }
    else if (kPropertyName == ms_kPropCalcSectorSize)
    {
        uiData = GetPropCalcSectorSize();
    }
    else if (kPropertyName == ms_kPropMaskSize)
    {
        uiData = GetPropMaskSize();
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::SetPropertyData(const NiFixedString& kPropertyName,
    NiUInt32 uiData, NiUInt32 uiIndex)
{
    if (uiIndex > 0 && kPropertyName != ms_kPropSectors)
        return false;

    if (kPropertyName == ms_kPropSectors)
    {
        if (uiIndex == 0) {
            SetPropSectorsX(uiData);
            return true;
        }
        else if (uiIndex == 1) 
        {
            SetPropSectorsY(uiData);
            return true;
        }
    }
    else if (kPropertyName == ms_kPropBlockSize)
    {
        return SetPropBlockSize(uiData);
    }
    else if (kPropertyName == ms_kPropNumLOD)
    {
        return SetPropNumLOD(uiData);
    }
    else if (kPropertyName == ms_kPropMaskSize)
    {
        return SetPropMaskSize(uiData);
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::GetPropertyData(const NiFixedString& kPropertyName,
        void*& pvData, size_t& stDataSizeInBytes, NiUInt32)
        const
{
    if (kPropertyName == ms_kPropInteractor)
    {
        NIASSERT(m_spTerrain);
        NiTerrainInteractor* pkInteractor = m_spTerrain->GetInteractor();
        pvData = (void*)(pkInteractor);

        if(stDataSizeInBytes)
            stDataSizeInBytes = sizeof(pkInteractor);

        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::SetPropertyData(const NiFixedString&,
        const void*, size_t, NiUInt32)
{    
    return false;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::GetPropertyData(const NiFixedString& kPropertyName,
    NiInt32& iData, NiUInt32) const
{
    if (kPropertyName == ms_kPropTerrainMinHeight)
    {
        iData = GetPropTerrainMinHeight();
        return true;
    }
    else if (kPropertyName == ms_kPropTerrainMaxHeight)
    {
        iData = GetPropTerrainMaxHeight();
        return true;
    }
    else
    {
        return false;
    }    
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::SetPropertyData(const NiFixedString& kPropertyName,
    NiInt32 iData, NiUInt32)
{
    if (kPropertyName == ms_kPropTerrainMinHeight)
    {
        return SetPropTerrainMinHeight(iData);
    }
    else if (kPropertyName == ms_kPropTerrainMaxHeight)
    {
        return SetPropTerrainMaxHeight(iData);
    }
    else
    {
        return false;
    }    
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::GetPropertyData(const NiFixedString& kPropertyName,
        NiObject*& pkData, NiUInt32 uiIndex) const
{
	if (kPropertyName == ms_kPropSceneRoot)
	{
	    if (m_spTerrain && uiIndex == 0) 
        {
            pkData = m_spTerrain;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::SetPropertyData(const NiFixedString&,
    NiObject*, NiUInt32 uiIndex)
{
    if (uiIndex > 1)
        return false;

    return false;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::GetPropertyData(const NiFixedString& kPropertyName,
        NiPoint2& kData, NiUInt32 uiIndex) const
{
    if (kPropertyName == ms_kPropSectorList)
    {
        if (uiIndex >= m_kSectorList.GetSize())
            return false;
        NiUInt32 uiValue = m_kSectorList.GetAt(uiIndex);
        kData.x = (float)(NiInt16)(uiValue >> 16);
        kData.y = (float)(NiInt16)(uiValue & 0x0000FFFF);
    }
    else
    {
        return false;
    }

	return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::SetPropertyData(const NiFixedString& kPropertyName,
    const NiPoint2& kData, NiUInt32 uiIndex)
{
    if (kPropertyName == ms_kPropSectorList)
    {
        return SetPropSectorList(uiIndex, kData);
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
NiBool NiTerrainComponent::GetPropertyData(const NiFixedString& kPropertyName,
        bool& bData, NiUInt32 uiIndex) const
{
    if (uiIndex > 0)
        return false;
    if (kPropertyName == ms_kPropCookPhysXData)
    {
        bData = GetPropCookPhysXData(); 
        return true;
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------    
NiBool NiTerrainComponent::SetPropertyData(const NiFixedString& kPropertyName,
        bool bData, NiUInt32 uiIndex)
{
    if (uiIndex > 0)
        return false;
    if (kPropertyName == ms_kPropCookPhysXData)
    {
        return SetPropCookPhysXData(bData);
    }
    else
    {
        return false;
    }
}
//---------------------------------------------------------------------------
bool NiTerrainComponent::SetPropTerrainMinHeight(NiInt32 iNewValue)
{
    if (iNewValue > m_iTerrainMaxHeight)
        return false;

    if (iNewValue != m_iTerrainMinHeight)
    {  
        m_iTerrainMinHeight = iNewValue;
        SetBit(true, PROP_MIN_ELEVATION_CHANGED);
        SetBit(true, PROP_SETTING_CHANGED);

        // Setting a new minimum height truncates anything below it.
        NiUInt32 uiSectorWidth = (m_uiPropBlockSize << m_uiPropNumLOD) + 1;
        NiUInt32 uiNumValues = uiSectorWidth * uiSectorWidth;
        float fMaxVal, fMinVal;
        float* pfBuffer = NiAlloc(float, uiNumValues);
        
        NIASSERT(m_spTerrain);
        NiTerrainInteractor* pkTerrainInteractor = 
            m_spTerrain->GetInteractor();
        if (pkTerrainInteractor->GetHeightMapValues(pfBuffer, 
            uiNumValues * sizeof(float), uiNumValues, fMinVal, fMaxVal))
        {
            float fMinHeight = (float)m_iTerrainMinHeight;
            for (NiUInt32 ui = 0; ui < uiNumValues; ui++)
                pfBuffer[ui] = NiMax(fMinHeight, pfBuffer[ui]);

            pkTerrainInteractor->SetHeightFromMap(pfBuffer, sizeof(float) * 
                uiNumValues, uiNumValues);
        }
        
        NiFree(pfBuffer);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiTerrainComponent::SetPropTerrainMaxHeight(NiInt32 iNewValue)
{
    if (iNewValue < m_iTerrainMinHeight)
        return false;

    if (iNewValue != m_iTerrainMaxHeight)
    {
        m_iTerrainMaxHeight = iNewValue;
        SetBit(true, PROP_MAX_ELEVATION_CHANGED);
        SetBit(true, PROP_SETTING_CHANGED);

        // Setting a new minimum height truncates anything below it.
        NiUInt32 uiSectorWidth = (m_uiPropBlockSize << m_uiPropNumLOD) + 1;
        NiUInt32 uiNumValues = uiSectorWidth * uiSectorWidth;
        float fMaxVal, fMinVal;
        float* pfBuffer = NiAlloc(float, uiNumValues);

        NIASSERT(m_spTerrain);
        NiTerrainInteractor* pkTerrainInteractor = 
            m_spTerrain->GetInteractor();
        if (pkTerrainInteractor->GetHeightMapValues(pfBuffer, 
            uiNumValues * sizeof(float), uiNumValues, fMinVal, fMaxVal))
        {
            float fMaxHeight = (float)m_iTerrainMaxHeight;
            for (NiUInt32 ui = 0; ui < uiNumValues; ui++)
                pfBuffer[ui] = NiMin(fMaxHeight, pfBuffer[ui]);

            pkTerrainInteractor->SetHeightFromMap(pfBuffer, sizeof(float) * 
                uiNumValues, uiNumValues);
        }

        NiFree(pfBuffer);
    }

    return true;
}