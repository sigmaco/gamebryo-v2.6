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
// Precompiled Header
#include "NiD3DXEffectShaderLibPCH.h"

#include "NiD3DXEffectFile.h"
#include "NiD3DXEffectFactory.h"
#include "NiD3DXEffectParameter.h"
#include "NiD3DXEffectParser.h"
#include "NiD3DXEffectShader.h"
#include "NiD3DXEffectTechniqueDesc.h"

#include <NiShaderDesc.h>
#include <NiD3DUtility.h>
#include <NiD3DTextureStage.h>
#include <NiD3DRendererHeaders.h>
#include <NiD3DShaderFactory.h>

//---------------------------------------------------------------------------
NiD3DXEffectFile::NiD3DXEffectFile() :
    m_pcName(NULL),
    m_uiDXVersion(0),
    m_kTechniqueIter(NULL),
    m_pkCurrTechniqueSet(NULL),
    m_uiIndex(0),
    m_pkD3DXEffect(NULL),
    m_bRenderableEffect(false),
    m_bParsed(false)
{ /* */ }
//---------------------------------------------------------------------------
NiD3DXEffectFile::~NiD3DXEffectFile()
{
    NiD3DUtility::Log("Killing NiD3DXEffectFile %s\n", m_pcName);

    // NiD3DXEffectFactory won't exist if it has already been shut down.
    // In this case, there's no need to remove the effect from the map.
    NiD3DXEffectFactory* pkEffectFactory = 
        NiD3DXEffectFactory::GetInstance(false);
    if (pkEffectFactory)
        pkEffectFactory->RemoveD3DXEffectFileFromMap(m_pcName);

    DestroyRendererData();

    NiFree(m_pcName);

    NiTStringMap<NiD3DXEffectTechniqueSetPtr> m_kTechniques;

    m_kTechniqueIter = m_kTechniques.GetFirstPos();
    while (m_kTechniqueIter)
    {
        const char* pcName;
        NiD3DXEffectTechniqueSetPtr spTechniqueSet;
        m_kTechniques.GetNext(m_kTechniqueIter, pcName, spTechniqueSet);
        m_kTechniques.SetAt(pcName, NULL);
    }
    m_kTechniques.RemoveAll();

    NiTListIterator kParamIter = m_kParameters.GetHeadPos();
    while (kParamIter)
    {
        NiD3DXEffectParameter* pkParam = m_kParameters.GetNext(kParamIter);
        NiDelete pkParam;
    }
    m_kParameters.RemoveAll();

}
//---------------------------------------------------------------------------
bool NiD3DXEffectFile::Initialize(LPD3DXBASEEFFECT pkD3DXEffect,
    D3DDevicePtr pkD3DDevice)
{
    if (m_pkD3DXEffect == pkD3DXEffect)
        return false;

    if (m_pkD3DXEffect)
    {
        m_pkD3DXEffect->Release();
        m_pkD3DXEffect = NULL;
        m_bRenderableEffect = false;
    }

    m_pkD3DXEffect = pkD3DXEffect;

    if (!ParseEffect(pkD3DDevice))
        return false;

    if (pkD3DDevice)
    {
        m_bRenderableEffect = true;
        return LinkHandles();
    }

    return true;
}
/*//---------------------------------------------------------------------------
NiD3DXEffectShader* NiD3DXEffectFile::GetD3DShader(NiD3DRenderer* pkRenderer, 
    const char* pcName, unsigned int uiImplementation)
{
    // Before a shader can be returned, it needs to have the effect created
    LoadEffect(pkRenderer);

    NiD3DXEffectTechniqueSetPtr spTechniqueSet;
    m_kTechniques.GetAt(pcName, spTechniqueSet);

    if (spTechniqueSet)
        return spTechniqueSet->GetD3DShader(pkRenderer, uiImplementation);

    return NULL;
}
//---------------------------------------------------------------------------
bool NiD3DXEffectFile::ReleaseShader(const char* pcName, 
    unsigned int uiImplementation)
{
    NiD3DXEffectTechniqueSetPtr spTechniqueSet;
    m_kTechniques.GetAt(pcName, spTechniqueSet);

    if (spTechniqueSet == NULL)
        return false;

    NiD3DXEffectTechnique* pkTechnique = 
        spTechniqueSet->GetTechnique(uiImplementation);
    if (pkTechnique == NULL)
        return false;

    return pkTechnique->ReleaseShader();
}*/
//---------------------------------------------------------------------------
void NiD3DXEffectFile::SetName(const char* pcName)
{
    NiD3DUtility::SetString(m_pcName, 0, pcName);
}
//---------------------------------------------------------------------------
NiD3DXEffectTechnique* NiD3DXEffectFile::GetFirstTechnique()
{
    m_kTechniqueIter = m_kTechniques.GetFirstPos();
    if (m_kTechniqueIter == NULL)
        return NULL;
    m_pkCurrTechniqueSet = NULL;
    m_uiIndex = 0;
    return GetNextTechnique();
}
//---------------------------------------------------------------------------
NiD3DXEffectTechnique* NiD3DXEffectFile::GetNextTechnique()
{
    while (m_kTechniqueIter)
    {
        if (m_pkCurrTechniqueSet == NULL)
        {
            const char* pcName = NULL;
            NiD3DXEffectTechniqueSetPtr spTechniqueSet;
            m_kTechniques.GetNext(m_kTechniqueIter, pcName, spTechniqueSet);
            m_pkCurrTechniqueSet = spTechniqueSet;
        }
        NIASSERT(m_pkCurrTechniqueSet);
        while (m_uiIndex < m_pkCurrTechniqueSet->GetTechniqueArrayCount())
        {
            NiD3DXEffectTechnique* pkTech = 
                m_pkCurrTechniqueSet->GetTechnique(m_uiIndex++);
            if (pkTech)
                return pkTech;
        }
        m_pkCurrTechniqueSet = NULL;
        m_uiIndex = 0;
    }
    return NULL;
}
/*
//---------------------------------------------------------------------------
NiD3DXEffectTechnique* NiD3DXEffectFile::GetTechnique(const char* pcName, 
    unsigned int uiImplementation) const
{
    NiD3DXEffectTechniqueSetPtr spTechniqueSet;
    m_kTechniques.GetAt(pcName, spTechniqueSet);
    if (spTechniqueSet != NULL)
    {
        NiD3DXEffectTechnique* pkTechnique = spTechniqueSet->GetTechnique(
            uiImplementation);
        return pkTechnique;
    }
    return NULL;
}*/
//---------------------------------------------------------------------------
NiD3DXEffectTechniqueSet* NiD3DXEffectFile::GetTechniqueSet(const char* pcName)
    const
{
    NiD3DXEffectTechniqueSetPtr spTechniqueSet;
    m_kTechniques.GetAt(pcName, spTechniqueSet);
    return spTechniqueSet;
}
//---------------------------------------------------------------------------
NiShaderDesc* NiD3DXEffectFile::GetFirstShaderDesc()
{
    m_kTechniqueIter = m_kTechniques.GetFirstPos();
    return GetNextShaderDesc();
}
//---------------------------------------------------------------------------
NiShaderDesc* NiD3DXEffectFile::GetNextShaderDesc()
{
    if (m_kTechniqueIter)
    {
        const char* pcName = NULL;
        NiD3DXEffectTechniqueSetPtr spTechniqueSet;
        m_kTechniques.GetNext(m_kTechniqueIter, pcName, spTechniqueSet);
        if (spTechniqueSet != NULL)
            return spTechniqueSet->GetShaderDesc(&m_kAttribTable);
    }
    return NULL;
}
//---------------------------------------------------------------------------
void NiD3DXEffectFile::LoadEffect(NiD3DRenderer* pkRenderer)
{
    NiD3DXEffectFactory::GetInstance()->SetD3DRenderer(pkRenderer);
    NiD3DXEffectFactory::GetInstance()->CreateD3DXEffectFromFile(m_pcName);

    // Link up all techniques with valid handles
    LinkHandles();
}
//---------------------------------------------------------------------------
void NiD3DXEffectFile::HandleLostDevice()
{
    if (m_pkD3DXEffect != NULL && m_bRenderableEffect)
        ((LPD3DXEFFECT)m_pkD3DXEffect)->OnLostDevice();
}
//---------------------------------------------------------------------------
void NiD3DXEffectFile::HandleResetDevice()
{
    if (m_pkD3DXEffect != NULL && m_bRenderableEffect)
        ((LPD3DXEFFECT)m_pkD3DXEffect)->OnResetDevice();
}
//---------------------------------------------------------------------------
void NiD3DXEffectFile::DestroyRendererData()
{
    if (m_pkD3DXEffect)
    {

#if defined(WIN32) 
        if (m_bRenderableEffect)
            ((LPD3DXEFFECT)m_pkD3DXEffect)->SetStateManager(NULL);
#elif defined(XENON)
        if (m_bRenderableEffect)
            ((LPD3DXEFFECT)m_pkD3DXEffect)->SetStateManager(NULL);
#endif  // #if defined(WIN32)

        m_pkD3DXEffect->Release();
        m_pkD3DXEffect = NULL;
        m_bRenderableEffect = false;

        NiTMapIterator kTechIter = m_kTechniques.GetFirstPos();
        while (kTechIter)
        {
            const char* pcName = NULL;
            NiD3DXEffectTechniqueSetPtr spSet;
            m_kTechniques.GetNext(kTechIter, pcName, spSet);
            spSet->DestroyRendererData();
        }

        NiTListIterator kParamIter = m_kParameters.GetHeadPos();
        while (kParamIter)
        {
            NiD3DXEffectParameter* pkParam = 
                m_kParameters.GetNext(kParamIter);
            pkParam->DestroyRendererData();
        }
    }
}
//---------------------------------------------------------------------------
bool NiD3DXEffectFile::ParseEffect(D3DDevicePtr pkD3DDevice)
{
    if (m_pkD3DXEffect == NULL || m_bParsed)
        return true;

    unsigned int uiNumParameters = 0;
    unsigned int uiNumTechniques = 0;
    bool bSuccess = NiD3DXEffectParser::GetParametersAndTechniques(
        m_pkD3DXEffect, uiNumParameters, uiNumTechniques);

    if (!bSuccess)
        return false;

    NiD3DXEffectParser::ResetIndices();
    unsigned int i = 0;
    for (; i < uiNumParameters; i++)
    {
        NiD3DXEffectParameterDesc* pkParamDesc = 
            NiD3DXEffectParser::GetParameter(m_pkD3DXEffect, i);

        if (pkParamDesc)
        {
            NiD3DXEffectParameter* pkParam = NiNew NiD3DXEffectParameter;
            if (pkParam->Initialize(pkParamDesc, &m_kAttribTable))
                m_kParameters.AddTail(pkParam);
        }
    }
    
    for (i = 0; i < uiNumTechniques; i++)
    {
        NiD3DXEffectTechniqueDesc kTechDesc;
        NiD3DXEffectParser::GetTechnique(kTechDesc, m_pkD3DXEffect, i);

        NiD3DXEffectTechnique* pkTechnique = NiNew NiD3DXEffectTechnique;
        pkTechnique->Initialize(&kTechDesc);
        pkTechnique->SetFile(this);

        AddTechnique(pkTechnique, pkD3DDevice);
    }

    // Remove unused parameters
    NiTListIterator kParamIter = m_kParameters.GetHeadPos();
    while (kParamIter)
    {
        NiD3DXEffectParameter* pkParam = m_kParameters.GetNext(kParamIter);
        if (!pkParam->GetUsed())
        {
            NiDelete pkParam;
            NiTListIterator kPrev = m_kParameters.GetPrevPos(kParamIter);
            if (kPrev)
                m_kParameters.RemovePos(kPrev);
            else
                m_kParameters.RemoveTail();
        }
    }

    m_bParsed = true;
    return true;
}
//---------------------------------------------------------------------------
void NiD3DXEffectFile::AddTechnique(NiD3DXEffectTechnique* pkTechnique,
    D3DDevicePtr)
{
    // Add appropriate parameters
    NiTListIterator kParamIter = m_kParameters.GetHeadPos();
    while (kParamIter)
    {
        NiD3DXEffectParameter* pkParam = m_kParameters.GetNext(kParamIter);
        bool bUsed = NiD3DXEffectParser::IsParameterUsed(m_pkD3DXEffect, 
            pkParam->GetName(), pkTechnique->GetTechniqueName());

        if (bUsed)
        {
            pkParam->SetUsed(true);
            pkTechnique->AddParameter(pkParam);
        }
    }

    const char* pcName = pkTechnique->GetShaderName();
    NiD3DXEffectTechniqueSetPtr spTechniqueSet;
    if (!m_kTechniques.GetAt(pcName, spTechniqueSet) || 
        spTechniqueSet == NULL)
    {
        spTechniqueSet = NiNew NiD3DXEffectTechniqueSet;
        spTechniqueSet->SetName(pcName);
        m_kTechniques.SetAt(pcName, spTechniqueSet);
    }

    spTechniqueSet->AddImplementation(pkTechnique);
}
//---------------------------------------------------------------------------
bool NiD3DXEffectFile::LinkHandles()
{
    if (m_pkD3DXEffect == NULL || m_bRenderableEffect == false)
        return false;

    LPD3DXEFFECT pkEffect;
#ifdef _XENON
    pkEffect = (LPD3DXEFFECT)m_pkD3DXEffect;
    pkEffect->AddRef();
#else
#ifdef NIDEBUG
    HRESULT hr = 
#endif
        m_pkD3DXEffect->QueryInterface(IID_ID3DXEffect, (LPVOID*)&pkEffect);
    NIASSERT(SUCCEEDED(hr) && pkEffect);
#endif

    // Link parameters
    NiTListIterator kParamIter = m_kParameters.GetHeadPos();
    while (kParamIter)
    {
        NiD3DXEffectParameter* pkParam = m_kParameters.GetNext(kParamIter);
        pkParam->LinkHandle(pkEffect);
    }

    // Link each technique to a NiD3DXEffectTechnique
    NiD3DXEffectTechniqueSetPtr spSet;
    NiTMapIterator kTechIter = m_kTechniques.GetFirstPos();
    while (kTechIter)
    {
        const char* pcName = NULL;
        m_kTechniques.GetNext(kTechIter, pcName, spSet);
        if (spSet == NULL)
            continue;

        unsigned int uiNumTechniques = spSet->GetTechniqueArrayCount();
        for (unsigned int i = 0; i < uiNumTechniques; i++)
        {
            NiD3DXEffectTechnique* pkTech = spSet->GetTechnique(i);
            if (pkTech == NULL)
                continue;

            pkTech->LinkHandle(pkEffect);
        }
    }

    pkEffect->Release();

    return true;
}
//---------------------------------------------------------------------------


