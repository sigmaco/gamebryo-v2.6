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
#include "NiD3DRendererPCH.h"

#include "NiD3DPass.h"
#include "NiD3DShader.h"
#include "NiD3DRendererHeaders.h"

//---------------------------------------------------------------------------
// Static access pointers
D3DDevicePtr NiD3DPass::ms_pkD3DDevice = 0;
NiD3DRenderer* NiD3DPass::ms_pkD3DRenderer = 0;
NiD3DRenderState* NiD3DPass::ms_pkD3DRenderState = 0;
NiCriticalSection NiD3DPass::ms_kPassPoolCriticalSection;
NiTObjectPool<NiD3DPass>* NiD3DPass::ms_pkPassPool = NULL;

unsigned int NiD3DPass::ms_uiMaxTextureBlendStages = 2;
unsigned int NiD3DPass::ms_uiMaxSimultaneousTextures = 2;
unsigned int NiD3DPass::ms_uiMaxSamplers = 2;

//---------------------------------------------------------------------------
// Static access functions
//---------------------------------------------------------------------------
void NiD3DPass::SetD3DRenderer(NiD3DRenderer* pkD3DRenderer)
{
    ms_pkD3DRenderer = pkD3DRenderer;
    if (ms_pkD3DRenderer)
    {
        SetD3DDevice(ms_pkD3DRenderer->GetD3DDevice());
        SetD3DRenderState(ms_pkD3DRenderer->GetRenderState());
    }
    else
    {
        SetD3DDevice(0);
        SetD3DRenderState(0);
    }
}
//---------------------------------------------------------------------------
void NiD3DPass::SetD3DDevice(D3DDevicePtr pkD3DDevice)
{
    if (ms_pkD3DDevice)
        NiD3DRenderer::ReleaseDevice(ms_pkD3DDevice);
    ms_pkD3DDevice = pkD3DDevice;
    if (ms_pkD3DDevice)
        D3D_POINTER_REFERENCE(ms_pkD3DDevice);
}
//---------------------------------------------------------------------------
void NiD3DPass::SetD3DRenderState(NiD3DRenderState* pkRS)
{
    ms_pkD3DRenderState = pkRS;
}
//---------------------------------------------------------------------------
D3DDevicePtr NiD3DPass::GetD3DDevice()
{
    return ms_pkD3DDevice;
}
//---------------------------------------------------------------------------
NiD3DRenderer* NiD3DPass::GetD3DRenderer()
{
    return ms_pkD3DRenderer;
}
//---------------------------------------------------------------------------
NiD3DRenderState* NiD3DPass::GetD3DRenderState()
{
    return ms_pkD3DRenderState;
}
//---------------------------------------------------------------------------
// Class implementation
//---------------------------------------------------------------------------
NiD3DPass::NiD3DPass() :
    m_uiCurrentStage(0), 
    m_uiStageCount(0), 
    m_uiTexturesPerPass(0), 
    m_bRendererOwned(false), 
    m_pkRenderStateGroup(0), 
    m_pcPixelShaderProgramFile(0), 
    m_pcPixelShaderEntryPoint(0), 
    m_pcPixelShaderTarget(0), 
    m_pcVertexShaderProgramFile(0),
    m_pcVertexShaderEntryPoint(0), 
    m_pcVertexShaderTarget(0), 
    m_uiRefCount(0),
    m_kStages(ms_uiMaxTextureBlendStages, 4)
{
    m_szName[0] = 0;
}
//---------------------------------------------------------------------------
NiD3DPass::~NiD3DPass()
{
    NiD3DRenderStateGroup::ReleaseRenderStateGroup(m_pkRenderStateGroup);

    m_spPixelConstantMap = 0;
    NiFree(m_pcPixelShaderProgramFile);
    NiFree(m_pcPixelShaderEntryPoint);
    NiFree(m_pcPixelShaderTarget);
    m_spPixelShader = 0;
    m_spVertexConstantMap = 0;
    NiFree(m_pcVertexShaderProgramFile);
    NiFree(m_pcVertexShaderEntryPoint);
    NiFree(m_pcVertexShaderTarget);
    m_spVertexShader = 0;

    ReleaseTextureStages();
}
//---------------------------------------------------------------------------
void NiD3DPass::InitializePools()
{
    ms_kPassPoolCriticalSection.Lock();
    NIASSERT(ms_pkPassPool == NULL);
    ms_pkPassPool = NiNew NiTObjectPool<NiD3DPass>;
    ms_kPassPoolCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
void NiD3DPass::ShutdownPools()
{
    ms_kPassPoolCriticalSection.Lock();
    NiDelete ms_pkPassPool;
    ms_pkPassPool = NULL;
    ms_kPassPoolCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
NiD3DPassPtr NiD3DPass::CreateNewPass()
{
    ms_kPassPoolCriticalSection.Lock();
    NIASSERT(ms_pkPassPool != NULL);
    NiD3DPassPtr spReturn = ms_pkPassPool->GetFreeObject();
    ms_kPassPoolCriticalSection.Unlock();

    spReturn->SetRendererOwned();

    return spReturn;
}
//---------------------------------------------------------------------------
void NiD3DPass::ReturnPassToPool()
{
    if (!IsRendererOwned())
    {
        NiDelete this;
        return;
    }

    m_spPixelConstantMap = NULL;
    m_spPixelShader = NULL;
    m_spVertexConstantMap = NULL;
    m_spVertexShader = NULL;

    NiD3DRenderStateGroup::ReleaseRenderStateGroup(m_pkRenderStateGroup);
    m_pkRenderStateGroup = NULL;

    SetPixelShaderProgramFileName(NULL);
    SetPixelShaderProgramEntryPoint(NULL);
    SetPixelShaderProgramShaderTarget(NULL);
    SetVertexShaderProgramFileName(NULL);
    SetVertexShaderProgramEntryPoint(NULL);
    SetVertexShaderProgramShaderTarget(NULL);
    ReleaseTextureStages();

    ms_kPassPoolCriticalSection.Lock();
    NIASSERT(ms_pkPassPool != NULL);
    ms_pkPassPool->ReleaseObject(this);
    ms_kPassPoolCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
unsigned int NiD3DPass::SetupShaderPrograms(const NiRenderCallContext& kRCC)
{
    NiD3DError eErr = NISHADERERR_OK;

    NIASSERT(kRCC.m_pkWorld);

    NIASSERT(kRCC.m_pkWorldBound);

    // Pixel Shader
    if (m_spPixelShader)
    {
        ms_pkD3DRenderState->SetPixelShader(
            m_spPixelShader->GetShaderHandle());
        if (m_spPixelConstantMap)
        {
            eErr = m_spPixelConstantMap->SetShaderConstants(
                kRCC, m_spPixelShader, false);
        }
    }
    else
    {
        ms_pkD3DRenderState->SetPixelShader(0);
    }

    // Vertex Shader
    if (m_spVertexShader)
    {
        ms_pkD3DRenderState->SetVertexShader(
            m_spVertexShader->GetShaderHandle());

        if (m_spVertexConstantMap)
        {
            eErr = m_spVertexConstantMap->SetShaderConstants(
                kRCC, m_spVertexShader, false);
        }

        // Set TSS states as required for the programmable pipeline
        for (unsigned int i = 0; i < ms_uiMaxTextureBlendStages; i++)
        {
            ms_pkD3DRenderState->SetTextureStageState(i, 
                D3DTSS_TEXCOORDINDEX, i);

            // Check specific stage for Projected texture transform flag,
            // which is the only valid texture transform value when
            // using vertex shaders
            unsigned int uiTexTrans = D3DTTFF_DISABLE;
            if (i < m_uiStageCount)
            {
                NiD3DTextureStage* pkStage = m_kStages.GetAt(i);
                if (pkStage)
                {
                    NiD3DTextureStageGroup* pkGroup = 
                        pkStage->GetTextureStageGroup();
                    if (pkGroup)
                    {
                        unsigned int uiValue = 0;
                        bool bSave = false;
                        bool bResult = pkGroup->GetStageState(
                            D3DTSS_TEXTURETRANSFORMFLAGS, uiValue, bSave);
                        if (bResult && 
                            (uiValue & D3DTTFF_PROJECTED) != 0)
                        {
                            uiTexTrans = D3DTTFF_PROJECTED;
                        }
                    }
                }
            } 
            ms_pkD3DRenderState->SetTextureStageState(i, 
                D3DTSS_TEXTURETRANSFORMFLAGS, uiTexTrans);
        }
    }
    else
    {
        ms_pkD3DRenderState->SetVertexShader(0);

        NiDX9MeshMaterialBinding* pkMMB = 
            (NiDX9MeshMaterialBinding*)kRCC.m_pkMeshMaterialBinding;
        ms_pkD3DRenderState->SetDeclaration(pkMMB->GetD3DDeclaration());
    }

    return 0;
}
//---------------------------------------------------------------------------
unsigned int NiD3DPass::SetupRenderingPass(const NiRenderCallContext&)
{
    unsigned int uiRet = 0;

    // Set the 'local' rendering states
    if (m_pkRenderStateGroup != NULL)
    {
        m_pkRenderStateGroup->SetRenderStates();
    }

    // Configure the stages and render states for this pass.
    unsigned int uiNumStages = m_kStages.GetSize();
    unsigned int ui = 0;
    for (; ui < uiNumStages; ui++)
    {
        NiD3DTextureStage* pkStage = m_kStages.GetAt(ui);
        if (pkStage)
        {
            uiRet = pkStage->ConfigureStage();
        }
        else
        {
            // All contiguous stages complete
            break;
        }
    }

#if !defined(_XENON)
    if (ui < ms_uiMaxSamplers)
        NiD3DTextureStage::DisableStage(ui);
#endif  //#if !defined(_XENON)

    return uiRet;
}
//---------------------------------------------------------------------------
unsigned int NiD3DPass::PostProcessRenderingPass(unsigned int)
{
    // Restore the 'local' rendering states
    if (m_pkRenderStateGroup != NULL)
        m_pkRenderStateGroup->RestoreRenderStates();

    return 0;
}
//---------------------------------------------------------------------------
NiD3DError NiD3DPass::SetStage(unsigned int uiStageNum, 
    NiD3DTextureStage* pkStage)
{
    // Sampler-only stages are allowed when 
    // ms_uiMaxSamplers > ms_uiMaxTextureBlendStages.
    if (uiStageNum >= ms_uiMaxSamplers)
        return NISHADERERR_UNKNOWN;

    NiD3DTextureStage* pkOldStage = NULL;
    if (uiStageNum < m_kStages.GetSize())
        /*NiD3DTextureStage* pkOldStage = */m_kStages.GetAt(uiStageNum);

    if (pkOldStage)
    {
        m_uiStageCount--;
        if (pkOldStage->GetTexture() != NULL)
            m_uiTexturesPerPass--;
    }

    if (uiStageNum >= ms_uiMaxTextureBlendStages)
        m_kStages.SetAtGrow(uiStageNum, pkStage);
    else
        m_kStages.SetAt(uiStageNum, pkStage);

    if (pkStage)
    {
        pkStage->SetStage(uiStageNum);
        if (m_uiCurrentStage == uiStageNum)
            m_uiCurrentStage++;

        m_uiStageCount++;
        if (pkStage->GetTexture() != NULL)
            m_uiTexturesPerPass++;
    }
    else if (m_uiCurrentStage == uiStageNum + 1)
    {
        while (m_uiCurrentStage != 0)
        {
            if (m_kStages.GetAt(m_uiCurrentStage - 1) == NULL)
            {
                m_uiCurrentStage--;
                break;
            }
        }
    }

    return NISHADERERR_OK;
}
//---------------------------------------------------------------------------
bool NiD3DPass::CheckFreeStages(unsigned int uiStages, 
    unsigned int uiTextures) const
{
    // Make sure we have a spare stage
    if (ms_uiMaxTextureBlendStages < uiStages + m_uiCurrentStage)
        return false;

    // Make sure we have a spare texture slot
    if (ms_uiMaxSimultaneousTextures < uiTextures + m_uiTexturesPerPass)
        return false;

    // Make sure the current stage can handle a texture, since many cards
    // that can hold more stages than textures can only access textures in
    // the first n stages, where n is the number of textures it can access.
    if (ms_uiMaxSimultaneousTextures < uiTextures + m_uiCurrentStage)
        return false;

    return true;
}
//---------------------------------------------------------------------------
unsigned int NiD3DPass::ValidatePass()
{
    return 0;
}
//---------------------------------------------------------------------------
void NiD3DPass::ReleaseTextureStages()
{
    m_kStages.RemoveAll();

    m_uiCurrentStage = 0;
    m_uiStageCount = 0;
    m_uiTexturesPerPass = 0;
}
//---------------------------------------------------------------------------
void NiD3DPass::SetRendererOwned()
{
    if (!m_bRendererOwned)
    {
        // If the renderer owns this pass, it is part of the NiD3DPass pool
        m_bRendererOwned = true;
    }
}
//---------------------------------------------------------------------------
bool NiD3DPass::IsRendererOwned() const
{
    return m_bRendererOwned;
}
//---------------------------------------------------------------------------
void NiD3DPass::SetMaxTextureBlendStages(unsigned int uiMaxTextureBlendStages)
{
    ms_uiMaxTextureBlendStages = uiMaxTextureBlendStages;
}
//---------------------------------------------------------------------------
void NiD3DPass::SetMaxSimultaneousTextures(
    unsigned int uiMaxSimultaneousTextures)
{
    ms_uiMaxSimultaneousTextures = uiMaxSimultaneousTextures;
}
//---------------------------------------------------------------------------
void NiD3DPass::SetMaxSamplers(unsigned int uiMaxSamplers)
{
    ms_uiMaxSamplers = uiMaxSamplers;
}
//---------------------------------------------------------------------------
inline void NiD3DPass::DeleteThis()
{
    ReturnPassToPool();
}
//---------------------------------------------------------------------------
