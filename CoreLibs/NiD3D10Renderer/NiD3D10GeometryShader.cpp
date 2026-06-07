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

#include "NiD3D10GeometryShader.h"
#include "NiD3D10ShaderProgramFactory.h"

NiImplementRTTI(NiD3D10GeometryShader, NiD3D10ShaderProgram);

//---------------------------------------------------------------------------
NiD3D10GeometryShader::NiD3D10GeometryShader() :
    NiD3D10ShaderProgram(NiGPUProgram::PROGRAM_GEOMETRY, NULL),
    m_pkGeometryShader(NULL),
    m_pkSODeclaration(NULL),
    m_uiNumSOEntries(0),
    m_uiOutputStreamStride(0)
{
    /* */
}
//---------------------------------------------------------------------------
NiD3D10GeometryShader::NiD3D10GeometryShader(
    ID3D10GeometryShader* pkGeometryShader,
    ID3D10Blob* pkShaderByteCode) :
    NiD3D10ShaderProgram(NiGPUProgram::PROGRAM_GEOMETRY, pkShaderByteCode),
    m_pkGeometryShader(pkGeometryShader),
    m_pkSODeclaration(NULL),
    m_uiNumSOEntries(0),
    m_uiOutputStreamStride(0)
{
    if (m_pkGeometryShader)
        m_pkGeometryShader->AddRef();
}
//---------------------------------------------------------------------------
NiD3D10GeometryShader::~NiD3D10GeometryShader()
{
    if (m_pkGeometryShader)
        m_pkGeometryShader->Release();

    NiFree(m_pkSODeclaration);
}

//---------------------------------------------------------------------------
void NiD3D10GeometryShader::SetGeometryShader(
    ID3D10GeometryShader* pkGeometryShader)
{
    if (pkGeometryShader == m_pkGeometryShader)
        return;

    if (pkGeometryShader)
        pkGeometryShader->AddRef();
    if (m_pkGeometryShader)
        m_pkGeometryShader->Release();
    m_pkGeometryShader = pkGeometryShader;
}
//---------------------------------------------------------------------------
void NiD3D10GeometryShader::DestroyRendererData()
{
    if (m_pkGeometryShader)
    {
        m_pkGeometryShader->Release();
        m_pkGeometryShader = NULL;
    }
}
//---------------------------------------------------------------------------
void NiD3D10GeometryShader::RecreateRendererData()
{
    NiD3D10ShaderProgramFactory::RecreateGeometryShader(this);
}
//---------------------------------------------------------------------------
void NiD3D10GeometryShader::SetStreamOutputDeclaration(
    const D3D10_SO_DECLARATION_ENTRY* pkSODeclaration, 
    unsigned int uiNumEntries, unsigned int uiOutputStreamStride)
{
    if (uiNumEntries != m_uiNumSOEntries)
    {
        NiFree(m_pkSODeclaration);
        m_pkSODeclaration = NULL;
        m_uiNumSOEntries = 0;
        m_uiOutputStreamStride = 0;
    }

    if (pkSODeclaration == NULL || uiNumEntries == 0 || 
        uiOutputStreamStride == 0)
    {
        NIASSERT(m_pkSODeclaration == NULL && m_uiNumSOEntries == 0 &&
            m_uiOutputStreamStride == 0)
        return;
    }

    if (m_pkSODeclaration == NULL)
    {
        m_uiNumSOEntries = uiNumEntries;
        m_pkSODeclaration = NiAlloc(D3D10_SO_DECLARATION_ENTRY, 
            uiNumEntries);
    }

    for (unsigned int i = 0; i < uiNumEntries; i++)
    {
        m_pkSODeclaration[i] = pkSODeclaration[i];
    }

    m_uiOutputStreamStride = uiOutputStreamStride;
}

//---------------------------------------------------------------------------
