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

#include "NiD3D10ShaderConstantManager.h"

#include "NiD3D10DataStream.h"
#include "NiD3D10ShaderConstantMap.h"
#include "NiD3D10ShaderProgram.h"

//---------------------------------------------------------------------------
NiD3D10ShaderConstantManager::NiD3D10ShaderConstantManager(
    NiD3D10DeviceState* pkDeviceState) :
    m_pkDeviceState(pkDeviceState)
{
    NIASSERT(m_pkDeviceState);

    memset(m_apkVertexConstantBuffers, 0,
        sizeof(m_apkVertexConstantBuffers));
    memset(m_apkGeometryConstantBuffers, 0,
        sizeof(m_apkGeometryConstantBuffers));
    memset(m_apkPixelConstantBuffers, 0,
        sizeof(m_apkPixelConstantBuffers));
}
//---------------------------------------------------------------------------
NiD3D10ShaderConstantManager::~NiD3D10ShaderConstantManager()
{
    for (unsigned int i = 0; 
        i < D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT; i++)
    {
        if (m_apkVertexConstantBuffers[i])
            m_apkVertexConstantBuffers[i]->Release();
        if (m_apkGeometryConstantBuffers[i])
            m_apkGeometryConstantBuffers[i]->Release();
        if (m_apkPixelConstantBuffers[i])
            m_apkPixelConstantBuffers[i]->Release();
    }
}
//---------------------------------------------------------------------------
void NiD3D10ShaderConstantManager::SetShaderConstantMap(
    NiD3D10ShaderProgram* pkShaderProgram, 
    NiD3D10ShaderConstantMap* pkConstantMap)
{
    if (pkShaderProgram == NULL || pkConstantMap == NULL)
        return;

    NiD3D10DataStream* pkDataStream = 
        pkConstantMap->GetShaderConstantDataStream();
    if (pkDataStream == NULL)
        return;

    pkDataStream->UpdateD3D10Buffers();

    ID3D10Buffer* pkShaderConstantBuffer = pkDataStream->GetBuffer();
    if (pkShaderConstantBuffer == NULL)
        return;

    unsigned int uiIndex = pkConstantMap->GetShaderConstantBufferIndex();
    if (uiIndex == NiD3D10ShaderConstantMap::BUFFER_INDEX_UNASSIGNED)
    {
        const unsigned int uiConstantBufferCount = 
            pkShaderProgram->GetConstantBufferCount();
        for (unsigned int i = 0; i < uiConstantBufferCount; i++)
        {
            // Special-case constant buffer name = $Globals matching a NULL
            // shader 
            // constant map name.
            NiFixedString kTemp = pkConstantMap->GetName();
            if (kTemp.GetLength() == 0)
                kTemp = "$Globals";
            if (kTemp == pkShaderProgram->GetConstantBufferName(i))
            {
                uiIndex = i;
                break;
            }
        }
        if (uiIndex == NiD3D10ShaderConstantMap::BUFFER_INDEX_UNASSIGNED)
        {
            return;
        }
    }

    NIASSERT(uiIndex != NiD3D10ShaderConstantMap::BUFFER_INDEX_UNASSIGNED);

    ID3D10Buffer** ppkBufferArray = NULL;
    NiGPUProgram::ProgramType eType = pkShaderProgram->GetProgramType();
    switch (eType)
    {
    case NiGPUProgram::PROGRAM_VERTEX:
        ppkBufferArray = m_apkVertexConstantBuffers;
        break;
    case NiGPUProgram::PROGRAM_GEOMETRY:
        ppkBufferArray = m_apkGeometryConstantBuffers;
        break;
    case NiGPUProgram::PROGRAM_PIXEL:
        ppkBufferArray = m_apkPixelConstantBuffers;
        break;
    }
    NIASSERT(ppkBufferArray);

    pkShaderConstantBuffer->AddRef();
    if (ppkBufferArray[uiIndex])
        ppkBufferArray[uiIndex]->Release();
    ppkBufferArray[uiIndex] = pkShaderConstantBuffer;
}
//---------------------------------------------------------------------------
void NiD3D10ShaderConstantManager::SetVertexShaderConstantBuffers(
    unsigned int uiStartSlot, unsigned int uiNumViews, 
    ID3D10Buffer*const* ppkBufferViews)
{
    if (uiStartSlot >= D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT)
        return;
    if (uiNumViews > 
        D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT - uiStartSlot)
    {
        uiNumViews = 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT - uiStartSlot;
    }

    if (ppkBufferViews)
    {
        NIASSERT(uiStartSlot + uiNumViews <= 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT);
        for (unsigned int i = 0; i < uiNumViews; i++)
        {
            if (ppkBufferViews[i] != 
                m_apkVertexConstantBuffers[uiStartSlot + i])
            {
                if (ppkBufferViews[i])
                    ppkBufferViews[i]->AddRef();
                if (m_apkVertexConstantBuffers[uiStartSlot + i])
                {
                    m_apkVertexConstantBuffers[uiStartSlot + i]->
                        Release();
                }
                m_apkVertexConstantBuffers[uiStartSlot + i] = 
                    ppkBufferViews[i];
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiD3D10ShaderConstantManager::GetVertexShaderConstantBuffers(
    unsigned int uiStartSlot, unsigned int uiNumViews, 
    ID3D10Buffer** ppkBufferViews) const
{
    if (uiStartSlot >= D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT)
        return;
    if (uiNumViews > 
        D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT - uiStartSlot)
    {
        uiNumViews = 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT - uiStartSlot;
    }

    if (ppkBufferViews)
    {
        NIASSERT(uiStartSlot + uiNumViews <= 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT);
        for (unsigned int i = 0; i < uiNumViews; i++)
        {
            ppkBufferViews[i] = 
                m_apkVertexConstantBuffers[uiStartSlot + i];
        }
    }
}
//---------------------------------------------------------------------------
void NiD3D10ShaderConstantManager::ClearVertexShaderConstantBuffers()
{
    for (unsigned int i = 0; 
        i < D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT; i++)
    {
        if (m_apkVertexConstantBuffers[i])
            m_apkVertexConstantBuffers[i]->Release();
        m_apkVertexConstantBuffers[i] = NULL;
    }
}
//---------------------------------------------------------------------------
void NiD3D10ShaderConstantManager::SetGeometryShaderConstantBuffers(
    unsigned int uiStartSlot, unsigned int uiNumViews, 
    ID3D10Buffer*const* ppkBufferViews)
{
    if (uiStartSlot >= D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT)
        return;
    if (uiNumViews > 
        D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT - uiStartSlot)
    {
        uiNumViews = 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT - uiStartSlot;
    }

    if (ppkBufferViews)
    {
        NIASSERT(uiStartSlot + uiNumViews <= 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT);
        for (unsigned int i = 0; i < uiNumViews; i++)
        {
            if (ppkBufferViews[i] != 
                m_apkGeometryConstantBuffers[uiStartSlot + i])
            {
                if (ppkBufferViews[i])
                    ppkBufferViews[i]->AddRef();
                if (m_apkGeometryConstantBuffers[uiStartSlot + i])
                {
                    m_apkGeometryConstantBuffers[uiStartSlot + i]->
                        Release();
                }
                m_apkGeometryConstantBuffers[uiStartSlot + i] = 
                    ppkBufferViews[i];
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiD3D10ShaderConstantManager::GetGeometryShaderConstantBuffers(
    unsigned int uiStartSlot, unsigned int uiNumViews, 
    ID3D10Buffer** ppkBufferViews) const
{
    if (uiStartSlot >= D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT)
        return;
    if (uiNumViews > 
        D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT - uiStartSlot)
    {
        uiNumViews = 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT - uiStartSlot;
    }

    if (ppkBufferViews)
    {
        NIASSERT(uiStartSlot + uiNumViews <= 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT);
        for (unsigned int i = 0; i < uiNumViews; i++)
        {
            ppkBufferViews[i] = 
                m_apkGeometryConstantBuffers[uiStartSlot + i];
        }
    }
}
//---------------------------------------------------------------------------
void NiD3D10ShaderConstantManager::ClearGeometryShaderConstantBuffers()
{
    for (unsigned int i = 0; 
        i < D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT; i++)
    {
        if (m_apkGeometryConstantBuffers[i])
            m_apkGeometryConstantBuffers[i]->Release();
        m_apkGeometryConstantBuffers[i] = NULL;
    }
}
//---------------------------------------------------------------------------
void NiD3D10ShaderConstantManager::SetPixelShaderConstantBuffers(
    unsigned int uiStartSlot, unsigned int uiNumViews, 
    ID3D10Buffer*const* ppkBufferViews)
{
    if (uiStartSlot >= D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT)
        return;
    if (uiNumViews > 
        D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT - uiStartSlot)
    {
        uiNumViews = 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT - uiStartSlot;
    }

    if (ppkBufferViews)
    {
        NIASSERT(uiStartSlot + uiNumViews <= 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT);
        for (unsigned int i = 0; i < uiNumViews; i++)
        {
            if (ppkBufferViews[i] != 
                m_apkPixelConstantBuffers[uiStartSlot + i])
            {
                if (ppkBufferViews[i])
                    ppkBufferViews[i]->AddRef();
                if (m_apkPixelConstantBuffers[uiStartSlot + i])
                {
                    m_apkPixelConstantBuffers[uiStartSlot + i]->
                        Release();
                }
                m_apkPixelConstantBuffers[uiStartSlot + i] = 
                    ppkBufferViews[i];
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiD3D10ShaderConstantManager::GetPixelShaderConstantBuffers(
    unsigned int uiStartSlot, unsigned int uiNumViews, 
    ID3D10Buffer** ppkBufferViews) const
{
    if (uiStartSlot >= D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT)
        return;
    if (uiNumViews > 
        D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT - uiStartSlot)
    {
        uiNumViews = 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT - uiStartSlot;
    }

    if (ppkBufferViews)
    {
        NIASSERT(uiStartSlot + uiNumViews <= 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT);
        for (unsigned int i = 0; i < uiNumViews; i++)
        {
            ppkBufferViews[i] = 
                m_apkPixelConstantBuffers[uiStartSlot + i];
        }
    }
}
//---------------------------------------------------------------------------
void NiD3D10ShaderConstantManager::ClearPixelShaderConstantBuffers()
{
    for (unsigned int i = 0; 
        i < D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT; i++)
    {
        if (m_apkPixelConstantBuffers[i])
            m_apkPixelConstantBuffers[i]->Release();
        m_apkPixelConstantBuffers[i] = NULL;
    }
}
//---------------------------------------------------------------------------
void NiD3D10ShaderConstantManager::ApplyShaderConstants()
{
// block shader constants here
    m_pkDeviceState->VSSetConstantBuffers(0, 
        D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT,
        m_apkVertexConstantBuffers);
    m_pkDeviceState->GSSetConstantBuffers(0, 
        D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT,
        m_apkGeometryConstantBuffers);
    m_pkDeviceState->PSSetConstantBuffers(0, 
        D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COUNT,
        m_apkPixelConstantBuffers);
}
//---------------------------------------------------------------------------
