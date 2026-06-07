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
#include "NiD3D10Error.h"
#include "NiD3D10PixelFormat.h"
#include "NiD3D10Renderer.h"
#include "NiD3D10DynamicTextureData.h"

//---------------------------------------------------------------------------
NiD3D10DynamicTextureData* NiD3D10DynamicTextureData::Create(
    NiDynamicTexture* pkTexture)
{
    NiD3D10DynamicTextureData* pkThis = NiNew NiD3D10DynamicTextureData(
        pkTexture);

    const NiPixelFormat* pkFmt =
        pkThis->CreateTexture(pkTexture);
    if (!pkFmt)
    {
        NiDelete pkThis;
        return NULL;
    }

    pkThis->m_usTextureType |= TEXTURETYPE_DYNAMIC;
    
    pkThis->m_pkTexture->SetRendererData(pkThis);

    return pkThis;
}
//---------------------------------------------------------------------------
NiD3D10DynamicTextureData::NiD3D10DynamicTextureData(
    NiDynamicTexture* pkTexture) : 
    NiD3D10TextureData(pkTexture), m_bTextureLocked(false),
    m_pkStagingTexture(NULL),
    m_kMapFlags(D3D10_MAP_READ_WRITE)
{
    NIASSERT(pkTexture);
    m_usTextureType |= TEXTURETYPE_DYNAMIC;
}
//---------------------------------------------------------------------------
NiD3D10DynamicTextureData::~NiD3D10DynamicTextureData()
{
    if (m_pkStagingTexture)
        m_pkStagingTexture->Release();
    m_pkStagingTexture = NULL;
}
//---------------------------------------------------------------------------
const NiPixelFormat* NiD3D10DynamicTextureData::CreateTexture(
    const NiDynamicTexture* pkTexture)
{
    if (pkTexture == 0)
        return NULL;

    //for now all we do is 2D dynamic textures.
    m_uiWidth = pkTexture->GetWidth();
    m_uiHeight = pkTexture->GetHeight();
    m_usLevels = 1;
    m_usTextureType = TEXTURETYPE_2D;

    // Renderer must exist.
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    if (pkRenderer == NULL)
        return NULL;

    // Need a format.  Determine the desired pixel format for the buffer.  For
    // now, just look at the alpha.
    const NiTexture::FormatPrefs kPrefs = pkTexture->GetFormatPreferences();
   
    // Dynamic Textures are always write discard.
    m_kMapFlags = D3D10_MAP_WRITE;

    const NiPixelFormat* pkFmt = NiD3D10TextureData::FindMatchingPixelFormat(
        kPrefs,D3D10_FORMAT_SUPPORT_TEXTURE2D);

    if (pkFmt == NULL)
        return NULL;

    m_kPixelFormat = *pkFmt;

    DXGI_FORMAT eD3DFmt = (DXGI_FORMAT)pkFmt->GetRendererHint();

    ID3D10Device* pkD3DDevice10 = pkRenderer->GetD3D10Device();
        
    HRESULT eD3DRet = E_FAIL;
    HRESULT eTextureD3DRet = E_FAIL;
    HRESULT eStagingD3DRet = E_FAIL;
    ID3D10ShaderResourceView* pkResourceView = NULL;
    D3D10_SHADER_RESOURCE_VIEW_DESC kSrcDesc;
        
    if (m_pkD3D10Texture)
        m_pkD3D10Texture->Release();
    m_pkD3D10Texture = NULL;

    if (m_pkStagingTexture)
        m_pkStagingTexture->Release();
    m_pkStagingTexture = NULL;

    NiD3D10ResourceManager* pkResourceManager = 
        pkRenderer->GetResourceManager();

    if (IsTexture1D())
    {
        D3D10_TEXTURE1D_DESC kDesc;
        kDesc.Width = m_uiWidth;
        kDesc.MipLevels = 1;
        kDesc.Format = eD3DFmt;
        kDesc.Usage = D3D10_USAGE_DEFAULT;
        kDesc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
        kDesc.CPUAccessFlags = 0;
        eTextureD3DRet = pkD3DDevice10->CreateTexture1D(&kDesc, NULL, 
            (ID3D10Texture1D**)&m_pkD3D10Texture);
        if (FAILED(eTextureD3DRet))
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_TEXTURE1D_CREATION_FAILED,
                "Dynamic 1D Texture creation failed");
            m_pkD3D10Texture = NULL;
            return NULL;
        }

        //create the staging texture
        kDesc.Usage = D3D10_USAGE_STAGING;
        kDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;
        eStagingD3DRet = pkD3DDevice10->CreateTexture1D(&kDesc, NULL, 
            (ID3D10Texture1D**)&m_pkStagingTexture);  
        //set format and dimension
        kSrcDesc.Format = kDesc.Format;
        kSrcDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;

    }
    else if (IsTexture2D() || IsCubeMap())
    {
        unsigned int uiMiscFlags = 
            IsCubeMap() ? D3D10_RESOURCE_MISC_TEXTURECUBE : 0;

        m_pkD3D10Texture = pkResourceManager->CreateTexture2D(
            m_uiWidth, m_uiHeight, 1, IsCubeMap() ? 6 : 1, eD3DFmt,
            1, 0, D3D10_USAGE_DEFAULT, D3D10_BIND_SHADER_RESOURCE, 0,
            uiMiscFlags);
        if (m_pkD3D10Texture == NULL)
        {
            // Error already reported
            m_pkD3D10Texture = NULL;
            return NULL;
        }

        // Create the staging texture.
        m_pkStagingTexture = pkResourceManager->CreateTexture2D( 
            m_uiWidth, m_uiHeight, 1, IsCubeMap() ? 6 : 1, eD3DFmt,
            1, 0, D3D10_USAGE_STAGING, 0, D3D10_CPU_ACCESS_WRITE
            | D3D10_CPU_ACCESS_READ, uiMiscFlags);

        // Test for failure.
        if (m_pkStagingTexture == NULL)
            eStagingD3DRet = E_FAIL; 
        else                             
            eStagingD3DRet = S_OK; 

        // set format and dimension for creating 
        // resource view.
        kSrcDesc.Format = eD3DFmt;
        kSrcDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;

    }
    else if (IsTexture3D())
    {
        D3D10_TEXTURE3D_DESC kDesc;
        kDesc.Width = m_uiWidth;
        kDesc.Height = m_uiHeight;
        kDesc.MipLevels = 1;
        kDesc.Format = eD3DFmt;
        kDesc.Usage = D3D10_USAGE_DEFAULT;
        kDesc.CPUAccessFlags = 0;
        kDesc.BindFlags = D3D10_BIND_SHADER_RESOURCE;
        eTextureD3DRet = pkD3DDevice10->CreateTexture3D(&kDesc, NULL, 
            (ID3D10Texture3D**)&m_pkD3D10Texture);
        if (FAILED(eTextureD3DRet))
        {
            NiD3D10Error::ReportError(
                NiD3D10Error::NID3D10ERROR_TEXTURE3D_CREATION_FAILED,
                "Dynamic 3D Texture creation failed");
            m_pkD3D10Texture = NULL;
            return NULL;
        }
        
        //create the staging texture
        kDesc.Usage = D3D10_USAGE_STAGING;
        kDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE | D3D10_CPU_ACCESS_READ;
        eStagingD3DRet = pkD3DDevice10->CreateTexture3D(&kDesc, NULL, 
            (ID3D10Texture3D**)&m_pkStagingTexture);
        //set format and dimension
        kSrcDesc.Format = kDesc.Format;
        kSrcDesc.ViewDimension = D3D10_SRV_DIMENSION_TEXTURE2D;
    }
           
    if (!FAILED(eStagingD3DRet))
    {
        // Create the shader-resource view
        kSrcDesc.Texture2D.MostDetailedMip = 0;
        kSrcDesc.Texture2D.MipLevels = 1;

        eD3DRet = pkD3DDevice10->CreateShaderResourceView( m_pkD3D10Texture, 
        &kSrcDesc, &pkResourceView );
    }

    if (FAILED(eStagingD3DRet))
    {
        // Error already reported
        m_pkStagingTexture = NULL;
        return NULL;
    }

    if (FAILED(eD3DRet))
    {
        NiD3D10Error::ReportError(
            NiD3D10Error::NiD3D10ERROR_SHADER_RESOURCE_VIEW_CREATION_FAILED,
            "Dynamic Texture creation failed when creating resource view");
        pkResourceView = NULL;
        return NULL;
    }
    SetResourceView(pkResourceView);

    return pkFmt;
}
//---------------------------------------------------------------------------

void* NiD3D10DynamicTextureData::Lock(int& iPitch)
{
    void* pvMem = NULL;
    if (m_pkD3D10Texture)
    {
        if (IsTexture1D())
        {
            pvMem = Texture1DLock();
        }
        else if (IsTexture2D() || IsCubeMap())
        {
            pvMem = Texture2DLock( iPitch );
        }
        else if (IsTexture3D())
        {
            NiD3D10Error::ReportError(
            NiD3D10Error::NiD3D10ERROR_UNSUPPORTED_RESOURCE_LOCK_FAILED,
            "3D dynamic texture locking is unsupported.");
            NIASSERT(false);
        }
    }
    return pvMem;
}

//---------------------------------------------------------------------------

bool NiD3D10DynamicTextureData::UnLock() const
{
    if (!m_pkD3D10Texture)
        return false;
    if (IsTexture1D())
    {
        ID3D10Texture1D* pkTexture = (ID3D10Texture1D*) m_pkD3D10Texture;
        if (pkTexture)
        {
            ID3D10Texture1D *pkStagingTexture = 
                (ID3D10Texture1D*)m_pkStagingTexture;

            if (pkStagingTexture==NULL)
                return false;

            pkStagingTexture->Unmap(D3D10CalcSubresource(0, 0, 1));

            // Renderer must exist.
            NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
            if (pkRenderer == NULL)
                return NULL;

            ID3D10Device *pkD3DDevice10 = pkRenderer->GetD3D10Device();
            if (pkD3DDevice10==NULL)
                return NULL;
            
            pkD3DDevice10->CopyResource(pkTexture,pkStagingTexture);
            return true;   
        }
    }
    else if (IsTexture2D() || IsCubeMap())
    {

        ID3D10Texture2D* pkTexture = (ID3D10Texture2D*) m_pkD3D10Texture;
        if (pkTexture)
        {
            ID3D10Texture2D *pkStagingTexture = 
                (ID3D10Texture2D*)m_pkStagingTexture;

            if (pkStagingTexture==NULL)
                return false;

            pkStagingTexture->Unmap(D3D10CalcSubresource(0, 0, 1));

            // Renderer must exist.
            NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
            if (pkRenderer == NULL)
                return NULL;

            ID3D10Device *pkD3DDevice10 = pkRenderer->GetD3D10Device();
            if (pkD3DDevice10==NULL)
                return NULL;
            
            pkD3DDevice10->CopyResource(pkTexture,pkStagingTexture);
            return true;
        }
    }
    else if (IsTexture3D())
    {
        ID3D10Texture3D* pkTexture = (ID3D10Texture3D*) m_pkD3D10Texture;
        if (pkTexture)
        {
            ID3D10Texture3D *pkStagingTexture = 
                (ID3D10Texture3D*)m_pkStagingTexture;

            if (pkStagingTexture==NULL)
                return false;

            pkStagingTexture->Unmap(D3D10CalcSubresource(0, 0, 1));

            // Renderer must exist.
            NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
            if (pkRenderer == NULL)
                return NULL;

            ID3D10Device *pkD3DDevice10 = pkRenderer->GetD3D10Device();
            if (pkD3DDevice10==NULL)
                return NULL;
            
            pkD3DDevice10->CopyResource(pkTexture,pkStagingTexture);
            return true;
        }
    }
    else
    {
        NiD3D10Error::ReportError(
            NiD3D10Error::NiD3D10ERROR_UNSUPPORTED_RESOURCE_LOCK_FAILED);
    }
    return false;
}

//---------------------------------------------------------------------------

void* NiD3D10DynamicTextureData::Texture1DLock()
{
    void* pvMem = NULL;
    if (!m_pkD3D10Texture)
        return NULL;

    if (!IsTexture1D())
        return NULL;

    ID3D10Texture1D* pkTexture = (ID3D10Texture1D*) m_pkD3D10Texture;
    if (pkTexture)
    {        
        ID3D10Texture1D* pkStagingTexture = 
            (ID3D10Texture1D*)m_pkStagingTexture;

        if (pkStagingTexture)
        {
            if (!FAILED(pkStagingTexture->Map(D3D10CalcSubresource(0, 0, 1),
                m_kMapFlags, 0, &pvMem )))
            {
                return pvMem;
            }
        }
    }

    NiD3D10Error::ReportError(
        NiD3D10Error::NiD3D10ERROR_TEXTURE1D_LOCK_FAILED);
    
    return NULL;
}

//---------------------------------------------------------------------------

void* NiD3D10DynamicTextureData::Texture2DLock(int& iPitch)
{
    void* pvMem = NULL;
    if (!m_pkD3D10Texture)
        return NULL;

    if (!IsTexture2D()&& !IsCubeMap())
        return NULL;

    D3D10_MAPPED_TEXTURE2D kMappedTex;
    ID3D10Texture2D* pkTexture = (ID3D10Texture2D*) m_pkD3D10Texture;
    if (pkTexture)
    {        
        ID3D10Texture2D* pkStagingTexture = 
            (ID3D10Texture2D*)m_pkStagingTexture;

        if (pkStagingTexture)
        {
            if (!FAILED(pkStagingTexture->Map(D3D10CalcSubresource(0, 0, 1),
                m_kMapFlags, 0, &kMappedTex )))
            {
                pvMem = (void*)kMappedTex.pData;
                iPitch = kMappedTex.RowPitch;
                return pvMem;
            }
        }
    }

    NiD3D10Error::ReportError(
        NiD3D10Error::NiD3D10ERROR_TEXTURE2D_LOCK_FAILED);
    
    return NULL;
}

//---------------------------------------------------------------------------

void* NiD3D10DynamicTextureData::Texture3DLock(int& iRowPitch,int& iDepthPitch)
{
    void* pvMem = NULL;
    if (!m_pkD3D10Texture)
        return NULL;

    if (!IsTexture3D())
        return NULL;

    D3D10_MAPPED_TEXTURE3D kMappedTex;
    ID3D10Texture3D* pkTexture = (ID3D10Texture3D*) m_pkD3D10Texture;

    if (pkTexture)
    {   

        ID3D10Texture3D* pkStagingTexture = 
            (ID3D10Texture3D*)m_pkStagingTexture;

        if (pkStagingTexture)
        {
            if (!FAILED(pkStagingTexture->Map(D3D10CalcSubresource(0, 0, 1),
                m_kMapFlags, 0, &kMappedTex )))
            {
                pvMem = (void*)kMappedTex.pData;
                iRowPitch = kMappedTex.RowPitch;
                iDepthPitch = kMappedTex.DepthPitch;
                return pvMem;
            }
        }
    }
        
    NiD3D10Error::ReportError(
        NiD3D10Error::NiD3D10ERROR_TEXTURE3D_LOCK_FAILED);
    
    return NULL;
}

//---------------------------------------------------------------------------
