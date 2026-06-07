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

#include "NiD3DVertexShader.h"
#include "NiD3DShaderProgramFactory.h"
NiImplementRTTI(NiD3DVertexShader, NiD3DShaderProgram);

//---------------------------------------------------------------------------
NiD3DVertexShader::~NiD3DVertexShader()
{
    NiD3DShaderProgramFactory::ReleaseVertexShader(this);
    DestroyRendererData();
}
//---------------------------------------------------------------------------
void NiD3DVertexShader::DestroyRendererData()
{
    if (m_hShader)
    {
        if (NiDX9Renderer::GetRenderer())
        {
            NiDX9Renderer::GetRenderer()->PurgeVertexShader(this);
        }
        else
        {
            NIASSERT(!m_hShader && "No renderer to free handle to. "
                "Check that resources freed before renderer destroyed.")
        }
    }
}
//---------------------------------------------------------------------------
void NiD3DVertexShader::RecreateRendererData()
{
    NiD3DShaderProgramFactory::RecreateVertexShader(this);
}
//---------------------------------------------------------------------------
bool NiD3DVertexShader::SetShaderConstant(
    NiShaderConstantMapEntry* pkEntry, const void* pvDataSource, 
    unsigned int uiRegisterCount)
{
    if (pvDataSource == NULL)
        pvDataSource = pkEntry->GetDataSource();

    if (uiRegisterCount == 0)
    {
        uiRegisterCount = pkEntry->GetRegisterCount();
        if (uiRegisterCount == 0)
            uiRegisterCount = 1;
    }

    NiShaderAttributeDesc::AttributeType eAttributeType = 
        pkEntry->GetAttributeType();

    if (NiShaderAttributeDesc::ATTRIB_TYPE_BOOL == eAttributeType)
    {
        return m_pkD3DRenderState->SetVertexShaderConstantB(
            pkEntry->GetShaderRegister(), (const BOOL*)pvDataSource, 
            uiRegisterCount);
    }
    else if (NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT == eAttributeType)
    {
        return m_pkD3DRenderState->SetVertexShaderConstantI(
            pkEntry->GetShaderRegister(), (const int*)pvDataSource, 
            uiRegisterCount);
    }

    return m_pkD3DRenderState->SetVertexShaderConstantF(
        pkEntry->GetShaderRegister(), (const float*)pvDataSource, 
        uiRegisterCount);
}
//---------------------------------------------------------------------------
bool NiD3DVertexShader::SetShaderConstantArray(
    NiShaderConstantMapEntry* pkEntry,
    const void* pvDataSource, 
    unsigned int uiNumEntries,
    unsigned int uiRegistersPerEntry,
    const unsigned short* pusReorderArray)
{
    unsigned int uiRegister = pkEntry->GetShaderRegister();

    for (unsigned int i = 0; i < uiNumEntries; i++)
    {
        unsigned short usNewIndex = static_cast<unsigned short>(
            (pusReorderArray == NULL) ? i : pusReorderArray[i]);
        bool bSuccess = m_pkD3DRenderState->SetVertexShaderConstantF(
            uiRegister, ((const float*)pvDataSource) + 
            (usNewIndex * uiRegistersPerEntry * 4), uiRegistersPerEntry);
        if (bSuccess == false)
            return false;
        uiRegister += uiRegistersPerEntry;
    }

    return true;
}
//---------------------------------------------------------------------------
