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

#include "NiD3D10PixelShader.h"
#include "NiD3D10ShaderProgramFactory.h"

NiImplementRTTI(NiD3D10PixelShader, NiD3D10ShaderProgram);

//---------------------------------------------------------------------------
NiD3D10PixelShader::NiD3D10PixelShader() :
    NiD3D10ShaderProgram(NiGPUProgram::PROGRAM_PIXEL, NULL),
    m_pkPixelShader(NULL)
{
    /* */
}
//---------------------------------------------------------------------------
NiD3D10PixelShader::NiD3D10PixelShader(ID3D10PixelShader* pkPixelShader,
    ID3D10Blob* pkShaderByteCode) :
    NiD3D10ShaderProgram(NiGPUProgram::PROGRAM_PIXEL, pkShaderByteCode),
    m_pkPixelShader(pkPixelShader)
{
    if (m_pkPixelShader)
        m_pkPixelShader->AddRef();
}
//---------------------------------------------------------------------------
NiD3D10PixelShader::~NiD3D10PixelShader()
{
    if (m_pkPixelShader)
        m_pkPixelShader->Release();
}
//---------------------------------------------------------------------------
void NiD3D10PixelShader::SetPixelShader(ID3D10PixelShader* pkPixelShader)
{
    if (pkPixelShader == m_pkPixelShader)
        return;

    if (pkPixelShader)
        pkPixelShader->AddRef();
    if (m_pkPixelShader)
        m_pkPixelShader->Release();
    m_pkPixelShader = pkPixelShader;
}
//---------------------------------------------------------------------------
void NiD3D10PixelShader::DestroyRendererData()
{
    if (m_pkPixelShader)
    {
        m_pkPixelShader->Release();
        m_pkPixelShader = NULL;
    }
}
//---------------------------------------------------------------------------
void NiD3D10PixelShader::RecreateRendererData()
{
    NiD3D10ShaderProgramFactory::RecreatePixelShader(this);
}
//---------------------------------------------------------------------------

