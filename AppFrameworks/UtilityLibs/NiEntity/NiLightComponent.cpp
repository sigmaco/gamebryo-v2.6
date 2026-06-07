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

#include "NiLightComponent.h"
#include <NiAmbientLight.h>
#include <NiDirectionalLight.h>
#include <NiSpotLight.h>
#include <NiNode.h>

NiFixedString NiLightComponent::ERR_TRANSLATION_NOT_FOUND;
NiFixedString NiLightComponent::ERR_ROTATION_NOT_FOUND;
NiFixedString NiLightComponent::ERR_SCALE_NOT_FOUND;
NiFixedString NiLightComponent::ERR_LIGHT_TYPE_NOT_VALID;

NiFixedString NiLightComponent::LT_AMBIENT;
NiFixedString NiLightComponent::LT_DIRECTIONAL;
NiFixedString NiLightComponent::LT_POINT;
NiFixedString NiLightComponent::LT_SPOT;

NiFixedString NiLightComponent::ms_kClassName;
NiFixedString NiLightComponent::ms_kComponentName;

NiFixedString NiLightComponent::ms_kLightTypeName;
NiFixedString NiLightComponent::ms_kDimmerName;
NiFixedString NiLightComponent::ms_kAmbientColorName;
NiFixedString NiLightComponent::ms_kDiffuseColorName;
NiFixedString NiLightComponent::ms_kSpecularColorName;
NiFixedString NiLightComponent::ms_kConstantAttenuationName;
NiFixedString NiLightComponent::ms_kLinearAttenuationName;
NiFixedString NiLightComponent::ms_kQuadraticAttenuationName;
NiFixedString NiLightComponent::ms_kOuterSpotAngleName;
NiFixedString NiLightComponent::ms_kInnerSpotAngleName;
NiFixedString NiLightComponent::ms_kSpotExponentName;
NiFixedString NiLightComponent::ms_kAffectedEntitiesName;
NiFixedString NiLightComponent::ms_kWorldDirName;
NiFixedString NiLightComponent::ms_kSceneRootPointerName;

NiFixedString NiLightComponent::ms_kLightTypeDescription;
NiFixedString NiLightComponent::ms_kDimmerDescription;
NiFixedString NiLightComponent::ms_kAmbientColorDescription;
NiFixedString NiLightComponent::ms_kDiffuseColorDescription;
NiFixedString NiLightComponent::ms_kSpecularColorDescription;
NiFixedString NiLightComponent::ms_kConstantAttenuationDescription;
NiFixedString NiLightComponent::ms_kLinearAttenuationDescription;
NiFixedString NiLightComponent::ms_kQuadraticAttenuationDescription;
NiFixedString NiLightComponent::ms_kOuterSpotAngleDescription;
NiFixedString NiLightComponent::ms_kInnerSpotAngleDescription;
NiFixedString NiLightComponent::ms_kSpotExponentDescription;
NiFixedString NiLightComponent::ms_kAffectedEntitiesDescription;
NiFixedString NiLightComponent::ms_kWorldDirDescription;

NiFixedString NiLightComponent::ms_kTranslationName;
NiFixedString NiLightComponent::ms_kRotationName;
NiFixedString NiLightComponent::ms_kScaleName;

NiFixedString NiLightComponent::ms_kSpotAngleName;

//---------------------------------------------------------------------------
void NiLightComponent::_SDMInit()
{
    ERR_TRANSLATION_NOT_FOUND = "Translation property not found.";
    ERR_ROTATION_NOT_FOUND = "Rotation property not found.";
    ERR_SCALE_NOT_FOUND = "Scale property not found.";
    ERR_LIGHT_TYPE_NOT_VALID = "The specified light type is not valid.";

    LT_AMBIENT = "Ambient";
    LT_DIRECTIONAL = "Directional";
    LT_POINT = "Point";
    LT_SPOT = "Spot";

    ms_kClassName = "NiLightComponent";
    ms_kComponentName = "Light";

    ms_kLightTypeName = "Light Type";
    ms_kDimmerName = "Dimmer";
    ms_kAmbientColorName = "Color (Ambient)";
    ms_kDiffuseColorName = "Color (Diffuse)";
    ms_kSpecularColorName = "Color (Specular)";
    ms_kConstantAttenuationName = "Attenuation (Constant)";
    ms_kLinearAttenuationName = "Attenuation (Linear)";
    ms_kQuadraticAttenuationName = "Attenuation (Quadratic)";
    ms_kOuterSpotAngleName = "Spot Angle (Outer)";
    ms_kInnerSpotAngleName = "Spot Angle (Inner)";
    ms_kSpotExponentName = "Spot Exponent";
    ms_kAffectedEntitiesName = "Affected Entities";
    ms_kWorldDirName = "Direction";
    ms_kSceneRootPointerName = "Scene Root Pointer";

    ms_kLightTypeDescription = "The type of light.";
    ms_kDimmerDescription = "The relative intensity of the light.";
    ms_kAmbientColorDescription = "The ambient color of the light.";
    ms_kDiffuseColorDescription = "The diffuse color of the light.";
    ms_kSpecularColorDescription = "The specular color of the light.";
    ms_kConstantAttenuationDescription = "The constant distance attenuation "
        "factor for the light.";
    ms_kLinearAttenuationDescription = "The linear distance attenuation "
        "factor for the light.";
    ms_kQuadraticAttenuationDescription = "The quadratic distance "
        "attenuation factor for the light.";
    ms_kOuterSpotAngleDescription = "The angle of the spot light's outer "
        "cone. Objects outside of this cone will not receive any light from "
        "the spot light.";
    ms_kInnerSpotAngleDescription = "The angle of the spot light's inner "
        "cone. Objects inside of this cone will receive the full "
        "contribution from the spot light.";
    ms_kSpotExponentDescription = "The angular attenuation factor for the "
        "spot light.";
    ms_kAffectedEntitiesDescription = "Entities that the light will affect.";
    ms_kWorldDirDescription = "The wold-space direction of the light.";

    ms_kTranslationName = "Translation";
    ms_kRotationName = "Rotation";
    ms_kScaleName = "Scale";

    ms_kSpotAngleName = "Spot Angle";
}
//---------------------------------------------------------------------------
void NiLightComponent::_SDMShutdown()
{
    ERR_TRANSLATION_NOT_FOUND = NULL;
    ERR_ROTATION_NOT_FOUND = NULL;
    ERR_SCALE_NOT_FOUND = NULL;
    ERR_LIGHT_TYPE_NOT_VALID = NULL;

    LT_AMBIENT = NULL;
    LT_DIRECTIONAL = NULL;
    LT_POINT = NULL;
    LT_SPOT = NULL;

    ms_kClassName = NULL;
    ms_kComponentName = NULL;

    ms_kLightTypeName = NULL;
    ms_kDimmerName = NULL;
    ms_kAmbientColorName = NULL;
    ms_kDiffuseColorName = NULL;
    ms_kSpecularColorName = NULL;
    ms_kConstantAttenuationName = NULL;
    ms_kLinearAttenuationName = NULL;
    ms_kQuadraticAttenuationName = NULL;
    ms_kOuterSpotAngleName = NULL;
    ms_kInnerSpotAngleName = NULL;
    ms_kSpotExponentName = NULL;
    ms_kAffectedEntitiesName = NULL;
    ms_kWorldDirName = NULL;
    ms_kSceneRootPointerName = NULL;

    ms_kLightTypeDescription = NULL;
    ms_kDimmerDescription = NULL;
    ms_kAmbientColorDescription = NULL;
    ms_kDiffuseColorDescription = NULL;
    ms_kSpecularColorDescription = NULL;
    ms_kConstantAttenuationDescription = NULL;
    ms_kLinearAttenuationDescription = NULL;
    ms_kQuadraticAttenuationDescription = NULL;
    ms_kOuterSpotAngleDescription = NULL;
    ms_kInnerSpotAngleDescription = NULL;
    ms_kSpotExponentDescription = NULL;
    ms_kAffectedEntitiesDescription = NULL;
    ms_kWorldDirDescription = NULL;

    ms_kTranslationName = NULL;
    ms_kRotationName = NULL;
    ms_kScaleName = NULL;

    ms_kSpotAngleName = NULL;
}
//---------------------------------------------------------------------------
// NiEntityComponentInterface overrides.
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiLightComponent::Clone(bool)
{
    NiLightComponent* pkClone = NiNew NiLightComponent(m_kLightType,
        m_fDimmer, m_kAmbientColor, m_kDiffuseColor, m_kSpecularColor,
        m_fConstantAttenuation, m_fLinearAttenuation,
        m_fQuadraticAttenuation, m_fOuterSpotAngle, m_fInnerSpotAngle,
        m_fSpotExponent, m_kAffectedEntities.GetAllocatedSize(),
        m_kAffectedEntities.GetGrowBy());

    for (unsigned int ui = 0; ui < m_kAffectedEntities.GetSize(); ui++)
    {
        pkClone->m_kAffectedEntities.SetAt(ui, m_kAffectedEntities.GetAt(ui));
    }

    return pkClone;
}
//---------------------------------------------------------------------------
NiEntityComponentInterface* NiLightComponent::GetMasterComponent() const
{
    // This component does not have a master component.
    return NULL;
}
//---------------------------------------------------------------------------
void NiLightComponent::SetMasterComponent(NiEntityComponentInterface*)
{
    // This component does not have a master component.
}
//---------------------------------------------------------------------------
void NiLightComponent::GetDependentPropertyNames(
    NiTObjectSet<NiFixedString>& kDependentPropertyNames)
{
    kDependentPropertyNames.Add(ms_kTranslationName);
    kDependentPropertyNames.Add(ms_kRotationName);
    kDependentPropertyNames.Add(ms_kScaleName);
}
//---------------------------------------------------------------------------
// NiEntityPropertyInterface overrides.
//---------------------------------------------------------------------------
NiBool NiLightComponent::SetTemplateID(const NiUniqueID&)
{
    //Not supported for custom components
    return false;
}
//---------------------------------------------------------------------------
NiUniqueID  NiLightComponent::GetTemplateID()
{
    static const NiUniqueID kUniqueID = 
        NiUniqueID(0x18,0x26,0xF,0xD9,0xF0,0xE1,0x8A,0x4D,0x91,0xEF,0x47,0xE8,
        0x20,0xE8,0x7E,0xFE);
    return kUniqueID;
}
//---------------------------------------------------------------------------
void NiLightComponent::AddReference()
{
    this->IncRefCount();
}
//---------------------------------------------------------------------------
void NiLightComponent::RemoveReference()
{
    this->DecRefCount();
}
//---------------------------------------------------------------------------
NiFixedString NiLightComponent::GetClassName() const
{
    return ms_kClassName;
}
//---------------------------------------------------------------------------
NiFixedString NiLightComponent::GetName() const
{
    return ms_kComponentName;
}
//---------------------------------------------------------------------------
NiBool NiLightComponent::SetName(const NiFixedString&)
{
    // This component does not allow its name to be set.
    return false;
}
//---------------------------------------------------------------------------
void NiLightComponent::Update(NiEntityPropertyInterface* pkParentEntity,
    float fTime, NiEntityErrorInterface* pkErrors,
    NiExternalAssetManager*)
{
    if (!m_spLight && !GetLightCreationErrorHit())
    {
        // Only create light if the light type is valid.
        if (!IsLightTypeValid(m_kLightType))
        {
            pkErrors->ReportError(ERR_LIGHT_TYPE_NOT_VALID, m_kLightType,
                pkParentEntity->GetName(), ms_kLightTypeName);
            SetLightCreationErrorHit(true);
            return;
        }

        // Create the light based on the light type.
        if (m_kLightType == LT_AMBIENT)
        {
            m_spLight = NiNew NiAmbientLight();
        }
        else if (m_kLightType == LT_DIRECTIONAL)
        {
            m_spLight = NiNew NiDirectionalLight();
        }
        else if (m_kLightType == LT_POINT)
        {
            m_spLight = NiNew NiPointLight();
        }
        else if (m_kLightType == LT_SPOT)
        {
            m_spLight = NiNew NiSpotLight();
        }
        else
        {
            // This assertion should never be hit, since that case is handled
            // by the call to IsLightTypeValid above.
            NIASSERT(false);
        }

        // Enable setting of light properties.
        SetLightPropertiesChanged(true);
        SetAffectedEntitiesChanged(true);

        // Perform initial update.
        m_spLight->Update(0.0f);
    }

    if (m_spLight)
    {
        if (GetLightPropertiesChanged())
        {
            // Set the properties of the light.
            m_spLight->SetDimmer(GetDimmer());
            m_spLight->SetAmbientColor(GetAmbientColor());
            m_spLight->SetDiffuseColor(GetDiffuseColor());
            m_spLight->SetSpecularColor(GetSpecularColor());
            NiPointLight* pkPointLight = NiDynamicCast(NiPointLight,
                m_spLight);
            if (pkPointLight)
            {
                pkPointLight->SetConstantAttenuation(
                    GetConstantAttenuation());
                pkPointLight->SetLinearAttenuation(GetLinearAttenuation());
                pkPointLight->SetQuadraticAttenuation(
                    GetQuadraticAttenuation());

                NiSpotLight* pkSpotLight = NiDynamicCast(NiSpotLight,
                    pkPointLight);
                if (pkSpotLight)
                {
                    pkSpotLight->SetSpotAngle(GetOuterSpotAngle());
                    pkSpotLight->SetInnerSpotAngle(GetInnerSpotAngle());
                    pkSpotLight->SetSpotExponent(GetSpotExponent());
                }
            }

            SetLightPropertiesChanged(false);
        }

        if (GetAffectedEntitiesChanged())
        {
            // Clear all old affected entities.
            const NiNodeList& kAffectedNodesList =
                m_spLight->GetAffectedNodeList();
            NiTPrimitiveSet<NiNode*> kAffectedNodesSet(
                kAffectedNodesList.GetSize());
            NiTListIterator kIter = kAffectedNodesList.GetHeadPos();
            while (kIter)
            {
                kAffectedNodesSet.Add(kAffectedNodesList.GetNext(kIter));
            }
            m_spLight->DetachAllAffectedNodes();
            for (unsigned int ui = 0; ui < kAffectedNodesSet.GetSize();
                ui++)
            {
                kAffectedNodesSet.GetAt(ui)->UpdateEffects();
            }

            // Set affected entities for the light.
            NiBool bAllSuccessful = true;
            for (unsigned int ui = 0; ui < m_kAffectedEntities.GetSize();
                ui++)
            {
                NiEntityInterface* pkEntity = m_kAffectedEntities.GetAt(ui);
                if (!pkEntity)
                {
                    continue;
                }

                unsigned int uiCount;
                if (!pkEntity->GetElementCount(ms_kSceneRootPointerName,
                    uiCount))
                {
                    continue;
                }

                for (unsigned int uiIndex = 0; uiIndex < uiCount; uiIndex++)
                {
                    NiObject* pkSceneRoot;
                    NIVERIFY(pkEntity->GetPropertyData(
                        ms_kSceneRootPointerName, pkSceneRoot, uiIndex));

                    if (!pkSceneRoot)
                    {
                        bAllSuccessful = false;
                    }

                    NiNode* pkNode = NiDynamicCast(NiNode, pkSceneRoot);
                    if (pkNode)
                    {
                        m_spLight->AttachAffectedNode(pkNode);
                        pkNode->UpdateEffects();
                    }
                }
            }

            if (bAllSuccessful)
            {
                SetAffectedEntitiesChanged(false);
            }
        }

        // Find dependent properties.
        NiBool bDependentPropertiesFound = true;
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

        // Use dependent properties to update the transform of the light.
        bool bUpdatedTransforms = false;
        if (bDependentPropertiesFound)
        {
            if (m_spLight->GetTranslate() != kTranslation)
            {
                m_spLight->SetTranslate(kTranslation);
                bUpdatedTransforms = true;
            }
            if (m_spLight->GetRotate() != kRotation)
            {
                m_spLight->SetRotate(kRotation);
                bUpdatedTransforms = true;
            }
            if (m_spLight->GetScale() != fScale)
            {
                m_spLight->SetScale(fScale);
                bUpdatedTransforms = true;
            }
        }

        // Update the light with the provided time.
        if (bUpdatedTransforms)
        {
            m_spLight->Update(fTime);
        }
    }
}
//---------------------------------------------------------------------------
void NiLightComponent::BuildVisibleSet(NiEntityRenderingContext*,
    NiEntityErrorInterface*)
{
    // This component has no work to do during BuildVisibleSet.
}
//---------------------------------------------------------------------------
void NiLightComponent::GetPropertyNames(
    NiTObjectSet<NiFixedString>& kPropertyNames) const
{
    kPropertyNames.Add(ms_kLightTypeName);
    kPropertyNames.Add(ms_kDimmerName);
    kPropertyNames.Add(ms_kAmbientColorName);
    kPropertyNames.Add(ms_kDiffuseColorName);
    kPropertyNames.Add(ms_kSpecularColorName);
    kPropertyNames.Add(ms_kConstantAttenuationName);
    kPropertyNames.Add(ms_kLinearAttenuationName);
    kPropertyNames.Add(ms_kQuadraticAttenuationName);
    kPropertyNames.Add(ms_kOuterSpotAngleName);
    kPropertyNames.Add(ms_kInnerSpotAngleName);
    kPropertyNames.Add(ms_kSpotExponentName);
    kPropertyNames.Add(ms_kAffectedEntitiesName);
    kPropertyNames.Add(ms_kWorldDirName);
    kPropertyNames.Add(ms_kSceneRootPointerName);
}
//---------------------------------------------------------------------------
NiBool NiLightComponent::CanResetProperty(const NiFixedString& kPropertyName,
    bool& bCanReset) const
{
    if (kPropertyName == ms_kLightTypeName ||
        kPropertyName == ms_kDimmerName ||
        kPropertyName == ms_kAmbientColorName ||
        kPropertyName == ms_kDiffuseColorName ||
        kPropertyName == ms_kSpecularColorName ||
        kPropertyName == ms_kConstantAttenuationName ||
        kPropertyName == ms_kLinearAttenuationName ||
        kPropertyName == ms_kQuadraticAttenuationName ||
        kPropertyName == ms_kOuterSpotAngleName ||
        kPropertyName == ms_kInnerSpotAngleName ||
        kPropertyName == ms_kSpotExponentName ||
        kPropertyName == ms_kAffectedEntitiesName ||
        kPropertyName == ms_kWorldDirName ||
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
NiBool NiLightComponent::ResetProperty(const NiFixedString&)
{
    // No properties can be reset.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiLightComponent::MakePropertyUnique(const NiFixedString& kPropertyName,
    bool& bMadeUnique)
{
    if (kPropertyName == ms_kLightTypeName ||
        kPropertyName == ms_kDimmerName ||
        kPropertyName == ms_kAmbientColorName ||
        kPropertyName == ms_kDiffuseColorName ||
        kPropertyName == ms_kSpecularColorName ||
        kPropertyName == ms_kConstantAttenuationName ||
        kPropertyName == ms_kLinearAttenuationName ||
        kPropertyName == ms_kQuadraticAttenuationName ||
        kPropertyName == ms_kOuterSpotAngleName ||
        kPropertyName == ms_kInnerSpotAngleName ||
        kPropertyName == ms_kSpotExponentName ||
        kPropertyName == ms_kAffectedEntitiesName ||
        kPropertyName == ms_kWorldDirName ||
        kPropertyName == ms_kSceneRootPointerName)
    {
        // No properties are inherited, so they are all already unique.
        bMadeUnique = false;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiLightComponent::GetDisplayName(const NiFixedString& kPropertyName,
    NiFixedString& kDisplayName) const
{
    if (kPropertyName == ms_kLightTypeName ||
        kPropertyName == ms_kDimmerName ||
        kPropertyName == ms_kAmbientColorName ||
        kPropertyName == ms_kDiffuseColorName ||
        kPropertyName == ms_kSpecularColorName ||
        kPropertyName == ms_kConstantAttenuationName ||
        kPropertyName == ms_kLinearAttenuationName ||
        kPropertyName == ms_kQuadraticAttenuationName ||
        kPropertyName == ms_kOuterSpotAngleName ||
        kPropertyName == ms_kInnerSpotAngleName ||
        kPropertyName == ms_kSpotExponentName ||
        kPropertyName == ms_kAffectedEntitiesName ||
        kPropertyName == ms_kWorldDirName)
    {
        kDisplayName = kPropertyName;
    }
    else if (kPropertyName == ms_kSceneRootPointerName)
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
NiBool NiLightComponent::SetDisplayName(const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the display name to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiLightComponent::GetPrimitiveType(const NiFixedString& kPropertyName,
    NiFixedString& kPrimitiveType) const
{
    if (kPropertyName == ms_kLightTypeName)
    {
        kPrimitiveType = PT_STRING;
    }
    else if (kPropertyName == ms_kDimmerName)
    {
        kPrimitiveType = PT_FLOAT;
    }
    else if (kPropertyName == ms_kAmbientColorName)
    {
        kPrimitiveType = PT_COLOR;
    }
    else if (kPropertyName == ms_kDiffuseColorName)
    {
        kPrimitiveType = PT_COLOR;
    }
    else if (kPropertyName == ms_kSpecularColorName)
    {
        kPrimitiveType = PT_COLOR;
    }
    else if (kPropertyName == ms_kConstantAttenuationName)
    {
        kPrimitiveType = PT_FLOAT;
    }
    else if (kPropertyName == ms_kLinearAttenuationName)
    {
        kPrimitiveType = PT_FLOAT;
    }
    else if (kPropertyName == ms_kQuadraticAttenuationName)
    {
        kPrimitiveType = PT_FLOAT;
    }
    else if (kPropertyName == ms_kOuterSpotAngleName)
    {
        kPrimitiveType = PT_FLOAT;
    }
    else if (kPropertyName == ms_kInnerSpotAngleName)
    {
        kPrimitiveType = PT_FLOAT;
    }
    else if (kPropertyName == ms_kSpotExponentName)
    {
        kPrimitiveType = PT_FLOAT;
    }
    else if (kPropertyName == ms_kAffectedEntitiesName)
    {
        kPrimitiveType = PT_ENTITYPOINTER;
    }
    else if (kPropertyName == ms_kWorldDirName)
    {
        kPrimitiveType = PT_POINT3;
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
NiBool NiLightComponent::SetPrimitiveType(const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the primitive type to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiLightComponent::GetSemanticType(const NiFixedString& kPropertyName,
    NiFixedString& kSemanticType) const
{
    if (kPropertyName == ms_kLightTypeName)
    {
        kSemanticType = "Light Type";
    }
    else if (kPropertyName == ms_kDimmerName)
    {
        kSemanticType = PT_FLOAT;
    }
    else if (kPropertyName == ms_kAmbientColorName)
    {
        kSemanticType = PT_COLOR;
    }
    else if (kPropertyName == ms_kDiffuseColorName)
    {
        kSemanticType = PT_COLOR;
    }
    else if (kPropertyName == ms_kSpecularColorName)
    {
        kSemanticType = PT_COLOR;
    }
    else if (kPropertyName == ms_kConstantAttenuationName)
    {
        kSemanticType = PT_FLOAT;
    }
    else if (kPropertyName == ms_kLinearAttenuationName)
    {
        kSemanticType = PT_FLOAT;
    }
    else if (kPropertyName == ms_kQuadraticAttenuationName)
    {
        kSemanticType = PT_FLOAT;
    }
    else if (kPropertyName == ms_kOuterSpotAngleName)
    {
        kSemanticType = PT_FLOAT;
    }
    else if (kPropertyName == ms_kInnerSpotAngleName)
    {
        kSemanticType = PT_FLOAT;
    }
    else if (kPropertyName == ms_kSpotExponentName)
    {
        kSemanticType = PT_FLOAT;
    }
    else if (kPropertyName == ms_kAffectedEntitiesName)
    {
        kSemanticType = PT_ENTITYPOINTER;
    }
    else if (kPropertyName == ms_kWorldDirName)
    {
        kSemanticType = PT_POINT3;
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
NiBool NiLightComponent::SetSemanticType(const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the semantic type to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiLightComponent::GetDescription(const NiFixedString& kPropertyName,
    NiFixedString& kDescription) const
{
    if (kPropertyName == ms_kLightTypeName)
    {
        kDescription = ms_kLightTypeDescription;
    }
    else if (kPropertyName == ms_kDimmerName)
    {
        kDescription = ms_kDimmerDescription;
    }
    else if (kPropertyName == ms_kAmbientColorName)
    {
        kDescription = ms_kAmbientColorDescription;
    }
    else if (kPropertyName == ms_kDiffuseColorName)
    {
        kDescription = ms_kDiffuseColorDescription;
    }
    else if (kPropertyName == ms_kSpecularColorName)
    {
        kDescription = ms_kSpecularColorDescription;
    }
    else if (kPropertyName == ms_kConstantAttenuationName)
    {
        kDescription = ms_kConstantAttenuationDescription;
    }
    else if (kPropertyName == ms_kLinearAttenuationName)
    {
        kDescription = ms_kLinearAttenuationDescription;
    }
    else if (kPropertyName == ms_kQuadraticAttenuationName)
    {
        kDescription = ms_kQuadraticAttenuationDescription;
    }
    else if (kPropertyName == ms_kOuterSpotAngleName)
    {
        kDescription = ms_kOuterSpotAngleDescription;
    }
    else if (kPropertyName == ms_kInnerSpotAngleName)
    {
        kDescription = ms_kInnerSpotAngleDescription;
    }
    else if (kPropertyName == ms_kSpotExponentName)
    {
        kDescription = ms_kSpotExponentDescription;
    }
    else if (kPropertyName == ms_kAffectedEntitiesName)
    {
        kDescription = ms_kAffectedEntitiesDescription;
    }
    else if (kPropertyName == ms_kWorldDirName)
    {
        kDescription = ms_kWorldDirDescription;
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
NiBool NiLightComponent::SetDescription(const NiFixedString&,
    const NiFixedString&)
{
    // This component does not allow the description to be set.
    return false;
}
//---------------------------------------------------------------------------
NiBool NiLightComponent::GetCategory(const NiFixedString& kPropertyName,
    NiFixedString& kCategory) const
{
    if (kPropertyName == ms_kLightTypeName ||
        kPropertyName == ms_kDimmerName ||
        kPropertyName == ms_kAmbientColorName ||
        kPropertyName == ms_kDiffuseColorName ||
        kPropertyName == ms_kSpecularColorName ||
        kPropertyName == ms_kConstantAttenuationName ||
        kPropertyName == ms_kLinearAttenuationName ||
        kPropertyName == ms_kQuadraticAttenuationName ||
        kPropertyName == ms_kOuterSpotAngleName ||
        kPropertyName == ms_kInnerSpotAngleName ||
        kPropertyName == ms_kSpotExponentName ||
        kPropertyName == ms_kAffectedEntitiesName ||
        kPropertyName == ms_kWorldDirName ||
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
NiBool NiLightComponent::IsPropertyReadOnly(const NiFixedString& kPropertyName,
    bool& bIsReadOnly)
{
    if (kPropertyName == ms_kLightTypeName ||
        kPropertyName == ms_kDimmerName ||
        kPropertyName == ms_kAmbientColorName ||
        kPropertyName == ms_kDiffuseColorName ||
        kPropertyName == ms_kSpecularColorName ||
        kPropertyName == ms_kConstantAttenuationName ||
        kPropertyName == ms_kLinearAttenuationName ||
        kPropertyName == ms_kQuadraticAttenuationName ||
        kPropertyName == ms_kOuterSpotAngleName ||
        kPropertyName == ms_kInnerSpotAngleName ||
        kPropertyName == ms_kSpotExponentName ||
        kPropertyName == ms_kAffectedEntitiesName)
    {
        bIsReadOnly = false;
    }
    else if (kPropertyName == ms_kWorldDirName ||
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
NiBool NiLightComponent::IsPropertyUnique(
    const NiFixedString& kPropertyName, bool& bIsUnique)
{
    if (kPropertyName == ms_kLightTypeName ||
        kPropertyName == ms_kDimmerName ||
        kPropertyName == ms_kAmbientColorName ||
        kPropertyName == ms_kDiffuseColorName ||
        kPropertyName == ms_kSpecularColorName ||
        kPropertyName == ms_kConstantAttenuationName ||
        kPropertyName == ms_kLinearAttenuationName ||
        kPropertyName == ms_kQuadraticAttenuationName ||
        kPropertyName == ms_kOuterSpotAngleName ||
        kPropertyName == ms_kInnerSpotAngleName ||
        kPropertyName == ms_kSpotExponentName ||
        kPropertyName == ms_kAffectedEntitiesName ||
        kPropertyName == ms_kWorldDirName ||
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
NiBool NiLightComponent::IsPropertySerializable(
    const NiFixedString& kPropertyName, bool& bIsSerializable)
{
    if (kPropertyName == ms_kLightTypeName ||
        kPropertyName == ms_kDimmerName ||
        kPropertyName == ms_kAmbientColorName ||
        kPropertyName == ms_kDiffuseColorName ||
        kPropertyName == ms_kSpecularColorName ||
        kPropertyName == ms_kConstantAttenuationName ||
        kPropertyName == ms_kLinearAttenuationName ||
        kPropertyName == ms_kQuadraticAttenuationName ||
        kPropertyName == ms_kOuterSpotAngleName ||
        kPropertyName == ms_kInnerSpotAngleName ||
        kPropertyName == ms_kSpotExponentName ||
        kPropertyName == ms_kAffectedEntitiesName ||
        kPropertyName == ms_kWorldDirName)
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
NiBool NiLightComponent::IsPropertyInheritable(
    const NiFixedString& kPropertyName, bool& bIsInheritable)
{
    if (kPropertyName == ms_kLightTypeName ||
        kPropertyName == ms_kDimmerName ||
        kPropertyName == ms_kAmbientColorName ||
        kPropertyName == ms_kDiffuseColorName ||
        kPropertyName == ms_kSpecularColorName ||
        kPropertyName == ms_kConstantAttenuationName ||
        kPropertyName == ms_kLinearAttenuationName ||
        kPropertyName == ms_kQuadraticAttenuationName ||
        kPropertyName == ms_kOuterSpotAngleName ||
        kPropertyName == ms_kInnerSpotAngleName ||
        kPropertyName == ms_kSpotExponentName ||
        kPropertyName == ms_kAffectedEntitiesName ||
        kPropertyName == ms_kWorldDirName ||
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
NiBool NiLightComponent::IsExternalAssetPath(
    const NiFixedString& kPropertyName, unsigned int,
    bool& bIsExternalAssetPath) const
{
    if (kPropertyName == ms_kLightTypeName ||
        kPropertyName == ms_kDimmerName ||
        kPropertyName == ms_kAmbientColorName ||
        kPropertyName == ms_kDiffuseColorName ||
        kPropertyName == ms_kSpecularColorName ||
        kPropertyName == ms_kConstantAttenuationName ||
        kPropertyName == ms_kLinearAttenuationName ||
        kPropertyName == ms_kQuadraticAttenuationName ||
        kPropertyName == ms_kOuterSpotAngleName ||
        kPropertyName == ms_kInnerSpotAngleName ||
        kPropertyName == ms_kSpotExponentName ||
        kPropertyName == ms_kAffectedEntitiesName ||
        kPropertyName == ms_kWorldDirName ||
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
NiBool NiLightComponent::GetElementCount(const NiFixedString& kPropertyName,
    unsigned int& uiCount) const
{
    if (kPropertyName == ms_kLightTypeName ||
        kPropertyName == ms_kDimmerName ||
        kPropertyName == ms_kAmbientColorName ||
        kPropertyName == ms_kDiffuseColorName ||
        kPropertyName == ms_kSpecularColorName ||
        kPropertyName == ms_kConstantAttenuationName ||
        kPropertyName == ms_kLinearAttenuationName ||
        kPropertyName == ms_kQuadraticAttenuationName ||
        kPropertyName == ms_kOuterSpotAngleName ||
        kPropertyName == ms_kInnerSpotAngleName ||
        kPropertyName == ms_kSpotExponentName ||
        kPropertyName == ms_kWorldDirName ||
        kPropertyName == ms_kSceneRootPointerName)
    {
        uiCount = 1;
    }
    else if (kPropertyName == ms_kAffectedEntitiesName)
    {
        uiCount = m_kAffectedEntities.GetSize();
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiLightComponent::SetElementCount(const NiFixedString& kPropertyName,
    unsigned int uiCount, bool& bCountSet)
{
    if (kPropertyName == ms_kLightTypeName ||
        kPropertyName == ms_kDimmerName ||
        kPropertyName == ms_kAmbientColorName ||
        kPropertyName == ms_kDiffuseColorName ||
        kPropertyName == ms_kSpecularColorName ||
        kPropertyName == ms_kConstantAttenuationName ||
        kPropertyName == ms_kLinearAttenuationName ||
        kPropertyName == ms_kQuadraticAttenuationName ||
        kPropertyName == ms_kOuterSpotAngleName ||
        kPropertyName == ms_kInnerSpotAngleName ||
        kPropertyName == ms_kSpotExponentName ||
        kPropertyName == ms_kWorldDirName ||
        kPropertyName == ms_kSceneRootPointerName)
    {
        bCountSet = false;
    }
    else if (kPropertyName == ms_kAffectedEntitiesName)
    {
        const unsigned int uiOrigCount = m_kAffectedEntities.GetSize();
        for (unsigned int ui = uiCount; ui < uiOrigCount; ui++)
        {
            SetPropertyData(kPropertyName, (NiEntityInterface*) NULL, ui);
        }
        m_kAffectedEntities.SetSize(uiCount);
        bCountSet = true;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiLightComponent::IsCollection(const NiFixedString& kPropertyName,
    bool& bIsCollection) const
{
    if (kPropertyName == ms_kLightTypeName ||
        kPropertyName == ms_kDimmerName ||
        kPropertyName == ms_kAmbientColorName ||
        kPropertyName == ms_kDiffuseColorName ||
        kPropertyName == ms_kSpecularColorName ||
        kPropertyName == ms_kConstantAttenuationName ||
        kPropertyName == ms_kLinearAttenuationName ||
        kPropertyName == ms_kQuadraticAttenuationName ||
        kPropertyName == ms_kOuterSpotAngleName ||
        kPropertyName == ms_kInnerSpotAngleName ||
        kPropertyName == ms_kSpotExponentName ||
        kPropertyName == ms_kWorldDirName ||
        kPropertyName == ms_kSceneRootPointerName)
    {
        bIsCollection = false;
    }
    else if (kPropertyName == ms_kAffectedEntitiesName)
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
NiBool NiLightComponent::GetPropertyData(const NiFixedString& kPropertyName,
    float& fData, unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kDimmerName)
    {
        fData = GetDimmer();
    }
    else if (kPropertyName == ms_kConstantAttenuationName)
    {
        fData = GetConstantAttenuation();
    }
    else if (kPropertyName == ms_kLinearAttenuationName)
    {
        fData = GetLinearAttenuation();
    }
    else if (kPropertyName == ms_kQuadraticAttenuationName)
    {
        fData = GetQuadraticAttenuation();
    }
    else if (kPropertyName == ms_kOuterSpotAngleName)
    {
        fData = GetOuterSpotAngle();
    }
    else if (kPropertyName == ms_kInnerSpotAngleName)
    {
        fData = GetInnerSpotAngle();
    }
    else if (kPropertyName == ms_kSpotExponentName)
    {
        fData = GetSpotExponent();
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiLightComponent::SetPropertyData(const NiFixedString& kPropertyName,
    float fData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kDimmerName)
    {
        SetDimmer(fData);
    }
    else if (kPropertyName == ms_kConstantAttenuationName)
    {
        SetConstantAttenuation(fData);
    }
    else if (kPropertyName == ms_kLinearAttenuationName)
    {
        SetLinearAttenuation(fData);
    }
    else if (kPropertyName == ms_kQuadraticAttenuationName)
    {
        SetQuadraticAttenuation(fData);
    }
    else if (kPropertyName == ms_kOuterSpotAngleName)
    {
        SetOuterSpotAngle(fData);
    }
    else if (kPropertyName == ms_kInnerSpotAngleName)
    {
        SetInnerSpotAngle(fData);
    }
    else if (kPropertyName == ms_kSpotExponentName)
    {
        SetSpotExponent(fData);
    }
    else if (kPropertyName == ms_kSpotAngleName)
    {
        // Backwards compatibility.
        SetOuterSpotAngle(fData);
        SetInnerSpotAngle(0.0f);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiLightComponent::GetPropertyData(const NiFixedString& kPropertyName,
    NiFixedString& kData, unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kLightTypeName)
    {
        kData = GetLightType();
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiLightComponent::SetPropertyData(const NiFixedString& kPropertyName,
    const NiFixedString& kData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kLightTypeName)
    {
        if (!IsLightTypeValid(kData))
        {
            return false;
        }
        SetLightType(kData);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiLightComponent::GetPropertyData(const NiFixedString& kPropertyName,
    NiColor& kData, unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kAmbientColorName)
    {
        kData = GetAmbientColor();
    }
    else if (kPropertyName == ms_kDiffuseColorName)
    {
        kData = GetDiffuseColor();
    }
    else if (kPropertyName == ms_kSpecularColorName)
    {
        kData = GetSpecularColor();
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiLightComponent::SetPropertyData(const NiFixedString& kPropertyName,
    const NiColor& kData, unsigned int uiIndex)
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kAmbientColorName)
    {
        SetAmbientColor(kData);
    }
    else if (kPropertyName == ms_kDiffuseColorName)
    {
        SetDiffuseColor(kData);
    }
    else if (kPropertyName == ms_kSpecularColorName)
    {
        SetSpecularColor(kData);
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiLightComponent::GetPropertyData(const NiFixedString& kPropertyName,
    NiPoint3& kData, unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kWorldDirName)
    {
        if (m_spLight)
        {
            m_spLight->GetWorldRotate().GetCol(0, kData);
        }
        else
        {
            kData = NiPoint3::ZERO;
        }
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiLightComponent::GetPropertyData(const NiFixedString& kPropertyName,
    NiObject*& pkData, unsigned int uiIndex) const
{
    if (uiIndex != 0)
    {
        return false;
    }
    else if (kPropertyName == ms_kSceneRootPointerName)
    {
        pkData = m_spLight;
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiBool NiLightComponent::GetPropertyData(const NiFixedString& kPropertyName,
    NiEntityInterface*& pkData, unsigned int uiIndex) const
{
    if (kPropertyName == ms_kAffectedEntitiesName)
    {
        if (uiIndex < m_kAffectedEntities.GetSize())
        {
            pkData = m_kAffectedEntities.GetAt(uiIndex);
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
NiBool NiLightComponent::SetPropertyData(const NiFixedString& kPropertyName,
    NiEntityInterface* pkData, unsigned int uiIndex)
{
    if (kPropertyName == ms_kAffectedEntitiesName)
    {
        if (m_spLight && !pkData && uiIndex < m_kAffectedEntities.GetSize())
        {
            NiEntityInterface* pkOldEntity = m_kAffectedEntities.GetAt(
                uiIndex);
            if (pkOldEntity)
            {
                // Detach affected node for the light and update the effects
                // on the old entity. This is so that the light no longer
                // affects the old entity without the need to update the
                // component. The added affected entity will not be affected
                // until the next update.
                unsigned int uiCount;
                if (pkOldEntity->GetElementCount(ms_kSceneRootPointerName,
                    uiCount))
                {
                    for (unsigned int ui = 0; ui < uiCount; ui++)
                    {
                        NiObject* pkSceneRoot;
                        NIVERIFY(pkOldEntity->GetPropertyData(
                            ms_kSceneRootPointerName, pkSceneRoot, ui));

                        NiNode* pkNode = NiDynamicCast(NiNode, pkSceneRoot);
                        if (pkNode)
                        {
                            m_spLight->DetachAffectedNode(pkNode);
                            pkNode->UpdateEffects();
                        }
                    }
                }
            }
        }

        m_kAffectedEntities.SetAtGrow(uiIndex, pkData);
        m_kAffectedEntities.UpdateSize();
        if (pkData)
        {
            SetAffectedEntitiesChanged(true);
        }
    }
    else
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
