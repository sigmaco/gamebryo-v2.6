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
#include "NiD3DXEffectShaderLibPCH.h"

#include "NiD3DXEffectStateManager.h"
#include <NiD3DPass.h>

#if defined(WIN32) || defined(_XENON)
#pragma warning( disable: 4268 )
#endif
const GUID IID_NiD3DXEffectStateManager; 
#if defined(WIN32) || defined(_XENON)
#pragma warning( default: 4268 )
#endif

NiD3DXEffectStateManager* NiD3DXEffectStateManager::ms_pkManager = NULL;

//---------------------------------------------------------------------------
NiD3DXEffectStateManager::NiD3DXEffectStateManager(
    NiD3DRenderState* pkRenderStateManager, LPDIRECT3DDEVICE9 pkDevice) :
    m_pkRenderStateManager(pkRenderStateManager),
    m_pkDevice(pkDevice),
    m_iLastTextureStage(-1),
    m_lRefCount(0)
{
    m_pkDevice->AddRef();
}
//---------------------------------------------------------------------------
NiD3DXEffectStateManager::~NiD3DXEffectStateManager()
{
    m_pkDevice->Release();
    ms_pkManager = NULL;
}
//---------------------------------------------------------------------------
NiD3DXEffectStateManager* NiD3DXEffectStateManager::GetInstance(
    NiD3DRenderer* pkRenderer)
{
    if (ms_pkManager == NULL)
    {
        NiD3DRenderState* pkRSManager = NULL;
        LPDIRECT3DDEVICE9 pkDevice = NULL;
        if (pkRenderer == NULL || 
            (pkRSManager = pkRenderer->GetRenderState()) == NULL ||
            (pkDevice = pkRenderer->GetD3DDevice()) == NULL)
        {
            return NULL;
        }

        ms_pkManager = NiExternalNew 
            NiD3DXEffectStateManager(pkRSManager, pkDevice);
    }

    return ms_pkManager;
}
//---------------------------------------------------------------------------
HRESULT NiD3DXEffectStateManager::QueryInterface(REFIID iid, LPVOID* ppv)
{
    if (iid == IID_IUnknown || iid == IID_ID3DXEffectStateManager)
    {
        *ppv = static_cast<ID3DXEffectStateManager*>(this);
    } 
    else
    {
        *ppv = NULL;
        return E_NOINTERFACE;
    }
    
    reinterpret_cast<IUnknown*>(this)->AddRef();
    return S_OK;
}
//---------------------------------------------------------------------------
ULONG NiD3DXEffectStateManager::AddRef()
{
    return (ULONG)InterlockedIncrement( &m_lRefCount );
}
//---------------------------------------------------------------------------
ULONG NiD3DXEffectStateManager::Release()
{
    if( 0L == InterlockedDecrement( &m_lRefCount ) )
    {
        NiExternalDelete this;
        return 0L;
    }

    return m_lRefCount;
}
//---------------------------------------------------------------------------

    // The following methods are called by the Effect when it wants to make 
    // the corresponding device call.  Note that:
    // 1. Users manage the state and are therefore responsible for making the 
    //    the corresponding device calls themselves inside their callbacks.  
    // 2. Effects pay attention to the return values of the callbacks, and so 
    //    users must pay attention to what they return in their callbacks.

HRESULT NiD3DXEffectStateManager::SetTransform(D3DTRANSFORMSTATETYPE eState, 
    const D3DMATRIX* pkMatrix)
{
#if !defined(_XENON)
    return m_pkDevice->SetTransform(eState, pkMatrix);
#else   //#if !defined(_XENON)
    return S_OK;
#endif  //#if !defined(_XENON)
}
//---------------------------------------------------------------------------
HRESULT NiD3DXEffectStateManager::SetMaterial(const D3DMATERIAL9* pkMaterial)
{
#if !defined(_XENON)
    return m_pkDevice->SetMaterial(pkMaterial);
#else   //#if !defined(_XENON)
    return S_OK;
#endif  //#if !defined(_XENON)
}
//---------------------------------------------------------------------------
HRESULT NiD3DXEffectStateManager::SetLight(DWORD uiIndex, 
    const D3DLIGHT9* pkLight)
{
#if !defined(_XENON)
    return m_pkDevice->SetLight(uiIndex, pkLight);
#else   //#if !defined(_XENON)
    return S_OK;
#endif  //#if !defined(_XENON)
}
//---------------------------------------------------------------------------
HRESULT NiD3DXEffectStateManager::LightEnable(DWORD uiIndex, BOOL bEnable)
{
#if !defined(_XENON)
    return m_pkDevice->LightEnable(uiIndex, bEnable);
#else   //#if !defined(_XENON)
    return S_OK;
#endif  //#if !defined(_XENON)
}
//---------------------------------------------------------------------------
HRESULT NiD3DXEffectStateManager::SetRenderState(D3DRENDERSTATETYPE eState, 
    DWORD uiValue)
{
    m_pkRenderStateManager->SetRenderState(eState, uiValue);

    return S_OK;
}
//---------------------------------------------------------------------------
HRESULT NiD3DXEffectStateManager::SetTexture(DWORD uiStage, 
    D3DBaseTexturePtr pkTexture)
{
    m_pkRenderStateManager->SetTexture(uiStage, pkTexture);

    // Record highest sampler that uses a texture
    if ((int)uiStage > m_iLastTextureStage)
        m_iLastTextureStage = (int)uiStage;

    return S_OK;
}
//---------------------------------------------------------------------------
#if defined(_XENON)
//---------------------------------------------------------------------------
HRESULT NiD3DXEffectStateManager::Do_Not_Use_SetTextureStageState(
    DWORD,DWORD,DWORD)
{
    return S_OK;
}
#else   //#if defined(_XENON)
//---------------------------------------------------------------------------
HRESULT NiD3DXEffectStateManager::SetTextureStageState(DWORD uiStage, 
    D3DTEXTURESTAGESTATETYPE eType, DWORD uiValue)
{
    m_pkRenderStateManager->SetTextureStageState(uiStage, eType, uiValue);

    return S_OK;
}
//---------------------------------------------------------------------------
#endif  //#if defined(_XENON)
//---------------------------------------------------------------------------
HRESULT NiD3DXEffectStateManager::SetSamplerState(DWORD uiSampler, 
    D3DSAMPLERSTATETYPE eType, DWORD uiValue)
{

#if defined (WIN32)
    ((NiDX9RenderState*)m_pkRenderStateManager)->SetSamplerState(
#elif defined (_XENON)
    ((NiXenonRenderState*)m_pkRenderStateManager)->SetSamplerState(
#endif
        uiSampler, eType, uiValue);

    return S_OK;
}
//---------------------------------------------------------------------------
HRESULT NiD3DXEffectStateManager::SetNPatchMode(FLOAT fNumSegments)
{
#if !defined(_XENON)
    return m_pkDevice->SetNPatchMode(fNumSegments);
#else
    return S_OK;
#endif
}
//---------------------------------------------------------------------------
HRESULT NiD3DXEffectStateManager::SetFVF(DWORD uiFVF)
{
    m_pkRenderStateManager->SetFVF(uiFVF);

    return S_OK;
}
//---------------------------------------------------------------------------
HRESULT NiD3DXEffectStateManager::SetVertexShader(
    LPDIRECT3DVERTEXSHADER9 pkShader)
{
    m_pkRenderStateManager->SetVertexShader(pkShader);

    return S_OK;
}
//---------------------------------------------------------------------------
HRESULT NiD3DXEffectStateManager::SetVertexShaderConstantF(
    UINT uiRegisterIndex, const FLOAT* pfConstantData, UINT uiRegisterCount)
{
    if (m_pkRenderStateManager->SetVertexShaderConstantF(uiRegisterIndex, 
        pfConstantData, uiRegisterCount))
    {
        return S_OK;
    }
    
    return D3DERR_INVALIDCALL;
}
//---------------------------------------------------------------------------
HRESULT NiD3DXEffectStateManager::SetVertexShaderConstantI(
    UINT uiRegisterIndex, const INT* piConstantData, UINT uiRegisterCount)
{
    if (m_pkRenderStateManager->SetVertexShaderConstantI(uiRegisterIndex, 
        piConstantData, uiRegisterCount))
    {
        return S_OK;
    }
    
    return D3DERR_INVALIDCALL;
}
//---------------------------------------------------------------------------
HRESULT NiD3DXEffectStateManager::SetVertexShaderConstantB(
    UINT uiRegisterIndex, const BOOL* pbConstantData, UINT uiRegisterCount)
{
    if (m_pkRenderStateManager->SetVertexShaderConstantB(uiRegisterIndex, 
        pbConstantData, uiRegisterCount))
    {
        return S_OK;
    }
    
    return D3DERR_INVALIDCALL;
}
//---------------------------------------------------------------------------
HRESULT NiD3DXEffectStateManager::SetPixelShader(
    LPDIRECT3DPIXELSHADER9 pkShader)
{
    m_pkRenderStateManager->SetPixelShader(pkShader);

    return S_OK;
}
//---------------------------------------------------------------------------
HRESULT NiD3DXEffectStateManager::SetPixelShaderConstantF(
    UINT uiRegisterIndex, const FLOAT* pfConstantData, UINT uiRegisterCount)
{
    if (m_pkRenderStateManager->SetPixelShaderConstantF(uiRegisterIndex, 
        pfConstantData, uiRegisterCount))
    {
        return S_OK;
    }
    
    return D3DERR_INVALIDCALL;
}
//---------------------------------------------------------------------------
HRESULT NiD3DXEffectStateManager::SetPixelShaderConstantI(
    UINT uiRegisterIndex, const INT* piConstantData, UINT uiRegisterCount)
{
    if (m_pkRenderStateManager->SetPixelShaderConstantI(uiRegisterIndex, 
        piConstantData, uiRegisterCount))
    {
        return S_OK;
    }
    
    return D3DERR_INVALIDCALL;
}
//---------------------------------------------------------------------------
HRESULT NiD3DXEffectStateManager::SetPixelShaderConstantB(
    UINT uiRegisterIndex, const BOOL* pbConstantData, UINT uiRegisterCount)
{
    if (m_pkRenderStateManager->SetPixelShaderConstantB(uiRegisterIndex, 
        pbConstantData, uiRegisterCount))
    {
        return S_OK;
    }
    
    return D3DERR_INVALIDCALL;
}
//---------------------------------------------------------------------------
void NiD3DXEffectStateManager::ResetLastTextureStage()
{
    m_iLastTextureStage = -1;
}
//---------------------------------------------------------------------------
int NiD3DXEffectStateManager::GetLastTextureStage() const
{
    return m_iLastTextureStage;
}
//---------------------------------------------------------------------------
