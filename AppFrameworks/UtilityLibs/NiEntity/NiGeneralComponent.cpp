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

#include "NiGeneralComponent.h"
#include "NiEntityErrorInterface.h"

const unsigned int NiGeneralComponent::INVALID_INDEX = (unsigned int) -1;

NiFixedString NiGeneralComponent::ms_kClassName;

//---------------------------------------------------------------------------
// NiEntityComponentInterface overrides.
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiGeneralComponent::Clone(bool bInheritProperties)
{
    if (bInheritProperties)
    {
        return NiNew NiGeneralComponent(m_kName, m_kTemplateID, 
            m_kProperties.GetSize(), this);
    }
    else
    {
        unsigned int uiSize = m_kProperties.GetSize();

        NiGeneralComponent* pkClone = NiNew NiGeneralComponent(m_kName,
            m_kTemplateID, uiSize, m_spMasterComponent);

        for (unsigned int ui = 0; ui < uiSize; ui++)
        {
            pkClone->m_kProperties.Add(m_kProperties.GetAt(ui)->Clone());
        }
        return pkClone;
    }
}
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiGeneralComponent::GetMasterComponent() const
{
    return m_spMasterComponent;
}
//---------------------------------------------------------------------------
void NiGeneralComponent::SetMasterComponent(
    NiEntityComponentInterface* pkMasterComponent)
{
    if (pkMasterComponent)
    {
        NIASSERT(pkMasterComponent->GetClassName() == GetClassName());
        m_spMasterComponent = (NiGeneralComponent*) pkMasterComponent;

        // Remove all properties that do not exist on the new master
        // component.
        NiTPrimitiveSet<NiGeneralComponentProperty*> kPropertiesToRemove(
            m_kProperties.GetSize());

        unsigned int uiPropertiesSize = m_kProperties.GetSize();
        for (unsigned int ui = 0; ui < uiPropertiesSize; ui++)
        {
            NiGeneralComponentProperty* pkProperty = m_kProperties.GetAt(ui);
            if (!m_spMasterComponent->FindPropertyByName(
                pkProperty->GetPropertyName(), true))
            {
                kPropertiesToRemove.Add(pkProperty);
            }
        }

        unsigned int uiPropertiesToRemoveSize = kPropertiesToRemove.GetSize();
        for (unsigned int ui = 0; ui < uiPropertiesToRemoveSize; ui++)
        {
            unsigned int uiIndex = FindPropertyIndexByName(
                kPropertiesToRemove.GetAt(ui)->GetPropertyName());
            if (uiIndex != INVALID_INDEX)
            {
                m_kProperties.RemoveAt(uiIndex);
            }
        }
    }
    else
    {
        NiTObjectSet<NiFixedString> kPropertyNames(10);
        GetPropertyNames(kPropertyNames);
        unsigned int uiSize = kPropertyNames.GetSize();
        for (unsigned int ui = 0; ui < uiSize; ui++)
        {
            bool bMadeUnique;
            NIVERIFY(NIBOOL_IS_TRUE(
                MakePropertyUnique(kPropertyNames.GetAt(ui), bMadeUnique)));
        }
        m_spMasterComponent = NULL;
    }
}
//---------------------------------------------------------------------------
void NiGeneralComponent::GetDependentPropertyNames(
    NiTObjectSet<NiFixedString>&)
{
    // This component has no dependent properties.
}
//---------------------------------------------------------------------------
// NiEntityPropertyInterface overrides.
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::SetTemplateID(const NiUniqueID& kTemplateID)
{
    m_kTemplateID = kTemplateID;
    return true;
}
//---------------------------------------------------------------------------
NiUniqueID  NiGeneralComponent::GetTemplateID()
{
    return m_kTemplateID;
}
//---------------------------------------------------------------------------
void NiGeneralComponent::AddReference()
{
    this->IncRefCount();
}
//---------------------------------------------------------------------------
void NiGeneralComponent::RemoveReference()
{
    this->DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiGeneralComponent::GetClassName() const
{
    return ms_kClassName; 
}
//---------------------------------------------------------------------------
NiFixedString NiGeneralComponent::GetName() const
{
    return m_kName;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::SetName(const NiFixedString& kName)
{
    m_kName = kName;
    return true;
}
//---------------------------------------------------------------------------
void NiGeneralComponent::Update(NiEntityPropertyInterface*,
    float, NiEntityErrorInterface*, NiExternalAssetManager*)
{
    // This component has no work to do during Update.
}
//---------------------------------------------------------------------------
void NiGeneralComponent::BuildVisibleSet(NiEntityRenderingContext*,
    NiEntityErrorInterface*)
{
    // This component has no work to do during BuildVisibleSet.
}
//---------------------------------------------------------------------------
void NiGeneralComponent::GetPropertyNames(
    NiTObjectSet<NiFixedString>& kPropertyNames) const
{
    if (m_spMasterComponent)
    {
        m_spMasterComponent->GetPropertyNames(kPropertyNames);
    }
    else
    {
        unsigned int uiSize = m_kProperties.GetSize();
        for (unsigned int ui = 0; ui < uiSize; ui++)
        {
            kPropertyNames.Add(m_kProperties.GetAt(ui)->GetPropertyName());
        }
    }
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::CanResetProperty(
    const NiFixedString& kPropertyName, bool& bCanReset) const
{
    NiGeneralComponentProperty* pkLocalProperty = FindPropertyByName(
        kPropertyName, false);
    NiGeneralComponentProperty* pkMasterProperty = NULL;
    if (m_spMasterComponent)
    {
        pkMasterProperty = m_spMasterComponent->FindPropertyByName(
            kPropertyName, true);
    }
    if (pkLocalProperty || pkMasterProperty)
    {
        bCanReset = (pkLocalProperty && pkMasterProperty);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::ResetProperty(const NiFixedString& kPropertyName)
{
    bool bCanReset;
    if (!CanResetProperty(kPropertyName, bCanReset) || !bCanReset)
    {
        return false;
    }

    unsigned int uiIndex = FindPropertyIndexByName(kPropertyName);
    NIASSERT(uiIndex != INVALID_INDEX);
    m_kProperties.RemoveAt(uiIndex);

    return true;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::MakePropertyUnique(
    const NiFixedString& kPropertyName, bool& bMadeUnique)
{
    bool bCanReset;
    if (!CanResetProperty(kPropertyName, bCanReset))
    {
        return false;
    }

    if (!bCanReset && m_spMasterComponent)
    {
        NiGeneralComponentProperty* pkMasterProperty =
            m_spMasterComponent->FindPropertyByName(kPropertyName, true);
        NIASSERT(pkMasterProperty);
        m_kProperties.Add(pkMasterProperty->Clone());
        bMadeUnique = true;
    }
    else
    {
        bMadeUnique = false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::IsAddPropertySupported() 
{
    return true;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::AddProperty(const NiFixedString& kPropertyName,
    const NiFixedString& kDisplayName, const NiFixedString& kPrimitiveType,
    const NiFixedString& kSemanticType, const NiFixedString& kDescription)
{
    // Cannot add a property with the same name as an existing property.
    if (FindPropertyByName(kPropertyName, true))
    {
        return false;
    }

    NiGeneralComponentProperty* pkProperty = NiNew NiGeneralComponentProperty(
        kPropertyName, kDisplayName, kPrimitiveType, kSemanticType,
        kDescription);
    m_kProperties.Add(pkProperty);

    // Set initial default value for property.
    if (kPrimitiveType == PT_FLOAT)
    {
        SetPropertyData(kPropertyName, 0.0f, 0);
    }
    else if (kPrimitiveType == PT_BOOL)
    {
        SetPropertyData(kPropertyName, false, 0);
    }
    else if (kPrimitiveType == PT_INT)
    {
        SetPropertyData(kPropertyName, (int) 0, 0);
    }
    else if (kPrimitiveType == PT_UINT)
    {
        SetPropertyData(kPropertyName, (unsigned int) 0, 0);
    }
    else if (kPrimitiveType == PT_SHORT)
    {
        SetPropertyData(kPropertyName, (short) 0, 0);
    }
    else if (kPrimitiveType == PT_USHORT)
    {
        SetPropertyData(kPropertyName, (unsigned short) 0, 0);
    }
    else if (kPrimitiveType == PT_STRING)
    {
        SetPropertyData(kPropertyName, NiFixedString(""), 0);
    }
    else if (kPrimitiveType == PT_POINT2)
    {
        SetPropertyData(kPropertyName, NiPoint2::ZERO, 0);
    }
    else if (kPrimitiveType == PT_POINT3)
    {
        SetPropertyData(kPropertyName, NiPoint3::ZERO, 0);
    }
    else if (kPrimitiveType == PT_QUATERNION)
    {
        SetPropertyData(kPropertyName, NiQuaternion::IDENTITY, 0);
    }
    else if (kPrimitiveType == PT_MATRIX3)
    {
        SetPropertyData(kPropertyName, NiMatrix3::IDENTITY, 0);
    }
    else if (kPrimitiveType == PT_COLOR)
    {
        SetPropertyData(kPropertyName, NiColor::BLACK, 0);
    }
    else if (kPrimitiveType == PT_COLORA)
    {
        SetPropertyData(kPropertyName, NiColorA::BLACK, 0);
    }
    else if (kPrimitiveType == PT_NIOBJECTPOINTER)
    {
        SetPropertyData(kPropertyName, (NiObject*) NULL, 0);
    }
    else if (kPrimitiveType == PT_ENTITYPOINTER)
    {
        SetPropertyData(kPropertyName, (NiEntityInterface*) NULL, 0);
    }
    else
    {
        SetPropertyData(kPropertyName, (void*) NULL, 0, 0);
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::MakeCollection(const NiFixedString& kPropertyName,
    bool bCollection)
{
    // Cannot change the cardinality of properties on a component that is 
    // inheriting properties from a master component unless those properties
    // are not in the master
    // component.
    if (m_spMasterComponent &&
        m_spMasterComponent->FindPropertyByName(kPropertyName, true))
    {
        return false;
    }

    unsigned int uiIndex = FindPropertyIndexByName(kPropertyName);
    if (uiIndex != INVALID_INDEX)
    {
        NiGeneralComponentProperty* pkProperty = m_kProperties.GetAt(uiIndex);
        pkProperty->SetIsCollection(bCollection);        
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::RemoveProperty(const NiFixedString& kPropertyName)
{
    // Cannot remove properties from a component that is inheriting properties
    // from a master component unless those properties are not in the master
    // component.
    if (m_spMasterComponent &&
        m_spMasterComponent->FindPropertyByName(kPropertyName, true))
    {
        return false;
    }

    unsigned int uiIndex = FindPropertyIndexByName(kPropertyName);
    if (uiIndex != INVALID_INDEX)
    {
        m_kProperties.RemoveAt(uiIndex);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::GetDisplayName(const NiFixedString& kPropertyName,
    NiFixedString& kDisplayName) const
{
    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, true);
    if (pkProperty)
    {
        kDisplayName = pkProperty->GetDisplayName();
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::SetDisplayName(const NiFixedString& kPropertyName,
    const NiFixedString& kDisplayName)
{
    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, false);
    if (!pkProperty && m_spMasterComponent)
    {
        NiGeneralComponentProperty* pkMasterProperty = m_spMasterComponent
            ->FindPropertyByName(kPropertyName, true);
        if (pkMasterProperty)
        {
            pkProperty = pkMasterProperty->Clone();
            m_kProperties.Add(pkProperty);
        }
    }
    if (pkProperty)
    {
        pkProperty->SetDisplayName(kDisplayName);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::GetPrimitiveType(
    const NiFixedString& kPropertyName, NiFixedString& kPrimitiveType) const
{
    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, true);
    if (pkProperty)
    {
        kPrimitiveType = pkProperty->GetPrimitiveType();
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::SetPrimitiveType(const NiFixedString& kPropertyName,
    const NiFixedString& kPrimitiveType)
{
    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, false);
    if (!pkProperty && m_spMasterComponent)
    {
        NiGeneralComponentProperty* pkMasterProperty = m_spMasterComponent
            ->FindPropertyByName(kPropertyName, true);
        if (pkMasterProperty)
        {
            pkProperty = pkMasterProperty->Clone();
            m_kProperties.Add(pkProperty);
        }
    }
    if (pkProperty)
    {
        pkProperty->SetPrimitiveType(kPrimitiveType);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::GetSemanticType(const NiFixedString& kPropertyName,
    NiFixedString& kSemanticType) const
{
    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, true);
    if (pkProperty)
    {
        kSemanticType = pkProperty->GetSemanticType();
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::SetSemanticType(const NiFixedString& kPropertyName,
    const NiFixedString& kSemanticType)
{
    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, false);
    if (!pkProperty && m_spMasterComponent)
    {
        NiGeneralComponentProperty* pkMasterProperty = m_spMasterComponent
            ->FindPropertyByName(kPropertyName, true);
        if (pkMasterProperty)
        {
            pkProperty = pkMasterProperty->Clone();
            m_kProperties.Add(pkProperty);
        }
    }
    if (pkProperty)
    {
        pkProperty->SetSemanticType(kSemanticType);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::GetDescription(const NiFixedString& kPropertyName,
    NiFixedString& kDescription) const
{
    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, true);
    if (pkProperty)
    {
        kDescription = pkProperty->GetDescription();
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::SetDescription(const NiFixedString& kPropertyName,
    const NiFixedString& kDescription)
{
    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, false);
    if (!pkProperty && m_spMasterComponent)
    {
        NiGeneralComponentProperty* pkMasterProperty = m_spMasterComponent
            ->FindPropertyByName(kPropertyName, true);
        if (pkMasterProperty)
        {
            pkProperty = pkMasterProperty->Clone();
            m_kProperties.Add(pkProperty);
        }
    }
    if (pkProperty)
    {
        pkProperty->SetDescription(kDescription);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::GetCategory(const NiFixedString& kPropertyName,
    NiFixedString& kCategory) const
{
    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, true);
    if (pkProperty)
    {
        kCategory = m_kName;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::IsPropertyReadOnly(
    const NiFixedString& kPropertyName, bool& bIsReadOnly)
{
    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, true);
    if (pkProperty)
    {
        bIsReadOnly = false;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::IsPropertyUnique(
    const NiFixedString& kPropertyName, bool& bIsUnique)
{
    NiGeneralComponentProperty* pkLocalProperty = FindPropertyByName(
        kPropertyName, false);
    NiGeneralComponentProperty* pkMasterProperty = NULL;
    if (m_spMasterComponent)
    {
        pkMasterProperty = m_spMasterComponent->FindPropertyByName(
            kPropertyName, true);
    }
    if (pkLocalProperty || pkMasterProperty)
    {
        bIsUnique = (pkLocalProperty != NULL);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::IsPropertySerializable(
    const NiFixedString& kPropertyName, bool& bIsSerializable)
{
    bool bIsUnique;
    NiBool bSuccess = IsPropertyUnique(kPropertyName, bIsUnique);
    if (bSuccess)
    {
        bool bIsReadOnly;
        bSuccess = IsPropertyReadOnly(kPropertyName, bIsReadOnly);
        NIASSERT(bSuccess);

        bIsSerializable = bIsUnique && !bIsReadOnly;
    }

    return bSuccess;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::IsPropertyInheritable(
    const NiFixedString& kPropertyName, bool& bIsInheritable)
{
    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, true);
    if (pkProperty)
    {
       bIsInheritable = true;
       return true;
    }
    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::IsExternalAssetPath(
    const NiFixedString& kPropertyName, unsigned int,
    bool& bIsExternalAssetPath) const
{
    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, true);
    if (pkProperty)
    {
       bIsExternalAssetPath = false;
       return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::GetElementCount(const NiFixedString& kPropertyName,
    unsigned int& uiCount) const
{
    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, true);
    if (pkProperty)
    {
        bool bIsCollection;
        if (IsCollection(kPropertyName, bIsCollection))
        {
            if (bIsCollection)
            {
                uiCount = pkProperty->GetCollectionSize();
            }
            else
            {
                uiCount = 1;
            }
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::SetElementCount(const NiFixedString& kPropertyName,
    unsigned int uiCount, bool& bCountSet)
{
    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, false);
    if (!pkProperty && m_spMasterComponent)
    {
        if (m_spMasterComponent->FindPropertyByName(kPropertyName, true))
        {
            bool bMadeUnique;
            NIVERIFY(MakePropertyUnique(kPropertyName, bMadeUnique));
            NIASSERT(bMadeUnique);
            pkProperty = FindPropertyByName(kPropertyName, false);
            NIASSERT(pkProperty);
        }
    }
    if (pkProperty)
    {
        bool bIsCollection;
        if (IsCollection(kPropertyName, bIsCollection))
        {
            if (bIsCollection)
            {
                pkProperty->SetCollectionSize(uiCount);
                bCountSet = true;
            }
            else
            {
                bCountSet = false;
            }
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::IsCollection(const NiFixedString& kPropertyName,
    bool& bIsCollection) const
{
    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, true);
    if (pkProperty)
    {
        bIsCollection = pkProperty->GetIsCollection() != 0;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::GetPropertyData(const NiFixedString& kPropertyName,
    float& fData, unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, true);
    if (pkProperty && uiIndex < pkProperty->GetCollectionSize())
    {
        NIASSERT(pkProperty->GetPrimitiveType() == PT_FLOAT);
        NIASSERT(pkProperty->GetDataSizeInBytes(uiIndex) == sizeof(float));
        fData = *((float*) pkProperty->GetData(uiIndex));
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::GetPropertyData(const NiFixedString& kPropertyName,
    bool& bData, unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, true);
    if (pkProperty && uiIndex < pkProperty->GetCollectionSize())
    {
        NIASSERT(pkProperty->GetPrimitiveType() == PT_BOOL);
        NIASSERT(pkProperty->GetDataSizeInBytes(uiIndex) == sizeof(bool));
        bData = *((bool*) pkProperty->GetData(uiIndex));
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::GetPropertyData(const NiFixedString& kPropertyName,
    int& iData, unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, true);
    if (pkProperty && uiIndex < pkProperty->GetCollectionSize())
    {
        NIASSERT(pkProperty->GetPrimitiveType() == PT_INT);
        NIASSERT(pkProperty->GetDataSizeInBytes(uiIndex) == sizeof(int));
        iData = *((int*) pkProperty->GetData(uiIndex));
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::GetPropertyData(const NiFixedString& kPropertyName,
    unsigned int& uiData, unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, true);
    if (pkProperty && uiIndex < pkProperty->GetCollectionSize())
    {
        NIASSERT(pkProperty->GetPrimitiveType() == PT_UINT);
        NIASSERT(pkProperty->GetDataSizeInBytes(uiIndex) ==
            sizeof(unsigned int));
        uiData = *((unsigned int*) pkProperty->GetData(uiIndex));
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::GetPropertyData(const NiFixedString& kPropertyName,
    short& sData, unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, true);
    if (pkProperty && uiIndex < pkProperty->GetCollectionSize())
    {
        NIASSERT(pkProperty->GetPrimitiveType() == PT_SHORT);
        NIASSERT(pkProperty->GetDataSizeInBytes(uiIndex) == sizeof(short));
        sData = *((short*) pkProperty->GetData(uiIndex));
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::GetPropertyData(const NiFixedString& kPropertyName,
    unsigned short& usData, unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, true);
    if (pkProperty && uiIndex < pkProperty->GetCollectionSize())
    {
        NIASSERT(pkProperty->GetPrimitiveType() == PT_USHORT);
        NIASSERT(pkProperty->GetDataSizeInBytes(uiIndex) ==
            sizeof(unsigned short));
        usData = *((unsigned short*) pkProperty->GetData(uiIndex));
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::GetPropertyData(const NiFixedString& kPropertyName,
    NiFixedString& kData, unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, true);
    if (pkProperty && uiIndex < pkProperty->GetCollectionSize())
    {
        NIASSERT(pkProperty->GetPrimitiveType() == PT_STRING);
        kData = (const char*) pkProperty->GetData(uiIndex);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::GetPropertyData(const NiFixedString& kPropertyName,
    NiPoint2& kData, unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, true);
    if (pkProperty && uiIndex < pkProperty->GetCollectionSize())
    {
        NIASSERT(pkProperty->GetPrimitiveType() == PT_POINT2);
        NIASSERT(pkProperty->GetDataSizeInBytes(uiIndex) == 2 * sizeof(float));
        float* pfData = (float*) pkProperty->GetData(uiIndex);
        kData.x = pfData[0];
        kData.y = pfData[1];
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::GetPropertyData(const NiFixedString& kPropertyName,
    NiPoint3& kData, unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, true);
    if (pkProperty && uiIndex < pkProperty->GetCollectionSize())
    {
        NIASSERT(pkProperty->GetPrimitiveType() == PT_POINT3);
        NIASSERT(pkProperty->GetDataSizeInBytes(uiIndex) == 3 * sizeof(float));
        float* pfData = (float*) pkProperty->GetData(uiIndex);
        kData.x = pfData[0];
        kData.y = pfData[1];
        kData.z = pfData[2];
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::GetPropertyData(const NiFixedString& kPropertyName,
    NiQuaternion& kData, unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, true);
    if (pkProperty && uiIndex < pkProperty->GetCollectionSize())
    {
        NIASSERT(pkProperty->GetPrimitiveType() == PT_QUATERNION);
        NIASSERT(pkProperty->GetDataSizeInBytes(uiIndex) == 4 * sizeof(float));
        float* pfData = (float*) pkProperty->GetData(uiIndex);
        kData.SetW(pfData[0]);
        kData.SetX(pfData[1]);
        kData.SetY(pfData[2]);
        kData.SetZ(pfData[3]);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::GetPropertyData(const NiFixedString& kPropertyName,
    NiMatrix3& kData, unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, true);
    if (pkProperty && uiIndex < pkProperty->GetCollectionSize())
    {
        NIASSERT(pkProperty->GetPrimitiveType() == PT_MATRIX3);
        NIASSERT(pkProperty->GetDataSizeInBytes(uiIndex) == 9 * sizeof(float));
        float* pfData = (float*) pkProperty->GetData(uiIndex);
        kData.SetCol(0, &pfData[0]);
        kData.SetCol(1, &pfData[3]);
        kData.SetCol(2, &pfData[6]);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::GetPropertyData(const NiFixedString& kPropertyName,
    NiColor& kData, unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, true);
    if (pkProperty && uiIndex < pkProperty->GetCollectionSize())
    {
        NIASSERT(pkProperty->GetPrimitiveType() == PT_COLOR);
        NIASSERT(pkProperty->GetDataSizeInBytes(uiIndex) == 3 * sizeof(float));
        float* pfData = (float*) pkProperty->GetData(uiIndex);
        kData.r = pfData[0];
        kData.g = pfData[1];
        kData.b = pfData[2];
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::GetPropertyData(const NiFixedString& kPropertyName,
    NiColorA& kData, unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, true);
    if (pkProperty && uiIndex < pkProperty->GetCollectionSize())
    {
        NIASSERT(pkProperty->GetPrimitiveType() == PT_COLORA);
        NIASSERT(pkProperty->GetDataSizeInBytes(uiIndex) == 4 * sizeof(float));
        float* pfData = (float*) pkProperty->GetData(uiIndex);
        kData.r = pfData[0];
        kData.g = pfData[1];
        kData.b = pfData[2];
        kData.a = pfData[3];
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::GetPropertyData(const NiFixedString& kPropertyName,
    NiObject*& pkData, unsigned int uiIndex) const
{
    void* pvData = NULL;
    size_t stSize;
    NiBool bSuccess = GetPropertyData(kPropertyName, pvData, stSize, uiIndex);
    if (bSuccess)
    {
        NIASSERT(pvData == NULL || stSize == sizeof(NiObject*));
        NiMemcpy(&pkData, sizeof(pkData), pvData, stSize);
    }

    return bSuccess;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::GetPropertyData(const NiFixedString& kPropertyName,
    NiEntityInterface*& pkData, unsigned int uiIndex) const
{
    void* pvData = NULL;
    size_t stSize;
    NiBool bSuccess = GetPropertyData(kPropertyName, pvData, stSize, uiIndex);
    
    if (bSuccess)
    {
        NIASSERT(pvData == NULL || stSize == sizeof(NiEntityInterface*));
        NiMemcpy(&pkData, sizeof(pkData), pvData, stSize);
    }

    return bSuccess;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::GetPropertyData(const NiFixedString& kPropertyName,
    void*& pvData, size_t& stDataSizeInBytes, unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, true);
    if (pkProperty && uiIndex < pkProperty->GetCollectionSize())
    {
        pvData = pkProperty->GetData(uiIndex);
        stDataSizeInBytes = pkProperty->GetDataSizeInBytes(uiIndex);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::SetPropertyData(const NiFixedString& kPropertyName,
    float fData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, false);
    if (!pkProperty && m_spMasterComponent)
    {
        NiGeneralComponentProperty* pkMasterProperty = m_spMasterComponent
            ->FindPropertyByName(kPropertyName, true);
        if (pkMasterProperty)
        {
            pkProperty = pkMasterProperty->Clone();
            m_kProperties.Add(pkProperty);
        }
    }
    if (pkProperty)
    {
        NIASSERT(pkProperty->GetPrimitiveType() == PT_FLOAT);
        pkProperty->SetData((float*) &fData, 1, uiIndex);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::SetPropertyData(const NiFixedString& kPropertyName,
    bool bData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, false);
    if (!pkProperty && m_spMasterComponent)
    {
        NiGeneralComponentProperty* pkMasterProperty = m_spMasterComponent
            ->FindPropertyByName(kPropertyName, true);
        if (pkMasterProperty)
        {
            pkProperty = pkMasterProperty->Clone();
            m_kProperties.Add(pkProperty);
        }
    }
    if (pkProperty)
    {
        NIASSERT(pkProperty->GetPrimitiveType() == PT_BOOL);
        pkProperty->SetData((bool*) &bData, 1, uiIndex);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::SetPropertyData(const NiFixedString& kPropertyName,
    int iData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, false);
    if (!pkProperty && m_spMasterComponent)
    {
        NiGeneralComponentProperty* pkMasterProperty = m_spMasterComponent
            ->FindPropertyByName(kPropertyName, true);
        if (pkMasterProperty)
        {
            pkProperty = pkMasterProperty->Clone();
            m_kProperties.Add(pkProperty);
        }
    }
    if (pkProperty)
    {
        NIASSERT(pkProperty->GetPrimitiveType() == PT_INT);
        pkProperty->SetData((int*) &iData, 1, uiIndex);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::SetPropertyData(const NiFixedString& kPropertyName,
    unsigned int uiData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, false);
    if (!pkProperty && m_spMasterComponent)
    {
        NiGeneralComponentProperty* pkMasterProperty = m_spMasterComponent
            ->FindPropertyByName(kPropertyName, true);
        if (pkMasterProperty)
        {
            pkProperty = pkMasterProperty->Clone();
            m_kProperties.Add(pkProperty);
        }
    }
    if (pkProperty)
    {
        NIASSERT(pkProperty->GetPrimitiveType() == PT_UINT);
        pkProperty->SetData((unsigned int*) &uiData, 1, uiIndex);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::SetPropertyData(const NiFixedString& kPropertyName,
    short sData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, false);
    if (!pkProperty && m_spMasterComponent)
    {
        NiGeneralComponentProperty* pkMasterProperty = m_spMasterComponent
            ->FindPropertyByName(kPropertyName, true);
        if (pkMasterProperty)
        {
            pkProperty = pkMasterProperty->Clone();
            m_kProperties.Add(pkProperty);
        }
    }
    if (pkProperty)
    {
        NIASSERT(pkProperty->GetPrimitiveType() == PT_SHORT);
        pkProperty->SetData((short*) &sData, 1, uiIndex);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::SetPropertyData(const NiFixedString& kPropertyName,
    unsigned short usData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, false);
    if (!pkProperty && m_spMasterComponent)
    {
        NiGeneralComponentProperty* pkMasterProperty = m_spMasterComponent
            ->FindPropertyByName(kPropertyName, true);
        if (pkMasterProperty)
        {
            pkProperty = pkMasterProperty->Clone();
            m_kProperties.Add(pkProperty);
        }
    }
    if (pkProperty)
    {
        NIASSERT(pkProperty->GetPrimitiveType() == PT_USHORT);
        pkProperty->SetData((unsigned short*) &usData, 1, uiIndex);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::SetPropertyData(const NiFixedString& kPropertyName,
    const NiFixedString& kData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, false);
    if (!pkProperty && m_spMasterComponent)
    {
        NiGeneralComponentProperty* pkMasterProperty = m_spMasterComponent
            ->FindPropertyByName(kPropertyName, true);
        if (pkMasterProperty)
        {
            pkProperty = pkMasterProperty->Clone();
            m_kProperties.Add(pkProperty);
        }
    }
    if (pkProperty)
    {
        NIASSERT(pkProperty->GetPrimitiveType() == PT_STRING);
        char* pcString = (char*) ((const char*) kData);
        size_t stArrayCount = 0;
        if (pcString)
        {
            stArrayCount = strlen(pcString) + 1;
        }
        pkProperty->SetData(pcString, stArrayCount, uiIndex);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::SetPropertyData(const NiFixedString& kPropertyName,
    const NiPoint2& kData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, false);
    if (!pkProperty && m_spMasterComponent)
    {
        NiGeneralComponentProperty* pkMasterProperty = m_spMasterComponent
            ->FindPropertyByName(kPropertyName, true);
        if (pkMasterProperty)
        {
            pkProperty = pkMasterProperty->Clone();
            m_kProperties.Add(pkProperty);
        }
    }
    if (pkProperty)
    {
        NIASSERT(pkProperty->GetPrimitiveType() == PT_POINT2);
        float afPoint2[] = {kData.x, kData.y};
        pkProperty->SetData((float*) &afPoint2, 2, uiIndex);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::SetPropertyData(const NiFixedString& kPropertyName,
    const NiPoint3& kData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, false);
    if (!pkProperty && m_spMasterComponent)
    {
        NiGeneralComponentProperty* pkMasterProperty = m_spMasterComponent
            ->FindPropertyByName(kPropertyName, true);
        if (pkMasterProperty)
        {
            pkProperty = pkMasterProperty->Clone();
            m_kProperties.Add(pkProperty);
        }
    }
    if (pkProperty)
    {
        NIASSERT(pkProperty->GetPrimitiveType() == PT_POINT3);
        float afPoint3[] = {kData.x, kData.y, kData.z};
        pkProperty->SetData((float*) &afPoint3, 3, uiIndex);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::SetPropertyData(const NiFixedString& kPropertyName,
    const NiQuaternion& kData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, false);
    if (!pkProperty && m_spMasterComponent)
    {
        NiGeneralComponentProperty* pkMasterProperty = m_spMasterComponent
            ->FindPropertyByName(kPropertyName, true);
        if (pkMasterProperty)
        {
            pkProperty = pkMasterProperty->Clone();
            m_kProperties.Add(pkProperty);
        }
    }
    if (pkProperty)
    {
        NIASSERT(pkProperty->GetPrimitiveType() == PT_QUATERNION);
        float afQuaternion[] = {kData.GetW(), kData.GetX(), kData.GetY(),
            kData.GetZ()};
        pkProperty->SetData((float*) &afQuaternion, 4, uiIndex);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::SetPropertyData(const NiFixedString& kPropertyName,
    const NiMatrix3& kData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, false);
    if (!pkProperty && m_spMasterComponent)
    {
        NiGeneralComponentProperty* pkMasterProperty = m_spMasterComponent
            ->FindPropertyByName(kPropertyName, true);
        if (pkMasterProperty)
        {
            pkProperty = pkMasterProperty->Clone();
            m_kProperties.Add(pkProperty);
        }
    }
    if (pkProperty)
    {
        NIASSERT(pkProperty->GetPrimitiveType() == PT_MATRIX3);
        float afMatrix3[] = {kData.GetEntry(0, 0), kData.GetEntry(1, 0),
            kData.GetEntry(2, 0), kData.GetEntry(0, 1), kData.GetEntry(1, 1),
            kData.GetEntry(2, 1), kData.GetEntry(0, 2), kData.GetEntry(1, 2),
            kData.GetEntry(2, 2)};
        pkProperty->SetData((float*) &afMatrix3, 9, uiIndex);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::SetPropertyData(const NiFixedString& kPropertyName,
    const NiColor& kData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, false);
    if (!pkProperty && m_spMasterComponent)
    {
        NiGeneralComponentProperty* pkMasterProperty = m_spMasterComponent
            ->FindPropertyByName(kPropertyName, true);
        if (pkMasterProperty)
        {
            pkProperty = pkMasterProperty->Clone();
            m_kProperties.Add(pkProperty);
        }
    }
    if (pkProperty)
    {
        NIASSERT(pkProperty->GetPrimitiveType() == PT_COLOR);
        float afColor[] = {kData.r, kData.g, kData.b};
        pkProperty->SetData((float*) &afColor, 3, uiIndex);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::SetPropertyData(const NiFixedString& kPropertyName,
    const NiColorA& kData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, false);
    if (!pkProperty && m_spMasterComponent)
    {
        NiGeneralComponentProperty* pkMasterProperty = m_spMasterComponent
            ->FindPropertyByName(kPropertyName, true);
        if (pkMasterProperty)
        {
            pkProperty = pkMasterProperty->Clone();
            m_kProperties.Add(pkProperty);
        }
    }
    if (pkProperty)
    {
        NIASSERT(pkProperty->GetPrimitiveType() == PT_COLORA);
        float afColorA[] = {kData.r, kData.g, kData.b, kData.a};
        pkProperty->SetData((float*) &afColorA, 4, uiIndex);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::SetPropertyData(const NiFixedString& kPropertyName,
    NiObject* pkData, unsigned int uiIndex)
{
    return SetPropertyData(kPropertyName, (void*) &pkData, sizeof(pkData), 
        uiIndex);
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::SetPropertyData(const NiFixedString& kPropertyName,
    NiEntityInterface* pkData, unsigned int uiIndex)
{
    return SetPropertyData(kPropertyName, (void*) &pkData, sizeof(pkData), 
        uiIndex);
}
//---------------------------------------------------------------------------
NiBool NiGeneralComponent::SetPropertyData(const NiFixedString& kPropertyName,
    const void* pvData, size_t stDataSizeInBytes, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        bool bIsCollection;
        if (!IsCollection(kPropertyName, bIsCollection) || 
            bIsCollection == false)
        {
            return false;
        }
    }

    NiGeneralComponentProperty* pkProperty = FindPropertyByName(
        kPropertyName, false);
    if (!pkProperty && m_spMasterComponent)
    {
        NiGeneralComponentProperty* pkMasterProperty = m_spMasterComponent
            ->FindPropertyByName(kPropertyName, true);
        if (pkMasterProperty)
        {
            pkProperty = pkMasterProperty->Clone();
            m_kProperties.Add(pkProperty);
        }
    }
    if (pkProperty)
    {
        pkProperty->SetData((char*) pvData, stDataSizeInBytes, uiIndex);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void NiGeneralComponent::_SDMInit()
{
    ms_kClassName = "NiGeneralComponent";
}
//---------------------------------------------------------------------------
void NiGeneralComponent::_SDMShutdown()
{
    ms_kClassName = NULL;
}
//---------------------------------------------------------------------------
