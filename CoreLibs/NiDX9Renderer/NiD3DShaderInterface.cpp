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

#include "NiD3DRendererHeaders.h"
#include "NiD3DShaderInterface.h"
#include "NiD3DShader.h"
#include "NiD3DPass.h"
#include "NiD3DRenderStateGroup.h"
#include "NiD3DShaderFactory.h"
#include "NiD3DShaderProgram.h"
#include "NiD3DShaderProgramFactory.h"

#include "NiD3DShaderProgramCreator.h"

NiImplementRTTI(NiD3DShaderInterface, NiShader);

//---------------------------------------------------------------------------
void NiD3DShaderInterface::SetD3DDevice(D3DDevicePtr pkD3DDevice)
{
    if (m_pkD3DDevice)
        NiD3DRenderer::ReleaseDevice(m_pkD3DDevice);
    m_pkD3DDevice = pkD3DDevice;
    if (m_pkD3DDevice)
        D3D_POINTER_REFERENCE(m_pkD3DDevice);
}
//---------------------------------------------------------------------------
void NiD3DShaderInterface::SetD3DRenderer(NiD3DRenderer* pkD3DRenderer)
{
    m_pkD3DRenderer = pkD3DRenderer;
    if (m_pkD3DRenderer)
    {
        SetD3DDevice(m_pkD3DRenderer->GetD3DDevice());
        SetD3DRenderState(m_pkD3DRenderer->GetRenderState());
    }
    else 
    {
        SetD3DRenderState(0);
        SetD3DDevice(0);
    }
}
//---------------------------------------------------------------------------
NiD3DError NiD3DShaderInterface::InitializeShaderSystem(
    NiD3DRenderer* pkD3DRenderer)
{
    // For EACH type of class that requires them, set the static pointers.
    // This will be a bit of a maintainence hassle, but we will deal with it.
    // EACH OF THESE WILL HAVE TO BE 'RELEASED' IN THE SHUTDOWN CALL!
    pkD3DRenderer->GetD3DDevice();
    NiD3DRenderState* pkRS = 
        (NiD3DRenderState*)pkD3DRenderer->GetRenderState();

    // NiD3DPass
    NiD3DPass::SetD3DRenderer(pkD3DRenderer);

    // NiD3DTextureStage
    NiD3DTextureStage::SetD3DRenderer(pkD3DRenderer);
    NiD3DTextureStageGroup::SetD3DRenderState(pkRS);

    // NiD3DShaderProgramFactory
    NiD3DShaderProgramFactory::SetD3DRenderer(pkD3DRenderer);

    // NiD3DShaderProgramCreator
    NiD3DShaderProgramCreator::SetD3DRenderer(pkD3DRenderer);

    // NiD3DRenderStateGroup
    NiD3DRenderStateGroup::SetD3DRenderer(pkD3DRenderer);

    pkRS->InitRenderModes();

    // Set the shader factory renderer pointer
    NiD3DShaderFactory* pkShaderFactory = 
        NiD3DShaderFactory::GetD3DShaderFactory();
    if (pkShaderFactory)
        pkShaderFactory->SetRenderer(pkD3DRenderer);

    // Create the shader program factory
    NiD3DShaderProgramFactory* pkShaderProgramFactory = 
        NiD3DShaderProgramFactory::Create();

    if (!pkShaderProgramFactory)
        NiD3DRenderer::Error("Failed to create shader program factory!");

    return NISHADERERR_OK;
}
//---------------------------------------------------------------------------
NiD3DError NiD3DShaderInterface::ShutdownShaderSystem()
{
    // Set the shader factory renderer pointer
    NiD3DShaderFactory* pkShaderFactory = 
        NiD3DShaderFactory::GetD3DShaderFactory();
    if (pkShaderFactory)
    {
        pkShaderFactory->ReleaseAllShaders();
        pkShaderFactory->SetRenderer(0);
        pkShaderFactory->UnregisterAllLibraries();
    }

    // NiD3DPass
    NiD3DPass::SetD3DRenderer(0);

    // NiD3DTextureStage
    NiD3DTextureStage::SetD3DRenderer(0);
    NiD3DTextureStageGroup::SetD3DRenderState(0);

    // NiD3DShaderProgramFactory
    NiD3DShaderProgramFactory::Shutdown();
    NiD3DShaderProgramFactory::SetD3DRenderer(0);

    // NiD3DShaderProgramCreator
    NiD3DShaderProgramCreator::SetD3DRenderer(0);

    // NiD3DRenderStateGroup
    NiD3DRenderStateGroup::SetD3DRenderer(0);

    return NISHADERERR_OK;
}
//---------------------------------------------------------------------------
NiD3DShaderInterface::NiD3DShaderInterface() :
    m_pkD3DDevice(0), 
    m_pkD3DRenderer(0), 
    m_pkD3DRenderState(0)
{
}
//---------------------------------------------------------------------------
NiD3DShaderInterface::~NiD3DShaderInterface()
{
    SetD3DRenderer(0);
}
//---------------------------------------------------------------------------
bool NiD3DShaderInterface::Initialize()
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(NiIsKindOf(NiD3DRenderer, pkRenderer));
    if (pkRenderer == NULL)
        return false;

    SetD3DRenderer((NiD3DRenderer*)pkRenderer);

    return NiShader::Initialize();
}
//---------------------------------------------------------------------------
void NiD3DShaderInterface::HandleLostDevice()
{ /* */ }
//---------------------------------------------------------------------------
void NiD3DShaderInterface::HandleResetDevice()
{ /* */ }
//---------------------------------------------------------------------------
void NiD3DShaderInterface::DestroyRendererData()
{ /* */ }
//---------------------------------------------------------------------------
void NiD3DShaderInterface::RecreateRendererData()
{ /* */ }
//---------------------------------------------------------------------------
bool NiD3DShaderInterface::GetVSPresentAllPasses() const
{
    return false;
}
//---------------------------------------------------------------------------
bool NiD3DShaderInterface::GetVSPresentAnyPass() const
{
    return false;
}
//---------------------------------------------------------------------------
bool NiD3DShaderInterface::GetPSPresentAllPasses() const
{
    return false;
}
//---------------------------------------------------------------------------
bool NiD3DShaderInterface::GetPSPresentAnyPass() const
{
    return false;
}
//---------------------------------------------------------------------------
void NiD3DShaderInterface::ReferenceVertexDeclarationCache(
    NiVertexDeclarationCache kCache) const
{
    // In DX9, NiVertexDeclarationCache is a NiDX9MeshMaterialBinding
    if (kCache)
    {
        ((NiDX9MeshMaterialBinding*)kCache)->IncRefCount();
    }
}
//---------------------------------------------------------------------------
void NiD3DShaderInterface::ReleaseVertexDeclarationCache(
    NiVertexDeclarationCache kCache) const
{
    // In DX9, NiVertexDeclarationCache is NiDX9MeshMaterialBinding
    if (kCache)
    {
        ((NiDX9MeshMaterialBinding*)kCache)->DecRefCount();
    }
}
//---------------------------------------------------------------------------
