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

#include "NiD3DTextureStage.h"
#include "NiD3DDefines.h"
#include "NiD3DRendererHeaders.h"
#include "NiD3DShaderFactory.h"

//---------------------------------------------------------------------------
D3DDevicePtr NiD3DTextureStage::ms_pkD3DDevice = 0;
NiD3DRenderer* NiD3DTextureStage::ms_pkD3DRenderer = 0;
NiD3DRenderState* NiD3DTextureStage::ms_pkD3DRenderState = 0;
D3DXMATRIX NiD3DTextureStage::ms_kCalculatedTextureTransformation;

NiCriticalSection NiD3DTextureStage::ms_kTextureStagePoolCriticalSection;
NiTObjectPool<NiD3DTextureStage>* NiD3DTextureStage::ms_pkTextureStagePool = 
    NULL;

//---------------------------------------------------------------------------
void NiD3DTextureStage::SetD3DRenderer(NiD3DRenderer* pkD3DRenderer)
{
    ms_pkD3DRenderer = pkD3DRenderer;
    if (ms_pkD3DRenderer)
    {
        SetD3DDevice(ms_pkD3DRenderer->GetD3DDevice());
        SetD3DRenderState(ms_pkD3DRenderer->GetRenderState());
    }
    else
    {
        SetD3DDevice(0);
        SetD3DRenderState(0);
    }
}
//---------------------------------------------------------------------------
void NiD3DTextureStage::SetD3DDevice(D3DDevicePtr pkD3DDevice)
{
    if (ms_pkD3DDevice)
        NiD3DRenderer::ReleaseDevice(ms_pkD3DDevice);
    ms_pkD3DDevice = pkD3DDevice;
    if (ms_pkD3DDevice)
        D3D_POINTER_REFERENCE(ms_pkD3DDevice);
}
//---------------------------------------------------------------------------
void NiD3DTextureStage::SetD3DRenderState(NiD3DRenderState* pkRS)
{
    ms_pkD3DRenderState = pkRS;
}
//---------------------------------------------------------------------------
NiD3DTextureStage::NiD3DTextureStage() :
    NiTextureStage(),
    m_uiStage(0), 
    m_pkTexture(0),
    m_uiTextureFlags(0), 
    m_uiTextureTransformFlags(TSTTF_IGNORE), 
    m_pkGlobalEntry(0), 
    m_bTextureTransform(false), 
    m_bRendererOwned(false),
    m_usObjTextureFlags(0)
{
    m_pkTextureStageGroup = 
        NiD3DTextureStageGroup::GetFreeTextureStageGroup();
}
//---------------------------------------------------------------------------
void NiD3DTextureStage::InitializePools()
{
    ms_kTextureStagePoolCriticalSection.Lock();
    NIASSERT(ms_pkTextureStagePool == NULL);
    ms_pkTextureStagePool = NiNew NiTObjectPool<NiD3DTextureStage>;
    ms_kTextureStagePoolCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
void NiD3DTextureStage::ShutdownPools()
{
    ms_kTextureStagePoolCriticalSection.Lock();
    NiDelete ms_pkTextureStagePool;
    ms_pkTextureStagePool = NULL;
    ms_kTextureStagePoolCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
NiD3DTextureStagePtr NiD3DTextureStage::CreateNewStage()
{
    ms_kTextureStagePoolCriticalSection.Lock();
    NIASSERT(ms_pkTextureStagePool != NULL);
    NiD3DTextureStagePtr spReturn = ms_pkTextureStagePool->GetFreeObject();
    ms_kTextureStagePoolCriticalSection.Unlock();

    spReturn->SetRendererOwned();

    return spReturn;
}
//---------------------------------------------------------------------------
void NiD3DTextureStage::ReturnStageToPool()
{
    if (!IsRendererOwned())
    {
        NiDelete this;
        return;
    }

    m_pkTexture = NULL;

    m_pkTextureStageGroup->RemoveAllStageStates();
    m_pkTextureStageGroup->RemoveAllSamplerStates();

    if (m_pkGlobalEntry)
    {
        NiShaderFactory::ReleaseGlobalShaderConstant(
            m_pkGlobalEntry->GetKey());
    }
    m_pkGlobalEntry = NULL;
    m_uiStage = 0; 
    m_uiTextureFlags = 0; 
    m_usObjTextureFlags = 0;
    m_uiTextureTransformFlags = TSTTF_IGNORE; 
    m_bTextureTransform = false; 

    ms_kTextureStagePoolCriticalSection.Lock();
    NIASSERT(ms_pkTextureStagePool != NULL);
    ms_pkTextureStagePool->ReleaseObject(this);
    ms_kTextureStagePoolCriticalSection.Unlock();
}
//---------------------------------------------------------------------------
NiD3DTextureStage::~NiD3DTextureStage()
{
    if (m_pkGlobalEntry)
    {
        NiShaderFactory::ReleaseGlobalShaderConstant(
            m_pkGlobalEntry->GetKey());
    }
    NiD3DTextureStageGroup::ReleaseTextureStageGroup(m_pkTextureStageGroup);
}
//---------------------------------------------------------------------------
void NiD3DTextureStage::SetTextureTransformFlags(unsigned int uiFlags, 
    const char* pszGlobalName)
{
    if (uiFlags != TSTTF_IGNORE)
        m_bTextureTransform = true;

    m_uiTextureTransformFlags = uiFlags;
    if (pszGlobalName && ((uiFlags & TSTTF_SOURCE_MASK) == TSTTF_GLOBAL))
    {
        // Look up the global in the renderer
        m_pkGlobalEntry = NiD3DShaderFactory::GetD3DShaderFactory()->
            GetGlobalShaderConstantEntry(pszGlobalName);
        if (!m_pkGlobalEntry)
        {
            NIASSERT(!"Unable to find texture transform global!");
        }

        NIASSERT(m_pkGlobalEntry->IsMatrix4());

        // Increment the reference count
        m_pkGlobalEntry->IncRefCount();
    }
    else
    {
        if (m_pkGlobalEntry)
        {
            // Tell the renderer to release this instance
            NiShaderFactory::ReleaseGlobalShaderConstant(
                m_pkGlobalEntry->GetKey());
//            m_pkGlobalEntry->DecRefCount();
            m_pkGlobalEntry = 0;
        }
    }
}
//---------------------------------------------------------------------------
void NiD3DTextureStage::SetClampMode(NiTexturingProperty::ClampMode eClamp)
{
    SetSamplerState((unsigned int)NiD3DRenderState::NISAMP_ADDRESSU,
        ms_pkD3DRenderer->GetD3DClampMode(eClamp).m_eU);
    SetSamplerState((unsigned int)NiD3DRenderState::NISAMP_ADDRESSV,
        ms_pkD3DRenderer->GetD3DClampMode(eClamp).m_eV);
}
//---------------------------------------------------------------------------
void NiD3DTextureStage::SetFilterMode(
    NiTexturingProperty::FilterMode eFilter)
{
    SetSamplerState((unsigned int)NiD3DRenderState::NISAMP_MAGFILTER, 
        ms_pkD3DRenderer->GetD3DFilterMode(eFilter).m_eMag);
    SetSamplerState((unsigned int)NiD3DRenderState::NISAMP_MINFILTER, 
        ms_pkD3DRenderer->GetD3DFilterMode(eFilter).m_eMin);
    SetSamplerState((unsigned int)NiD3DRenderState::NISAMP_MIPFILTER, 
        ms_pkD3DRenderer->GetD3DFilterMode(eFilter).m_eMip);
}
//---------------------------------------------------------------------------
void NiD3DTextureStage::SetMaxAnisotropy(unsigned short usMaxAnisotropyValue)
{
    // The quality of the anisotropic filtering is adjusted by setting the
    // number of anisotropic texture taps. 
    // The maximum anisotropy should be set to the minimum of the renderer 
    // maximum and the value usMaxAnisotropyValue.
    SetSamplerState(
        (unsigned int)NiD3DRenderState::NISAMP_MAXANISOTROPY, 
        NiMin(ms_pkD3DRenderer->GetMaxAnisotropy(), usMaxAnisotropyValue));
}
//---------------------------------------------------------------------------
void NiD3DTextureStage::ModifyFilterMode(bool bMipmap, bool bAllowMiplerp)
{
    // Change values on the device
    if (!bMipmap)
    {
        ms_pkD3DRenderState->SetSamplerState(m_uiStage,
            NiD3DRenderState::NISAMP_MIPFILTER, D3DTEXF_NONE);
    }
    else if (!bAllowMiplerp)
    {
        unsigned int uiValue;
        bool bSave;
        bool bUseMapValue;
        
        if (m_pkTextureStageGroup->GetSamplerState(
            (unsigned int)NiD3DRenderState::NISAMP_MIPFILTER,
            uiValue, bSave, bUseMapValue))
        {
            if (uiValue == D3DTEXF_NONE)
            {
                ms_pkD3DRenderState->SetSamplerState(m_uiStage,
                    NiD3DRenderState::NISAMP_MIPFILTER, D3DTEXF_POINT);
            }
        }
        else
        {
            ms_pkD3DRenderState->SetSamplerState(m_uiStage,
                NiD3DRenderState::NISAMP_MIPFILTER, D3DTEXF_POINT);
        }
    }
}
//---------------------------------------------------------------------------
void NiD3DTextureStage::ModifyClampMode(bool bNonPow2)
{
    // Change values, if necessary, for the device.
    if (bNonPow2 &&
        ((ms_pkD3DRenderer->GetFlags() & 
        NiRenderer::CAPS_NONPOW2_TEXT) == 0) &&
        (ms_pkD3DRenderer->GetFlags() & 
        NiRenderer::CAPS_NONPOW2_CONDITIONAL_TEXT))
    {
        ms_pkD3DRenderState->SetSamplerState(m_uiStage,
            NiD3DRenderState::NISAMP_ADDRESSU, D3DTADDRESS_CLAMP);
        ms_pkD3DRenderState->SetSamplerState(m_uiStage,
            NiD3DRenderState::NISAMP_ADDRESSV, D3DTADDRESS_CLAMP);
        for (unsigned int i = 0; i < 8; i++)
        {
            ms_pkD3DRenderState->SetRenderState(
                (D3DRENDERSTATETYPE)(D3DRS_WRAP0 + i), 0);
            ms_pkD3DRenderState->SetRenderState(
                (D3DRENDERSTATETYPE)(D3DRS_WRAP8 + i), 0);
        }
    }
}
//---------------------------------------------------------------------------
bool NiD3DTextureStage::ConfigureStage()
{
    if (m_pkTextureStageGroup->SetTextureStage(m_uiStage) != NISHADERERR_OK)
        return false;
    // Set the texture for the stage. We do this AFTER the confuration of
    // the stage as it can override the texture coordinate index setting.
    ApplyTexture();

    return true;
}
//---------------------------------------------------------------------------
void NiD3DTextureStage::DisableStage()
{
    DisableStage(m_uiStage);
}
//---------------------------------------------------------------------------
void NiD3DTextureStage::DisableStage(unsigned int uiStage)
{
    while (uiStage < NiD3DPass::ms_uiMaxTextureBlendStages)
    {
        ms_pkD3DRenderState->SetTexture(uiStage, 0);
        ms_pkD3DRenderState->SetTextureStageState(uiStage, D3DTSS_COLOROP, 
            D3DTOP_DISABLE);
        ms_pkD3DRenderState->SetTextureStageState(uiStage, D3DTSS_ALPHAOP, 
            D3DTOP_DISABLE);
        ms_pkD3DRenderState->SetTextureStageState(uiStage,
            D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
        uiStage++;
    }

    while (uiStage < NiD3DPass::ms_uiMaxSamplers)
    {
        ms_pkD3DRenderState->SetTexture(uiStage, 0);
        uiStage++;
    }
}
//---------------------------------------------------------------------------
void NiD3DTextureStage::ApplyTextureTransform()
{
    if (m_uiStage >= NiD3DPass::ms_uiMaxTextureBlendStages)
        return;

    // NOTE there are cases where setting the texture coord mode when there 
    // is no texture will result in D3D errors and dropped objects.  The 
    // error states that the texture coord index is greater than the number 
    // of texture sets.  This is annoying, as in the case where we get the 
    // error, there is no texture to be mapped, and the D3D manual _clearly_
    // states that invalid tex coord set indices should simply result in 
    // texture coords of 0,0

    // note - this code assumes that texture transforms and automatically
    // generated texture coordinates are _always_ used together.  One implies
    // the other.  This is safe now, but if we decide to allow transforms of
    // app-supplied texture coords, this may not be true

    // set the basic index - this may be a texture stage index, but it may 
    // also be one of the following flags (for projected textures):
    // D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR or 
    // D3DTSS_TCI_CAMERASPACEPOSITION

    // now, set up the texture coordinate transforms if coordinate
    // transforms are enabled
    if (!m_bTextureTransform)
    {
        ms_pkD3DRenderState->SetTextureStageState(m_uiStage, 
            D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);
        return;
    }

    switch (m_uiTextureTransformFlags & TSTTF_SOURCE_MASK)
    {
    case TSTTF_IGNORE:
        {
            // warning - the following assumes that _TEXTUREX are 
            // contiguous
#if !defined(_XENON)
            ms_pkD3DDevice->SetTransform(
                (D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + m_uiStage), 
                &m_kTextureTransformation);
#endif  //#if !defined(_XENON)
        }
        break;
    case TSTTF_GLOBAL:
        {
            if ((m_uiTextureTransformFlags & ~TSTTF_SOURCE_MASK) == 0)
            {
                // Set it directly
                NIASSERT(m_pkGlobalEntry);
#if !defined(_XENON)
                ms_pkD3DDevice->SetTransform(
                    (D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + m_uiStage), 
                    (CONST D3DMATRIX*)m_pkGlobalEntry->GetDataSource());
#endif  //#if !defined(_XENON)
            }
            else
            {
                unsigned int uiCalculationMethod = 
                    m_uiTextureTransformFlags & ~TSTTF_SOURCE_MASK;
                D3DMATRIX* pkD3DMatSource = 
                    (D3DMATRIX*)m_pkGlobalEntry->GetDataSource();

                if (CalculateTransformationMatrix(uiCalculationMethod, 
                    pkD3DMatSource))
                {
#if !defined(_XENON)
                    ms_pkD3DDevice->SetTransform(
                        (D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + m_uiStage), 
                        &ms_kCalculatedTextureTransformation);
#endif  //#if !defined(_XENON)
                }
            }
        }
        break;
    case TSTTF_CONSTANT:
        {
            if ((m_uiTextureTransformFlags & ~TSTTF_SOURCE_MASK) == 0)
            {
                // Set it directly
                NIASSERT(m_pkGlobalEntry);
#if !defined(_XENON)
                ms_pkD3DDevice->SetTransform(
                    (D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + m_uiStage), 
                    &m_kTextureTransformation);
#endif  //#if !defined(_XENON)
            }
            else
            {
                unsigned int uiCalculationMethod = 
                    m_uiTextureTransformFlags & ~TSTTF_SOURCE_MASK;
                D3DMATRIX* pkD3DMatSource = &m_kTextureTransformation;

                if (CalculateTransformationMatrix(uiCalculationMethod, 
                    pkD3DMatSource))
                {
#if !defined(_XENON)
                    ms_pkD3DDevice->SetTransform(
                        (D3DTRANSFORMSTATETYPE)(D3DTS_TEXTURE0 + m_uiStage), 
                        &ms_kCalculatedTextureTransformation);
#endif  //#if !defined(_XENON)
                }
            }
        }
        break;
    }
}
//---------------------------------------------------------------------------
void NiD3DTextureStage::ApplyTexture()
{
    // This state may or may not have been set during the call to 
    // m_pkTextureStageGroup->SetTextureStage.
    // If it wasn't, make sure it gets done here. 
    // If it was, no harm is done, even if bSave should be true, because
    // the original value would have been saved in SetTextureStage.
    if (m_uiStage < NiD3DPass::ms_uiMaxTextureBlendStages)
    {
        ms_pkD3DRenderState->SetTextureStageState(m_uiStage, 
            D3DTSS_TEXCOORDINDEX, GetTexCoordIndex(), false);
    }
    D3DBaseTexturePtr pkD3DTexture = 0;

    bool bS0Mipmap = false;
    bool bNonPow2 = false;

    if (m_pkTexture)
    {
        bool bChanged; 
        pkD3DTexture = ms_pkD3DRenderer->GetTextureManager()->
            PrepareTextureForRendering(m_pkTexture, bChanged, bS0Mipmap,
            bNonPow2);
    }

    ms_pkD3DRenderState->SetTexture(m_uiStage, pkD3DTexture);
    if (pkD3DTexture)
    {
        ModifyFilterMode(bS0Mipmap, true);
        ModifyClampMode(bNonPow2);
        ApplyTextureTransform();
    }
}
//---------------------------------------------------------------------------
inline void NiD3DTextureStage::DeleteThis()
{
    ReturnStageToPool();
}
//---------------------------------------------------------------------------
bool NiD3DTextureStage::CalculateTransformationMatrix(
    unsigned int uiCalculationMethod, const D3DMATRIX* pkD3DMatSource)
{
    bool bResult = true;

    // All of these functions will set the matrix to be used in
    // ms_kCalculatedTextureTransformation.

    switch (uiCalculationMethod)
    {
    case TSTTF_NI_WORLD_PARALLEL:
        {
            // We need to ensure that:
            //     D3DTSS_TEXCOORDINDEX = D3DTSS_TCI_CAMERASPACEPOSITION
            // and
            //     D3DTSS_TEXTURETRANSFORMFLAGS = D3DTTFF_COUNT2

            // kMat._14, kMat._24, kMat._34 and kMat._44 are always 0.0  

            // cam matrix = kWorldMat * m_invView
            const D3DMATRIX& kD3DInvView = ms_pkD3DRenderer->GetInvView();

            ms_kCalculatedTextureTransformation._11 = 
                pkD3DMatSource->_11 * kD3DInvView._11 +  
                pkD3DMatSource->_21 * kD3DInvView._12 +  
                pkD3DMatSource->_31 * kD3DInvView._13; 
            ms_kCalculatedTextureTransformation._21 = 
                pkD3DMatSource->_11 * kD3DInvView._21 +  
                pkD3DMatSource->_21 * kD3DInvView._22 +  
                pkD3DMatSource->_31 * kD3DInvView._23; 
            ms_kCalculatedTextureTransformation._31 = 
                pkD3DMatSource->_11 * kD3DInvView._31 +  
                pkD3DMatSource->_21 * kD3DInvView._32 +  
                pkD3DMatSource->_31 * kD3DInvView._33; 

            ms_kCalculatedTextureTransformation._12 = 
                pkD3DMatSource->_12 * kD3DInvView._11 +  
                pkD3DMatSource->_22 * kD3DInvView._12 +  
                pkD3DMatSource->_32 * kD3DInvView._13; 
            ms_kCalculatedTextureTransformation._22 = 
                pkD3DMatSource->_12 * kD3DInvView._21 +  
                pkD3DMatSource->_22 * kD3DInvView._22 +  
                pkD3DMatSource->_32 * kD3DInvView._23; 
            ms_kCalculatedTextureTransformation._32 = 
                pkD3DMatSource->_12 * kD3DInvView._31 +  
                pkD3DMatSource->_22 * kD3DInvView._32 +  
                pkD3DMatSource->_32 * kD3DInvView._33; 

            ms_kCalculatedTextureTransformation._13 = 
                pkD3DMatSource->_13 * kD3DInvView._11 +  
                pkD3DMatSource->_23 * kD3DInvView._12 +  
                pkD3DMatSource->_33 * kD3DInvView._13; 
            ms_kCalculatedTextureTransformation._23 = 
                pkD3DMatSource->_13 * kD3DInvView._21 +  
                pkD3DMatSource->_23 * kD3DInvView._22 +  
                pkD3DMatSource->_33 * kD3DInvView._23; 
            ms_kCalculatedTextureTransformation._33 = 
                pkD3DMatSource->_13 * kD3DInvView._31 +  
                pkD3DMatSource->_23 * kD3DInvView._32 +  
                pkD3DMatSource->_33 * kD3DInvView._33; 

            // cam trans = kWorldMat * pkInvViewTrans + kWorldTrans
            ms_kCalculatedTextureTransformation._41 = 
                pkD3DMatSource->_11 * kD3DInvView._41 +  
                pkD3DMatSource->_21 * kD3DInvView._42 +  
                pkD3DMatSource->_31 * kD3DInvView._43 + pkD3DMatSource->_41; 
            ms_kCalculatedTextureTransformation._42 = 
                pkD3DMatSource->_12 * kD3DInvView._41 +  
                pkD3DMatSource->_22 * kD3DInvView._42 +  
                pkD3DMatSource->_32 * kD3DInvView._43 + pkD3DMatSource->_42; 
            ms_kCalculatedTextureTransformation._43 = 
                pkD3DMatSource->_13 * kD3DInvView._41 +  
                pkD3DMatSource->_23 * kD3DInvView._42 +  
                pkD3DMatSource->_33 * kD3DInvView._43 + pkD3DMatSource->_43; 

            ms_kCalculatedTextureTransformation._14 = 0.0f;
            ms_kCalculatedTextureTransformation._24 = 0.0f;
            ms_kCalculatedTextureTransformation._34 = 0.0f;
            ms_kCalculatedTextureTransformation._44 = 0.0f;
        }
        break;
    case TSTTF_NI_WORLD_PERSPECTIVE:
        {
            // We need to ensure that:
            //     D3DTSS_TEXCOORDINDEX = D3DTSS_TCI_CAMERASPACEPOSITION
            // and
            //     D3DTSS_TEXTURETRANSFORMFLAGS = D3DTTFF_COUNTn

            // kMat._14, kMat._24, kMat._34 and kMat._44 are always 0.0  

            // cam matrix = kWorldMat * kInvMat
            const D3DMATRIX& kD3DInvView = ms_pkD3DRenderer->GetInvView();

            ms_kCalculatedTextureTransformation._11 = 
                pkD3DMatSource->_11 * kD3DInvView._11 +  
                pkD3DMatSource->_21 * kD3DInvView._12 +  
                pkD3DMatSource->_31 * kD3DInvView._13; 
            ms_kCalculatedTextureTransformation._21 = 
                pkD3DMatSource->_11 * kD3DInvView._21 +  
                pkD3DMatSource->_21 * kD3DInvView._22 +  
                pkD3DMatSource->_31 * kD3DInvView._23; 
            ms_kCalculatedTextureTransformation._31 = 
                pkD3DMatSource->_11 * kD3DInvView._31 +  
                pkD3DMatSource->_21 * kD3DInvView._32 +  
                pkD3DMatSource->_31 * kD3DInvView._33; 

            ms_kCalculatedTextureTransformation._12 = 
                pkD3DMatSource->_12 * kD3DInvView._11 +  
                pkD3DMatSource->_22 * kD3DInvView._12 +  
                pkD3DMatSource->_32 * kD3DInvView._13; 
            ms_kCalculatedTextureTransformation._22 = 
                pkD3DMatSource->_12 * kD3DInvView._21 +  
                pkD3DMatSource->_22 * kD3DInvView._22 +  
                pkD3DMatSource->_32 * kD3DInvView._23; 
            ms_kCalculatedTextureTransformation._32 = 
                pkD3DMatSource->_12 * kD3DInvView._31 +  
                pkD3DMatSource->_22 * kD3DInvView._32 +  
                pkD3DMatSource->_32 * kD3DInvView._33; 

            ms_kCalculatedTextureTransformation._13 = 
                pkD3DMatSource->_13 * kD3DInvView._11 +  
                pkD3DMatSource->_23 * kD3DInvView._12 +  
                pkD3DMatSource->_33 * kD3DInvView._13; 
            ms_kCalculatedTextureTransformation._23 = 
                pkD3DMatSource->_13 * kD3DInvView._21 +  
                pkD3DMatSource->_23 * kD3DInvView._22 +  
                pkD3DMatSource->_33 * kD3DInvView._23; 
            ms_kCalculatedTextureTransformation._33 = 
                pkD3DMatSource->_13 * kD3DInvView._31 +  
                pkD3DMatSource->_23 * kD3DInvView._32 +  
                pkD3DMatSource->_33 * kD3DInvView._33; 

            // cam trans = kWorldMat * pkInvViewTrans + kWorldTrans
            ms_kCalculatedTextureTransformation._41 = 
                pkD3DMatSource->_11 * kD3DInvView._41 +  
                pkD3DMatSource->_21 * kD3DInvView._42 +  
                pkD3DMatSource->_31 * kD3DInvView._43 + pkD3DMatSource->_41; 
            ms_kCalculatedTextureTransformation._42 = 
                pkD3DMatSource->_12 * kD3DInvView._41 +  
                pkD3DMatSource->_22 * kD3DInvView._42 +  
                pkD3DMatSource->_32 * kD3DInvView._43 + pkD3DMatSource->_42; 
            ms_kCalculatedTextureTransformation._43 = 
                pkD3DMatSource->_13 * kD3DInvView._41 +  
                pkD3DMatSource->_23 * kD3DInvView._42 +  
                pkD3DMatSource->_33 * kD3DInvView._43 + pkD3DMatSource->_43; 

            ms_kCalculatedTextureTransformation._14 = 0.0f;
            ms_kCalculatedTextureTransformation._24 = 0.0f;
            ms_kCalculatedTextureTransformation._34 = 0.0f;
            ms_kCalculatedTextureTransformation._44 = 0.0f;
        }
        break;
    case TSTTF_NI_WORLD_SPHERE_MAP:
        {
            // We need to ensure that:
            //     D3DTSS_TEXCOORDINDEX = 
            //         D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR
            // and
            //     D3DTSS_TEXTURETRANSFORMFLAGS = D3DTTFF_COUNT2

            // kMat._14, kMat._24, kMat._34 and kMat._44 are always 0.0  

            // cam matrix = kWorldMat * kInvMat
            // D3DMatrices are transposed with respect to NiMatrix3.
            const D3DMATRIX& kD3DInvView = ms_pkD3DRenderer->GetInvView();

            ms_kCalculatedTextureTransformation._11 = 
                pkD3DMatSource->_11 * kD3DInvView._11 +  
                pkD3DMatSource->_21 * kD3DInvView._12 +  
                pkD3DMatSource->_31 * kD3DInvView._13; 
            ms_kCalculatedTextureTransformation._21 = 
                pkD3DMatSource->_11 * kD3DInvView._21 +  
                pkD3DMatSource->_21 * kD3DInvView._22 +  
                pkD3DMatSource->_31 * kD3DInvView._23; 
            ms_kCalculatedTextureTransformation._31 = 
                pkD3DMatSource->_11 * kD3DInvView._31 +  
                pkD3DMatSource->_21 * kD3DInvView._32 +  
                pkD3DMatSource->_31 * kD3DInvView._33; 

            ms_kCalculatedTextureTransformation._12 = 
                pkD3DMatSource->_12 * kD3DInvView._11 +  
                pkD3DMatSource->_22 * kD3DInvView._12 +  
                pkD3DMatSource->_32 * kD3DInvView._13; 
            ms_kCalculatedTextureTransformation._22 = 
                pkD3DMatSource->_12 * kD3DInvView._21 +  
                pkD3DMatSource->_22 * kD3DInvView._22 +  
                pkD3DMatSource->_32 * kD3DInvView._23; 
            ms_kCalculatedTextureTransformation._32 = 
                pkD3DMatSource->_12 * kD3DInvView._31 +  
                pkD3DMatSource->_22 * kD3DInvView._32 +  
                pkD3DMatSource->_32 * kD3DInvView._33; 

            ms_kCalculatedTextureTransformation._13 = 
                pkD3DMatSource->_13 * kD3DInvView._11 +  
                pkD3DMatSource->_23 * kD3DInvView._12 +  
                pkD3DMatSource->_33 * kD3DInvView._13; 
            ms_kCalculatedTextureTransformation._23 = 
                pkD3DMatSource->_13 * kD3DInvView._21 +  
                pkD3DMatSource->_23 * kD3DInvView._22 +  
                pkD3DMatSource->_33 * kD3DInvView._23; 
            ms_kCalculatedTextureTransformation._33 = 
                pkD3DMatSource->_13 * kD3DInvView._31 +  
                pkD3DMatSource->_23 * kD3DInvView._32 +  
                pkD3DMatSource->_33 * kD3DInvView._33; 

            // cam trans is used directly
            ms_kCalculatedTextureTransformation._41 = pkD3DMatSource->_41;
            ms_kCalculatedTextureTransformation._42 = pkD3DMatSource->_42;
            ms_kCalculatedTextureTransformation._43 = pkD3DMatSource->_43;

            ms_kCalculatedTextureTransformation._14 = 0.0f;
            ms_kCalculatedTextureTransformation._24 = 0.0f;
            ms_kCalculatedTextureTransformation._34 = 0.0f;
            ms_kCalculatedTextureTransformation._44 = 0.0f;
        }
        break;
    case TSTTF_NI_CAMERA_SPHERE_MAP:
        {
            // We need to ensure that:
            //     D3DTSS_TEXCOORDINDEX = 
            //         D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR
            // and
            //     D3DTSS_TEXTURETRANSFORMFLAGS = D3DTTFF_COUNT2

            // This is always the same matrix in camera space
            memset((void*)&ms_kCalculatedTextureTransformation, 0, 
                sizeof(ms_kCalculatedTextureTransformation));

            ms_kCalculatedTextureTransformation._22 = 0.5f;
            ms_kCalculatedTextureTransformation._31 = 0.5f;
            
            ms_kCalculatedTextureTransformation._41 = 0.5f;
            ms_kCalculatedTextureTransformation._42 = 0.5f;
        }
        break;
    case TSTTF_NI_SPECULAR_CUBE_MAP:
        {
            // We need to ensure that:
            //     D3DTSS_TEXCOORDINDEX = 
            //         D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR
            // and
            //     D3DTSS_TEXTURETRANSFORMFLAGS = D3DTTFF_COUNT3

            // kMat._14, kMat._24, kMat._34 and kMat._44 are always 0.0  

            // cam matrix = kWorldMat * kInvMat
            // D3DMatrices are transposed with respect to NiMatrix3.
            const D3DMATRIX& kD3DInvView = ms_pkD3DRenderer->GetInvView();

            ms_kCalculatedTextureTransformation._11 = 
                pkD3DMatSource->_11 * kD3DInvView._11 +  
                pkD3DMatSource->_21 * kD3DInvView._12 +  
                pkD3DMatSource->_31 * kD3DInvView._13; 
            ms_kCalculatedTextureTransformation._21 = 
                pkD3DMatSource->_11 * kD3DInvView._21 +  
                pkD3DMatSource->_21 * kD3DInvView._22 +  
                pkD3DMatSource->_31 * kD3DInvView._23; 
            ms_kCalculatedTextureTransformation._31 = 
                pkD3DMatSource->_11 * kD3DInvView._31 +  
                pkD3DMatSource->_21 * kD3DInvView._32 +  
                pkD3DMatSource->_31 * kD3DInvView._33; 

            ms_kCalculatedTextureTransformation._12 = 
                pkD3DMatSource->_12 * kD3DInvView._11 +  
                pkD3DMatSource->_22 * kD3DInvView._12 +  
                pkD3DMatSource->_32 * kD3DInvView._13; 
            ms_kCalculatedTextureTransformation._22 = 
                pkD3DMatSource->_12 * kD3DInvView._21 +  
                pkD3DMatSource->_22 * kD3DInvView._22 +  
                pkD3DMatSource->_32 * kD3DInvView._23; 
            ms_kCalculatedTextureTransformation._32 = 
                pkD3DMatSource->_12 * kD3DInvView._31 +  
                pkD3DMatSource->_22 * kD3DInvView._32 +  
                pkD3DMatSource->_32 * kD3DInvView._33; 

            ms_kCalculatedTextureTransformation._13 = 
                pkD3DMatSource->_13 * kD3DInvView._11 +  
                pkD3DMatSource->_23 * kD3DInvView._12 +  
                pkD3DMatSource->_33 * kD3DInvView._13; 
            ms_kCalculatedTextureTransformation._23 = 
                pkD3DMatSource->_13 * kD3DInvView._21 +  
                pkD3DMatSource->_23 * kD3DInvView._22 +  
                pkD3DMatSource->_33 * kD3DInvView._23; 
            ms_kCalculatedTextureTransformation._33 = 
                pkD3DMatSource->_13 * kD3DInvView._31 +  
                pkD3DMatSource->_23 * kD3DInvView._32 +  
                pkD3DMatSource->_33 * kD3DInvView._33; 

            // cam trans is used directly
            ms_kCalculatedTextureTransformation._41 = pkD3DMatSource->_41; 
            ms_kCalculatedTextureTransformation._42 = pkD3DMatSource->_42; 
            ms_kCalculatedTextureTransformation._43 = pkD3DMatSource->_43; 

            ms_kCalculatedTextureTransformation._14 = 0.0f;
            ms_kCalculatedTextureTransformation._24 = 0.0f;
            ms_kCalculatedTextureTransformation._34 = 0.0f;
            ms_kCalculatedTextureTransformation._44 = 0.0f;
        }
        break;
    case TSTTF_NI_DIFFUSE_CUBE_MAP:
        {
            // We need to ensure that:
            //     D3DTSS_TEXCOORDINDEX = D3DTSS_TCI_CAMERASPACENORMAL
            // and
            //     D3DTSS_TEXTURETRANSFORMFLAGS = D3DTTFF_COUNT3

            // kMat._14, kMat._24, kMat._34 and kMat._44 are always 0.0  

            // cam matrix = kWorldMat * kInvMat
            // D3DMatrices are transposed with respect to NiMatrix3.
            const D3DMATRIX& kD3DInvView = ms_pkD3DRenderer->GetInvView();

            ms_kCalculatedTextureTransformation._11 = 
                pkD3DMatSource->_11 * kD3DInvView._11 +  
                pkD3DMatSource->_21 * kD3DInvView._12 +  
                pkD3DMatSource->_31 * kD3DInvView._13; 
            ms_kCalculatedTextureTransformation._21 = 
                pkD3DMatSource->_11 * kD3DInvView._21 +  
                pkD3DMatSource->_21 * kD3DInvView._22 +  
                pkD3DMatSource->_31 * kD3DInvView._23; 
            ms_kCalculatedTextureTransformation._31 = 
                pkD3DMatSource->_11 * kD3DInvView._31 +  
                pkD3DMatSource->_21 * kD3DInvView._32 +  
                pkD3DMatSource->_31 * kD3DInvView._33; 

            ms_kCalculatedTextureTransformation._12 = 
                pkD3DMatSource->_12 * kD3DInvView._11 +  
                pkD3DMatSource->_22 * kD3DInvView._12 +  
                pkD3DMatSource->_32 * kD3DInvView._13; 
            ms_kCalculatedTextureTransformation._22 = 
                pkD3DMatSource->_12 * kD3DInvView._21 +  
                pkD3DMatSource->_22 * kD3DInvView._22 +  
                pkD3DMatSource->_32 * kD3DInvView._23; 
            ms_kCalculatedTextureTransformation._32 = 
                pkD3DMatSource->_12 * kD3DInvView._31 +  
                pkD3DMatSource->_22 * kD3DInvView._32 +  
                pkD3DMatSource->_32 * kD3DInvView._33; 

            ms_kCalculatedTextureTransformation._13 = 
                pkD3DMatSource->_13 * kD3DInvView._11 +  
                pkD3DMatSource->_23 * kD3DInvView._12 +  
                pkD3DMatSource->_33 * kD3DInvView._13; 
            ms_kCalculatedTextureTransformation._23 = 
                pkD3DMatSource->_13 * kD3DInvView._21 +  
                pkD3DMatSource->_23 * kD3DInvView._22 +  
                pkD3DMatSource->_33 * kD3DInvView._23; 
            ms_kCalculatedTextureTransformation._33 = 
                pkD3DMatSource->_13 * kD3DInvView._31 +  
                pkD3DMatSource->_23 * kD3DInvView._32 +  
                pkD3DMatSource->_33 * kD3DInvView._33; 

            // cam trans is used directly
            ms_kCalculatedTextureTransformation._41 = pkD3DMatSource->_41; 
            ms_kCalculatedTextureTransformation._42 = pkD3DMatSource->_42; 
            ms_kCalculatedTextureTransformation._43 = pkD3DMatSource->_43; 

            ms_kCalculatedTextureTransformation._14 = 0.0f;
            ms_kCalculatedTextureTransformation._24 = 0.0f;
            ms_kCalculatedTextureTransformation._34 = 0.0f;
            ms_kCalculatedTextureTransformation._44 = 0.0f;
        }
        break;
    default:
        bResult = false;
        break;
    }
    
    return bResult;
}
//---------------------------------------------------------------------------
