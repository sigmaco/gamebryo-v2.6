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

#ifndef NIGEOMETRY_H
#define NIGEOMETRY_H

#include "NiDynamicEffectState.h"
#include "NiGeometryData.h"
#include "NiMaterialInstance.h"
#include "NiProcessor.h"
#include "NiPropertyState.h"
#include "NiRenderer.h"
#include "NiShader.h"
#include "NiSkinInstance.h"
#include "NiTPtrSet.h"
#include "NiShaderDeclaration.h"

class NiRenderer;
class NiMaterial;

/**
    This class is deprecated.

    It only exists to support loading old NIF files. It has been replaced by
    NiRenderObject and NiMesh.
*/

class NIMAIN_ENTRY NiGeometry : public NiAVObject
{
    NiDeclareRTTI;
    NiDeclareAbstractClone(NiGeometry);
    NiDeclareAbstractStream;

public:
    // *** begin Emergent internal use only ***
    virtual ~NiGeometry();

    // vertices
    inline unsigned short GetVertexCount() const;
    inline NiPoint3* GetVertices() const;
    inline void SetActiveVertexCount(unsigned short usActive);
    inline unsigned short GetActiveVertexCount() const;

    // normals
    inline NiPoint3* GetNormals() const;
    inline NiShaderRequirementDesc::NBTFlags
        GetNormalBinormalTangentMethod() const;
    inline void StoreNormalBinormalTangent(
        NiShaderRequirementDesc::NBTFlags eNBTMethod);

    // bounds
    inline void SetModelBound(const NiBound& kBound);
    inline NiBound& GetModelBound() const;

    // color access
    inline void CreateColors(bool bInitialize);
    inline NiColorA* GetColors() const;

    // texture coordinate access
    inline void CreateTextures(bool bInitialize, unsigned short usSets);
    inline NiUInt16 GetTextureSets() const;
    inline NiPoint2* GetTextures() const;
    inline NiPoint2* GetTextureSet(unsigned short usSet) const;

    // skinning
    inline NiSkinInstance* GetSkinInstance() const;
    inline void SetSkinInstance(NiSkinInstance* pkSkinInstance);

    // State access
    inline NiPropertyStatePtr GetPropertyState() const;
    inline void SetPropertyState(NiPropertyState* pkState);
    inline NiDynamicEffectStatePtr GetEffectState() const;
    inline void SetEffectState(NiDynamicEffectState* pkState);

    // additional geometry data
    inline NiAdditionalGeometryData* GetAdditionalGeometryData();
    inline void SetAdditionalGeometryData(NiAdditionalGeometryData* pkData);

    inline void SetSortObject(bool bSort);
    inline bool GetSortObject() const;

    // NiMaterial system
    bool ApplyMaterial(const NiFixedString& kName,
        unsigned int uiMaterialExtraData = 
        NiMaterialInstance::DEFAULT_EXTRA_DATA);
    bool ApplyMaterial(NiMaterial* pkMaterial,
        unsigned int uiMaterialExtraData = 
        NiMaterialInstance::DEFAULT_EXTRA_DATA);
    bool ApplyAndSetActiveMaterial(const NiFixedString& kName,
        unsigned int uiMaterialExtraData = 
        NiMaterialInstance::DEFAULT_EXTRA_DATA);
    bool ApplyAndSetActiveMaterial(NiMaterial* pkMaterial,
        unsigned int uiMaterialExtraData = 
        NiMaterialInstance::DEFAULT_EXTRA_DATA);
    inline bool IsMaterialApplied(const NiFixedString& kName) const;
    bool IsMaterialApplied(const NiMaterial* pkMaterial) const;

    inline bool SetActiveMaterial(const NiFixedString& kName);
    inline bool SetActiveMaterial(const NiMaterial* pkMaterial);
    inline const NiMaterial* GetActiveMaterial() const;

    static bool GetStreamingAppliesDefaultMaterial();
    static void SetStreamingAppliesDefaultMaterial(bool bApplyDefault = true);

    // material updates
    virtual void SetMaterialNeedsUpdate(bool bNeedsUpdate);

    virtual void SetDefaultMaterialNeedsUpdateFlag(bool bFlag);
    inline bool GetDefaultMaterialNeedsUpdateFlag() const;

    // shared data access
    virtual void SetModelData(NiGeometryData* pkModelData);
    inline NiGeometryData* GetModelData() const;

    unsigned int GetMaterialCount();
    const NiMaterialInstance* GetMaterialInstance(unsigned int uiInstance);
    bool ComputeCachedShader(unsigned int uiMaterial);
    void ClearCachedShader(unsigned int uiMaterial);

    inline NiShaderDeclaration* GetShaderDecl();
    inline void SetShaderDecl(NiShaderDeclaration* pkDecl);

    // updates
    virtual void UpdatePropertiesDownward(NiPropertyState* pkParentState);
    virtual void UpdateEffectsDownward(NiDynamicEffectState* pkParentState);

    // consistency
    inline NiGeometryData::Consistency GetConsistency() const;
    inline void SetConsistency(NiGeometryData::Consistency eConsistency);

    // cloning
    virtual void ProcessClone(NiCloningProcess& kCloning);

    // post-link
    virtual void PostLinkObject(NiStream& kStream);

    // set selective update flags
    virtual void SetSelectiveUpdateFlags(bool& bSelectiveUpdate, 
        bool bSelectiveUpdateTransforms, bool& bRigid);

    // renderer data
    virtual void PurgeRendererData(NiRenderer* pkRenderer);
    // *** end Emergent internal use only ***

protected:
    // construction (always uses a shared data object)
    NiGeometry(NiGeometryData* pkModelData);

    // streaming support
    NiGeometry();

    // updates
    virtual void UpdateWorldBound();

    // property and effect state caching
    NiPropertyStatePtr m_spPropertyState;
    NiDynamicEffectStatePtr m_spEffectState;

    // model data
    NiGeometryDataPtr m_spModelData;
    NiSkinInstancePtr m_spSkinInstance;

    // Materials
    NiTObjectSet<NiMaterialInstance> m_kMaterials;
    unsigned int m_uiActiveMaterial;
    NiShaderDeclarationPtr m_spShaderDecl;
    bool m_bDefaultMaterialDirtyFlag;

    static bool ms_bStreamingAppliesDefaultMaterial;

    enum 
    {
        NO_MATERIAL = UINT_MAX
    };
};

typedef NiPointer<NiGeometry> NiGeometryPtr;

// note these are stars, not smart pointers
typedef NiTPointerList<NiGeometry*> NiGeometryList;

#include "NiGeometry.inl"

#endif

