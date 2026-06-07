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

#include "NiSceneGraphComponent.h"
#include "NiEntityInterface.h"
#include "NiEntityErrorInterface.h"
#include <NiDrawSceneUtility.h>
#include "NiExternalAssetManager.h"
#include "NiParamsNIF.h"
#include <NiNode.h>
#include <NiMesh.h>

NiFixedString NiSceneGraphComponent::ERR_TRANSLATION_NOT_FOUND;
NiFixedString NiSceneGraphComponent::ERR_ROTATION_NOT_FOUND;
NiFixedString NiSceneGraphComponent::ERR_SCALE_NOT_FOUND;
NiFixedString NiSceneGraphComponent::ERR_FILE_LOAD_FAILED;
NiFixedString NiSceneGraphComponent::ERR_NIF_ROOT_OBJECT;

NiFixedString NiSceneGraphComponent::ms_kClassName;
NiFixedString NiSceneGraphComponent::ms_kComponentName;

NiFixedString NiSceneGraphComponent::ms_kNifFilePathName;
NiFixedString NiSceneGraphComponent::ms_kSceneRootPointerName;

NiFixedString NiSceneGraphComponent::ms_kNifFilePathDescription;

NiFixedString NiSceneGraphComponent::ms_kTranslationName;
NiFixedString NiSceneGraphComponent::ms_kRotationName;
NiFixedString NiSceneGraphComponent::ms_kScaleName;

//---------------------------------------------------------------------------
void NiSceneGraphComponent::_SDMInit()
{
    ERR_TRANSLATION_NOT_FOUND = "Translation property not found.";
    ERR_ROTATION_NOT_FOUND = "Rotation property not found.";
    ERR_SCALE_NOT_FOUND = "Scale property not found.";
    ERR_FILE_LOAD_FAILED = "NIF file load failed.";
    ERR_NIF_ROOT_OBJECT = "Root object at index 0 in NIF file is not an "
        "NiAVObject.";

    ms_kClassName = "NiSceneGraphComponent";
    ms_kComponentName = "Scene Graph";

    ms_kNifFilePathName = "NIF File Path";
    ms_kSceneRootPointerName = "Scene Root Pointer";

    ms_kNifFilePathDescription = "The path to a NIF file to use for the "
        "entity.";

    ms_kTranslationName = "Translation";
    ms_kRotationName = "Rotation";
    ms_kScaleName = "Scale";

}
//---------------------------------------------------------------------------
void NiSceneGraphComponent::_SDMShutdown()
{
    ERR_TRANSLATION_NOT_FOUND = NULL;
    ERR_ROTATION_NOT_FOUND = NULL;
    ERR_SCALE_NOT_FOUND = NULL;
    ERR_FILE_LOAD_FAILED = NULL; 
    ERR_NIF_ROOT_OBJECT = NULL;

    ms_kClassName = NULL;
    ms_kComponentName = NULL;

    ms_kNifFilePathName = NULL;
    ms_kSceneRootPointerName = NULL;

    ms_kNifFilePathDescription = NULL;

    ms_kTranslationName = NULL;
    ms_kRotationName = NULL;
    ms_kScaleName = NULL;
}
//---------------------------------------------------------------------------
NiBool NiSceneGraphComponent::RecursiveFindAnimations(NiAVObject* pkObject)
{
    NIASSERT(pkObject);

    if (pkObject->GetControllers())
    {
        return true;
    }

    const NiPropertyList& kPropList = pkObject->GetPropertyList();
    NiTListIterator kPropIter = kPropList.GetHeadPos();
    while (kPropIter)
    {
        NiProperty* pkProperty = kPropList.GetNext(kPropIter);
        NIASSERT(pkProperty);
        if (pkProperty->GetControllers())
        {
            return true;
        }
    }

    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            if (pkChild)
            {
                if (RecursiveFindAnimations(pkChild))
                {
                    return true;
                }
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
// NiEntityComponentInterface overrides.
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiSceneGraphComponent::Clone(
    bool bInheritProperties)
{
    if (bInheritProperties)
    {
        return NiNew NiSceneGraphComponent(this);
    }
    else
    {
        NiSceneGraphComponent* pkClone = NiNew NiSceneGraphComponent(
            m_spMasterComponent);

        pkClone->m_uFlags = m_uFlags;
        pkClone->m_kNifFilePath = m_kNifFilePath;

        return pkClone;
    }
}
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiSceneGraphComponent::GetMasterComponent() const
{
    return m_spMasterComponent;
}
//---------------------------------------------------------------------------
void NiSceneGraphComponent::SetMasterComponent(
    NiEntityComponentInterface* pkMasterComponent)
{
    if (pkMasterComponent)
    {
        NIASSERT(pkMasterComponent->GetClassName() == GetClassName());
        m_spMasterComponent = (NiSceneGraphComponent*) pkMasterComponent;
    }
    else
    {
        NiTObjectSet<NiFixedString> kPropertyNames(10);
        GetPropertyNames(kPropertyNames);
        for (unsigned int ui = 0; ui < kPropertyNames.GetSize(); ui++)
        {
            bool bMadeUnique;
            NIVERIFY(MakePropertyUnique(kPropertyNames.GetAt(ui),
                bMadeUnique));
        }
        m_spMasterComponent = NULL;
    }
}
//---------------------------------------------------------------------------
void NiSceneGraphComponent::GetDependentPropertyNames(
    NiTObjectSet<NiFixedString>& kDependentPropertyNames)
{
    kDependentPropertyNames.Add(ms_kTranslationName);
    kDependentPropertyNames.Add(ms_kRotationName);
    kDependentPropertyNames.Add(ms_kScaleName);
}
//---------------------------------------------------------------------------
// NiEntityPropertyInterface overrides.
//---------------------------------------------------------------------------
NiBool NiSceneGraphComponent::SetTemplateID(const NiUniqueID&)
{
    //Not supported for custom components
    return false;
}
//---------------------------------------------------------------------------
NiUniqueID  NiSceneGraphComponent::GetTemplateID()
{
    static const NiUniqueID kUniqueID = 
        NiUniqueID(0xA8,0x85,0x41,0x29,0xFD,0x24,0xA6,0x47,0xA8,0x78,0x2,0xC4,
        0x3E,0x7E,0xCD,0xBD);
    return kUniqueID;
}
//---------------------------------------------------------------------------
void NiSceneGraphComponent::AddReference()
{
    this->IncRefCount();
}
//---------------------------------------------------------------------------
void NiSceneGraphComponent::RemoveReference()
{
    this->DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiSceneGraphComponent::GetClassName() const
{
    return ms_kClassName;
}
//---------------------------------------------------------------------------
NiFixedString NiSceneGraphComponent::GetName() const
{
    return ms_kComponentName;
}
//---------------------------------------------------------------------------
NiBool NiSceneGraphComponent::SetName(const NiFixedString&)
{
    // This component does not allow its name to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiSceneGraphComponent::IsAnimated() const
{
    return GetShouldUpdateSceneRoot();
}
//---------------------------------------------------------------------------
void NiSceneGraphComponent::Update(NiEntityPropertyInterface* pkParentEntity,
    float fTime, NiEntityErrorInterface* pkErrors,
    NiExternalAssetManager* pkAssetManager)
{
    if (ShouldReloadScene())
    {
        // Re-synchronize default and local data.
        m_kNifFilePath = GetNifFilePath();

        // Clear out existing scene root.
        m_spSceneRoot = NULL;

        // If the scene root does not exist, create it using the property
        // data.
        if (GetNifFilePath().Exists() && pkAssetManager &&
            pkAssetManager->GetAssetFactory())
        {
            NIASSERT(pkAssetManager);
            NiParamsNIF kNIFParams;
            if (!kNIFParams.SetAssetPath(GetNifFilePath()) ||
                !pkAssetManager->RegisterAndResolve(&kNIFParams))
            {
                // Failing to resolve amounts to failed to load file.
                SetLoadErrorHit(true);
                pkErrors->ReportError(ERR_FILE_LOAD_FAILED,
                    GetNifFilePath(), pkParentEntity->GetName(),
                    ms_kNifFilePathName);
                return;
            }

            NiBool bSuccess = pkAssetManager->Retrieve(&kNIFParams);
            NIASSERT(bSuccess);

            NiAVObject* pkAVObject; 
            bSuccess = kNIFParams.GetSceneRoot(pkAVObject);

            if (bSuccess && pkAVObject != NULL)
            {
                m_spSceneRoot = pkAVObject;

                SetShouldUpdateSceneRoot(NIBOOL_IS_TRUE(
                    RecursiveFindAnimations(m_spSceneRoot)));

                // Perform initial update.
                m_spSceneRoot->Update(0.0f);               
                NiMesh::CompleteSceneModifiers(m_spSceneRoot);
                m_spSceneRoot->UpdateNodeBound();
                m_spSceneRoot->UpdateProperties();
                m_spSceneRoot->UpdateEffects();
            }
            else
            {
                NIASSERT(!"Failed to Retrieve file!");
                // Failing to retrieve amounts to failed to load file.
                SetLoadErrorHit(true);
                pkErrors->ReportError(ERR_FILE_LOAD_FAILED,
                    GetNifFilePath(), pkParentEntity->GetName(),
                    ms_kNifFilePathName);
                return;
            }
        }
    }

    if (m_spSceneRoot)
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
            if (m_spSceneRoot->GetTranslate() != kTranslation)
            {
                m_spSceneRoot->SetTranslate(kTranslation);
                bUpdatedTransforms = true;
            }
            if (m_spSceneRoot->GetRotate() != kRotation)
            {
                m_spSceneRoot->SetRotate(kRotation);
                bUpdatedTransforms = true;
            }
            if (m_spSceneRoot->GetScale() != fScale)
            {
                m_spSceneRoot->SetScale(fScale);
                bUpdatedTransforms = true;
            }
        }

        // Update scene root with the provided time.
        if (bUpdatedTransforms)
        {
            m_spSceneRoot->Update(fTime);
        }
        else if (GetShouldUpdateSceneRoot())
        {
            m_spSceneRoot->UpdateSelected(fTime);
        }
    }
}
//---------------------------------------------------------------------------
void NiSceneGraphComponent::BuildVisibleSet(
    NiEntityRenderingContext* pkRenderingContext,
    NiEntityErrorInterface*)
{
    if (m_spSceneRoot != NULL)
    {
        pkRenderingContext->m_pkCullingProcess->Cull(
            pkRenderingContext->m_pkCamera, m_spSceneRoot,
            pkRenderingContext->m_pkCullingProcess->GetVisibleSet());
    }
}
//---------------------------------------------------------------------------
void NiSceneGraphComponent::GetPropertyNames(
    NiTObjectSet<NiFixedString>& kPropertyNames) const
{
    kPropertyNames.Add(ms_kNifFilePathName);
    kPropertyNames.Add(ms_kSceneRootPointerName);
}
//---------------------------------------------------------------------------
NiBool NiSceneGraphComponent::CanResetProperty(
    const NiFixedString& kPropertyName, bool& bCanReset) const
{
    if (kPropertyName == ms_kNifFilePathName)
    {
        bCanReset = (m_spMasterComponent && GetNifFilePathChanged());
    }
    else if (kPropertyName == ms_kSceneRootPointerName)
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
NiBool NiSceneGraphComponent::ResetProperty(const NiFixedString& kPropertyName)
{
    bool bCanReset;
    if (!CanResetProperty(kPropertyName, bCanReset) || !bCanReset)
    {
        return false;
    }

    NIASSERT(m_spMasterComponent);

    if (kPropertyName == ms_kNifFilePathName)
    {
        SetNifFilePathChanged(false);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiSceneGraphComponent::MakePropertyUnique(
    const NiFixedString& kPropertyName, bool& bMadeUnique)
{
    bool bCanReset;
    if (!CanResetProperty(kPropertyName, bCanReset))
    {
        return false;
    }

    if (!bCanReset && m_spMasterComponent)
    {
        if (kPropertyName == ms_kNifFilePathName)
        {
            SetNifFilePathChanged(true);
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
NiBool NiSceneGraphComponent::GetDisplayName(
    const NiFixedString& kPropertyName, NiFixedString& kDisplayName) const
{
    if (kPropertyName == ms_kNifFilePathName)
    {
        kDisplayName = ms_kNifFilePathName;
    }
    else if (kPropertyName == ms_kSceneRootPointerName)
    {
        // Scene Root Pointer property should not be displayed.
        kDisplayName = NULL;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiSceneGraphComponent::SetDisplayName(const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the display name to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiSceneGraphComponent::GetPrimitiveType(
    const NiFixedString& kPropertyName, NiFixedString& kPrimitiveType) const
{
    if (kPropertyName == ms_kNifFilePathName)
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
NiBool NiSceneGraphComponent::SetPrimitiveType(
    const NiFixedString&, const NiFixedString&)
{
    // This component does not allow the primitive type to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiSceneGraphComponent::GetSemanticType(
    const NiFixedString& kPropertyName, NiFixedString& kSemanticType) const
{
    if (kPropertyName == ms_kNifFilePathName)
    {
        kSemanticType = "NIF Filename";
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
NiBool NiSceneGraphComponent::SetSemanticType(const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the semantic type to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiSceneGraphComponent::GetDescription(
    const NiFixedString& kPropertyName, NiFixedString& kDescription) const
{
    if (kPropertyName == ms_kNifFilePathName)
    {
        kDescription = ms_kNifFilePathDescription;
    }
    else if (kPropertyName == ms_kSceneRootPointerName)
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
NiBool NiSceneGraphComponent::SetDescription(
    const NiFixedString&, const NiFixedString&)
{
    // This component does not allow the description to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiSceneGraphComponent::GetCategory(const NiFixedString& kPropertyName,
    NiFixedString& kCategory) const
{
    if (kPropertyName == ms_kNifFilePathName ||
        kPropertyName == ms_kSceneRootPointerName)
    {
        kCategory = ms_kComponentName;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiSceneGraphComponent::IsPropertyReadOnly(
    const NiFixedString& kPropertyName, bool& bIsReadOnly)
{
    if (kPropertyName == ms_kNifFilePathName)
    {
        bIsReadOnly = false;
        return true;
    }
    else if (kPropertyName == ms_kSceneRootPointerName)
    {
        bIsReadOnly = true;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiSceneGraphComponent::IsPropertyUnique(
    const NiFixedString& kPropertyName, bool& bIsUnique)
{
    if (kPropertyName == ms_kNifFilePathName)
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
    else if (kPropertyName == ms_kSceneRootPointerName)
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
NiBool NiSceneGraphComponent::IsPropertySerializable(
    const NiFixedString& kPropertyName, bool& bIsSerializable)
{
    if (kPropertyName == ms_kNifFilePathName)
    {
        bool bIsUnique;
        NIVERIFY(IsPropertyUnique(kPropertyName, bIsUnique));

        bool bIsReadOnly;
        NIVERIFY(IsPropertyReadOnly(kPropertyName, bIsReadOnly));

        bIsSerializable = bIsUnique && !bIsReadOnly;
    }
    else if (kPropertyName == ms_kSceneRootPointerName)
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
NiBool NiSceneGraphComponent::IsPropertyInheritable(
    const NiFixedString& kPropertyName, bool& bIsInheritable)
{
    if (kPropertyName == ms_kNifFilePathName)
    {
        bIsInheritable = true;
    }
    else if (kPropertyName == ms_kSceneRootPointerName)
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
NiBool NiSceneGraphComponent::IsExternalAssetPath(
    const NiFixedString& kPropertyName, unsigned int,
    bool& bIsExternalAssetPath) const
{
    if (kPropertyName == ms_kNifFilePathName)
    {
        bIsExternalAssetPath = true;
    }
    else if (kPropertyName == ms_kSceneRootPointerName)
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
NiBool NiSceneGraphComponent::GetElementCount(
    const NiFixedString& kPropertyName, unsigned int& uiCount) const
{
    if (kPropertyName == ms_kNifFilePathName ||
        kPropertyName == ms_kSceneRootPointerName)
    {
        uiCount = 1;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiSceneGraphComponent::SetElementCount(
    const NiFixedString& kPropertyName, unsigned int, bool& bCountSet)
{
    if (kPropertyName == ms_kNifFilePathName ||
        kPropertyName == ms_kSceneRootPointerName)
    {
        bCountSet = false;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiSceneGraphComponent::IsCollection(const NiFixedString& kPropertyName,
    bool& bIsCollection) const
{
    if (kPropertyName == ms_kNifFilePathName ||
        kPropertyName == ms_kSceneRootPointerName)
    {
        bIsCollection = false;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiSceneGraphComponent::GetPropertyData(
    const NiFixedString& kPropertyName, NiFixedString& kData,
    unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kNifFilePathName)
    {
        kData = GetNifFilePath();
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiSceneGraphComponent::SetPropertyData(
    const NiFixedString& kPropertyName, const NiFixedString& kData,
    unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kNifFilePathName)
    {
        SetNifFilePath(kData);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiSceneGraphComponent::GetPropertyData(
    const NiFixedString& kPropertyName, NiObject*& pkData,
    unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kSceneRootPointerName)
    {
        pkData = m_spSceneRoot;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
