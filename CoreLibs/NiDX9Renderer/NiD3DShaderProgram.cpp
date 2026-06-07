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

#include "NiD3DShaderProgram.h"
#include "NiD3DRendererHeaders.h"

NiImplementRTTI(NiD3DShaderProgram, NiGPUProgram);
//---------------------------------------------------------------------------
void NiD3DShaderProgram::SetD3DDevice(D3DDevicePtr pkD3DDevice)
{
    if (m_pkD3DDevice)
        NiD3DRenderer::ReleaseDevice(m_pkD3DDevice);
    m_pkD3DDevice = pkD3DDevice;
    if (m_pkD3DDevice)
        D3D_POINTER_REFERENCE(m_pkD3DDevice);
}
//---------------------------------------------------------------------------
void NiD3DShaderProgram::SetD3DRenderer(NiD3DRenderer* pkD3DRenderer)
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
void NiD3DShaderProgram::SetD3DRenderState(NiD3DRenderState* pkRS)
{
    m_pkD3DRenderState = pkRS;
}
//---------------------------------------------------------------------------
NiD3DShaderProgram::NiD3DShaderProgram(NiD3DRenderer* pkRenderer, 
    NiGPUProgram::ProgramType eType) : NiGPUProgram(eType),
    m_pszName(0), m_pszShaderProgramName(0), m_uiCodeSize(0), 
    m_pvCode(0), m_pkCreator(NULL), m_pkD3DDevice(0), m_pkD3DRenderer(0), 
    m_pkD3DRenderState(0)
{
    SetD3DRenderer(pkRenderer);
}
//---------------------------------------------------------------------------
NiD3DShaderProgram::~NiD3DShaderProgram()
{
    NiFree(m_pszName);
    NiFree(m_pszShaderProgramName);
    NiFree(m_pvCode);

    SetD3DRenderer(0);
}
//---------------------------------------------------------------------------
bool NiD3DShaderProgram::SetShaderConstant(
    NiShaderConstantMapEntry*, const void*, unsigned int)
{
    // Do nothing
    return false;
}
//---------------------------------------------------------------------------
bool NiD3DShaderProgram::SetShaderConstantArray(
    NiShaderConstantMapEntry*,
    const void*,
    unsigned int,
    unsigned int,
    const unsigned short*)
{
    // Do nothing
    return false;
}
//---------------------------------------------------------------------------
