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

#include "NiD3D10Direct3DTextureData.h"
#include "NiD3D10Direct3DTexture.h"
#include "NiD3D10Renderer.h"

//---------------------------------------------------------------------------
NiD3D10Direct3DTextureData* NiD3D10Direct3DTextureData::Create(
    NiD3D10Direct3DTexture* pkTexture, ID3D10Resource* pkD3D10Texture, 
    ID3D10ShaderResourceView* pkResourceView)
{
    NiD3D10Direct3DTextureData* pkThis = NiNew NiD3D10Direct3DTextureData(
        pkTexture);

    if (pkThis == NULL)
        return NULL;

    bool bSuccess = pkThis->InitializeFromD3D10Resource(pkD3D10Texture,
        pkResourceView);
    if (bSuccess == false)
    {
        NiDelete pkThis;
        return NULL;
    }

    pkThis->m_pkD3D10Texture = pkD3D10Texture;
    if (pkThis->m_pkD3D10Texture)
        pkThis->m_pkD3D10Texture->AddRef();
    pkThis->m_pkResourceView = pkResourceView;
    if (pkThis->m_pkResourceView)
        pkThis->m_pkResourceView->AddRef();

    pkTexture->SetHeight(pkThis->GetHeight());
    pkTexture->SetWidth(pkThis->GetWidth());

    pkThis->m_pkTexture->SetRendererData(pkThis);

    return pkThis;
}
//---------------------------------------------------------------------------
NiD3D10Direct3DTextureData::NiD3D10Direct3DTextureData(
    NiD3D10Direct3DTexture* pkTexture) : 
    NiD3D10TextureData(pkTexture)
{
    /* */
}
//---------------------------------------------------------------------------
NiD3D10Direct3DTextureData::~NiD3D10Direct3DTextureData()
{
    if (m_pkD3D10Texture)
        m_pkD3D10Texture->Release();
    if (m_pkResourceView)
        m_pkResourceView->Release();
}
//---------------------------------------------------------------------------
