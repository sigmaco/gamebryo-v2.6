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

#ifndef NITRANSFORMATIONCOMPONENT_H
#define NITRANSFORMATIONCOMPONENT_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include <NiFlags.h>
#include "NiEntityComponentInterface.h"

NiSmartPointer(NiTransformationComponent);

class NIENTITY_ENTRY NiTransformationComponent : public NiRefObject,
    public NiEntityComponentInterface
{
    NiDeclareFlags(unsigned short);

public:
    // Property names.
    inline static const NiFixedString& PROP_TRANSLATION();
    inline static const NiFixedString& PROP_ROTATION();
    inline static const NiFixedString& PROP_SCALE();
    inline static const NiFixedString& PROP_SOURCE_ENTITY();
    inline static const NiFixedString& PROP_ATTACHMENT_POINT_NAME();
    inline static const NiFixedString& PROP_INHERIT_TRANSLATION();
    inline static const NiFixedString& PROP_INHERIT_ROTATION();
    inline static const NiFixedString& PROP_INHERIT_SCALE();
    inline static const NiFixedString& PROP_LOCAL_TRANSLATION();
    inline static const NiFixedString& PROP_LOCAL_ROTATION();
    inline static const NiFixedString& PROP_LOCAL_SCALE();

    // Error strings
    inline static const NiFixedString& ERR_ATTACHMENT_POINT_NOT_FOUND();

    NiTransformationComponent();
    NiTransformationComponent(
        NiEntityInterface* pkSourceEntity,
        NiFixedString kAttachmentPointName = NULL);
    NiTransformationComponent(
        const NiPoint3& kTranslation,
        const NiMatrix3& kRotation,
        float fScale,
        NiEntityInterface* pkSourceEntity = NULL,
        NiFixedString kAttachmentPointName = NULL,
        bool bInheritTranslation = true,
        bool bInheritRotation = true,
        bool bInheritScale = true);

    inline NiEntityInterface* GetSourceEntity() const;
    inline void SetSourceEntity(NiEntityInterface* pkSourceEntity);

    inline const NiFixedString& GetAttachmentPointName() const;
    inline void SetAttachmentPointName(
        const NiFixedString& kAttachmentPointName);

    inline NiBool GetInheritTranslation() const;
    inline void SetInheritTranslation(bool bInheritTranslation);

    inline NiBool GetInheritRotation() const;
    inline void SetInheritRotation(bool bInheritRotation);

    inline NiBool GetInheritScale() const;
    inline void SetInheritScale(bool bInheritScale);

    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    // *** end Emergent internal use only ***

protected:
    NiTransformationComponent(NiTransformationComponent* pkMasterComponent);
    inline void ClearAttachmentPoint();
    inline NiBool ShouldResolveAttachmentPoint();

    inline void GetTranslation(NiPoint3& kTranslation) const;
    inline void SetTranslation(const NiPoint3& kTranslation);

    inline void GetRotation(NiMatrix3& kRotation) const;
    inline void SetRotation(const NiMatrix3& kRotation);

    inline float GetScale() const;
    inline void SetScale(float fScale);

    NiBool GetSourceTranslation(NiPoint3& kSourceTranslation) const;
    NiBool GetSourceRotation(NiMatrix3& kSourceRotation) const;
    NiBool GetSourceScale(float& fSourceScale) const;

    // Flags.
    enum
    {
        INHERIT_TRANSLATION_MASK                = 0x0001,
        INHERIT_ROTATION_MASK                   = 0x0002,
        INHERIT_SCALE_MASK                      = 0x0004,
        ERROR_RESOLVING_ATTACHMENT_POINT_MASK   = 0x0008,
        TRANSLATION_UNIQUE_MASK                 = 0x0010,
        ROTATION_UNIQUE_MASK                    = 0x0020,
        SCALE_UNIQUE_MASK                       = 0x0040,
        ATTACHMENT_POINT_NAME_UNIQUE_MASK       = 0x0100,
        INHERIT_TRANSLATION_UNIQUE_MASK         = 0x0200,
        INHERIT_ROTATION_UNIQUE_MASK            = 0x0400,
        INHERIT_SCALE_UNIQUE_MASK               = 0x0800

    };
    inline NiBool GetErrorResolvingAttachmentPoint() const;
    inline void SetErrorResolvingAttachmentPoint(
        bool bErrorResolvingAttachmentPoint);
    inline NiBool GetTranslationUnique() const;
    inline void SetTranslationUnique(bool bTranslationUnique);
    inline NiBool GetRotationUnique() const;
    inline void SetRotationUnique(bool bRotationUnique);
    inline NiBool GetScaleUnique() const;
    inline void SetScaleUnique(bool bScaleUnique);
    inline NiBool GetAttachmentPointNameUnique() const;
    inline void SetAttachmentPointNameUnique(bool bAttachementPointNameUnique);
    inline NiBool GetInheritTranslationUnique() const;
    inline void SetInheritTranslationUnique(bool bInheritTranslationUnique);
    inline NiBool GetInheritRotationUnique() const;
    inline void SetInheritRotationUnique(bool bInheritRotationUnique);
    inline NiBool GetInheritScaleUnique() const;
    inline void SetInheritScaleUnique(bool bInheritScaleUnique);

    // Properties.
    NiPoint3 m_kTranslation;
    NiMatrix3 m_kRotation;
    float m_fScale;
    NiEntityInterface* m_pkSourceEntity;
    NiFixedString m_kAttachmentPointName;

    NiTransformationComponentPtr m_spMasterComponent;
    NiAVObject* m_pkAttachmentPoint;
    mutable bool m_bRetrievingTranslation;
    mutable bool m_bRetrievingRotation;
    mutable bool m_bRetrievingScale;

    // Class name.
    static NiFixedString ms_kClassName;

    // Component name.
    static NiFixedString ms_kComponentName;

    // Property names.
    static NiFixedString ms_kPropTranslation;
    static NiFixedString ms_kPropRotation;
    static NiFixedString ms_kPropScale;
    static NiFixedString ms_kPropSourceEntity;
    static NiFixedString ms_kPropAttachmentPointName;
    static NiFixedString ms_kPropInheritTranslation;
    static NiFixedString ms_kPropInheritRotation;
    static NiFixedString ms_kPropInheritScale;
    static NiFixedString ms_kPropLocalTranslation;
    static NiFixedString ms_kPropLocalRotation;
    static NiFixedString ms_kPropLocalScale;

    // Error strings.
    static NiFixedString ms_kDispTranslation;
    static NiFixedString ms_kDispRotation;
    static NiFixedString ms_kDispScale;

    // Property descriptions.
    static NiFixedString ms_kDescTranslation;
    static NiFixedString ms_kDescRotation;
    static NiFixedString ms_kDescScale;
    static NiFixedString ms_kDescSourceEntity;
    static NiFixedString ms_kDescAttachmentPointName;
    static NiFixedString ms_kDescInheritTranslation;
    static NiFixedString ms_kDescInheritRotation;
    static NiFixedString ms_kDescInheritScale;
    static NiFixedString ms_kDescLocalTranslation;
    static NiFixedString ms_kDescLocalRotation;
    static NiFixedString ms_kDescLocalScale;

    // Error strings.
    static NiFixedString ms_kErrAttachmentPointNotFound;

    // Dependent property names.
    static NiFixedString ms_kDepSceneRootPointer;

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
    virtual NiBool IsAnimated() const;
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
        bool& bData, unsigned int uiIndex) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        bool bData, unsigned int uiIndex);
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiPoint3& kData, unsigned int uiIndex) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiPoint3& kData, unsigned int uiIndex);
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiMatrix3& kData, unsigned int uiIndex) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiMatrix3& kData, unsigned int uiIndex);
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiFixedString& kData, unsigned int uiIndex) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiFixedString& kData, unsigned int uiIndex);
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiEntityInterface*& pkData, unsigned int uiIndex) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        NiEntityInterface* pkData, unsigned int uiIndex);
};

#include "NiTransformationComponent.inl"

#endif  // #ifndef NITRANSFORMATIONCOMPONENT_H
