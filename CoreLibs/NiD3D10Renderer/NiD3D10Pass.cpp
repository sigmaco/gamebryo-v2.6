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

#include "NiD3D10Pass.h"

#include "NiD3D10DeviceState.h"
#include "NiD3D10Error.h"
#include "NiD3D10GeometryShader.h"
#include "NiD3D10PixelShader.h"
#include "NiD3D10Renderer.h"
#include "NiD3D10RenderStateGroup.h"
#include "NiD3D10RenderStateManager.h"
#include "NiD3D10ShaderConstantMap.h"
#include "NiD3D10ShaderConstantManager.h"
#include "NiD3D10TextureData.h"
#include "NiD3D10VertexShader.h"

//---------------------------------------------------------------------------
NiD3D10Pass::NiD3D10Pass() :
    m_pkInputSignature(NULL),
    m_pcVertexShaderProgramFile(NULL),
    m_pcVertexShaderEntryPoint(NULL),
    m_pcVertexShaderTarget(NULL),
    m_pcGeometryShaderProgramFile(NULL),
    m_pcGeometryShaderEntryPoint(NULL),
    m_pcGeometryShaderTarget(NULL),
    m_pcPixelShaderProgramFile(NULL),
    m_pcPixelShaderEntryPoint(NULL),
    m_pcPixelShaderTarget(NULL),
    m_uiActiveTextureCount(0),
    m_uiActiveSamplerCount(0),
    m_bConstantMapsLinked(false),
    m_bShaderResourcesLinked(false)
{
    m_acName[0] = '\0';

    ResetTexturesAndSamplers();
}
//---------------------------------------------------------------------------
NiD3D10Pass::~NiD3D10Pass()
{
    NiFree(m_pcVertexShaderProgramFile);
    NiFree(m_pcVertexShaderEntryPoint);
    NiFree(m_pcVertexShaderTarget);
    NiFree(m_pcGeometryShaderProgramFile);
    NiFree(m_pcGeometryShaderEntryPoint);
    NiFree(m_pcGeometryShaderTarget);
    NiFree(m_pcPixelShaderProgramFile);
    NiFree(m_pcPixelShaderEntryPoint);
    NiFree(m_pcPixelShaderTarget);
}
//---------------------------------------------------------------------------
bool NiD3D10Pass::CreateNewPass(NiD3D10PassPtr& spNewPass)
{
    spNewPass = NiNew NiD3D10Pass;
    return true;
}
//---------------------------------------------------------------------------
void NiD3D10Pass::SetVertexShader(NiD3D10VertexShader* pkVertexShader)
{
    m_spVertexShader = pkVertexShader;
    if (m_spVertexShader)
        SetInputSignature(m_spVertexShader->GetShaderByteCode());
    else
        SetInputSignature(NULL);

    m_bConstantMapsLinked = false;
}
//---------------------------------------------------------------------------
NiTexture* NiD3D10Pass::GetVertexTexture(unsigned int uiTexture) const
{
    if (uiTexture < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
        return m_aspVertexTextures[uiTexture];
    else
        return NULL;
}
//---------------------------------------------------------------------------
void NiD3D10Pass::SetVertexTexture(unsigned int uiTexture, 
    NiTexture* pkTexture)
{
    if (uiTexture < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
        m_aspVertexTextures[uiTexture] = pkTexture;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Pass::GetGeometryConstantMapCount() const
{
    return m_kGeometryShaderConstantMaps.GetSize();
}
//---------------------------------------------------------------------------
NiD3D10ShaderConstantMap* NiD3D10Pass::GetGeometryConstantMap(
    unsigned int uiIndex) const
{
    if (uiIndex < m_kGeometryShaderConstantMaps.GetSize())
        return m_kGeometryShaderConstantMaps.GetAt(uiIndex);
    else
        return NULL;
}
//---------------------------------------------------------------------------
void NiD3D10Pass::SetGeometryConstantMap(unsigned int uiIndex, 
    NiD3D10ShaderConstantMap* pkSCMGeometry)
{
    m_kGeometryShaderConstantMaps.SetAtGrow(uiIndex, pkSCMGeometry);

    m_bConstantMapsLinked = false;
}
//---------------------------------------------------------------------------
NiD3D10GeometryShader* NiD3D10Pass::GetGeometryShader() const
{
    return m_spGeometryShader;
}
//---------------------------------------------------------------------------
void NiD3D10Pass::SetGeometryShader(NiD3D10GeometryShader* pkMeshShader)
{
    m_spGeometryShader = pkMeshShader;

    m_bConstantMapsLinked = false;
}
//---------------------------------------------------------------------------
NiTexture* NiD3D10Pass::GetGeometryTexture(unsigned int uiTexture) const
{
    if (uiTexture < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
        return m_aspGeometryTextures[uiTexture];
    else
        return NULL;
}
//---------------------------------------------------------------------------
void NiD3D10Pass::SetGeometryTexture(unsigned int uiTexture, 
    NiTexture* pkTexture)
{
    if (uiTexture < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
        m_aspGeometryTextures[uiTexture] = pkTexture;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Pass::GetPixelConstantMapCount() const
{
    return m_kPixelShaderConstantMaps.GetSize();
}
//---------------------------------------------------------------------------
NiD3D10ShaderConstantMap* NiD3D10Pass::GetPixelConstantMap(
    unsigned int uiIndex) const
{
    if (uiIndex < m_kPixelShaderConstantMaps.GetSize())
        return m_kPixelShaderConstantMaps.GetAt(uiIndex);
    else
        return NULL;
}
//---------------------------------------------------------------------------
void NiD3D10Pass::SetPixelConstantMap(unsigned int uiIndex, 
    NiD3D10ShaderConstantMap* pkSCMPixel)
{
    m_kPixelShaderConstantMaps.SetAtGrow(uiIndex, pkSCMPixel);

    m_bConstantMapsLinked = false;
}
//---------------------------------------------------------------------------
NiD3D10PixelShader* NiD3D10Pass::GetPixelShader() const
{
    return m_spPixelShader;
}
//---------------------------------------------------------------------------
void NiD3D10Pass::SetPixelShader(NiD3D10PixelShader* pkPixelShader)
{
    m_spPixelShader = pkPixelShader;

    m_bConstantMapsLinked = false;
}
//---------------------------------------------------------------------------
NiTexture* NiD3D10Pass::GetPixelTexture(unsigned int uiTexture) const
{
    if (uiTexture < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
        return m_aspPixelTextures[uiTexture];
    else
        return NULL;
}
//---------------------------------------------------------------------------
void NiD3D10Pass::SetPixelTexture(unsigned int uiTexture, NiTexture* pkTexture)
{
    if (uiTexture < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
        m_aspPixelTextures[uiTexture] = pkTexture;
}
//---------------------------------------------------------------------------
bool NiD3D10Pass::LinkUpConstantMaps()
{
    if (m_spVertexShader)
    {
        ID3D10ShaderReflection* pkShaderReflection = 
            m_spVertexShader->GetShaderReflection();
        if (pkShaderReflection)
        {
            const unsigned int uiConstantMaps = 
                m_kVertexShaderConstantMaps.GetSize();

            for (unsigned int i = 0; i < uiConstantMaps; i++)
            {
                NiD3D10ShaderConstantMap* pkConstantMap = 
                    m_kVertexShaderConstantMaps.GetAt(i);
                if (pkConstantMap == NULL)
                    continue;

                pkConstantMap->LinkShaderConstantBuffer(pkShaderReflection);
            }
        }
    }

    if (m_spGeometryShader)
    {
        ID3D10ShaderReflection* pkShaderReflection = 
            m_spGeometryShader->GetShaderReflection();
        if (pkShaderReflection)
        {
            const unsigned int uiConstantMaps = 
                m_kGeometryShaderConstantMaps.GetSize();

            for (unsigned int i = 0; i < uiConstantMaps; i++)
            {
                NiD3D10ShaderConstantMap* pkConstantMap = 
                    m_kGeometryShaderConstantMaps.GetAt(i);
                if (pkConstantMap == NULL)
                    continue;

                pkConstantMap->LinkShaderConstantBuffer(pkShaderReflection);
            }
        }
    }

    if (m_spPixelShader)
    {
        ID3D10ShaderReflection* pkShaderReflection = 
            m_spPixelShader->GetShaderReflection();
        if (pkShaderReflection)
        {
            const unsigned int uiConstantMaps = 
                m_kPixelShaderConstantMaps.GetSize();

            for (unsigned int i = 0; i < uiConstantMaps; i++)
            {
                NiD3D10ShaderConstantMap* pkConstantMap = 
                    m_kPixelShaderConstantMaps.GetAt(i);
                if (pkConstantMap == NULL)
                    continue;

                pkConstantMap->LinkShaderConstantBuffer(pkShaderReflection);
            }
        }
    }

    m_bConstantMapsLinked = true;

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Pass::LinkUpShaderResources()
{
    memset(m_aucShaderResourceRemapping, UCHAR_MAX, 
        sizeof(m_aucShaderResourceRemapping));
    unsigned int i = 0;
    for (; i < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT; i++)
    {
        m_aucSamplerRemapping[i] = (unsigned char)i;
    }

    if (m_spPixelShader)
    {
        ID3D10ShaderReflection* pkShaderReflection = 
            m_spPixelShader->GetShaderReflection();
        if (pkShaderReflection)
        {
            D3D10_SHADER_DESC kShaderDesc;
            // In previous versions of the DX SDK (at some unknown point 
            // earlier than October 2008), this function would write beyond 
            // the bounds of kDesc. This would cause problems, but it seems to 
            // have been fixed.
            HRESULT hr = pkShaderReflection->GetDesc(&kShaderDesc);
            if (FAILED(hr))
            {
                return false;
            }

            const unsigned int uiResources = kShaderDesc.BoundResources;
            for (i = 0; i < uiResources; i++)
            {
                D3D10_SHADER_INPUT_BIND_DESC kDesc;
                hr = pkShaderReflection->GetResourceBindingDesc(i, &kDesc);
                if (FAILED(hr))
                {
                    return false;
                }

                if (kDesc.Type == D3D10_SIT_CBUFFER ||
                    kDesc.Type == D3D10_SIT_TBUFFER)
                {
                    continue;
                }
                else if (kDesc.Type == D3D10_SIT_SAMPLER)
                {
                    // Match up samplers here
                    // If a mismatch occurs, then it is because the sampler
                    // may be used in a different order than it has been 
                    // defined. Find the correct sampler ID and remap it
                    // appropriately.
 
                    // The assumption is that an NiD3D10Pass's render state 
                    // group will contain all the sampler information, rather
                    // than that information existing in the NiD3D10Shader's
                    // render state group.

                    // Find sampler ID that matches this sampler
                    unsigned int j = 0;
                    unsigned char ucSamplerIndex = UCHAR_MAX;
                    for (; j < m_uiActiveSamplerCount; j++)
                    {
                        if (strcmp(kDesc.Name, 
                            (const char*)m_akSamplerNames[j]) == 0)
                        {
                            ucSamplerIndex = (unsigned char)j;
                            break;
                        }
                    }
                    // If this NIASSERTis hit, the name didn't match up 
                    // as expected
                    NIASSERT(ucSamplerIndex < m_uiActiveSamplerCount);

                    // Link up Samplers here
                    unsigned int uiBindPoint = kDesc.BindPoint;
                    const unsigned int uiBindCount = kDesc.BindCount;
                    for (j = 0; j < uiBindCount; j++)
                    {
                        m_aucSamplerRemapping[uiBindPoint + j] = 
                            ucSamplerIndex;
                    }
                }
                else
                {
                    NIASSERT(kDesc.Type == D3D10_SIT_TEXTURE);

                    // Match up textures here
                    // If a mismatch occurs, then it is because the texture
                    // may be used in a different order than it has been 
                    // defined. Find the correct texture ID and remap it
                    // appropriately.

                    // Find texture ID that matches this texture
                    unsigned int j = 0;
                    unsigned char ucTextureIndex = UCHAR_MAX;
                    for (; j < m_uiActiveTextureCount; j++)
                    {
                        if (strcmp(kDesc.Name, 
                            (const char*)m_akTextureNames[j]) == 0)
                        {
                            ucTextureIndex = (unsigned char)j;
                            break;
                        }
                    }
                    // If this NIASSERTis hit, the name didn't match up 
                    // as expected
                    NIASSERT(ucTextureIndex < m_uiActiveTextureCount);

                    // Link up textures here
                    unsigned int uiBindPoint = kDesc.BindPoint;
                    const unsigned int uiBindCount = kDesc.BindCount;
                    for (j = 0; j < uiBindCount; j++)
                    {
                        m_aucShaderResourceRemapping[uiBindPoint + j] = 
                            ucTextureIndex;
                    }
                }
            }
        }
    }

    m_bShaderResourcesLinked = true;

    return true;
}
//---------------------------------------------------------------------------
ID3D10Blob* NiD3D10Pass::GetInputSignature() const
{
    return m_pkInputSignature;
}
//---------------------------------------------------------------------------
void NiD3D10Pass::SetInputSignature(ID3D10Blob* pkInputSignature)
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
unsigned int NiD3D10Pass::ApplyShaderPrograms(
    const NiRenderCallContext&)
{
    NIASSERT(NiD3D10Renderer::GetRenderer());

    NiD3D10DeviceState* pkDeviceState = 
        NiD3D10Renderer::GetRenderer()->GetDeviceState();
    NIASSERT(pkDeviceState);

    // Vertex Shader
    if (m_spVertexShader)
        pkDeviceState->VSSetShader(m_spVertexShader->GetVertexShader());
    else
        pkDeviceState->VSClearShader();

    // Geometry Shader
    if (m_spGeometryShader)
        pkDeviceState->GSSetShader(m_spGeometryShader->GetGeometryShader());
    else
        pkDeviceState->GSClearShader();

    // Pixel Shader
    if (m_spPixelShader)
        pkDeviceState->PSSetShader(m_spPixelShader->GetPixelShader());
    else
        pkDeviceState->PSClearShader();

    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Pass::ApplyShaderConstants(const NiRenderCallContext& kRCC)
{
    NIASSERT(NiD3D10Renderer::GetRenderer());

    NiD3D10ShaderConstantManager* pkShaderConstantManager = 
        NiD3D10Renderer::GetRenderer()->GetShaderConstantManager();
    NIASSERT(pkShaderConstantManager);

    if (m_bConstantMapsLinked == false && LinkUpConstantMaps() == false)
    {
        NiD3D10Error::ReportError(
            NiD3D10Error::NiD3D10ERROR_SHADER_CONSTANT_MAPPING_FAILED);
        NIASSERT(false);
    }

    // Vertex Shader
    NiShaderError eErr = NISHADERERR_OK;
    if (m_spVertexShader)
    {
        const unsigned int uiMapCount = m_kVertexShaderConstantMaps.GetSize();
        for (unsigned int i = 0; i < uiMapCount; i++)
        {
            NiD3D10ShaderConstantMap* pkMap = 
                m_kVertexShaderConstantMaps.GetAt(i);
            if (pkMap)
            {
                eErr = pkMap->UpdateShaderConstants(kRCC, true);
                pkShaderConstantManager->SetShaderConstantMap(
                    m_spVertexShader, pkMap);
            }
        }
    }
    else
    {
        NiD3D10Error::ReportError(
            NiD3D10Error::NiD3D10ERROR_SHADER_MISSING,
            "The vertex shader is null.");
        NIASSERT(false);
    }

    // Geometry Shader
    if (m_spGeometryShader)
    {
        const unsigned int uiMapCount = 
            m_kGeometryShaderConstantMaps.GetSize();
        for (unsigned int i = 0; i < uiMapCount; i++)
        {
            NiD3D10ShaderConstantMap* pkMap = 
                m_kGeometryShaderConstantMaps.GetAt(i);
            if (pkMap)
            {
                eErr = pkMap->UpdateShaderConstants(kRCC, true);
                pkShaderConstantManager->SetShaderConstantMap(
                    m_spGeometryShader, pkMap);
            }
        }
    }

    // Pixel Shader
    if (m_spPixelShader)
    {
        const unsigned int uiMapCount = m_kPixelShaderConstantMaps.GetSize();
        for (unsigned int i = 0; i < uiMapCount; i++)
        {
            NiD3D10ShaderConstantMap* pkMap = 
                m_kPixelShaderConstantMaps.GetAt(i);
            if (pkMap)
            {
                eErr = pkMap->UpdateShaderConstants(kRCC, true);
                pkShaderConstantManager->SetShaderConstantMap(
                    m_spPixelShader, pkMap);
            }
        }
    }

    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Pass::ApplyTextures(const NiRenderCallContext&)
{
    ID3D10ShaderResourceView* apkResourceViews[
        D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT];

    NIASSERT(NiD3D10Renderer::GetRenderer());

    NiD3D10DeviceState* pkDeviceState = 
        NiD3D10Renderer::GetRenderer()->GetDeviceState();
    NIASSERT(pkDeviceState);

    // Vertex Shader
    if (m_spVertexShader)
    {
        memset(apkResourceViews, 0, sizeof(apkResourceViews));
        for (unsigned int i = 0; i < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT; 
            i++)
        {
            NiTexture* pkTexture = m_aspVertexTextures[i];
            if (pkTexture == 0)
                continue;

            NiD3D10TextureData* pkTextureData = (NiD3D10TextureData*)
                pkTexture->GetRendererData();
            if (pkTextureData == NULL)
            {
                // Pack texture if necessary
                NiRenderer::GetRenderer()->PrecacheTexture(pkTexture);
                pkTextureData = (NiD3D10TextureData*)
                    pkTexture->GetRendererData();
            }

            if (pkTextureData)
                apkResourceViews[i] = pkTextureData->GetResourceView();
        }

        pkDeviceState->VSSetShaderResources(0, 
            D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT, apkResourceViews);
    }
    else
    {
        pkDeviceState->VSClearShaderResources();
    }

    // Geometry Shader
    if (m_spGeometryShader)
    {
        memset(apkResourceViews, 0, sizeof(apkResourceViews));
        for (unsigned int i = 0; i < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT; 
            i++)
        {
            NiTexture* pkTexture = m_aspGeometryTextures[i];
            if (pkTexture == 0)
                continue;

            NiD3D10TextureData* pkTextureData = (NiD3D10TextureData*)
                pkTexture->GetRendererData();
            if (pkTextureData == NULL)
            {
                // Pack texture if necessary
                NiRenderer::GetRenderer()->PrecacheTexture(pkTexture);
                pkTextureData = (NiD3D10TextureData*)
                    pkTexture->GetRendererData();
            }

            if (pkTextureData)
                apkResourceViews[i] = pkTextureData->GetResourceView();
        }

        pkDeviceState->GSSetShaderResources(0, 
            D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT, apkResourceViews);
    }
    else
    {
        pkDeviceState->GSClearShaderResources();
    }

    // Pixel Shader
    if (m_spPixelShader)
    {
        memset(apkResourceViews, 0, sizeof(apkResourceViews));
        for (unsigned int i = 0; i < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT; 
            i++)
        {
            // Look up the correct texture using the remapping array
            unsigned char ucTextureToUse = m_aucShaderResourceRemapping[i];
            if (ucTextureToUse == UCHAR_MAX)
                continue;
            NiTexture* pkTexture = m_aspPixelTextures[ucTextureToUse];
            if (pkTexture == 0)
                continue;

            NiD3D10TextureData* pkTextureData = (NiD3D10TextureData*)
                pkTexture->GetRendererData();
            if (pkTextureData == NULL)
            {
                // Pack texture if necessary
                NiRenderer::GetRenderer()->PrecacheTexture(pkTexture);
                pkTextureData = (NiD3D10TextureData*)
                    pkTexture->GetRendererData();
            }

            if (pkTextureData)
                apkResourceViews[i] = pkTextureData->GetResourceView();
        }

        pkDeviceState->PSSetShaderResources(0, 
            D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT, apkResourceViews);
    }
    else
    {
        pkDeviceState->PSClearShaderResources();
    }

    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Pass::SetupRenderingPass(
    const NiRenderCallContext& kRCC)
{
    NIASSERT(NiD3D10Renderer::GetRenderer());

    if (m_bShaderResourcesLinked == false && 
        LinkUpShaderResources() == false)
    {
        /* */
    }

    NiD3D10RenderStateManager* pkRenderState = 
        NiD3D10Renderer::GetRenderer()->GetRenderStateManager();
    NIASSERT(pkRenderState);

    if (m_spRenderStateGroup)
    {
        m_spRenderStateGroup->SetSamplerRemapping(NiGPUProgram::PROGRAM_PIXEL,
            m_aucSamplerRemapping);
        pkRenderState->SetRenderStateGroup(m_spRenderStateGroup);
    }

    ApplyTextures(kRCC);

    return NULL;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Pass::PostProcessRenderingPass(unsigned int)
{
    // Does nothing
    return 0;
}
//---------------------------------------------------------------------------
void NiD3D10Pass::ResetTexturesAndSamplers()
{
    m_uiActiveTextureCount = 0;
    m_uiActiveSamplerCount = 0;

    memset(m_aucShaderResourceRemapping, 0, 
        sizeof(m_aucShaderResourceRemapping));
    memset(m_auiGamebryoMapFlags, 0, sizeof(m_auiGamebryoMapFlags));
    memset(m_ausObjectTextureFlags, 0, sizeof(m_ausObjectTextureFlags));

    for (unsigned int i = 0; i < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT; i++)
    {
        m_aucSamplerRemapping[i] = (unsigned char) i;
        m_akSamplerNames[i] = "";
        m_akTextureNames[i] = "";
    }
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Pass::GetFirstUnassignedTexture()
{
    if (m_uiActiveTextureCount < D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
        return m_uiActiveTextureCount++;
    else
        return UINT_MAX;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Pass::GetActiveTextureCount() const
{
    return m_uiActiveTextureCount;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Pass::GetActiveSamplerCount() const
{
    return m_uiActiveSamplerCount;
}
//---------------------------------------------------------------------------
bool NiD3D10Pass::SetTextureName(unsigned int uiTexture, 
    const NiFixedString& kTextureName)
{
    if (uiTexture >= D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
        return false;

    if (uiTexture <= m_uiActiveTextureCount)
        m_uiActiveTextureCount = uiTexture + 1;

    m_akTextureNames[uiTexture] = kTextureName;

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Pass::SetGamebryoMap(unsigned int uiTexture, 
    const NiFixedString& kTextureName, unsigned int uiGBMap, 
    unsigned short usInstance, unsigned short usObjectFlags)
{
    if (uiTexture >= D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
        return false;

    if (uiTexture <= m_uiActiveTextureCount)
        m_uiActiveTextureCount = uiTexture + 1;

    m_auiGamebryoMapFlags[uiTexture] = uiGBMap | usInstance;
    m_ausObjectTextureFlags[uiTexture] = usObjectFlags;
    m_akTextureNames[uiTexture] = kTextureName;
    m_bShaderResourcesLinked = false;

    return true;
}
//---------------------------------------------------------------------------
bool NiD3D10Pass::SetSamplerName(unsigned int uiSampler, 
    const NiFixedString& kSamplerName)
{
    if (uiSampler >= D3D10_COMMONSHADER_SAMPLER_SLOT_COUNT)
        return false;

    if (uiSampler <= m_uiActiveSamplerCount)
        m_uiActiveSamplerCount = uiSampler + 1;

    m_akSamplerNames[uiSampler] = kSamplerName;

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiD3D10Pass::GetGamebryoMapFlags(unsigned int uiTexture) const
{
    if (uiTexture < m_uiActiveTextureCount)
    {
        return m_auiGamebryoMapFlags[uiTexture];
    }

    return 0;
}
//---------------------------------------------------------------------------
unsigned short NiD3D10Pass::GetObjectTextureFlags(unsigned int uiTexture)
    const
{
    if (uiTexture < m_uiActiveTextureCount)
    {
        return m_ausObjectTextureFlags[uiTexture];
    }

    return 0;
}
//---------------------------------------------------------------------------
