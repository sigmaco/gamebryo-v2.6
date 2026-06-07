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

#ifndef NID3D10EFFECTTECHNIQUE_H
#define NID3D10EFFECTTECHNIQUE_H

#include "NiD3D10EffectShaderLibType.h"
#include "NiD3D10EffectParameterDesc.h"

#include <NiD3D10Headers.h>
#include <NiFixedString.h>
#include <NiSemanticAdapterTable.h>
#include <NiShaderRequirementDesc.h>
#include <NiTPointerList.h>

NiSmartPointer(NiD3D10EffectShader);

class NiD3D10EffectFile;
class NiD3D10EffectParameter;
class NiD3D10EffectTechniqueDesc;
class NiShaderDesc;

class NID3D10EFFECTSHADER_ENTRY NiD3D10EffectTechnique : public NiRefObject
{
public:
    NiD3D10EffectTechnique();
    ~NiD3D10EffectTechnique();

    void Initialize(NiD3D10EffectTechniqueDesc* pkTechDesc);

    inline NiFixedString GetShaderName() const;
    inline NiFixedString GetDescription() const;
    inline NiFixedString GetTechniqueName() const;

    inline NiUInt32 GetImplementation() const;
    inline NiD3D10EffectFile* GetFile() const;

    // Get the D3D10 shader
    NiD3D10EffectShader* GetD3D10Shader();
    bool ReleaseShader();

    bool IsParameterUsed(NiD3D10EffectParameter* pkParam) const;

    // *** begin Emergent internal use only ***

    // These functions are used during the creation
    inline void SetFile(NiD3D10EffectFile* pkFile);

    void AddParameter(NiD3D10EffectParameter* pkParam);
    NiD3D10EffectParameter* GetFirstParameter(
        NiD3D10EffectParameterDesc::ParameterType kType = 
        NiD3D10EffectParameterDesc::PT_INVALID);

    NiD3D10EffectParameter* GetNextParameter(
        NiD3D10EffectParameterDesc::ParameterType kType = 
        NiD3D10EffectParameterDesc::PT_INVALID);

    NiD3D10EffectParameter* GetParameter(
        const NiFixedString& kParamName,
        NiD3D10EffectParameterDesc::ParameterType kType = 
        NiD3D10EffectParameterDesc::PT_INVALID);

    inline bool GetTimeParameterPresent() const;

    inline bool HasParametersOfType(
        NiD3D10EffectParameterDesc::ParameterType kType);
    NiShaderRequirementDesc* GetRequirements();

    void SetD3D10Technique(ID3D10EffectTechnique* pkTechnique);
    ID3D10EffectTechnique* GetD3D10Technique() const;

    void SetShaderD3D10Effect(ID3D10Effect* pkEffect);

    void DestroyRendererData();

    // *** end Emergent internal use only ***

protected:
    void FillSemanticAdapterTable();

    class EffectPass : public NiMemObject
    {
    public:
        EffectPass(
            NiUInt32 uiVSBufferCount, 
            NiUInt32 uiGSBufferCount, 
            NiUInt32 uiPSBufferCount);
        virtual ~EffectPass();

        ID3D10EffectPass* GetD3D10Pass() const;
        void SetD3D10Pass(ID3D10EffectPass* pkPass);

        ID3D10Blob* GetInputSignature() const;
        void SetInputSignature(ID3D10Blob* pkInputSignature);

        const D3D10_STATE_BLOCK_MASK* GetStateBlockMask() const;
        void SetStateBlockMask(const D3D10_STATE_BLOCK_MASK& kMask);

        NiUInt32 GetVSConstantBufferCount() const;
        NiUInt32 GetGSConstantBufferCount() const;
        NiUInt32 GetPSConstantBufferCount() const;

        void SetVSConstantBufferName(
            NiUInt32 uiIndex, 
            const NiFixedString& kName);
        void SetGSConstantBufferName(
            NiUInt32 uiIndex, 
            const NiFixedString& kName);
        void SetPSConstantBufferName(
            NiUInt32 uiIndex, 
            const NiFixedString& kName);

        const NiFixedString& GetVSConstantBufferName(NiUInt32 uiIndex);
        const NiFixedString& GetGSConstantBufferName(NiUInt32 uiIndex);
        const NiFixedString& GetPSConstantBufferName(NiUInt32 uiIndex);

    protected:
        ID3D10EffectPass* m_pkPass;
        ID3D10Blob* m_pkInputSignature;
        D3D10_STATE_BLOCK_MASK m_kStateBlockMask;
        NiTObjectArray<NiFixedString> m_kVSConstantBuffers;
        NiTObjectArray<NiFixedString> m_kGSConstantBuffers;
        NiTObjectArray<NiFixedString> m_kPSConstantBuffers;
    };

    NiTPrimitiveArray<EffectPass*> m_kPasses;

    NiFixedString m_kTechniqueName;
    NiUInt32 m_uiImplementation;

    NiD3D10EffectFile* m_pkFile;

    ID3D10EffectTechnique* m_pkD3D10Technique;

    NiD3D10EffectShaderPtr m_spShader;

    NiSemanticAdapterTable m_kAdapterTable;

    NiTPointerList<NiD3D10EffectParameter*> m_kParameters;
    NiTPointerList<NiD3D10EffectParameter*> 
        m_kTypeParameters[NiD3D10EffectParameterDesc::PT_NUMBER_TYPES];

    mutable NiTListIterator m_kParamIter;
    mutable NiTListIterator 
        m_kTypeParamIter[NiD3D10EffectParameterDesc::PT_NUMBER_TYPES];

    bool m_bHasParameterType[NiD3D10EffectParameterDesc::PT_NUMBER_TYPES];    
    bool m_bTimeParameter;
    bool m_bUsesNiRenderState;

    NiShaderRequirementDesc* m_pkReqDesc;
};

NiSmartPointer(NiD3D10EffectTechnique);

#include "NiD3D10EffectTechnique.inl"

#endif  //NID3D10EFFECTTECHNIQUE_H
