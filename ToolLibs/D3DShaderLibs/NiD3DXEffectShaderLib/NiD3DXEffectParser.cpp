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

#include "NiD3DXEffectShaderLibrary.h"
#include "NiD3DXEffectParser.h"
#include "NiD3DXEffectParameterDesc.h"
#include "NiD3DShaderFactory.h"
#include "NiD3DXEffectTechniqueDesc.h"

#include <NiShaderConstantMap.h>
#include <NiShaderDesc.h>
#include <NiSemanticAdapterTableParser.h>

unsigned int NiD3DXEffectParser::ms_uiShaderMapIndex = 0;
unsigned int NiD3DXEffectParser::ms_uiGeneralLightIndex = 0;
unsigned int NiD3DXEffectParser::ms_uiPointLightIndex = 0;
unsigned int NiD3DXEffectParser::ms_uiDirectionalLightIndex = 0;
unsigned int NiD3DXEffectParser::ms_uiSpotLightIndex = 0;
unsigned int NiD3DXEffectParser::ms_uiShadowPointLightIndex = 0;
unsigned int NiD3DXEffectParser::ms_uiShadowDirectionalLightIndex = 0;
unsigned int NiD3DXEffectParser::ms_uiShadowSpotLightIndex = 0;
unsigned int NiD3DXEffectParser::ms_uiProjShadowIndex = 0;
unsigned int NiD3DXEffectParser::ms_uiProjLightIndex = 0;

//---------------------------------------------------------------------------
bool NiD3DXEffectParser::GetParametersAndTechniques(void* pkEffect, 
    unsigned int& uiNumParameters, unsigned int& uiNumTechniques)
{
    LPD3DXBASEEFFECT pkD3DXEffect = (LPD3DXBASEEFFECT)pkEffect;

    D3DXEFFECT_DESC kEffectDesc;
    HRESULT hr = pkD3DXEffect->GetDesc(&kEffectDesc);
    if (FAILED(hr))
        return false;

    uiNumParameters = kEffectDesc.Parameters;
    uiNumTechniques = kEffectDesc.Techniques;
    return true;
}
//---------------------------------------------------------------------------
NiD3DXEffectParameterDesc* NiD3DXEffectParser::GetParameter(void* pkEffect, 
    unsigned int uiIndex)
{
    LPD3DXBASEEFFECT pkD3DXEffect = (LPD3DXBASEEFFECT)pkEffect;

    D3DXHANDLE hParam = pkD3DXEffect->GetParameter(NULL, uiIndex);
    
    NiD3DXEffectParameterDesc* pkParamDesc = 
        NiD3DXEffectParser::GetParameterDesc(pkD3DXEffect, 
        (void*)hParam);

    return pkParamDesc;
}
//---------------------------------------------------------------------------
void NiD3DXEffectParser::GetTechnique(
    NiD3DXEffectTechniqueDesc& kTechniqueOut,
    void* pkEffect, 
    NiUInt32 uiIndex)
{
    LPD3DXBASEEFFECT pkD3DXEffect = (LPD3DXBASEEFFECT)pkEffect;

    D3DXHANDLE hTech = pkD3DXEffect->GetTechnique(uiIndex);

    NiD3DXEffectParser::GetTechniqueDesc(kTechniqueOut, pkD3DXEffect, 
        (void*)hTech);
}
//---------------------------------------------------------------------------
NiD3DXEffectParameterDesc* NiD3DXEffectParser::GetParameterDesc(
    void* pvD3DXEffect, void* pvParamHandle)
{
    LPD3DXBASEEFFECT pkEffect = (LPD3DXBASEEFFECT)pvD3DXEffect;
    D3DXHANDLE hParam = (D3DXHANDLE)pvParamHandle;

    D3DXPARAMETER_DESC kParamDesc;
    pkEffect->GetParameterDesc(hParam, &kParamDesc);

    const char* pcSemantic = kParamDesc.Semantic;
    D3DXPARAMETER_CLASS eClass = kParamDesc.Class;
    D3DXPARAMETER_TYPE eType = kParamDesc.Type;

    // First check for numerical parameters
    if (eType == D3DXPT_BOOL ||
        eType == D3DXPT_INT ||
        eType == D3DXPT_FLOAT)
    {
        // Check for "StandardsGlobal" parameter
        if (pcSemantic && NiStricmp(pcSemantic, "StandardsGlobal") == 0)
        {
            // Currently unable to do anything about it, really
            return NULL;
        }

        // Map semantic to either a predefined constant map value
        // or to an NiShader attribute

        NiD3DXEffectParameterDesc* pkDesc = NULL;

        // Default all of these to false. If we fall through the code below,
        // we will default to an attribute.
        bool bAttribute = false;
        bool bGlobal = false;
        bool bObject = false;
        bool bPredefined = false;

        // Get VarType from annotations as was added with FXL support.
        bool bFoundAnnotation = false;
        const char* pcVarType = NULL;
        D3DXHANDLE hVarType = pkEffect->GetAnnotationByName(hParam, "VarType");
        if (hVarType)
        {
            HRESULT eRes = pkEffect->GetString(hVarType, &pcVarType);
            if (SUCCEEDED(eRes))
            {
                bAttribute = (NiStricmp(pcVarType, "ATTRIBUTE") == 0);
                bGlobal = (NiStricmp(pcVarType, "GLOBAL") == 0);
                bObject = (NiStricmp(pcVarType, "OBJECT") == 0);
                bPredefined = (NiStricmp(pcVarType, "PREDEFINED") == 0);
                bFoundAnnotation = true;
            }

            // If VarType is "Predefined" let's try to find the
            // DefinedMapping annotation and replace pcSemantic with that
            // value.
            if (NiStricmp(pcVarType, "PREDEFINED") == 0)
            {
                D3DXHANDLE hDefMap = pkEffect->GetAnnotationByName(
                    hParam, "DefinedMapping");
                if (hDefMap)
                {
                    const char* pcDefMap = NULL;
                    eRes = pkEffect->GetString(hDefMap, &pcDefMap);
                    if (SUCCEEDED(eRes) && pcDefMap)
                    {
                        pcSemantic = pcDefMap;
                    }
                }
            }
        }

        // We didn't find the annotation and we have a semantic. We could flag
        // as ATTRIBUTE or GLOBAL here.
        if (pcSemantic != NULL && !bFoundAnnotation)
        {
            bAttribute = (strcmp(pcSemantic, "ATTRIBUTE") == 0);
            bGlobal = (strcmp(pcSemantic, "GLOBAL") == 0);
        }

        // At this point, the parameter is object, predefined, or unknown.
        if (!bAttribute && !bGlobal)
        {
            pkDesc = QueryObject(pkEffect, (void*) hParam, &kParamDesc,
                pcSemantic);
            if (pkDesc)
            {
                bObject = true;
            }
            else if (pcSemantic)
            {
                pkDesc = QueryPredefined(pcSemantic, 
                    (eClass == D3DXPC_MATRIX_COLUMNS), kParamDesc.Elements);
                if (pkDesc)
                {
                    bPredefined = true;
                    ParsePredefinedAnnotations(pkEffect, &kParamDesc,
                        (void*)hParam,
                        (NiD3DXEffectParameterPredefinedDesc*)pkDesc);
                }
            }
        }

        // Unknown semantic -> attribute
        if (!bGlobal && !bObject && !bPredefined)
            bAttribute = true;

        if (!pkDesc)
        {
            // Semantic must map to an attribute
            if ((eClass == D3DXPC_VECTOR && kParamDesc.Columns == 1) ||
                ((eClass == D3DXPC_MATRIX_ROWS || 
                eClass == D3DXPC_MATRIX_COLUMNS) && kParamDesc.Elements == 1))
            {
                eClass = D3DXPC_SCALAR;
            }

            if (eType  == D3DXPT_FLOAT && kParamDesc.Elements > 1 && 
               (eClass == D3DXPC_VECTOR || eClass == D3DXPC_SCALAR))
            {
                NiD3DXEffectParameterArrayDesc* pkArrayDesc = 
                    NiD3DXEffectParameterDesc::GetArrayDesc();

                pkArrayDesc->m_uiNumRows = kParamDesc.Rows;
                pkArrayDesc->m_uiNumCols = kParamDesc.Columns;
                pkArrayDesc->m_uiNumElements = kParamDesc.Elements;
                
                if (pkArrayDesc->m_uiNumElements >
                    NiD3DXEffectParameterArrayDesc::MAX_ROWS)
                {
                    NiOutputDebugString("Array has more than "
                        "NiD3DXEffectParameterArrayDesc::MAX_ROWS worth"
                        " of elements. Please use fewer elements.");
                    
                    return false;
                }
                
                // Search for annotations
                ParseArrayAnnotations(pkEffect, &kParamDesc, (void*)hParam, 
                    pkArrayDesc);
                
                HRESULT hr = 0;
                bool bCompactTempArray = false;
                if (kParamDesc.Class == D3DXPC_SCALAR)
                {
                    hr = pkEffect->GetFloatArray(hParam, 
                        pkArrayDesc->m_afInitValue, 
                        pkArrayDesc->m_uiNumElements);
                }
                else if (kParamDesc.Class == D3DXPC_VECTOR)
                {
                    if (kParamDesc.Rows == 1 && kParamDesc.Columns == 4)
                    {
                        // Vector 4's are the same size as the 
                        // array, so no need to waste time on a temp 
                        // array.
                        hr = pkEffect->GetVectorArray(hParam, 
                            (D3DXVECTOR4*)(pkArrayDesc->m_afInitValue), 
                            pkArrayDesc->m_uiNumElements);
                    }
                    else
                    {
                        // Read into the temporary buffer
                        // because D3DX forces the initial values to be
                        // D3DXVECTOR4's when they will often be smaller.
                        // The temp array will hold the values as 
                        // D3DXVECTOR4's and we will compact the values
                        // in a later method.
                        hr = pkEffect->GetVectorArray(hParam, 
                            (D3DXVECTOR4*)(pkArrayDesc->m_afTempValues), 
                            pkArrayDesc->m_uiNumElements);
                        bCompactTempArray = true;
                    }
                }
                        
                if (FAILED(hr))
                {
                    for (unsigned int ui = 0; ui < 
                        NiD3DXEffectParameterArrayDesc::MAX_ENTRIES; ui++)
                    {
                        pkArrayDesc->m_afInitValue[ui] = 0.0f;
                    }
                }
                else if (bCompactTempArray)
                {
#ifdef NIDEBUG
                    bool bCompacted = 
#endif
                        pkArrayDesc->CompactFromVector4Array(
                        (const D3DXVECTOR4*) pkArrayDesc->m_afTempValues, 
                        pkArrayDesc->m_afInitValue,
                        pkArrayDesc->m_uiNumRows, pkArrayDesc->m_uiNumCols,
                        pkArrayDesc->m_uiNumElements);
                    NIASSERT(bCompacted);
                }
                
                pkDesc = pkArrayDesc;
            }
            else if (eClass == D3DXPC_MATRIX_ROWS || 
                eClass == D3DXPC_MATRIX_COLUMNS)
            {
                // Can currently only handle float matrices
                if (eType != D3DXPT_FLOAT)
                    return false;

                NiD3DXEffectParameterMatrixDesc* pkMatrixDesc = 
                    NiD3DXEffectParameterDesc::GetMatrixDesc();

                pkMatrixDesc->m_uiNumRows = kParamDesc.Rows;
                pkMatrixDesc->m_uiNumCols = kParamDesc.Columns;

                // Search for annotations
                ParseMatrixAnnotations(pkEffect, &kParamDesc, (void*)hParam, 
                    pkMatrixDesc);

                // Check for initial value
                D3DXMATRIX* pkMatrix = 
                    (D3DXMATRIX*)(pkMatrixDesc->m_afInitValue);
                HRESULT hr;
                if (eClass == D3DXPC_MATRIX_COLUMNS)
                    hr = pkEffect->GetMatrix(hParam, pkMatrix);
                else
                    hr = pkEffect->GetMatrixTranspose(hParam, pkMatrix);
                if (FAILED(hr))
                {
                    D3DXMatrixIdentity(pkMatrix);
                }

                pkDesc = pkMatrixDesc;
            }
            else if (eClass == D3DXPC_VECTOR)
            {
                // Can currently only handle float vectors/colors
                if (eType != D3DXPT_FLOAT)
                    return false;

                NiD3DXEffectParameterVectorDesc* pkVectorDesc = 
                    NiD3DXEffectParameterDesc::GetVectorDesc();

                pkVectorDesc->m_uiNumRows = kParamDesc.Rows;
                pkVectorDesc->m_uiNumCols = kParamDesc.Columns;
                pkVectorDesc->m_uiNumAtomicElements = kParamDesc.Columns;
                
                if (pkVectorDesc->m_uiNumAtomicElements >
                    NiD3DXEffectParameterVectorDesc::MAX_ENTRIES)
                {
                    return false;
                }

                // Search for annotations
                ParseVectorAnnotations(pkEffect, &kParamDesc, (void*)hParam, 
                    pkVectorDesc);
                
                HRESULT hr = pkEffect->GetVector(hParam, 
                    (LPD3DXVECTOR4)pkVectorDesc->m_afInitValue);
                    
                if (FAILED(hr))
                {
                    for (unsigned int ui = 0; ui < 
                        NiD3DXEffectParameterVectorDesc::MAX_ENTRIES; ui++)
                    {
                        pkVectorDesc->m_afInitValue[ui] = 0.0f;
                    }
                }
                
                pkDesc = pkVectorDesc;
            }
            else if (eClass == D3DXPC_SCALAR)
            {
                // Can currently only handle float, bool, or int scalars
                if (eType == D3DXPT_FLOAT)
                {
                    NiD3DXEffectParameterFloatDesc* pkFloatDesc = 
                        NiD3DXEffectParameterDesc::GetFloatDesc();

                    // Search for annotations
                    ParseFloatAnnotations(pkEffect, &kParamDesc, 
                        (void*)hParam, pkFloatDesc);

                    // Check for initial value
                    float fFloat;
                    HRESULT hr = pkEffect->GetFloat(hParam, &fFloat);
                    if (SUCCEEDED(hr))
                        pkFloatDesc->m_fInitValue = fFloat;
                    else
                        pkFloatDesc->m_fInitValue = 0.0f;

                    pkDesc = pkFloatDesc;
                }
                else if (eType == D3DXPT_BOOL)
                {
                    NiD3DXEffectParameterBooleanDesc* pkBooleanDesc = 
                        NiD3DXEffectParameterDesc::GetBooleanDesc();

                    // Search for annotations
                    ParseBooleanAnnotations(pkEffect, &kParamDesc, 
                        (void*)hParam, pkBooleanDesc);

                    // Check for initial value
                    BOOL bBool;
                    HRESULT hr = pkEffect->GetBool(hParam, &bBool);
                    if (SUCCEEDED(hr))
                        pkBooleanDesc->m_bInitValue = (bBool != FALSE);
                    else
                        pkBooleanDesc->m_bInitValue = false;

                    pkDesc = pkBooleanDesc;
                }
                else if (eType == D3DXPT_INT)
                {
                    NiD3DXEffectParameterIntegerDesc* pkIntegerDesc = 
                        NiD3DXEffectParameterDesc::GetIntegerDesc();

                    // Search for annotations
                    ParseIntegerAnnotations(pkEffect, &kParamDesc, 
                        (void*)hParam, pkIntegerDesc);

                    // Check for initial value
                    int iInt;
                    HRESULT hr = pkEffect->GetInt(hParam, &iInt);
                    if (SUCCEEDED(hr))
                        pkIntegerDesc->m_uiInitValue = (unsigned int)iInt;
                    else
                        pkIntegerDesc->m_uiInitValue = 0;

                    pkDesc = pkIntegerDesc;
                }
            }
            if (pkDesc)
                pkDesc->m_bGlobal = bGlobal;
        }

        if (pkDesc)
        {
            if (pkDesc->m_pcName == NULL ||
                pkDesc->m_pcName[0] == '\0')
            {
                pkDesc->m_pcName = kParamDesc.Name;
            }
            return pkDesc;
        }
        return NULL;
    }

    // Not a numeric parameter - continue on to check for texture
    if (eType == D3DXPT_TEXTURE ||
        eType == D3DXPT_TEXTURE2D ||
        eType == D3DXPT_TEXTURE3D ||
        eType == D3DXPT_TEXTURECUBE)
    {
        NiD3DXEffectParameterTextureDesc* pkDesc = 
            NiD3DXEffectParameterDesc::GetTextureDesc();

        pkDesc->m_pcName = kParamDesc.Name;

        // Search for annotations
        ParseTextureAnnotations(pkEffect, &kParamDesc, (void*)hParam, 
            pkDesc);

        // Check for initial value
        D3DBaseTexturePtr pkTexture;
        HRESULT hr = pkEffect->GetTexture(hParam, &pkTexture);
        if (FAILED(hr))
            pkTexture = NULL;
        pkDesc->m_pvInitValue = pkTexture;
        return pkDesc;
    }

    // No luck
    return NULL;
}
//---------------------------------------------------------------------------
void NiD3DXEffectParser::GetTechniqueDesc(
    NiD3DXEffectTechniqueDesc& kDesc,
    void* pvD3DXEffect, void* pvTechHandle)
{
    LPD3DXBASEEFFECT pkEffect = (LPD3DXBASEEFFECT)pvD3DXEffect;
    D3DXHANDLE hTech = (D3DXHANDLE)pvTechHandle;

    D3DXTECHNIQUE_DESC kTechniqueDesc;
    pkEffect->GetTechniqueDesc(hTech, &kTechniqueDesc);

    kDesc.m_pcTechniqueName = kTechniqueDesc.Name;
    kDesc.m_uiNBTSourceUV = NiShaderDesc::BINORMALTANGENTUVSOURCEDEFAULT;

    if (kTechniqueDesc.Annotations)
    {
        ParseTechniqueAnnotations(pkEffect, &kTechniqueDesc, (void*)hTech, 
            &kDesc);
    }

    if (kDesc.m_pcShaderName == NULL)
        kDesc.m_pcShaderName = kDesc.m_pcTechniqueName;

    // Get pixel/vertex shader annotations
    for (unsigned int uiPass = 0; uiPass < kTechniqueDesc.Passes; uiPass++)
    {
        D3DXHANDLE hPass = pkEffect->GetPass(hTech, uiPass);
        D3DXPASS_DESC kPassDesc;
        pkEffect->GetPassDesc(hPass, &kPassDesc);

        unsigned int uiVSVersion = 0;
        unsigned int uiPSVersion = 0;

        if (kPassDesc.pVertexShaderFunction != NULL)
        {
            uiVSVersion = D3DXGetShaderVersion(
                kPassDesc.pVertexShaderFunction);
        }

        if (kPassDesc.pPixelShaderFunction != NULL)
        {
            uiPSVersion = D3DXGetShaderVersion(
                kPassDesc.pPixelShaderFunction);
        }

        if (kDesc.m_uiVSVersion < uiVSVersion)
            kDesc.m_uiVSVersion = uiVSVersion;
        if (kDesc.m_uiPSVersion < uiPSVersion)
            kDesc.m_uiPSVersion = uiPSVersion;
    }
}
//---------------------------------------------------------------------------
bool NiD3DXEffectParser::IsParameterUsed(void* pvD3DXEffect, 
    const char* pcParamName, const char* pcTechName)
{
    LPD3DXBASEEFFECT pkBaseEffect = (LPD3DXBASEEFFECT)pvD3DXEffect;
    LPD3DXEFFECT pkEffect;

#ifdef _XENON
    // Xenon does not support the QueryInterface interface so we manually
    // perform a static cast and call AddRef()
    pkEffect = (LPD3DXEFFECT)pkBaseEffect;
    pkEffect->AddRef();
    //if (1)
    {
#else

    if (SUCCEEDED(pkBaseEffect->QueryInterface(IID_ID3DXEffect, 
        (LPVOID*)&pkEffect)))
    {
#endif
        NIASSERT(pkEffect);
        D3DXHANDLE hParam = pkEffect->GetParameterByName(NULL, pcParamName);
        D3DXHANDLE hTech = pkEffect->GetTechniqueByName(pcTechName);

        bool bReturn = (pkEffect->IsParameterUsed(hParam, hTech) != FALSE);
        pkEffect->Release();
        return bReturn;
    }

    // Assume parameter always used if no device exists
#ifndef _XENON
    return true;
#endif
}
//---------------------------------------------------------------------------
void NiD3DXEffectParser::ResetIndices()
{
    ms_uiShaderMapIndex = 0;
    ms_uiGeneralLightIndex = 0;
    ms_uiPointLightIndex = 0;
    ms_uiDirectionalLightIndex = 0;
    ms_uiSpotLightIndex = 0;
    ms_uiShadowPointLightIndex = 0;
    ms_uiShadowDirectionalLightIndex = 0;
    ms_uiShadowSpotLightIndex = 0;
    ms_uiProjShadowIndex = 0;
    ms_uiProjLightIndex = 0;
}
//---------------------------------------------------------------------------
void NiD3DXEffectParser::ParseMatrixAnnotations(void* pvD3DXEffect, 
    void* pvParamDesc, void* pvParamHandle, 
    NiD3DXEffectParameterMatrixDesc* pkDesc)
{
    LPD3DXEFFECT pkEffect = (LPD3DXEFFECT)pvD3DXEffect;
    D3DXPARAMETER_DESC* pkParamDesc = (D3DXPARAMETER_DESC*)pvParamDesc;
    D3DXHANDLE hParam = (D3DXHANDLE)pvParamHandle;

    // Query annotations
    for (unsigned int i = 0; i < pkParamDesc->Annotations; i++)
    {
        D3DXHANDLE hAnnot = pkEffect->GetAnnotation(hParam, i);
        D3DXPARAMETER_DESC kAnnotDesc;
        pkEffect->GetParameterDesc(hAnnot, &kAnnotDesc);
        BOOL bBool;
        /*if (NiStricmp(kAnnotDesc.Name, "uiname") == 0)
        {
            pkEffect->GetString(hAnnot, &(pkDesc->m_pcName));
        }
        else */if (NiStricmp(kAnnotDesc.Name, "desc") == 0 ||
            NiStricmp(kAnnotDesc.Name, "description") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uihelp") == 0)
        {
            pkEffect->GetString(hAnnot, &(pkDesc->m_pcDesc));
        }
        else if (NiStricmp(kAnnotDesc.Name, "hidden") == 0)
        {
            pkEffect->GetBool(hAnnot, &bBool);
            pkDesc->m_bHidden = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.Name, "uiwidget") == 0)
        {
            const char* pcValue = NULL;
            pkEffect->GetString(hAnnot, &pcValue);
            if (pcValue && NiStricmp(pcValue, "none") == 0)
            {
                pkDesc->m_bHidden = true;
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiD3DXEffectParser::ParseVectorAnnotations(void* pvD3DXEffect, 
    void* pvParamDesc, void* pvParamHandle, 
    NiD3DXEffectParameterVectorDesc* pkDesc)
{
    LPD3DXEFFECT pkEffect = (LPD3DXEFFECT)pvD3DXEffect;
    D3DXPARAMETER_DESC* pkParamDesc = (D3DXPARAMETER_DESC*)pvParamDesc;
    D3DXHANDLE hParam = (D3DXHANDLE)pvParamHandle;

    // Query annotations
    for (unsigned int i = 0; i < pkParamDesc->Annotations; i++)
    {
        D3DXHANDLE hAnnot = pkEffect->GetAnnotation(hParam, i);
        D3DXPARAMETER_DESC kAnnotDesc;
        pkEffect->GetParameterDesc(hAnnot, &kAnnotDesc);
        BOOL bBool;
        /*if (NiStricmp(kAnnotDesc.Name, "uiname") == 0)
        {
            pkEffect->GetString(hAnnot, &(pkDesc->m_pcName));
        }
        else */if (NiStricmp(kAnnotDesc.Name, "desc") == 0 ||
            NiStricmp(kAnnotDesc.Name, "description") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uihelp") == 0)
        {
            pkEffect->GetString(hAnnot, &(pkDesc->m_pcDesc));
        }
        else if (NiStricmp(kAnnotDesc.Name, "hidden") == 0)
        {
            pkEffect->GetBool(hAnnot, &bBool);
            pkDesc->m_bHidden = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.Name, "uiwidget") == 0)
        {
            const char* pcValue = NULL;
            pkEffect->GetString(hAnnot, &pcValue);
            if (pcValue && 
                (NiStricmp(pcValue, "color") == 0 ||
                NiStricmp(pcValue, "colorswatch") == 0))
            {
                pkDesc->m_bColor = true;
            }
            else if (pcValue && NiStricmp(pcValue, "none") == 0)
            {
                pkDesc->m_bHidden = true;
            }
        }
        else if (NiStricmp(kAnnotDesc.Name, "color") == 0)
        {
            pkEffect->GetBool(hAnnot, &bBool);
            pkDesc->m_bColor = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.Name, "uitype") == 0)
        {
            const char* pcValue = NULL;
            pkEffect->GetString(hAnnot, &pcValue);
            if (pcValue && 
                (NiStricmp(pcValue, "color") == 0 ||
                NiStricmp(pcValue, "colorswatch") == 0))
            {
                pkDesc->m_bColor = true;
            }

        }
#if defined(WIN32) || defined(_XENON)
#pragma warning(disable: 4238)
#endif
        else if (NiStricmp(kAnnotDesc.Name, "min") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uimin") == 0)
        {
            pkEffect->GetVector(hAnnot, &((D3DXVECTOR4)pkDesc->m_afMinValue));
        }
        else if (NiStricmp(kAnnotDesc.Name, "max") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uimax") == 0)
        {
            pkEffect->GetVector(hAnnot, &((D3DXVECTOR4)pkDesc->m_afMaxValue));
        }
#if defined(WIN32) || defined(_XENON)
#pragma warning(default: 4238)
#endif
    }
}
//---------------------------------------------------------------------------
void NiD3DXEffectParser::ParseArrayAnnotations(void* pvD3DXEffect, 
    void* pvParamDesc, void* pvParamHandle, 
    NiD3DXEffectParameterArrayDesc* pkDesc)
{
    LPD3DXEFFECT pkEffect = (LPD3DXEFFECT)pvD3DXEffect;
    D3DXPARAMETER_DESC* pkParamDesc = (D3DXPARAMETER_DESC*)pvParamDesc;
    D3DXHANDLE hParam = (D3DXHANDLE)pvParamHandle;

    // Query annotations
    for (unsigned int i = 0; i < pkParamDesc->Annotations; i++)
    {
        D3DXHANDLE hAnnot = pkEffect->GetAnnotation(hParam, i);
        D3DXPARAMETER_DESC kAnnotDesc;
        pkEffect->GetParameterDesc(hAnnot, &kAnnotDesc);
        BOOL bBool;
        
        if (NiStricmp(kAnnotDesc.Name, "desc") == 0 ||
            NiStricmp(kAnnotDesc.Name, "description") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uihelp") == 0)
        {
            pkEffect->GetString(hAnnot, &(pkDesc->m_pcDesc));
        }
        else if (NiStricmp(kAnnotDesc.Name, "hidden") == 0)
        {
            pkEffect->GetBool(hAnnot, &bBool);
            pkDesc->m_bHidden = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.Name, "uiwidget") == 0)
        {
            const char* pcValue = NULL;
            pkEffect->GetString(hAnnot, &pcValue);
            if (pcValue && 
                (NiStricmp(pcValue, "color") == 0 ||
                NiStricmp(pcValue, "colorswatch") == 0))
            {
                if (pkDesc->m_uiNumCols != 4 && 
                    pkDesc->m_uiNumRows != 1)
                {
                    NiOutputDebugString("Annotation indicates \"color\", "
                        "but the variable is not a float4. UI will not be"
                        " a color.");
                    continue;
                }
                pkDesc->m_bColor = true;
            }
            else if (pcValue && NiStricmp(pcValue, "none") == 0)
            {
                pkDesc->m_bHidden = true;
            }
        }
        else if (NiStricmp(kAnnotDesc.Name, "color") == 0)
        {
            if (pkDesc->m_uiNumCols != 4 && 
                pkDesc->m_uiNumRows != 1)
            {
                NiOutputDebugString("Annotation indicates \"color\", but the"
                    " variable is not a float4. UI will not be a color.");
                continue;
            }
            pkEffect->GetBool(hAnnot, &bBool);
            pkDesc->m_bColor = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.Name, "uitype") == 0)
        {
            const char* pcValue = NULL;
            pkEffect->GetString(hAnnot, &pcValue);
            if (pcValue && 
                (NiStricmp(pcValue, "color") == 0 ||
                NiStricmp(pcValue, "colorswatch") == 0))
            {
                if (pkDesc->m_uiNumCols != 4 && 
                    pkDesc->m_uiNumRows != 1)
                {
                    NiOutputDebugString("Annotation indicates \"color\", "
                        "but the variable is not a float4. UI will not be"
                        " a color.");
                    continue;
                }
                pkDesc->m_bColor = true;
            }

        }
        else if (NiStricmp(kAnnotDesc.Name, "min") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uimin") == 0)
        {
            HRESULT hr = 0;
            if (kAnnotDesc.Columns != pkDesc->m_uiNumCols ||
                kAnnotDesc.Rows != pkDesc->m_uiNumRows ||
                kAnnotDesc.Elements != pkDesc->m_uiNumElements)
            {
                NiOutputDebugString("Array annotation \"min\" does not match"
                    " parent in either number of columns, rows, or elements");
                continue;
            }

            bool bCompactTempArray = false;
            if (kAnnotDesc.Class == D3DXPC_SCALAR)
            {
                hr = pkEffect->GetFloatArray(hAnnot, 
                    pkDesc->m_afMinValue, 
                    pkDesc->m_uiNumElements);
            }
            else if (kAnnotDesc.Class == D3DXPC_VECTOR)
            {
                if (kAnnotDesc.Rows == 1 && kAnnotDesc.Columns == 4)
                {
                    // Vector 4's are the same size as the 
                    // array, so no need to waste time on a temp 
                    // array.
                    hr = pkEffect->GetVectorArray(hAnnot, 
                        (D3DXVECTOR4*)(pkDesc->m_afMinValue), 
                        pkDesc->m_uiNumElements);
                }
                else
                {
                    // Read into the temporary buffer
                    // because D3DX forces the initial values to be
                    // D3DXVECTOR4's when they will often be smaller.
                    // The temp array will hold the values as D3DXVECTOR4's
                    // and we will compact the values in a later method.
                    
                    hr = pkEffect->GetVectorArray(hAnnot, 
                        (D3DXVECTOR4*)(pkDesc->m_afTempValues), 
                        pkDesc->m_uiNumElements);
                    bCompactTempArray = true;
                }
            }
                    
            if (FAILED(hr))
            {
                for (unsigned int ui = 0; ui < 
                    NiD3DXEffectParameterArrayDesc::MAX_ENTRIES; ui++)
                {
                    pkDesc->m_afMinValue[ui] = 0.0f;
                }
            }
            else if (bCompactTempArray)
            {
#ifdef NIDEBUG
                bool bCompacted = 
#endif
                    pkDesc->CompactFromVector4Array(
                    (const D3DXVECTOR4*) pkDesc->m_afTempValues, 
                    pkDesc->m_afMinValue,
                    pkDesc->m_uiNumRows, pkDesc->m_uiNumCols,
                    pkDesc->m_uiNumElements);
                NIASSERT(bCompacted);
            }
                
        }
        else if (NiStricmp(kAnnotDesc.Name, "max") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uimax") == 0)
        {
            HRESULT hr = 0;
            if (kAnnotDesc.Columns != pkDesc->m_uiNumCols ||
                kAnnotDesc.Rows != pkDesc->m_uiNumRows ||
                kAnnotDesc.Elements != pkDesc->m_uiNumElements)
            {
                NiOutputDebugString("Array annotation \"max\" does not match"
                    " parent in either number of columns, rows, or elements");
                continue;
            }

            bool bCompactTempArray = false;
            if (kAnnotDesc.Class == D3DXPC_SCALAR)
            {
                hr = pkEffect->GetFloatArray(hAnnot, 
                    pkDesc->m_afMaxValue, 
                    pkDesc->m_uiNumElements);
            }
            else if (kAnnotDesc.Class == D3DXPC_VECTOR)
            {
                if (kAnnotDesc.Rows == 1 && kAnnotDesc.Columns == 4)
                {
                    // Vector 4's are the same size as the 
                    // array, so no need to waste time on a temp 
                    // array.
                    hr = pkEffect->GetVectorArray(hAnnot, 
                        (D3DXVECTOR4*)(pkDesc->m_afMaxValue), 
                        pkDesc->m_uiNumElements);
                }
                else
                {
                    // Read into the temporary buffer
                    // because D3DX forces the initial values to be
                    // D3DXVECTOR4's when they will often be smaller.
                    // The temp array will hold the values as D3DXVECTOR4's
                    // and we will compact the values in a later method.
                    
                    hr = pkEffect->GetVectorArray(hAnnot, 
                        (D3DXVECTOR4*)(pkDesc->m_afTempValues), 
                        pkDesc->m_uiNumElements);
                    bCompactTempArray = true;
                }
            }
                    
            if (FAILED(hr))
            {
                for (unsigned int ui = 0; ui < 
                    NiD3DXEffectParameterArrayDesc::MAX_ENTRIES; ui++)
                {
                    pkDesc->m_afMaxValue[ui] = 0.0f;
                }
            }
            else if (bCompactTempArray)
            {
#ifdef NIDEBUG
                bool bCompacted = 
#endif
                    pkDesc->CompactFromVector4Array(
                    (const D3DXVECTOR4*) pkDesc->m_afTempValues, 
                    pkDesc->m_afMaxValue,
                    pkDesc->m_uiNumRows, pkDesc->m_uiNumCols,
                    pkDesc->m_uiNumElements);
                NIASSERT(bCompacted);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiD3DXEffectParser::ParseFloatAnnotations(void* pvD3DXEffect, 
    void* pvParamDesc, void* pvParamHandle, 
    NiD3DXEffectParameterFloatDesc* pkDesc)
{
    LPD3DXEFFECT pkEffect = (LPD3DXEFFECT)pvD3DXEffect;
    D3DXPARAMETER_DESC* pkParamDesc = (D3DXPARAMETER_DESC*)pvParamDesc;
    D3DXHANDLE hParam = (D3DXHANDLE)pvParamHandle;

    // Query annotations
    for (unsigned int i = 0; i < pkParamDesc->Annotations; i++)
    {
        D3DXHANDLE hAnnot = pkEffect->GetAnnotation(hParam, i);
        D3DXPARAMETER_DESC kAnnotDesc;
        pkEffect->GetParameterDesc(hAnnot, &kAnnotDesc);
        BOOL bBool;
        /*if (NiStricmp(kAnnotDesc.Name, "uiname") == 0)
        {
            pkEffect->GetString(hAnnot, &(pkDesc->m_pcName));
        }
        else */if (NiStricmp(kAnnotDesc.Name, "desc") == 0 ||
            NiStricmp(kAnnotDesc.Name, "description") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uihelp") == 0)
        {
            pkEffect->GetString(hAnnot, &pkDesc->m_pcDesc);
        }
        else if (NiStricmp(kAnnotDesc.Name, "hidden") == 0)
        {
            pkEffect->GetBool(hAnnot, &bBool);
            pkDesc->m_bHidden = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.Name, "uiwidget") == 0)
        {
            const char* pcValue = NULL;
            pkEffect->GetString(hAnnot, &pcValue);
            if (pcValue && NiStricmp(pcValue, "none") == 0)
            {
                pkDesc->m_bHidden = true;
            }
        }
        else if (NiStricmp(kAnnotDesc.Name, "min") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uimin") == 0)
        {
            pkEffect->GetFloat(hAnnot, &pkDesc->m_fMinValue);
        }
        else if (NiStricmp(kAnnotDesc.Name, "max") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uimax") == 0)
        {
            pkEffect->GetFloat(hAnnot, &pkDesc->m_fMaxValue);
        }
    }
}
//---------------------------------------------------------------------------
void NiD3DXEffectParser::ParseBooleanAnnotations(void* pvD3DXEffect, 
    void* pvParamDesc, void* pvParamHandle, 
    NiD3DXEffectParameterBooleanDesc* pkDesc)
{
    LPD3DXEFFECT pkEffect = (LPD3DXEFFECT)pvD3DXEffect;
    D3DXPARAMETER_DESC* pkParamDesc = (D3DXPARAMETER_DESC*)pvParamDesc;
    D3DXHANDLE hParam = (D3DXHANDLE)pvParamHandle;

    BOOL bBool = FALSE;

    // Query annotations
    for (unsigned int i = 0; i < pkParamDesc->Annotations; i++)
    {
        D3DXHANDLE hAnnot = pkEffect->GetAnnotation(hParam, i);
        D3DXPARAMETER_DESC kAnnotDesc;
        pkEffect->GetParameterDesc(hAnnot, &kAnnotDesc);
        /*if (NiStricmp(kAnnotDesc.Name, "uiname") == 0)
        {
            pkEffect->GetString(hAnnot, &(pkDesc->m_pcName));
        }
        else */if (NiStricmp(kAnnotDesc.Name, "desc") == 0 ||
            NiStricmp(kAnnotDesc.Name, "description") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uihelp") == 0)
        {
            pkEffect->GetString(hAnnot, &pkDesc->m_pcDesc);
        }
        else if (NiStricmp(kAnnotDesc.Name, "hidden") == 0)
        {
            pkEffect->GetBool(hAnnot, &bBool);
            pkDesc->m_bHidden = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.Name, "uiwidget") == 0)
        {
            const char* pcValue = NULL;
            pkEffect->GetString(hAnnot, &pcValue);
            if (pcValue && NiStricmp(pcValue, "none") == 0)
            {
                pkDesc->m_bHidden = true;
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiD3DXEffectParser::ParseIntegerAnnotations(void* pvD3DXEffect, 
    void* pvParamDesc, void* pvParamHandle, 
    NiD3DXEffectParameterIntegerDesc* pkDesc)
{
    LPD3DXEFFECT pkEffect = (LPD3DXEFFECT)pvD3DXEffect;
    D3DXPARAMETER_DESC* pkParamDesc = (D3DXPARAMETER_DESC*)pvParamDesc;
    D3DXHANDLE hParam = (D3DXHANDLE)pvParamHandle;

    int iInt = 0;

    // Query annotations
    for (unsigned int i = 0; i < pkParamDesc->Annotations; i++)
    {
        D3DXHANDLE hAnnot = pkEffect->GetAnnotation(hParam, i);
        D3DXPARAMETER_DESC kAnnotDesc;
        pkEffect->GetParameterDesc(hAnnot, &kAnnotDesc);
        BOOL bBool;
        /*if (NiStricmp(kAnnotDesc.Name, "uiname") == 0)
        {
            pkEffect->GetString(hAnnot, &(pkDesc->m_pcName));
        }
        else */if (NiStricmp(kAnnotDesc.Name, "desc") == 0 ||
            NiStricmp(kAnnotDesc.Name, "description") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uihelp") == 0)
        {
            pkEffect->GetString(hAnnot, &pkDesc->m_pcDesc);
        }
        else if (NiStricmp(kAnnotDesc.Name, "hidden") == 0)
        {
            pkEffect->GetBool(hAnnot, &bBool);
            pkDesc->m_bHidden = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.Name, "uiwidget") == 0)
        {
            const char* pcValue = NULL;
            pkEffect->GetString(hAnnot, &pcValue);
            if (pcValue && NiStricmp(pcValue, "none") == 0)
            {
                pkDesc->m_bHidden = true;
            }
        }
        else if (NiStricmp(kAnnotDesc.Name, "min") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uimin") == 0)
        {
            pkEffect->GetInt(hAnnot, &iInt);
            pkDesc->m_uiMinValue = (unsigned int)iInt;
        }
        else if (NiStricmp(kAnnotDesc.Name, "max") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uimax") == 0)
        {
            pkEffect->GetInt(hAnnot, &iInt);
            pkDesc->m_uiMaxValue = (unsigned int)iInt;
        }
    }
}
//---------------------------------------------------------------------------
void NiD3DXEffectParser::ParseTextureAnnotations(void* pvD3DXEffect, 
    void* pvParamDesc, void* pvParamHandle, 
    NiD3DXEffectParameterTextureDesc* pkDesc)
{
    LPD3DXEFFECT pkEffect = (LPD3DXEFFECT)pvD3DXEffect;
    D3DXPARAMETER_DESC* pkParamDesc = (D3DXPARAMETER_DESC*)pvParamDesc;
    D3DXHANDLE hParam = (D3DXHANDLE)pvParamHandle;

    int iInt = 0;
    BOOL bBool = FALSE;
    const char* pcTextureType = NULL;

    pkDesc->m_uiTextureWidth = D3DX_DEFAULT;
    pkDesc->m_uiTextureHeight = D3DX_DEFAULT;
    pkDesc->m_uiTextureDepth = D3DX_DEFAULT;

    pkDesc->m_uiTextureIndex = 0xFFFFFFFF;
    pkDesc->m_uiObjectType = NiShaderAttributeDesc::OT_UNDEFINED;

    // Query annotations
    const char* pcObjectType = NULL;
    unsigned int uiObjectIndex = 0;
    bool bFoundObjectIndex = false;
    for (unsigned int i = 0; i < pkParamDesc->Annotations; i++)
    {
        D3DXHANDLE hAnnot = pkEffect->GetAnnotation(hParam, i);
        D3DXPARAMETER_DESC kAnnotDesc;
        pkEffect->GetParameterDesc(hAnnot, &kAnnotDesc);
        /*if (NiStricmp(kAnnotDesc.Name, "uiname") == 0)
        {
            pkEffect->GetString(hAnnot, &(pkDesc->m_pcName));
        }
        else */if (NiStricmp(kAnnotDesc.Name, "desc") == 0 ||
            NiStricmp(kAnnotDesc.Name, "description") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uihelp") == 0)
        {
            pkEffect->GetString(hAnnot, &pkDesc->m_pcDesc);
        }
        else if (NiStricmp(kAnnotDesc.Name, "hidden") == 0)
        {
            pkEffect->GetBool(hAnnot, &bBool);
            pkDesc->m_bHidden = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.Name, "uiwidget") == 0)
        {
            const char* pcValue = NULL;
            pkEffect->GetString(hAnnot, &pcValue);
            if (pcValue && NiStricmp(pcValue, "none") == 0)
            {
                pkDesc->m_bHidden = true;
            }
        }
        else if (NiStricmp(kAnnotDesc.Name, "name") == 0)
            pkEffect->GetString(hAnnot, &pkDesc->m_pcTextureName);
        else if (NiStricmp(kAnnotDesc.Name, "function") == 0)
            pkEffect->GetString(hAnnot, &pkDesc->m_pcTextureFunction);
        else if (NiStricmp(kAnnotDesc.Name, "target") == 0) 
            pkEffect->GetString(hAnnot, &pkDesc->m_pcTextureTarget);
        else if (NiStricmp(kAnnotDesc.Name, "width") == 0)
        {
            pkEffect->GetInt(hAnnot, &iInt);
            pkDesc->m_uiTextureWidth = (unsigned int)iInt;
        }
        else if (NiStricmp(kAnnotDesc.Name, "height") == 0)
        {
            pkEffect->GetInt(hAnnot, &iInt);
            pkDesc->m_uiTextureHeight = (unsigned int)iInt;
        }
        else if (NiStricmp(kAnnotDesc.Name, "depth") == 0)
        {
            pkEffect->GetInt(hAnnot, &iInt);
            pkDesc->m_uiTextureDepth = (unsigned int)iInt;
        }
        else if (NiStricmp(kAnnotDesc.Name, "type") == 0)
            pkEffect->GetString(hAnnot, &pcTextureType);
        else if (NiStricmp(kAnnotDesc.Name, "NTM") == 0)
            pkEffect->GetString(hAnnot, &pkDesc->m_pcTextureMap);
        else if (NiStricmp(kAnnotDesc.Name, "NTMIndex") == 0)
        {
            pkEffect->GetInt(hAnnot, &iInt);
            pkDesc->m_uiTextureIndex = (unsigned int)iInt;
        }
        else if (NiStricmp(kAnnotDesc.Name, "Object") == 0)
        {
            pkEffect->GetString(hAnnot, &pcObjectType);
        }
        else if (NiStricmp(kAnnotDesc.Name, "ObjectIndex") == 0)
        {
            pkEffect->GetInt(hAnnot, &iInt);
            uiObjectIndex = (unsigned int) iInt;
            bFoundObjectIndex = true;
        }
    }

    if (pcObjectType)
    {
        pkDesc->m_uiObjectType = DetermineObjectType(pcObjectType,
            uiObjectIndex, bFoundObjectIndex);
        pkDesc->m_uiObjectIndex = uiObjectIndex;
        pkDesc->m_pcTextureMap = NULL;
    }

    if (pcTextureType != NULL) 
    {
        if (NiStricmp(pcTextureType, "volume") == 0)
        {
            pkDesc->m_eTextureType = 
                NiD3DXEffectParameterTextureDesc::TT_3D;
        }
        else if (NiStricmp(pcTextureType, "cube") == 0)
        {
            pkDesc->m_eTextureType = 
                NiD3DXEffectParameterTextureDesc::TT_CUBE;
        }
    }
    else
    {
        pkDesc->m_eTextureType = NiD3DXEffectParameterTextureDesc::TT_2D;
    }

    // First try and locate the texture if it is given by name
    if (pkDesc->m_pcTextureName)
    {
        // Only look in current directory for now
        HANDLE hFile = CreateFile(pkDesc->m_pcTextureName, GENERIC_READ, 
            FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hFile == INVALID_HANDLE_VALUE) 
            pkDesc->m_pcTextureName = NULL;
        else
            CloseHandle(hFile);
    }

    // Indicate shader map if no other clues are given
    if (pkDesc->m_pcTextureName == NULL &&
        pkDesc->m_pcTextureFunction == NULL &&
        !pcObjectType)
    {
        if (pkDesc->m_pcTextureMap == NULL)
            pkDesc->m_pcTextureMap = "shader";

        if (strcmp(pkDesc->m_pcTextureMap, "shader") == 0 &&
            pkDesc->m_uiTextureIndex == 0xFFFFFFFF)
        {
            pkDesc->m_uiTextureIndex = ms_uiShaderMapIndex++;
        }
    }
}
//---------------------------------------------------------------------------
void NiD3DXEffectParser::ParsePredefinedAnnotations(void* pvD3DXEffect, 
    void* pvParamDesc, void* pvParamHandle, 
    NiD3DXEffectParameterPredefinedDesc* pkDesc)
{
    LPD3DXEFFECT pkEffect = (LPD3DXEFFECT)pvD3DXEffect;
    D3DXPARAMETER_DESC* pkParamDesc = (D3DXPARAMETER_DESC*)pvParamDesc;
    D3DXHANDLE hParam = (D3DXHANDLE)pvParamHandle;

    int iInt = 0;

    // Query annotations
    for (unsigned int i = 0; i < pkParamDesc->Annotations; i++)
    {
        D3DXHANDLE hAnnot = pkEffect->GetAnnotation(hParam, i);
        D3DXPARAMETER_DESC kAnnotDesc;
        pkEffect->GetParameterDesc(hAnnot, &kAnnotDesc);
        if (NiStricmp(kAnnotDesc.Name, "NTMIndex") == 0)
        {
            pkEffect->GetInt(hAnnot, &iInt);
            pkDesc->m_uiExtraData = (unsigned int)iInt;
        }
    }
}
//---------------------------------------------------------------------------
void NiD3DXEffectParser::ParseTechniqueAnnotations(void* pvD3DXEffect, 
    void* pvTechDesc, void* pvTechHandle, NiD3DXEffectTechniqueDesc* pkDesc)
{
    LPD3DXEFFECT pkEffect = (LPD3DXEFFECT)pvD3DXEffect;
    D3DXTECHNIQUE_DESC* pkTechDesc = (D3DXTECHNIQUE_DESC*)pvTechDesc;
    int iInt;
    BOOL bBool = FALSE;
    const char* pcNBT = NULL;

    // Get name, description, implentation, and user version annotations
    for (unsigned int i = 0; i < pkTechDesc->Annotations; i++)
    {
        D3DXHANDLE hAnnot = 
            pkEffect->GetAnnotation((D3DXHANDLE)pvTechHandle, i);
        D3DXPARAMETER_DESC kAnnotDesc;
        pkEffect->GetParameterDesc(hAnnot, &kAnnotDesc);
        if (NiStricmp(kAnnotDesc.Name, "desc") == 0 ||
            NiStricmp(kAnnotDesc.Name, "description") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uihelp") == 0)
        {
            pkEffect->GetString(hAnnot, &pkDesc->m_pcDesc);
        }
        else if (NiStricmp(kAnnotDesc.Name, "shadername") == 0)
        {
            pkEffect->GetString(hAnnot, &pkDesc->m_pcShaderName);
        }
        else if (NiStricmp(kAnnotDesc.Name, "implementation") == 0)
        {
            pkEffect->GetInt(hAnnot, &iInt);
            pkDesc->m_uiImplementation = (unsigned int)iInt;
        }
        else if (NiStricmp(kAnnotDesc.Name, "userversion") == 0)
        {
            pkEffect->GetInt(hAnnot, &iInt);
            pkDesc->m_uiUserVersion = (unsigned int)iInt;
        }
        else if (NiStricmp(kAnnotDesc.Name, "bonesperpartition") == 0)
        {
            pkEffect->GetInt(hAnnot, &iInt);
            pkDesc->m_uiBonesPerPartition = (unsigned int)iInt;
        }
        else if (NiStricmp(kAnnotDesc.Name, "nbtmethod") == 0)
        {
            pkEffect->GetString(hAnnot, &pcNBT);
        }
        else if (NiStricmp(kAnnotDesc.Name, "nbtsourceuv") == 0)
        {
            if (kAnnotDesc.Type == D3DXPT_STRING)
            {
                const char* pcString = NULL;
                pkEffect->GetString(hAnnot, &pcString);

                if (NiStricmp(pcString, "nbtuvdefault") == 0)
                {
                    pkDesc->m_uiNBTSourceUV = 
                        NiShaderDesc::BINORMALTANGENTUVSOURCEDEFAULT;
                }
            }
            else
            {
                pkEffect->GetInt(hAnnot, &iInt);
                pkDesc->m_uiNBTSourceUV = (unsigned int)iInt;
            }
        }
        else if (NiStricmp(kAnnotDesc.Name, "usesnirenderstate") == 0)
        {
            pkEffect->GetBool(hAnnot, &bBool);
            pkDesc->m_bUsesNiRenderState = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.Name, "usesnilightstate") == 0)
        {
            pkEffect->GetBool(hAnnot, &bBool);
            pkDesc->m_bUsesNiLightState = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.Name, "blendindicesasd3dcolor") == 0)
        {
            pkEffect->GetBool(hAnnot, &bBool);
            pkDesc->m_bBlendIndicesAsD3DColor = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.Name, "softwarevprequired") == 0)
        {
            pkEffect->GetBool(hAnnot, &bBool);
            pkDesc->m_bSoftwareVPRequired = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.Name, "softwarevpfallback") == 0)
        {
            pkEffect->GetBool(hAnnot, &bBool);
            pkDesc->m_bSoftwareVPAcceptible = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.Name, "semanticadaptertable") == 0)
        {
            // parse the SemanticAdapterTable annotation to build a real table
            const char* pcSemanticString = NULL;
            HRESULT eRes = pkEffect->GetString(hAnnot, &pcSemanticString);
            if (SUCCEEDED(eRes) && pcSemanticString)
            {
                bool bSuccess = NiSemanticAdapterTableParser::ParseString(
                    pcSemanticString, pkDesc->m_kAdapterTable);

                if (!bSuccess)
                {
                    NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                        true,
                        "NiD3DXEffectParser Warning: Invalid "
                        "SemanticAdapterTable annotation specified on the"
                        "\"%s\" technique",
                        pkTechDesc->Name);
                }
            }
        }
    }

    pkDesc->m_eNBTMethod = NiShaderRequirementDesc::NBT_METHOD_NONE;
    if (pcNBT)
    {
        if (NiStricmp(pcNBT, "NDL") == 0)
        {
            pkDesc->m_eNBTMethod = NiShaderRequirementDesc::NBT_METHOD_NDL;
        }
        else if (NiStricmp(pcNBT, "MAX") == 0)
        {
            NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, true,
                "NiD3DXEffectParser Warning: deprecated \"MAX\" value "
                "assigned to the \"NBTMethod\" annotation specified on "
                "\"%s\" technique", pkTechDesc->Name);
            pkDesc->m_eNBTMethod = NiShaderRequirementDesc::NBT_METHOD_NDL;
        }
        else if (NiStricmp(pcNBT, "ATI") == 0)
        {
            pkDesc->m_eNBTMethod = NiShaderRequirementDesc::NBT_METHOD_ATI;
        }
    }
}
//---------------------------------------------------------------------------
NiD3DXEffectParameterDesc* NiD3DXEffectParser::QueryPredefined(
    const char* pcSemantic, bool, unsigned int uiArrayElements)
{
    NIASSERT(pcSemantic != NULL);

    // First check the unmodified version
    unsigned int uiMapping = 0;
    unsigned int uiNumRegisters = 0;
    bool bSuccess = NiShaderConstantMap::LookUpPredefinedMapping(pcSemantic,
        uiMapping, uiNumRegisters);

    if (!bSuccess)
    {
        // Create new copy of semantic name for mangling
        size_t stLen = strlen(pcSemantic);
        char* pcLowerSemantic = NiAlloc(char, stLen + 1);
        unsigned int i = 0;
        for (; i < stLen; i++)
            pcLowerSemantic[i] = (char)tolower(pcSemantic[i]);
        char* pcLastChar = pcLowerSemantic + i;
        *pcLastChar = '\0';

        // Attempt to support a number of variations:
        //   - 3ds Max semantics:
        //     [XXX]
        //     [XXX]I   (inverse)
        //     [XXX]IT  (inverse transpose)
        //       where XXX = WORLD, VIEW, PROJECTION, WORLDVIEW, or
        //                   WORLDVIEWPROJ
        //   - DirectX Standard Semantics:
        //     [XXX]
        //     [XXX]Inverse             (inverse)
        //     [XXX]InverseTranspose    (inverse transpose)
        //       where XXX = World, View, Projection, WorldView, 
        //         WorldViewProjection, ViewProjection, Joint, JointWorld,
        //         JointWorldView, or JointWorldViewProjection
        bool bInverse = false;
        bool bTranspose = false;

        // Check for a Transpose matrix
        //   - [  ]IT (inverse as well!)    (3ds Max)
        //   - [  ]Transpose                (Gamebryo, DirectX)

        if (stLen > 2 && strncmp(pcLastChar - 2, "it", 2) == 0)
        {
            bInverse = true;
            bTranspose = true;
            stLen -= 2;
            pcLastChar -= 2;
            *pcLastChar = '\0';
        }
        else if (stLen > 5 && strncmp(pcLastChar - 5, "trans", 5) == 0)
        {
            bTranspose = true;
            stLen -= 5;
            pcLastChar -= 5;
            *pcLastChar = '\0';
        }
        else if (stLen > 9 && strncmp(pcLastChar - 9, "transpose", 9) == 0)
        {
            bTranspose = true;
            stLen -= 9;
            pcLastChar -= 9;
            *pcLastChar = '\0';
        }

        // Check for an Inverse matrix
        //   - Inv[]                (Gamebryo)
        //   - []I                  (3ds Max)
        //   - []Inverse            (DirectX)
        if (stLen > 3 && strncmp(pcLowerSemantic, "inv", 3) == 0)
        {
            bInverse = true;
            for (i = 0; i < stLen - 3; i++)
                pcLowerSemantic[i] = pcLowerSemantic[i + 3];
            stLen -= 3;
            pcLastChar -= 3;
            *pcLastChar = '\0';
        }
        else if (stLen > 1 && strncmp(pcLastChar - 1, "i", 1) == 0)
        {
            bInverse = true;
            stLen -= 1;
            pcLastChar -= 1;
            *pcLastChar = '\0';
        }
        else if (stLen > 7 && strncmp(pcLastChar - 7, "inverse", 7) == 0)
        {
            bInverse = true;
            stLen -= 7;
            pcLastChar -= 7;
            *pcLastChar = '\0';
        }

        // Assemble components into new semantic;
        unsigned int uiNewLen = (unsigned int)(stLen + 1 +
            (bInverse ? 3 : 0) + (bTranspose ? 9 : 0));
        char* pcNewSemantic = NiAlloc(char, uiNewLen);
        NiSprintf(pcNewSemantic, uiNewLen, "%s%s%s",
            (bInverse ? "Inv" : ""),
            pcLowerSemantic,
            (bTranspose ? "Transpose" : ""));
        NiFree(pcLowerSemantic);

        bSuccess = 
            NiShaderConstantMap::LookUpPredefinedMapping(pcNewSemantic, 
            uiMapping, uiNumRegisters);

        NiFree(pcNewSemantic);
    }

    if (bSuccess)
    {
        // Semantic corresponds to a Gamebryo constant map value

        NiD3DXEffectParameterPredefinedDesc* pkDesc = 
            NiD3DXEffectParameterDesc::GetPredefinedDesc();
        pkDesc->m_pcPredefined = pcSemantic;
        pkDesc->m_uiMapping = uiMapping;
        // Handle bone matrices
        if (uiMapping >=     NiShaderConstantMap::SCM_DEF_BONE_MATRIX_3 &&
            uiMapping <=     NiShaderConstantMap::SCM_DEF_SKINBONE_MATRIX_4)
        {
            pkDesc->m_uiArrayCount = uiArrayElements;
        }
        else
        {
            pkDesc->m_uiArrayCount = 1;
        }

        return pkDesc;
    }

    return NULL;
}
//---------------------------------------------------------------------------
NiD3DXEffectParameterDesc* NiD3DXEffectParser::QueryObject(void* pvD3DXEffect,
    void* pvParamHandle, void* pvParamDesc, const char* pcSemantic)
{
    LPD3DXEFFECT pkD3DXEffect = (LPD3DXEFFECT) pvD3DXEffect;
    D3DXHANDLE hParamHandle = (D3DXHANDLE) pvParamHandle;
    D3DXPARAMETER_DESC* pkParamDesc = (D3DXPARAMETER_DESC*) pvParamDesc;

    // This used to assert on pcSemantic as well. However, the data previously
    // in the semantic may now be in the ObjectProperty annotation so we'll
    // try to grab that annotation before we handle the object.
    NIASSERT(pkD3DXEffect && hParamHandle && pkParamDesc);

    // First, look for the required "Object" annotation.
    const char* pcObjectValue = LookupObjectAnnotation(pvD3DXEffect,
        pvParamHandle, pvParamDesc, "Object");
    if (!pcObjectValue)
    {
        // This parameter is not an object.
        return NULL;
    }

    const char* pcObjProp = NULL;
    D3DXHANDLE hObjProp = pkD3DXEffect->GetAnnotationByName(hParamHandle, 
        "ObjectProperty");
    if (hObjProp)
    {
        HRESULT eRes = pkD3DXEffect->GetString(hObjProp, &pcObjProp);
        if (SUCCEEDED(eRes) && pcObjProp)
        {
            // Replace pcSemantic
            pcSemantic = pcObjProp;
        }
    }

    // We have to have the semantic value of the object now either from
    // the actual FX semantic or from the annotation.
    if (!pcSemantic)
        return NULL;

    // Get the specified object index, if any.
    unsigned int uiObjectIndex = 0;
    bool bFoundObjectIndex = false;
    for (unsigned int ui = 0; ui < pkParamDesc->Annotations; ui++)
    {
        D3DXHANDLE hAnnot = pkD3DXEffect->GetAnnotation(hParamHandle, ui);
        D3DXPARAMETER_DESC kAnnotDesc;
        pkD3DXEffect->GetParameterDesc(hAnnot, &kAnnotDesc);
        if (NiStricmp(kAnnotDesc.Name, "ObjectIndex") == 0)
        {
            NIASSERT(kAnnotDesc.Type == D3DXPT_INT);
            int iIndex = 0;
            pkD3DXEffect->GetInt(hAnnot, &iIndex);
            uiObjectIndex = iIndex;
            bFoundObjectIndex = true;
            break;
        }
    }

    // Identify the object type from the appropriate annotation.
    NiShaderAttributeDesc::ObjectType eObjectType =
        (NiShaderAttributeDesc::ObjectType) DetermineObjectType(
        pcObjectValue, uiObjectIndex, bFoundObjectIndex);
    if (eObjectType == NiShaderAttributeDesc::OT_UNDEFINED)
    {
        return NULL;
    }

    // Identify object mapping from semantic.
    unsigned int uiMapping;
    if (!NiShaderConstantMap::LookUpObjectMapping(pcSemantic, uiMapping))
    {
        return NULL;
    }

    // Verify that the mapping is appropriate for the object type. Also,
    // read any supported additional annotations for the semantic, adjusting
    // the object mapping as necessary. uiMapping may be changed by this
    // function. This function returns false if the mapping is not
    // appropriate for the object type.
    if (!VerifyAndAdjustMapping(pvD3DXEffect, pvParamHandle, pvParamDesc,
        eObjectType, uiMapping))
    {
        return NULL;
    }

    // Create the parameter desc with the appropriate object type and mapping.
    NiD3DXEffectParameterObjectDesc* pkDesc =
        NiD3DXEffectParameterDesc::GetObjectDesc();
    pkDesc->m_uiMapping = uiMapping;
    pkDesc->m_eObjectType = eObjectType;
    pkDesc->m_uiObjectIndex = uiObjectIndex;

    return pkDesc;
}
//---------------------------------------------------------------------------
const char* NiD3DXEffectParser::LookupObjectAnnotation(void* pvD3DXEffect,
    void* pvParamHandle, void* pvParamDesc, const char* pcAnnotation)
{
    LPD3DXEFFECT pkD3DXEffect = (LPD3DXEFFECT) pvD3DXEffect;
    D3DXHANDLE hParamHandle = (D3DXHANDLE) pvParamHandle;
    D3DXPARAMETER_DESC* pkParamDesc = (D3DXPARAMETER_DESC*) pvParamDesc;

    NIASSERT(pkD3DXEffect && hParamHandle && pkParamDesc && pcAnnotation);

    const char* pcValue = NULL;
    for (unsigned int ui = 0; ui < pkParamDesc->Annotations; ui++)
    {
        D3DXHANDLE hAnnot = pkD3DXEffect->GetAnnotation(hParamHandle, ui);
        D3DXPARAMETER_DESC kAnnotDesc;
        pkD3DXEffect->GetParameterDesc(hAnnot, &kAnnotDesc);
        if (NiStricmp(kAnnotDesc.Name, pcAnnotation) == 0)
        {
            NIASSERT(kAnnotDesc.Type == D3DXPT_STRING);
            pkD3DXEffect->GetString(hAnnot, &pcValue);
            break;
        }
    }

    return pcValue;
}
//---------------------------------------------------------------------------
unsigned int NiD3DXEffectParser::DetermineObjectType(
    const char* pcObjectValue, unsigned int& uiObjectIndex,
    bool bFoundObjectIndex)
{
    unsigned int uiObjectType = NiShaderAttributeDesc::OT_UNDEFINED;
    if (NiStricmp(pcObjectValue, "GeneralLight") == 0)
    {
        uiObjectType = NiShaderAttributeDesc::OT_EFFECT_GENERALLIGHT;
        if (!bFoundObjectIndex)
        {
            uiObjectIndex = ms_uiGeneralLightIndex++;
        }
    }
    else if (NiStricmp(pcObjectValue, "PointLight") == 0)
    {
        uiObjectType = NiShaderAttributeDesc::OT_EFFECT_POINTLIGHT;
        if (!bFoundObjectIndex)
        {
            uiObjectIndex = ms_uiPointLightIndex++;
        }
    }
    else if (NiStricmp(pcObjectValue, "DirectionalLight") == 0)
    {
        uiObjectType = NiShaderAttributeDesc::OT_EFFECT_DIRECTIONALLIGHT;
        if (!bFoundObjectIndex)
        {
            uiObjectIndex = ms_uiDirectionalLightIndex++;
        }
    }
    else if (NiStricmp(pcObjectValue, "SpotLight") == 0)
    {
        uiObjectType = NiShaderAttributeDesc::OT_EFFECT_SPOTLIGHT;
        if (!bFoundObjectIndex)
        {
            uiObjectIndex = ms_uiSpotLightIndex++;
        }
    }
    else if (NiStricmp(pcObjectValue, "ShadowPointLight") == 0)
    {
        uiObjectType = NiShaderAttributeDesc::OT_EFFECT_SHADOWPOINTLIGHT;
        if (!bFoundObjectIndex)
        {
            uiObjectIndex = ms_uiShadowPointLightIndex++;
        }
    }
    else if (NiStricmp(pcObjectValue, "ShadowDirectionalLight") == 0)
    {
        uiObjectType = NiShaderAttributeDesc::OT_EFFECT_SHADOWDIRECTIONALLIGHT;
        if (!bFoundObjectIndex)
        {
            uiObjectIndex = ms_uiShadowDirectionalLightIndex++;
        }
    }
    else if (NiStricmp(pcObjectValue, "ShadowSpotLight") == 0)
    {
        uiObjectType = NiShaderAttributeDesc::OT_EFFECT_SHADOWSPOTLIGHT;
        if (!bFoundObjectIndex)
        {
            uiObjectIndex = ms_uiShadowSpotLightIndex++;
        }
    }
    else if (NiStricmp(pcObjectValue, "EnvironmentMap") == 0)
    {
        uiObjectType = NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP;
    }
    else if (NiStricmp(pcObjectValue, "ProjectedShadowMap") == 0)
    {
        uiObjectType = NiShaderAttributeDesc::OT_EFFECT_PROJECTEDSHADOWMAP;
        if (!bFoundObjectIndex)
        {
            uiObjectIndex = ms_uiProjShadowIndex++;
        }
    }
    else if (NiStricmp(pcObjectValue, "ProjectedLightMap") == 0)
    {
        uiObjectType = NiShaderAttributeDesc::OT_EFFECT_PROJECTEDLIGHTMAP;
        if (!bFoundObjectIndex)
        {
            uiObjectIndex = ms_uiProjLightIndex++;
        }
    }
    else if (NiStricmp(pcObjectValue, "FogMap") == 0)
    {
        uiObjectType = NiShaderAttributeDesc::OT_EFFECT_FOGMAP;
    }

    return uiObjectType;
}
//---------------------------------------------------------------------------
bool NiD3DXEffectParser::VerifyAndAdjustMapping(void* pvD3DXEffect,
    void* pvParamHandle, void* pvParamDesc, unsigned int uiObjectType,
    unsigned int& uiMapping)
{
    // This function reads any supported additional annotations for the input
    // mapping, adjusting the mapping as necessary. uiMapping may be changed
    // by this function. If the mapping is not appropriate for the input
    // object type, this function returns false.

    if (!NiShaderConstantMap::IsObjectMappingValidForType(
        (NiShaderConstantMap::ObjectMappings) uiMapping,
        (NiShaderAttributeDesc::ObjectType) uiObjectType))
    {
        NiD3DShaderFactory::ReportError(NISHADERERR_UNKNOWN, false,
            "NiD3DXEffectParser Error: Semantic specified for incompatible "
            "object type on \"%s\" parameter\n",
            ((D3DXPARAMETER_DESC*) pvParamDesc)->Name);
        return false;
    }

    const char* pcAnnotation = NULL;
    switch (uiMapping)
    {
        case NiShaderConstantMap::SCM_OBJ_DIMMER:
            // No additional annotations supported.
            break;
        case NiShaderConstantMap::SCM_OBJ_AMBIENT:
            pcAnnotation = LookupObjectAnnotation(pvD3DXEffect, pvParamHandle,
                pvParamDesc, "Usage");
            if (pcAnnotation && NiStricmp(pcAnnotation, "Undimmed") == 0)
            {
                uiMapping = NiShaderConstantMap::SCM_OBJ_UNDIMMEDAMBIENT;
            }
            break;
        case NiShaderConstantMap::SCM_OBJ_DIFFUSE:
            pcAnnotation = LookupObjectAnnotation(pvD3DXEffect, pvParamHandle,
                pvParamDesc, "Usage");
            if (pcAnnotation && NiStricmp(pcAnnotation, "Undimmed") == 0)
            {
                uiMapping = NiShaderConstantMap::SCM_OBJ_UNDIMMEDDIFFUSE;
            }
            break;
        case NiShaderConstantMap::SCM_OBJ_SPECULAR:
            pcAnnotation = LookupObjectAnnotation(pvD3DXEffect, pvParamHandle,
                pvParamDesc, "Usage");
            if (pcAnnotation && NiStricmp(pcAnnotation, "Undimmed") == 0)
            {
                uiMapping = NiShaderConstantMap::SCM_OBJ_UNDIMMEDSPECULAR;
            }
            break;
        case NiShaderConstantMap::SCM_OBJ_WORLDPOSITION:
            pcAnnotation = LookupObjectAnnotation(pvD3DXEffect, pvParamHandle,
                pvParamDesc, "Space");
            if (pcAnnotation)
            {
                if (NiStricmp(pcAnnotation, "Model") == 0)
                {
                    uiMapping = NiShaderConstantMap::SCM_OBJ_MODELPOSITION;
                }
            }
            break;
        case NiShaderConstantMap::SCM_OBJ_WORLDDIRECTION:
            pcAnnotation = LookupObjectAnnotation(pvD3DXEffect, pvParamHandle,
                pvParamDesc, "Space");
            if (pcAnnotation)
            {
                if (NiStricmp(pcAnnotation, "Model") == 0)
                {
                    uiMapping = NiShaderConstantMap::SCM_OBJ_MODELDIRECTION;
                }
            }
            break;
        case NiShaderConstantMap::SCM_OBJ_WORLDTRANSFORM:
            pcAnnotation = LookupObjectAnnotation(pvD3DXEffect, pvParamHandle,
                pvParamDesc, "Space");
            if (pcAnnotation)
            {
                if (NiStricmp(pcAnnotation, "Model") == 0)
                {
                    uiMapping = NiShaderConstantMap::SCM_OBJ_MODELTRANSFORM;
                }
            }
            break;
        case NiShaderConstantMap::SCM_OBJ_ATTENUATION:
            pcAnnotation = LookupObjectAnnotation(pvD3DXEffect, pvParamHandle,
                pvParamDesc, "Attenuation");
            if (pcAnnotation && NiStricmp(pcAnnotation, "Spot") == 0)
            {
                uiMapping = NiShaderConstantMap::SCM_OBJ_SPOTATTENUATION;
            }
            break;
        case NiShaderConstantMap::SCM_OBJ_WORLDPROJECTIONMATRIX:
            pcAnnotation = LookupObjectAnnotation(pvD3DXEffect, pvParamHandle,
                pvParamDesc, "Space");
            if (pcAnnotation)
            {
                if (NiStricmp(pcAnnotation, "Model") == 0)
                {
                    uiMapping =
                        NiShaderConstantMap::SCM_OBJ_MODELPROJECTIONMATRIX;
                }
            }
            break;
        case NiShaderConstantMap::SCM_OBJ_WORLDPROJECTIONTRANSLATION:
            pcAnnotation = LookupObjectAnnotation(pvD3DXEffect, pvParamHandle,
                pvParamDesc, "Space");
            if (pcAnnotation)
            {
                if (NiStricmp(pcAnnotation, "Model") == 0)
                {
                    uiMapping = NiShaderConstantMap::
                        SCM_OBJ_MODELPROJECTIONTRANSLATION;
                }
            }
            break;
        case NiShaderConstantMap::SCM_OBJ_WORLDCLIPPINGPLANE:
            pcAnnotation = LookupObjectAnnotation(pvD3DXEffect, pvParamHandle,
                pvParamDesc, "Space");
            if (pcAnnotation)
            {
                if (NiStricmp(pcAnnotation, "Model") == 0)
                {
                    uiMapping =
                        NiShaderConstantMap::SCM_OBJ_MODELCLIPPINGPLANE;
                }
            }
            break;
        case NiShaderConstantMap::SCM_OBJ_TEXCOORDGEN:
            // No additional annotations supported.
            break;
        case NiShaderConstantMap::SCM_OBJ_WORLDPROJECTIONTRANSFORM:
            pcAnnotation = LookupObjectAnnotation(pvD3DXEffect, pvParamHandle,
                pvParamDesc, "Space");
            if (pcAnnotation)
            {
                if (NiStricmp(pcAnnotation, "Model") == 0)
                {
                    uiMapping =
                        NiShaderConstantMap::SCM_OBJ_MODELPROJECTIONTRANSFORM;
                }
            }
            break;
        case NiShaderConstantMap::SCM_OBJ_WORLDTOSHADOWMAPMATRIX:
        case NiShaderConstantMap::SCM_OBJ_SHADOWMAP:
        case NiShaderConstantMap::SCM_OBJ_SHADOWMAPTEXSIZE:
        case NiShaderConstantMap::SCM_OBJ_SHADOWBIAS:
        case NiShaderConstantMap::SCM_OBJ_SHADOW_VSM_POWER_EPSILON:
            // No additional annotations supported.
            break;
    }

    return true;
}
//---------------------------------------------------------------------------
