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

// Precompiled Header
#include "NiEntityPCH.h"

#include "NiGeneralEntity.h"
#include "NiPrefabComponent.h"

NiFixedString NiGeneralEntity::ms_kClassName;

//---------------------------------------------------------------------------
void NiGeneralEntity::_SDMInit()
{
    ms_kClassName = "NiGeneralEntity";
}
//---------------------------------------------------------------------------
void NiGeneralEntity::_SDMShutdown()
{
    ms_kClassName = NULL;
}
//---------------------------------------------------------------------------
NiGeneralEntity::NiGeneralEntity() : m_kComponents(0), m_pkPrefabRoot(NULL),
    m_bHidden(false)
{
}
//---------------------------------------------------------------------------
NiGeneralEntity::NiGeneralEntity(const NiFixedString& kName,
    const NiUniqueID& kTemplateID, unsigned int uiComponentArraySize) :
    m_kComponents(uiComponentArraySize), m_kName(kName), m_pkPrefabRoot(NULL),
    m_kTemplateID(kTemplateID), m_bHidden(false)
{
}
//---------------------------------------------------------------------------
// NiEntityInterface overrides.
//---------------------------------------------------------------------------
NiEntityInterface* NiGeneralEntity::Clone(
    const NiFixedString& kNewName,
    bool bInheritProperties)
{
    NiGeneralEntity* pkClone = NiNew NiGeneralEntity(kNewName,
        m_kTemplateID, m_kComponents.GetSize());

    if (!bInheritProperties)
    {
        pkClone->SetTags(GetTags());
    }

    pkClone->m_pkPrefabRoot = NULL;

    if (bInheritProperties)
    {
        pkClone->m_spMasterEntity = this;
    }
    else
    {
        pkClone->m_spMasterEntity = m_spMasterEntity;
    }

    unsigned int uiSize_Components = m_kComponents.GetSize();
    for (unsigned int ui = 0; ui < uiSize_Components; ui++)
    {
        pkClone->m_kComponents.Add(m_kComponents.GetAt(ui)->Clone(
            bInheritProperties));
    }

    return pkClone;
}
//---------------------------------------------------------------------------
NiEntityInterface* NiGeneralEntity::GetMasterEntity() const
{
    return m_spMasterEntity;
}
//---------------------------------------------------------------------------
void NiGeneralEntity::SetMasterEntity(NiEntityInterface* pkMasterEntity)
{
    m_spMasterEntity = pkMasterEntity;
}
//---------------------------------------------------------------------------
void NiGeneralEntity::ReplaceMasterEntity(NiEntityInterface* pkMasterEntity)
{
    if (pkMasterEntity)
    {
        // Remove components on this entity that are not on the master entity.
        NiTPrimitiveSet<NiEntityComponentInterface*> kComponentsToRemove(0);

        unsigned int uiSize_Components = m_kComponents.GetSize();
        for (unsigned int ui = 0; ui < uiSize_Components; ui++)
        {
            NiEntityComponentInterface* pkComponent = m_kComponents.GetAt(ui);
            bool bFound = false;
            for (unsigned int uj = 0;
                uj < pkMasterEntity->GetComponentCount(); uj++)
            {
                NiEntityComponentInterface* pkMasterComponent =
                    pkMasterEntity->GetComponentAt(uj);
                if (pkComponent->GetTemplateID() ==
                    pkMasterComponent->GetTemplateID())
                {
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
            {
                kComponentsToRemove.Add(pkComponent);
            }
        }

        unsigned int uiSize_ComponentsToRemove = kComponentsToRemove.GetSize();
        for (unsigned int ui = 0; ui < uiSize_ComponentsToRemove; ui++)
        {
            RemoveComponent(kComponentsToRemove.GetAt(ui), false);
        }

        // Add components on the master entity that are not on this entity and
        // replace master component for components that are.
        for (unsigned int ui = 0; ui < pkMasterEntity->GetComponentCount();
            ui++)
        {
            NiEntityComponentInterface* pkMasterComponent =
                pkMasterEntity->GetComponentAt(ui);
            NiEntityComponentInterface* pkComponent = NULL;
            for (unsigned int uj = 0; uj < m_kComponents.GetSize(); uj++)
            {
                NiEntityComponentInterface* pkTempComponent =
                    m_kComponents.GetAt(uj);
                if (pkMasterComponent->GetTemplateID() ==
                    pkTempComponent->GetTemplateID())
                {
                    pkComponent = pkTempComponent;
                    break;
                }
            }
            if (pkComponent)
            {
                pkComponent->SetMasterComponent(pkMasterComponent);
            }
            else
            {
                AddComponent(pkMasterComponent->Clone(true), false);
            }
        }
    }
    else
    {
        // Clear the master components from all subcomponents.
        for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
        {
            m_kComponents.GetAt(ui)->SetMasterComponent(NULL);
        }
    }

    SetMasterEntity(pkMasterEntity);
}
//---------------------------------------------------------------------------
NiEntityInterface* NiGeneralEntity::GetPrefabRoot() const
{
    return m_pkPrefabRoot;
}
//---------------------------------------------------------------------------
void NiGeneralEntity::SetPrefabRoot(NiEntityInterface* pkPrefabRoot,
    bool bAddToPrefabEntitiesProperty)
{
    if (pkPrefabRoot != m_pkPrefabRoot)
    {
        // If the prefab root is not null we need to remove this entity
        // from it's list
        if (m_pkPrefabRoot)
        {
            unsigned int uiCount = 0;
            NIVERIFY(m_pkPrefabRoot->GetElementCount(
                NiPrefabComponent::PROP_PREFAB_ENTITIES(), uiCount));
            for (unsigned int ui = 0; ui < uiCount; ++ui)
            {
                NiEntityInterface* pkEntity = NULL;
                NIVERIFY(m_pkPrefabRoot->GetPropertyData(
                    NiPrefabComponent::PROP_PREFAB_ENTITIES(), pkEntity, ui));
                if (pkEntity == this)
                {
                    NIVERIFY(m_pkPrefabRoot->SetPropertyData(
                        NiPrefabComponent::PROP_PREFAB_ENTITIES(),
                        (NiEntityInterface*) NULL, ui));
                    break;
                }
            }
        }

        m_pkPrefabRoot = pkPrefabRoot;

        // If a new prefab root is specified, add this entity to its "Prefab
        // Entities" property.
        if (m_pkPrefabRoot && bAddToPrefabEntitiesProperty)
        {
            unsigned int uiCount = 0;
            NIVERIFY(m_pkPrefabRoot->GetElementCount(
                NiPrefabComponent::PROP_PREFAB_ENTITIES(), uiCount));
            NIVERIFY(m_pkPrefabRoot->SetPropertyData(
                NiPrefabComponent::PROP_PREFAB_ENTITIES(), this, uiCount));
        }
    }
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::GetHidden() const
{
    return m_bHidden;
}
//---------------------------------------------------------------------------
void NiGeneralEntity::SetHidden(bool bHidden)
{
    m_bHidden = bHidden;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::SupportsComponents()
{
    return true;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::AddComponent(NiEntityComponentInterface* pkComponent,
    bool bPerformErrorChecking)
{
    NIASSERT(pkComponent);
    if (!bPerformErrorChecking || pkComponent->CanAttachToEntity(this))
    {
        m_kComponents.Add(pkComponent);
        pkComponent->SetSourceFilename(GetSourceFilename());
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
unsigned int NiGeneralEntity::GetComponentCount() const
{
    return m_kComponents.GetSize();
}
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiGeneralEntity::GetComponentAt(
    unsigned int uiIndex) const
{
    NIASSERT(uiIndex < m_kComponents.GetSize());
    return m_kComponents.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiGeneralEntity::GetComponentByTemplateID(
    const NiUniqueID& kTemplateID) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        NiEntityComponentInterface* pkComponent = m_kComponents.GetAt(ui);
        if (pkComponent->GetTemplateID() == kTemplateID)
        {
            return pkComponent;
        }
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::RemoveComponent(
    NiEntityComponentInterface* pkComponent, bool bPerformErrorChecking)
{
    NIASSERT(pkComponent);
    int iIndex = m_kComponents.Find(pkComponent);
    if (iIndex != -1 && (!bPerformErrorChecking ||
        pkComponent->CanDetachFromEntity(this)))
    {
        m_kComponents.GetAt(iIndex)->SetSourceFilename(NULL);
        m_kComponents.RemoveAt(iIndex);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::RemoveComponentAt(unsigned int uiIndex,
    bool bPerformErrorChecking)
{
    NIASSERT(uiIndex < m_kComponents.GetSize());
    NiEntityComponentInterface* pkComponent = m_kComponents.GetAt(uiIndex);
    if (!bPerformErrorChecking || pkComponent->CanDetachFromEntity(this))
    {
        m_kComponents.GetAt(uiIndex)->SetSourceFilename(NULL);
        m_kComponents.RemoveAt(uiIndex);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiGeneralEntity::RemoveAllComponents()
{
    const unsigned int uiComponentCount = m_kComponents.GetSize();
    for (unsigned int ui = 0; ui < uiComponentCount; ++ui)
    {
        m_kComponents.GetAt(ui)->SetSourceFilename(NULL);
    }
    m_kComponents.RemoveAll();
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::IsComponentProperty(
    const NiFixedString& kPropertyName, bool& bIsComponentProperty)
{
    // For NiGeneralEntity, if the property name exists, then it is a 
    // component property. Other NiEntityInterface derived classes may
    // have non-component properties.

    NiTObjectSet<NiFixedString> kPropertyNames;
    GetPropertyNames(kPropertyNames);

    for (unsigned int ui = 0; ui < kPropertyNames.GetSize(); ui++)
    {
        const NiFixedString& kCurrPropertyName = kPropertyNames.GetAt(ui);

        if (kCurrPropertyName == kPropertyName)
        {
            bIsComponentProperty = true;
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
// NiEntityPropertyInterface overrides.
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::SetTemplateID(const NiUniqueID& kTemplateID)
{
    m_kTemplateID = kTemplateID;
    return true;
}
//---------------------------------------------------------------------------
NiUniqueID NiGeneralEntity::GetTemplateID()
{
    return m_kTemplateID;
}
//---------------------------------------------------------------------------
void NiGeneralEntity::AddReference()
{
    this->IncRefCount();
}
//---------------------------------------------------------------------------
void NiGeneralEntity::RemoveReference()
{
    this->DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiGeneralEntity::GetClassName() const
{
    return ms_kClassName; 
}
//---------------------------------------------------------------------------
NiFixedString NiGeneralEntity::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::SetName(const NiFixedString& kName)
{
    m_kName = kName;
    return true;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::IsAnimated() const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->IsAnimated())
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void NiGeneralEntity::Update(NiEntityPropertyInterface*,
    float fTime, NiEntityErrorInterface* pkErrors,
    NiExternalAssetManager* pkAssetManager)
{
    // Update all components.
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        m_kComponents.GetAt(ui)->Update(
            this, fTime, pkErrors, pkAssetManager);
    }
}
//---------------------------------------------------------------------------
void NiGeneralEntity::BuildVisibleSet(NiEntityRenderingContext* 
    pkRenderingContext, NiEntityErrorInterface* pkErrors)
{
    if (!m_bHidden)
    {
        for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
        {
            m_kComponents.GetAt(ui)->BuildVisibleSet(pkRenderingContext,
                pkErrors);
        }
    }
}
//---------------------------------------------------------------------------
void NiGeneralEntity::GetPropertyNames(
    NiTObjectSet<NiFixedString>& kPropertyNames) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        m_kComponents.GetAt(ui)->GetPropertyNames(kPropertyNames);
    }
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::CanResetProperty(const NiFixedString& kPropertyName,
    bool& bCanReset) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->CanResetProperty(kPropertyName,
            bCanReset))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::ResetProperty(const NiFixedString& kPropertyName)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->ResetProperty(kPropertyName))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::MakePropertyUnique(const NiFixedString& kPropertyName,
    bool& bMadeUnique)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->MakePropertyUnique(kPropertyName,
            bMadeUnique))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::GetDisplayName(const NiFixedString& kPropertyName,
    NiFixedString& kDisplayName) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->GetDisplayName(kPropertyName,
            kDisplayName))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::SetDisplayName(const NiFixedString& kPropertyName,
    const NiFixedString& kDisplayName)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->SetDisplayName(kPropertyName,
            kDisplayName))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::GetPrimitiveType(const NiFixedString& kPropertyName,
    NiFixedString& kPrimitiveType) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->GetPrimitiveType(kPropertyName,
            kPrimitiveType))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::SetPrimitiveType(const NiFixedString& kPropertyName,
    const NiFixedString& kPrimitiveType)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->SetPrimitiveType(kPropertyName,
            kPrimitiveType))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::GetSemanticType(const NiFixedString& kPropertyName,
    NiFixedString& kSemanticType) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->GetSemanticType(kPropertyName,
            kSemanticType))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::SetSemanticType(const NiFixedString& kPropertyName,
    const NiFixedString& kSemanticType)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->SetSemanticType(kPropertyName,
            kSemanticType))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::GetDescription(const NiFixedString& kPropertyName,
    NiFixedString& kDescription) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->GetDescription(kPropertyName,
            kDescription))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::SetDescription(const NiFixedString& kPropertyName,
    const NiFixedString& kDescription)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->SetDescription(kPropertyName,
            kDescription))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::GetCategory(const NiFixedString& kPropertyName,
    NiFixedString& kCategory) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->GetCategory(kPropertyName,
            kCategory))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::IsPropertyReadOnly(
    const NiFixedString& kPropertyName, bool& bIsReadOnly)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->IsPropertyReadOnly(kPropertyName,
            bIsReadOnly))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::IsPropertyUnique(
    const NiFixedString& kPropertyName, bool& bIsUnique)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->IsPropertyUnique(kPropertyName,
            bIsUnique))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::IsPropertySerializable(
    const NiFixedString& kPropertyName, bool& bIsSerializable)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->IsPropertySerializable(kPropertyName,
            bIsSerializable))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::IsPropertyInheritable(
    const NiFixedString& kPropertyName, bool& bIsInheritable)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->IsPropertyInheritable(kPropertyName,
            bIsInheritable))
        {
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::IsExternalAssetPath(
    const NiFixedString& kPropertyName, unsigned int uiIndex,
    bool& bIsExternalAssetPath) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->IsExternalAssetPath(kPropertyName,
            uiIndex, bIsExternalAssetPath))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::GetElementCount(const NiFixedString& kPropertyName,
    unsigned int& uiCount) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->GetElementCount(kPropertyName, uiCount))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::SetElementCount(const NiFixedString& kPropertyName,
    unsigned int uiCount, bool& bCountSet)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->SetElementCount(kPropertyName, uiCount,
            bCountSet))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::IsCollection(const NiFixedString& kPropertyName,
    bool& bIsCollection) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->IsCollection(kPropertyName,
            bIsCollection))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::GetPropertyData(const NiFixedString& kPropertyName,
    float& fData, unsigned int uiIndex) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->GetPropertyData(kPropertyName, fData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::GetPropertyData(const NiFixedString& kPropertyName,
    bool& bData, unsigned int uiIndex) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->GetPropertyData(kPropertyName, bData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::GetPropertyData(const NiFixedString& kPropertyName,
    int& iData, unsigned int uiIndex) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->GetPropertyData(kPropertyName, iData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::GetPropertyData(const NiFixedString& kPropertyName,
    unsigned int& uiData, unsigned int uiIndex) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->GetPropertyData(kPropertyName, uiData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::GetPropertyData(const NiFixedString& kPropertyName,
    short& sData, unsigned int uiIndex) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->GetPropertyData(kPropertyName, sData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::GetPropertyData(const NiFixedString& kPropertyName,
    unsigned short& usData, unsigned int uiIndex) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->GetPropertyData(kPropertyName, usData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::GetPropertyData(const NiFixedString& kPropertyName,
    NiFixedString& kData, unsigned int uiIndex) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->GetPropertyData(kPropertyName, kData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::GetPropertyData(const NiFixedString& kPropertyName,
    NiPoint2& kData, unsigned int uiIndex) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->GetPropertyData(kPropertyName, kData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::GetPropertyData(const NiFixedString& kPropertyName,
    NiPoint3& kData, unsigned int uiIndex) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->GetPropertyData(kPropertyName, kData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::GetPropertyData(const NiFixedString& kPropertyName,
    NiQuaternion& kData, unsigned int uiIndex) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->GetPropertyData(kPropertyName, kData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::GetPropertyData(const NiFixedString& kPropertyName,
    NiMatrix3& kData, unsigned int uiIndex) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->GetPropertyData(kPropertyName, kData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::GetPropertyData(const NiFixedString& kPropertyName,
    NiColor& kData, unsigned int uiIndex) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->GetPropertyData(kPropertyName, kData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::GetPropertyData(const NiFixedString& kPropertyName,
    NiColorA& kData, unsigned int uiIndex) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->GetPropertyData(kPropertyName, kData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::GetPropertyData(const NiFixedString& kPropertyName,
    NiObject*& pkData, unsigned int uiIndex) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->GetPropertyData(kPropertyName, pkData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::GetPropertyData(const NiFixedString& kPropertyName,
    NiEntityInterface*& pkData, unsigned int uiIndex) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->GetPropertyData(kPropertyName, pkData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::GetPropertyData(const NiFixedString& kPropertyName,
    void*& pvData, size_t& stDataSizeInBytes, unsigned int uiIndex) const
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->GetPropertyData(kPropertyName, pvData,
            stDataSizeInBytes, uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::SetPropertyData(const NiFixedString& kPropertyName,
    float fData, unsigned int uiIndex)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->SetPropertyData(kPropertyName, fData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::SetPropertyData(const NiFixedString& kPropertyName,
    bool bData, unsigned int uiIndex)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->SetPropertyData(kPropertyName, bData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::SetPropertyData(const NiFixedString& kPropertyName,
    int iData, unsigned int uiIndex)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->SetPropertyData(kPropertyName, iData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::SetPropertyData(const NiFixedString& kPropertyName,
    unsigned int uiData, unsigned int uiIndex)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->SetPropertyData(kPropertyName, uiData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::SetPropertyData(const NiFixedString& kPropertyName,
    short sData, unsigned int uiIndex)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->SetPropertyData(kPropertyName, sData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::SetPropertyData(const NiFixedString& kPropertyName,
    unsigned short usData, unsigned int uiIndex)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->SetPropertyData(kPropertyName, usData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::SetPropertyData(const NiFixedString& kPropertyName,
    const NiFixedString& kData, unsigned int uiIndex)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->SetPropertyData(kPropertyName, kData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::SetPropertyData(const NiFixedString& kPropertyName,
    const NiPoint2& kData, unsigned int uiIndex)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->SetPropertyData(kPropertyName, kData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::SetPropertyData(const NiFixedString& kPropertyName,
    const NiPoint3& kData, unsigned int uiIndex)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->SetPropertyData(kPropertyName, kData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::SetPropertyData(const NiFixedString& kPropertyName,
    const NiQuaternion& kData, unsigned int uiIndex)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->SetPropertyData(kPropertyName, kData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::SetPropertyData(const NiFixedString& kPropertyName,
    const NiMatrix3& kData, unsigned int uiIndex)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->SetPropertyData(kPropertyName, kData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::SetPropertyData(const NiFixedString& kPropertyName,
    const NiColor& kData, unsigned int uiIndex)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->SetPropertyData(kPropertyName, kData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::SetPropertyData(const NiFixedString& kPropertyName,
    const NiColorA& kData, unsigned int uiIndex)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->SetPropertyData(kPropertyName, kData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::SetPropertyData(const NiFixedString& kPropertyName,
    NiObject* pkData, unsigned int uiIndex)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->SetPropertyData(kPropertyName, pkData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::SetPropertyData(const NiFixedString& kPropertyName,
    NiEntityInterface* pkData, unsigned int uiIndex)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->SetPropertyData(kPropertyName, pkData,
            uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralEntity::SetPropertyData(const NiFixedString& kPropertyName,
    const void* pvData, size_t stDataSizeInBytes, unsigned int uiIndex)
{
    for (unsigned int ui = 0; ui < m_kComponents.GetSize(); ui++)
    {
        if (m_kComponents.GetAt(ui)->SetPropertyData(kPropertyName, pvData,
            stDataSizeInBytes, uiIndex))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void NiGeneralEntity::SetSourceFilename(const NiFixedString& kSourceFilename)
{
    NiEntityInterface::SetSourceFilename(kSourceFilename);

    const unsigned int uiComponentCount = m_kComponents.GetSize();
    for (unsigned int ui = 0; ui < uiComponentCount; ++ui)
    {
        m_kComponents.GetAt(ui)->SetSourceFilename(kSourceFilename);
    }
}
//---------------------------------------------------------------------------
