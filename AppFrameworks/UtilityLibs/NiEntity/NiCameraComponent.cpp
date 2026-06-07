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

#include "NiCameraComponent.h"
#include "NiEntityErrorInterface.h"
#include <NiCloningProcess.h>

NiFixedString NiCameraComponent::ms_kFOVString;
NiFixedString NiCameraComponent::ms_kNearClipString;
NiFixedString NiCameraComponent::ms_kFarClipString;
NiFixedString NiCameraComponent::ms_kOrthoString;
NiFixedString NiCameraComponent::ms_kAspectString;
NiFixedString NiCameraComponent::ms_kWidthString;
NiFixedString NiCameraComponent::ms_kSceneRootString;

NiFixedString NiCameraComponent::ms_kClassName;
NiFixedString NiCameraComponent::ms_kComponentName;

NiFixedString NiCameraComponent::ms_kTranslationName;
NiFixedString NiCameraComponent::ms_kRotationName;
NiFixedString NiCameraComponent::ms_kScaleName;

//---------------------------------------------------------------------------
void NiCameraComponent::_SDMInit()
{
    ms_kFOVString = "Field of View";
    ms_kAspectString = "Aspect Ratio";
    ms_kWidthString = "Orthographic Frustum Width";
    ms_kNearClipString = "Near Clipping Plane";
    ms_kFarClipString = "Far Clipping Plane";
    ms_kOrthoString = "Orthographic";
    ms_kSceneRootString = "Scene Root Pointer";

    ms_kClassName = "NiCameraComponent";
    ms_kComponentName = "Camera";

    ms_kTranslationName = "Translation";
    ms_kRotationName = "Rotation";
    ms_kScaleName = "Scale";
}
//---------------------------------------------------------------------------
void NiCameraComponent::_SDMShutdown()
{
    ms_kFOVString = NULL;
    ms_kAspectString = NULL;
    ms_kWidthString = NULL;
    ms_kNearClipString = NULL;
    ms_kFarClipString = NULL;
    ms_kOrthoString = NULL;
    ms_kSceneRootString = NULL;

    ms_kClassName = NULL;
    ms_kComponentName = NULL;

    ms_kTranslationName = NULL;
    ms_kRotationName = NULL;
    ms_kScaleName = NULL;
}
//---------------------------------------------------------------------------
NiCameraComponent::NiCameraComponent() : m_fFOV(90.0f), m_fAspect(1.33333f), 
    m_fOrthoWidth(100.0f)
{
    m_spCamera = NiNew NiCamera();
    NiFrustum kFrustum = m_spCamera->GetViewFrustum();
    kFrustum.m_fNear = 1.0f;
    kFrustum.m_fFar = 10000.0f;
    m_spCamera->SetViewFrustum(kFrustum);

    // Perform initial update.
    m_spCamera->Update(0.0f);
}
//---------------------------------------------------------------------------
NiCameraComponent::NiCameraComponent(NiCamera* pkCamera, float fFOV,
    float fAspect, float fOrthoWidth) : m_spCamera(pkCamera), m_fFOV(fFOV),
    m_fAspect(fAspect), m_fOrthoWidth(fOrthoWidth)
{
    // Perform initial update.
    m_spCamera->Update(0.0f);
}
//---------------------------------------------------------------------------
void NiCameraComponent::BuildFrustum()
{
    NiFrustum kNewFrustum;
    kNewFrustum = m_spCamera->GetViewFrustum();
    if (m_spCamera->GetViewFrustum().m_bOrtho)
    {
        // if we are building an ortho frustum, use orthowidth
        kNewFrustum.m_fRight = m_fOrthoWidth / 2.0f;
    }
    else
    {
        // if we are building a perspective frustum, use FOV instead of width
        kNewFrustum.m_fRight = NiSin((m_fFOV / 2.0f) * NI_TWO_PI / 360.0f);
    }
    kNewFrustum.m_fLeft = - kNewFrustum.m_fRight;
    kNewFrustum.m_fTop = kNewFrustum.m_fRight / m_fAspect;
    kNewFrustum.m_fBottom = -kNewFrustum.m_fTop;
    m_spCamera->SetViewFrustum(kNewFrustum);
}
//---------------------------------------------------------------------------
// NiEntityComponentInterface overrides.
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiCameraComponent::Clone(bool)
{
    NiCloningProcess kCloning;
    kCloning.m_eCopyType = NiObjectNET::COPY_EXACT;
    return NiNew NiCameraComponent((NiCamera*) m_spCamera->Clone(kCloning),
        m_fFOV, m_fAspect, m_fOrthoWidth);
}
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiCameraComponent::GetMasterComponent() const
{
    // This component does not have a master component.
    return NULL;
}
//---------------------------------------------------------------------------
void NiCameraComponent::SetMasterComponent(NiEntityComponentInterface*)
{
    // This component does not have a master component.
}
//---------------------------------------------------------------------------
void NiCameraComponent::GetDependentPropertyNames(
    NiTObjectSet<NiFixedString>& kDependentPropertyNames)
{
    kDependentPropertyNames.Add(ms_kTranslationName);
    kDependentPropertyNames.Add(ms_kRotationName);
    kDependentPropertyNames.Add(ms_kScaleName);
}
//---------------------------------------------------------------------------
// NiEntityPropertyInterface overrides.
//---------------------------------------------------------------------------
NiBool NiCameraComponent::SetTemplateID(const NiUniqueID&)
{
    //Not supported for custom components
    return false;
}
//---------------------------------------------------------------------------
NiUniqueID  NiCameraComponent::GetTemplateID()
{
    static const NiUniqueID kUniqueID = 
        NiUniqueID(0xFD,0xAB,0x42,0xB6,0x60,0x4B,0xEA,0x4C,
        0x8D,0x6,0x6C,0x7E,0xB0,0xF,0x99,0xFF);
    return kUniqueID;
}
//---------------------------------------------------------------------------
void NiCameraComponent::AddReference()
{
    this->IncRefCount();
}
//---------------------------------------------------------------------------
void NiCameraComponent::RemoveReference()
{
    this->DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiCameraComponent::GetClassName() const
{
    return ms_kClassName;
}
//---------------------------------------------------------------------------
NiFixedString NiCameraComponent::GetName() const
{
    return ms_kComponentName;
}
//---------------------------------------------------------------------------
NiBool NiCameraComponent::SetName(const NiFixedString&)
{
    // This component does not allow its name to be set.
    return false;
}
//---------------------------------------------------------------------------
void NiCameraComponent::Update(NiEntityPropertyInterface* pkParentEntity, 
    float fTime, NiEntityErrorInterface*,
    NiExternalAssetManager*)
{
    bool bDependentPropertiesFound = true;

    // Find dependent properties.
    NiPoint3 kTranslation;
    if (!pkParentEntity->GetPropertyData(ms_kTranslationName,
        kTranslation, 0))
    {
        bDependentPropertiesFound = false;
    }
    NiMatrix3 kRotation;
    if (!pkParentEntity->GetPropertyData(ms_kRotationName, kRotation, 0))
    {
        bDependentPropertiesFound = false;
    }
    float fScale;
    if (!pkParentEntity->GetPropertyData(ms_kScaleName, fScale, 0))
    {
        bDependentPropertiesFound = false;
    }

    // Use dependent properties to update transform of scene root.
    bool bUpdatedTransforms = false;
    if (bDependentPropertiesFound)
    {
        if (m_spCamera->GetTranslate() != kTranslation)
        {
            m_spCamera->SetTranslate(kTranslation);
            bUpdatedTransforms = true;
        }
        if (m_spCamera->GetRotate() != kRotation)
        {
            m_spCamera->SetRotate(kRotation);
            bUpdatedTransforms = true;
        }
        if (m_spCamera->GetScale() != fScale)
        {
            m_spCamera->SetScale(fScale);
            bUpdatedTransforms = true;
        }
    }

    if (bUpdatedTransforms)
    {
        m_spCamera->Update(fTime);
    }
}
//---------------------------------------------------------------------------
void NiCameraComponent::BuildVisibleSet(NiEntityRenderingContext*,
    NiEntityErrorInterface*)
{
    // This component has no work to do during BuildVisibleSet.
}
//---------------------------------------------------------------------------
void NiCameraComponent::GetPropertyNames(
    NiTObjectSet<NiFixedString>& kPropertyNames) const
{
    kPropertyNames.Add(ms_kFOVString);
    kPropertyNames.Add(ms_kAspectString);
    kPropertyNames.Add(ms_kWidthString);
    kPropertyNames.Add(ms_kNearClipString);
    kPropertyNames.Add(ms_kFarClipString);
    kPropertyNames.Add(ms_kOrthoString);
    kPropertyNames.Add(ms_kSceneRootString);
}
//---------------------------------------------------------------------------
NiBool NiCameraComponent::CanResetProperty(const NiFixedString& kPropertyName, 
    bool& bCanReset) const
{
    if ((kPropertyName == ms_kFOVString) ||
        (kPropertyName == ms_kAspectString) ||
        (kPropertyName == ms_kWidthString) ||
        (kPropertyName == ms_kNearClipString) ||
        (kPropertyName == ms_kFarClipString) ||
        (kPropertyName == ms_kOrthoString) ||
        (kPropertyName == ms_kSceneRootString))
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
NiBool NiCameraComponent::ResetProperty(const NiFixedString&)
{
    // No properties can be reset.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiCameraComponent::MakePropertyUnique(
    const NiFixedString& kPropertyName, bool& bMadeUnique)
{
    bool bCanReset;
    if (!CanResetProperty(kPropertyName, bCanReset))
    {
        return false;
    }

    // No properties are inherited, so they are all already unique.
    bMadeUnique = false;

    return true;
}
//---------------------------------------------------------------------------
NiBool NiCameraComponent::GetDisplayName(const NiFixedString& kPropertyName, 
    NiFixedString& kDisplayName) const
{
    if (kPropertyName == ms_kFOVString ||
        kPropertyName == ms_kAspectString ||
        kPropertyName == ms_kWidthString ||
        kPropertyName == ms_kNearClipString ||
        kPropertyName == ms_kFarClipString ||
        kPropertyName == ms_kOrthoString)
    {
        kDisplayName = kPropertyName;
    }
    else if (kPropertyName == ms_kSceneRootString)
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
NiBool NiCameraComponent::SetDisplayName(const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the display name to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiCameraComponent::GetPrimitiveType(const NiFixedString& kPropertyName, 
    NiFixedString& kPrimitiveType) const
{
    if ((kPropertyName == ms_kFOVString) || 
        (kPropertyName == ms_kAspectString) ||
        (kPropertyName == ms_kWidthString) ||
        (kPropertyName == ms_kNearClipString) ||
        (kPropertyName == ms_kFarClipString))
    {
        kPrimitiveType = PT_FLOAT;
    }
    else if (kPropertyName == ms_kOrthoString)
    {
        kPrimitiveType = PT_BOOL;
    }
    else if (kPropertyName == ms_kSceneRootString)
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
NiBool NiCameraComponent::SetPrimitiveType(const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the primitive type to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiCameraComponent::GetSemanticType(const NiFixedString& kPropertyName, 
    NiFixedString& kSemanticType) const
{
    if ((kPropertyName == ms_kFOVString) || 
        (kPropertyName == ms_kAspectString) ||
        (kPropertyName == ms_kWidthString) ||
        (kPropertyName == ms_kNearClipString) ||
        (kPropertyName == ms_kFarClipString))
    {
        kSemanticType = PT_FLOAT;
    }
    else if (kPropertyName == ms_kOrthoString)
    {
        kSemanticType = PT_BOOL;
    }
    else if (kPropertyName == ms_kSceneRootString)
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
NiBool NiCameraComponent::SetSemanticType(const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the semantic type to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiCameraComponent::GetDescription(const NiFixedString& kPropertyName, 
    NiFixedString& kDescription) const
{
    if (kPropertyName == ms_kFOVString ||
        kPropertyName == ms_kAspectString ||
        kPropertyName == ms_kWidthString ||
        kPropertyName == ms_kNearClipString ||
        kPropertyName == ms_kFarClipString ||
        kPropertyName == ms_kOrthoString)
    {
        kDescription = kPropertyName;
    }
    else if (kPropertyName == ms_kSceneRootString)
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
NiBool NiCameraComponent::SetDescription(const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the description to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiCameraComponent::GetCategory(const NiFixedString& kPropertyName, 
    NiFixedString& kCategory) const
{
    if ((kPropertyName == ms_kFOVString) ||
        (kPropertyName == ms_kAspectString) ||
        (kPropertyName == ms_kWidthString) ||
        (kPropertyName == ms_kNearClipString) ||
        (kPropertyName == ms_kFarClipString) ||
        (kPropertyName == ms_kOrthoString) ||
        (kPropertyName == ms_kSceneRootString))
    {
        kCategory = ms_kComponentName;
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiCameraComponent::IsPropertyReadOnly(
    const NiFixedString& kPropertyName, bool& bIsReadOnly)
{
    if ((kPropertyName == ms_kAspectString) ||
        (kPropertyName == ms_kNearClipString) ||
        (kPropertyName == ms_kFarClipString) ||
        (kPropertyName == ms_kOrthoString))
    {
        bIsReadOnly = false;
        return true;
    }
    else if (kPropertyName == ms_kSceneRootString)
    {
        bIsReadOnly = true;
        return true;
    }

    if (m_spCamera->GetViewFrustum().m_bOrtho)
    {
        // if the camera is ortho, the available properties are different
        if (kPropertyName == ms_kWidthString)
        {
            bIsReadOnly = false;
            return true;
        }
        else if (kPropertyName == ms_kFOVString)
        {
            bIsReadOnly = true;
            return true;
        }
    }
    else
    {
        if (kPropertyName == ms_kWidthString)
        {
            bIsReadOnly = true;
            return true;
        }
        else if (kPropertyName == ms_kFOVString)
        {
            bIsReadOnly = false;
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiBool NiCameraComponent::IsPropertyUnique(
    const NiFixedString& kPropertyName, bool& bIsUnique)
{
    if ((kPropertyName == ms_kFOVString) ||
        (kPropertyName == ms_kAspectString) ||
        (kPropertyName == ms_kWidthString) ||
        (kPropertyName == ms_kNearClipString) ||
        (kPropertyName == ms_kFarClipString) ||
        (kPropertyName == ms_kOrthoString) ||
        (kPropertyName == ms_kSceneRootString))
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
NiBool NiCameraComponent::IsPropertySerializable(
    const NiFixedString& kPropertyName, bool& bIsSerializable)
{
    if (kPropertyName == ms_kFOVString ||
        kPropertyName == ms_kAspectString ||
        kPropertyName == ms_kWidthString ||
        kPropertyName == ms_kNearClipString ||
        kPropertyName == ms_kFarClipString ||
        kPropertyName == ms_kOrthoString)
    {
        bool bIsUnique;
        NIVERIFY(IsPropertyUnique(kPropertyName, bIsUnique));

        bool bIsReadOnly;
        NIVERIFY(IsPropertyReadOnly(kPropertyName, bIsReadOnly));

        bIsSerializable = bIsUnique && !bIsReadOnly;
    }
    else if (kPropertyName == ms_kSceneRootString)
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
NiBool NiCameraComponent::IsPropertyInheritable(
    const NiFixedString& kPropertyName, bool& bIsInheritable)
{
    if ((kPropertyName == ms_kFOVString) ||
        (kPropertyName == ms_kAspectString) ||
        (kPropertyName == ms_kWidthString) ||
        (kPropertyName == ms_kNearClipString) ||
        (kPropertyName == ms_kFarClipString) ||
        (kPropertyName == ms_kOrthoString) ||
        (kPropertyName == ms_kSceneRootString))
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
NiBool NiCameraComponent::IsExternalAssetPath(
    const NiFixedString& kPropertyName, unsigned int,
    bool& bIsExternalAssetPath) const
{
    if (kPropertyName == ms_kFOVString ||
        kPropertyName == ms_kAspectString ||
        kPropertyName == ms_kWidthString ||
        kPropertyName == ms_kNearClipString ||
        kPropertyName == ms_kFarClipString ||
        kPropertyName == ms_kOrthoString ||
        kPropertyName == ms_kSceneRootString)
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
NiBool NiCameraComponent::GetElementCount(const NiFixedString& kPropertyName,
    unsigned int& uiCount) const
{
    if ((kPropertyName == ms_kFOVString) ||
        (kPropertyName == ms_kAspectString) ||
        (kPropertyName == ms_kWidthString) ||
        (kPropertyName == ms_kNearClipString) ||
        (kPropertyName == ms_kFarClipString) ||
        (kPropertyName == ms_kOrthoString) ||
        (kPropertyName == ms_kSceneRootString))
    {
        uiCount = 1;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiCameraComponent::SetElementCount(const NiFixedString& kPropertyName,
    unsigned int, bool& bCountSet)
{
    if ((kPropertyName == ms_kFOVString) ||
        (kPropertyName == ms_kAspectString) ||
        (kPropertyName == ms_kWidthString) ||
        (kPropertyName == ms_kNearClipString) ||
        (kPropertyName == ms_kFarClipString) ||
        (kPropertyName == ms_kOrthoString) ||
        (kPropertyName == ms_kSceneRootString))
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
NiBool NiCameraComponent::IsCollection(const NiFixedString& kPropertyName,
    bool& bIsCollection) const
{
    if ((kPropertyName == ms_kFOVString) ||
        (kPropertyName == ms_kAspectString) ||
        (kPropertyName == ms_kWidthString) ||
        (kPropertyName == ms_kNearClipString) ||
        (kPropertyName == ms_kFarClipString) ||
        (kPropertyName == ms_kOrthoString) ||
        (kPropertyName == ms_kSceneRootString))
    {
        bIsCollection = false;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiCameraComponent::GetPropertyData(const NiFixedString& kPropertyName, 
    float& fData, unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kFOVString)
    {
        fData = m_fFOV;
    }
    else if (kPropertyName == ms_kAspectString)
    {
        fData = m_fAspect;
    }
    else if (kPropertyName == ms_kWidthString)
    {
        fData = m_fOrthoWidth;
    }
    else if (kPropertyName == ms_kNearClipString)
    {
        fData = m_spCamera->GetViewFrustum().m_fNear;
    }
    else if (kPropertyName == ms_kFarClipString)
    {
        fData = m_spCamera->GetViewFrustum().m_fFar;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiCameraComponent::SetPropertyData(const NiFixedString& kPropertyName, 
    float fData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kFOVString)
    {
        m_fFOV = fData;
        BuildFrustum();
    }
    else if (kPropertyName == ms_kAspectString)
    {
        m_fAspect = fData;
        BuildFrustum();
    }
    else if (kPropertyName == ms_kWidthString)
    {
        m_fOrthoWidth = fData;
        BuildFrustum();
    }
    else if (kPropertyName == ms_kNearClipString)
    {
        NiFrustum kNewFrustum = m_spCamera->GetViewFrustum();
        kNewFrustum.m_fNear = fData;
        m_spCamera->SetViewFrustum(kNewFrustum);
    }
    else if (kPropertyName == ms_kFarClipString)
    {
        NiFrustum kNewFrustum = m_spCamera->GetViewFrustum();
        kNewFrustum.m_fFar = fData;
        m_spCamera->SetViewFrustum(kNewFrustum);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiCameraComponent::GetPropertyData(const NiFixedString& kPropertyName, 
    bool& bData, unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kOrthoString)
    {
       bData = m_spCamera->GetViewFrustum().m_bOrtho;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiCameraComponent::SetPropertyData(const NiFixedString& kPropertyName, 
    bool bData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kOrthoString)
    {
        NiFrustum kNewFrustum = m_spCamera->GetViewFrustum();
        kNewFrustum.m_bOrtho = bData;
        m_spCamera->SetViewFrustum(kNewFrustum);
        BuildFrustum();
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiCameraComponent::GetPropertyData(const NiFixedString& kPropertyName, 
    NiObject*& pkData, unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kSceneRootString)
    {
        pkData = m_spCamera;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
