// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
//  software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2008 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

#ifndef NID3DXEFFECTSTATEMANAGER_H
#define NID3DXEFFECTSTATEMANAGER_H

#include "NiD3DXEffectShaderLibType.h"
#include <NiD3DRendererHeaders.h>

typedef interface NiD3DXEffectStateManager NiD3DXEffectStateManager;

// {0000C0EC-DB5D-4822-8D9A-65F78D2530CE}
DEFINE_GUID(IID_NiD3DXEffectStateManager, 
0xc0ec, 0xdb5d, 0x4822, 0x8d, 0x9a, 0x65, 0xf7, 0x8d, 0x25, 0x30, 0xce);

interface NID3DXEFFECTSHADER_ENTRY NiD3DXEffectStateManager : 
    public ID3DXEffectStateManager
{
public:
    // The user must correctly implement QueryInterface, AddRef, and Release.

    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, LPVOID* pkpv);
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();

    // The following methods are called by the Effect when it wants to make 
    // the corresponding device call.  Note that:
    // 1. Users manage the state and are therefore responsible for making the 
    //    the corresponding device calls themselves inside their callbacks.  
    // 2. Effects pay attention to the return values of the callbacks, and so 
    //    users must pay attention to what they return in their callbacks.

    virtual HRESULT STDMETHODCALLTYPE SetTransform(
        D3DTRANSFORMSTATETYPE eState, const D3DMATRIX* pkMatrix);
    virtual HRESULT STDMETHODCALLTYPE SetMaterial(
        const D3DMATERIAL9* pkMaterial);
    virtual HRESULT STDMETHODCALLTYPE SetLight(DWORD uiIndex, 
        const D3DLIGHT9* pkLight);
    virtual HRESULT STDMETHODCALLTYPE LightEnable(DWORD uiIndex, BOOL bEnable);
    virtual HRESULT STDMETHODCALLTYPE SetRenderState(D3DRENDERSTATETYPE State, 
        DWORD uiValue);
    virtual HRESULT STDMETHODCALLTYPE SetTexture(DWORD uiStage, 
        D3DBaseTexturePtr pTexture);
#if defined(_XENON)
    virtual HRESULT Do_Not_Use_SetTextureStageState(DWORD,DWORD,DWORD);
#else   //#if defined(_XENON)
    virtual HRESULT STDMETHODCALLTYPE SetTextureStageState(DWORD uiStage, 
        D3DTEXTURESTAGESTATETYPE Type, DWORD uiValue);
#endif  //#if defined(_XENON)
    virtual HRESULT STDMETHODCALLTYPE SetSamplerState(DWORD uiSampler, 
        D3DSAMPLERSTATETYPE Type, DWORD uiValue);
    virtual HRESULT STDMETHODCALLTYPE SetNPatchMode(FLOAT fNumSegments);
    virtual HRESULT STDMETHODCALLTYPE SetFVF(DWORD uiFVF);
    virtual HRESULT STDMETHODCALLTYPE SetVertexShader(
        LPDIRECT3DVERTEXSHADER9 pShader);
    virtual HRESULT STDMETHODCALLTYPE SetVertexShaderConstantF(
        UINT uiRegisterIndex, const FLOAT* pfConstantData, 
        UINT uiRegisterCount);
    virtual HRESULT STDMETHODCALLTYPE SetVertexShaderConstantI(
        UINT uiRegisterIndex, const INT* piConstantData, 
        UINT uiRegisterCount);
    virtual HRESULT STDMETHODCALLTYPE SetVertexShaderConstantB(
        UINT uiRegisterIndex, const BOOL* pbConstantData, 
        UINT uiRegisterCount);
    virtual HRESULT STDMETHODCALLTYPE SetPixelShader(
        LPDIRECT3DPIXELSHADER9 pShader);
    virtual HRESULT STDMETHODCALLTYPE SetPixelShaderConstantF(
        UINT uiRegisterIndex, const FLOAT* pfConstantData, 
        UINT uiRegisterCount);
    virtual HRESULT STDMETHODCALLTYPE SetPixelShaderConstantI(
        UINT uiRegisterIndex, const INT* piConstantData, 
        UINT uiRegisterCount);
    virtual HRESULT STDMETHODCALLTYPE SetPixelShaderConstantB(
        UINT uiRegisterIndex, const BOOL* pbConstantData, 
        UINT uiRegisterCount);
    
    void ResetLastTextureStage();
    int GetLastTextureStage() const;

    static NiD3DXEffectStateManager* GetInstance(NiD3DRenderer* pkRenderer);

protected:
    NiD3DXEffectStateManager(NiD3DRenderState* pkRenderStateManager,
        LPDIRECT3DDEVICE9 pkDevice);
    virtual ~NiD3DXEffectStateManager();

    int m_iLastTextureStage;

    long m_lRefCount;

    NiD3DRenderState* m_pkRenderStateManager;
    LPDIRECT3DDEVICE9 m_pkDevice;

    static NiD3DXEffectStateManager* ms_pkManager;
};

#endif // NID3DXEFFECTSTATEMANAGER_H
