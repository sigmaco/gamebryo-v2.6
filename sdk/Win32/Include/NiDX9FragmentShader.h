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

#ifndef NIDX9FRAGMENTSHADER_H
#define NIDX9FRAGMENTSHADER_H

#include "NiD3DShader.h"
#include <NiShaderDesc.h>
#include <NiMaterialDescriptor.h>
#include <NiFragmentShaderInstanceDescriptor.h>

class NIDX9RENDERER_ENTRY NiDX9FragmentShader : public NiD3DShader
{
    NiDeclareRTTI;
public:
    NiDX9FragmentShader(NiMaterialDescriptor* pkDesc);
    virtual ~NiDX9FragmentShader();

    // Initialize function
    virtual bool Initialize(NiD3DRenderer* pkRenderer);

    // Override these functions to implement a custom pipeline...
    virtual unsigned int UpdatePipeline(const NiRenderCallContext& kRCC);

    virtual void DestroyRendererData();
    virtual void RecreateRendererData();

    virtual bool IsGenericallyConfigurable();

    // Render Pass
    virtual bool AppendRenderPass(unsigned int& uiPassId);

    // Render States
    virtual bool SetAlphaOverride(
        unsigned int uiPassId, 
        bool bAlphaBlend,
        bool bUsePreviousSrcBlendMode,
        NiAlphaProperty::AlphaFunction eSrcBlendMode,
        bool bUsePreviousDestBlendMode,
        NiAlphaProperty::AlphaFunction eDestBlendMode);

    // GPU Programs
    virtual bool SetGPUProgram(
        unsigned int uiPassId,
        NiGPUProgram* pkProgram, 
        NiGPUProgram::ProgramType& eProgramType);
    virtual NiShaderConstantMap* CreateShaderConstantMap(
        unsigned int uiPassId, 
        NiGPUProgram::ProgramType eProgramType,
        unsigned int uiMapIndex = 0);

    // Texture samplers
    virtual bool AppendTextureSampler(
        unsigned int uiPassId,
        unsigned int& uiSamplerId, 
        const NiFixedString& kSemantic,
        const NiFixedString& kVariableName, 
        unsigned int uiInstance = 0);

    virtual const NiShader::NiShaderInstanceDescriptor*
        GetShaderInstanceDesc() const;

protected:

    void PrepareTextureStage(
        NiD3DTextureStage* pkStage,
        const NiRenderCallContext& kRCC);

    static unsigned int GetSamplerValue(
        NiD3DRenderer* pkD3DRenderer,
        unsigned int uiState, unsigned int uiValue,
        NiTexturingProperty::ClampMode eClampMode,
        NiTexturingProperty::FilterMode eFilterMode);

    NiFragmentShaderInstanceDescriptor m_kDescriptor;
};

typedef NiPointer<NiDX9FragmentShader> NiDX9FragmentShaderPtr;

#endif  //#ifndef NIDX9FRAGMENTSHADER_H
