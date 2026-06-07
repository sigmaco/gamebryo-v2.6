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
//---------------------------------------------------------------------------
#ifndef NIDX92DBUFFERDATA_H
#define NIDX92DBUFFERDATA_H

#include "NiDX9Headers.h"
#include "NiDX9RendererLibType.h"
#include <Ni2DBuffer.h>
#include <NiCriticalSection.h>

class NiPixelFormat;
class NiDepthStencilBuffer;

//---------------------------------------------------------------------------
// Abstract base class designed to represent all rendering targets
class NIDX9RENDERER_ENTRY NiDX92DBufferData  : 
    public Ni2DBuffer::RendererData
{
    NiDeclareRootRTTI(NiDX92DBufferData);
public:
    virtual ~NiDX92DBufferData();

    static void GetMSAAD3DTypeAndQualityFromPref(
        Ni2DBuffer::MultiSamplePreference ePref, 
        D3DMULTISAMPLE_TYPE& eType, unsigned int& uiQuality);
    static Ni2DBuffer::MultiSamplePreference GetMSAAPrefFromD3DTypeAndQuality(
        D3DMULTISAMPLE_TYPE eType, unsigned int uiQuality);

    // *** begin Emergent internal use only ***
    inline virtual bool CanDisplayFrame() { return false; }
    inline virtual bool DisplayFrame() { return false; }

    virtual bool Shutdown();
    // Rebuilds the surfaces after a device reset - returns false if the
    // target could not be successfully reset at the time of the call
    // In such cases, this call will leave the object with IsValid false
    virtual bool Recreate(LPDIRECT3DDEVICE9 pkD3DDevice9);

    virtual bool SetRenderTarget(LPDIRECT3DDEVICE9 pkD3DDevice9, 
        unsigned int uiTarget = 0);
    virtual bool SetDepthTarget(LPDIRECT3DDEVICE9 pkD3DDevice9);
    static void ClearRenderTarget(LPDIRECT3DDEVICE9 pkD3DDevice9, 
        unsigned int uiTarget);

    inline bool IsValid() { return m_pkSurface ? true : false; }

    inline LPDIRECT3DSURFACE9 GetSurface() { return m_pkSurface; }

    // *** end Emergent internal use only ***

protected:
    NiDX92DBufferData();
    virtual void ReleaseReferences();
    void ReleaseRTReferences();

    LPDIRECT3DSURFACE9 m_pkSurface;

    static LPDIRECT3DSURFACE9 ms_apkCurrentBuffer[4];
    static LPDIRECT3DSURFACE9 ms_pkCurrentDepthStencil;

    static unsigned int ms_uiNumBuffers;
    static bool ms_bIndependentBitDepths;
};
//---------------------------------------------------------------------------
// Class representing textures to which the system can render
class NIDX9RENDERER_ENTRY NiDX9TextureBufferData : 
    public NiDX92DBufferData
{
    NiDeclareRTTI;
public:
    virtual ~NiDX9TextureBufferData();

    // *** begin Emergent internal use only ***

    // Various creation methods.  
    // IMPORTANT NOTE: All of these pass in a  Ni2DBuffer* reference. 
    // Since DX9 requires that you create the texture prior to creating
    // the buffer, the buffer and its renderer data are populated by 
    // the Create call. Note that if the pkBuffer is NULL, a new buffer
    // is created. If it is not stored in a smart pointer, it will be 
    // LEAKED by the application.
    static NiDX9TextureBufferData* Create(LPDIRECT3DTEXTURE9 pkD3DTexture,
        LPDIRECT3DDEVICE9 pkD3DDevice9, Ni2DBuffer*& pkBuffer);
    static NiDX9TextureBufferData* Create(LPDIRECT3DTEXTURE9 pkD3DTexture, 
        Ni2DBuffer*& pkBuffer);
    static NiDX9TextureBufferData* CreateCubeFace(
        LPDIRECT3DCUBETEXTURE9 pkD3DCubeTexture, D3DCUBEMAP_FACES eFace, 
        NiDX9TextureBufferData* pkExistingFace, 
        LPDIRECT3DDEVICE9 pkD3DDevice9, Ni2DBuffer*& pkBuffer);
    static NiDX9TextureBufferData* CreateCubeFace(
        LPDIRECT3DCUBETEXTURE9 pkD3DCubeTexture, D3DCUBEMAP_FACES eFace, 
        NiDX9TextureBufferData* pkExistingFace, Ni2DBuffer*& pkBuffer);

    // *** end Emergent internal use only ***

protected:
    NiDX9TextureBufferData();
    virtual void ReleaseReferences();
    void ReleaseTRTReferences();

    LPDIRECT3DBASETEXTURE9 m_pkRenderTexture;
};
//---------------------------------------------------------------------------
// Class representing a manual color buffer
class NIDX9RENDERER_ENTRY NiDX9Direct3DBufferData : public NiDX92DBufferData
{
    NiDeclareRTTI;
public:
    // IMPORTANT NOTE:The create method passes in a Ni2DBuffer* reference.
    // Since DX9 requires that you create the device prior to creating
    // the buffer, the buffer and its renderer data are populated by 
    // the Create call. Note that if the pkBuffer is NULL, a new buffer
    // is created. If it is not stored in a smart pointer, it will be 
    // LEAKED by the application.
    static NiDX9Direct3DBufferData* Create(LPDIRECT3DSURFACE9
        pkD3DSurface, Ni2DBuffer*& pkBuffer);

    void PurgeBufferData();
    bool RecreateBufferData(LPDIRECT3DSURFACE9 pkD3DSurface);
};
//---------------------------------------------------------------------------
// Class representing a depth/stencil buffer
class NIDX9RENDERER_ENTRY NiDX9DepthStencilBufferData : 
    public NiDX92DBufferData
{
    NiDeclareRTTI;
public:
    // *** begin Emergent internal use only ***
    virtual ~NiDX9DepthStencilBufferData();

    virtual bool SetDepthTarget(LPDIRECT3DDEVICE9 pkD3DDevice9);
    static bool SetNULLDepthStencilTarget(LPDIRECT3DDEVICE9 pkD3DDevice9);
    bool HasValidDepthBuffer();
    bool HasValidStencilBuffer();

    // *** end Emergent internal use only ***
};
//---------------------------------------------------------------------------
// Class representing a depth/stencil buffer
class NIDX9RENDERER_ENTRY NiDX9ImplicitDepthStencilBufferData : 
    public NiDX9DepthStencilBufferData
{
    NiDeclareRTTI;
public:
    // *** begin Emergent internal use only ***

    virtual bool Recreate(LPDIRECT3DDEVICE9 pkD3DDevice9);

    // IMPORTANT NOTE:The create method passes in a Ni2DBuffer* reference.
    // Since DX9 requires that you create the device prior to creating
    // the buffer, the buffer and its renderer data are populated by 
    // the Create call. Note that if the pkBuffer is NULL, a new buffer
    // is created. If it is not stored in a smart pointer, it will be 
    // LEAKED by the application.
    static NiDX9ImplicitDepthStencilBufferData* Create(LPDIRECT3DDEVICE9
        pkD3DDevice9, NiDepthStencilBuffer*& pkBuffer);
    // *** end Emergent internal use only ***
};
//---------------------------------------------------------------------------
// Class representing a manual depth/stencil buffer
class NIDX9RENDERER_ENTRY NiDX9Direct3DDepthStencilBufferData : 
    public NiDX9DepthStencilBufferData
{
    NiDeclareRTTI;
public:
    // IMPORTANT NOTE:The create method passes in a Ni2DBuffer* reference.
    // Since DX9 requires that you create the device prior to creating
    // the buffer, the buffer and its renderer data are populated by 
    // the Create call. Note that if the pkBuffer is NULL, a new buffer
    // is created. If it is not stored in a smart pointer, it will be 
    // LEAKED by the application.
    static NiDX9Direct3DDepthStencilBufferData* Create(LPDIRECT3DSURFACE9
        pkD3DDepthStencilSurface, NiDepthStencilBuffer*& pkBuffer);

    void PurgeBufferData();
    bool RecreateBufferData(LPDIRECT3DSURFACE9 pkD3DDepthStencilSurface);
};
//---------------------------------------------------------------------------
// Class representing a depth/stencil buffer
class NIDX9RENDERER_ENTRY NiDX9AdditionalDepthStencilBufferData : 
    public NiDX9DepthStencilBufferData
{
    NiDeclareRTTI;
public:
    virtual ~NiDX9AdditionalDepthStencilBufferData();

    // *** begin Emergent internal use only ***

    virtual bool Recreate(LPDIRECT3DDEVICE9 pkD3DDevice9);

    // IMPORTANT NOTE:The create method passes in a Ni2DBuffer* reference.
    // Since DX9 requires that you create the device prior to creating
    // the buffer, the buffer and its renderer data are populated by 
    // the Create call. Note that if the pkBuffer is NULL, a new buffer
    // is created. If it is not stored in a smart pointer, it will be 
    // LEAKED by the application.
    static NiDX9AdditionalDepthStencilBufferData* Create(
        LPDIRECT3DDEVICE9 pkD3DDevice9, NiDepthStencilBuffer*& pkBuffer,
        const NiPixelFormat* pkFormat, 
        Ni2DBuffer::MultiSamplePreference eMSAAPref);

    static void ShutdownAll();
    static void RecreateAll(LPDIRECT3DDEVICE9 pkD3DDevice9);

    // *** end Emergent internal use only ***

protected:
    D3DFORMAT m_eCreateFormat;
    NiDX9AdditionalDepthStencilBufferData();
    static NiCriticalSection ms_kAddDSListCriticalSection;
    static NiTPointerList<NiDX9AdditionalDepthStencilBufferData*> 
        ms_kAddDepthStencil;
};
//---------------------------------------------------------------------------
// Abstract class representing rendering targets that are visible on-screen,
// either windowed or fullscreen
class NIDX9RENDERER_ENTRY NiDX9OnscreenBufferData : 
    public NiDX92DBufferData
{
    NiDeclareRTTI;
public:
    // *** begin Emergent internal use only ***
    // Rebuilds the surfaces after a device reset - returns false if the
    // target could not be successfully reset at the time of the call
    // In such cases, this call will leave the object with IsValid false
    virtual bool Recreate(LPDIRECT3DDEVICE9 pkD3DDevice9) = 0;

    inline D3DPRESENT_PARAMETERS& GetPresentParams() 
    { return m_kD3DPresentParams; }
    // *** end Emergent internal use only ***

protected:
    D3DPRESENT_PARAMETERS m_kD3DPresentParams;
};
//---------------------------------------------------------------------------
// Class representing the render target that is created along with a device,
// which is considered "implicit", as the device owns a special reference to 
// it
class NIDX9RENDERER_ENTRY NiDX9ImplicitBufferData : 
    public NiDX9OnscreenBufferData
{
    NiDeclareRTTI;
public:
    virtual ~NiDX9ImplicitBufferData();
    
    // *** begin Emergent internal use only ***
    inline virtual bool CanDisplayFrame() { return true; }
    virtual bool DisplayFrame();

    virtual bool Recreate(LPDIRECT3DDEVICE9 pkD3DDevice9);

    // Creates a new render target object, but only queries the existing 
    // buffers from the device - it does not create new ones. The Ni2DBuffer
    // reference's renderer data is populated by this method. Note that if 
    // the pkBuffer is NULL, a new buffer is created. If it is not stored in 
    // a smart pointer, it will be LEAKED by the application.
    static NiDX9ImplicitBufferData* Create(LPDIRECT3DDEVICE9 pkD3DDevice9,
        const D3DPRESENT_PARAMETERS& kD3DPresentParams, 
        Ni2DBuffer*& pkBuffer);
    // *** end Emergent internal use only ***

protected:
    NiDX9ImplicitBufferData();
    virtual void ReleaseReferences();
    void ReleaseIRTReferences();

    LPDIRECT3DDEVICE9 m_pkD3DDevice9;
};
//---------------------------------------------------------------------------
// Class representing a render target that was created after the device,
// generally as a secondary window.  Such render targets are created from
// swap chain objects, and are thus called SwapChain render targets
class NIDX9RENDERER_ENTRY NiDX9SwapChainBufferData : 
    public NiDX9OnscreenBufferData
{
    NiDeclareRTTI;
public:
    virtual ~NiDX9SwapChainBufferData();

    // *** begin Emergent internal use only ***
    inline virtual bool CanDisplayFrame() { return true; }
    virtual bool DisplayFrame();

    virtual bool Recreate(LPDIRECT3DDEVICE9 pkD3DDevice9);

    // Creates a new target for the given device based on the given creation
    // parameters, including backbuffers and a depth/stencil surface.
    // Returns NULL on failure. The Ni2DBuffer
    // reference's renderer data is populated by this method.
    static NiDX9SwapChainBufferData* Create(
        LPDIRECT3DDEVICE9 pkD3DDevice9, 
        const D3DPRESENT_PARAMETERS& kD3DPresentParams, 
        Ni2DBuffer*& pkBuffer);
    // *** end Emergent internal use only ***

protected:
    NiDX9SwapChainBufferData();
    virtual void ReleaseReferences();
    void ReleaseSCRTReferences();

    // The swap chain contains references to all of the backbuffers for the
    // target.  The base class m_pkSurface holds a reference to the
    // first backbuffer, which it uses for rendering. 
    LPDIRECT3DSWAPCHAIN9 m_pkSwapChain;
};
//---------------------------------------------------------------------------
// Class representing a depth/stencil buffer
class NIDX9RENDERER_ENTRY NiDX9SwapChainDepthStencilBufferData : 
    public NiDX9DepthStencilBufferData
{
    NiDeclareRTTI;
public:
    // *** begin Emergent internal use only ***
    virtual bool Recreate(LPDIRECT3DDEVICE9 pkD3DDevice9);

    // Create method for a swap buffer's depth/stencil buffer
    // Note that the Ni2DBuffer reference's renderer data is populated by 
    // this method. Note that if the pkBuffer is NULL, a new buffer
    // is created. If it is not stored in a smart pointer, it will be 
    // LEAKED by the application.
    static NiDX9SwapChainDepthStencilBufferData* Create(LPDIRECT3DDEVICE9
        pkD3DDevice9, NiDX9SwapChainBufferData* pSwapChainBuffer, 
        NiDepthStencilBuffer*& pkBuffer);
    // *** end Emergent internal use only ***

protected:
    NiDX9SwapChainBufferData* m_pkSwapChainBufferData;
};
//---------------------------------------------------------------------------
#endif // NIDX92DBUFFERDATA_H
