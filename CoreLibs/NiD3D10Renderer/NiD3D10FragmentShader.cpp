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
#include "NiD3D10RendererPCH.h"

#include "NiD3D10FragmentShader.h"
#include "NiD3D10Pass.h"
#include "NiD3D10Renderer.h"
#include "NiD3D10RenderStateManager.h"
#include "NiD3D10ShaderProgramFactory.h"

#include <NiLight.h>
#include <NiMaterialDescriptor.h>
#include <NiShaderDesc.h>
#include <NiShadowGenerator.h>
#include <NiShadowMap.h>
#include <NiStandardMaterial.h>
#include <NiTextureEffect.h>
#include <NiMesh.h>

NiImplementRTTI(NiD3D10FragmentShader, NiD3D10Shader);

//---------------------------------------------------------------------------
NiD3D10FragmentShader::NiD3D10FragmentShader(NiMaterialDescriptor* pkDesc) 
{
    NIASSERT(pkDesc != NULL);
    m_kDescriptor.m_kMatDesc = *pkDesc;
}
//---------------------------------------------------------------------------
NiD3D10FragmentShader::~NiD3D10FragmentShader()
{
    /* */
}
//---------------------------------------------------------------------------
bool NiD3D10FragmentShader::IsGenericallyConfigurable()
{
    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10FragmentShader::AppendRenderPass(unsigned int& uiPassId)
{
    NiD3D10PassPtr spPass;
    NiD3D10Pass::CreateNewPass(spPass);
    NIASSERT(spPass);
    uiPassId = m_kPasses.Add(spPass);

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10FragmentShader::SetAlphaOverride(unsigned int uiPassId, 
    bool bAlphaBlend, bool bUsePreviousSrcBlendMode, 
    NiAlphaProperty::AlphaFunction eSrcBlendMode, 
    bool bUsePreviousDestBlendMode, 
    NiAlphaProperty::AlphaFunction eDestBlendMode)
{
    if (uiPassId >= m_kPasses.GetSize())
        return false;

    NiD3D10Pass* pkPass = m_kPasses.GetAt(uiPassId);
    if (!pkPass)
        return false;

    NiD3D10Pass* pkPreviousPass = NULL;
    if (uiPassId > 0)
        pkPreviousPass = m_kPasses.GetAt(uiPassId - 1);

    NiD3D10RenderStateGroup* pkRSGroup = pkPass->GetRenderStateGroup();
    if (pkRSGroup == NULL)
    {
        pkRSGroup = NiNew NiD3D10RenderStateGroup;
        pkPass->SetRenderStateGroup(pkRSGroup);
    }

    NiD3D10RenderStateGroup* pkPreviousPassRSGroup = NULL;
    if (pkPreviousPass)
        pkPreviousPassRSGroup = pkPreviousPass->GetRenderStateGroup();

    if (bAlphaBlend)
    {
        for (unsigned int i = 0; i < D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT; 
            i++)
        {
            pkRSGroup->SetBSBlendEnable(i, true);
        }
        if (bUsePreviousSrcBlendMode)
        {
            bool bValid = false;
            D3D10_BLEND eSrc = D3D10_BLEND_ZERO;
            if (pkPreviousPassRSGroup)
                bValid = pkPreviousPassRSGroup->GetBSSrcBlend(eSrc);
            if (bValid)
                pkRSGroup->SetBSSrcBlend(eSrc);
            else
                pkRSGroup->RemoveBSSrcBlend();
        }
        else
        {
            pkRSGroup->SetBSSrcBlend(
                NiD3D10RenderStateManager::ConvertGbBlendToD3D10Blend(
                eSrcBlendMode));
        }

        if (bUsePreviousDestBlendMode)
        {
            bool bValid = false;
            D3D10_BLEND eDest = D3D10_BLEND_ZERO;
            if (pkPreviousPassRSGroup)
                bValid = pkPreviousPassRSGroup->GetBSDestBlend(eDest);
            if (bValid)
                pkRSGroup->SetBSDestBlend(eDest);
            else
                pkRSGroup->RemoveBSDestBlend();
        }
        else
        {
            pkRSGroup->SetBSSrcBlend(
                NiD3D10RenderStateManager::ConvertGbBlendToD3D10Blend(
                eDestBlendMode));
        }

        // This function does assume that the operation is add.
        pkRSGroup->SetBSBlendOp(D3D10_BLEND_OP_ADD);
    }
    else
    {
        for (unsigned int i = 0; i < D3D10_SIMULTANEOUS_RENDER_TARGET_COUNT;
            i++)
        {
            pkRSGroup->SetBSBlendEnable(i, false);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10FragmentShader::SetGPUProgram(unsigned int uiPassId, 
    NiGPUProgram* pkProgram, NiGPUProgram::ProgramType& eProgramType)
{
    if (uiPassId >= m_kPasses.GetSize())
        return false;

    NiD3D10Pass* pkPass = m_kPasses.GetAt(uiPassId);
    if (!pkPass)
        return false;

    switch (pkProgram->GetProgramType())
    {
    case NiGPUProgram::PROGRAM_VERTEX:
        pkPass->SetVertexShader((NiD3D10VertexShader*) pkProgram);
        eProgramType = NiGPUProgram::PROGRAM_VERTEX;
        break;
    case NiGPUProgram::PROGRAM_GEOMETRY:
        pkPass->SetGeometryShader((NiD3D10GeometryShader*) pkProgram);
        eProgramType = NiGPUProgram::PROGRAM_GEOMETRY;
        break;
    case NiGPUProgram::PROGRAM_PIXEL:
        pkPass->SetPixelShader((NiD3D10PixelShader*) pkProgram);
        eProgramType = NiGPUProgram::PROGRAM_PIXEL;
        break;
    }
    return true;
}
//---------------------------------------------------------------------------
NiShaderConstantMap* NiD3D10FragmentShader::CreateShaderConstantMap(
    unsigned int uiPassId, NiGPUProgram::ProgramType eProgramType,
    unsigned int uiMapIndex)
{
    if (uiPassId >= m_kPasses.GetSize())
        return false;

    NiD3D10Pass* pkPass = m_kPasses.GetAt(uiPassId);
    if (!pkPass)
        return false;

    NiD3D10ShaderConstantMap* pkSCM = NULL;
    if (eProgramType == NiGPUProgram::PROGRAM_VERTEX)
    {
        pkSCM = pkPass->GetVertexConstantMap(uiMapIndex);

        if (!pkSCM)
        {
            pkSCM = NiNew NiD3D10ShaderConstantMap(eProgramType);
            pkPass->SetVertexConstantMap(uiMapIndex, pkSCM);
        }
    }
    else if (eProgramType == NiGPUProgram::PROGRAM_GEOMETRY)
    {
        pkSCM = pkPass->GetGeometryConstantMap(uiMapIndex);
        if (!pkSCM)
        {
            pkSCM = NiNew NiD3D10ShaderConstantMap(eProgramType);
            pkPass->SetGeometryConstantMap(uiMapIndex, pkSCM);
        }
    }
    else if (eProgramType == NiGPUProgram::PROGRAM_PIXEL)
    {
        pkSCM = pkPass->GetPixelConstantMap(uiMapIndex);
        if (!pkSCM)
        {
            pkSCM = NiNew NiD3D10ShaderConstantMap(eProgramType);
            pkPass->SetPixelConstantMap(uiMapIndex, pkSCM);
        }
    }

    return pkSCM;
}
//---------------------------------------------------------------------------
bool NiD3D10FragmentShader::AppendTextureSampler(unsigned int uiPassId, 
    unsigned int& uiSamplerId, const NiFixedString& kSemantic,
    const NiFixedString& kVariableName, unsigned int uiInstance)
{
    if (uiPassId >= m_kPasses.GetSize())
        return false;

    NiD3D10Pass* pkPass = m_kPasses.GetAt(uiPassId);
    if (!pkPass)
        return false;

    unsigned int uiTextureId = pkPass->GetFirstUnassignedTexture();

    unsigned int uiGBTextureFlag = 0;
    if (NiTexturingProperty::GetMapIDFromName(kSemantic, uiGBTextureFlag))
    {
        switch(uiGBTextureFlag)
        {
        case NiTexturingProperty::BASE_INDEX:
            pkPass->SetGamebryoMap(uiTextureId, kVariableName, 
                NiD3D10Pass::GB_MAP_BASE, 0, 0);
            break;
        case NiTexturingProperty::DARK_INDEX:
            pkPass->SetGamebryoMap(uiTextureId, kVariableName, 
                NiD3D10Pass::GB_MAP_DARK, 0, 0);
            break;
        case NiTexturingProperty::DETAIL_INDEX:
            pkPass->SetGamebryoMap(uiTextureId, kVariableName, 
                NiD3D10Pass::GB_MAP_DETAIL, 0, 0);
            break;
        case NiTexturingProperty::GLOSS_INDEX:
            pkPass->SetGamebryoMap(uiTextureId, kVariableName, 
                NiD3D10Pass::GB_MAP_GLOSS, 0, 0);
            break;
        case NiTexturingProperty::GLOW_INDEX:
            pkPass->SetGamebryoMap(uiTextureId, kVariableName, 
                NiD3D10Pass::GB_MAP_GLOW, 0, 0);
            break;
        case NiTexturingProperty::BUMP_INDEX:
            pkPass->SetGamebryoMap(uiTextureId, kVariableName, 
                NiD3D10Pass::GB_MAP_BUMP, 0, 0);
            break;
        case NiTexturingProperty::NORMAL_INDEX:
            pkPass->SetGamebryoMap(uiTextureId, kVariableName, 
                NiD3D10Pass::GB_MAP_NORMAL, 0, 0);
            break;
        case NiTexturingProperty::PARALLAX_INDEX:
            pkPass->SetGamebryoMap(uiTextureId, kVariableName, 
                NiD3D10Pass::GB_MAP_PARALLAX, 0, 0);
            break;
        case NiTexturingProperty::DECAL_BASE:
            pkPass->SetGamebryoMap(uiTextureId, kVariableName, 
                NiD3D10Pass::GB_MAP_DECAL, (unsigned short)uiInstance, 0);
            break;
        case NiTexturingProperty::SHADER_BASE:
            pkPass->SetGamebryoMap(uiTextureId, kVariableName, 
                NiD3D10Pass::GB_MAP_SHADER, (unsigned short)uiInstance, 0);
            break;
        default:
            NIASSERT(!"Unknown map entry!");
            return false;
            break;
        }
    }
    else if (NiTextureEffect::GetTypeIDFromName(kSemantic, uiGBTextureFlag))
    {
        unsigned short usObjectTextureFlags = 0;
        switch((NiTextureEffect::TextureType) uiGBTextureFlag)
        {
        case NiTextureEffect::PROJECTED_LIGHT:
            usObjectTextureFlags = static_cast<unsigned short>(
                (usObjectTextureFlags & ~NiD3D10Pass::OTF_TYPE_MASK) |
                (NiShaderAttributeDesc::OT_EFFECT_PROJECTEDLIGHTMAP << 
                NiD3D10Pass::OTF_TYPE_SHIFT));
            break;
        case NiTextureEffect::PROJECTED_SHADOW:
            usObjectTextureFlags = static_cast<unsigned short>(
                (usObjectTextureFlags & ~NiD3D10Pass::OTF_TYPE_MASK) |
                (NiShaderAttributeDesc::OT_EFFECT_PROJECTEDSHADOWMAP << 
                NiD3D10Pass::OTF_TYPE_SHIFT));
            break;
        case NiTextureEffect::ENVIRONMENT_MAP:
            usObjectTextureFlags = static_cast<unsigned short>(
                (usObjectTextureFlags & ~NiD3D10Pass::OTF_TYPE_MASK) |
                (NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP << 
                NiD3D10Pass::OTF_TYPE_SHIFT));
            break;
        case NiTextureEffect::FOG_MAP:
            usObjectTextureFlags = static_cast<unsigned short>(
                (usObjectTextureFlags & ~NiD3D10Pass::OTF_TYPE_MASK) |
                (NiShaderAttributeDesc::OT_EFFECT_FOGMAP << 
                NiD3D10Pass::OTF_TYPE_SHIFT));
            break;
        default:
            NIASSERT(!"Unknown texture effect type");
            break;
        }

        usObjectTextureFlags = static_cast<unsigned short>(
            (usObjectTextureFlags & ~NiD3D10Pass::OTF_INDEX_MASK) | 
            uiInstance);

        pkPass->SetGamebryoMap(uiTextureId, kVariableName, 
            0, 0, usObjectTextureFlags);
    }
    else if (NiShadowMap::GetLightTypeFromName(kSemantic, uiGBTextureFlag))
    {
        unsigned short usObjectTextureFlags = 0;
        switch ((NiStandardMaterial::LightType)uiGBTextureFlag)
        {
        case NiStandardMaterial::LIGHT_DIR:
            usObjectTextureFlags = static_cast<unsigned short>(
                (usObjectTextureFlags & ~NiD3D10Pass::OTF_TYPE_MASK) |
                (NiShaderAttributeDesc::OT_EFFECT_DIRSHADOWMAP << 
                NiD3D10Pass::OTF_TYPE_SHIFT));
            break;
        case NiStandardMaterial::LIGHT_SPOT:
            usObjectTextureFlags = static_cast<unsigned short>(
                (usObjectTextureFlags & ~NiD3D10Pass::OTF_TYPE_MASK) |
                (NiShaderAttributeDesc::OT_EFFECT_SPOTSHADOWMAP << 
                NiD3D10Pass::OTF_TYPE_SHIFT));
            break;
        case NiStandardMaterial::LIGHT_POINT:
            usObjectTextureFlags = static_cast<unsigned short>(
                (usObjectTextureFlags & ~NiD3D10Pass::OTF_TYPE_MASK) |
                (NiShaderAttributeDesc::OT_EFFECT_POINTSHADOWMAP << 
                NiD3D10Pass::OTF_TYPE_SHIFT));
            break;
        default:
            NIASSERT(!"Unknown light type");
            break;
        };

        usObjectTextureFlags = static_cast<unsigned short>(
            (usObjectTextureFlags & ~NiD3D10Pass::OTF_INDEX_MASK) | 
            uiInstance);

        pkPass->SetGamebryoMap(uiTextureId, kVariableName, 
            0, 0, usObjectTextureFlags);
    }

    // Currently, the sampler and the texture share the same name.
    uiSamplerId = uiTextureId;
    pkPass->SetSamplerName(uiSamplerId, kVariableName);

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10FragmentShader::Initialize()
{
    if (m_bInitialized)
        return true;

    // All bone matrices will be transposed, 3-bone matrices in world space
    SetBoneParameters(true, 3, true);

    if (!NiD3D10Shader::Initialize())
        return false;

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10FragmentShader::UpdatePipeline(
    const NiRenderCallContext& kRCC)
{
    for (unsigned int uiPass = 0; uiPass < m_kPasses.GetSize(); uiPass++)
    {
        NiD3D10Pass* pkPass = m_kPasses.GetAt(uiPass);
        if (!pkPass)
        {
            continue;
        }

        const NiTexturingProperty* pkTexProp = kRCC.m_pkState->GetTexturing();

        const unsigned int uiSamplerCount = pkPass->GetActiveSamplerCount();
        NIASSERT(uiSamplerCount == pkPass->GetActiveTextureCount());

        for (unsigned int i = 0; i < uiSamplerCount; i++)
        {
            PrepareTexture(kRCC, i, pkPass, pkTexProp);
        }
    }

    return NiD3D10Shader::UpdatePipeline(kRCC);
}
//---------------------------------------------------------------------------
void NiD3D10FragmentShader::PrepareTexture(
    const NiRenderCallContext& kRCC,
    unsigned int uiTexture,
    NiD3D10Pass* pkPass,
    const NiTexturingProperty* pkTexProp)
{
    NI_UNUSED_ARG(pkTexProp);
    NIASSERT(pkPass != 0 && uiTexture < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT);

    // The provided NiTexturingProperty had better match what's in the 
    // property state object.
    NIASSERT(kRCC.m_pkState != NULL &&
        kRCC.m_pkState->GetTexturing() == pkTexProp);

    // For the time being, assume that the texture ID and the sampler ID
    // correspond directly.

    NiTexture* pkTexture = NULL;
    NiTexturingProperty::ClampMode eClampMode =
        NiTexturingProperty::CLAMP_S_CLAMP_T;
    NiTexturingProperty::FilterMode eFilterMode =
        NiTexturingProperty::FILTER_NEAREST;
    unsigned short usMaxAnisotropy = 1;

    NiUInt32 uiGBMapFlags = pkPass->GetGamebryoMapFlags(uiTexture);
    NiUInt16 uiObjectTextureFlags = pkPass->GetObjectTextureFlags(uiTexture);

    ObtainTexture(uiGBMapFlags, 
        uiObjectTextureFlags, 
        kRCC, 
        pkTexture, 
        eClampMode, 
        eFilterMode,
        usMaxAnisotropy);

    // Even if the texture is NULL, we need to set that here to prevent a 
    // stale texture from getting used by accident.
    pkPass->SetPixelTexture(uiTexture, pkTexture);

    NiD3D10RenderStateGroup* pkRSGroup = pkPass->GetRenderStateGroup();
    if (pkRSGroup == NULL)
    {
        pkRSGroup = NiNew NiD3D10RenderStateGroup;
        pkPass->SetRenderStateGroup(pkRSGroup);
    }
    pkRSGroup->SetSamplerFilter(
        NiGPUProgram::PROGRAM_PIXEL, 
        uiTexture,
        NiD3D10RenderStateManager::ConvertGbFilterModeToD3D10Filter(eFilterMode));

    pkRSGroup->SetSamplerMaxAnisotropy(NiGPUProgram::PROGRAM_PIXEL, uiTexture, usMaxAnisotropy);

    bool bMipmapEnable = 
        NiD3D10RenderStateManager::ConvertGbFilterModeToMipmapEnable(
        eFilterMode);

    float fMaxLOD = (bMipmapEnable ? D3D10_FLOAT32_MAX : 0.0f);
    pkRSGroup->SetSamplerMaxLOD(NiGPUProgram::PROGRAM_PIXEL, uiTexture, 
        fMaxLOD);

    pkRSGroup->SetSamplerAddressU(NiGPUProgram::PROGRAM_PIXEL, uiTexture,
        NiD3D10RenderStateManager::ConvertGbClampModeToD3D10AddressU(
        eClampMode));

    pkRSGroup->SetSamplerAddressV(NiGPUProgram::PROGRAM_PIXEL, uiTexture,
        NiD3D10RenderStateManager::ConvertGbClampModeToD3D10AddressV(
        eClampMode));
}
//---------------------------------------------------------------------------
const NiShader::NiShaderInstanceDescriptor* 
    NiD3D10FragmentShader::GetShaderInstanceDesc() const
{
    return &m_kDescriptor;
}
//---------------------------------------------------------------------------
