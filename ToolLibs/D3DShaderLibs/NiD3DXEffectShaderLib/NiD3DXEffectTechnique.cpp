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
//---------------------------------------------------------------------------
// Precompiled Header
#include "NiD3DXEffectShaderLibPCH.h"

#include "NiD3DXEffectTechnique.h"
#include "NiD3DXEffectFile.h"
#include "NiD3DXEffectParameter.h"
#include "NiD3DXEffectShader.h"
#include "NiD3DXEffectStateManager.h"
#include "NiD3DXEffectTechniqueDesc.h"
#include "NiD3DXEffectTechniqueSet.h"

#include <NiD3DUtility.h>

//---------------------------------------------------------------------------
NiD3DXEffectTechnique::NiD3DXEffectTechnique() :
    m_pcTechniqueName(NULL), 
    m_uiImplementation(0), 
    m_pkTechnique(0),
    m_pkFile(NULL),
    m_kParamIter(NULL),
    m_uiNumPasses(0),
    m_abVertexShaderPresent(NULL),
    m_abPixelShaderPresent(NULL),
    m_bTimeParameter(false),
    m_bUsesNiRenderState(false),
    m_bUsesNiLightState(false),
    m_bBlendIndicesAsD3DColor(false),
    m_bSoftwareVPRequired(false),
    m_bSoftwareVPAcceptable(false)

{ /* */ }
//---------------------------------------------------------------------------
NiD3DXEffectTechnique::~NiD3DXEffectTechnique()
{
    NiD3DUtility::Log("Killing NiD3DXEffectTechnique %d\n", 
        m_uiImplementation);

    NiFree(m_pcTechniqueName);
    NiFree(m_abVertexShaderPresent);
    NiFree(m_abPixelShaderPresent);

    // Don't delete parameters - NiD3DXEffectFile owns them
    m_kParameters.RemoveAll();
}
//---------------------------------------------------------------------------
NiD3DXEffectShader* NiD3DXEffectTechnique::GetD3DShader(
    NiD3DRenderer* pkRenderer)
{
    NIASSERT(pkRenderer);

    LPD3DXEFFECT pkD3DXEffect = m_pkFile->GetRenderableEffect();

    if (m_spShader == NULL)
    {
        if (m_pkTechnique == NULL)
            return NULL;

        NIASSERT(m_pkReqDesc);
        unsigned int uiBonesPerPartition = 
            m_pkReqDesc->GetBonesPerPartition();
        NiD3DXEffectShader::BoneMatrixCalcMethod eMethod = 
            NiD3DXEffectShader::BONECALC_SKIN;
        unsigned int uiBoneMatrixRegisters = 4;

        if (uiBonesPerPartition != 0)
        {
            // Skinning shader; search for associated bone array
            NiTListIterator kIter = m_kParameters.GetHeadPos();
            NiD3DXEffectParameter* pkParam = NULL;
            NiD3DShaderConstantMap::DefinedMappings eMapping = 
                NiD3DShaderConstantMap::SCM_DEF_INVALID;
            while (kIter)
            {
                pkParam = m_kParameters.GetNext(kIter);
                if (pkParam == NULL)
                    continue;

                eMapping = pkParam->GetPredefinedMapping();

                if (eMapping < NiD3DShaderConstantMap::SCM_DEF_BONE_MATRIX_3 ||
                    eMapping > 
                        NiD3DShaderConstantMap::SCM_DEF_SKINBONE_MATRIX_4)
                {
                    continue;
                }

                // Possible hit; check array size and presence in shader
                if (uiBonesPerPartition > pkParam->GetArrayCount())
                    continue;

                // GetUsed not sufficient, if FX file was originally 
                // parsed with ID3DXEffectCompiler
                if (pkD3DXEffect->IsParameterUsed(
                    pkParam->GetParameterPtr(), m_pkTechnique) == FALSE)
                {
                    continue;
                }

                // pkParam is the correct parameter
                break;
            }

            if (pkParam)
            {
                if (eMapping == NiD3DShaderConstantMap::SCM_DEF_BONE_MATRIX_3)
                {
                    eMethod = NiD3DXEffectShader::BONECALC_NO_SKIN;
                    uiBoneMatrixRegisters = 3;
                }
                else if (eMapping == 
                    NiD3DShaderConstantMap::SCM_DEF_BONE_MATRIX_4)
                {
                    eMethod = NiD3DXEffectShader::BONECALC_NO_SKIN;
                    uiBoneMatrixRegisters = 4;
                }
                else if (eMapping == 
                    NiD3DShaderConstantMap::SCM_DEF_SKINBONE_MATRIX_3)
                {
                    eMethod = NiD3DXEffectShader::BONECALC_SKIN;
                    uiBoneMatrixRegisters = 3;
                }
                else if (eMapping == 
                    NiD3DShaderConstantMap::SCM_DEF_SKINBONE_MATRIX_4)
                {
                    eMethod = NiD3DXEffectShader::BONECALC_SKIN;
                    uiBoneMatrixRegisters = 4;
                }
            }
        }

        HRESULT hr = pkD3DXEffect->ValidateTechnique(m_pkTechnique);

        if (SUCCEEDED(hr))
        {

#if defined(WIN32)
            pkD3DXEffect->SetStateManager(
                NiD3DXEffectStateManager::GetInstance(pkRenderer));
#endif  // #if defined(WIN32)

            m_spShader = NiNew NiD3DXEffectShader;
            m_spShader->SetEffectTechnique(this);
            m_spShader->SetName(GetShaderName());
            m_spShader->SetImplementation(m_uiImplementation);
            m_spShader->SetActualImplementation(m_uiImplementation);

            m_spShader->SetUsesNiRenderState(m_bUsesNiRenderState);
            m_spShader->SetUsesNiLightState(m_bUsesNiLightState);
            m_spShader->SetConvertBlendIndicesToD3DColor(
                m_bBlendIndicesAsD3DColor);

            if (uiBonesPerPartition != 0)
            {
                m_spShader->SetBoneMatrixRegisters(uiBoneMatrixRegisters);
                m_spShader->SetBoneCalcMethod(eMethod);
            }
        }
    }

    return m_spShader;
}
//---------------------------------------------------------------------------
bool NiD3DXEffectTechnique::ReleaseShader()
{
    bool bRet = (m_spShader != NULL);
    m_spShader = NULL;
    return bRet;
}
//---------------------------------------------------------------------------
void NiD3DXEffectTechnique::Initialize(NiD3DXEffectTechniqueDesc* pkTechDesc)
{
    NIASSERT(pkTechDesc);

    NiD3DUtility::SetString(m_pcTechniqueName, 0,
        pkTechDesc->m_pcTechniqueName);

    m_pkReqDesc = NiNew NiShaderRequirementDesc;
    m_pkReqDesc->SetName(pkTechDesc->m_pcShaderName);
    m_pkReqDesc->SetDescription(pkTechDesc->m_pcDesc);

    m_pkReqDesc->AddPixelShaderVersion(pkTechDesc->m_uiPSVersion);
    m_pkReqDesc->AddVertexShaderVersion(pkTechDesc->m_uiVSVersion);
    m_pkReqDesc->AddUserDefinedVersion(pkTechDesc->m_uiUserVersion);

    m_pkReqDesc->SetPlatformDX9(true);
    m_pkReqDesc->SetPlatformXenon(true);

    m_pkReqDesc->SetBonesPerPartition(pkTechDesc->m_uiBonesPerPartition);
    m_pkReqDesc->SetBinormalTangentMethod(pkTechDesc->m_eNBTMethod);

    m_uiImplementation = pkTechDesc->m_uiImplementation;

    m_bUsesNiRenderState = pkTechDesc->m_bUsesNiRenderState;
    m_bUsesNiLightState  = pkTechDesc->m_bUsesNiLightState;
    m_bBlendIndicesAsD3DColor = pkTechDesc->m_bBlendIndicesAsD3DColor;

    m_bSoftwareVPRequired = pkTechDesc->m_bSoftwareVPRequired;
    m_bSoftwareVPAcceptable = pkTechDesc->m_bSoftwareVPAcceptible;

    m_kAdapterTable = pkTechDesc->m_kAdapterTable;
}
//---------------------------------------------------------------------------
bool NiD3DXEffectTechnique::GetVSPresentAllPasses() const
{
    for (unsigned int i = 0; i < m_uiNumPasses; i++)
    {
        if (m_abVertexShaderPresent[i] == false)
            return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiD3DXEffectTechnique::GetVSPresentAnyPass() const
{
    for (unsigned int i = 0; i < m_uiNumPasses; i++)
    {
        if (m_abVertexShaderPresent[i] == true)
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiD3DXEffectTechnique::GetPSPresentAllPasses() const
{
    for (unsigned int i = 0; i < m_uiNumPasses; i++)
    {
        if (m_abPixelShaderPresent[i] == false)
            return false;
    }
    return true;
}
//---------------------------------------------------------------------------
bool NiD3DXEffectTechnique::GetPSPresentAnyPass() const
{
    for (unsigned int i = 0; i < m_uiNumPasses; i++)
    {
        if (m_abPixelShaderPresent[i] == true)
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiD3DXEffectTechnique::SetTechniquePtr(D3DXTechniquePtr pkTechnique)
{
    m_pkTechnique = pkTechnique;
    if (m_spShader)
        m_spShader->SetTechniquePtr(pkTechnique);
}
//---------------------------------------------------------------------------
bool NiD3DXEffectTechnique::VertexShaderPresent(unsigned int uiPass) const
{
    if (m_abVertexShaderPresent && uiPass < m_uiNumPasses)
        return m_abVertexShaderPresent[uiPass];

    return false;
}
//---------------------------------------------------------------------------
bool NiD3DXEffectTechnique::PixelShaderPresent(unsigned int uiPass) const
{
    if (m_abPixelShaderPresent && uiPass < m_uiNumPasses)
        return m_abPixelShaderPresent[uiPass];

    return false;
}
//---------------------------------------------------------------------------
void NiD3DXEffectTechnique::LinkHandle(LPD3DXEFFECT pkEffect)
{
    D3DXTechniquePtr pkTechnique = 
        pkEffect->GetTechniqueByName(m_pcTechniqueName);
    if (pkTechnique == m_pkTechnique)
        return;

    m_pkTechnique = pkTechnique;
    NIASSERT(m_pkTechnique);

    // Fill in the array of vertex shader flags
    D3DXTECHNIQUE_DESC kTechDesc;
    HRESULT hr = pkEffect->GetTechniqueDesc(m_pkTechnique, &kTechDesc);
    NIASSERT(SUCCEEDED(hr));

    if (m_uiNumPasses < kTechDesc.Passes)
    {
        NiDelete[] m_abVertexShaderPresent;
        m_abVertexShaderPresent = NiAlloc(bool, kTechDesc.Passes);
        NiDelete[] m_abPixelShaderPresent;
        m_abPixelShaderPresent = NiAlloc(bool, kTechDesc.Passes);
        m_uiNumPasses = kTechDesc.Passes;
    }

    D3DXHANDLE hPass;
    D3DXPASS_DESC kDesc;
    for (unsigned int i = 0; i < m_uiNumPasses; i++)
    {
        hPass = pkEffect->GetPass(m_pkTechnique, i);
        hr = pkEffect->GetPassDesc(hPass, &kDesc);
        m_abVertexShaderPresent[i] = 
            (SUCCEEDED(hr)) && (kDesc.pVertexShaderFunction != NULL);
        m_abPixelShaderPresent[i] = 
            (SUCCEEDED(hr)) && (kDesc.pPixelShaderFunction != NULL);
    }
}
//---------------------------------------------------------------------------
void NiD3DXEffectTechnique::DestroyRendererData()
{
    m_pkTechnique = NULL;
    if (m_spShader)
        m_spShader->DestroyRendererData();
}
//---------------------------------------------------------------------------
void NiD3DXEffectTechnique::AddParameter(NiD3DXEffectParameter* pkParam)
{
    m_kParameters.AddTail(pkParam);
    if (pkParam->RequiresTime())
        m_bTimeParameter = true;
}
//---------------------------------------------------------------------------
NiD3DXEffectParameter* NiD3DXEffectTechnique::GetFirstParameter()
{
    m_kParamIter = m_kParameters.GetHeadPos();
    return GetNextParameter();
}
//---------------------------------------------------------------------------
NiD3DXEffectParameter* NiD3DXEffectTechnique::GetNextParameter()
{
    if (m_kParamIter == NULL)
        return NULL;
    return m_kParameters.GetNext(m_kParamIter);
}
//---------------------------------------------------------------------------
NiD3DXEffectParameter* NiD3DXEffectTechnique::GetParameter(
    const char* pcParamName)
{
    NiTListIterator kIter = m_kParameters.GetHeadPos();
    while (kIter)
    {
        NiD3DXEffectParameter* pkParam = m_kParameters.GetNext(kIter);
        if (pkParam && strcmp(pkParam->GetName(), pcParamName) == 0)
            return pkParam;
    }
    return NULL;
}
//---------------------------------------------------------------------------
NiShaderRequirementDesc* NiD3DXEffectTechnique::GetRequirements()
{
    return m_pkReqDesc;
}
//---------------------------------------------------------------------------
