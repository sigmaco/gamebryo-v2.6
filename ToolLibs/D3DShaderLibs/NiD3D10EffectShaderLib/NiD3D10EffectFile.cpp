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
#include "NiD3D10EffectShaderLibPCH.h"

#include "NiD3D10EffectFile.h"
#include "NiD3D10EffectFactory.h"
#include "NiD3D10EffectParameter.h"
#include "NiD3D10EffectParser.h"
#include "NiD3D10EffectShader.h"
#include "NiD3D10EffectTechniqueDesc.h"

#include <NiD3D10DataStream.h>
#include <NiD3D10ShaderConstantMap.h>
#include <NiD3D10Renderer.h>
#include <NiShaderDesc.h>
//---------------------------------------------------------------------------
NiD3D10EffectFile::NiD3D10EffectFile(ID3D10Blob* pkCompiledEffect) :
    m_kTechniqueIter(NULL),
    m_pkCurrTechniqueSet(NULL),
    m_uiTechniqueIndex(0),
    m_pkCompiledEffect(pkCompiledEffect),
    m_pkD3D10Effect(NULL),
    m_bConstantBuffersLoaded(false)
{
    if (m_pkCompiledEffect)
        m_pkCompiledEffect->AddRef();
}
//---------------------------------------------------------------------------
NiD3D10EffectFile::~NiD3D10EffectFile()
{
    NILOG(
        "%s> Killing NiD3D10EffectFile %s\n", 
        __FUNCTION__,
        m_kName);

    // NiD3D10EffectFactory won't exist if it has already been shut down.
    // In this case, there's no need to remove the effect from the map.
    NiD3D10EffectFactory* pkEffectFactory = 
        NiD3D10EffectFactory::GetInstance(false);
    if (pkEffectFactory)
        pkEffectFactory->RemoveD3D10EffectFileFromMap(m_kName);

    DestroyRendererData();

    m_kTechniqueIter = m_kTechniques.GetFirstPos();
    while (m_kTechniqueIter)
    {
        NiFixedString kName;
        NiD3D10EffectTechniqueSetPtr spTechniqueSet;
        m_kTechniques.GetNext(m_kTechniqueIter, kName, spTechniqueSet);
        m_kTechniques.SetAt(kName, NULL);
    }
    m_kTechniques.RemoveAll();

    NiTListIterator kParamIter = m_kParameters.GetHeadPos();
    while (kParamIter)
    {
        NiD3D10EffectParameter* pkParam = m_kParameters.GetNext(kParamIter);
        NiDelete pkParam;
    }
    m_kParameters.RemoveAll();

    m_kConstantMaps.RemoveAll();

    if (m_pkCompiledEffect)
        m_pkCompiledEffect->Release();
}
//---------------------------------------------------------------------------
void NiD3D10EffectFile::SetD3D10Effect(ID3D10Effect* pkEffect)
{
    if (pkEffect != m_pkD3D10Effect)
    {
        if (m_pkD3D10Effect)
            m_pkD3D10Effect->Release();
        m_pkD3D10Effect = pkEffect;
        if (m_pkD3D10Effect)
            m_pkD3D10Effect->AddRef();
    }

    // Pass effect to techniques
    NiTMapIterator kTechniqueIter = m_kTechniques.GetFirstPos();
    while (kTechniqueIter)
    {
        NiFixedString kName;
        NiD3D10EffectTechniqueSetPtr spSet;
        m_kTechniques.GetNext(kTechniqueIter, kName, spSet);
        if (spSet == NULL)
            continue;

        const NiUInt32 uiTechniques = spSet->GetImplementationCount();
        for (NiUInt32 i = 0; i < uiTechniques; i++)
        {
            NiD3D10EffectTechnique* pkTechnique = spSet->GetTechnique(i);
            NIASSERT(pkTechnique);
            NiFixedString kTechniqueName = pkTechnique->GetTechniqueName();
            ID3D10EffectTechnique* pkD3D10Technique = NULL;
            if (pkEffect)
            {
                pkD3D10Technique = 
                    pkEffect->GetTechniqueByName(kTechniqueName);
            }
            pkTechnique->SetD3D10Technique(pkD3D10Technique);
            pkTechnique->SetShaderD3D10Effect(pkEffect);
        }
    }

    // Pass effect to parameters
    NiTListIterator kParamIter = m_kParameters.GetHeadPos();
    while (kParamIter)
    {
        NiD3D10EffectParameter* pkParam = 
            m_kParameters.GetNext(kParamIter);
        NIASSERT(pkParam);
        NiFixedString kParameterName = pkParam->GetName();
        ID3D10EffectVariable* pkD3D10Variable = NULL;
        if (pkEffect)
        {
            pkD3D10Variable = 
                pkEffect->GetVariableByName(kParameterName);
        }
        pkParam->SetD3D10Parameter(pkD3D10Variable);
    }
}
//---------------------------------------------------------------------------
void NiD3D10EffectFile::ReleaseD3D10Effect()
{
    SetD3D10Effect(NULL);
}
//---------------------------------------------------------------------------
void NiD3D10EffectFile::LinkRendererData()
{
    NiD3D10Renderer* pkRenderer = NiD3D10Renderer::GetRenderer();
    NIASSERT(pkRenderer);

    // Ensure effect is loaded
    if (m_pkD3D10Effect == NULL)
    {
        NiD3D10EffectFactory* pkEffectFactory = 
            NiD3D10EffectFactory::GetInstance(false);
        NIASSERT(pkEffectFactory);

        pkEffectFactory->CreateD3D10Effect(
            pkRenderer->GetD3D10Device(), 
            this,
            false);
    }

    NIASSERT(m_pkD3D10Effect != NULL);

    // Find constant buffers for shader constant maps
    if (m_bConstantBuffersLoaded)
        return;

    NiTMapIterator kConstantMapIter = m_kConstantMaps.GetFirstPos();
    while (kConstantMapIter)
    {
        NiFixedString kConstantMapName;
        NiD3D10ShaderConstantMapPtr spConstantMap;
        m_kConstantMaps.GetNext(
            kConstantMapIter, 
            kConstantMapName, 
            spConstantMap);

        ID3D10EffectConstantBuffer* pkEffectBuffer = 
            m_pkD3D10Effect->GetConstantBufferByName(kConstantMapName);
        NIASSERT(pkEffectBuffer && pkEffectBuffer->IsValid());

        ID3D10Buffer* pkConstantBuffer = NULL;
        
#ifdef NIDEBUG
        HRESULT hr = 
#endif
            pkEffectBuffer->GetConstantBuffer(&pkConstantBuffer);
        NIASSERT(SUCCEEDED(hr) && pkConstantBuffer);

        NiD3D10DataStreamPtr spDataStream = 
            NiD3D10DataStream::Create(pkConstantBuffer);
        spConstantMap->SetShaderConstantDataStream(spDataStream);
    }


    // Optimize - all data for all shaders in the effect should have already 
    // been extracted
    NIASSERT(!m_pkD3D10Effect->IsOptimized());
    m_pkD3D10Effect->Optimize();

    m_bConstantBuffersLoaded = true;
}
//---------------------------------------------------------------------------
void NiD3D10EffectFile::DestroyRendererData()
{
    ReleaseD3D10Effect();

    if (m_bConstantBuffersLoaded)
    {
        NiTMapIterator kIter = m_kConstantMaps.GetFirstPos();
        while (kIter)
        {
            NiFixedString kConstantMapName;
            NiD3D10ShaderConstantMapPtr spConstantMap;
            m_kConstantMaps.GetNext(kIter, kConstantMapName, spConstantMap);


            spConstantMap->SetShaderConstantDataStream(NULL);
        }
        m_bConstantBuffersLoaded = false;
    }
}

//---------------------------------------------------------------------------
NiD3D10EffectTechnique* NiD3D10EffectFile::GetFirstTechnique()
{
    m_kTechniqueIter = m_kTechniques.GetFirstPos();
    if (m_kTechniqueIter == NULL)
        return NULL;
    m_pkCurrTechniqueSet = NULL;
    m_uiTechniqueIndex = 0;
    return GetNextTechnique();
}
//---------------------------------------------------------------------------
NiD3D10EffectTechnique* NiD3D10EffectFile::GetNextTechnique()
{
    while (m_kTechniqueIter)
    {
        if (m_pkCurrTechniqueSet == NULL)
        {
            NiFixedString kName;
            NiD3D10EffectTechniqueSetPtr spTechniqueSet;
            m_kTechniques.GetNext(m_kTechniqueIter, kName, spTechniqueSet);
            m_pkCurrTechniqueSet = spTechniqueSet;
        }
        NIASSERT(m_pkCurrTechniqueSet);
        while (m_uiTechniqueIndex < 
            m_pkCurrTechniqueSet->GetTechniqueArrayCount())
        {
            NiD3D10EffectTechnique* pkTech = 
                m_pkCurrTechniqueSet->GetTechnique(m_uiTechniqueIndex++);
            if (pkTech)
                return pkTech;
        }
        m_pkCurrTechniqueSet = NULL;
        m_uiTechniqueIndex = 0;
    }
    return NULL;
}
//---------------------------------------------------------------------------
NiD3D10EffectTechniqueSet* NiD3D10EffectFile::GetTechniqueSet(
    const NiFixedString& kName) const
{
    NiD3D10EffectTechniqueSetPtr spTechniqueSet;
    m_kTechniques.GetAt(kName, spTechniqueSet);
    return spTechniqueSet;
}
//---------------------------------------------------------------------------
NiShaderDesc* NiD3D10EffectFile::GetFirstShaderDesc() const
{
    m_kTechniqueIter = m_kTechniques.GetFirstPos();
    return GetNextShaderDesc();
}
//---------------------------------------------------------------------------
NiShaderDesc* NiD3D10EffectFile::GetNextShaderDesc() const
{
    if (m_kTechniqueIter)
    {
        NiFixedString kName;
        NiD3D10EffectTechniqueSetPtr spTechniqueSet;
        m_kTechniques.GetNext(m_kTechniqueIter, kName, spTechniqueSet);
        if (spTechniqueSet != NULL)
            return spTechniqueSet->GetShaderDesc(&m_kAttribTable);
    }
    return NULL;
}
//---------------------------------------------------------------------------
NiD3D10ShaderConstantMap* NiD3D10EffectFile::GetShaderConstantMap(
    const NiFixedString& kName) const
{
    NiD3D10ShaderConstantMapPtr spMap;
    m_kConstantMaps.GetAt(kName, spMap);
    return spMap;
}

//---------------------------------------------------------------------------
bool NiD3D10EffectFile::ParseEffect(ID3D10Effect* pkEffect)
{
    // This function should only ever be called once.
    NIASSERT(m_kParameters.GetSize() == 0 &&
        m_kTechniques.GetCount() == 0 &&
        m_kConstantMaps.GetCount() == 0);

    if (pkEffect == NULL)
    {
        return false;
    }

    D3D10_EFFECT_DESC kEffectDesc;
    HRESULT hr = pkEffect->GetDesc(&kEffectDesc);
    if (FAILED(hr))
        return false;

    const NiUInt32 uiNumParameters = kEffectDesc.GlobalVariables;
    const NiUInt32 uiNumTechniques = kEffectDesc.Techniques;

    NiD3D10EffectParser::ResetIndices();
    NiUInt32 i = 0;
    for (; i < uiNumParameters; i++)
    {
        NiD3D10EffectParameterDesc* pkParamDesc = 
            NiD3D10EffectParser::GetParameter(pkEffect, i);

        if (pkParamDesc)
        {
            NiD3D10EffectParameter* pkParam = NiNew NiD3D10EffectParameter;
            if (pkParam->Initialize(pkParamDesc, &m_kAttribTable))
                m_kParameters.AddTail(pkParam);
            else
                NiDelete pkParam;
        }
    }
    
    for (i = 0; i < uiNumTechniques; i++)
    {
        NiD3D10EffectTechniqueDesc kTechDesc;
        NiD3D10EffectParser::GetTechnique(kTechDesc, pkEffect, i);

        NiD3D10EffectTechnique* pkTechnique = NiNew NiD3D10EffectTechnique;
        pkTechnique->Initialize(&kTechDesc);
        pkTechnique->SetFile(this);

        AddTechnique(pkTechnique);
    }

    // Remove unused parameters
    NiTListIterator kParamIter = m_kParameters.GetHeadPos();
    while (kParamIter)
    {
        NiD3D10EffectParameter* pkParam = m_kParameters.GetNext(kParamIter);
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

    // Create the shader constant maps
    CreateShaderConstantMaps(pkEffect);

    return true;
}
//---------------------------------------------------------------------------
void NiD3D10EffectFile::CreateShaderConstantMaps(ID3D10Effect* pkEffect)
{
    NIASSERT(m_kConstantMaps.GetCount() == 0);
    
    D3D10_EFFECT_DESC kEffectDesc;
    HRESULT hr = pkEffect->GetDesc(&kEffectDesc);
    NIASSERT(SUCCEEDED(hr));

    const NiUInt32 uiConstantBuffers = kEffectDesc.ConstantBuffers;
    NiUInt32 i = 0;
    for (; i < uiConstantBuffers; i++)
    {
        ID3D10EffectConstantBuffer* pkEffectBuffer = 
            pkEffect->GetConstantBufferByIndex(i);
        D3D10_EFFECT_VARIABLE_DESC kVariableDesc;
        hr = pkEffectBuffer->GetDesc(&kVariableDesc);
        NIASSERT(SUCCEEDED(hr));

        ID3D10EffectType* pkType = pkEffectBuffer->GetType();
        NIASSERT(pkType->IsValid());
        D3D10_EFFECT_TYPE_DESC kTypeDesc;
        hr = pkType->GetDesc(&kTypeDesc);
        NIASSERT(SUCCEEDED(hr));

        if (kTypeDesc.Type == D3D10_SVT_CBUFFER)
        {
            ID3D10Buffer* pkConstantBuffer = NULL;
            hr = pkEffectBuffer->GetConstantBuffer(&pkConstantBuffer);
            // If the constant buffer is empty, then it won't be returned here
            // In that case, don't register it.
            if( SUCCEEDED(hr) && pkConstantBuffer)
            {
                NiFixedString kName = kVariableDesc.Name;
                NiD3D10ShaderConstantMapPtr spMap = 
                    NiNew NiD3D10ShaderConstantMap(
                    NiGPUProgram::PROGRAM_VERTEX);

                m_kConstantMaps.SetAt(kName, spMap);
            }
        }
    }

    // Remove unused parameters
    NiTListIterator kParamIter = m_kParameters.GetHeadPos();
    while (kParamIter)
    {
        NiD3D10EffectParameter* pkParam = m_kParameters.GetNext(kParamIter);

        const NiFixedString& kBufferName = pkParam->GetConstantBufferName();
        // Parameters like textures do not exist in constant buffers.
        if (!kBufferName.Exists())
            continue;

        NiD3D10ShaderConstantMapPtr spMap;
        bool bFound = m_kConstantMaps.GetAt(kBufferName, spMap);
        if (!bFound)
        {
            // This is likely becaue the buffer was a TBuffer, which is
            // not yet supported.
            continue;
        }
        NIASSERT(spMap);

        ID3D10EffectVariable* pkVariable = 
            pkEffect->GetVariableByName(pkParam->GetName());
        // Don't set the variable on the NiD3D10EffectParameter now - it will
        // be done during LinkHandles.

        // Translate to shader constant map entry
        D3D10_EFFECT_VARIABLE_DESC kVariableDesc;
        hr = pkVariable->GetDesc(&kVariableDesc);
        NIASSERT(SUCCEEDED(hr));

        ID3D10EffectType* pkType = pkVariable->GetType();
        NIASSERT(pkType->IsValid());
        D3D10_EFFECT_TYPE_DESC kTypeDesc;
        hr = pkType->GetDesc(&kTypeDesc);
        NIASSERT(SUCCEEDED(hr));

        // Encode register values
        const size_t uiElementSize = 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_COMPONENT_BIT_COUNT / 8;
   
        NIASSERT(kVariableDesc.BufferOffset % uiElementSize == 0);
        NiUInt32 uiOffsetElements = 
            kVariableDesc.BufferOffset / uiElementSize;

        NiUInt32 uiStartRegister = uiOffsetElements / 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COMPONENTS;
        NiUInt32 uiStartElement = uiOffsetElements % 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COMPONENTS;

        NiUInt32 uiElementCount = 
            kTypeDesc.UnpackedSize / uiElementSize;

        NiUInt32 uiRegisterCount = uiElementCount / 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COMPONENTS;
        NiUInt32 uiExtraElementCount = uiElementCount % 
            D3D10_COMMONSHADER_CONSTANT_BUFFER_REGISTER_COMPONENTS;

        NiUInt32 uiEncodedStartRegister = 0;
        NiUInt32 uiEncodedRegisterCount = 0;
        NiD3D10ShaderConstantMap::EncodePackedRegisterAndElement(
            uiEncodedStartRegister, 
            uiStartRegister, 
            uiStartElement, 
            false);

        NiD3D10ShaderConstantMap::EncodePackedRegisterAndElement(
            uiEncodedRegisterCount, 
            uiRegisterCount, 
            uiExtraElementCount, 
            kTypeDesc.UnpackedSize == kTypeDesc.PackedSize);

        // Get default value
        char* pcDefaultValue = NiStackAlloc(char, kTypeDesc.UnpackedSize);
        pkVariable->GetRawValue(
            pcDefaultValue, 
            0, 
            kTypeDesc.UnpackedSize);

#ifdef NIDEBUG
        NiShaderError eError = 
#endif
            spMap->AddEntry(
            pkParam->GetKey(), 
            pkParam->GetFlags(),
            pkParam->GetExtra(), 
            uiEncodedStartRegister, 
            uiEncodedRegisterCount, 
            pkParam->GetName(), 
            kTypeDesc.UnpackedSize, 
            kTypeDesc.UnpackedSize, 
            pcDefaultValue, 
            true);
        NiStackFree(pcDefaultValue);

        NIASSERT(eError == NISHADERERR_OK);
        // The row major/column major status is not set at this point - set it
        NIASSERT(spMap->GetEntry(pkParam->GetKey()));
        spMap->GetEntry(pkParam->GetKey())->SetColumnMajor(
            kTypeDesc.Class == D3D10_SVC_MATRIX_COLUMNS);
    }

    NIASSERT(m_bConstantBuffersLoaded == false);
}
//---------------------------------------------------------------------------
void NiD3D10EffectFile::AddTechnique(NiD3D10EffectTechnique* pkTechnique)
{
    // Add appropriate parameters
    NiTListIterator kParamIter = m_kParameters.GetHeadPos();
    while (kParamIter)
    {
        NiD3D10EffectParameter* pkParam = m_kParameters.GetNext(kParamIter);
        bool bUsed = pkTechnique->IsParameterUsed(pkParam);

        if (bUsed)
        {
            pkParam->SetUsed(true);
            pkTechnique->AddParameter(pkParam);
        }
    }

    NiFixedString kName = pkTechnique->GetShaderName();
    NiD3D10EffectTechniqueSetPtr spTechniqueSet;
    if (!m_kTechniques.GetAt(kName, spTechniqueSet) || 
        spTechniqueSet == NULL)
    {
        spTechniqueSet = NiNew NiD3D10EffectTechniqueSet;
        spTechniqueSet->SetName(kName);
        m_kTechniques.SetAt(kName, spTechniqueSet);
    }

    // might not last much longer.
    spTechniqueSet->AddImplementation(pkTechnique);
}
//---------------------------------------------------------------------------
