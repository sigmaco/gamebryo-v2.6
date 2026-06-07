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

#include "NiTerrainSectorComponent.h"
#include "NiTerrainComponent.h"

#include <NiFactories.h>

// Streaming
NiFactoryDeclarePropIntf(NiTerrainSectorComponent);
    
// Class name.
NiFixedString NiTerrainSectorComponent::ms_kClassName;
NiFixedString NiTerrainSectorComponent::ms_kComponentName;

// Properties
NiFixedString NiTerrainSectorComponent::ms_kPropDeformable;
NiFixedString NiTerrainSectorComponent::ms_kPropTerrainID;
NiFixedString NiTerrainSectorComponent::ms_kPropMaskSize;
NiFixedString NiTerrainSectorComponent::ms_kPropSectorIndex;
NiFixedString NiTerrainSectorComponent::ms_kPropSceneRoot;
NiFixedString NiTerrainSectorComponent::ms_kPropHasChanged;

// Sibling Properties (Terrain)
NiFixedString NiTerrainSectorComponent::ms_kPropStorageFileName;
NiFixedString NiTerrainSectorComponent::ms_kPropSectors;
NiFixedString NiTerrainSectorComponent::ms_kPropBlockSize;
NiFixedString NiTerrainSectorComponent::ms_kPropNumLOD;
NiFixedString NiTerrainSectorComponent::ms_kPropCalcSectorSize;
NiFixedString NiTerrainSectorComponent::ms_kPropTerrainMinHeight;
NiFixedString NiTerrainSectorComponent::ms_kPropTerrainMaxHeight;

// Descriptions
NiFixedString NiTerrainSectorComponent::ms_kDescDeformable;
NiFixedString NiTerrainSectorComponent::ms_kDescTerrainID;
NiFixedString NiTerrainSectorComponent::ms_kDescMaskSize;
NiFixedString NiTerrainSectorComponent::ms_kDescSectorIndex;
NiFixedString NiTerrainSectorComponent::ms_kDescHasChanged;

// Errors
NiFixedString NiTerrainSectorComponent::ms_kErrorNoTerrainComponent;

//---------------------------------------------------------------------------
NiTerrainSectorComponent::NiTerrainSectorComponent() :
    m_uFlags(0),
    m_bPropDeformable(false),    
    m_kPropTerrainID(""),
    m_uiPropMaskSize(64),
    m_bHasChanged(false),    
    m_spMasterComponent(0),
    m_spTerrainComponent(0),
    m_spSector(0)
{
    m_iPropSectorIndex[0] = 0;
    m_iPropSectorIndex[1] = 0;
}
//---------------------------------------------------------------------------
NiTerrainSectorComponent::NiTerrainSectorComponent(
    NiTerrainSectorComponent* pkMasterComponent) :
    m_uFlags(0),
    m_bPropDeformable(pkMasterComponent->GetPropDeformable()),
    m_kPropTerrainID(pkMasterComponent->GetPropTerrainID()),
    m_uiPropMaskSize(pkMasterComponent->GetPropMaskSize()),
    m_bHasChanged(false),
    m_spMasterComponent(pkMasterComponent),
    m_spTerrainComponent(0),
    m_spSector(0)
{
    // Properties that we should take from the parent
    m_iPropSectorIndex[0] = pkMasterComponent->GetPropSectorIndexX();
    m_iPropSectorIndex[1] = pkMasterComponent->GetPropSectorIndexX();
}
//---------------------------------------------------------------------------
NiTerrainSectorComponent::~NiTerrainSectorComponent()
{
    m_spTerrainComponent = 0;
    m_spSector = 0;
}
//---------------------------------------------------------------------------
void NiTerrainSectorComponent::_SDMInit()
{
    // Class name.
    ms_kClassName = "NiTerrainSectorComponent";
    ms_kComponentName = "Terrain Sector";

    // Properties
    ms_kPropDeformable = "Deformable";
    ms_kPropTerrainID = "Parent Terrain ID";
    ms_kPropMaskSize = "Default Surface Mask Size";
    ms_kPropHasChanged = "Sector Changed";

    ms_kPropSectorIndex = "Sector Index";
    ms_kPropSceneRoot = "Scene Root Pointer";

    // Sibling Properties (Terrain)
    ms_kPropStorageFileName = "Terrain Archive";
    ms_kPropSectors = "Number of Sectors";
    ms_kPropBlockSize = "Block Side Length";
    ms_kPropNumLOD = "Number of block sub-divisions";
    ms_kPropCalcSectorSize = "Resulting sector size";
    ms_kPropTerrainMinHeight = "Min. Elevation";
    ms_kPropTerrainMaxHeight = "Max. Elevation";

    // Descriptions
    ms_kDescDeformable = "Allow this sector to be deformed in realtime";
    ms_kDescTerrainID = "Name of the entity that contains the parent "
        "NiTerrainComponent from which this sector should load its data";
    ms_kDescMaskSize = "The side length (in pixels) of the opacity mask that "
        "will be created for each painted surface in this sector. Note, that "
        "changing this value will not affect existing masks on this sector.";
    ms_kDescSectorIndex = "The index of the sector data to load from the "
        "terrain archive within the parent NiTerrainComponent.";
    ms_kDescHasChanged = "This property is used by the water to know whether "
        "or not its associated terrain has changed and an update should be "
        "done";

    // Errors
    ms_kErrorNoTerrainComponent = "Could not find an NiTerrainComponent "
        " associated with the NiTerrainSectorComponent. Resetting to NULL.";
}
//---------------------------------------------------------------------------
void NiTerrainSectorComponent::_SDMShutdown()
{
    // Class name.
    ms_kClassName = NULL;
    ms_kComponentName = NULL;

    // Properties
    ms_kPropDeformable = NULL;
    ms_kPropTerrainID = NULL;
    ms_kPropMaskSize = NULL;
    ms_kPropSectorIndex = NULL;
    ms_kPropSceneRoot = NULL;
    ms_kPropHasChanged = NULL;

    // Sibling Properties (Terrain)
    ms_kPropStorageFileName = NULL;
    ms_kPropSectors = NULL;
    ms_kPropBlockSize = NULL;
    ms_kPropNumLOD = NULL;
    ms_kPropCalcSectorSize = NULL;
    ms_kPropTerrainMinHeight = NULL;
    ms_kPropTerrainMaxHeight = NULL;

    // Descriptions
    ms_kDescDeformable = NULL;
    ms_kDescTerrainID = NULL;
    ms_kDescMaskSize = NULL;
    ms_kDescSectorIndex = NULL;
    ms_kDescHasChanged = NULL;

    // Errors
    ms_kErrorNoTerrainComponent = NULL;
}
//---------------------------------------------------------------------------
NiTerrainSector* NiTerrainSectorComponent::GetSector() const
{
    return m_spSector;
}
//---------------------------------------------------------------------------
NiNode* NiTerrainSectorComponent::GetSectorAsNode() const
{
    return m_spSector;
}
//---------------------------------------------------------------------------
void NiTerrainSectorComponent::SetPropTerrainID(
    const NiFixedString& kTerrainID)
{
    if (GetPropTerrainID() != kTerrainID)
    {        
        if (m_spTerrainComponent)
        {
            m_spTerrainComponent = 0;
        }

        m_kPropTerrainID = kTerrainID;
        SetBit(true, PROP_TERRAIN_ID_CHANGED);
        MarkSettingChanged();
    }
}
//---------------------------------------------------------------------------
bool NiTerrainSectorComponent::SetPropMaskSize(NiUInt32 uiSize)
{
    if (!NiIsPowerOf2(uiSize))
        return false;

    if (GetPropMaskSize() != uiSize)
    {
        m_uiPropMaskSize = uiSize;

        SetBit(true, PROP_MASK_SIZE_CHANGED);
        MarkSettingChanged();
    }
    
    return true;
}
//---------------------------------------------------------------------------
void NiTerrainSectorComponent::SetPropDeformable(bool bDeformable)
{
    if (NIBOOL_IS_TRUE(GetPropDeformable()) != bDeformable)
    {
        m_bPropDeformable = bDeformable;
        SetBit(true, PROP_DEFORMABLE_CHANGED);
        MarkSettingChanged();
    }
}
//---------------------------------------------------------------------------
bool NiTerrainSectorComponent::Save(const char* pcArchive,
    NiEntityErrorInterface* pkError)
{    
    NiUInt32 uiErrorCode = 0;
    bool bResult = m_spSector->Save(pcArchive, &uiErrorCode);

    if (pkError != 0)
    {
        NiTerrainComponent::GenerateErrorMessages(uiErrorCode, pkError, true);
    }

    return bResult;
}
//---------------------------------------------------------------------------
bool NiTerrainSectorComponent::Load(const char* pcArchive,
    NiEntityErrorInterface* pkError)
{
    NiUInt32 uiErrorCode = 0;
    bool bResult = m_spSector->Load(pcArchive, &uiErrorCode);

    if (pkError != 0)
    {
        NiTerrainComponent::GenerateErrorMessages(uiErrorCode, pkError);
    }

    return bResult;
}
//---------------------------------------------------------------------------
void NiTerrainSectorComponent::Update(
    NiEntityPropertyInterface* pkParentEntity,
    float fTime, NiEntityErrorInterface* pkErrors,
    NiExternalAssetManager* pkAssetManager) 
{
    m_bHasChanged = false;
    
    NIASSERT(pkParentEntity->GetInterfaceType() == 
        NiEntityInterface::IT_ENTITYINTERFACE);

    NiEntityInterface* pkEntity = ((NiEntityInterface*)pkParentEntity);
    
    if (!m_kPropTerrainID.Exists() || m_kPropTerrainID.GetLength() == 0)
    {
        // This is a template or a master component. We need to remove the 
        // NiTerrain component from it
        if (m_spMasterComponent)
        {
            pkEntity->GetMasterEntity()->RemoveComponent(
                m_spMasterComponent, false);
            m_spMasterComponent = 0;
        }
        NiTerrainComponent::FindTerrainComponent(pkEntity)->SetPropertyData(
            NiTerrainComponent::ms_kPropMaskSize, GetPropMaskSize());
        pkEntity->RemoveComponent(this, false); 

        return;
    }    

    // Do we need to search for the corresponding Terrain?
    if (m_spTerrainComponent == 0)
    {
        m_spTerrainComponent = 
            NiTerrainComponent::FindTerrainComponent(pkEntity);
        NiTerrain* pkTerrain = m_spTerrainComponent->GetTerrain();

        if (!m_spTerrainComponent)
        {   
            pkErrors->ReportError(ms_kErrorNoTerrainComponent,
                "No Terrain Component", pkParentEntity->GetName(), NULL);
            m_kPropTerrainID = NULL;
            return;
        }
        else if (m_spSector)
        {
            m_spSector->SetTerrain(pkTerrain);
        }
        else
        {
            m_spSector = pkTerrain->GetSector(GetPropSectorIndexX(),
                GetPropSectorIndexY());

            if (m_spSector == 0)
            {
                m_spSector = pkTerrain->CreateSector(GetPropSectorIndexX(),
                    GetPropSectorIndexY());
            }
        }

        MarkSettingChanged();
    }

    // Have either our, or our parent sectors settings changed since the last 
    // update?
    if (HasSettingChanged())
    {
        // Push our data to the sector:
        // Deformable value
        m_spSector->SetIsDeformable(NIBOOL_IS_TRUE(GetPropDeformable()));

        // Surface mask size
        m_spTerrainComponent->SetPropertyData(ms_kPropMaskSize, 
            GetPropMaskSize());

        // Sector Index
        m_spSector->SetSectorIndex(GetPropSectorIndexX(), 
            GetPropSectorIndexY());
        
        // Don't do transformation if we are in the same entity as the 
        // Terrain Component
        void* pvInteractor = NULL;
        size_t uiSize;
        if (!pkParentEntity->GetPropertyData(
            NiTerrainComponent::ms_kPropInteractor, 
            pvInteractor, uiSize, 0))
        {
            // Propagate translation
            NiPoint3 kTranslate;
            NiMatrix3 kRotate;
            float fScale;
    
            if (pkParentEntity->GetPropertyData("Translation", kTranslate, 0))
            {
                pkParentEntity->GetPropertyData("Rotation", kRotate, 0);
                pkParentEntity->GetPropertyData("Scale", fScale, 0);
    
                m_spSector->SetTranslate(kTranslate);
                m_spSector->SetRotate(kRotate);
                m_spSector->SetScale(fScale);
            }        
        }
        SetBit(false, PROP_SETTING_CHANGED);
    }

    if (GetSector()->HasShapeChangedLastUpdate())
        m_bHasChanged = true;

    // Ask the terrain to trigger an update in the sector. 
    // Must be done this way to make sure terrain component has
    // uploaded all its data to the sectors before the first update
    // on a sector is called.
    if (m_spTerrainComponent)
    {
        m_spTerrainComponent->Update(pkParentEntity, fTime, pkErrors, 
            pkAssetManager);
    }
    else
    {
        m_spSector->Update(fTime);
    }

    // Now that we passed on information from this component to the
    // NiTerrainComponent, we can remove it from the entity
    if (m_spMasterComponent)
    {
        // The master component properties must be transferred to the 
        // terrain component before being removed.
        NiTerrainComponent::FindTerrainComponent(pkEntity->GetMasterEntity())
            ->SetPropertyData(NiTerrainComponent::ms_kPropMaskSize, 
            m_spMasterComponent->GetPropMaskSize());
        
        pkEntity->GetMasterEntity()->RemoveComponent(
            m_spMasterComponent, false);
        m_spMasterComponent = 0;
    }
    pkEntity->RemoveComponent(this, false); 

    if (NiTerrainInteractor::InToolMode())
    {
        pkErrors->ReportError("NiTerrainSectorComponent is deprecated", 
            "References to the NiTerrainSectorComponent have been removed from "
            "the scene.", 
            "NiTerrainSectorComponent", "Deprecated");
    }
        
}
//---------------------------------------------------------------------------
void NiTerrainSectorComponent::BuildVisibleSet(
    NiEntityRenderingContext* pkRenderingContext,
    NiEntityErrorInterface*) 
{
    // Do not draw if we have a parent terrain object. Sector will be drawn
    // through that interface only.
    if (!m_spTerrainComponent && m_spSector)
    {
        NiCullingProcess* pkCuller = 
            pkRenderingContext->m_pkCullingProcess; 

        // Cull this sector's section of the scene graph:
        NiCullScene(pkRenderingContext->m_pkCamera,
            m_spSector,
            *pkCuller,
            *pkCuller->GetVisibleSet(),
            false);
    }
}
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiTerrainSectorComponent::Clone(
    bool bInheritProperties)
{
    NiTerrainSectorComponent* pkClone = NiNew NiTerrainSectorComponent(this);

    if (!bInheritProperties)
    {
        pkClone->SetMasterComponent(0);
        pkClone->m_uFlags = m_uFlags;
    }

    return pkClone;
}
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiTerrainSectorComponent::GetMasterComponent()
    const
{
    // This component does not have a master component.
    return m_spMasterComponent;
}
//---------------------------------------------------------------------------
bool NiTerrainSectorComponent::IsTemplate()
{
    return m_spMasterComponent == 0;
}
//---------------------------------------------------------------------------
void NiTerrainSectorComponent::SetMasterComponent(
    NiEntityComponentInterface* pkMasterComponent)
{
    if (pkMasterComponent)
    {
        // We were given a master component
        NIASSERT(pkMasterComponent->GetClassName() == GetClassName());
        m_spMasterComponent = (NiTerrainSectorComponent*)pkMasterComponent;
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
            NIVERIFY(MakePropertyUnique(kPropertyNames.GetAt(ui),
                bMadeUnique));
        }
        m_spMasterComponent = NULL;
    }
}
//---------------------------------------------------------------------------
NiBool NiTerrainSectorComponent::IsAnimated() const 
{
    return false;
}
//---------------------------------------------------------------------------
void NiTerrainSectorComponent::GetDependentPropertyNames(
    NiTObjectSet<NiFixedString>& kDependentPropertyNames) 
{
    kDependentPropertyNames.Add(ms_kPropStorageFileName);
    kDependentPropertyNames.Add(ms_kPropSectors);
    kDependentPropertyNames.Add(ms_kPropBlockSize);
    kDependentPropertyNames.Add(ms_kPropNumLOD);
    kDependentPropertyNames.Add(ms_kPropCalcSectorSize);
    kDependentPropertyNames.Add(ms_kPropTerrainMinHeight);
    kDependentPropertyNames.Add(ms_kPropTerrainMaxHeight);
    return;
}
//---------------------------------------------------------------------------
NiBool NiTerrainSectorComponent::SetTemplateID(const NiUniqueID&) 
{
    // Not supported for custom components
    return false;
}
//---------------------------------------------------------------------------
NiUniqueID NiTerrainSectorComponent::GetTemplateID() 
{
    static const NiUniqueID kUniqueID = 
        NiUniqueID(0x88,0x9F,0x34,0x14,0x50,0x8A,0x3F,0x44,
        0x7B,0x18,0x29,0x2,0xA7,0x2A,0x4B,0x00);
    return kUniqueID;
}
//---------------------------------------------------------------------------
void NiTerrainSectorComponent::AddReference() 
{
    IncRefCount();
}
//---------------------------------------------------------------------------
void NiTerrainSectorComponent::RemoveReference() 
{
    DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiTerrainSectorComponent::GetClassName() const 
{
    return ms_kClassName;
}
//---------------------------------------------------------------------------
NiFixedString NiTerrainSectorComponent::GetName() const 
{
    return ms_kComponentName;
}
//---------------------------------------------------------------------------
NiBool NiTerrainSectorComponent::SetName(const NiFixedString&) 
{
    // This component does not allow its name to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiTerrainSectorComponent::GetDisplayName(
    const NiFixedString& kPropertyName, NiFixedString& kDisplayName) const 
{
    // There are no longer any properties we want to display
    if (kPropertyName == ms_kPropSceneRoot ||
        kPropertyName == ms_kPropMaskSize ||
        kPropertyName == ms_kPropDeformable ||
        kPropertyName == ms_kPropSectorIndex ||
        kPropertyName == ms_kPropTerrainID)
    {
        kDisplayName = NULL;
    }
    else 
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainSectorComponent::SetDisplayName(
    const NiFixedString&, const NiFixedString&) 
{
    // This component does not allow the display name to be set.
    return false;
}
//---------------------------------------------------------------------------
void NiTerrainSectorComponent::GetPropertyNames(
    NiTObjectSet<NiFixedString>& kPropertyNames) const 
{
    kPropertyNames.Add(ms_kPropSceneRoot);
    return;
}
//---------------------------------------------------------------------------
NiBool NiTerrainSectorComponent::CanResetProperty(
    const NiFixedString& kPropertyName, bool& bCanReset) const 
{
    if (kPropertyName == ms_kPropDeformable)
    {
        bCanReset = m_spMasterComponent && GetBit(PROP_DEFORMABLE_CHANGED);
    }
    else if (kPropertyName == ms_kPropTerrainID)
    {
        bCanReset = m_spMasterComponent && GetBit(PROP_TERRAIN_ID_CHANGED);
    } 
    else if (
        kPropertyName == ms_kPropSceneRoot ||
        kPropertyName == ms_kPropSectorIndex ||
        kPropertyName == ms_kPropMaskSize
        )
    {
        bCanReset = false;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainSectorComponent::ResetProperty(
    const NiFixedString& kPropertyName) 
{
    bool bCanReset;
    if (!CanResetProperty(kPropertyName, bCanReset) || !bCanReset)
    {
        return false;
    }

    if (kPropertyName == ms_kPropDeformable)
    {
        SetPropDeformable(
            NIBOOL_IS_TRUE(m_spMasterComponent->GetPropDeformable()));
        SetBit(false, PROP_DEFORMABLE_CHANGED);
    }
    else if (kPropertyName == ms_kPropTerrainID)
    {
        SetPropTerrainID(m_spMasterComponent->GetPropTerrainID());
        SetBit(false, PROP_TERRAIN_ID_CHANGED);
    } 
    else 
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainSectorComponent::MakePropertyUnique(
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
        if (kPropertyName == ms_kPropDeformable)
        {
            SetPropDeformable(NIBOOL_IS_TRUE(GetPropDeformable()));
        }
        else if (kPropertyName == ms_kPropTerrainID)
        {
            SetPropTerrainID(GetPropTerrainID());
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
NiBool NiTerrainSectorComponent::GetPrimitiveType(
    const NiFixedString& kPropertyName, NiFixedString& kPrimitiveType) const 
{
    if (kPropertyName == ms_kPropDeformable)
    {
        kPrimitiveType = PT_BOOL;
    }
    else if (kPropertyName == ms_kPropTerrainID)
    {
        kPrimitiveType = PT_STRING;
    }
    else if (kPropertyName == ms_kPropSectorIndex ||
        kPropertyName == ms_kPropMaskSize)
    {
        kPrimitiveType = PT_UINT;
    }
    else if (kPropertyName == ms_kPropSceneRoot)
    {
        kPrimitiveType = PT_NIOBJECTPOINTER;
    }
    else 
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainSectorComponent::SetPrimitiveType(
    const NiFixedString&, const NiFixedString&) 
{    
    // Do not allow the setting of primitive types
    return false;
}
//---------------------------------------------------------------------------
NiBool NiTerrainSectorComponent::GetSemanticType(
    const NiFixedString& kPropertyName, NiFixedString& kSemanticType) const 
{
    if (kPropertyName == ms_kPropDeformable )
    {
        kSemanticType = PT_BOOL;
    }
    else if (kPropertyName == ms_kPropTerrainID)
    {
        kSemanticType = PT_STRING;
    }
    else if (kPropertyName == ms_kPropSectorIndex ||
        kPropertyName == ms_kPropMaskSize)
    {
        kSemanticType = PT_UINT;
    }
    else if (kPropertyName == ms_kPropSceneRoot)
    {
        kSemanticType = PT_NIOBJECTPOINTER;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainSectorComponent::SetSemanticType(
    const NiFixedString&, const NiFixedString&) 
{
    // Do not allow the setting of semantic types
    return false;
}
//---------------------------------------------------------------------------
NiBool NiTerrainSectorComponent::GetDescription(
    const NiFixedString& kPropertyName, NiFixedString& kDescription) const 
{
    if (kPropertyName == ms_kPropDeformable)
    {
        kDescription = ms_kDescDeformable;
    }
    else if (kPropertyName == ms_kPropTerrainID)
    {
        kDescription = ms_kDescTerrainID;
    }
    else if (kPropertyName == ms_kPropMaskSize)
    {
        kDescription = ms_kDescMaskSize;
    }
    else if (kPropertyName == ms_kPropSectorIndex)
    {
        kDescription = ms_kDescSectorIndex;
    }
    else if (kPropertyName == ms_kPropSceneRoot)
    {
        kDescription = ms_kPropSceneRoot;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainSectorComponent::SetDescription(
    const NiFixedString&, const NiFixedString&) 
{
    // Do not allow the setting of descriptions
    return false;
}
//---------------------------------------------------------------------------
NiBool NiTerrainSectorComponent::GetCategory(
    const NiFixedString& kPropertyName, NiFixedString& kCategory) const 
{
    if ( kPropertyName == ms_kPropDeformable ||
        kPropertyName == ms_kPropTerrainID ||
        kPropertyName == ms_kPropMaskSize)
    {
        kCategory = ms_kComponentName;
    }
    else if (kPropertyName == ms_kPropSectorIndex)
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
NiBool NiTerrainSectorComponent::IsPropertyReadOnly(
    const NiFixedString& kPropertyName, bool& bIsReadOnly) 
{
    if (kPropertyName == ms_kPropDeformable ||
        kPropertyName == ms_kPropTerrainID ||
        kPropertyName == ms_kPropSectorIndex ||
        kPropertyName == ms_kPropMaskSize)
    {
        bIsReadOnly = false;
    }
    else if (kPropertyName == ms_kPropSceneRoot)
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
NiBool NiTerrainSectorComponent::IsPropertyUnique(
    const NiFixedString& kPropertyName, bool& bIsUnique) 
{
    if (kPropertyName == ms_kPropDeformable ||
        kPropertyName == ms_kPropTerrainID ||
        kPropertyName == ms_kPropMaskSize)
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
    else if (
        kPropertyName == ms_kPropSectorIndex ||
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
NiBool NiTerrainSectorComponent::IsPropertySerializable(
    const NiFixedString& kPropertyName, bool& bIsSerializable) 
{   
    // There are no longer any serializable properties as we do not
    // wish to save them
    if (kPropertyName == ms_kPropSceneRoot ||
        kPropertyName == ms_kPropMaskSize ||
        kPropertyName == ms_kPropDeformable ||
        kPropertyName == ms_kPropSectorIndex ||
        kPropertyName == ms_kPropTerrainID)
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
NiBool NiTerrainSectorComponent::IsPropertyInheritable(
    const NiFixedString& kPropertyName, bool& bIsInheritable) 
{    
    if (kPropertyName == ms_kPropDeformable ||
        kPropertyName == ms_kPropTerrainID ||
        kPropertyName == ms_kPropMaskSize)
    {
        bIsInheritable = true;
    }
    else if (
        kPropertyName == ms_kPropSectorIndex ||
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
NiBool NiTerrainSectorComponent::IsExternalAssetPath(
    const NiFixedString& kPropertyName,
    NiUInt32, bool& bIsExternalAssetPath) const 
{
    
    if (kPropertyName == ms_kPropDeformable || 
        kPropertyName == ms_kPropSceneRoot ||
        kPropertyName == ms_kPropTerrainID ||
        kPropertyName == ms_kPropSectorIndex ||
        kPropertyName == ms_kPropMaskSize) 
    {
        bIsExternalAssetPath = false;
    }
    else 
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainSectorComponent::SetElementCount(
    const NiFixedString& kPropertyName, NiUInt32, bool &bCountSet)
{
    if (kPropertyName == ms_kPropDeformable || 
        kPropertyName == ms_kPropSceneRoot ||
        kPropertyName == ms_kPropTerrainID ||
        kPropertyName == ms_kPropSectorIndex ||
        kPropertyName == ms_kPropMaskSize) 
    {
        bCountSet = false;
    } 
    else 
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainSectorComponent::GetElementCount(
    const NiFixedString& kPropertyName, NiUInt32& uiCount) const 
{

    if (kPropertyName == ms_kPropDeformable || 
        kPropertyName == ms_kPropSceneRoot ||
        kPropertyName == ms_kPropTerrainID ||
        kPropertyName == ms_kPropMaskSize) 
    {
        uiCount = 1;
    }
    else if (kPropertyName == ms_kPropSectorIndex) 
    {
        uiCount = 2;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainSectorComponent::IsCollection(
    const NiFixedString& kPropertyName, bool& bIsCollection) const 
{    
    if (kPropertyName == ms_kPropDeformable || 
        kPropertyName == ms_kPropSceneRoot ||
        kPropertyName == ms_kPropTerrainID ||
        kPropertyName == ms_kPropMaskSize) 
    {
        bIsCollection = false;
    } 
    else if (kPropertyName == ms_kPropSectorIndex) 
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
NiBool NiTerrainSectorComponent::GetPropertyData(
    const NiFixedString& kPropertyName,
    NiFixedString& kData, NiUInt32 uiIndex) const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kPropTerrainID)
    {
        kData = GetPropTerrainID();
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainSectorComponent::SetPropertyData(
    const NiFixedString& kPropertyName,
    const NiFixedString& kData, NiUInt32 uiIndex) 
{
    if (uiIndex != 0) 
    {
        return false;
    }
    else if (kPropertyName == ms_kPropTerrainID)
    {
        SetPropTerrainID(kData);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainSectorComponent::GetPropertyData(
    const NiFixedString& kPropertyName,
    bool& bData, NiUInt32 uiIndex) const
{    
    if (uiIndex != 0) 
    {
        return false;
    }
    else if (kPropertyName == ms_kPropDeformable)
    {
        bData = NIBOOL_IS_TRUE(GetPropDeformable());
    }
    else if (kPropertyName == ms_kPropHasChanged)
    {
        bData = NIBOOL_IS_TRUE(m_bHasChanged);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainSectorComponent::SetPropertyData(
    const NiFixedString& kPropertyName,
    bool bData, NiUInt32 uiIndex) 
{
    if (uiIndex != 0) 
    {
        return false;
    }
    else if (kPropertyName == ms_kPropDeformable)
    {
        SetPropDeformable(bData);
    }
    else if (kPropertyName == ms_kPropHasChanged)
    {
        m_bHasChanged = bData;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTerrainSectorComponent::GetPropertyData(
    const NiFixedString& kPropertyName, 
    NiObject*& pkData, NiUInt32 uiIndex) const
{
    if (uiIndex != 0) 
    {
        return false;
    }
    else if (kPropertyName == ms_kPropSceneRoot)
    {
        if (m_spSector) 
        {
            pkData = m_spSector;
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
NiBool NiTerrainSectorComponent::SetPropertyData(
    const NiFixedString&, NiObject*, NiUInt32)
{
    // Cannot set 'ms_kPropSceneRoot'
    return false;
}
//---------------------------------------------------------------------------
NiBool NiTerrainSectorComponent::GetPropertyData(
    const NiFixedString& kPropertyName,
    NiUInt32& uiData, NiUInt32 uiIndex) const
{
    if (kPropertyName == ms_kPropSectorIndex)
    {
        if (uiIndex == 0) 
        {
            uiData = GetPropSectorIndexX();
            return true;
        }
        else if (uiIndex == 1) 
        {
            uiData = GetPropSectorIndexY();
            return true;
        }
    }
    else if (kPropertyName == ms_kPropMaskSize && uiIndex == 0)
    {
        uiData = GetPropMaskSize();
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
NiBool NiTerrainSectorComponent::SetPropertyData(
    const NiFixedString& kPropertyName,
    NiUInt32 uiData, NiUInt32 uiIndex)
{
    if (kPropertyName == ms_kPropSectorIndex)
    {
        if (uiIndex == 0) 
        {
            SetPropSectorIndexX((NiInt16)uiData);
            return true;
        }
        else if (uiIndex == 1) 
        {
            SetPropSectorIndexY((NiInt16)uiData);
            return true;
        }
    }
    else if (kPropertyName == ms_kPropMaskSize && uiIndex == 0)
    {
        return SetPropMaskSize(uiData);
    }
    return false;
}
