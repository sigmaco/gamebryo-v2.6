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

#ifndef NID3DXEFFECTSHADER_H
#define NID3DXEFFECTSHADER_H

#include "NiD3DXEffectShaderLibType.h"
#include <NiD3DShader.h>

#if defined(_XENON )
#include "NiD3DXEffectHeaders_Xenon.h"
#elif defined (WIN32)
#include "NiD3DXEffectHeaders_DX9.h"
#endif

class NiD3DXEffectTechnique;

class NID3DXEFFECTSHADER_ENTRY NiD3DXEffectShader : public NiD3DShader
{
    NiDeclareRTTI;
public:
    NiD3DXEffectShader();
    virtual ~NiD3DXEffectShader();

    virtual bool Initialize();

    virtual unsigned int PreProcessPipeline(const NiRenderCallContext& kRCC);
    virtual unsigned int UpdatePipeline(const NiRenderCallContext& kRCC);
    virtual unsigned int SetupRenderingPass(const NiRenderCallContext& kRCC);
    virtual unsigned int SetupTransformations(const NiRenderCallContext& kRCC);
    virtual unsigned int SetupShaderPrograms(const NiRenderCallContext& kRCC);
    virtual unsigned int PostRenderSubmesh(const NiRenderCallContext& kRCC);
    virtual unsigned int PostProcessPipeline(const NiRenderCallContext& kRCC);
    virtual unsigned int FirstPass();
    virtual unsigned int NextPass();

    virtual void HandleLostDevice();
    virtual void HandleResetDevice();

    virtual void DestroyRendererData();
    virtual void RecreateRendererData();

    // *** begin Emergent internal use only

    enum BoneMatrixCalcMethod
    {
        BONECALC_SKIN       = 0x00000000,
        BONECALC_NO_SKIN    = 0x00000001
    };

    inline unsigned int GetBoneMatrixRegisters() const;
    inline void SetBoneMatrixRegisters(unsigned int uiRegisters);
    BoneMatrixCalcMethod GetBoneCalcMethod() const;
    inline void SetBoneCalcMethod(BoneMatrixCalcMethod eMethod);

    inline void SetEffectTechnique(NiD3DXEffectTechnique* pkD3DXEffectTechnique);
    inline void SetTechniquePtr(D3DXTechniquePtr pkTechnique);

    virtual bool SetupGeometry(NiRenderObject* pkGeometry, 
        NiMaterialInstance* pkMaterialInstance);

    inline void SetSoftwareVP(bool bSoftwareVP);

    // Query shader for presence of shader programs
    virtual bool GetVSPresentAllPasses() const;
    virtual bool GetVSPresentAnyPass() const;
    virtual bool GetPSPresentAllPasses() const;
    virtual bool GetPSPresentAnyPass() const;

    inline void SetActualImplementation(unsigned int uiActualImplementation);
    inline unsigned int GetActualImplementation() const;

    // *** end Emergent internal use only

protected:
    void CreateSemanticAdapterTable();

    static bool GetShaderParameterValues(D3DDECLUSAGE eD3DUsage,
        unsigned int uiUsageIndex,
        NiShaderDeclaration::ShaderParameter& eInput, 
        NiShaderDeclaration::ShaderParameterType& eType, 
        NiShaderDeclaration::ShaderParameterUsage& eUsage, 
        NiShaderDeclaration::ShaderParameterTesselator& eTess);

    NiD3DXEffectTechnique* m_pkD3DXEffectTechnique;
    D3DXTechniquePtr m_pkTechnique;

    LPD3DXBASEEFFECT m_pkD3DXEffect;
    LPD3DXEFFECT m_pkD3DXRenderableEffect;

    // Actual implementation number in case this shader was created
    // with the "default implementation" value.
    unsigned int m_uiActualImplementation;

    unsigned int m_uiBoneMatrixRegisters;
    BoneMatrixCalcMethod m_eBoneCalcMethod;
    bool m_bSoftwareVP;
};

NiSmartPointer(NiD3DXEffectShader);

#include "NiD3DXEffectShader.inl"

#endif  //NID3DXEFFECTSHADER_H
