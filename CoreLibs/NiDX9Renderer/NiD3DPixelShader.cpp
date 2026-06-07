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

#include "NiD3DPixelShader.h"
#include "NiD3DShaderProgramFactory.h"

NiImplementRTTI(NiD3DPixelShader, NiD3DShaderProgram);

//---------------------------------------------------------------------------
NiD3DPixelShader::~NiD3DPixelShader()
{
    NiD3DShaderProgramFactory::ReleasePixelShader(this);
    DestroyRendererData();
    m_hShader = 0;
}
//---------------------------------------------------------------------------
void NiD3DPixelShader::DestroyRendererData()
{
    if (m_hShader)
    {
        if (NiDX9Renderer::GetRenderer())
        {
            NiDX9Renderer::GetRenderer()->PurgePixelShader(this);
        }
        else
        {
            NIASSERT(!m_hShader && "No renderer to free handle to. "
                "Check that resources freed before renderer destroyed.")
        }
    }
}
//---------------------------------------------------------------------------
void NiD3DPixelShader::RecreateRendererData()
{
    NiD3DShaderProgramFactory::RecreatePixelShader(this);
}
//---------------------------------------------------------------------------
bool NiD3DPixelShader::SetShaderConstant(
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
        return m_pkD3DRenderState->SetPixelShaderConstantB(
            pkEntry->GetShaderRegister(), (const BOOL*)pvDataSource, 
            uiRegisterCount);
    }
    else if (NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT == eAttributeType)
    {
        return m_pkD3DRenderState->SetPixelShaderConstantI(
            pkEntry->GetShaderRegister(), (const int*)pvDataSource, 
            uiRegisterCount);
    }

    return m_pkD3DRenderState->SetPixelShaderConstantF(
        pkEntry->GetShaderRegister(), (const float*)pvDataSource, 
        uiRegisterCount);
}
//---------------------------------------------------------------------------
bool NiD3DPixelShader::SetShaderConstantArray(
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
        bool bSuccess = m_pkD3DRenderState->SetPixelShaderConstantF(
            uiRegister, ((const float*)pvDataSource) + 
            (usNewIndex * uiRegistersPerEntry * 4), uiRegistersPerEntry);
        if (bSuccess == false)
            return false;
        uiRegister += uiRegistersPerEntry;
    }

    return true;
}
//---------------------------------------------------------------------------
