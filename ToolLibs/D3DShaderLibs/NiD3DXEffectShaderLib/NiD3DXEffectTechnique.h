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

#ifndef NID3DXEFFECTTECHNIQUE_H
#define NID3DXEFFECTTECHNIQUE_H

#include "NiD3DXEffectShaderLibType.h"
#include "NiD3DXEffectHeaders.h"
//#include <NiD3DRendererHeaders.h>
#include <NiShaderRequirementDesc.h>

NiSmartPointer(NiD3DXEffectShader);

class NiD3DXEffectFile;
class NiD3DXEffectParameter;
class NiD3DXEffectTechniqueDesc;
class NiShaderDesc;

class NID3DXEFFECTSHADER_ENTRY NiD3DXEffectTechnique : public NiRefObject
{
public:
    NiD3DXEffectTechnique();
    ~NiD3DXEffectTechnique();

    void Initialize(NiD3DXEffectTechniqueDesc* pkTechDesc);

    inline const char* GetShaderName() const;
    inline const char* GetDescription() const;
    inline const char* GetTechniqueName() const;
    inline unsigned int GetImplementation() const;
    inline D3DXTechniquePtr GetTechniquePtr() const;
    inline NiD3DXEffectFile* GetFile() const;

    // Get the D3D shader
    NiD3DXEffectShader* GetD3DShader(NiD3DRenderer* pkRenderer);
    bool ReleaseShader();

    // *** begin Emergent internal use only ***

    // These functions are used during the creation
    void SetTechniquePtr(D3DXTechniquePtr pkTechnique);
    inline void SetFile(NiD3DXEffectFile* pkFile);

    void AddParameter(NiD3DXEffectParameter* pkParam);
    NiD3DXEffectParameter* GetFirstParameter();
    NiD3DXEffectParameter* GetNextParameter();
    NiD3DXEffectParameter* GetParameter(const char* pcParamName);
    inline bool GetTimeParameterPresent() const;

    NiShaderRequirementDesc* GetRequirements();

    // Query shader for presence of shader programs
    bool GetVSPresentAllPasses() const;
    bool GetVSPresentAnyPass() const;
    bool GetPSPresentAllPasses() const;
    bool GetPSPresentAnyPass() const;

    bool VertexShaderPresent(unsigned int uiPass) const;
    bool PixelShaderPresent(unsigned int uiPass) const;

    void LinkHandle(LPD3DXEFFECT pkEffect);

    void DestroyRendererData();

    // *** end Emergent internal use only ***

    // Semantic adapter table
    inline const NiSemanticAdapterTable& GetSemanticAdapterTable() const;
    inline NiSemanticAdapterTable& GetSemanticAdapterTable();
protected:
    char* m_pcTechniqueName;
    unsigned int m_uiImplementation;

    D3DXTechniquePtr m_pkTechnique;
    NiD3DXEffectFile* m_pkFile;

    NiD3DXEffectShaderPtr m_spShader;

    NiTPointerList<NiD3DXEffectParameter*> m_kParameters;
    NiTListIterator m_kParamIter;

    unsigned int m_uiNumPasses;
    bool* m_abVertexShaderPresent;
    bool* m_abPixelShaderPresent;

    bool m_bTimeParameter;

    bool m_bUsesNiRenderState;
    bool m_bUsesNiLightState;
    bool m_bBlendIndicesAsD3DColor;
    bool m_bSoftwareVPRequired;
    bool m_bSoftwareVPAcceptable;

    NiShaderRequirementDesc* m_pkReqDesc;

    // Semantic adapter table
    NiSemanticAdapterTable m_kAdapterTable;
};

NiSmartPointer(NiD3DXEffectTechnique);

#include "NiD3DXEffectTechnique.inl"

#endif  //NID3DXEFFECTTECHNIQUE_H
