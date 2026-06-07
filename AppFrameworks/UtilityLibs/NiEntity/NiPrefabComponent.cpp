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

#include "NiPrefabComponent.h"

NiFixedString NiPrefabComponent::ms_kClassName;
NiFixedString NiPrefabComponent::ms_kComponentName;
NiFixedString NiPrefabComponent::ms_kPropPrefabPath;
NiFixedString NiPrefabComponent::ms_kPropPrefabEntities;
NiFixedString NiPrefabComponent::ms_kDescPrefabPath;
NiFixedString NiPrefabComponent::ms_kDescPrefabEntities;

//---------------------------------------------------------------------------
void NiPrefabComponent::_SDMInit()
{
    ms_kClassName = "NiPrefabComponent";
    ms_kComponentName = "Prefab";
    ms_kPropPrefabPath = "Prefab Path";
    ms_kPropPrefabEntities = "Prefab Entities";
    ms_kDescPrefabPath = "The path to the scene file corresponding to this "
        "prefab.";
    ms_kDescPrefabEntities = "The entities that are contained in this prefab.";
}
//---------------------------------------------------------------------------
void NiPrefabComponent::_SDMShutdown()
{
    ms_kClassName = NULL;
    ms_kComponentName = NULL;
    ms_kPropPrefabPath = NULL;
    ms_kPropPrefabEntities = NULL;
    ms_kDescPrefabPath = NULL;
    ms_kDescPrefabEntities = NULL;
}
//---------------------------------------------------------------------------
NiPrefabComponent::NiPrefabComponent() : m_kPrefabEntities(0, 8)
{
}
//---------------------------------------------------------------------------
NiPrefabComponent::NiPrefabComponent(const NiFixedString& kPrefabPath) :
    m_kPrefabPath(kPrefabPath),
    m_kPrefabEntities(0, 8)
{
}
//---------------------------------------------------------------------------
// NiEntityComponentInterface overrides.
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiPrefabComponent::Clone(bool)
{
    NiPrefabComponent* pkClone = NiNew NiPrefabComponent(m_kPrefabPath);

    // We don't support cloning a prefab component, just make a copy
    // and return, don't copy the entities in it.
    return pkClone;
}
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiPrefabComponent::GetMasterComponent() const
{
    // This component does not have a master component.
    return NULL;
}
//---------------------------------------------------------------------------
void NiPrefabComponent::SetMasterComponent(NiEntityComponentInterface*)
{
    // This component does not support setting a master component.
}
//---------------------------------------------------------------------------
void NiPrefabComponent::GetDependentPropertyNames(
    NiTObjectSet<NiFixedString>&)
{
    // This component does not have any dependent properties.
}
//---------------------------------------------------------------------------
// NiEntityPropertyInterface overrides.
//---------------------------------------------------------------------------
void NiPrefabComponent::AddReference()
{
    IncRefCount();
}
//---------------------------------------------------------------------------
void NiPrefabComponent::RemoveReference()
{
    DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiPrefabComponent::GetClassName() const
{
    return ms_kClassName;
}
//---------------------------------------------------------------------------
NiUniqueID NiPrefabComponent::GetTemplateID()
{
    // {7EF51180-4BD8-41fd-AF81-EEE4BEFC3C89}
    return NiUniqueID(0x80, 0x11, 0xF5, 0x7E, 0xD8, 0x4B, 0xFD, 0x41, 0x81,
        0xAF, 0xEE, 0xE4, 0xBE, 0xFC, 0x3C, 0x89);
}
//---------------------------------------------------------------------------
NiBool NiPrefabComponent::SetTemplateID(const NiUniqueID&)
{
    // This component does not support setting the template ID.
    return false;
}
//---------------------------------------------------------------------------
NiFixedString NiPrefabComponent::GetName() const
{
    return ms_kComponentName;
}
//---------------------------------------------------------------------------
NiBool NiPrefabComponent::SetName(const NiFixedString&)
{
    // This component does not support setting its name.
    return false;
}
//---------------------------------------------------------------------------
void NiPrefabComponent::Update(
    NiEntityPropertyInterface*,
    float,
    NiEntityErrorInterface*,
    NiExternalAssetManager*)
{
    // This component has nothing to do during Update.
}
//---------------------------------------------------------------------------
void NiPrefabComponent::BuildVisibleSet(
    NiEntityRenderingContext*,
    NiEntityErrorInterface*)
{
    // This component has nothing to do during BuildVisibleSet.
}
//---------------------------------------------------------------------------
void NiPrefabComponent::GetPropertyNames(
    NiTObjectSet<NiFixedString>& kPropertyNames) const
{
    kPropertyNames.Add(ms_kPropPrefabPath);
    kPropertyNames.Add(ms_kPropPrefabEntities);
}
//---------------------------------------------------------------------------
NiBool NiPrefabComponent::CanResetProperty(
    const NiFixedString& kPropertyName,
    bool& bCanReset) const
{
    if (kPropertyName == ms_kPropPrefabPath ||
        kPropertyName == ms_kPropPrefabEntities)
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
NiBool NiPrefabComponent::ResetProperty(const NiFixedString&)
{
    // No properties can be reset.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiPrefabComponent::MakePropertyUnique(
    const NiFixedString& kPropertyName,
    bool& bMadeUnique)
{
    if (kPropertyName == ms_kPropPrefabPath ||
        kPropertyName == ms_kPropPrefabEntities)
    {
        // No properties are inherited, so that are all already unique.
        bMadeUnique = false;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiPrefabComponent::GetDisplayName(
    const NiFixedString& kPropertyName,
    NiFixedString& kDisplayName) const
{
    if (kPropertyName == ms_kPropPrefabPath ||
        kPropertyName == ms_kPropPrefabEntities)
    {
        // None of the properties should be displayed; they are programmatic
        // only.
        kDisplayName = NULL;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiPrefabComponent::SetDisplayName(
    const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the display name to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiPrefabComponent::GetPrimitiveType(
    const NiFixedString& kPropertyName,
    NiFixedString& kPrimitiveType) const
{
    if (kPropertyName == ms_kPropPrefabPath)
    {
        kPrimitiveType = PT_STRING;
    }
    else if (kPropertyName == ms_kPropPrefabEntities)
    {
        kPrimitiveType = PT_ENTITYPOINTER;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiPrefabComponent::SetPrimitiveType(
    const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the primitive type to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiPrefabComponent::GetSemanticType(
    const NiFixedString& kPropertyName,
    NiFixedString& kSemanticType) const
{
    if (kPropertyName == ms_kPropPrefabPath)
    {
        kSemanticType = "Filename";
    }
    else if (kPropertyName == ms_kPropPrefabEntities)
    {
        kSemanticType = PT_ENTITYPOINTER;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiPrefabComponent::SetSemanticType(
    const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the semantic type to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiPrefabComponent::GetDescription(
    const NiFixedString& kPropertyName,
    NiFixedString& kDescription) const
{
    if (kPropertyName == ms_kPropPrefabPath)
    {
        kDescription = ms_kDescPrefabPath;
    }
    else if (kPropertyName == ms_kPropPrefabEntities)
    {
        kDescription = ms_kDescPrefabEntities;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiPrefabComponent::SetDescription(
    const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the description to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiPrefabComponent::GetCategory(
    const NiFixedString& kPropertyName,
    NiFixedString& kCategory) const
{
    if (kPropertyName == ms_kPropPrefabPath ||
        kPropertyName == ms_kPropPrefabEntities)
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
NiBool NiPrefabComponent::IsPropertyReadOnly(
    const NiFixedString& kPropertyName,
    bool& bIsReadOnly)
{
    if (kPropertyName == ms_kPropPrefabPath ||
        kPropertyName == ms_kPropPrefabEntities)
    {
        bIsReadOnly = false;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiPrefabComponent::IsPropertyUnique(
    const NiFixedString& kPropertyName,
    bool& bIsUnique)
{
    if (kPropertyName == ms_kPropPrefabPath ||
        kPropertyName == ms_kPropPrefabEntities)
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
NiBool NiPrefabComponent::IsPropertySerializable(
    const NiFixedString& kPropertyName,
    bool& bIsSerializable)
{
    if (kPropertyName == ms_kPropPrefabPath ||
        kPropertyName == ms_kPropPrefabEntities)
    {
        bool bIsUnique;
        NIVERIFY(IsPropertyUnique(kPropertyName, bIsUnique));

        bool bIsReadOnly;
        NIVERIFY(IsPropertyReadOnly(kPropertyName, bIsReadOnly));

        bIsSerializable = bIsUnique && !bIsReadOnly;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiPrefabComponent::IsPropertyInheritable(
    const NiFixedString& kPropertyName,
    bool& bIsInheritable)
{
    if (kPropertyName == ms_kPropPrefabPath ||
        kPropertyName == ms_kPropPrefabEntities)
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
NiBool NiPrefabComponent::IsExternalAssetPath(
    const NiFixedString& kPropertyName,
    unsigned int,
    bool& bIsExternalAssetPath) const
{
    if (kPropertyName == ms_kPropPrefabPath)
    {
        bIsExternalAssetPath = true;
    }
    else if (kPropertyName == ms_kPropPrefabEntities)
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
NiBool NiPrefabComponent::GetElementCount(
    const NiFixedString& kPropertyName,
    unsigned int& uiCount) const
{
    if (kPropertyName == ms_kPropPrefabPath)
    {
        uiCount = 1;
    }
    else if (kPropertyName == ms_kPropPrefabEntities)
    {
        uiCount = m_kPrefabEntities.GetSize();
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiPrefabComponent::SetElementCount(
    const NiFixedString& kPropertyName,
    unsigned int uiCount,
    bool& bCountSet)
{
    if (kPropertyName == ms_kPropPrefabPath)
    {
        bCountSet = false;
    }
    else if (kPropertyName == ms_kPropPrefabEntities)
    {
        const unsigned int uiOrigCount = m_kPrefabEntities.GetSize();
        for (unsigned int ui = uiCount; ui < uiOrigCount; ++ui)
        {
            SetPropertyData(kPropertyName, (NiEntityInterface*) NULL, ui);
        }
        m_kPrefabEntities.SetSize(uiCount);
        bCountSet = true;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiPrefabComponent::IsCollection(
    const NiFixedString& kPropertyName,
    bool& bIsCollection) const
{
    if (kPropertyName == ms_kPropPrefabPath)
    {
        bIsCollection = false;
    }
    else if (kPropertyName == ms_kPropPrefabEntities)
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
NiBool NiPrefabComponent::GetPropertyData(
    const NiFixedString& kPropertyName,
    NiFixedString& kData,
    unsigned int uiIndex) const
{
    if (uiIndex > 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kPropPrefabPath)
    {
        kData = GetPrefabPath();
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiPrefabComponent::SetPropertyData(
    const NiFixedString& kPropertyName,
    const NiFixedString& kData,
    unsigned int uiIndex)
{
    if (uiIndex > 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kPropPrefabPath)
    {
        SetPrefabPath(kData);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiPrefabComponent::GetPropertyData(
    const NiFixedString& kPropertyName,
    NiEntityInterface*& pkData,
    unsigned int uiIndex) const
{
    if (kPropertyName == ms_kPropPrefabEntities)
    {
        if (uiIndex < m_kPrefabEntities.GetSize())
        {
            pkData = m_kPrefabEntities.GetAt(uiIndex);
        }
        else
        {
            pkData = NULL;
        }
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiPrefabComponent::SetPropertyData(
    const NiFixedString& kPropertyName,
    NiEntityInterface* pkData,
    unsigned int uiIndex)
{
    if (kPropertyName == ms_kPropPrefabEntities)
    {
        m_kPrefabEntities.SetAtGrow(uiIndex, pkData);
        m_kPrefabEntities.UpdateSize();
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
