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

#ifndef NIGENERALCOMPONENT_H
#define NIGENERALCOMPONENT_H

#include <NiRefObject.h>
#include <NiSmartPointer.h>
#include "NiEntityComponentInterface.h"
#include <NiTPtrSet.h>
#include "NiGeneralComponentProperty.h"

NiSmartPointer(NiGeneralComponent);

class NIENTITY_ENTRY NiGeneralComponent : public NiRefObject,
    public NiEntityComponentInterface
{
public:
    NiGeneralComponent(
        const NiFixedString& kName,
        const NiUniqueID& kTemplateID,
        unsigned int uiPropertyArraySize = 0);

    // *** begin Emergent internal use only ***
    NiGeneralComponent();
    // *** end Emergent internal use only ***

protected:
    static const unsigned int INVALID_INDEX;
    
    // Class name.
    static NiFixedString ms_kClassName;

    NiGeneralComponent(
        const NiFixedString& kName,
        const NiUniqueID& kTemplateID, 
        unsigned int uiPropertyArraySize,
        NiGeneralComponent* pkMasterComponent);

    inline NiGeneralComponentProperty* FindPropertyByName(
        const NiFixedString& kName,
        bool bSearchMaster) const;
    inline unsigned int FindPropertyIndexByName(const NiFixedString& kName)
        const;

    NiFixedString m_kName;
    NiTObjectPtrSet<NiGeneralComponentPropertyPtr> m_kProperties;
    NiGeneralComponentPtr m_spMasterComponent;
    NiUniqueID m_kTemplateID;

public:
    // NiEntityComponentInterface overrides.
    virtual NiEntityComponentInterface* Clone(bool bInheritProperties);
    virtual NiEntityComponentInterface* GetMasterComponent() const;
    virtual void SetMasterComponent(
        NiEntityComponentInterface* pkMasterComponent);
    virtual void GetDependentPropertyNames(
        NiTObjectSet<NiFixedString>& kDependentPropertyNames);

    // NiEntityPropertyInterface overrides.
    virtual NiBool SetTemplateID(const NiUniqueID& kTemplateID);
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
    virtual NiBool IsAddPropertySupported();
    virtual NiBool AddProperty(const NiFixedString& kPropertyName,
        const NiFixedString& kDisplayName,
        const NiFixedString& kPrimitiveType,
        const NiFixedString& kSemanticType,
        const NiFixedString& kDescription);
    virtual NiBool MakeCollection(const NiFixedString& kPropertyName,
        bool bCollection);
    virtual NiBool RemoveProperty(const NiFixedString& kPropertyName);
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
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        bool& bData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        int& iData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        unsigned int& uiData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        short& sData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        unsigned short& usData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiFixedString& kData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiPoint2& kData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiPoint3& kData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiQuaternion& kData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiMatrix3& kData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiColor& kData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiColorA& kData, unsigned int uiIndex) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiObject*& pkData, unsigned int uiIndex = 0) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiEntityInterface*& pkData, unsigned int uiIndex = 0) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        void*& pvData, size_t& stDataSizeInBytes, unsigned int uiIndex) const;

    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        float fData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        bool bData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        int iData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        unsigned int uiData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        short sData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        unsigned short usData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiFixedString& kData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiPoint2& kData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiPoint3& kData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiQuaternion& kData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiMatrix3& kData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiColor& kData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiColorA& kData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        NiObject* pkData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        NiEntityInterface* pkData, unsigned int uiIndex);
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const void* pvData, size_t stDataSizeInBytes, unsigned int uiIndex);

    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    inline virtual ~NiGeneralComponent() {}
    // *** end Emergent internal use only ***
};

#include "NiGeneralComponent.inl"

#endif // NIGENERALCOMPONENT_H
