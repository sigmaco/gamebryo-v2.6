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

// Precompiled Header
#include "NiD3D10EffectShaderLibPCH.h"

#include "NiD3D10EffectTechnique.h"
#include "NiD3D10EffectFile.h"
#include "NiD3D10EffectPass.h"
#include "NiD3D10EffectParameter.h"
#include "NiD3D10EffectShader.h"
#include "NiD3D10EffectTechniqueDesc.h"
#include "NiD3D10EffectTechniqueSet.h"

#include <NiD3D10Renderer.h>

//---------------------------------------------------------------------------
NiD3D10EffectTechnique::NiD3D10EffectTechnique() :
    m_uiImplementation(0), 
    m_pkFile(NULL),
    m_pkD3D10Technique(NULL),
    m_kParamIter(NULL),
    m_bTimeParameter(false),
    m_bUsesNiRenderState(false)
{ /* */ }
//---------------------------------------------------------------------------
NiD3D10EffectTechnique::~NiD3D10EffectTechnique()
{
    NILOG(
        "%s> Killing NiD3D10EffectTechnique %d\n", 
        __FUNCTION__,
        m_uiImplementation);

    DestroyRendererData();

    const NiUInt32 uiPassCount = m_kPasses.GetSize();
    for (NiUInt32 i = 0; i < uiPassCount; i++)
    {
        EffectPass* pkPass = m_kPasses.GetAt(i);
        NiDelete pkPass;
    }

    // Don't delete parameters - NiD3D10EffectFile owns them
}
//---------------------------------------------------------------------------
NiD3D10EffectShader* NiD3D10EffectTechnique::GetD3D10Shader()
{
#ifdef NIDEBUG
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
#endif
    NIASSERT(pkRenderer);

    // Ensure effect is loaded
    m_pkFile->LinkRendererData();

    ID3D10Effect* pkD3D10Effect = m_pkFile->GetEffect();

    if (m_spShader == NULL)
    {
        m_spShader = NiNew NiD3D10EffectShader;
        m_spShader->SetEffectTechnique(this);
        m_spShader->SetD3D10Effect(pkD3D10Effect);
        m_spShader->SetName(GetShaderName());
        m_spShader->SetImplementation(m_uiImplementation);
        m_spShader->SetActualImplementation(m_uiImplementation);

        m_spShader->SetUsesNiRenderState(m_bUsesNiRenderState);
        m_spShader->SetAdapterSemanticTable(m_kAdapterTable);

        const NiUInt32 uiPassCount = m_kPasses.GetSize();
        m_spShader->InitializePassArray(uiPassCount);

        for (NiUInt32 i = 0; i < uiPassCount; i++)
        {
            EffectPass* pkEffectPass = m_kPasses.GetAt(i);
            ID3D10EffectPass* pkD3D10Pass = pkEffectPass->GetD3D10Pass();

            // Assign pass variable
            NiD3D10EffectPass* pkPass = m_spShader->GetPass(i);
            pkPass->SetD3D10Pass(pkD3D10Pass);

            // Set up constant maps for pass
            const NiUInt32 uiVSCBCount = 
                pkEffectPass->GetVSConstantBufferCount();
            NiUInt32 j = 0;
            for (; j < uiVSCBCount; j++)
            {
                NiFixedString kName = 
                    pkEffectPass->GetVSConstantBufferName(j);
                NIASSERT(kName.Exists());
                NiD3D10ShaderConstantMap* pkMap =
                    m_pkFile->GetShaderConstantMap(kName);
                NIASSERT(pkMap);

                pkPass->AddConstantMap(pkMap);
            }

            const NiUInt32 uiGSCBCount = 
                pkEffectPass->GetGSConstantBufferCount();
            for (j = 0; j < uiGSCBCount; j++)
            {
                NiFixedString kName = 
                    pkEffectPass->GetGSConstantBufferName(j);
                NIASSERT(kName.Exists());
                NiD3D10ShaderConstantMap* pkMap =
                    m_pkFile->GetShaderConstantMap(kName);
                NIASSERT(pkMap);

                pkPass->AddConstantMap(pkMap);
            }

            const NiUInt32 uiPSCBCount = 
                pkEffectPass->GetPSConstantBufferCount();
            for (j = 0; j < uiPSCBCount; j++)
            {
                NiFixedString kName = 
                   pkEffectPass->GetPSConstantBufferName(j);
                NIASSERT(kName.Exists());
                NiD3D10ShaderConstantMap* pkMap =
                    m_pkFile->GetShaderConstantMap(kName);
                NIASSERT(pkMap);

                pkPass->AddConstantMap(pkMap);
            }

            pkPass->SetInputSignature(pkEffectPass->GetInputSignature());
            pkPass->SetStateBlockMask(pkEffectPass->GetStateBlockMask());
        }
    }

    return m_spShader;
}
//---------------------------------------------------------------------------
bool NiD3D10EffectTechnique::ReleaseShader()
{
    bool bRet = (m_spShader != NULL);
    m_spShader = NULL;
    return bRet;
}
//---------------------------------------------------------------------------
bool NiD3D10EffectTechnique::IsParameterUsed(NiD3D10EffectParameter* pkParam) 
    const
{
    const NiFixedString& kCBName = pkParam->GetConstantBufferName();

    // If it's not in a constant buffer, there's no way to know whether the
    // parameter is used or not. Must assume it is.
    if (!kCBName.Exists())
        return true;

    const NiUInt32 uiPassCount = m_kPasses.GetSize();
    for (NiUInt32 i = 0; i < uiPassCount; i++)
    {
        EffectPass* pkPass = m_kPasses.GetAt(i);
        NIASSERT(pkPass);

        NiUInt32 j = 0;
        const NiUInt32 uiVSBufferCount = pkPass->GetVSConstantBufferCount();
        for (; j < uiVSBufferCount; j++)
        {
            if (pkPass->GetVSConstantBufferName(j) == kCBName)
                return true;
        }
        const NiUInt32 uiGSBufferCount = pkPass->GetGSConstantBufferCount();
        for (j = 0; j < uiGSBufferCount; j++)
        {
            if (pkPass->GetGSConstantBufferName(j) == kCBName)
                return true;
        }
        const NiUInt32 uiPSBufferCount = pkPass->GetPSConstantBufferCount();
        for (j = 0; j < uiPSBufferCount; j++)
        {
            if (pkPass->GetPSConstantBufferName(j) == kCBName)
                return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3D10EffectTechnique::Initialize(
    NiD3D10EffectTechniqueDesc* pkTechDesc)
{
    NIASSERT(pkTechDesc);

    m_kTechniqueName = pkTechDesc->m_kTechniqueName;

    m_pkReqDesc = NiNew NiShaderRequirementDesc;
    m_pkReqDesc->SetName(pkTechDesc->m_kShaderName);
    m_pkReqDesc->SetDescription(pkTechDesc->m_kDesc);

    m_pkReqDesc->AddPixelShaderVersion(pkTechDesc->m_uiPSVersion);
    m_pkReqDesc->AddVertexShaderVersion(pkTechDesc->m_uiVSVersion);
    m_pkReqDesc->AddUserDefinedVersion(pkTechDesc->m_uiUserVersion);

    m_pkReqDesc->SetPlatformD3D10(true);

    m_pkReqDesc->SetBonesPerPartition(pkTechDesc->m_uiBonesPerPartition);
    m_pkReqDesc->SetBinormalTangentMethod(pkTechDesc->m_eNBTMethod);

    m_uiImplementation = pkTechDesc->m_uiImplementation;

    m_bUsesNiRenderState = pkTechDesc->m_bUsesNiRenderState;

    const NiUInt32 uiPassCount = pkTechDesc->m_uiPassCount;
    m_kPasses.SetSize(uiPassCount);
    for (NiUInt32 i = 0; i < uiPassCount; i++)
    {
        const NiUInt32 uiVSBufferCount = 
            pkTechDesc->m_auiBufferCount[i][NiGPUProgram::PROGRAM_VERTEX];
        const NiUInt32 uiGSBufferCount = 
            pkTechDesc->m_auiBufferCount[i][NiGPUProgram::PROGRAM_GEOMETRY];
        const NiUInt32 uiPSBufferCount = 
            pkTechDesc->m_auiBufferCount[i][NiGPUProgram::PROGRAM_PIXEL];
        EffectPass* pkPass = NiNew EffectPass(
            uiVSBufferCount, 
            uiGSBufferCount, 
            uiPSBufferCount);
        NiUInt32 j = 0;
        for (; j < uiVSBufferCount; j++)
        {
            pkPass->SetVSConstantBufferName(
                j, 
                pkTechDesc->m_aaakConstantBuffers[i]
                [NiGPUProgram::PROGRAM_VERTEX][j]);
        }
        for (j = 0; j < uiGSBufferCount; j++)
        {
            pkPass->SetGSConstantBufferName(
                j, 
                pkTechDesc->m_aaakConstantBuffers[i]
                [NiGPUProgram::PROGRAM_GEOMETRY][j]);
        }
        for (j = 0; j < uiPSBufferCount; j++)
        {
            pkPass->SetPSConstantBufferName(
                j, 
                pkTechDesc->m_aaakConstantBuffers[i]
                [NiGPUProgram::PROGRAM_PIXEL][j]);
        }
            
        m_kPasses.SetAt(i, pkPass);
    }

    // Check the description for a valid semantic adapter table and move it
    // over if available
    if (pkTechDesc->m_kAdapterTable.GetNumFilledTableEntries() > 0)
    {
        NIASSERT(m_kAdapterTable.GetNumFilledTableEntries() == 0);
        m_kAdapterTable = pkTechDesc->m_kAdapterTable;
    }
    
    unsigned int i = NiD3D10EffectParameterDesc::PT_NONE;
    while (i< NiD3D10EffectParameterDesc::PT_NUMBER_TYPES)
    {
        m_kTypeParamIter[i] = NULL;
        m_bHasParameterType[i] = false;    
        i++;
    }
}
//---------------------------------------------------------------------------
void NiD3D10EffectTechnique::SetD3D10Technique(
    ID3D10EffectTechnique* pkTechnique)
{
    if (m_pkD3D10Technique == pkTechnique)
        return;

    m_pkD3D10Technique = pkTechnique;

    const NiUInt32 uiPassCount = m_kPasses.GetSize();
    // Grab input signature for each pass
    for (NiUInt32 i = 0; i < uiPassCount; i++)
    {
        EffectPass* pkEffectPass = m_kPasses.GetAt(i);

        ID3D10EffectPass* pkD3D10Pass = NULL;
        ID3D10Blob* pkInputSignature = NULL;
        if (pkTechnique)
        {
            pkD3D10Pass = pkTechnique->GetPassByIndex(i);
            NIASSERT(pkD3D10Pass && pkD3D10Pass->IsValid())
    
            D3D10_PASS_DESC kPassDesc;
            HRESULT hr = pkD3D10Pass->GetDesc(&kPassDesc);
            NIASSERT(SUCCEEDED(hr));
    
            hr = NiD3D10Renderer::D3D10CreateBlob(
                kPassDesc.IAInputSignatureSize, 
                &pkInputSignature);
            NIASSERT(SUCCEEDED(hr));
            memcpy(
                pkInputSignature->GetBufferPointer(), 
                kPassDesc.pIAInputSignature, 
                kPassDesc.IAInputSignatureSize);

            // Figure out which render state objects the pass does not set
            D3D10_STATE_BLOCK_MASK kStateBlockMask;
            hr = NiD3D10Renderer::D3D10StateBlockMaskDisableAll(
                &kStateBlockMask);
            hr = pkD3D10Pass->ComputeStateBlockMask(&kStateBlockMask);
            NIASSERT(SUCCEEDED(hr));

            // Need to store the inverse of this, indicating which
            // render state objects Gamebryo should set
            D3D10_STATE_BLOCK_MASK kFullStateBlockMask;
            hr = NiD3D10Renderer::D3D10StateBlockMaskEnableAll(
                &kFullStateBlockMask);
            NIASSERT(SUCCEEDED(hr));
            D3D10_STATE_BLOCK_MASK kInvertedStateBlockMask;
            hr = NiD3D10Renderer::D3D10StateBlockMaskDifference(
                &kFullStateBlockMask,
                &kStateBlockMask,
                &kInvertedStateBlockMask);
            NIASSERT(SUCCEEDED(hr));

            // Because of the way D3D10StateBlockMaskEnableAll works, the
            // bitfields will be set correctly but the individual Boolean
            // flags will not - they will have a value of 0xFFFF (true) or
            // 0xFFFE (false). Fix that up here.
            kInvertedStateBlockMask.VS &= 0x0001;
            kInvertedStateBlockMask.GS &= 0x0001;
            kInvertedStateBlockMask.PS &= 0x0001;
            kInvertedStateBlockMask.IAIndexBuffer &= 0x0001;
            kInvertedStateBlockMask.IAInputLayout &= 0x0001;
            kInvertedStateBlockMask.IAPrimitiveTopology &= 0x0001;
            kInvertedStateBlockMask.OMRenderTargets &= 0x0001;
            kInvertedStateBlockMask.OMDepthStencilState &= 0x0001;
            kInvertedStateBlockMask.OMBlendState &= 0x0001;
            kInvertedStateBlockMask.RSViewports &= 0x0001;
            kInvertedStateBlockMask.RSScissorRects &= 0x0001;
            kInvertedStateBlockMask.RSRasterizerState &= 0x0001;
            kInvertedStateBlockMask.SOBuffers &= 0x0001;
            kInvertedStateBlockMask.Predication &= 0x0001;

            pkEffectPass->SetStateBlockMask(kInvertedStateBlockMask);
        }

        pkEffectPass->SetD3D10Pass(pkD3D10Pass);
        pkEffectPass->SetInputSignature(pkInputSignature);

        if (m_spShader)
        {
            NiD3D10EffectPass* pkPass = m_spShader->GetPass(i);
            pkPass->SetD3D10Pass(pkD3D10Pass);
            pkPass->SetInputSignature(pkInputSignature);
        }

        if (pkInputSignature)
            pkInputSignature->Release();
    }

    FillSemanticAdapterTable();
}
//---------------------------------------------------------------------------
ID3D10EffectTechnique* NiD3D10EffectTechnique::GetD3D10Technique() const
{
    return m_pkD3D10Technique;
}
//---------------------------------------------------------------------------
void NiD3D10EffectTechnique::SetShaderD3D10Effect(ID3D10Effect* pkEffect)
{
    if (m_spShader)
        m_spShader->SetD3D10Effect(pkEffect);
}
//---------------------------------------------------------------------------
void NiD3D10EffectTechnique::DestroyRendererData()
{
    m_pkD3D10Technique = NULL;
    if (m_spShader)
        m_spShader->DestroyRendererData();
        
    unsigned int i = NiD3D10EffectParameterDesc::PT_NONE;
    while (i< NiD3D10EffectParameterDesc::PT_NUMBER_TYPES)
    {
        m_kTypeParameters[i].RemoveAll();
        i++;
    }
}
//---------------------------------------------------------------------------
void NiD3D10EffectTechnique::AddParameter(NiD3D10EffectParameter* pkParam)
{
    m_kParameters.AddTail(pkParam);
    if (pkParam->RequiresTime())
        m_bTimeParameter = true;     
    m_bHasParameterType[pkParam->GetType()] = true; 
    m_kTypeParameters[pkParam->GetType()].AddTail(pkParam);
}
//---------------------------------------------------------------------------
NiD3D10EffectParameter* NiD3D10EffectTechnique::GetFirstParameter(
    NiD3D10EffectParameterDesc::ParameterType kType)
{
    if(kType == NiD3D10EffectParameterDesc::PT_INVALID)
    {
        m_kParamIter = m_kParameters.GetHeadPos();
        return GetNextParameter();
    }
    else
    {
        m_kTypeParamIter[kType] = m_kTypeParameters[kType].GetHeadPos();
        return GetNextParameter(kType);
    }
}
//---------------------------------------------------------------------------
NiD3D10EffectParameter* NiD3D10EffectTechnique::
    GetNextParameter(NiD3D10EffectParameterDesc::ParameterType kType)
{
    if(kType == NiD3D10EffectParameterDesc::PT_INVALID)
    {
        if (m_kParamIter == NULL)
            return NULL;
        return m_kParameters.GetNext(m_kParamIter);
    }
    else
    {    
        if (m_kTypeParamIter[kType] == NULL)
            return NULL;
        return m_kTypeParameters[kType].GetNext(m_kTypeParamIter[kType]);
    }
}
//---------------------------------------------------------------------------
NiD3D10EffectParameter* NiD3D10EffectTechnique::GetParameter(
    const NiFixedString& kParamName,
    NiD3D10EffectParameterDesc::ParameterType kType)
{
    if (kType == NiD3D10EffectParameterDesc::PT_INVALID)
    {
        NiTListIterator kIter = m_kParameters.GetHeadPos();
        while (kIter)
        {
            NiD3D10EffectParameter* pkParam = m_kParameters.GetNext(kIter);
            if (pkParam && pkParam->GetName() == kParamName)
                return pkParam;
        }
    }
    else
    {
        NiTListIterator kIter = m_kTypeParameters[kType].GetHeadPos();
        while (kIter)
        {
            NiD3D10EffectParameter* pkParam = 
                m_kTypeParameters[kType].GetNext(kIter);
            if (pkParam && pkParam->GetName() == kParamName)
                return pkParam;
        }
    }
    return NULL;
}
//---------------------------------------------------------------------------
NiShaderRequirementDesc* NiD3D10EffectTechnique::GetRequirements()
{
    return m_pkReqDesc;
}
//---------------------------------------------------------------------------
void NiD3D10EffectTechnique::FillSemanticAdapterTable()
{
    if (m_kAdapterTable.GetNumFilledTableEntries())
    {
        // Table has already been filled; 
        // presumably by parsing the annotations
        return;
    }

    NIASSERT(m_pkD3D10Technique);
    D3D10_TECHNIQUE_DESC kTechniqueDesc;
    m_pkD3D10Technique->GetDesc(&kTechniqueDesc);

    D3D10_PASS_SHADER_DESC* pkVSDescs = 
        NiAlloc(D3D10_PASS_SHADER_DESC, kTechniqueDesc.Passes);

    NiUInt32 i = 0;
    NiUInt32 uiTotalSemanticCount = 0;
    for (; i < kTechniqueDesc.Passes; i++)
    {
        ID3D10EffectPass* pkPass = m_pkD3D10Technique->GetPassByIndex(i);
        HRESULT hr = pkPass->GetVertexShaderDesc(pkVSDescs + i);
        if (FAILED(hr))
            continue;

        D3D10_EFFECT_SHADER_DESC kEffectShaderDesc;
        ID3D10EffectShaderVariable* pkShaderVariable = 
            pkVSDescs[i].pShaderVariable;
        hr = pkShaderVariable->GetShaderDesc(
            pkVSDescs[i].ShaderIndex,
            &kEffectShaderDesc);
        if (FAILED(hr))
            continue;

        uiTotalSemanticCount += kEffectShaderDesc.NumInputSignatureEntries;
    }
    // Failing this assertion assumes no vertex shaders have any input - 
    // if that's the case, tripping this assertion is the least of our 
    // problems.
    NIASSERT(uiTotalSemanticCount)

        NiUInt32 uiFilledSemanticCount = 0;

    D3D10_SIGNATURE_PARAMETER_DESC* pkInputDescs = 
        NiAlloc(D3D10_SIGNATURE_PARAMETER_DESC, uiTotalSemanticCount);

    // Look for skinned objects - identifiable by the BLENDWEIGHT semantic
    bool bSkinned = false;
    for (i = 0; i < kTechniqueDesc.Passes; i++)
    {
        const NiUInt32 uiShaderIndex = pkVSDescs[i].ShaderIndex;
        D3D10_EFFECT_SHADER_DESC kEffectShaderDesc;
        ID3D10EffectShaderVariable* pkShaderVariable = 
            pkVSDescs[i].pShaderVariable;
        HRESULT hr = pkShaderVariable->GetShaderDesc(
            uiShaderIndex,
            &kEffectShaderDesc);
        if (FAILED(hr))
            continue;

        const NiUInt32 uiNumInputs = 
            kEffectShaderDesc.NumInputSignatureEntries;
        for (NiUInt32 j = 0; j < uiNumInputs; j++)
        {
            hr = pkShaderVariable->GetInputSignatureElementDesc(
                uiShaderIndex, j, &(pkInputDescs[uiFilledSemanticCount]));
            NIASSERT(SUCCEEDED(hr));

            if (NiCommonSemantics::BLENDWEIGHT().EqualsNoCase(
                pkInputDescs[uiFilledSemanticCount].SemanticName))
            {
                bSkinned = true;
            }

            // System values don't come from the vertex buffer - kill it
            bool bKeep = (D3D10_NAME_UNDEFINED == 
                pkInputDescs[uiFilledSemanticCount].SystemValueType);

            if (bKeep && i > 0)
            {
                // Eliminate redundant semantics
                for (NiUInt32 k = 0; k < uiFilledSemanticCount; k++)
                {
                    if ((0 == strcmp(pkInputDescs[k].SemanticName, 
                        pkInputDescs[uiFilledSemanticCount].SemanticName)) &&
                        (pkInputDescs[k].SemanticIndex == 
                        pkInputDescs[uiFilledSemanticCount].SemanticIndex))
                    {
                        // Semantic already exists
                        bKeep = false;
                        break;
                    }
                }
            }
            if (bKeep)
                uiFilledSemanticCount++;
            else
                uiTotalSemanticCount--;
        }
    }

    NIASSERT(uiFilledSemanticCount == uiTotalSemanticCount);

    NiFree(pkVSDescs);

    m_kAdapterTable.InitializeTable(uiFilledSemanticCount);


    for (i = 0; i < uiFilledSemanticCount; i++)
    {
        NiFixedString kRendererSemanticName = pkInputDescs[i].SemanticName;
        NiFixedString kGenericSemanticName = kRendererSemanticName;
        if (bSkinned)
        {
            // Replace POSITION/NORMAL/BINORMAL/TANGENT with the *_BP 
            // alternatives when skinned objects are used.
            if (kRendererSemanticName.EqualsNoCase(
                NiCommonSemantics::POSITION()))
            {
                kGenericSemanticName = NiCommonSemantics::POSITION_BP();
            }
            else if (kRendererSemanticName.EqualsNoCase(
                NiCommonSemantics::NORMAL()))
            {
                kGenericSemanticName = NiCommonSemantics::NORMAL_BP();
            }
            else if (kRendererSemanticName.EqualsNoCase(
                NiCommonSemantics::BINORMAL()))
            {
                kGenericSemanticName = NiCommonSemantics::BINORMAL_BP();
            }
            else if (kRendererSemanticName.EqualsNoCase(
                NiCommonSemantics::TANGENT()))
            {
                kGenericSemanticName = NiCommonSemantics::TANGENT_BP();
            }
        }
        m_kAdapterTable.SetGenericSemantic(i, 
            kGenericSemanticName, 
            (NiUInt8)pkInputDescs[i].SemanticIndex);
        m_kAdapterTable.SetRendererSemantic(i, 
            kRendererSemanticName, 
            (NiUInt8)pkInputDescs[i].SemanticIndex);
        NiUInt32 uiComponentCount = 0;
        NiUInt32 uiBit = sizeof(pkInputDescs[i].Mask) * 8;
        bool bContinue = true;
        while (uiBit != 0 && bContinue)
        {
            // Count the number of bits between the 
            // first bit and the last used bit.
            if (pkInputDescs[i].Mask & (1 << --uiBit))
            {
                uiComponentCount = uiBit + 1;
                bContinue = false;
            }
        }
        NIASSERT(bContinue == false && uiComponentCount != 0);
        m_kAdapterTable.SetComponentCount(i, (NiUInt8)uiComponentCount);
    }

    NiFree(pkInputDescs);
}
//---------------------------------------------------------------------------
NiD3D10EffectTechnique::EffectPass::EffectPass(
    NiUInt32 uiVSBufferCount, 
    NiUInt32 uiGSBufferCount, 
    NiUInt32 uiPSBufferCount) :
    m_pkPass(NULL),
    m_pkInputSignature(NULL),
    m_kVSConstantBuffers(uiVSBufferCount, 1),
    m_kGSConstantBuffers(uiGSBufferCount, 1),
    m_kPSConstantBuffers(uiPSBufferCount, 1)
{
    NiD3D10Renderer::D3D10StateBlockMaskDisableAll(&m_kStateBlockMask);
}
//---------------------------------------------------------------------------
NiD3D10EffectTechnique::EffectPass::~EffectPass()
{
    /* */
}
//---------------------------------------------------------------------------
ID3D10EffectPass* NiD3D10EffectTechnique::EffectPass::GetD3D10Pass() const
{
    return m_pkPass;
}
//---------------------------------------------------------------------------
void NiD3D10EffectTechnique::
    EffectPass::SetD3D10Pass(ID3D10EffectPass* pkPass)
{
    m_pkPass = pkPass;
}
//---------------------------------------------------------------------------
ID3D10Blob* NiD3D10EffectTechnique::EffectPass::GetInputSignature() const
{
    return m_pkInputSignature;
}
//---------------------------------------------------------------------------
void NiD3D10EffectTechnique::EffectPass::SetInputSignature(
    ID3D10Blob* pkInputSignature)
{
    if (m_pkInputSignature != pkInputSignature)
    {
        if (m_pkInputSignature)
            m_pkInputSignature->Release();
        m_pkInputSignature = pkInputSignature;
        if (m_pkInputSignature)
            m_pkInputSignature->AddRef();
    }
}
//---------------------------------------------------------------------------
const D3D10_STATE_BLOCK_MASK* 
    NiD3D10EffectTechnique::EffectPass::GetStateBlockMask() const
{
    return &m_kStateBlockMask;
}
//---------------------------------------------------------------------------
void NiD3D10EffectTechnique::EffectPass::SetStateBlockMask(
    const D3D10_STATE_BLOCK_MASK& kMask)
{
    m_kStateBlockMask = kMask;
}
//---------------------------------------------------------------------------
NiUInt32 NiD3D10EffectTechnique::EffectPass::GetVSConstantBufferCount() const
{
    return m_kVSConstantBuffers.GetAllocatedSize();
}
//---------------------------------------------------------------------------
NiUInt32 NiD3D10EffectTechnique::EffectPass::GetGSConstantBufferCount() const
{
    return m_kGSConstantBuffers.GetAllocatedSize();
}
//---------------------------------------------------------------------------
NiUInt32 NiD3D10EffectTechnique::EffectPass::GetPSConstantBufferCount() const
{
    return m_kPSConstantBuffers.GetAllocatedSize();
}
//---------------------------------------------------------------------------
void NiD3D10EffectTechnique::EffectPass::SetVSConstantBufferName(
    NiUInt32 uiIndex, 
    const NiFixedString& kName)
{
    NIASSERT(uiIndex < m_kVSConstantBuffers.GetAllocatedSize());
    m_kVSConstantBuffers.SetAt(uiIndex, kName);
}
//---------------------------------------------------------------------------
void NiD3D10EffectTechnique::EffectPass::SetGSConstantBufferName(
    NiUInt32 uiIndex, 
    const NiFixedString& kName)
{
    NIASSERT(uiIndex < m_kGSConstantBuffers.GetAllocatedSize());
    m_kGSConstantBuffers.SetAt(uiIndex, kName);
}
//---------------------------------------------------------------------------
void NiD3D10EffectTechnique::EffectPass::SetPSConstantBufferName(
    NiUInt32 uiIndex, 
    const NiFixedString& kName)
{
    NIASSERT(uiIndex < m_kPSConstantBuffers.GetAllocatedSize());
    m_kPSConstantBuffers.SetAt(uiIndex, kName);
}
//---------------------------------------------------------------------------
const NiFixedString& 
    NiD3D10EffectTechnique::EffectPass::GetVSConstantBufferName(
    NiUInt32 uiIndex)
{
    NIASSERT(uiIndex < m_kVSConstantBuffers.GetAllocatedSize());
    return m_kVSConstantBuffers.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
const NiFixedString& 
    NiD3D10EffectTechnique::EffectPass::GetGSConstantBufferName(
    NiUInt32 uiIndex)
{
    NIASSERT(uiIndex < m_kGSConstantBuffers.GetAllocatedSize());
    return m_kGSConstantBuffers.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
const NiFixedString& 
    NiD3D10EffectTechnique::EffectPass::GetPSConstantBufferName(
    NiUInt32 uiIndex)
{
    NIASSERT(uiIndex < m_kPSConstantBuffers.GetAllocatedSize());
    return m_kPSConstantBuffers.GetAt(uiIndex);
}
//---------------------------------------------------------------------------
