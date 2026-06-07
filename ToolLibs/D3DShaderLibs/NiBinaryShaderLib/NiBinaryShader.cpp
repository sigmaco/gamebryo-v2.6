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
#include "NiBinaryShaderLibPCH.h"

#include "NiBinaryShader.h"
#include "NSBUtility.h"

#include <NiD3DShaderProgramFactory.h>
#include <NiShaderDesc.h>
#include <NiShadowGenerator.h>
#include <NiShadowMap.h>
#include <NiTextureStage.h>
#include <NiTimeSyncController.h>

NiImplementRTTI(NiBinaryShader, NiD3DShader);

//---------------------------------------------------------------------------
// Helper functions
static inline unsigned int GetSamplerValue(
    unsigned int uiState,
    unsigned int uiValue,
    NiTexturingProperty::ClampMode eClampMode,
    NiTexturingProperty::FilterMode eFilterMode)
{
    NiD3DRenderer* pkD3DRenderer =
        NiVerifyStaticCast(NiD3DRenderer, NiRenderer::GetRenderer());
    (void)pkD3DRenderer;
    switch (uiState)
    {
        case NiD3DRenderState::NISAMP_ADDRESSU:
            return pkD3DRenderer->GetD3DClampMode(eClampMode).m_eU;
        case NiD3DRenderState::NISAMP_ADDRESSV:
            return pkD3DRenderer->GetD3DClampMode(eClampMode).m_eV;
        case NiD3DRenderState::NISAMP_MAGFILTER:
            return pkD3DRenderer->GetD3DFilterMode(eFilterMode).m_eMag;
        case NiD3DRenderState::NISAMP_MINFILTER:
            return pkD3DRenderer->GetD3DFilterMode(eFilterMode).m_eMin;
        case NiD3DRenderState::NISAMP_MIPFILTER:
            return pkD3DRenderer->GetD3DFilterMode(eFilterMode).m_eMip;
        default:
            // Invalid sampler state.
            NIASSERT(false);
            return uiValue;
    }
}
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
NiBinaryShader::NiBinaryShader() :
    NiD3DShader(),
    m_uiBonesPerPartition(0), 
    m_uiBoneMatrixRegisters(0), 
    m_eBoneCalcMethod(BONECALC_SKIN),
    m_eBinormalTangentMethod(NiShaderRequirementDesc::NBT_METHOD_NONE),
    m_uiBinormalTangentUVSource(NiShaderDesc::BINORMALTANGENTUVSOURCEDEFAULT)
{
    for (unsigned int ui = 0; ui < m_kPasses.GetAllocatedSize(); ui++)
        m_kPasses.SetAt(ui, 0);
}
//---------------------------------------------------------------------------
NiBinaryShader::~NiBinaryShader()
{
}
//---------------------------------------------------------------------------
bool NiBinaryShader::Initialize()
{
    if (m_bInitialized)
        return true;

    if (!NiD3DShader::Initialize())
        return false;

    // Cycle through the passes and load the shader programs.  That step
    // should be all that's needed at this point.
    NiD3DPass* pkPass;
    unsigned int uiCount = m_kPasses.GetSize();
    for (unsigned int ui = 0; ui < uiCount; ui++)
    {
        pkPass = m_kPasses.GetAt(ui);
        if (pkPass)
        {
            if (!LoadVertexShaderProgram(*pkPass))
            {
                // Implement a failure response.
            }
            if (!LoadGeometryShaderProgram(*pkPass))
            {
                // Implement a failure response.
            }
            if (!LoadPixelShaderProgram(*pkPass))
            {
                // Implement a failure response.
            }
        }
    }

    return m_bInitialized;
}
//---------------------------------------------------------------------------
unsigned int NiBinaryShader::UpdatePipeline(const NiRenderCallContext& kRCC)
{
    for (unsigned int uiPass = 0; uiPass < m_kPasses.GetSize(); uiPass++)
    {
        NiPlatformShaderPass* pkCurPass = m_kPasses.GetAt(uiPass);
        if (!pkCurPass)
        {
            continue;
        }

        for (unsigned int uiStage = 0; uiStage < pkCurPass->GetStageCount();
            uiStage++)
        {
            NiPlatformTextureSampler* pkCurStage =
                pkCurPass->GetStage(uiStage);
            if (!pkCurStage)
            {
                continue;
            }
            PrepareTextureStage(pkCurStage, kRCC);
        }
    }

    return NiD3DShader::UpdatePipeline(kRCC);
}
//---------------------------------------------------------------------------
unsigned int NiBinaryShader::SetupTransformations(
    const NiRenderCallContext& kRCC)
{
    // Set if there is a vertex shader
    NiPlatformShaderPass* pkPass = m_kPasses.GetAt(m_uiCurrentPass);
    if (pkPass && pkPass->GetVertexShader())
    {
        NIASSERT(kRCC.m_pkWorld);
        const NiTransform& kWorld = *kRCC.m_pkWorld;

        // This has to be done each pass so that when multipass
        // objects are being batch rendered, the martices are
        // updated properly! We may want to have some sort of
        // flag indicate that a batch is in progress, so that
        // we don't do this every pass for non-batched objects.
        m_pkD3DRenderer->SetModelTransform(kWorld, false);
        return 0;
    }
    else
    {
        return NiD3DShader::SetupTransformations(kRCC);
    }
}
//---------------------------------------------------------------------------
bool NiBinaryShader::SetupGeometry(NiRenderObject* pkMesh,
    NiMaterialInstance* pkMaterialInstance)
{
    if (!pkMesh)
        return NULL;

    //-----------------------------------------------------------------------
    // Add a 'dummy' alpha property if one is needed
    NiAlphaProperty* pkAlphaProperty = 0;
    unsigned int uiValue;
    bool bSave;

    // Check the renderstate group for alphablend/alphatest
    if (m_pkRenderStateGroup)
    {
        if (m_pkRenderStateGroup->GetRenderState(
            D3DRS_ALPHABLENDENABLE, uiValue, bSave))
        {
            // If it's enabled, set it in the property
            if (uiValue == 1)
            {
                if (!pkAlphaProperty)
                {
                    pkAlphaProperty = NiNew NiAlphaProperty();
                    NIASSERT(pkAlphaProperty);
                }
                pkAlphaProperty->SetAlphaBlending(true);
            }
        }

        if (m_pkRenderStateGroup->GetRenderState(
            D3DRS_ALPHATESTENABLE, uiValue, bSave))
        {
            if (uiValue == 1)
            {
                if (!pkAlphaProperty)
                {
                    pkAlphaProperty = NiNew NiAlphaProperty();
                    NIASSERT(pkAlphaProperty);
                }
                pkAlphaProperty->SetAlphaTesting(true);
            }
        }
    }

    // Check the first pass afterwards, as it can override the global
    NiD3DPass* pkPass = m_kPasses.GetAt(0);
    if (pkPass)
    {
        if (pkPass->GetRenderState(D3DRS_ALPHABLENDENABLE, uiValue, bSave))
        {
            // If it's enabled, set it in the property
            if (uiValue == 1)
            {
                if (!pkAlphaProperty)
                {
                    pkAlphaProperty = NiNew NiAlphaProperty();
                    NIASSERT(pkAlphaProperty);
                }
                pkAlphaProperty->SetAlphaBlending(true);
            }
        }

        if (pkPass->GetRenderState(D3DRS_ALPHATESTENABLE, uiValue, bSave))
        {
            if (uiValue == 1)
            {
                if (!pkAlphaProperty)
                {
                    pkAlphaProperty = NiNew NiAlphaProperty();
                    NIASSERT(pkAlphaProperty);
                }
                pkAlphaProperty->SetAlphaTesting(true);
            }
        }
    }

    if (pkAlphaProperty)
    {
        NiAlphaProperty* pkAttached =
            (NiAlphaProperty*)pkMesh->GetProperty(NiProperty::ALPHA);
        if (pkAttached)
        {
            if (!pkAttached->GetAlphaBlending() &&
                !pkAttached->GetAlphaTesting())
            {
                pkMesh->RemoveProperty(NiProperty::ALPHA);
                pkMesh->AttachProperty(pkAlphaProperty);
            }
            else
            {
                NiDelete pkAlphaProperty;
            }
        }
        else
        {
            pkMesh->AttachProperty(pkAlphaProperty);
        }
    }

    //-----------------------------------------------------------------------
    // See if there is a 'time' attribute required
    bool bTimeRequired = false;

    // Cycle over each pass and check the contant maps for occurances of one
    // of the time-based pre-defines.
    // First, check the 'global' attributes
    if (m_spPixelConstantMap || m_spVertexConstantMap)
    {
        // Pixel shader map
        if (m_spPixelConstantMap &&
            (m_spPixelConstantMap->GetEntry("time") ||
             m_spPixelConstantMap->GetEntry("sin_time") ||
             m_spPixelConstantMap->GetEntry("cos_time") ||
             m_spPixelConstantMap->GetEntry("tan_time") ||
             m_spPixelConstantMap->GetEntry("time_sin_cos_tan")))
        {
            bTimeRequired = true;
        }
        // Vertex shader map
        if (m_spVertexConstantMap &&
            (m_spVertexConstantMap->GetEntry("time") ||
             m_spVertexConstantMap->GetEntry("sin_time") ||
             m_spVertexConstantMap->GetEntry("cos_time") ||
             m_spVertexConstantMap->GetEntry("tan_time") ||
             m_spVertexConstantMap->GetEntry("time_sin_cos_tan")))
        {
            bTimeRequired = true;
        }
    }

    if (!bTimeRequired)
    {
        // Check each pass
        for (unsigned int ui = 0; ui < m_kPasses.GetSize(); ui++)
        {
            pkPass = m_kPasses.GetAt(ui);
            if (pkPass)
            {
                NiPlatformShaderConstantMap* pkPixelConstantMap =
                    pkPass->GetPixelConstantMap();
                NiPlatformShaderConstantMap* pkVertexConstantMap =
                    pkPass->GetVertexConstantMap();
                // Pixel shader map
                if (pkPixelConstantMap &&
                    (pkPixelConstantMap->GetEntry("time") ||
                     pkPixelConstantMap->GetEntry("sin_time") ||
                     pkPixelConstantMap->GetEntry("cos_time") ||
                     pkPixelConstantMap->GetEntry("tan_time") ||
                     pkPixelConstantMap->GetEntry("time_sin_cos_tan")))
                {
                    bTimeRequired = true;
                }
                // Vertex shader map
                if (pkVertexConstantMap &&
                    (pkVertexConstantMap->GetEntry("time") ||
                     pkVertexConstantMap->GetEntry("sin_time") ||
                     pkVertexConstantMap->GetEntry("cos_time") ||
                     pkVertexConstantMap->GetEntry("tan_time") ||
                     pkVertexConstantMap->GetEntry("time_sin_cos_tan")))
                {
                    bTimeRequired = true;
                }
            }

            if (bTimeRequired)
                break;
        }
    }

    if (bTimeRequired)
    {
        NiTimeSyncController* pkTimeCtrlr = NiNew NiTimeSyncController();
        NIASSERT(pkTimeCtrlr);
        pkTimeCtrlr->SetTarget(pkMesh);

        // Ensure object will be updated by UpdateSelected.
        NiAVObject* pkObject = pkMesh;
        while (pkObject != NULL)
        {
            pkObject->SetSelectiveUpdate(true);
            pkObject = (NiAVObject*)(pkObject->GetParent());
        }
    }

    return NiD3DShader::SetupGeometry(pkMesh, pkMaterialInstance);
}
//---------------------------------------------------------------------------
bool NiBinaryShader::LoadVertexShaderProgram(NiD3DPass& kPass)
{
    const char* pcProgramFileName = kPass.GetVertexShaderProgramFileName();

    // If there is no program to load, it's not a failure.
    // Just return true!
    if ((pcProgramFileName == NULL) || (*pcProgramFileName == '\0'))
        return true;

    char acTrueFileName[_MAX_PATH];
    if (!ResolveVertexShaderFileName(pcProgramFileName, acTrueFileName,
        _MAX_PATH))
    {
        return false;
    }

    // Construct a shader name from the shader file and entry point
    const char* pcShaderEntryPoint = 
        kPass.GetVertexShaderProgramEntryPoint();
    char acShaderName[_MAX_PATH];
    if (pcShaderEntryPoint != NULL && *pcShaderEntryPoint != '\0')
    {
        NIASSERT(strlen(pcProgramFileName) + strlen(pcShaderEntryPoint) + 2 
            < _MAX_PATH);
        NiSprintf(acShaderName, _MAX_PATH, "%s##%s", pcProgramFileName, 
            pcShaderEntryPoint);
    }
    else
    {
        NiStrcpy(acShaderName, _MAX_PATH, pcProgramFileName);
    }

    // Passing NULL as the NiShader declaration, as the value passed in is
    // never used
    NiD3DVertexShader* pkVS = 
        NiD3DShaderProgramFactory::CreateVertexShaderFromFile(acTrueFileName, 
        acShaderName, pcShaderEntryPoint, 
        kPass.GetVertexShaderProgramShaderTarget(), 
        NULL, 0);

    if (!pkVS)
        return false;

    kPass.SetVertexShader(pkVS);

    return true;
}
//---------------------------------------------------------------------------
bool NiBinaryShader::LoadGeometryShaderProgram(NiD3DPass&)
{
    NIASSERT(NiIsKindOf(NiD3DRenderer, NiRenderer::GetRenderer()));
    return false;
}
//---------------------------------------------------------------------------
bool NiBinaryShader::LoadPixelShaderProgram(NiD3DPass& kPass)
{
    const char* pcProgramFileName = kPass.GetPixelShaderProgramFileName();

    // If there is no program to load, it's not a failure.
    // Just return true!
    if (!pcProgramFileName || (strcmp(pcProgramFileName, "") == 0))
        return true;

    char acTrueFileName[_MAX_PATH];
    if (!ResolvePixelShaderFileName(pcProgramFileName, acTrueFileName,
        _MAX_PATH))
    {
        return false;
    }

    // Construct a shader name from the shader file and entry point
    const char* pcShaderEntryPoint = 
        kPass.GetPixelShaderProgramEntryPoint();
    char acShaderName[_MAX_PATH];
    if (pcShaderEntryPoint != NULL && *pcShaderEntryPoint != '\0')
    {
        NIASSERT(strlen(pcProgramFileName) + strlen(pcShaderEntryPoint) + 2 
            < _MAX_PATH);
        NiSprintf(acShaderName, _MAX_PATH, "%s##%s", pcProgramFileName, 
            pcShaderEntryPoint);
    }
    else
    {
        NiStrcpy(acShaderName, _MAX_PATH, pcProgramFileName);
    }

    NiD3DPixelShader* pkPS = 
        NiD3DShaderProgramFactory::CreatePixelShaderFromFile(acTrueFileName, 
        acShaderName, pcShaderEntryPoint, 
        kPass.GetPixelShaderProgramShaderTarget());
    if (!pkPS)
        return false;

    kPass.SetPixelShader(pkPS);

    return true;
}
//---------------------------------------------------------------------------
bool NiBinaryShader::ResolveVertexShaderFileName(const char* pcOriginalName,
    char* pcTrueName, unsigned int uiNameLength)
{
    char acDrive[_MAX_DRIVE];
    char acDir[_MAX_PATH];
    char acFileName[_MAX_PATH];
    char acFileExt[_MAX_EXT];

    NiD3DUtility::GetSplitPath(pcOriginalName, acDrive, acDir, acFileName, 
        acFileExt);

    if (acFileExt[0] == '\0')
    {
        NiSprintf(pcTrueName, uiNameLength, "%s.vsh", acFileName);
    }
    else
    {
        NiSprintf(pcTrueName, uiNameLength, "%s%s", acFileName, acFileExt);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiBinaryShader::ResolveGeometryShaderFileName(const char* pcOriginalName,
    char* pcTrueName, unsigned int uiNameLength)
{
    char acDrive[_MAX_DRIVE];
    char acDir[_MAX_PATH];
    char acFileName[_MAX_PATH];
    char acFileExt[_MAX_EXT];

    NiD3DUtility::GetSplitPath(pcOriginalName, acDrive, acDir, acFileName, 
        acFileExt);

    if (acFileExt[0] == '\0')
    {
        NiSprintf(pcTrueName, uiNameLength, "%s.gsh", acFileName);
    }
    else
    {
        NiSprintf(pcTrueName, uiNameLength, "%s%s", acFileName, acFileExt);
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiBinaryShader::ResolvePixelShaderFileName(const char* pcOriginalName,
    char* pcTrueName, unsigned int uiNameLength)
{
    char acDrive[_MAX_DRIVE];
    char acDir[_MAX_PATH];
    char acFileName[_MAX_PATH];
    char acFileExt[_MAX_EXT];

    NiD3DUtility::GetSplitPath(pcOriginalName, acDrive, acDir, acFileName, 
        acFileExt);

    if (acFileExt[0] == '\0')
    {
        NiSprintf(pcTrueName, uiNameLength, "%s.psh", acFileName);
    }
    else
    {
        NiSprintf(pcTrueName, uiNameLength, "%s%s", acFileName, acFileExt);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiBinaryShader::PrepareTextureStage(NiPlatformTextureSampler* pkStage,
    const NiRenderCallContext& kRCC)
{
    const NiDynamicEffectState* pkEffects = kRCC.m_pkEffects;

    NiRenderObject* pkMesh = kRCC.m_pkMesh;

    NiTexture* pkTexture = NULL;
    NiTexturingProperty::ClampMode eClampMode =
        NiTexturingProperty::CLAMP_S_CLAMP_T;
    NiTexturingProperty::FilterMode eFilterMode =
        NiTexturingProperty::FILTER_NEAREST;

    unsigned int uiTextureFlags = pkStage->GetTextureFlags();
    unsigned short usObjTextureFlags = pkStage->GetObjTextureFlags();

    if (usObjTextureFlags == NiTextureStage::TSOTF_IGNORE)
    {
        if (uiTextureFlags != NiTextureStage::TSTF_IGNORE)
        {
            const NiTexturingProperty::Map* pkMap = NULL;
            const NiTexturingProperty* pkTexProp =
                kRCC.m_pkState->GetTexturing();
            NIASSERT(pkTexProp);
            if ((uiTextureFlags & NiTextureStage::TSTF_MAP_MASK) == 0)
            {
                switch (uiTextureFlags & NiTextureStage::TSTF_NDL_TYPEMASK)
                {
                case NiTextureStage::TSTF_NONE:
                    break;
                case NiTextureStage::TSTF_NDL_BASE:
                    pkMap = pkTexProp->GetBaseMap();
                    break;
                case NiTextureStage::TSTF_NDL_DARK:
                    pkMap = pkTexProp->GetDarkMap();
                    break;
                case NiTextureStage::TSTF_NDL_DETAIL:
                    pkMap = pkTexProp->GetDetailMap();
                    break;
                case NiTextureStage::TSTF_NDL_GLOSS:
                    pkMap = pkTexProp->GetGlossMap();
                    break;
                case NiTextureStage::TSTF_NDL_GLOW:
                    pkMap = pkTexProp->GetGlowMap();
                    break;
                case NiTextureStage::TSTF_NDL_BUMP:
                    pkMap = pkTexProp->GetBumpMap();
                    break;
                case NiTextureStage::TSTF_NDL_NORMAL:
                    pkMap = pkTexProp->GetNormalMap();
                    break;
                case NiTextureStage::TSTF_NDL_PARALLAX:
                    pkMap = pkTexProp->GetParallaxMap();
                    break;
                }
            }
            else if ((uiTextureFlags & NiTextureStage::TSTF_MAP_MASK) ==
                NiTextureStage::TSTF_MAP_DECAL)
            {
                unsigned int uiIndex = uiTextureFlags &
                    NiTextureStage::TSTF_INDEX_MASK;
                if (uiIndex < pkTexProp->GetDecalArrayCount())
                {
                    pkMap = pkTexProp->GetDecalMap(uiIndex);
                }
            }
            else if ((uiTextureFlags & NiTextureStage::TSTF_MAP_MASK) ==
                NiTextureStage::TSTF_MAP_SHADER)
            {
                unsigned int uiIndex = uiTextureFlags &
                    NiTextureStage::TSTF_INDEX_MASK;
                if (uiIndex < pkTexProp->GetShaderArrayCount())
                {
                    pkMap = pkTexProp->GetShaderMap(uiIndex);
                }
            }

            if (pkMap)
            {
                pkTexture = pkMap->GetTexture();
                eClampMode = pkMap->GetClampMode();
                eFilterMode = pkMap->GetFilterMode();
                // What else is needed?

#if !defined(_PS3)
                // If we are to use the index from the map, set that now
                // as well
                if ((uiTextureFlags & NiTextureStage::TSTF_MAP_USE_MASK) == 
                    NiTextureStage::TSTF_MAP_USE_INDEX)
                {
                    unsigned int uiTexCoordIndex = pkMap->GetTextureIndex();

                    // We need to take the transformation info into account
                    uiTexCoordIndex |= pkStage->GetTexCoordIndex() &
                        ~NiTextureStage::TSTF_INDEX_MASK;
                    
                    pkStage->SetStageState(D3DTSS_TEXCOORDINDEX,
                        uiTexCoordIndex, pkStage->GetSaveTexCoordIndex());
                }
#endif
            }
        }
    }
    else
    {
        NiTextureEffect* pkTextureEffect = NULL;
        NiShadowGenerator* pkGenerator = NULL;
        unsigned short usObjectIndex = (unsigned short)(usObjTextureFlags &
            NiTextureStage::TSOTF_INDEX_MASK);
        switch ((usObjTextureFlags & NiTextureStage::TSOTF_TYPE_MASK) >>
            NiTextureStage::TSOTF_TYPE_SHIFT)
        {
            case NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP:
                pkTextureEffect = pkEffects->GetEnvironmentMap();
                break;
            case NiShaderAttributeDesc::OT_EFFECT_PROJECTEDSHADOWMAP:
            {
                unsigned short usIndex = 0;
                NiDynEffectStateIter kIter =
                    pkEffects->GetProjShadowHeadPos();
                while (kIter)
                {
                    NiTextureEffect* pkProjShadow =
                        pkEffects->GetNextProjShadow(kIter);
                    if (usIndex++ == usObjectIndex)
                    {
                        pkTextureEffect = pkProjShadow;
                        break;
                    }
                }
                break;
            }
            case NiShaderAttributeDesc::OT_EFFECT_PROJECTEDLIGHTMAP:
            {
                unsigned short usIndex = 0;
                NiDynEffectStateIter kIter =
                    pkEffects->GetProjLightHeadPos();
                while (kIter)
                {
                    NiTextureEffect* pkProjLight =
                        pkEffects->GetNextProjLight(kIter);
                    if (usIndex++ == usObjectIndex)
                    {
                        pkTextureEffect = pkProjLight;
                        break;
                    }
                }
                break;
            }
            case NiShaderAttributeDesc::OT_EFFECT_FOGMAP:
                pkTextureEffect = pkEffects->GetFogMap();
                break;
            case NiShaderAttributeDesc::OT_EFFECT_POINTLIGHT:
            {
                NiDynamicEffect* pkDynEffect =
                    NiShaderConstantMap::GetDynamicEffectForObject(
                    pkEffects,
                    NiShaderAttributeDesc::OT_EFFECT_POINTLIGHT,
                    usObjectIndex);

                if (pkDynEffect)
                    pkGenerator = pkDynEffect->GetShadowGenerator();
                break;
            }
            case NiShaderAttributeDesc::OT_EFFECT_SPOTLIGHT:
            {
                NiDynamicEffect* pkDynEffect =
                    NiShaderConstantMap::GetDynamicEffectForObject(
                    pkEffects,
                    NiShaderAttributeDesc::OT_EFFECT_SPOTLIGHT,
                    usObjectIndex);

                if (pkDynEffect)
                    pkGenerator = pkDynEffect->GetShadowGenerator();
                break;
            }
            case NiShaderAttributeDesc::OT_EFFECT_DIRECTIONALLIGHT:
            {
                NiDynamicEffect* pkDynEffect =
                    NiShaderConstantMap::GetDynamicEffectForObject(
                    pkEffects,
                    NiShaderAttributeDesc::OT_EFFECT_DIRECTIONALLIGHT,
                    usObjectIndex);

                if (pkDynEffect)
                    pkGenerator = pkDynEffect->GetShadowGenerator();
                break;
            }
            case NiShaderAttributeDesc::OT_EFFECT_SHADOWPOINTLIGHT:
            {
                NiDynamicEffect* pkDynEffect =
                    NiShaderConstantMap::GetDynamicEffectForObject(
                    pkEffects,
                    NiShaderAttributeDesc::OT_EFFECT_SHADOWPOINTLIGHT,
                    usObjectIndex);

                if (pkDynEffect)
                    pkGenerator = pkDynEffect->GetShadowGenerator();
                break;
            }
            case NiShaderAttributeDesc::OT_EFFECT_SHADOWSPOTLIGHT:
            {
                NiDynamicEffect* pkDynEffect =
                    NiShaderConstantMap::GetDynamicEffectForObject(
                    pkEffects,
                    NiShaderAttributeDesc::OT_EFFECT_SHADOWSPOTLIGHT,
                    usObjectIndex);

                if (pkDynEffect)
                    pkGenerator = pkDynEffect->GetShadowGenerator();
                break;
            }
            case NiShaderAttributeDesc::OT_EFFECT_SHADOWDIRECTIONALLIGHT:
            {
                NiDynamicEffect* pkDynEffect =
                    NiShaderConstantMap::GetDynamicEffectForObject(
                    pkEffects,
                    NiShaderAttributeDesc::OT_EFFECT_SHADOWDIRECTIONALLIGHT,
                    usObjectIndex);

                if (pkDynEffect)
                    pkGenerator = pkDynEffect->GetShadowGenerator();
                break;
            }
            default:
                // This assertion is hit when the object type is not one
                // that has a texture.
                NIASSERT(false);
                break;
        }

        if (pkTextureEffect)
        {
            pkTexture = pkTextureEffect->GetEffectTexture();
            eClampMode = pkTextureEffect->GetTextureClamp();
            eFilterMode = pkTextureEffect->GetTextureFilter();

#if !defined(_PS3)
            // If UseMapIndex is specified, grab and set the tex coord gen
            // mode and 
            if ((uiTextureFlags & NiTextureStage::TSTF_MAP_USE_MASK) == 
                NiTextureStage::TSTF_MAP_USE_INDEX)
            {
                PackDynamicEffect(pkTextureEffect, pkStage,
                    pkStage->GetSaveTexCoordIndex(), m_pkD3DRenderer);
            }
#endif
        }
        else if (pkGenerator)
        {
            NiShadowMap* pkShadowMap = pkGenerator->RetrieveShadowMap(
                NiShadowGenerator::AUTO_DETERMINE_SM_INDEX, pkMesh);
            NIASSERT(pkShadowMap);

            pkTexture = pkShadowMap->GetTexture();
            eClampMode = pkShadowMap->GetClampMode();
            eFilterMode = pkShadowMap->GetFilterMode();
        }
    }

    // Even if the texture is NULL, we need to set that here to prevent a
    // stale texture from getting used by accident.
    pkStage->SetTexture(pkTexture);

    // Set "UseMapValue" sampler states.
#if !defined(_PS3)
    NiD3DTextureStageGroup* pkGroup = pkStage->GetTextureStageGroup();
    if (pkGroup && pkGroup->GetUseMapValueCount() > 0)
    {
        unsigned int uiState, uiValue;
        bool bUseMapValue;

        if (pkGroup->GetSavedSamplerStateCount() > 0)
        {
            NIVERIFY(pkGroup->GetFirstSavedSamplerState(uiState,
                uiValue, bUseMapValue));

            if (bUseMapValue)
            {
                pkGroup->SetSamplerState(uiState, GetSamplerValue(
                    uiState, uiValue, eClampMode, eFilterMode), true);
            }
            while (pkGroup->GetNextSavedSamplerState(uiState, uiValue,
                bUseMapValue))
            {
                if (bUseMapValue)
                {
                    pkGroup->SetSamplerState(uiState, GetSamplerValue(
                        uiState, uiValue, eClampMode, eFilterMode), true,
                        bUseMapValue);
                }
            }
        }
        if (pkGroup->GetNoSaveSamplerStateCount() > 0)
        {
            NIVERIFY(pkGroup->GetFirstNoSaveSamplerState(uiState,
                uiValue, bUseMapValue));

            if (bUseMapValue)
            {
                pkGroup->SetSamplerState(uiState, GetSamplerValue(
                    uiState, uiValue, eClampMode, eFilterMode), false,
                    bUseMapValue);
            }
            while (pkGroup->GetNextNoSaveSamplerState(uiState, uiValue,
                bUseMapValue))
            {
                if (bUseMapValue)
                {
                    pkGroup->SetSamplerState(uiState, GetSamplerValue(
                        uiState, uiValue, eClampMode, eFilterMode), false,
                        bUseMapValue);
                }
            }
        }
    }
#endif
}
//---------------------------------------------------------------------------
