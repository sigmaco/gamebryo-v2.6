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

#include "NiD3D10ResourceManager.h"
#include "NiD3D10Error.h"

//---------------------------------------------------------------------------
NiD3D10ResourceManager::NiD3D10ResourceManager(ID3D10Device* pkDevice) :
    m_pkDevice(pkDevice)
{
    assert (m_pkDevice);
    if (m_pkDevice)
        m_pkDevice->AddRef();
}
//---------------------------------------------------------------------------
NiD3D10ResourceManager::~NiD3D10ResourceManager()
{
    if (m_pkDevice)
        m_pkDevice->Release();
}
//---------------------------------------------------------------------------
ID3D10Buffer* NiD3D10ResourceManager::CreateBuffer(unsigned int uiBufferSize,
    unsigned int uiBindFlags, D3D10_USAGE eUsage, 
    unsigned int uiCPUAccessFlags, unsigned int uiMiscFlags, 
    D3D10_SUBRESOURCE_DATA* pkInitialData)
{
    ID3D10Buffer* pkBuffer = NULL;
    D3D10_BUFFER_DESC kDesc;
    kDesc.ByteWidth = uiBufferSize;
    kDesc.BindFlags = uiBindFlags;
    kDesc.Usage = eUsage;
    kDesc.CPUAccessFlags = uiCPUAccessFlags;
    kDesc.MiscFlags = uiMiscFlags;

    HRESULT hr = m_pkDevice->CreateBuffer(&kDesc, pkInitialData, &pkBuffer);
    if (FAILED(hr) || pkBuffer == NULL)
    {
        if (FAILED(hr))
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_BUFFER_CREATION_FAILED,
                "Error HRESULT = 0x%08X.", (unsigned int)hr);
        }
        else
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_BUFFER_CREATION_FAILED,
                "No error message from D3D10, but buffer is NULL.");
        }

        if (pkBuffer)
        {
            pkBuffer->Release();
            pkBuffer = NULL;
        }
    }

    return pkBuffer;
}
//---------------------------------------------------------------------------
ID3D10Texture2D* NiD3D10ResourceManager::CreateTexture2D(
    unsigned int uiWidth, unsigned int uiHeight, unsigned int uiMipLevels,
    unsigned int uiArraySize, DXGI_FORMAT eFormat, unsigned int uiMSAACount,
    unsigned int uiMSAAQuality, D3D10_USAGE eUsage, unsigned int uiBindFlags,
    unsigned int uiCPUAccessFlags, unsigned int uiMiscFlags, 
    D3D10_SUBRESOURCE_DATA* pkInitialData)
{
    ID3D10Texture2D* pkTexture = NULL;
    D3D10_TEXTURE2D_DESC kDesc;
    kDesc.Width = uiWidth;
    kDesc.Height = uiHeight;
    kDesc.MipLevels = uiMipLevels;
    kDesc.ArraySize = uiArraySize;
    kDesc.Format = eFormat;
    kDesc.SampleDesc.Count = uiMSAACount;
    kDesc.SampleDesc.Quality = uiMSAAQuality;
    kDesc.Usage = eUsage;
    kDesc.BindFlags = uiBindFlags;
    kDesc.CPUAccessFlags = uiCPUAccessFlags;
    kDesc.MiscFlags = uiMiscFlags;

    HRESULT hr = m_pkDevice->CreateTexture2D(&kDesc, pkInitialData, 
        &pkTexture);
    if (FAILED(hr) || pkTexture == NULL)
    {
        if (FAILED(hr))
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_TEXTURE2D_CREATION_FAILED,
                "Error HRESULT = 0x%08X.", (unsigned int)hr);
        }
        else
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_TEXTURE2D_CREATION_FAILED,
                "No error message from D3D10, but texture is NULL.");
        }

        if (pkTexture)
        {
            pkTexture->Release();
            pkTexture = NULL;
        }
    }

    return pkTexture;
}
//---------------------------------------------------------------------------
