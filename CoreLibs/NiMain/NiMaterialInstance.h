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

#ifndef NIMATERIALINSTANCE_H
#define NIMATERIALINSTANCE_H

#include "NiMemObject.h"
#include "NiMaterial.h"
#include "NiShader.h"

class NiDynamicEffectState;
class NiRenderObject;
class NiPropertyState;

NiSmartPointer(NiMaterialInstance);

class NIMAIN_ENTRY NiMaterialInstance : public NiMemObject
{
public:
    NiMaterialInstance();
    NiMaterialInstance(const NiMaterialInstance& kMatInst);
    NiMaterialInstance(NiMaterial* pkMaterial);
    ~NiMaterialInstance();

    inline NiMaterialInstance& operator=(const NiMaterialInstance& kMatInst);

    inline NiMaterial* GetMaterial() const;
    inline void ClearCachedShader();

    NiShader* GetCachedShader(const NiRenderObject* pkGeometry, 
        const NiPropertyState* pkState, 
        const NiDynamicEffectState* pkEffects) const;
    NiShader* GetCurrentShader(NiRenderObject* pkGeometry, 
        const NiPropertyState* pkState, 
        const NiDynamicEffectState* pkEffects);

    inline void SetMaterialNeedsUpdate(bool bNeedsUpdate);

    inline const NiSemanticAdapterTable& GetSemanticAdapterTable() const;
    inline NiSemanticAdapterTable& GetSemanticAdapterTable();
    void UpdateSemanticAdapterTable(NiRenderObject* pkMesh);

    // *** begin Emergent internal use only ***
    NiMaterialInstance(NiMaterial* pkMaterial, 
        unsigned int uiMaterialExtraData);
    inline void SetMaterialExtraData(unsigned int uiExtraData);
    inline unsigned int GetMaterialExtraData() const;

    inline NiVertexDeclarationCache GetVertexDeclarationCache() const;
    inline void SetVertexDeclarationCache(NiVertexDeclarationCache kCache);
    inline void ClearVertexDeclarationCache();

    enum
    {
        DEFAULT_EXTRA_DATA = NiShader::DEFAULT_IMPLEMENTATION
    };

    inline void Reinitialize();
    // *** end Emergent internal use only ***

protected:
    NiShaderPtr m_spCachedShader;
    NiMaterialPtr m_spMaterial;
    unsigned int m_uiMaterialExtraData;

    NiVertexDeclarationCache m_kVertexDeclarationCache;
    NiSemanticAdapterTable m_kAdapterTable;
    
    enum MaterialUpdateFlag
    {
        CLEAN,
        DIRTY,
        UNKNOWN
    };

    MaterialUpdateFlag m_eNeedsUpdate;
};

#include "NiMaterialInstance.inl"

#endif // NIMATERIALINSTANCE_H
