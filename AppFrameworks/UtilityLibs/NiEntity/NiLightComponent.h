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

#ifndef NILIGHTCOMPONENT_H
#define NILIGHTCOMPONENT_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include "NiEntityComponentInterface.h"
#include <NiFlags.h>
#include <NiLight.h>
#include <NiTArray.h>
#include <NiNode.h>
#include "NiEntityInterface.h"

class NIENTITY_ENTRY NiLightComponent : public NiRefObject,
    public NiEntityComponentInterface
{
    NiDeclareFlags(unsigned char);

public:
    // Error strings.
    static NiFixedString ERR_TRANSLATION_NOT_FOUND;
    static NiFixedString ERR_ROTATION_NOT_FOUND;
    static NiFixedString ERR_SCALE_NOT_FOUND;
    static NiFixedString ERR_LIGHT_TYPE_NOT_VALID;

    // Light types.
    static NiFixedString LT_AMBIENT;
    static NiFixedString LT_DIRECTIONAL;
    static NiFixedString LT_POINT;
    static NiFixedString LT_SPOT;

    NiLightComponent(unsigned int uiArraySize = 0,
        unsigned int uiArrayGrowBy = 8);
    NiLightComponent(const NiFixedString& kLightType, float fDimmer,
        const NiColor& kAmbientColor, const NiColor& kDiffuseColor,
        const NiColor& kSpecularColor, float fConstantAttenuation,
        float fLinearAttenuation, float fQuadraticAttenuation,
        float fOuterSpotAngle, float fInnerSpotAngle, float fSpotExponent,
        unsigned int uiAffectedEntitiesArraySize = 0,
        unsigned int uiAffectedEntitiesArrayGrowBy = 8);

    inline const NiFixedString& GetLightType() const;
    inline void SetLightType(const NiFixedString& kLightType);

    inline float GetDimmer() const;
    inline void SetDimmer(float fDimmer);

    inline const NiColor& GetAmbientColor() const;
    inline void SetAmbientColor(const NiColor& kAmbientColor);

    inline const NiColor& GetDiffuseColor() const;
    inline void SetDiffuseColor(const NiColor& kDiffuseColor);

    inline const NiColor& GetSpecularColor() const;
    inline void SetSpecularColor(const NiColor& kSpecularColor);

    inline float GetConstantAttenuation() const;
    inline void SetConstantAttenuation(float fConstantAttenuation);

    inline float GetLinearAttenuation() const;
    inline void SetLinearAttenuation(float fLinearAttenuation);

    inline float GetQuadraticAttenuation() const;
    inline void SetQuadraticAttenuation(float fQuadraticAttenuation);

    inline float GetOuterSpotAngle() const;
    inline void SetOuterSpotAngle(float fOuterSpotAngle);

    inline float GetInnerSpotAngle() const;
    inline void SetInnerSpotAngle(float fInnerSpotAngle);

    inline float GetSpotExponent() const;
    inline void SetSpotExponent(float fSpotExponent);

    inline unsigned int GetAffectedEntitiesCount() const;
    inline NiEntityInterface* GetAffectedEntityAt(unsigned int uiIndex) const;
    inline void AddAffectedEntity(NiEntityInterface* pkEntity);
    inline void RemoveAffectedEntity(NiEntityInterface* pkEntity);
    inline void RemoveAffectedEntityAt(unsigned int uiIndex);
    inline void RemoveAllAffectedEntities();

    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    // *** end Emergent internal use only ***

protected:
    inline NiBool IsLightTypeValid(const NiFixedString& kLightType) const;
    inline void DeleteLight();

    // Flags.
    enum
    {
        LIGHT_CREATION_ERROR_HIT_MASK   = 0x01,
        LIGHT_PROPERTIES_CHANGED_MASK   = 0x02,
        AFFECTED_ENTITIES_CHANGED_MASK  = 0x04
    };
    inline NiBool GetLightCreationErrorHit() const;
    inline void SetLightCreationErrorHit(bool bLightCreationErrorHit);
    inline NiBool GetLightPropertiesChanged() const;
    inline void SetLightPropertiesChanged(bool bLightPropertiesChanged);
    inline NiBool GetAffectedEntitiesChanged() const;
    inline void SetAffectedEntitiesChanged(bool bAffectedEntitiesChanged);

    // Properties.
    NiFixedString m_kLightType;
    float m_fDimmer;
    NiColor m_kAmbientColor;
    NiColor m_kDiffuseColor;
    NiColor m_kSpecularColor;
    float m_fConstantAttenuation;
    float m_fLinearAttenuation;
    float m_fQuadraticAttenuation;
    float m_fOuterSpotAngle;
    float m_fInnerSpotAngle;
    float m_fSpotExponent;
    NiTPrimitiveArray<NiEntityInterface*> m_kAffectedEntities;

    NiLightPtr m_spLight;

    // Class name.
    static NiFixedString ms_kClassName;

    // Component name.
    static NiFixedString ms_kComponentName;

    // Property names.
    static NiFixedString ms_kLightTypeName;
    static NiFixedString ms_kDimmerName;
    static NiFixedString ms_kAmbientColorName;
    static NiFixedString ms_kDiffuseColorName;
    static NiFixedString ms_kSpecularColorName;
    static NiFixedString ms_kConstantAttenuationName;
    static NiFixedString ms_kLinearAttenuationName;
    static NiFixedString ms_kQuadraticAttenuationName;
    static NiFixedString ms_kOuterSpotAngleName;
    static NiFixedString ms_kInnerSpotAngleName;
    static NiFixedString ms_kSpotExponentName;
    static NiFixedString ms_kAffectedEntitiesName;
    static NiFixedString ms_kWorldDirName;
    static NiFixedString ms_kSceneRootPointerName;

    // Property descriptions.
    static NiFixedString ms_kLightTypeDescription;
    static NiFixedString ms_kDimmerDescription;
    static NiFixedString ms_kAmbientColorDescription;
    static NiFixedString ms_kDiffuseColorDescription;
    static NiFixedString ms_kSpecularColorDescription;
    static NiFixedString ms_kConstantAttenuationDescription;
    static NiFixedString ms_kLinearAttenuationDescription;
    static NiFixedString ms_kQuadraticAttenuationDescription;
    static NiFixedString ms_kOuterSpotAngleDescription;
    static NiFixedString ms_kInnerSpotAngleDescription;
    static NiFixedString ms_kSpotExponentDescription;
    static NiFixedString ms_kAffectedEntitiesDescription;
    static NiFixedString ms_kWorldDirDescription;

    // Dependent property names.
    static NiFixedString ms_kTranslationName;
    static NiFixedString ms_kRotationName;
    static NiFixedString ms_kScaleName;

    // For backwards compatibility.
    static NiFixedString ms_kSpotAngleName;

public:
    // NiEntityComponentInterface overrides.
    virtual NiEntityComponentInterface* Clone(bool bInheritProperties);
    virtual NiEntityComponentInterface* GetMasterComponent() const;
    virtual void SetMasterComponent(
        NiEntityComponentInterface* pkMasterComponent);
    virtual void GetDependentPropertyNames(
        NiTObjectSet<NiFixedString>& kDependentPropertyNames);

    // NiEntityPropertyInterface overrides.
    virtual NiBool SetTemplateID(const NiUniqueID& kID);
    virtual NiUniqueID GetTemplateID();
    virtual void AddReference();
    virtual void RemoveReference();
    virtual NiFixedString GetClassName() const;
    virtual NiFixedString GetName() const;
    virtual NiBool SetName(const NiFixedString& kName);
    virtual void Update(NiEntityPropertyInterface* pkParentEntity,
        float fTime, NiEntityErrorInterface* pkErrors,
        NiExternalAssetManager* pkAssetManager);
    virtual void BuildVisibleSet(NiEntityRenderingContext* pkRenderingContext,
        NiEntityErrorInterface* pkErrors);
    virtual void GetPropertyNames(
        NiTObjectSet<NiFixedString>& kPropertyNames) const;
    virtual NiBool CanResetProperty(const NiFixedString& kPropertyName,
        bool& bCanReset) const;
    virtual NiBool ResetProperty(const NiFixedString& kPropertyName);
    virtual NiBool MakePropertyUnique(const NiFixedString& kPropertyName,
        bool& bMadeUnique);
    virtual NiBool GetDisplayName(const NiFixedString& kPropertyName,
        NiFixedString& kDisplayName) const;
    virtual NiBool SetDisplayName(const NiFixedString& kPropertyName,
        const NiFixedString& kDisplayName);
    virtual NiBool GetPrimitiveType(const NiFixedString& kPropertyName,
        NiFixedString& kPrimitiveType) const;
    virtual NiBool SetPrimitiveType(const NiFixedString& kPropertyName,
        const NiFixedString& kPrimitiveType);
    virtual NiBool GetSemanticType(const NiFixedString& kPropertyName,
        NiFixedString& kSemanticType) const;
    virtual NiBool SetSemanticType(const NiFixedString& kPropertyName,
        const NiFixedString& kSemanticType);
    virtual NiBool GetDescription(const NiFixedString& kPropertyName,
        NiFixedString& kDescription) const;
    virtual NiBool SetDescription(const NiFixedString& kPropertyName,
        const NiFixedString& kDescription);
    virtual NiBool GetCategory(const NiFixedString& kPropertyName,
        NiFixedString& kCategory) const;
    virtual NiBool IsPropertyReadOnly(const NiFixedString& kPropertyName,
        bool& bIsReadOnly);
    virtual NiBool IsPropertyUnique(const NiFixedString& kPropertyName,
        bool& bIsUnique);
    virtual NiBool IsPropertySerializable(const NiFixedString& kPropertyName,
        bool& bIsSerializable);
    virtual NiBool IsPropertyInheritable(const NiFixedString& kPropertyName,
        bool& bIsInheritable);
    virtual NiBool IsExternalAssetPath(const NiFixedString& kPropertyName,
        unsigned int uiIndex, bool& bIsExternalAssetPath) const;
    virtual NiBool GetElementCount(const NiFixedString& kPropertyName,
        unsigned int& uiCount) const;
    virtual NiBool SetElementCount(const NiFixedString& kPropertyName,
        unsigned int uiCount, bool& bCountSet);
    virtual NiBool IsCollection(const NiFixedString& kPropertyName,
        bool& bIsCollection) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        float& fData, unsigned int uiIndex) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        float fData, unsigned int uiIndex);
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiFixedString& kData, unsigned int uiIndex) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiFixedString& kData, unsigned int uiIndex);
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiColor& kData, unsigned int uiIndex) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiColor& kData, unsigned int uiIndex);
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiPoint3& kData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiObject*& pkData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiEntityInterface*& pkData, unsigned int uiIndex) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        NiEntityInterface* pkData, unsigned int uiIndex);
};

NiSmartPointer(NiLightComponent);

#include "NiLightComponent.inl"

#endif // NILIGHTCOMPONENT_H
