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

#ifndef NID3D10EFFECTSHADER_H
#define NID3D10EFFECTSHADER_H

#include "NiD3D10EffectShaderLibType.h"
#include "NiD3D10EffectParameterDesc.h"
#include <NiD3D10Shader.h>

class NiD3D10EffectTechnique;
class NiD3D10EffectPass;

class NID3D10EFFECTSHADER_ENTRY NiD3D10EffectShader : public NiD3D10Shader
{
    NiDeclareRTTI;
public:
    NiD3D10EffectShader();
    virtual ~NiD3D10EffectShader();

    virtual bool Initialize();

    virtual unsigned int PreProcessPipeline(const NiRenderCallContext& kRCC);
    virtual unsigned int SetupShaderPrograms(const NiRenderCallContext& kRCC);
    virtual unsigned int PostProcessPipeline(const NiRenderCallContext& kRCC);

    virtual void DestroyRendererData();
    virtual void RecreateRendererData();

    virtual bool GetVertexInputSemantics(
        NiShaderDeclaration::ShaderRegisterEntry* pkSemantics, 
        unsigned int uiSemanticEntryCount);

    virtual const D3D10_STATE_BLOCK_MASK* GetStateBlockMask(
        const NiRenderCallContext& kRCC) const;

    // *** begin Emergent internal use only

    // For use when first initializing the NiShader object.
    void SetEffectTechnique(NiD3D10EffectTechnique* pkD3D10EffectTechnique);

    virtual bool SetupGeometry(NiRenderObject* pkGeometry, 
        NiMaterialInstance* pkMaterialInstance);

    void SetActualImplementation(unsigned int uiActualImplementation);
    unsigned int GetActualImplementation() const;

    void InitializePassArray(NiUInt32 uiPassCount);
    void SetAdapterSemanticTable(const NiSemanticAdapterTable& kTable);
    NiD3D10EffectPass* GetPass(NiUInt32 uiPass) const;
    static void EnableParameterType(NiD3D10EffectParameterDesc::ParameterType kType,
        bool bval);

    void SetD3D10Effect(ID3D10Effect* pkEffect);
    static bool NiD3D10EffectShader::
        bEnabledParameterTypes[NiD3D10EffectParameterDesc::PT_NUMBER_TYPES];

    // *** end Emergent internal use only

protected:
    // 

    ID3D10Effect* m_pkD3D10Effect;

    NiD3D10EffectTechnique* m_pkD3D10EffectTechnique;

    // Actual implementation number in case this shader was created
    // with the "default implementation" value.
    NiUInt32 m_uiActualImplementation;
};

NiSmartPointer(NiD3D10EffectShader);

#endif  //NID3D10EFFECTSHADER_H
