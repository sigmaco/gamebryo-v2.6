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

#include "NiDX9Direct3DTextureData.h"
#include "NiDX9Direct3DTexture.h"
#include "NiDX9Renderer.h"

//---------------------------------------------------------------------------
NiDX9Direct3DTextureData* NiDX9Direct3DTextureData::Create(
    NiDX9Direct3DTexture* pkTexture, NiDX9Renderer* pkRenderer, 
    D3DBaseTexturePtr pkD3DTexture)
{
    NiDX9Direct3DTextureData* pkThis = NiNew NiDX9Direct3DTextureData(
        pkTexture, pkRenderer);

    bool bSuccess = pkThis->InitializeFromD3DTexture(pkD3DTexture);
    if (bSuccess == false)
    {
        NiDelete pkThis;
        return NULL;
    }

    pkThis->m_pkD3DTexture = pkD3DTexture;
    if (pkThis->m_pkD3DTexture)
        pkThis->m_pkD3DTexture->AddRef();

    pkTexture->SetHeight(pkThis->GetHeight());
    pkTexture->SetWidth(pkThis->GetWidth());

    pkThis->m_pkTexture->SetRendererData(pkThis);

    return pkThis;
}
//---------------------------------------------------------------------------
NiDX9Direct3DTextureData::NiDX9Direct3DTextureData(
    NiDX9Direct3DTexture* pkTexture, NiDX9Renderer* pkRenderer) : 
    NiDX9TextureData(pkTexture, pkRenderer)
{
    /* */
}
//---------------------------------------------------------------------------
