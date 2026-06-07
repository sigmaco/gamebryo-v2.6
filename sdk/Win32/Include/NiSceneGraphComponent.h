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

#ifndef NISCENEGRAPHCOMPONENT_H
#define NISCENEGRAPHCOMPONENT_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include "NiEntityComponentInterface.h"
#include <NiFlags.h>
#include <NiAVObject.h>

NiSmartPointer(NiSceneGraphComponent);

class NIENTITY_ENTRY NiSceneGraphComponent : public NiRefObject,
    public NiEntityComponentInterface
{
    NiDeclareFlags(unsigned char);

public:
    // Error strings.
    static NiFixedString ERR_TRANSLATION_NOT_FOUND;
    static NiFixedString ERR_ROTATION_NOT_FOUND;
    static NiFixedString ERR_SCALE_NOT_FOUND;
    static NiFixedString ERR_FILE_LOAD_FAILED;
    static NiFixedString ERR_NIF_ROOT_OBJECT;

    NiSceneGraphComponent();
    NiSceneGraphComponent(const NiFixedString& kNifFilePath);

    inline const NiFixedString& GetNifFilePath() const;
    inline void SetNifFilePath(const NiFixedString& kNifFilePath);

    inline NiAVObject* GetSceneRoot() const;

    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    // *** end Emergent internal use only ***

protected:
    NiSceneGraphComponent(NiSceneGraphComponent* pkMasterComponent);

    inline NiBool ShouldReloadScene() const;
    NiBool RecursiveFindAnimations(NiAVObject* pkObject);

    // Flags.
    enum
    {
        LOAD_ERROR_HIT_MASK             = 0x01,
        NIF_FILE_PATH_CHANGED_MASK      = 0x02,
        SHOULD_UPDATE_SCENE_ROOT_MASK   = 0x04
    };
    inline NiBool GetLoadErrorHit() const;
    inline void SetLoadErrorHit(bool bLoadErrorHit);
    inline NiBool GetNifFilePathChanged() const;
    inline void SetNifFilePathChanged(bool bNifFilePathChanged);
    inline NiBool GetShouldUpdateSceneRoot() const;
    inline void SetShouldUpdateSceneRoot(bool bShouldUpdateSceneRoot);

    // Properties.
    NiFixedString m_kNifFilePath;

    NiAVObjectPtr m_spSceneRoot;
    NiSceneGraphComponentPtr m_spMasterComponent;

    // Class name.
    static NiFixedString ms_kClassName;

    // Component name.
    static NiFixedString ms_kComponentName;

    // Property names.
    static NiFixedString ms_kNifFilePathName;
    static NiFixedString ms_kSceneRootPointerName;

    // Property descriptions.
    static NiFixedString ms_kNifFilePathDescription;

    // Dependent property names.
    static NiFixedString ms_kTranslationName;
    static NiFixedString ms_kRotationName;
    static NiFixedString ms_kScaleName;

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
        NiFixedString& kData, unsigned int uiIndex) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiFixedString& kData, unsigned int uiIndex);
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiObject*& pkData, unsigned int uiIndex) const;
};

#include "NiSceneGraphComponent.inl"

#endif // NISCENEGRAPHCOMPONENT_H
