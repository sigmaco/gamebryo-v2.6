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

#include "NiD3D10VertexShader.h"
#include "NiD3D10ShaderProgramFactory.h"

NiImplementRTTI(NiD3D10VertexShader, NiD3D10ShaderProgram);

//---------------------------------------------------------------------------
NiD3D10VertexShader::NiD3D10VertexShader() :
    NiD3D10ShaderProgram(NiGPUProgram::PROGRAM_VERTEX, NULL),
    m_pkVertexShader(NULL)
{
    /* */
}
//---------------------------------------------------------------------------
NiD3D10VertexShader::NiD3D10VertexShader(ID3D10VertexShader* pkVertexShader,
    ID3D10Blob* pkShaderByteCode) :
    NiD3D10ShaderProgram(NiGPUProgram::PROGRAM_VERTEX, pkShaderByteCode),
    m_pkVertexShader(pkVertexShader)
{
    if (m_pkVertexShader)
        m_pkVertexShader->AddRef();
}
//---------------------------------------------------------------------------
NiD3D10VertexShader::~NiD3D10VertexShader()
{
    DestroyRendererData();
}
//---------------------------------------------------------------------------
ID3D10VertexShader* NiD3D10VertexShader::GetVertexShader() const
{
    return m_pkVertexShader;
}
//---------------------------------------------------------------------------
void NiD3D10VertexShader::SetVertexShader(ID3D10VertexShader* pkVertexShader)
{
    if (pkVertexShader == m_pkVertexShader)
        return;

    if (pkVertexShader)
        pkVertexShader->AddRef();
    if (m_pkVertexShader)
        m_pkVertexShader->Release();
    m_pkVertexShader = pkVertexShader;
}
//---------------------------------------------------------------------------
void NiD3D10VertexShader::DestroyRendererData()
{
    if (m_pkVertexShader)
    {
        m_pkVertexShader->Release();
        m_pkVertexShader = NULL;
    }
}
//---------------------------------------------------------------------------
void NiD3D10VertexShader::RecreateRendererData()
{
    NiD3D10ShaderProgramFactory::RecreateVertexShader(this);
}
//---------------------------------------------------------------------------
