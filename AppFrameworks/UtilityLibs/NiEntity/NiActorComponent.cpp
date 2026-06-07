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

#include "NiActorComponent.h"
#include "NiEntityInterface.h"
#include "NiEntityErrorInterface.h"
#include <NiDrawSceneUtility.h>
#include "NiExternalAssetManager.h"
#include "NiParamsKFM.h"
#include <NiMesh.h>

NiFixedString NiActorComponent::ERR_TRANSLATION_NOT_FOUND;
NiFixedString NiActorComponent::ERR_ROTATION_NOT_FOUND;
NiFixedString NiActorComponent::ERR_SCALE_NOT_FOUND;
NiFixedString NiActorComponent::ERR_ACTOR_MANAGER_CREATION_FAILED;


NiFixedString NiActorComponent::ms_kClassName;
NiFixedString NiActorComponent::ms_kComponentName;

NiFixedString NiActorComponent::ms_kKfmFilePathName;
NiFixedString NiActorComponent::ms_kActiveSequenceIDName;
NiFixedString NiActorComponent::ms_kAccumulateTransformsName;
NiFixedString NiActorComponent::ms_kSequenceIDsName;
NiFixedString NiActorComponent::ms_kSequenceNamesName;
NiFixedString NiActorComponent::ms_kSceneRootPointerName;

NiFixedString NiActorComponent::ms_kKfmFilePathDescription;
NiFixedString NiActorComponent::ms_kActiveSequenceIDDescription;
NiFixedString NiActorComponent::ms_kAccumulateTransformsDescription;

NiFixedString NiActorComponent::ms_kTranslationName;
NiFixedString NiActorComponent::ms_kRotationName;
NiFixedString NiActorComponent::ms_kScaleName;

//---------------------------------------------------------------------------
void NiActorComponent::_SDMInit()
{
    ERR_TRANSLATION_NOT_FOUND = "Translation property not found.";
    ERR_ROTATION_NOT_FOUND = "Rotation property not found.";
    ERR_SCALE_NOT_FOUND = "Scale property not found.";
    ERR_ACTOR_MANAGER_CREATION_FAILED = "The actor manager could not be "
        "created from the specified KFM file.";

    ms_kComponentName = "Actor";
    ms_kClassName = "NiActorComponent";

    ms_kKfmFilePathName = "KFM File Path";
    ms_kActiveSequenceIDName = "Active Sequence ID";
    ms_kAccumulateTransformsName = "Accumulate Transforms";
    ms_kSequenceIDsName = "Sequence IDs";
    ms_kSequenceNamesName = "Sequence Names";
    ms_kSceneRootPointerName = "Scene Root Pointer";

    ms_kKfmFilePathDescription = "The path to a KFM file to use for the "
        "actor.";
    ms_kActiveSequenceIDDescription = "The currently active sequence ID for "
        "the actor.";
    ms_kAccumulateTransformsDescription = "Indicates whether or not the "
        "actor accumulates transformations on its accumulation root.";

    ms_kTranslationName = "Translation";
    ms_kRotationName = "Rotation";
    ms_kScaleName = "Scale";

}
//---------------------------------------------------------------------------
void NiActorComponent::_SDMShutdown()
{
    ERR_TRANSLATION_NOT_FOUND = NULL;
    ERR_ROTATION_NOT_FOUND = NULL;
    ERR_SCALE_NOT_FOUND = NULL;
    ERR_ACTOR_MANAGER_CREATION_FAILED = NULL;

    ms_kClassName = NULL;
    ms_kComponentName = NULL;

    ms_kKfmFilePathName = NULL;
    ms_kActiveSequenceIDName = NULL;
    ms_kAccumulateTransformsName = NULL;
    ms_kSequenceIDsName = NULL;
    ms_kSequenceNamesName = NULL;
    ms_kSceneRootPointerName = NULL;

    ms_kKfmFilePathDescription = NULL;
    ms_kActiveSequenceIDDescription = NULL;
    ms_kAccumulateTransformsDescription = NULL;

    ms_kTranslationName = NULL;
    ms_kRotationName = NULL;
    ms_kScaleName = NULL;
}
//---------------------------------------------------------------------------
// NiEntityComponentInterface overrides.
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiActorComponent::Clone(bool bInheritProperties)
{
    if (bInheritProperties)
    {
        return NiNew NiActorComponent(this);
    }
    else
    {
        NiActorComponent* pkClone = NiNew NiActorComponent(
            m_spMasterComponent);

        pkClone->m_uFlags = m_uFlags;
        pkClone->m_kKfmFilePath = m_kKfmFilePath;
        pkClone->m_uiActiveSequenceID = m_uiActiveSequenceID;
        pkClone->m_bAccumulateTransforms = m_bAccumulateTransforms;

        return pkClone;
    }
}
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiActorComponent::GetMasterComponent() const
{
    return m_spMasterComponent;
}
//---------------------------------------------------------------------------
void NiActorComponent::SetMasterComponent(
    NiEntityComponentInterface* pkMasterComponent)
{
    if (pkMasterComponent)
    {
        NIASSERT(pkMasterComponent->GetClassName() == GetClassName());
        m_spMasterComponent = (NiActorComponent*) pkMasterComponent;
    }
    else
    {
        NiTObjectSet<NiFixedString> kPropertyNames(10);
        GetPropertyNames(kPropertyNames);
        unsigned int uiSize = kPropertyNames.GetSize();
        for (unsigned int ui = 0; ui < uiSize; ui++)
        {
            bool bMadeUnique;
            NIVERIFY(MakePropertyUnique(kPropertyNames.GetAt(ui),
                bMadeUnique));
        }
        m_spMasterComponent = NULL;
    }
}
//---------------------------------------------------------------------------
void NiActorComponent::GetDependentPropertyNames(
    NiTObjectSet<NiFixedString>& kDependentPropertyNames)
{
    kDependentPropertyNames.Add(ms_kTranslationName);
    kDependentPropertyNames.Add(ms_kRotationName);
    kDependentPropertyNames.Add(ms_kScaleName);
}
//---------------------------------------------------------------------------
// NiEntityPropertyInterface overrides.
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
NiBool NiActorComponent::SetTemplateID(const NiUniqueID&)
{
    //Not supported for custom components
    return false;
}
//---------------------------------------------------------------------------
NiUniqueID  NiActorComponent::GetTemplateID()
{
    static const NiUniqueID kUniqueID = 
        NiUniqueID(0x26,0x7B,0x61,0x14,0x50,0x8A,0x37,0x44,
        0xAB,0x18,0x27,0x2,0xA0,0x2A,0x4B,0xED);
    return kUniqueID;
}
//---------------------------------------------------------------------------
void NiActorComponent::AddReference()
{
    this->IncRefCount();
}
//---------------------------------------------------------------------------
void NiActorComponent::RemoveReference()
{
    this->DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiActorComponent::GetClassName() const
{
    return ms_kClassName;
}
//---------------------------------------------------------------------------
NiFixedString NiActorComponent::GetName() const
{
    return ms_kComponentName;
}
//---------------------------------------------------------------------------
NiBool NiActorComponent::SetName(const NiFixedString&)
{
    // This component does not allow its name to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiActorComponent::IsAnimated() const
{
    // This component represents an animated character and thus should always
    // be updated.
    return true;
}
//---------------------------------------------------------------------------
void NiActorComponent::Update(NiEntityPropertyInterface* pkParentEntity,
    float fTime, NiEntityErrorInterface* pkErrors,
    NiExternalAssetManager* pkAssetManager)
{
    if (ShouldReloadActor())
    {
        // Re-synchronize default and local data.
        m_kKfmFilePath = GetKfmFilePath();
        m_uiActiveSequenceID = GetActiveSequenceID();
        m_bAccumulateTransforms = NIBOOL_IS_TRUE(GetAccumulateTransforms());

        // Previously active sequence ID.
        unsigned int uiActiveSequenceID = GetActiveSequenceID();

        // Clear out existing actor.
        ClearActor();

        // If the actor manager does not exist, create it using the property
        // data.
        if (GetKfmFilePath().Exists() && pkAssetManager && 
            pkAssetManager->GetAssetFactory())
        {
            NiFixedString kAssetType;
            NiParamsKFM kKFMParams;

            if (GetAccumulateTransforms())
            {
                kKFMParams.SetAccumulated(true);
            }

            if (!kKFMParams.SetAssetPath(GetKfmFilePath()) ||
                !pkAssetManager->RegisterAndResolve(&kKFMParams))
            {
                SetLoadErrorHit(true);
                pkErrors->ReportError(ERR_ACTOR_MANAGER_CREATION_FAILED,
                    GetKfmFilePath(), pkParentEntity->GetName(),
                    ms_kKfmFilePathName);
                return;
            }

            NIVERIFY(pkAssetManager->Retrieve(&kKFMParams));
            m_spActor = kKFMParams.GetActor();

            if (!m_spActor)
            {
                SetLoadErrorHit(true);
                pkErrors->ReportError(ERR_ACTOR_MANAGER_CREATION_FAILED,
                    GetKfmFilePath(), pkParentEntity->GetName(),
                    ms_kKfmFilePathName);
                return;
            }
            NIASSERT(m_spActor->GetNIFRoot());

            // Perform initial update.
            m_spActor->GetNIFRoot()->Update(0.0f);
            NiMesh::CompleteSceneModifiers(m_spActor->GetNIFRoot());
            m_spActor->GetNIFRoot()->UpdateNodeBound();
            m_spActor->GetNIFRoot()->UpdateProperties();
            m_spActor->GetNIFRoot()->UpdateEffects();

            m_spActor->SetTargetAnimation(uiActiveSequenceID);
        }
    }

    if (m_spActor)
    {
        NiBool bDependentPropertiesFound = true;

        // Find dependent properties.
        NiPoint3 kTranslation;
        if (!pkParentEntity->GetPropertyData(ms_kTranslationName,
            kTranslation))
        {
            bDependentPropertiesFound = false;
            pkErrors->ReportError(ERR_TRANSLATION_NOT_FOUND, NULL,
                pkParentEntity->GetName(), ms_kTranslationName);
        }
        NiMatrix3 kRotation;
        if (!pkParentEntity->GetPropertyData(ms_kRotationName, kRotation))
        {
            bDependentPropertiesFound = false;
            pkErrors->ReportError(ERR_ROTATION_NOT_FOUND, NULL,
                pkParentEntity->GetName(), ms_kRotationName);
        }
        float fScale;
        if (!pkParentEntity->GetPropertyData(ms_kScaleName, fScale))
        {
            bDependentPropertiesFound = false;
            pkErrors->ReportError(ERR_SCALE_NOT_FOUND, NULL,
                pkParentEntity->GetName(), ms_kScaleName);
        }

        // Use dependent properties to update transform of scene root.
        bool bUpdatedTransforms = false;
        if (bDependentPropertiesFound)
        {
            NiAVObject* pkNIFRoot = m_spActor->GetNIFRoot();
            if (pkNIFRoot->GetTranslate() != kTranslation)
            {
                pkNIFRoot->SetTranslate(kTranslation);
                bUpdatedTransforms = true;
            }
            if (pkNIFRoot->GetRotate() != kRotation)
            {
                pkNIFRoot->SetRotate(kRotation);
                bUpdatedTransforms = true;
            }
            if (pkNIFRoot->GetScale() != fScale)
            {
                pkNIFRoot->SetScale(fScale);
                bUpdatedTransforms = true;
            }
        }

        if (ShouldChangeSequenceID())
        {
            m_spActor->SetTargetAnimation(GetActiveSequenceID());
            m_uiActiveSequenceID = GetActiveSequenceID();
        }

        // Update the actor manager and scene root with the provided time.
        m_spActor->Update(fTime);
        if (bUpdatedTransforms)
        {
            m_spActor->GetNIFRoot()->Update(fTime);
        }
        else
        {
            m_spActor->GetNIFRoot()->UpdateSelected(fTime);
        }
    }
}
//---------------------------------------------------------------------------
void NiActorComponent::BuildVisibleSet(NiEntityRenderingContext*
    pkRenderingContext, NiEntityErrorInterface*)
{
    if (m_spActor != NULL)
    {
        pkRenderingContext->m_pkCullingProcess->Cull(
            pkRenderingContext->m_pkCamera, m_spActor->GetNIFRoot(),
            pkRenderingContext->m_pkCullingProcess->GetVisibleSet());
    }
}
//---------------------------------------------------------------------------
void NiActorComponent::GetPropertyNames(
    NiTObjectSet<NiFixedString>& kPropertyNames) const
{
    kPropertyNames.Add(ms_kKfmFilePathName);
    kPropertyNames.Add(ms_kActiveSequenceIDName);
    kPropertyNames.Add(ms_kAccumulateTransformsName);
    kPropertyNames.Add(ms_kSequenceIDsName);
    kPropertyNames.Add(ms_kSequenceNamesName);
    kPropertyNames.Add(ms_kSceneRootPointerName);
}
//---------------------------------------------------------------------------
NiBool NiActorComponent::CanResetProperty(const NiFixedString& kPropertyName,
    bool& bCanReset) const
{
    if (kPropertyName == ms_kKfmFilePathName)
    {
        bCanReset = (m_spMasterComponent && GetKfmFilePathChanged());
    }
    else if (kPropertyName == ms_kActiveSequenceIDName)
    {
        bCanReset = (m_spMasterComponent && GetActiveSequenceIDChanged());
    }
    else if (kPropertyName == ms_kAccumulateTransformsName)
    {
        bCanReset = (m_spMasterComponent &&
            GetAccumulateTransformsChanged());
    }
    else if (kPropertyName == ms_kSequenceIDsName ||
        kPropertyName == ms_kSequenceNamesName ||
        kPropertyName == ms_kSceneRootPointerName)
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
NiBool NiActorComponent::ResetProperty(const NiFixedString& kPropertyName)
{
    bool bCanReset;
    if (!CanResetProperty(kPropertyName, bCanReset) || !bCanReset)
    {
        return false;
    }

    NIASSERT(m_spMasterComponent);

    if (kPropertyName == ms_kKfmFilePathName)
    {
        SetKfmFilePathChanged(false);
    }
    else if (kPropertyName == ms_kActiveSequenceIDName)
    {
        SetActiveSequenceIDChanged(false);
    }
    else if (kPropertyName == ms_kAccumulateTransformsName)
    {
        SetAccumulateTransformsChanged(false);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiActorComponent::MakePropertyUnique(const NiFixedString& kPropertyName,
    bool& bMadeUnique)
{
    bool bCanReset;
    if (!CanResetProperty(kPropertyName, bCanReset))
    {
        return false;
    }

    if (!bCanReset && m_spMasterComponent)
    {
        if (kPropertyName == ms_kKfmFilePathName)
        {
            SetKfmFilePathChanged(true);
            bMadeUnique = true;
        }
        else if (kPropertyName == ms_kActiveSequenceIDName)
        {
            SetActiveSequenceIDChanged(true);
            bMadeUnique = true;
        }
        else if (kPropertyName == ms_kAccumulateTransformsName)
        {
            SetAccumulateTransformsChanged(true);
            bMadeUnique = true;
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
NiBool NiActorComponent::GetDisplayName(const NiFixedString& kPropertyName,
    NiFixedString& kDisplayName) const
{
    if (kPropertyName == ms_kKfmFilePathName)
    {
        kDisplayName = ms_kKfmFilePathName;
    }
    else if (kPropertyName == ms_kActiveSequenceIDName)
    {
        kDisplayName = ms_kActiveSequenceIDName;
    }
    else if (kPropertyName == ms_kAccumulateTransformsName)
    {
        kDisplayName = ms_kAccumulateTransformsName;
    }
    else if (kPropertyName == ms_kSequenceIDsName ||
        kPropertyName == ms_kSequenceNamesName ||
        kPropertyName == ms_kSceneRootPointerName)
    {
        // This property should not be displayed.
        kDisplayName = NULL;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiActorComponent::SetDisplayName(const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the display name to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiActorComponent::GetPrimitiveType(const NiFixedString& kPropertyName,
    NiFixedString& kPrimitiveType) const
{
    if (kPropertyName == ms_kKfmFilePathName)
    {
        kPrimitiveType = PT_STRING;
    }
    else if (kPropertyName == ms_kActiveSequenceIDName)
    {
        kPrimitiveType = PT_UINT;
    }
    else if (kPropertyName == ms_kAccumulateTransformsName)
    {
        kPrimitiveType = PT_BOOL;
    }
    else if (kPropertyName == ms_kSequenceIDsName)
    {
        kPrimitiveType = PT_UINT;
    }
    else if (kPropertyName == ms_kSequenceNamesName)
    {
        kPrimitiveType = PT_STRING;
    }
    else if (kPropertyName == ms_kSceneRootPointerName)
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
NiBool NiActorComponent::SetPrimitiveType(const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the primitive type to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiActorComponent::GetSemanticType(const NiFixedString& kPropertyName,
    NiFixedString& kSemanticType) const
{
    if (kPropertyName == ms_kKfmFilePathName)
    {
        kSemanticType = "KFM Filename";
    }
    else if (kPropertyName == ms_kActiveSequenceIDName)
    {
        kSemanticType = "Sequence ID";
    }
    else if (kPropertyName == ms_kAccumulateTransformsName)
    {
        kSemanticType = PT_BOOL;
    }
    else if (kPropertyName == ms_kSequenceIDsName)
    {
        kSemanticType = PT_UINT;
    }
    else if (kPropertyName == ms_kSequenceNamesName)
    {
        kSemanticType = PT_STRING;
    }
    else if (kPropertyName == ms_kSceneRootPointerName)
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
NiBool NiActorComponent::SetSemanticType(const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the semantic type to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiActorComponent::GetDescription(const NiFixedString& kPropertyName,
    NiFixedString& kDescription) const
{
    if (kPropertyName == ms_kKfmFilePathName)
    {
        kDescription = ms_kKfmFilePathDescription;
    }
    else if (kPropertyName == ms_kActiveSequenceIDName)
    {
        kDescription = ms_kActiveSequenceIDDescription;
    }
    else if (kPropertyName == ms_kAccumulateTransformsName)
    {
        kDescription = ms_kAccumulateTransformsDescription;
    }
    else if (kPropertyName == ms_kSequenceIDsName ||
        kPropertyName == ms_kSequenceNamesName ||
        kPropertyName == ms_kSceneRootPointerName)
    {
        // This is a hidden property, so no description is provided.
        kDescription = NULL;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiActorComponent::SetDescription(const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the description to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiActorComponent::GetCategory(const NiFixedString& kPropertyName,
    NiFixedString& kCategory) const
{
    if (kPropertyName == ms_kKfmFilePathName ||
        kPropertyName == ms_kActiveSequenceIDName ||
        kPropertyName == ms_kAccumulateTransformsName ||
        kPropertyName == ms_kSequenceIDsName ||
        kPropertyName == ms_kSequenceNamesName ||
        kPropertyName == ms_kSceneRootPointerName)
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
NiBool NiActorComponent::IsPropertyReadOnly(
    const NiFixedString& kPropertyName, bool& bIsReadOnly)
{
    if (kPropertyName == ms_kKfmFilePathName ||
        kPropertyName == ms_kActiveSequenceIDName ||
        kPropertyName == ms_kAccumulateTransformsName)
    {
        bIsReadOnly = false;
    }
    else if (kPropertyName == ms_kSequenceIDsName ||
        kPropertyName == ms_kSequenceNamesName ||
        kPropertyName == ms_kSceneRootPointerName)
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
NiBool NiActorComponent::IsPropertyUnique(
    const NiFixedString& kPropertyName, bool& bIsUnique)
{
    if (kPropertyName == ms_kKfmFilePathName ||
        kPropertyName == ms_kActiveSequenceIDName ||
        kPropertyName == ms_kAccumulateTransformsName)
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
    else if (kPropertyName == ms_kSequenceIDsName ||
        kPropertyName == ms_kSequenceNamesName ||
        kPropertyName == ms_kSceneRootPointerName)
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
NiBool NiActorComponent::IsPropertySerializable(
    const NiFixedString& kPropertyName, bool& bIsSerializable)
{
    if (kPropertyName == ms_kKfmFilePathName ||
        kPropertyName == ms_kActiveSequenceIDName ||
        kPropertyName == ms_kAccumulateTransformsName)
    {
        bool bIsUnique;
        NIVERIFY(IsPropertyUnique(kPropertyName, bIsUnique));

        bool bIsReadOnly;
        NIVERIFY(IsPropertyReadOnly(kPropertyName, bIsReadOnly));

        bIsSerializable = bIsUnique && !bIsReadOnly;
    }
    else if (kPropertyName == ms_kSequenceIDsName ||
        kPropertyName == ms_kSequenceNamesName ||
        kPropertyName == ms_kSceneRootPointerName)
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
NiBool NiActorComponent::IsPropertyInheritable(
    const NiFixedString& kPropertyName, bool& bIsInheritable)
{
    if (kPropertyName == ms_kKfmFilePathName ||
        kPropertyName == ms_kActiveSequenceIDName ||
        kPropertyName == ms_kAccumulateTransformsName)
    {
        bIsInheritable = true;
    }
    else if (kPropertyName == ms_kSequenceIDsName ||
        kPropertyName == ms_kSequenceNamesName ||
        kPropertyName == ms_kSceneRootPointerName)
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
NiBool NiActorComponent::IsExternalAssetPath(
    const NiFixedString& kPropertyName, unsigned int,
    bool& bIsExternalAssetPath) const
{
    if (kPropertyName == ms_kKfmFilePathName)
    {
        bIsExternalAssetPath = true;
    }
    else if (kPropertyName == ms_kActiveSequenceIDName ||
        kPropertyName == ms_kAccumulateTransformsName ||
        kPropertyName == ms_kSequenceIDsName ||
        kPropertyName == ms_kSequenceNamesName ||
        kPropertyName == ms_kSceneRootPointerName)
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
NiBool NiActorComponent::GetElementCount(const NiFixedString& kPropertyName,
    unsigned int& uiCount) const
{
    if (kPropertyName == ms_kKfmFilePathName ||
        kPropertyName == ms_kActiveSequenceIDName ||
        kPropertyName == ms_kAccumulateTransformsName ||
        kPropertyName == ms_kSceneRootPointerName)
    {
        uiCount = 1;
    }
    else if (kPropertyName == ms_kSequenceIDsName ||
        kPropertyName == ms_kSequenceNamesName)
    {
        if (m_spActor)
        {
            uiCount = m_spActor->GetControllerManager()->GetSequenceCount();
        }
        else
        {
            uiCount = 0;
        }
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiActorComponent::SetElementCount(const NiFixedString& kPropertyName,
    unsigned int, bool& bCountSet)
{
    if (kPropertyName == ms_kKfmFilePathName ||
        kPropertyName == ms_kActiveSequenceIDName ||
        kPropertyName == ms_kAccumulateTransformsName ||
        kPropertyName == ms_kSceneRootPointerName ||
        kPropertyName == ms_kSequenceIDsName ||
        kPropertyName == ms_kSequenceNamesName)
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
NiBool NiActorComponent::IsCollection(const NiFixedString& kPropertyName,
    bool& bIsCollection) const
{
    if (kPropertyName == ms_kKfmFilePathName ||
        kPropertyName == ms_kActiveSequenceIDName ||
        kPropertyName == ms_kAccumulateTransformsName ||
        kPropertyName == ms_kSceneRootPointerName)
    {
        bIsCollection = false;
    }
    else if (kPropertyName == ms_kSequenceIDsName ||
        kPropertyName == ms_kSequenceNamesName)
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
NiBool NiActorComponent::GetPropertyData(const NiFixedString& kPropertyName,
    NiFixedString& kData, unsigned int uiIndex) const
{
    if (kPropertyName == ms_kKfmFilePathName)
    {
        if (uiIndex != 0)
        {
            return false;
        }
        kData = GetKfmFilePath();
    }
    else if (kPropertyName == ms_kSequenceNamesName)
    {
        if (!m_spActor || uiIndex >=
            m_spActor->GetControllerManager()->GetSequenceCount())
        {
            return false;
        }
        kData = m_spActor->GetControllerManager()->GetSequenceAt(uiIndex)
            ->GetName();
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiActorComponent::SetPropertyData(const NiFixedString& kPropertyName,
    const NiFixedString& kData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kKfmFilePathName)
    {
        SetKfmFilePath(kData);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiActorComponent::GetPropertyData(const NiFixedString& kPropertyName,
    unsigned int& uiData, unsigned int uiIndex) const
{
    if (kPropertyName == ms_kActiveSequenceIDName)
    {
        if (uiIndex != 0)
        {
            return false;
        }
        uiData = GetActiveSequenceID();
    }
    else if (kPropertyName == ms_kSequenceIDsName)
    {
        if (!m_spActor || uiIndex >=
            m_spActor->GetControllerManager()->GetSequenceCount())
        {
            return false;
        }
        const NiFixedString& kSeqName = m_spActor->GetControllerManager()
            ->GetSequenceAt(uiIndex)->GetName();
        NIASSERT(kSeqName.Exists());
        uiData = m_spActor->FindSequenceID(kSeqName);
        NIASSERT(uiData != NiActorManager::INVALID_SEQUENCE_ID);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiActorComponent::SetPropertyData(const NiFixedString& kPropertyName,
    unsigned int uiData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kActiveSequenceIDName)
    {
        SetActiveSequenceID(uiData);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiActorComponent::GetPropertyData(const NiFixedString& kPropertyName,
    bool& bData, unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kAccumulateTransformsName)
    {
        bData = NIBOOL_IS_TRUE(GetAccumulateTransforms());
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiActorComponent::SetPropertyData(const NiFixedString& kPropertyName,
    bool bData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kAccumulateTransformsName)
    {
        SetAccumulateTransforms(bData);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiActorComponent::GetPropertyData(const NiFixedString& kPropertyName,
    NiObject*& pkData, unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kSceneRootPointerName)
    {
        if (m_spActor)
        {
            pkData = m_spActor->GetNIFRoot();
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
