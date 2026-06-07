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

#pragma once

#include <NiRefObject.h>
#include "NiEntityComponentInterface.h"
#include <NiFlags.h>

class NiGridComponent : public NiRefObject,
    public NiEntityComponentInterface
{
    NiDeclareFlags(unsigned char);

public:
    NiGridComponent();
    ~NiGridComponent();

    void SetDisplayGrid(bool bDisplayGrid);
    void SetGridSpacing(float fSpacing);
    void SetMajorLineSpacing(int iSpacing);
    void SetGridExtent(int iExtent);
    void SetGridColor(const NiColor& kColor);
    void SetMajorColor(const NiColor& kColor);
    void SetStatic(bool bStatic);
    void RebuildInvalidGrid();

    // we have to initialize and clear the NiFixedStrings within Gamebryo
    static void _SDMInit();
    static void _SDMShutdown();

protected:
    NiGridComponent(float fGridSpacing, int iMajorLineSpacing,
        int iGridExtent, const NiColor& kGridColor,
        const NiColor& kMajorColor, bool bStatic);

    NiMeshPtr m_spGrid;
    NiMeshPtr m_spMajorLines;
    bool m_bDisplayGrid;
    bool m_bValidGrid;
    bool m_bStaticGrid;
    float m_fMinDensity;
    float m_fMaxDensity;

    float m_fGridSpacing;
    int m_iMajorLines;
    int m_iGridExtent;
    NiColor m_kGridColor;
    NiColor m_kMajorColor;

    float GetTrueSpacing(NiCamera* pkCamera, float fSpacing);
    void ConstructStaticGrid(float fSpacing);
    void ConstructDynamicGrid(float fSpacing);
    void AlignGrid(NiCamera* pkCamera);

    // Properties.
    static NiFixedString ms_kMajorLinesString;
    static NiFixedString ms_kGridExtentString;
    static NiFixedString ms_kGridColorString;
    static NiFixedString ms_kMajorColorString;
    static NiFixedString ms_kStaticString;
    static NiFixedString ms_kSceneRootString;

    // Component name.
    static NiFixedString ms_kClassName;

    // Component name.
    static NiFixedString ms_kComponentName;

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
        bool& bData, unsigned int uiIndex = 0) const;
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        int& iData, unsigned int uiIndex) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        int iData, unsigned int uiIndex);
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiColor& kData, unsigned int uiIndex) const;
    virtual NiBool SetPropertyData(const NiFixedString& kPropertyName,
        const NiColor& kData, unsigned int uiIndex);
    virtual NiBool GetPropertyData(const NiFixedString& kPropertyName,
        NiObject*& pkData, unsigned int uiIndex = 0) const;
};

NiSmartPointer(NiGridComponent);

#include "NiGridComponent.inl"

