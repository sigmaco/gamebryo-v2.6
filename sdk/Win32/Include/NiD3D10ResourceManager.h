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

#ifndef NID3D10RESOURCEMANAGER_H
#define NID3D10RESOURCEMANAGER_H

#include <NiMemObject.h>

#include "NiD3D10RendererLibType.h"
#include "NiD3D10Headers.h"

#include <Ni2DBuffer.h>

class NID3D10RENDERER_ENTRY NiD3D10ResourceManager : public NiMemObject
{
public:
    NiD3D10ResourceManager(ID3D10Device* pkDevice);
    virtual ~NiD3D10ResourceManager();

    // Raw resource creation functions
    ID3D10Buffer* CreateBuffer(unsigned int uiBufferSize,
        unsigned int uiBindFlags, D3D10_USAGE eUsage, 
        unsigned int uiCPUAccessFlags, unsigned int uiMiscFlags, 
        D3D10_SUBRESOURCE_DATA* pkInitialData);
    ID3D10Texture2D* CreateTexture2D(unsigned int uiWidth, 
        unsigned int uiHeight, unsigned int uiMipLevels, 
        unsigned int uiArraySize, DXGI_FORMAT eFormat, 
        unsigned int uiMSAACount, unsigned int uiMSAAQuality, 
        D3D10_USAGE eUsage, unsigned int uiBindFlags, 
        unsigned int uiCPUAccessFlags, unsigned int uiMiscFlags, 
        D3D10_SUBRESOURCE_DATA* pkInitialData = NULL);

protected:
    ID3D10Device* m_pkDevice;
};

#endif // NID3D10RESOURCEMANAGER_H