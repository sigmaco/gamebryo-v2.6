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

#include "NiTransformationComponent.h"
#include "NiEntityErrorInterface.h"
#include <NiAVObject.h>
#include "NiEntityInterface.h"

NiFixedString NiTransformationComponent::ms_kClassName;
NiFixedString NiTransformationComponent::ms_kComponentName;
NiFixedString NiTransformationComponent::ms_kPropTranslation;
NiFixedString NiTransformationComponent::ms_kPropRotation;
NiFixedString NiTransformationComponent::ms_kPropScale;
NiFixedString NiTransformationComponent::ms_kPropSourceEntity;
NiFixedString NiTransformationComponent::ms_kPropAttachmentPointName;
NiFixedString NiTransformationComponent::ms_kPropInheritTranslation;
NiFixedString NiTransformationComponent::ms_kPropInheritRotation;
NiFixedString NiTransformationComponent::ms_kPropInheritScale;
NiFixedString NiTransformationComponent::ms_kPropLocalTranslation;
NiFixedString NiTransformationComponent::ms_kPropLocalRotation;
NiFixedString NiTransformationComponent::ms_kPropLocalScale;

NiFixedString NiTransformationComponent::ms_kDispTranslation;
NiFixedString NiTransformationComponent::ms_kDispRotation;
NiFixedString NiTransformationComponent::ms_kDispScale;

NiFixedString NiTransformationComponent::ms_kDescTranslation;
NiFixedString NiTransformationComponent::ms_kDescRotation;
NiFixedString NiTransformationComponent::ms_kDescScale;
NiFixedString NiTransformationComponent::ms_kDescSourceEntity;
NiFixedString NiTransformationComponent::ms_kDescAttachmentPointName;
NiFixedString NiTransformationComponent::ms_kDescInheritTranslation;
NiFixedString NiTransformationComponent::ms_kDescInheritRotation;
NiFixedString NiTransformationComponent::ms_kDescInheritScale;
NiFixedString NiTransformationComponent::ms_kDescLocalTranslation;
NiFixedString NiTransformationComponent::ms_kDescLocalRotation;
NiFixedString NiTransformationComponent::ms_kDescLocalScale;

NiFixedString NiTransformationComponent::ms_kErrAttachmentPointNotFound;

NiFixedString NiTransformationComponent::ms_kDepSceneRootPointer;

//---------------------------------------------------------------------------
void NiTransformationComponent::_SDMInit()
{
    ms_kClassName = "NiTransformationComponent";
    ms_kComponentName = "Transformation";
    ms_kPropTranslation = "Translation";
    ms_kPropRotation = "Rotation";
    ms_kPropScale = "Scale";
    ms_kPropSourceEntity = "Source Entity";
    ms_kPropAttachmentPointName = "Attachment Point Name";
    ms_kPropInheritTranslation = "Inherit Translation";
    ms_kPropInheritRotation = "Inherit Rotation";
    ms_kPropInheritScale = "Inherit Scale";
    ms_kPropLocalTranslation = "Local Translation";
    ms_kPropLocalRotation = "Local Rotation";
    ms_kPropLocalScale = "Local Scale";

    ms_kDispTranslation = "World Translation";
    ms_kDispRotation = "World Rotation";
    ms_kDispScale = "World Scale";
    ms_kDescTranslation = "The world-space position of the entity.";
    ms_kDescRotation = "The world-space orientation of the entity.";
    ms_kDescScale = "The world-space scale of the entity.";
    ms_kDescSourceEntity = "The entity from which the translation, "
        "rotation, and scale properties are inherited.";
    ms_kDescAttachmentPointName = "The attachment point in the source "
        "entity whose transforms are inherited.";
    ms_kDescInheritTranslation = "Whether or not to inherit the "
        "translation of the source entity.";
    ms_kDescInheritRotation = "Whether or not to inherit the "
        "rotation of the source entity.";
    ms_kDescInheritScale = "Whether or not to inherit the "
        "scale of the source entity.";
    ms_kDescLocalTranslation = "The position of the entity relative to the "
        "source entity and/or attachment point.";
    ms_kDescLocalRotation = "The orientation of the entity relative to the "
        "source entity and/or attachment point.";
    ms_kDescLocalScale = "The scale of the entity relative to the "
        "source entity and/or attachment point.";

    ms_kErrAttachmentPointNotFound = "The specified attachment point was not "
        "found; the base scene root will be used instead.";

    ms_kDepSceneRootPointer = "Scene Root Pointer";
}
//---------------------------------------------------------------------------
void NiTransformationComponent::_SDMShutdown()
{
    ms_kClassName = NULL;
    ms_kComponentName = NULL;
    ms_kPropTranslation = NULL;
    ms_kPropRotation = NULL;
    ms_kPropScale = NULL;
    ms_kPropSourceEntity = NULL;
    ms_kPropAttachmentPointName = NULL;
    ms_kPropInheritTranslation = NULL;
    ms_kPropInheritRotation = NULL;
    ms_kPropInheritScale = NULL;
    ms_kPropLocalTranslation = NULL;
    ms_kPropLocalRotation = NULL;
    ms_kPropLocalScale = NULL;

    ms_kDispTranslation = NULL;
    ms_kDispRotation = NULL;
    ms_kDispScale = NULL;
    ms_kDescTranslation = NULL;
    ms_kDescRotation = NULL;
    ms_kDescScale = NULL;
    ms_kDescSourceEntity = NULL;
    ms_kDescAttachmentPointName = NULL;
    ms_kDescInheritTranslation = NULL;
    ms_kDescInheritRotation = NULL;
    ms_kDescInheritScale = NULL;
    ms_kDescLocalTranslation = NULL;
    ms_kDescLocalRotation = NULL;
    ms_kDescLocalScale = NULL;

    ms_kErrAttachmentPointNotFound = NULL;

    ms_kDepSceneRootPointer = NULL;
}
//---------------------------------------------------------------------------
NiTransformationComponent::NiTransformationComponent() :
    m_uFlags(0),
    m_kTranslation(NiPoint3::ZERO),
    m_kRotation(NiMatrix3::IDENTITY),
    m_fScale(1.0f),
    m_pkSourceEntity(NULL),
    m_pkAttachmentPoint(NULL),
    m_bRetrievingTranslation(false),
    m_bRetrievingRotation(false),
    m_bRetrievingScale(false)
{
    SetInheritTranslation(true);
    SetInheritTranslationUnique(false);
    SetInheritRotation(true);
    SetInheritRotationUnique(false);
    SetInheritScale(true);
    SetInheritScaleUnique(false);
}
//---------------------------------------------------------------------------
NiTransformationComponent::NiTransformationComponent(
    NiEntityInterface* pkSourceEntity,
    NiFixedString kAttachmentPointName) :
    m_uFlags(0),
    m_kTranslation(NiPoint3::ZERO),
    m_kRotation(NiMatrix3::IDENTITY),
    m_fScale(1.0f),
    m_pkSourceEntity(pkSourceEntity),
    m_kAttachmentPointName(kAttachmentPointName),
    m_pkAttachmentPoint(NULL),
    m_bRetrievingTranslation(false),
    m_bRetrievingRotation(false),
    m_bRetrievingScale(false)
{
    SetInheritTranslation(true);
    SetInheritTranslationUnique(false);
    SetInheritRotation(true);
    SetInheritRotationUnique(false);
    SetInheritScale(true);
    SetInheritScaleUnique(false);
}
//---------------------------------------------------------------------------
NiTransformationComponent::NiTransformationComponent(
    const NiPoint3& kTranslation,
    const NiMatrix3& kRotation,
    float fScale,
    NiEntityInterface* pkSourceEntity,
    NiFixedString kAttachmentPointName,
    bool bInheritTranslation,
    bool bInheritRotation,
    bool bInheritScale) :
    m_uFlags(0),
    m_kTranslation(kTranslation),
    m_kRotation(kRotation),
    m_fScale(fScale),
    m_pkSourceEntity(pkSourceEntity),
    m_kAttachmentPointName(kAttachmentPointName),
    m_pkAttachmentPoint(NULL),
    m_bRetrievingTranslation(false),
    m_bRetrievingRotation(false),
    m_bRetrievingScale(false)
{
    SetInheritTranslation(bInheritTranslation);
    SetInheritTranslationUnique(false);
    SetInheritRotation(bInheritRotation);
    SetInheritRotationUnique(false);
    SetInheritScale(bInheritScale);
    SetInheritScaleUnique(false);
}
//---------------------------------------------------------------------------
NiTransformationComponent::NiTransformationComponent(
    NiTransformationComponent* pkMasterComponent) :
    m_uFlags(0),
    m_kTranslation(NiPoint3::ZERO),
    m_kRotation(NiMatrix3::IDENTITY),
    m_fScale(1.0f),
    m_pkSourceEntity(NULL),
    m_spMasterComponent(pkMasterComponent),
    m_pkAttachmentPoint(NULL),
    m_bRetrievingTranslation(false),
    m_bRetrievingRotation(false),
    m_bRetrievingScale(false)
{
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::GetSourceTranslation(
    NiPoint3& kSourceTranslation) const
{
    if (m_pkAttachmentPoint)
    {
        kSourceTranslation = m_pkAttachmentPoint->GetWorldTranslate();
        return true;
    }
    else
    {
        NiEntityInterface* pkSourceEntity = GetSourceEntity();
        if (pkSourceEntity && pkSourceEntity->GetPropertyData(
            ms_kPropTranslation, kSourceTranslation, 0))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::GetSourceRotation(
    NiMatrix3& kSourceRotation) const
{
    if (m_pkAttachmentPoint)
    {
        kSourceRotation = m_pkAttachmentPoint->GetWorldRotate();
        return true;
    }
    else
    {
        NiEntityInterface* pkSourceEntity = GetSourceEntity();
        if (pkSourceEntity && pkSourceEntity->GetPropertyData(
            ms_kPropRotation, kSourceRotation, 0))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::GetSourceScale(float& fSourceScale)
    const
{
    if (m_pkAttachmentPoint)
    {
        fSourceScale = m_pkAttachmentPoint->GetWorldScale();
        return true;
    }
    else
    {
        NiEntityInterface* pkSourceEntity = GetSourceEntity();
        if (pkSourceEntity && pkSourceEntity->GetPropertyData(
            ms_kPropScale, fSourceScale, 0))
        {
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
// NiEntityComponentInterface overrides.
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiTransformationComponent::Clone(
    bool bInheritProperties)
{
    if (bInheritProperties)
    {
        NiTransformationComponent* pkClone = NiNew NiTransformationComponent(
            this);
        pkClone->m_pkSourceEntity = m_pkSourceEntity;
        return pkClone;
    }
    else
    {
        NiTransformationComponent* pkClone = NiNew NiTransformationComponent(
            m_spMasterComponent);

        pkClone->m_uFlags = m_uFlags;
        pkClone->m_kTranslation = m_kTranslation;
        pkClone->m_kRotation = m_kRotation;
        pkClone->m_fScale = m_fScale;
        pkClone->m_pkSourceEntity = m_pkSourceEntity;
        pkClone->m_kAttachmentPointName = m_kAttachmentPointName;

        return pkClone;
    }
}
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiTransformationComponent::GetMasterComponent()
    const
{
    return m_spMasterComponent;
}
//---------------------------------------------------------------------------
void NiTransformationComponent::SetMasterComponent(
    NiEntityComponentInterface* pkMasterComponent)
{
    if (pkMasterComponent)
    {
        NIASSERT(pkMasterComponent->GetClassName() == GetClassName());
        m_spMasterComponent = (NiTransformationComponent*) pkMasterComponent;
    }
    else
    {
        NiTObjectSet<NiFixedString> kPropertyNames(10);
        GetPropertyNames(kPropertyNames);
        const unsigned int uiSize = kPropertyNames.GetSize();
        for (unsigned int ui = 0; ui < uiSize; ++ui)
        {
            bool bMadeUnique;
            NIVERIFY(MakePropertyUnique(kPropertyNames.GetAt(ui),
                bMadeUnique));
        }
        m_spMasterComponent = NULL;
    }
}
//---------------------------------------------------------------------------
void NiTransformationComponent::GetDependentPropertyNames(
    NiTObjectSet<NiFixedString>&)
{
    // This component has no dependent properties.
}
//---------------------------------------------------------------------------
// NiEntityPropertyInterface overrides.
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::SetTemplateID(const NiUniqueID&)
{
    //Not supported for custom components
    return false;
}
//---------------------------------------------------------------------------
NiUniqueID  NiTransformationComponent::GetTemplateID()
{
    static const NiUniqueID kUniqueID = 
        NiUniqueID(0x70,0x76,0x3E,0x14,0x2B,0x83,0x2,0x4D,0xAE,0xE1,0x76,0x99,
        0x64,0x93,0x22,0xF2);
    return kUniqueID;
}
//---------------------------------------------------------------------------
void NiTransformationComponent::AddReference()
{
    IncRefCount();
}
//---------------------------------------------------------------------------
void NiTransformationComponent::RemoveReference()
{
    DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiTransformationComponent::GetClassName() const
{
    return ms_kClassName;
}
//---------------------------------------------------------------------------
NiFixedString NiTransformationComponent::GetName() const
{
    return ms_kComponentName;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::SetName(const NiFixedString&)
{
    // This component does not allow its name to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::IsAnimated() const
{
    // This component only needs to be updated if it has a source entity.
    return (GetSourceEntity() != NULL);
}
//---------------------------------------------------------------------------
void NiTransformationComponent::Update(
    NiEntityPropertyInterface* pkParentEntity,
    float,
    NiEntityErrorInterface* pkErrors,
    NiExternalAssetManager*)
{
    if (ShouldResolveAttachmentPoint())
    {
        NIASSERT(m_pkSourceEntity == GetSourceEntity() && m_pkSourceEntity);

        NiObject* pkObject;
        if (m_pkSourceEntity->GetPropertyData(ms_kDepSceneRootPointer,
            pkObject, 0) && pkObject)
        {
            NIASSERT(!m_pkAttachmentPoint);
            NiAVObject* pkSceneRoot = NiDynamicCast(NiAVObject, pkObject);
            if (pkSceneRoot)
            {
                NIASSERT(m_kAttachmentPointName == GetAttachmentPointName() &&
                    m_kAttachmentPointName.Exists());

                m_pkAttachmentPoint = pkSceneRoot->GetObjectByName(
                    m_kAttachmentPointName);
                if (!m_pkAttachmentPoint)
                {
                    pkErrors->ReportError(ms_kErrAttachmentPointNotFound,
                        m_kAttachmentPointName, pkParentEntity->GetName(),
                        ms_kPropAttachmentPointName);
                    m_pkAttachmentPoint = pkSceneRoot;
                }
            }
            if (!m_pkAttachmentPoint)
            {
                SetErrorResolvingAttachmentPoint(true);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiTransformationComponent::BuildVisibleSet(
    NiEntityRenderingContext*,
    NiEntityErrorInterface*)
{
    // This component has no work to do during BuildVisibleSet.
}
//---------------------------------------------------------------------------
void NiTransformationComponent::GetPropertyNames(
    NiTObjectSet<NiFixedString>& kPropertyNames) const
{
    kPropertyNames.Add(ms_kPropSourceEntity);
    kPropertyNames.Add(ms_kPropAttachmentPointName);
    kPropertyNames.Add(ms_kPropTranslation);
    kPropertyNames.Add(ms_kPropRotation);
    kPropertyNames.Add(ms_kPropScale);
    kPropertyNames.Add(ms_kPropInheritTranslation);
    kPropertyNames.Add(ms_kPropInheritRotation);
    kPropertyNames.Add(ms_kPropInheritScale);
    kPropertyNames.Add(ms_kPropLocalTranslation);
    kPropertyNames.Add(ms_kPropLocalRotation);
    kPropertyNames.Add(ms_kPropLocalScale);
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::CanResetProperty(
    const NiFixedString& kPropertyName, bool& bCanReset) const
{
    if (kPropertyName == ms_kPropTranslation ||
        kPropertyName == ms_kPropLocalTranslation)
    {
        bCanReset = (m_spMasterComponent && GetTranslationUnique());
    }
    else if (kPropertyName == ms_kPropRotation ||
        kPropertyName == ms_kPropLocalRotation)
    {
        bCanReset = (m_spMasterComponent && GetRotationUnique());
    }
    else if (kPropertyName == ms_kPropScale ||
        kPropertyName == ms_kPropLocalScale)
    {
        bCanReset = (m_spMasterComponent && GetScaleUnique());
    }
    else if (kPropertyName == ms_kPropSourceEntity)
    {
        bCanReset = false;
    }
    else if (kPropertyName == ms_kPropAttachmentPointName)
    {
        bCanReset = (m_spMasterComponent && GetAttachmentPointNameUnique());
    }
    else if (kPropertyName == ms_kPropInheritTranslation)
    {
        bCanReset = (m_spMasterComponent && GetInheritTranslationUnique());
    }
    else if (kPropertyName == ms_kPropInheritRotation)
    {
        bCanReset = (m_spMasterComponent && GetInheritRotationUnique());
    }
    else if (kPropertyName == ms_kPropInheritScale)
    {
        bCanReset = (m_spMasterComponent && GetInheritScaleUnique());
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::ResetProperty(
    const NiFixedString& kPropertyName)
{
    bool bCanReset;
    if (!CanResetProperty(kPropertyName, bCanReset) || !bCanReset)
    {
        return false;
    }
    
    NIASSERT(m_spMasterComponent);

    if (kPropertyName == ms_kPropTranslation ||
        kPropertyName == ms_kPropLocalTranslation)
    {
        SetTranslationUnique(false);
    }
    else if (kPropertyName == ms_kPropRotation ||
        kPropertyName == ms_kPropLocalRotation)
    {
        SetRotationUnique(false);
    }
    else if (kPropertyName == ms_kPropScale ||
        kPropertyName == ms_kPropLocalScale)
    {
        SetScaleUnique(false);
    }
    else if (kPropertyName == ms_kPropAttachmentPointName)
    {
        SetAttachmentPointNameUnique(false);
    }
    else if (kPropertyName == ms_kPropInheritTranslation)
    {
        SetInheritTranslationUnique(false);
    }
    else if (kPropertyName == ms_kPropInheritRotation)
    {
        SetInheritRotationUnique(false);
    }
    else if (kPropertyName == ms_kPropInheritScale)
    {
        SetInheritScaleUnique(false);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::MakePropertyUnique(
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
        if (kPropertyName == ms_kPropTranslation ||
            kPropertyName == ms_kPropLocalTranslation)
        {
            GetTranslation(m_kTranslation);
            SetTranslationUnique(true);
        }
        else if (kPropertyName == ms_kPropRotation ||
            kPropertyName == ms_kPropLocalRotation)
        {
            GetRotation(m_kRotation);
            SetRotationUnique(true);
        }
        else if (kPropertyName == ms_kPropScale ||
            kPropertyName == ms_kPropLocalScale)
        {
            m_fScale = GetScale();
            SetScaleUnique(true);
        }
        else if (kPropertyName == ms_kPropAttachmentPointName)
        {
            m_kAttachmentPointName = GetAttachmentPointName();
            SetAttachmentPointNameUnique(true);
        }
        else if (kPropertyName == ms_kPropInheritTranslation)
        {
            SetBit(NIBOOL_IS_TRUE(GetInheritTranslation()),
                INHERIT_TRANSLATION_MASK);
            SetInheritTranslationUnique(true);
        }
        else if (kPropertyName == ms_kPropInheritRotation)
        {
            SetBit(NIBOOL_IS_TRUE(GetInheritRotation()),
                INHERIT_ROTATION_MASK);
            SetInheritRotationUnique(true);
        }
        else if (kPropertyName == ms_kPropInheritScale)
        {
            SetBit(NIBOOL_IS_TRUE(GetInheritScale()),
                INHERIT_SCALE_MASK);
            SetInheritScaleUnique(true);
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
NiBool NiTransformationComponent::GetDisplayName(
    const NiFixedString& kPropertyName, NiFixedString& kDisplayName) const
{
    if (kPropertyName == ms_kPropTranslation)
    {
        kDisplayName = ms_kDispTranslation;
    }
    else if (kPropertyName == ms_kPropRotation)
    {
        kDisplayName = ms_kDispRotation;
    }
    else if (kPropertyName == ms_kPropScale)
    {
        kDisplayName = ms_kDispScale;
    }
    else if (kPropertyName == ms_kPropSourceEntity ||
        kPropertyName == ms_kPropAttachmentPointName ||
        kPropertyName == ms_kPropInheritTranslation ||
        kPropertyName == ms_kPropInheritRotation ||
        kPropertyName == ms_kPropInheritScale ||
        kPropertyName == ms_kPropLocalTranslation ||
        kPropertyName == ms_kPropLocalRotation ||
        kPropertyName == ms_kPropLocalScale)
    {
        kDisplayName = kPropertyName;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::SetDisplayName(
    const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the display name to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::GetPrimitiveType(
    const NiFixedString& kPropertyName, NiFixedString& kPrimitiveType) const
{
    if (kPropertyName == ms_kPropTranslation ||
        kPropertyName == ms_kPropLocalTranslation)
    {
        kPrimitiveType = PT_POINT3;
    }
    else if (kPropertyName == ms_kPropRotation ||
        kPropertyName == ms_kPropLocalRotation)
    {
        kPrimitiveType = PT_MATRIX3;
    }
    else if (kPropertyName == ms_kPropScale ||
        kPropertyName == ms_kPropLocalScale)
    {
        kPrimitiveType = PT_FLOAT;
    }
    else if (kPropertyName == ms_kPropSourceEntity)
    {
        kPrimitiveType = PT_ENTITYPOINTER;
    }
    else if (kPropertyName == ms_kPropAttachmentPointName)
    {
        kPrimitiveType = PT_STRING;
    }
    else if (kPropertyName == ms_kPropInheritTranslation)
    {
        kPrimitiveType = PT_BOOL;
    }
    else if (kPropertyName == ms_kPropInheritRotation)
    {
        kPrimitiveType = PT_BOOL;
    }
    else if (kPropertyName == ms_kPropInheritScale)
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
NiBool NiTransformationComponent::SetPrimitiveType(
    const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the primitive type to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::GetSemanticType(
    const NiFixedString& kPropertyName, NiFixedString& kSemanticType) const
{
    // For this component, the semantic type of each property matches its
    // primitive type.
    return GetPrimitiveType(kPropertyName, kSemanticType);
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::SetSemanticType(
    const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the semantic type to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::GetDescription(
    const NiFixedString& kPropertyName, NiFixedString& kDescription) const
{
    if (kPropertyName == ms_kPropTranslation)
    {
        kDescription = ms_kDescTranslation;
    }
    else if (kPropertyName == ms_kPropRotation)
    {
        kDescription = ms_kDescRotation;
    }
    else if (kPropertyName == ms_kPropScale)
    {
        kDescription = ms_kDescScale;
    }
    else if (kPropertyName == ms_kPropSourceEntity)
    {
        kDescription = ms_kDescSourceEntity;
    }
    else if (kPropertyName == ms_kPropAttachmentPointName)
    {
        kDescription = ms_kDescAttachmentPointName;
    }
    else if (kPropertyName == ms_kPropInheritTranslation)
    {
        kDescription = ms_kDescInheritTranslation;
    }
    else if (kPropertyName == ms_kPropInheritRotation)
    {
        kDescription = ms_kDescInheritRotation;
    }
    else if (kPropertyName == ms_kPropInheritScale)
    {
        kDescription = ms_kDescInheritScale;
    }
    else if (kPropertyName == ms_kPropLocalTranslation)
    {
        kDescription = ms_kDescLocalTranslation;
    }
    else if (kPropertyName == ms_kPropLocalRotation)
    {
        kDescription = ms_kDescLocalRotation;
    }
    else if (kPropertyName == ms_kPropLocalScale)
    {
        kDescription = ms_kDescLocalScale;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::SetDescription(
    const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the description to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::GetCategory(
    const NiFixedString& kPropertyName, NiFixedString& kCategory) const
{
    if (kPropertyName == ms_kPropTranslation ||
        kPropertyName == ms_kPropRotation ||
        kPropertyName == ms_kPropScale ||
        kPropertyName == ms_kPropSourceEntity ||
        kPropertyName == ms_kPropAttachmentPointName ||
        kPropertyName == ms_kPropInheritTranslation ||
        kPropertyName == ms_kPropInheritRotation ||
        kPropertyName == ms_kPropInheritScale ||
        kPropertyName == ms_kPropLocalTranslation ||
        kPropertyName == ms_kPropLocalRotation ||
        kPropertyName == ms_kPropLocalScale)
    {
        kCategory = ms_kComponentName;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::IsPropertyReadOnly(
    const NiFixedString& kPropertyName, bool& bIsReadOnly)
{
    if (kPropertyName == ms_kPropTranslation ||
        kPropertyName == ms_kPropRotation ||
        kPropertyName == ms_kPropScale ||
        kPropertyName == ms_kPropSourceEntity ||
        kPropertyName == ms_kPropAttachmentPointName ||
        kPropertyName == ms_kPropInheritTranslation ||
        kPropertyName == ms_kPropInheritRotation ||
        kPropertyName == ms_kPropInheritScale ||
        kPropertyName == ms_kPropLocalTranslation ||
        kPropertyName == ms_kPropLocalRotation ||
        kPropertyName == ms_kPropLocalScale)
    {
        bIsReadOnly = false;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::IsPropertyUnique(
    const NiFixedString& kPropertyName, bool& bIsUnique)
{
    if (kPropertyName == ms_kPropTranslation ||
        kPropertyName == ms_kPropRotation ||
        kPropertyName == ms_kPropScale ||
        kPropertyName == ms_kPropAttachmentPointName ||
        kPropertyName == ms_kPropInheritTranslation ||
        kPropertyName == ms_kPropInheritRotation ||
        kPropertyName == ms_kPropInheritScale ||
        kPropertyName == ms_kPropLocalTranslation ||
        kPropertyName == ms_kPropLocalRotation ||
        kPropertyName == ms_kPropLocalScale)
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
    else if (kPropertyName == ms_kPropSourceEntity)
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
NiBool NiTransformationComponent::IsPropertySerializable(
    const NiFixedString& kPropertyName, bool& bIsSerializable)
{
    if (kPropertyName == ms_kPropTranslation ||
        kPropertyName == ms_kPropRotation ||
        kPropertyName == ms_kPropScale)
    {
        bIsSerializable = false;
        return true;
    }
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
NiBool NiTransformationComponent::IsPropertyInheritable(
    const NiFixedString& kPropertyName, bool& bIsInheritable)
{
    if (kPropertyName == ms_kPropTranslation ||
        kPropertyName == ms_kPropRotation ||
        kPropertyName == ms_kPropScale ||
        kPropertyName == ms_kPropAttachmentPointName ||
        kPropertyName == ms_kPropInheritTranslation ||
        kPropertyName == ms_kPropInheritRotation ||
        kPropertyName == ms_kPropInheritScale ||
        kPropertyName == ms_kPropLocalTranslation ||
        kPropertyName == ms_kPropLocalRotation ||
        kPropertyName == ms_kPropLocalScale)
    {
        bIsInheritable = true;
    }
    else if (kPropertyName == ms_kPropSourceEntity)
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
NiBool NiTransformationComponent::IsExternalAssetPath(
    const NiFixedString& kPropertyName,
    unsigned int,
    bool& bIsExternalAssetPath) const
{
    if (kPropertyName == ms_kPropTranslation ||
        kPropertyName == ms_kPropRotation ||
        kPropertyName == ms_kPropScale ||
        kPropertyName == ms_kPropSourceEntity ||
        kPropertyName == ms_kPropAttachmentPointName ||
        kPropertyName == ms_kPropInheritTranslation ||
        kPropertyName == ms_kPropInheritRotation ||
        kPropertyName == ms_kPropInheritScale ||
        kPropertyName == ms_kPropLocalTranslation ||
        kPropertyName == ms_kPropLocalRotation ||
        kPropertyName == ms_kPropLocalScale)
    {
        bIsExternalAssetPath = false;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::GetElementCount(
    const NiFixedString& kPropertyName, unsigned int& uiCount) const
{
    if (kPropertyName == ms_kPropTranslation ||
        kPropertyName == ms_kPropRotation ||
        kPropertyName == ms_kPropScale ||
        kPropertyName == ms_kPropSourceEntity ||
        kPropertyName == ms_kPropAttachmentPointName ||
        kPropertyName == ms_kPropInheritTranslation ||
        kPropertyName == ms_kPropInheritRotation ||
        kPropertyName == ms_kPropInheritScale ||
        kPropertyName == ms_kPropLocalTranslation ||
        kPropertyName == ms_kPropLocalRotation ||
        kPropertyName == ms_kPropLocalScale)
    {
        uiCount = 1;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::SetElementCount(
    const NiFixedString& kPropertyName,
    unsigned int,
    bool& bCountSet)
{
    if (kPropertyName == ms_kPropTranslation ||
        kPropertyName == ms_kPropRotation ||
        kPropertyName == ms_kPropScale ||
        kPropertyName == ms_kPropSourceEntity ||
        kPropertyName == ms_kPropAttachmentPointName ||
        kPropertyName == ms_kPropInheritTranslation ||
        kPropertyName == ms_kPropInheritRotation ||
        kPropertyName == ms_kPropInheritScale ||
        kPropertyName == ms_kPropLocalTranslation ||
        kPropertyName == ms_kPropLocalRotation ||
        kPropertyName == ms_kPropLocalScale)
    {
        bCountSet = false;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::IsCollection(
    const NiFixedString& kPropertyName, bool& bIsCollection) const
{
    if (kPropertyName == ms_kPropTranslation ||
        kPropertyName == ms_kPropRotation ||
        kPropertyName == ms_kPropScale ||
        kPropertyName == ms_kPropSourceEntity ||
        kPropertyName == ms_kPropAttachmentPointName ||
        kPropertyName == ms_kPropInheritTranslation ||
        kPropertyName == ms_kPropInheritRotation ||
        kPropertyName == ms_kPropInheritScale ||
        kPropertyName == ms_kPropLocalTranslation ||
        kPropertyName == ms_kPropLocalRotation ||
        kPropertyName == ms_kPropLocalScale)
    {
        bIsCollection = false;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::GetPropertyData(
    const NiFixedString& kPropertyName, float& fData, unsigned int uiIndex)
    const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kPropScale)
    {
        fData = GetScale();

        if (GetInheritScale() && !m_bRetrievingScale)
        {
            m_bRetrievingScale = true;

            float fSourceScale;
            if (GetSourceScale(fSourceScale))
            {
                fData = fSourceScale * fData;
            }

            m_bRetrievingScale = false;
        }
    }
    else if (kPropertyName == ms_kPropLocalScale)
    {
        fData = GetScale();
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::SetPropertyData(
    const NiFixedString& kPropertyName, float fData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kPropScale)
    {
        float fScaleToSet = fData;
        if (GetInheritScale() && !m_bRetrievingScale)
        {
            m_bRetrievingScale = true;

            float fSourceScale;
            if (GetSourceScale(fSourceScale))
            {
                fScaleToSet /= fSourceScale;
            }

            m_bRetrievingScale = false;
        }

        SetScale(fScaleToSet);
    }
    else if (kPropertyName == ms_kPropLocalScale)
    {
        SetScale(fData);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::GetPropertyData(
    const NiFixedString& kPropertyName, bool& bData, unsigned int uiIndex)
    const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kPropInheritTranslation)
    {
       bData = NIBOOL_IS_TRUE(GetInheritTranslation());
    }
    else if (kPropertyName == ms_kPropInheritRotation)
    {
       bData = NIBOOL_IS_TRUE(GetInheritRotation());
    }
    else if (kPropertyName == ms_kPropInheritScale)
    {
       bData = NIBOOL_IS_TRUE(GetInheritScale());
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::SetPropertyData(
    const NiFixedString& kPropertyName, bool bData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kPropInheritTranslation)
    {
       SetInheritTranslation(bData);
    }
    else if (kPropertyName == ms_kPropInheritRotation)
    {
       SetInheritRotation(bData);
    }
    else if (kPropertyName == ms_kPropInheritScale)
    {
       SetInheritScale(bData);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::GetPropertyData(
    const NiFixedString& kPropertyName, NiPoint3& kData, unsigned int uiIndex)
    const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kPropTranslation)
    {
        GetTranslation(kData);

        if (GetInheritTranslation() && !m_bRetrievingTranslation)
        {
            m_bRetrievingTranslation = true;

            NiPoint3 kSourceTranslation;
            NiMatrix3 kSourceRotation;
            float fSourceScale;
            if (GetSourceTranslation(kSourceTranslation) &&
                GetSourceRotation(kSourceRotation) &&
                GetSourceScale(fSourceScale))
            {
                kData = kSourceTranslation + fSourceScale *
                    (kSourceRotation * kData);
            }

            m_bRetrievingTranslation = false;
        }
    }
    else if (kPropertyName == ms_kPropLocalTranslation)
    {
        GetTranslation(kData);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::SetPropertyData(
    const NiFixedString& kPropertyName, const NiPoint3& kData,
    unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kPropTranslation)
    {
        NiPoint3 kTranslationToSet = kData;
        if (GetInheritTranslation() && !m_bRetrievingTranslation)
        {
            m_bRetrievingTranslation = true;

            NiPoint3 kSourceTranslation;
            NiMatrix3 kSourceRotation;
            float fSourceScale;
            if (GetSourceTranslation(kSourceTranslation) &&
                GetSourceRotation(kSourceRotation) &&
                GetSourceScale(fSourceScale))
            {
                // Invert source transforms.
                kSourceRotation = kSourceRotation.Transpose();
                fSourceScale = 1.0f / fSourceScale;
                kSourceTranslation = fSourceScale * (kSourceRotation *
                    -kSourceTranslation);

                kTranslationToSet = kSourceTranslation + fSourceScale *
                    (kSourceRotation * kTranslationToSet);
            }

            m_bRetrievingTranslation = false;
        }

        SetTranslation(kTranslationToSet);
    }
    else if (kPropertyName == ms_kPropLocalTranslation)
    {
        SetTranslation(kData);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::GetPropertyData(
    const NiFixedString& kPropertyName, NiMatrix3& kData,
    unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kPropRotation)
    {
        GetRotation(kData);

        if (GetInheritRotation() && !m_bRetrievingRotation)
        {
            m_bRetrievingRotation = true;

            NiMatrix3 kSourceRotation;
            if (GetSourceRotation(kSourceRotation))
            {
                kData = kSourceRotation * kData;
            }

            m_bRetrievingRotation = false;
        }
    }
    else if (kPropertyName == ms_kPropLocalRotation)
    {
        GetRotation(kData);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::SetPropertyData(
    const NiFixedString& kPropertyName, const NiMatrix3& kData,
    unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kPropRotation)
    {
        NiMatrix3 kRotationToSet = kData;
        if (GetInheritRotation() && !m_bRetrievingRotation)
        {
            m_bRetrievingRotation = true;

            NiMatrix3 kSourceRotation;
            if (GetSourceRotation(kSourceRotation))
            {
                kRotationToSet = kSourceRotation.TransposeTimes(
                    kRotationToSet);
            }

            m_bRetrievingRotation = false;
        }

        SetRotation(kRotationToSet);
    }
    else if (kPropertyName == ms_kPropLocalRotation)
    {
        SetRotation(kData);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::GetPropertyData(
    const NiFixedString& kPropertyName, NiFixedString& kData,
    unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kPropAttachmentPointName)
    {
        kData = GetAttachmentPointName();
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::SetPropertyData(
    const NiFixedString& kPropertyName, const NiFixedString& kData,
    unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kPropAttachmentPointName)
    {
        SetAttachmentPointName(kData);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::GetPropertyData(
    const NiFixedString& kPropertyName, NiEntityInterface*& pkData,
    unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kPropSourceEntity)
    {
        pkData = GetSourceEntity();
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiTransformationComponent::SetPropertyData(
    const NiFixedString& kPropertyName, NiEntityInterface* pkData,
    unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kPropSourceEntity)
    {
        SetSourceEntity(pkData);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
