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

#include "NiDX9Renderer.h"
#include "NiDX9ShaderConstantManager.h"

//---------------------------------------------------------------------------
NiDX9ShaderConstantManager::NiDX9ShaderConstantManager(
    NiD3DRenderer* pkD3DRenderer, const D3DCAPS9& kD3DCaps9) :
    m_pkD3DRenderer(pkD3DRenderer),
    m_pfFloatVSConstantData(NULL),
    m_pfSavedFloatVSConstantData(NULL),
    m_pfFloatPSConstantData(NULL),
    m_pfSavedFloatPSConstantData(NULL),
    m_uiFirstDirtyFloatVSReg(0),
    m_uiFirstCleanFloatVSReg(0),
    m_uiFirstDirtyFloatPSReg(0),
    m_uiFirstCleanFloatPSReg(0),
    m_uiNumFloatVSConstants(0),
    m_uiNumFloatPSConstants(0),
    m_piIntVSConstantData(NULL),
    m_piSavedIntVSConstantData(NULL),
    m_piIntPSConstantData(NULL),
    m_piSavedIntPSConstantData(NULL),
    m_uiFirstDirtyIntVSReg(0),
    m_uiFirstCleanIntVSReg(0),
    m_uiFirstDirtyIntPSReg(0),
    m_uiFirstCleanIntPSReg(0),
    m_uiNumIntVSConstants(0),
    m_uiNumIntPSConstants(0),
    m_pbBoolVSConstantData(NULL),
    m_pbSavedBoolVSConstantData(NULL),
    m_pbBoolPSConstantData(NULL),
    m_pbSavedBoolPSConstantData(NULL),
    m_uiFirstDirtyBoolVSReg(0),
    m_uiFirstCleanBoolVSReg(0),
    m_uiFirstDirtyBoolPSReg(0),
    m_uiFirstCleanBoolPSReg(0),
    m_uiNumBoolVSConstants(0),
    m_uiNumBoolPSConstants(0)
{
    if (m_pkD3DRenderer)
        m_pkD3DDevice = pkD3DRenderer->GetD3DDevice();
    if (m_pkD3DDevice)
        D3D_POINTER_REFERENCE(m_pkD3DDevice);

    m_uiNumFloatVSConstants = kD3DCaps9.MaxVertexShaderConst;
    unsigned int uiVertexShaderVersion = kD3DCaps9.VertexShaderVersion;

    unsigned int uiMajorVSVersion = 
        D3DSHADER_VERSION_MAJOR(uiVertexShaderVersion);

    // Keep this updated as future shader versions are released
    if (uiMajorVSVersion == 1)
    {
        m_uiNumBoolVSConstants = 0;
        m_uiNumIntVSConstants = 0;
    }
    else if (uiMajorVSVersion == 2)
    {
        // This is an absolutely awful way for DirectX to operate.
        if (kD3DCaps9.VS20Caps.DynamicFlowControlDepth == 0)
            m_uiNumBoolPSConstants = 0;
        else
            m_uiNumBoolPSConstants = 16;

        m_uiNumIntVSConstants = 16;
    }
    else if (uiMajorVSVersion == 3)
    {
        m_uiNumBoolVSConstants = 16;
        m_uiNumIntVSConstants = 16;
    }

    unsigned int uiPixelShaderVersion = kD3DCaps9.PixelShaderVersion;

    unsigned int uiMajorPSVersion = 
        D3DSHADER_VERSION_MAJOR(uiPixelShaderVersion);

    // Keep this updated as future shader versions are released
    if (uiMajorPSVersion == 1)
    {
        m_uiNumFloatPSConstants = 8;
        m_uiNumBoolPSConstants = 0;
        m_uiNumIntPSConstants = 0;
    }
    else if (uiMajorPSVersion == 2)
    {
        m_uiNumFloatPSConstants = 32;

        // This is an absolutely awful way for DirectX to operate.
        if (kD3DCaps9.PS20Caps.DynamicFlowControlDepth == 0)
            m_uiNumBoolPSConstants = 0;
        else
            m_uiNumBoolPSConstants = 16;

        if (kD3DCaps9.PS20Caps.StaticFlowControlDepth == 0)
            m_uiNumIntPSConstants = 16;
        else
            m_uiNumIntPSConstants = 16;
    }
    else if (uiMajorPSVersion == 3)
    {
        m_uiNumFloatPSConstants = 224;
        m_uiNumBoolPSConstants = 16;
        m_uiNumIntPSConstants = 16;
    }

    // Mark all registers as clean
    m_uiFirstDirtyFloatVSReg = m_uiNumFloatVSConstants;
    m_uiFirstDirtyFloatPSReg = m_uiNumFloatPSConstants;
    m_uiFirstDirtyIntVSReg = m_uiNumIntVSConstants;
    m_uiFirstDirtyIntPSReg = m_uiNumIntPSConstants;
    m_uiFirstDirtyBoolVSReg = m_uiNumBoolVSConstants;
    m_uiFirstDirtyBoolPSReg = m_uiNumBoolPSConstants;

    if (m_uiNumFloatVSConstants > 0)
    {
        unsigned int uiNumElements = m_uiNumFloatVSConstants * 4;
        m_pfFloatVSConstantData = NiAlignedAlloc(float, uiNumElements, 16);
        memset(m_pfFloatVSConstantData, 0, 
            uiNumElements * sizeof(*m_pfFloatVSConstantData));

        m_pfSavedFloatVSConstantData = 
            NiAlignedAlloc(float, uiNumElements, 16);
        memset(m_pfSavedFloatVSConstantData, 0, 
            uiNumElements * sizeof(*m_pfSavedFloatVSConstantData));
    }

    if (m_uiNumFloatPSConstants > 0)
    {
        unsigned int uiNumElements = m_uiNumFloatPSConstants * 4;
        m_pfFloatPSConstantData = NiAlignedAlloc(float, uiNumElements, 16);
        memset(m_pfFloatPSConstantData, 0, 
            uiNumElements * sizeof(*m_pfFloatPSConstantData));

        m_pfSavedFloatPSConstantData = 
            NiAlignedAlloc(float, uiNumElements, 16);
        memset(m_pfSavedFloatPSConstantData, 0, 
            uiNumElements * sizeof(*m_pfSavedFloatPSConstantData));
    }

    if (m_uiNumBoolVSConstants > 0)
    {
        m_pbBoolVSConstantData = NiAlloc(BOOL, m_uiNumBoolVSConstants);
        memset(m_pbBoolVSConstantData, 0, 
            m_uiNumBoolVSConstants * sizeof(*m_pbBoolVSConstantData));

        m_pbSavedBoolVSConstantData = NiAlloc(BOOL, m_uiNumBoolVSConstants);
        memset(m_pbSavedBoolVSConstantData, 0, 
            m_uiNumBoolVSConstants * sizeof(*m_pbSavedBoolVSConstantData));
    }

    if (m_uiNumBoolPSConstants > 0)
    {
        m_pbBoolPSConstantData = NiAlloc(BOOL, m_uiNumBoolPSConstants);
        memset(m_pbBoolPSConstantData, 0, 
            m_uiNumBoolPSConstants * sizeof(*m_pbBoolPSConstantData));

        m_pbSavedBoolPSConstantData = NiAlloc(BOOL, m_uiNumBoolPSConstants);
        memset(m_pbSavedBoolPSConstantData, 0, 
            m_uiNumBoolPSConstants * sizeof(*m_pbSavedBoolPSConstantData));
    }

    if (m_uiNumIntVSConstants > 0)
    {
        unsigned int uiNumElements = m_uiNumIntVSConstants * 4;
        m_piIntVSConstantData = NiAlignedAlloc(int, uiNumElements, 16);
        memset(m_piIntVSConstantData, 0, 
            uiNumElements * sizeof(*m_piIntVSConstantData));

        m_piSavedIntVSConstantData = NiAlignedAlloc(int, uiNumElements, 16);
        memset(m_piSavedIntVSConstantData, 0, 
            uiNumElements * sizeof(*m_piSavedIntVSConstantData));
    }

    if (m_uiNumIntPSConstants > 0)
    {
        unsigned int uiNumElements = m_uiNumIntPSConstants * 4;
        m_piIntPSConstantData = NiAlignedAlloc(int, uiNumElements, 16);
        memset(m_piIntPSConstantData, 0, 
            uiNumElements * sizeof(*m_piIntPSConstantData));

        m_piSavedIntPSConstantData = NiAlignedAlloc(int, uiNumElements, 16);
        memset(m_piSavedIntPSConstantData, 0, 
            uiNumElements * sizeof(*m_piSavedIntPSConstantData));
    }
}
//---------------------------------------------------------------------------
NiDX9ShaderConstantManager::~NiDX9ShaderConstantManager()
{
    NiAlignedFree(m_pfFloatVSConstantData);
    NiAlignedFree(m_pfSavedFloatVSConstantData);
    NiAlignedFree(m_pfFloatPSConstantData);
    NiAlignedFree(m_pfSavedFloatPSConstantData);
    NiAlignedFree(m_piIntVSConstantData);
    NiAlignedFree(m_piSavedIntVSConstantData);
    NiAlignedFree(m_piIntPSConstantData);
    NiAlignedFree(m_piSavedIntPSConstantData);
    NiFree(m_pbBoolVSConstantData);
    NiFree(m_pbSavedBoolVSConstantData);
    NiFree(m_pbBoolPSConstantData);
    NiFree(m_pbSavedBoolPSConstantData);

    if (m_pkD3DDevice)
    {
        NiD3DRenderer::ReleaseDevice(m_pkD3DDevice);
        m_pkD3DDevice = NULL;
    }
}
//---------------------------------------------------------------------------
NiDX9ShaderConstantManager* NiDX9ShaderConstantManager::Create(
    NiD3DRenderer* pkRenderer, const D3DCAPS9& kD3DCaps9)
{
    return NiNew NiDX9ShaderConstantManager(pkRenderer, kD3DCaps9);
}
//---------------------------------------------------------------------------
void NiDX9ShaderConstantManager::CommitChanges()
{
    if (m_uiFirstDirtyFloatVSReg < m_uiFirstCleanFloatVSReg)
    {
        HRESULT hr =
            m_pkD3DDevice->SetVertexShaderConstantF(
            m_uiFirstDirtyFloatVSReg, 
            m_pfFloatVSConstantData + m_uiFirstDirtyFloatVSReg * 4,
            m_uiFirstCleanFloatVSReg - m_uiFirstDirtyFloatVSReg);
        NI_UNUSED_ARG(hr);
        NIASSERT(SUCCEEDED(hr));
    }

    if (m_uiFirstDirtyFloatPSReg < m_uiFirstCleanFloatPSReg)
    {
        HRESULT hr = 
            m_pkD3DDevice->SetPixelShaderConstantF(
            m_uiFirstDirtyFloatPSReg, 
            m_pfFloatPSConstantData + m_uiFirstDirtyFloatPSReg * 4,
            m_uiFirstCleanFloatPSReg - m_uiFirstDirtyFloatPSReg);
        NI_UNUSED_ARG(hr);
        NIASSERT(SUCCEEDED(hr));
    }

    if (m_uiFirstDirtyBoolVSReg < m_uiFirstCleanBoolVSReg)
    {
        HRESULT hr =
            m_pkD3DDevice->SetVertexShaderConstantB(
            m_uiFirstDirtyBoolVSReg, 
            (BOOL*)(m_pbBoolVSConstantData + m_uiFirstDirtyBoolVSReg),
            m_uiFirstCleanBoolVSReg - m_uiFirstDirtyBoolVSReg);
        NI_UNUSED_ARG(hr);
        NIASSERT(SUCCEEDED(hr));
    }

    if (m_uiFirstDirtyBoolPSReg < m_uiFirstCleanBoolPSReg)
    {
        HRESULT hr =
            m_pkD3DDevice->SetPixelShaderConstantB(
            m_uiFirstDirtyBoolPSReg, 
            (BOOL*)(m_pbBoolPSConstantData + m_uiFirstDirtyBoolPSReg),
            m_uiFirstCleanBoolPSReg - m_uiFirstDirtyBoolPSReg);
        NI_UNUSED_ARG(hr);
        NIASSERT(SUCCEEDED(hr));
    }

    if (m_uiFirstDirtyIntVSReg < m_uiFirstCleanIntVSReg)
    {
        HRESULT hr = 
            m_pkD3DDevice->SetVertexShaderConstantI(
            m_uiFirstDirtyIntVSReg, 
            m_piIntVSConstantData + m_uiFirstDirtyIntVSReg * 4,
            m_uiFirstCleanIntVSReg - m_uiFirstDirtyIntVSReg);
        NI_UNUSED_ARG(hr);
        NIASSERT(SUCCEEDED(hr));
    }

    if (m_uiFirstDirtyIntPSReg < m_uiFirstCleanIntPSReg)
    {
        HRESULT hr = 
            m_pkD3DDevice->SetPixelShaderConstantI(
            m_uiFirstDirtyIntPSReg, 
            m_piIntPSConstantData + m_uiFirstDirtyIntPSReg * 4,
            m_uiFirstCleanIntPSReg - m_uiFirstDirtyIntPSReg);
        NI_UNUSED_ARG(hr);
        NIASSERT(SUCCEEDED(hr));
    }

    // Mark all registers as clean
    m_uiFirstDirtyFloatVSReg = m_uiNumFloatVSConstants;
    m_uiFirstDirtyFloatPSReg = m_uiNumFloatPSConstants;
    m_uiFirstDirtyIntVSReg = m_uiNumIntVSConstants;
    m_uiFirstDirtyIntPSReg = m_uiNumIntPSConstants;
    m_uiFirstDirtyBoolVSReg = m_uiNumBoolVSConstants;
    m_uiFirstDirtyBoolPSReg = m_uiNumBoolPSConstants;
    m_uiFirstCleanFloatVSReg = 0;
    m_uiFirstCleanFloatPSReg = 0;
    m_uiFirstCleanIntVSReg = 0;
    m_uiFirstCleanIntPSReg = 0;
    m_uiFirstCleanBoolVSReg = 0;
    m_uiFirstCleanBoolPSReg = 0;

}
//---------------------------------------------------------------------------
