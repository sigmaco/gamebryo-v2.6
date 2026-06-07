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

#ifndef NISHADER_H
#define NISHADER_H

#include "NiAlphaProperty.h"
#include "NiGPUProgram.h"
#include "NiRTTI.h"
#include "NiRefObject.h"
#include "NiRenderCallContext.h"
#include "NiSemanticAdapterTable.h"
#include "NiStream.h"
#include "NiOutputStreamDescriptor.h"

class NiDynamicEffectState;
class NiMaterialInstance;
class NiRenderObject;
class NiPropertyState;
class NiShaderConstantMap;
class NiShaderDeclaration;

typedef void* NiVertexDeclarationCache;// also defined in NiRenderCallContext.h

class NIMAIN_ENTRY NiShader : public NiRefObject
{
    NiDeclareRootRTTI(NiShader);
public:
    enum Platform
    {
        // Unused (legacy)  = 0x00000001,
        NISHADER_DX9        = 0x00000002,
        // Unused (legacy)  = 0x00000004,
        NISHADER_XENON      = 0x00000008,
        NISHADER_PS3        = 0x00000010,
        NISHADER_D3D10      = 0x00000020,
        NISHADER_AGNOSTIC   = NISHADER_DX9 | NISHADER_D3D10 |
                              NISHADER_XENON | NISHADER_PS3
    };

    enum
    {
        DEFAULT_IMPLEMENTATION = 0xffffffff
    };

    NiShader();
    virtual ~NiShader();

    const NiFixedString& GetName() const;
    unsigned int GetImplementation() const;

    // Overridable functions
    virtual bool IsInitialized();

    virtual bool Initialize();

    virtual bool SetupGeometry(NiRenderObject* pkGeometry, 
        NiMaterialInstance* pkMaterialInstance);

    // Renderer interaction functions, in the order in which they are called
    virtual unsigned int PreProcessPipeline(const NiRenderCallContext& kRCC);
    virtual unsigned int UpdatePipeline(const NiRenderCallContext& kRCC);
    virtual unsigned int FirstPass();
    virtual unsigned int SetupRenderingPass(const NiRenderCallContext& kRCC);
    virtual unsigned int SetupTransformations(const NiRenderCallContext& kRCC);
    virtual unsigned int SetupShaderPrograms(const NiRenderCallContext& kRCC);
    virtual unsigned int PreRenderSubmesh(const NiRenderCallContext& kRCC);
    virtual unsigned int PostRenderSubmesh(const NiRenderCallContext& kRCC);
    virtual unsigned int PostRender(const NiRenderCallContext& kRCC);
    virtual unsigned int NextPass();
    virtual unsigned int PostProcessPipeline(const NiRenderCallContext& kRCC);

    // Determine whether or not this class implements the generic
    // NiShader configuration API
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
    virtual bool SetUsesNiRenderState(bool bRenderState);

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

    const NiSemanticAdapterTable& GetSemanticAdapterTable() const;
    NiSemanticAdapterTable& GetSemanticAdapterTable();
    void SetSemanticAdapterTableFromShaderDeclaration(
        NiShaderDeclaration* pkShaderDecl);

    inline void SetOutputStreamDescriptors(
        const NiOutputStreamDescriptorArray& kDescriptors);
    
    inline unsigned int GetOutputStreamCount() const;
    inline NiOutputStreamDescriptor& GetOutputStreamDescriptor(
        unsigned int uiIndex) const;

    // *** begin Emergent internal use only ***
    void SetName(const NiFixedString& kName);
    void SetImplementation(unsigned int uiImplementation);

    // This is used by the shader factory to flag that this shader was the
    // best implementation for the hardware.
    bool GetIsBestImplementation() const;
    void SetIsBestImplementation(bool bIsBest);

    class NIMAIN_ENTRY NiShaderInstanceDescriptor : public NiMemObject
    {
        NiDeclareRootRTTI(NiShaderInstanceDescriptor);
    public:
        inline virtual ~NiShaderInstanceDescriptor(){};
    };

    virtual const NiShaderInstanceDescriptor*
        GetShaderInstanceDesc() const;

    virtual void ReferenceVertexDeclarationCache(
        NiVertexDeclarationCache kCache) const;
    virtual void ReleaseVertexDeclarationCache(
        NiVertexDeclarationCache kCache) const;

    // *** end Emergent internal use only ***

protected:
    bool GetVertexInputSemantics(
        NiShaderDeclaration::ShaderRegisterEntry* pkSemantics, 
        unsigned int uiSemanticEntryCount,
        NiShaderDeclaration* pkDecl);

    NiFixedString m_kName;
    unsigned int m_uiImplementation;

    NiSemanticAdapterTable m_kAdapterTable;

    NiOutputStreamDescriptorArray m_kOutputStreamDescriptors;

    bool m_bInitialized;
    bool m_bBestImplementation;
};

typedef NiPointer<NiShader> NiShaderPtr;

#include "NiShader.inl"

#endif  //#ifndef NISHADER_H
