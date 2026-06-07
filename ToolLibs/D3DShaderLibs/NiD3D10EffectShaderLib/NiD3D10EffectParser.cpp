
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

#include "NiD3D10EffectParser.h"
#include "NiD3D10EffectParameterDesc.h"
#include "NiD3D10ShaderFactory.h"
#include "NiD3D10EffectTechniqueDesc.h"

#include <NiD3D10Renderer.h>
#include <NiShaderDesc.h>
#include <NiSemanticAdapterTableParser.h>

NiUInt32 NiD3D10EffectParser::ms_uiShaderMapIndex = 0;
NiUInt32 NiD3D10EffectParser::ms_uiGeneralLightIndex = 0;
NiUInt32 NiD3D10EffectParser::ms_uiPointLightIndex = 0;
NiUInt32 NiD3D10EffectParser::ms_uiDirectionalLightIndex = 0;
NiUInt32 NiD3D10EffectParser::ms_uiSpotLightIndex = 0;
NiUInt32 NiD3D10EffectParser::ms_uiShadowPointLightIndex = 0;
NiUInt32 NiD3D10EffectParser::ms_uiShadowDirectionalLightIndex = 0;
NiUInt32 NiD3D10EffectParser::ms_uiShadowSpotLightIndex = 0;
NiUInt32 NiD3D10EffectParser::ms_uiProjShadowIndex = 0;
NiUInt32 NiD3D10EffectParser::ms_uiProjLightIndex = 0;

//---------------------------------------------------------------------------
void NiD3D10EffectParser::_SDMShutdown()
{
    // Release all the strings in the parameters by "getting" one of each.
    NiD3D10EffectParameterDesc::GetMatrixDesc();
    NiD3D10EffectParameterDesc::GetVectorDesc();
    NiD3D10EffectParameterDesc::GetFloatDesc();
    NiD3D10EffectParameterDesc::GetBooleanDesc();
    NiD3D10EffectParameterDesc::GetIntegerDesc();
    NiD3D10EffectParameterDesc::GetTextureDesc();
    NiD3D10EffectParameterDesc::GetPredefinedDesc();
    NiD3D10EffectParameterDesc::GetObjectDesc();
    NiD3D10EffectParameterDesc::GetArrayDesc();
}
//---------------------------------------------------------------------------
NiD3D10EffectParameterDesc* NiD3D10EffectParser::GetParameter(
    ID3D10Effect* pkEffect, 
    NiUInt32 uiIndex)
{
    ID3D10EffectVariable* pkVariable = pkEffect->GetVariableByIndex(uiIndex);
    
    NiD3D10EffectParameterDesc* pkParamDesc = GetParameterDesc(pkVariable);

    return pkParamDesc;
}
//---------------------------------------------------------------------------
void NiD3D10EffectParser::GetTechnique(
    NiD3D10EffectTechniqueDesc& kTechniqueOut,
    ID3D10Effect* pkEffect, 
    NiUInt32 uiIndex)
{
    ID3D10EffectTechnique* pkTechnique = 
        pkEffect->GetTechniqueByIndex(uiIndex);

    GetTechniqueDesc(kTechniqueOut, pkTechnique);

}
//---------------------------------------------------------------------------
NiD3D10EffectParameterDesc* NiD3D10EffectParser::GetParameterDesc(
    ID3D10EffectVariable* pkVariable)
{
    NIASSERT(pkVariable && pkVariable->IsValid());

    // Get description of variable
    D3D10_EFFECT_VARIABLE_DESC kParamDesc;
    HRESULT hr = pkVariable->GetDesc(&kParamDesc);
    NIASSERT(SUCCEEDED(hr));

    const char* pcSemantic = kParamDesc.Semantic;

    // Get description of variable type
    ID3D10EffectType* pkType = pkVariable->GetType();
    NIASSERT(pkType && pkType->IsValid());

    D3D10_EFFECT_TYPE_DESC kTypeDesc;
    hr = pkType->GetDesc(&kTypeDesc);
    NIASSERT(SUCCEEDED(hr));

    D3D10_SHADER_VARIABLE_CLASS eClass = kTypeDesc.Class;
    D3D10_SHADER_VARIABLE_TYPE eType = kTypeDesc.Type;

    // First check for numerical parameters
    if (eType == D3D10_SVT_BOOL ||
        eType == D3D10_SVT_INT ||
        eType == D3D10_SVT_FLOAT ||
        eType == D3D10_SVT_UINT ||
        eType == D3D10_SVT_UINT8)
    {
        // Check for "StandardsGlobal" parameter
        if (pcSemantic && NiStricmp(pcSemantic, "StandardsGlobal") == 0)
        {
            // Currently unable to do anything about it, really
            return NULL;
        }

        // Map semantic to either a predefined constant map value
        // or to an NiShader attribute

        NiD3D10EffectParameterDesc* pkDesc = NULL;

        // Default all of these to false. If we fall through the code below,
        // we will default to an attribute.
        bool bAttribute = false;
        bool bGlobal = false;
        bool bObject = false;
        bool bPredefined = false;

        // Get VarType from annotations as was added with FXL support.
        bool bFoundAnnotation = false;
        ID3D10EffectVariable* pkAnnotation = 
            pkVariable->GetAnnotationByName("VarType");
        if (pkAnnotation->IsValid() && pkAnnotation->AsString()->IsValid())
        {
            const char* pcVarType = NULL;
            HRESULT hr = pkAnnotation->AsString()->GetString(&pcVarType);

            if (SUCCEEDED(hr))
            {
                NIASSERT(pcVarType);
                bAttribute = (NiStricmp(pcVarType, "ATTRIBUTE") == 0);
                bGlobal = (NiStricmp(pcVarType, "GLOBAL") == 0);
                bObject = (NiStricmp(pcVarType, "OBJECT") == 0);
                bPredefined = (NiStricmp(pcVarType, "PREDEFINED") == 0);
                bFoundAnnotation = true;

                // If VarType is "Predefined" let's try to find the
                // DefinedMapping annotation and replace pcSemantic with that
                // value.
                if (bPredefined)
                {
                    pkAnnotation = 
                        pkVariable->GetAnnotationByName("DefinedMapping");
                    if (pkAnnotation->IsValid())
                    {
                        const char* pcDefMap = NULL;
                        hr = pkAnnotation->AsString()->GetString(&pcDefMap);
                        if (SUCCEEDED(hr))
                        {
                            NIASSERT(pcDefMap);
                            pcSemantic = pcDefMap;
                        }
                    }
                }
            }
        }

        // We didn't find the annotation and we have a semantic. We could flag
        // as ATTRIBUTE or GLOBAL here.
        if (pcSemantic != NULL && !bFoundAnnotation)
        {
            bAttribute = (NiStricmp(pcSemantic, "ATTRIBUTE") == 0);
            bGlobal = (NiStricmp(pcSemantic, "GLOBAL") == 0);
        }

        if (!bAttribute && !bGlobal)
        {
            // At this point, the parameter is object, predefined, or unknown.
            pkDesc = QueryObject(pkVariable, kParamDesc, pcSemantic);
            if (pkDesc)
            {
                bObject = true;
            }
            else if (pcSemantic)
            {
                pkDesc = QueryPredefined(pcSemantic, 
                    (eClass == D3D10_SVC_MATRIX_COLUMNS), kTypeDesc.Elements);
                if (pkDesc)
                {
                    bPredefined = true;
                    ParsePredefinedAnnotations(pkVariable,
                        kParamDesc, 
                        kTypeDesc,
                        (NiD3D10EffectParameterPredefinedDesc*)pkDesc);
                }
            }
        }

        // Unknown semantic -> attribute
        if (!bGlobal && !bObject && !bPredefined)
            bAttribute = true;

        NiShaderAttributeDesc::AttributeType eAttribType = 
            NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
        if (pkDesc == NULL)
        {
            // Semantic must map to an attribute
            if ((eClass == D3D10_SVC_VECTOR && kTypeDesc.Columns == 1) ||
                ((eClass == D3D10_SVC_MATRIX_ROWS || 
                eClass == D3D10_SVC_MATRIX_COLUMNS) && 
                kTypeDesc.Elements == 1))
            {
                eClass = D3D10_SVC_SCALAR;
            }

            if (eType  == D3D10_SVT_FLOAT && kTypeDesc.Elements > 1 && 
               (eClass == D3D10_SVC_VECTOR || eClass == D3D10_SVC_SCALAR))
            {
                // Handle float arrays (scalars and vectors only)
                NiD3D10EffectParameterArrayDesc* pkArrayDesc = 
                    NiD3D10EffectParameterDesc::GetArrayDesc();

                pkArrayDesc->m_uiNumRows = kTypeDesc.Rows;
                pkArrayDesc->m_uiNumCols = kTypeDesc.Columns;
                pkArrayDesc->m_uiNumElements = kTypeDesc.Elements;
                
                if (pkArrayDesc->m_uiNumElements >
                    NiD3D10EffectParameterArrayDesc::MAX_ROWS)
                {
                    NiOutputDebugString("Array has more than "
                        "NiD3D10EffectParameterArrayDesc::MAX_ROWS worth"
                        " of elements. Please use fewer elements.");
                    
                    return false;
                }

                // Search for annotations
                ParseArrayAnnotations(
                    pkVariable, 
                    kParamDesc, 
                    kTypeDesc, 
                    pkArrayDesc);
                
                HRESULT hr = S_OK;
                bool bCompactTempArray = false;
                if (kTypeDesc.Class == D3D10_SVC_SCALAR)
                {
                    NIASSERT(pkVariable->AsScalar()->IsValid());
                    hr = pkVariable->AsScalar()->GetFloatArray(
                        pkArrayDesc->m_afInitValue, 
                        0,
                        pkArrayDesc->m_uiNumElements);
                }
                else if (kTypeDesc.Class == D3D10_SVC_VECTOR)
                {
                    NIASSERT(pkVariable->AsVector()->IsValid());
                    float* pfDest = NULL;
                    if (kTypeDesc.Rows == 1 && kTypeDesc.Columns == 4)
                    {
                        // Vector 4's are the same size as the entries in the 
                        // array, so no need to waste time on a temp array.
                        pfDest = pkArrayDesc->m_afInitValue;
                    }
                    else
                    {
                        // Read into the temporary buffer
                        // because D3D10 forces the initial values to be
                        // D3DXVECTOR4's when they will often be smaller.
                        // The temp array will hold the values as 
                        // D3DXVECTOR4's and we will compact the values
                        // in a later method.
                        pfDest = pkArrayDesc->m_afTempValues;
                        bCompactTempArray = true;
                    }
                    hr = pkVariable->AsVector()->GetFloatVectorArray(
                        pfDest, 
                        0,
                        pkArrayDesc->m_uiNumElements);
                }
                        
                if (FAILED(hr))
                {
                    for (NiUInt32 ui = 0; ui < 
                        NiD3D10EffectParameterArrayDesc::MAX_ENTRIES; ui++)
                    {
                        pkArrayDesc->m_afInitValue[ui] = 0.0f;
                    }
                }
                else if (bCompactTempArray)
                {
                    NIVERIFY(pkArrayDesc->CompactFromVector4Array(
                        (const D3DXVECTOR4*)pkArrayDesc->m_afTempValues, 
                        pkArrayDesc->m_afInitValue,
                        pkArrayDesc->m_uiNumRows, pkArrayDesc->m_uiNumCols,
                        pkArrayDesc->m_uiNumElements));
                }
                
                eAttribType = NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY;
                pkDesc = pkArrayDesc;
            }
            else if (eClass == D3D10_SVC_MATRIX_ROWS || 
                eClass == D3D10_SVC_MATRIX_COLUMNS)
            {
                NIASSERT(pkVariable->AsMatrix()->IsValid());

                // Can currently only handle float matrices
                if (eType != D3D10_SVT_FLOAT)
                    return false;

                NiD3D10EffectParameterMatrixDesc* pkMatrixDesc = 
                    NiD3D10EffectParameterDesc::GetMatrixDesc();

                pkMatrixDesc->m_uiNumRows = kTypeDesc.Rows;
                pkMatrixDesc->m_uiNumCols = kTypeDesc.Columns;

                // Search for annotations
                ParseMatrixAnnotations(
                    pkVariable, 
                    kParamDesc, 
                    kTypeDesc,
                    pkMatrixDesc);

                // Check for initial value
                HRESULT hr;
                if (eClass == D3D10_SVC_MATRIX_COLUMNS)
                {
                    hr = pkVariable->AsMatrix()->GetMatrix(
                        pkMatrixDesc->m_afInitValue);
                }
                else
                {
                    hr = pkVariable->AsMatrix()->GetMatrixTranspose(
                        pkMatrixDesc->m_afInitValue);
                }

                if (FAILED(hr))
                {
                    D3DXMatrixIdentity(
                        (D3DXMATRIX*)pkMatrixDesc->m_afInitValue);
                }

                if (kTypeDesc.Rows == 2 && kTypeDesc.Columns == 4)
                    eAttribType = NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT8;
                else if (kTypeDesc.Rows == 3 && kTypeDesc.Columns == 4)
                    eAttribType = NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT12;
                else if (kTypeDesc.Rows == 3 && kTypeDesc.Columns == 3)
                    eAttribType = NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
                else
                    eAttribType = NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
                pkDesc = pkMatrixDesc;
            }
            else if (eClass == D3D10_SVC_VECTOR)
            {
                NIASSERT(pkVariable->AsVector()->IsValid());

                // Can currently only handle float vectors/colors
                if (eType != D3D10_SVT_FLOAT)
                    return false;

                NiD3D10EffectParameterVectorDesc* pkVectorDesc = 
                    NiD3D10EffectParameterDesc::GetVectorDesc();

                pkVectorDesc->m_uiNumRows = kTypeDesc.Rows;
                pkVectorDesc->m_uiNumCols = kTypeDesc.Columns;
                pkVectorDesc->m_uiNumAtomicElements = kTypeDesc.Columns;
                
                if (pkVectorDesc->m_uiNumAtomicElements >
                    NiD3D10EffectParameterVectorDesc::MAX_ENTRIES)
                {
                    return false;
                }

                // Search for annotations
                ParseVectorAnnotations(
                    pkVariable, 
                    kParamDesc, 
                    kTypeDesc,
                    pkVectorDesc);

                HRESULT hr = pkVariable->AsVector()->GetFloatVector(
                    pkVectorDesc->m_afInitValue);
                    
                if (FAILED(hr))
                {
                    for (NiUInt32 ui = 0; ui < 
                        NiD3D10EffectParameterVectorDesc::MAX_ENTRIES; ui++)
                    {
                        pkVectorDesc->m_afInitValue[ui] = 0.0f;
                    }
                }
                
                if (pkVectorDesc->GetVectorType() == 
                    NiD3D10EffectParameterVectorDesc::TYPE_POINT2)
                {
                    eAttribType = NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
                }
                else if (pkVectorDesc->GetVectorType() == 
                    NiD3D10EffectParameterVectorDesc::TYPE_POINT3)
                {
                    if (pkVectorDesc->m_bColor)
                    {
                        eAttribType = NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
                    }
                    else
                    {
                        eAttribType = 
                            NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
                    }
                }
                else if (pkVectorDesc->GetVectorType() == 
                    NiD3D10EffectParameterVectorDesc::TYPE_POINT4)
                {
                    if (pkVectorDesc->m_bColor)
                    {
                        eAttribType = NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
                    }
                    else
                    {
                        eAttribType = 
                            NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
                    }
                }

                pkDesc = pkVectorDesc;
            }
            else if (eClass == D3D10_SVC_SCALAR)
            {
                NIASSERT(pkVariable->AsScalar()->IsValid());

                // Can currently only handle float, bool, or int scalars
                if (eType == D3D10_SVT_FLOAT)
                {
                    NiD3D10EffectParameterFloatDesc* pkFloatDesc = 
                        NiD3D10EffectParameterDesc::GetFloatDesc();

                    // Search for annotations
                    ParseFloatAnnotations(
                        pkVariable, 
                        kParamDesc, 
                        kTypeDesc, 
                        pkFloatDesc);

                    // Check for initial value
                    float fFloat;
                    HRESULT hr = pkVariable->AsScalar()->GetFloat(&fFloat);
                    if (SUCCEEDED(hr))
                        pkFloatDesc->m_fInitValue = fFloat;
                    else
                        pkFloatDesc->m_fInitValue = 0.0f;

                    eAttribType = NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
                    pkDesc = pkFloatDesc;
                }
                else if (eType == D3D10_SVT_BOOL)
                {
                    NiD3D10EffectParameterBooleanDesc* pkBooleanDesc = 
                        NiD3D10EffectParameterDesc::GetBooleanDesc();

                    // Search for annotations
                    ParseBooleanAnnotations(
                        pkVariable, 
                        kParamDesc, 
                        kTypeDesc,
                        pkBooleanDesc);

                    // Check for initial value
                    BOOL bBool;
                    HRESULT hr = pkVariable->AsScalar()->GetBool(&bBool);
                    if (SUCCEEDED(hr))
                        pkBooleanDesc->m_bInitValue = (bBool != FALSE);
                    else
                        pkBooleanDesc->m_bInitValue = false;

                    eAttribType = NiShaderAttributeDesc::ATTRIB_TYPE_BOOL;
                    pkDesc = pkBooleanDesc;
                }
                else if (eType == D3D10_SVT_INT ||
                    eType == D3D10_SVT_UINT ||
                    eType == D3D10_SVT_UINT8)
                {
                    NiD3D10EffectParameterIntegerDesc* pkIntegerDesc = 
                        NiD3D10EffectParameterDesc::GetIntegerDesc();

                    // Search for annotations
                    ParseIntegerAnnotations(
                        pkVariable, 
                        kParamDesc, 
                        kTypeDesc,
                        pkIntegerDesc);

                    // Check for initial value
                    int iInt;
                    HRESULT hr = pkVariable->AsScalar()->GetInt(&iInt);
                    if (SUCCEEDED(hr))
                        pkIntegerDesc->m_uiInitValue = (NiUInt32)iInt;
                    else
                        pkIntegerDesc->m_uiInitValue = 0;

                    eAttribType = 
                        NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT;
                    pkDesc = pkIntegerDesc;
                }
            }
            if (pkDesc)
                pkDesc->m_bGlobal = bGlobal;
        }

        if (pkDesc)
        {
            NiUInt32 uiFlags = 0;
            if (bPredefined)
            {
                uiFlags = NiShaderConstantMapEntry::SCME_MAP_DEFINED;
            }
            else if (bObject)
            {
                NiD3D10EffectParameterObjectDesc* pkObjectDesc = 
                    (NiD3D10EffectParameterObjectDesc*)pkDesc;
                uiFlags = NiShaderConstantMapEntry::SCME_MAP_OBJECT |
                    NiShaderConstantMapEntry::GetObjectFlags(
                    pkObjectDesc->m_eObjectType);
            }
            else
            {
                if (bGlobal)
                    uiFlags = NiShaderConstantMapEntry::SCME_MAP_GLOBAL;
                else if (bAttribute)
                    uiFlags = NiShaderConstantMapEntry::SCME_MAP_ATTRIBUTE;
                NIASSERT(uiFlags != 0);

                NIASSERT(eAttribType != 
                    NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED);
                uiFlags |= eAttribType;
            }

            // Set flags
            pkDesc->m_uiFlags = uiFlags;

            // Get constant buffer name
            ID3D10EffectConstantBuffer* pkEffectBuffer = 
                pkVariable->GetParentConstantBuffer();
            D3D10_EFFECT_VARIABLE_DESC kDesc;
            hr = pkEffectBuffer->GetDesc(&kDesc);
            NIASSERT(SUCCEEDED(hr));
            pkDesc->m_kConstantBuffer = kDesc.Name;

            // Confirm name
            if (!pkDesc->m_kName.Exists())
                pkDesc->m_kName = kParamDesc.Name;
        }
        return pkDesc;
    }

    // Not a numeric parameter - continue on to check for texture
    if (   eType == D3D10_SVT_TEXTURE
        || eType == D3D10_SVT_TEXTURE1D
        || eType == D3D10_SVT_TEXTURE2D
        || eType == D3D10_SVT_TEXTURE3D
        || eType == D3D10_SVT_TEXTURECUBE
        || eType == D3D10_SVT_TEXTURE1DARRAY
        || eType == D3D10_SVT_TEXTURE2DARRAY
        || eType == D3D10_SVT_TEXTURE2DMS
        || eType == D3D10_SVT_TEXTURE2DMSARRAY
#if _DXSDK_PRODUCT_MINOR >= 18 // Check for April 2007 (or later) DX SDK
        || eType == D3D10_SVT_TEXTURECUBEARRAY
#endif //#if _DXSDK_PRODUCT_MINOR >= 18
        )
    {
        NIASSERT(pkVariable->AsShaderResource()->IsValid());
        NiD3D10EffectParameterTextureDesc* pkDesc = 
            NiD3D10EffectParameterDesc::GetTextureDesc();

        pkDesc->m_kName = kParamDesc.Name;

        // Search for annotations
        ParseTextureAnnotations(
            pkVariable, 
            kParamDesc, 
            kTypeDesc,
            pkDesc);

        // Check for initial value
        ID3D10ShaderResourceView* pkTexture;
        HRESULT hr = pkVariable->AsShaderResource()->GetResource(
            &pkTexture);
        if (FAILED(hr))
            pkTexture = NULL;
        pkDesc->m_pvInitValue = pkTexture;
        return pkDesc;
    }

    // No luck
    return NULL;
}
//---------------------------------------------------------------------------
void NiD3D10EffectParser::GetTechniqueDesc(
    NiD3D10EffectTechniqueDesc& kDesc,
    ID3D10EffectTechnique* pkTechnique)
{
    NIASSERT(pkTechnique && pkTechnique->IsValid());

    D3D10_TECHNIQUE_DESC kTechniqueDesc;
    HRESULT hr = pkTechnique->GetDesc(&kTechniqueDesc);
    NIASSERT(SUCCEEDED(hr));

    kDesc.m_kTechniqueName = kTechniqueDesc.Name;
    kDesc.m_uiNBTSourceUV = NiShaderDesc::BINORMALTANGENTUVSOURCEDEFAULT;

    if (kTechniqueDesc.Annotations)
        ParseTechniqueAnnotations(pkTechnique, kTechniqueDesc, &kDesc);

    if (!kDesc.m_kShaderName.Exists())
        kDesc.m_kShaderName = kDesc.m_kTechniqueName;

    // If this assertion gets hit, then the shader has more passes than
    // expected. Increase the value of MAX_PASS_COUNT as necessary and 
    // recompile.
    NIASSERT(kTechniqueDesc.Passes < 
        NiD3D10EffectTechniqueDesc::MAX_PASS_COUNT);

    // Get vertex/geometry/pixel shader versions
    kDesc.m_uiPassCount = kTechniqueDesc.Passes;
    for (NiUInt32 uiPass = 0; uiPass < kTechniqueDesc.Passes; uiPass++)
    {
        NiUInt32 uiVSVersion = 0;
        NiUInt32 uiGSVersion = 0;
        NiUInt32 uiPSVersion = 0;

        ID3D10EffectPass* pkPass = pkTechnique->GetPassByIndex(uiPass);
        D3D10_PASS_SHADER_DESC kPassShaderDesc;

        hr = pkPass->GetVertexShaderDesc(&kPassShaderDesc);
        if (SUCCEEDED(hr))
        {
            D3D10_EFFECT_SHADER_DESC kEffectShaderDesc;
            hr = kPassShaderDesc.pShaderVariable->GetShaderDesc(
                kPassShaderDesc.ShaderIndex, &kEffectShaderDesc);
            if (SUCCEEDED(hr))
            {
                ID3D10ShaderReflection* pkReflection = NULL;
                hr = NiD3D10Renderer::D3D10ReflectShader(
                    kEffectShaderDesc.pBytecode,
                    kEffectShaderDesc.BytecodeLength,
                    &pkReflection);
                if (SUCCEEDED(hr) && pkReflection)
                {
                    // Get shader version
                    D3D10_SHADER_DESC kShaderDesc;
                    hr = pkReflection->GetDesc(&kShaderDesc);
                    if (SUCCEEDED(hr))
                        uiVSVersion = kShaderDesc.Version;

                    kDesc.m_auiBufferCount[uiPass]
                        [NiGPUProgram::PROGRAM_VERTEX] =
                        (NiUInt8)kShaderDesc.ConstantBuffers;
                    for (NiUInt32 i = 0; i < kShaderDesc.ConstantBuffers; i++)
                    {
                        ID3D10ShaderReflectionConstantBuffer* pkCB = 
                            pkReflection->GetConstantBufferByIndex(i);
                        NIASSERT(pkCB);
                        D3D10_SHADER_BUFFER_DESC kShaderBufferDesc;
                        pkCB->GetDesc(&kShaderBufferDesc);
                        kDesc.m_aaakConstantBuffers[uiPass]
                            [NiGPUProgram::PROGRAM_VERTEX][i] = 
                            kShaderBufferDesc.Name;
                    }
                }

                if (pkReflection)
                    pkReflection->Release();
            }
        }

        hr = pkPass->GetGeometryShaderDesc(&kPassShaderDesc);
        if (SUCCEEDED(hr))
        {
            D3D10_EFFECT_SHADER_DESC kEffectShaderDesc;
            hr = kPassShaderDesc.pShaderVariable->GetShaderDesc(
                kPassShaderDesc.ShaderIndex, &kEffectShaderDesc);
            if (SUCCEEDED(hr))
            {
                ID3D10ShaderReflection* pkReflection = NULL;
                hr = NiD3D10Renderer::D3D10ReflectShader(
                    kEffectShaderDesc.pBytecode,
                    kEffectShaderDesc.BytecodeLength,
                    &pkReflection);
                if (SUCCEEDED(hr) && pkReflection)
                {
                    D3D10_SHADER_DESC kShaderDesc;
                    hr = pkReflection->GetDesc(&kShaderDesc);
                    if (SUCCEEDED(hr))
                        uiGSVersion = kShaderDesc.Version;

                    kDesc.m_auiBufferCount[uiPass]
                        [NiGPUProgram::PROGRAM_GEOMETRY] =
                        (NiUInt8)kShaderDesc.ConstantBuffers;
                    for (NiUInt32 i = 0; i < kShaderDesc.ConstantBuffers; i++)
                    {
                        ID3D10ShaderReflectionConstantBuffer* pkCB = 
                            pkReflection->GetConstantBufferByIndex(i);
                        NIASSERT(pkCB);
                        D3D10_SHADER_BUFFER_DESC kShaderBufferDesc;
                        pkCB->GetDesc(&kShaderBufferDesc);
                        kDesc.m_aaakConstantBuffers[uiPass]
                            [NiGPUProgram::PROGRAM_GEOMETRY][i] = 
                            kShaderBufferDesc.Name;
                    }
                }

                if (pkReflection)
                    pkReflection->Release();
            }
        }

        hr = pkPass->GetPixelShaderDesc(&kPassShaderDesc);
        if (SUCCEEDED(hr))
        {
            D3D10_EFFECT_SHADER_DESC kEffectShaderDesc;
            hr = kPassShaderDesc.pShaderVariable->GetShaderDesc(
                kPassShaderDesc.ShaderIndex, &kEffectShaderDesc);
            if (SUCCEEDED(hr))
            {
                ID3D10ShaderReflection* pkReflection = NULL;
                hr = NiD3D10Renderer::D3D10ReflectShader(
                    kEffectShaderDesc.pBytecode,
                    kEffectShaderDesc.BytecodeLength,
                    &pkReflection);
                if (SUCCEEDED(hr) && pkReflection)
                {
                    D3D10_SHADER_DESC kShaderDesc;
                    hr = pkReflection->GetDesc(&kShaderDesc);
                    if (SUCCEEDED(hr))
                        uiPSVersion = kShaderDesc.Version;

                    kDesc.m_auiBufferCount[uiPass]
                        [NiGPUProgram::PROGRAM_PIXEL] =
                        (NiUInt8)kShaderDesc.ConstantBuffers;
                    for (NiUInt32 i = 0; i < kShaderDesc.ConstantBuffers; i++)
                    {
                        ID3D10ShaderReflectionConstantBuffer* pkCB = 
                            pkReflection->GetConstantBufferByIndex(i);
                        NIASSERT(pkCB);
                        D3D10_SHADER_BUFFER_DESC kShaderBufferDesc;
                        pkCB->GetDesc(&kShaderBufferDesc);
                        kDesc.m_aaakConstantBuffers[uiPass]
                            [NiGPUProgram::PROGRAM_PIXEL][i] = 
                            kShaderBufferDesc.Name;
                    }
                }

                if (pkReflection)
                    pkReflection->Release();
            }
        }

        if (kDesc.m_uiVSVersion < uiVSVersion)
            kDesc.m_uiVSVersion = uiVSVersion;
        if (kDesc.m_uiGSVersion < uiGSVersion)
            kDesc.m_uiGSVersion = uiGSVersion;
        if (kDesc.m_uiPSVersion < uiPSVersion)
            kDesc.m_uiPSVersion = uiPSVersion;
    }
}
//---------------------------------------------------------------------------
void NiD3D10EffectParser::ResetIndices()
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
void NiD3D10EffectParser::ParseMatrixAnnotations(
    ID3D10EffectVariable* pkVariable, 
    const D3D10_EFFECT_VARIABLE_DESC& kVariableDesc,
    const D3D10_EFFECT_TYPE_DESC&,
    NiD3D10EffectParameterMatrixDesc* pkDesc)
{
    NIASSERT(pkVariable && pkVariable->IsValid());

    // Query annotations
    for (NiUInt32 i = 0; i < kVariableDesc.Annotations; i++)
    {
        ID3D10EffectVariable* pkAnnot = pkVariable->GetAnnotationByIndex(i);
        D3D10_EFFECT_VARIABLE_DESC kAnnotDesc;
        HRESULT hr = pkAnnot->GetDesc(&kAnnotDesc);
        if (FAILED(hr))
            continue;

        if (NiStricmp(kAnnotDesc.Name, "desc") == 0 ||
            NiStricmp(kAnnotDesc.Name, "description") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uihelp") == 0)
        {
            const char* pcString = NULL;
            hr = pkAnnot->AsString()->GetString(&pcString);
            if (SUCCEEDED(hr) && pcString)
                pkDesc->m_kDesc = pcString;
        }
        else if (NiStricmp(kAnnotDesc.Name, "hidden") == 0)
        {
            BOOL bBool;
            hr = pkAnnot->AsScalar()->GetBool(&bBool);
            if (SUCCEEDED(hr))
                pkDesc->m_bHidden = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.Name, "uiwidget") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uitype") == 0)
        {
            const char* pcString = NULL;
            hr = pkAnnot->AsString()->GetString(&pcString);
            if (SUCCEEDED(hr) && pcString && NiStricmp(pcString, "none") == 0)
                pkDesc->m_bHidden = true;
        }
    }
}
//---------------------------------------------------------------------------
void NiD3D10EffectParser::ParseVectorAnnotations(
    ID3D10EffectVariable* pkVariable, 
    const D3D10_EFFECT_VARIABLE_DESC& kVariableDesc,
    const D3D10_EFFECT_TYPE_DESC&,
    NiD3D10EffectParameterVectorDesc* pkDesc)
{
    NIASSERT(pkVariable && pkVariable->IsValid());

    // Query annotations
    for (NiUInt32 i = 0; i < kVariableDesc.Annotations; i++)
    {
        ID3D10EffectVariable* pkAnnot = pkVariable->GetAnnotationByIndex(i);
        D3D10_EFFECT_VARIABLE_DESC kAnnotDesc;
        HRESULT hr = pkAnnot->GetDesc(&kAnnotDesc);
        if (FAILED(hr))
            continue;

        if (NiStricmp(kAnnotDesc.Name, "desc") == 0 ||
            NiStricmp(kAnnotDesc.Name, "description") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uihelp") == 0)
        {
            const char* pcString = NULL;
            hr = pkAnnot->AsString()->GetString(&pcString);
            if (SUCCEEDED(hr) && pcString)
                pkDesc->m_kDesc = pcString;
        }
        else if (NiStricmp(kAnnotDesc.Name, "hidden") == 0)
        {
            BOOL bBool;
            hr = pkAnnot->AsScalar()->GetBool(&bBool);
            if (SUCCEEDED(hr))
                pkDesc->m_bHidden = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.Name, "uiwidget") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uitype") == 0)
        {
            const char* pcString = NULL;
            hr = pkAnnot->AsString()->GetString(&pcString);
            if (SUCCEEDED(hr) && pcString)
            {

                if ((NiStricmp(pcString, "color") == 0 ||
                    NiStricmp(pcString, "colorswatch") == 0))
                {
                    pkDesc->m_bColor = true;
                }
                else if (NiStricmp(pcString, "none") == 0)
                {
                    pkDesc->m_bHidden = true;
                }
            }
        }
        else if (NiStricmp(kAnnotDesc.Name, "color") == 0)
        {
            BOOL bBool;
            hr = pkAnnot->AsScalar()->GetBool(&bBool);
            if (SUCCEEDED(hr))
                pkDesc->m_bColor = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.Name, "min") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uimin") == 0)
        {
            hr = pkAnnot->AsVector()->GetFloatVector(pkDesc->m_afMinValue);
        }
        else if (NiStricmp(kAnnotDesc.Name, "max") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uimax") == 0)
        {
            hr = pkAnnot->AsVector()->GetFloatVector(pkDesc->m_afMaxValue);
        }
    }
}
//---------------------------------------------------------------------------
void NiD3D10EffectParser::ParseArrayAnnotations(
    ID3D10EffectVariable* pkVariable, 
    const D3D10_EFFECT_VARIABLE_DESC& kVariableDesc,
    const D3D10_EFFECT_TYPE_DESC& kTypeDesc,
    NiD3D10EffectParameterArrayDesc* pkDesc)
{
    NIASSERT(pkVariable && pkVariable->IsValid());

    // Query annotations
    for (NiUInt32 i = 0; i < kVariableDesc.Annotations; i++)
    {
        ID3D10EffectVariable* pkAnnot = pkVariable->GetAnnotationByIndex(i);
        D3D10_EFFECT_VARIABLE_DESC kAnnotDesc;
        HRESULT hr = pkAnnot->GetDesc(&kAnnotDesc);
        if (FAILED(hr))
            continue;

        if (NiStricmp(kAnnotDesc.Name, "desc") == 0 ||
            NiStricmp(kAnnotDesc.Name, "description") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uihelp") == 0)
        {
            const char* pcString = NULL;
            hr = pkAnnot->AsString()->GetString(&pcString);
            if (SUCCEEDED(hr) && pcString)
                pkDesc->m_kDesc = pcString;
        }
        else if (NiStricmp(kAnnotDesc.Name, "hidden") == 0)
        {
            BOOL bBool;
            hr = pkAnnot->AsScalar()->GetBool(&bBool);
            if (SUCCEEDED(hr))
                pkDesc->m_bHidden = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.Name, "uiwidget") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uitype") == 0)
        {
            const char* pcString = NULL;
            hr = pkAnnot->AsString()->GetString(&pcString);
            if (SUCCEEDED(hr) && pcString)
            {
                if ((NiStricmp(pcString, "color") == 0 ||
                    NiStricmp(pcString, "colorswatch") == 0))
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
                else if (NiStricmp(pcString, "none") == 0)
                {
                    pkDesc->m_bHidden = true;
                }
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
            BOOL bBool;
            hr = pkAnnot->AsScalar()->GetBool(&bBool);
            if (SUCCEEDED(hr))
                pkDesc->m_bColor = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.Name, "min") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uimin") == 0)
        {
            HRESULT hr = 0;
            if (kTypeDesc.Columns != pkDesc->m_uiNumCols ||
                kTypeDesc.Rows != pkDesc->m_uiNumRows ||
                kTypeDesc.Elements != pkDesc->m_uiNumElements)
            {
                NiOutputDebugString("Array annotation \"min\" does not match"
                    " parent in either number of columns, rows, or elements");
                continue;
            }

            bool bCompactTempArray = false;
            if (kTypeDesc.Class == D3D10_SVC_SCALAR)
            {
                hr = pkAnnot->AsScalar()->GetFloatArray(
                    pkDesc->m_afMinValue, 
                    0, 
                    pkDesc->m_uiNumElements);
            }
            else if (kTypeDesc.Class == D3D10_SVC_VECTOR)
            {
                float* pfDest = NULL;
                if (kTypeDesc.Rows == 1 && kTypeDesc.Columns == 4)
                {
                    // Vector 4's are the same size as the 
                    // array, so no need to waste time on a temp 
                    // array.
                    pfDest = pkDesc->m_afMinValue;
                }
                else
                {
                    // Read into the temporary buffer
                    // because D3D10 forces the initial values to be
                    // D3DXVECTOR4's when they will often be smaller.
                    // The temp array will hold the values as D3DXVECTOR4's
                    // and we will compact the values in a later method.
                    pfDest = pkDesc->m_afTempValues;
                    bCompactTempArray = true;
                }
                hr = pkAnnot->AsVector()->GetFloatVectorArray(
                    pfDest, 
                    0, 
                    pkDesc->m_uiNumElements);
            }
                    
            if (FAILED(hr))
            {
                for (NiUInt32 ui = 0; 
                    ui < NiD3D10EffectParameterArrayDesc::MAX_ENTRIES; ui++)
                {
                    pkDesc->m_afMinValue[ui] = 0.0f;
                }
            }
            else if (bCompactTempArray)
            {
                NIVERIFY(pkDesc->CompactFromVector4Array(
                    (const D3DXVECTOR4*)pkDesc->m_afTempValues, 
                    pkDesc->m_afMinValue,
                    pkDesc->m_uiNumRows, pkDesc->m_uiNumCols,
                    pkDesc->m_uiNumElements));
            }
                
        }
        else if (NiStricmp(kAnnotDesc.Name, "max") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uimax") == 0)
        {
            HRESULT hr = 0;
            if (kTypeDesc.Columns != pkDesc->m_uiNumCols ||
                kTypeDesc.Rows != pkDesc->m_uiNumRows ||
                kTypeDesc.Elements != pkDesc->m_uiNumElements)
            {
                NiOutputDebugString("Array annotation \"max\" does not match"
                    " parent in either number of columns, rows, or elements");
                continue;
            }

            bool bCompactTempArray = false;
            if (kTypeDesc.Class == D3D10_SVC_SCALAR)
            {
                hr = pkAnnot->AsScalar()->GetFloatArray(
                    pkDesc->m_afMaxValue, 
                    0, 
                    pkDesc->m_uiNumElements);
            }
            else if (kTypeDesc.Class == D3D10_SVC_VECTOR)
            {
                float* pfDest = NULL;
                if (kTypeDesc.Rows == 1 && kTypeDesc.Columns == 4)
                {
                    // Vector 4's are the same size as the 
                    // array, so no need to waste time on a temp 
                    // array.
                    pfDest = pkDesc->m_afMaxValue;
                }
                else
                {
                    // Read into the temporary buffer
                    // because D3D10 forces the initial values to be
                    // D3DXVECTOR4's when they will often be smaller.
                    // The temp array will hold the values as D3DXVECTOR4's
                    // and we will compact the values in a later method.
                    pfDest = pkDesc->m_afTempValues;
                    bCompactTempArray = true;
                }
                hr = pkAnnot->AsVector()->GetFloatVectorArray(
                    pfDest, 
                    0, 
                    pkDesc->m_uiNumElements);
            }
                    
            if (FAILED(hr))
            {
                for (NiUInt32 ui = 0; ui < 
                    NiD3D10EffectParameterArrayDesc::MAX_ENTRIES; ui++)
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
                    (const D3DXVECTOR4*)pkDesc->m_afTempValues, 
                    pkDesc->m_afMaxValue,
                    pkDesc->m_uiNumRows, pkDesc->m_uiNumCols,
                    pkDesc->m_uiNumElements);
                NIASSERT(bCompacted);
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiD3D10EffectParser::ParseFloatAnnotations(
    ID3D10EffectVariable* pkVariable, 
    const D3D10_EFFECT_VARIABLE_DESC& kVariableDesc,
    const D3D10_EFFECT_TYPE_DESC&,
    NiD3D10EffectParameterFloatDesc* pkDesc)
{
    NIASSERT(pkVariable && pkVariable->IsValid());

    // Query annotations
    for (NiUInt32 i = 0; i < kVariableDesc.Annotations; i++)
    {
        ID3D10EffectVariable* pkAnnot = pkVariable->GetAnnotationByIndex(i);
        D3D10_EFFECT_VARIABLE_DESC kAnnotDesc;
        HRESULT hr = pkAnnot->GetDesc(&kAnnotDesc);
        if (FAILED(hr))
            continue;

        if (NiStricmp(kAnnotDesc.Name, "desc") == 0 ||
            NiStricmp(kAnnotDesc.Name, "description") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uihelp") == 0)
        {
            const char* pcString = NULL;
            hr = pkAnnot->AsString()->GetString(&pcString);
            if (SUCCEEDED(hr) && pcString)
                pkDesc->m_kDesc = pcString;
        }
        else if (NiStricmp(kAnnotDesc.Name, "hidden") == 0)
        {
            BOOL bBool;
            hr = pkAnnot->AsScalar()->GetBool(&bBool);
            if (SUCCEEDED(hr))
                pkDesc->m_bHidden = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.Name, "uiwidget") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uitype") == 0)
        {
            const char* pcString = NULL;
            hr = pkAnnot->AsString()->GetString(&pcString);
            if (SUCCEEDED(hr) && pcString)
            {
                if (NiStricmp(pcString, "none") == 0)
                    pkDesc->m_bHidden = true;
            }
        }
        else if (NiStricmp(kAnnotDesc.Name, "min") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uimin") == 0)
        {
            hr = pkAnnot->AsScalar()->GetFloat(&pkDesc->m_fMinValue);
        }
        else if (NiStricmp(kAnnotDesc.Name, "max") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uimax") == 0)
        {
            hr = pkAnnot->AsScalar()->GetFloat(&pkDesc->m_fMaxValue);
        }
    }
}
//---------------------------------------------------------------------------
void NiD3D10EffectParser::ParseBooleanAnnotations(
    ID3D10EffectVariable* pkVariable, 
    const D3D10_EFFECT_VARIABLE_DESC& kVariableDesc,
    const D3D10_EFFECT_TYPE_DESC&,
    NiD3D10EffectParameterBooleanDesc* pkDesc)
{
    NIASSERT(pkVariable && pkVariable->IsValid());

    // Query annotations
    for (NiUInt32 i = 0; i < kVariableDesc.Annotations; i++)
    {
        ID3D10EffectVariable* pkAnnot = pkVariable->GetAnnotationByIndex(i);
        D3D10_EFFECT_VARIABLE_DESC kAnnotDesc;
        HRESULT hr = pkAnnot->GetDesc(&kAnnotDesc);
        if (FAILED(hr))
            continue;

        if (NiStricmp(kAnnotDesc.Name, "desc") == 0 ||
            NiStricmp(kAnnotDesc.Name, "description") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uihelp") == 0)
        {
            const char* pcString = NULL;
            hr = pkAnnot->AsString()->GetString(&pcString);
            if (SUCCEEDED(hr) && pcString)
                pkDesc->m_kDesc = pcString;
        }
        else if (NiStricmp(kAnnotDesc.Name, "hidden") == 0)
        {
            BOOL bBool;
            hr = pkAnnot->AsScalar()->GetBool(&bBool);
            if (SUCCEEDED(hr))
                pkDesc->m_bHidden = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.Name, "uiwidget") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uitype") == 0)
        {
            const char* pcString = NULL;
            hr = pkAnnot->AsString()->GetString(&pcString);
            if (SUCCEEDED(hr) && pcString)
            {
                if (NiStricmp(pcString, "none") == 0)
                    pkDesc->m_bHidden = true;
            }
        }
    }
}
//---------------------------------------------------------------------------
void NiD3D10EffectParser::ParseIntegerAnnotations(
    ID3D10EffectVariable* pkVariable, 
    const D3D10_EFFECT_VARIABLE_DESC& kVariableDesc,
    const D3D10_EFFECT_TYPE_DESC&,
    NiD3D10EffectParameterIntegerDesc* pkDesc)
{
    NIASSERT(pkVariable && pkVariable->IsValid());

    // Query annotations
    for (NiUInt32 i = 0; i < kVariableDesc.Annotations; i++)
    {
        ID3D10EffectVariable* pkAnnot = pkVariable->GetAnnotationByIndex(i);
        D3D10_EFFECT_VARIABLE_DESC kAnnotDesc;
        HRESULT hr = pkAnnot->GetDesc(&kAnnotDesc);
        if (FAILED(hr))
            continue;

        if (NiStricmp(kAnnotDesc.Name, "desc") == 0 ||
            NiStricmp(kAnnotDesc.Name, "description") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uihelp") == 0)
        {
            const char* pcString = NULL;
            hr = pkAnnot->AsString()->GetString(&pcString);
            if (SUCCEEDED(hr) && pcString)
                pkDesc->m_kDesc = pcString;
        }
        else if (NiStricmp(kAnnotDesc.Name, "hidden") == 0)
        {
            BOOL bBool;
            hr = pkAnnot->AsScalar()->GetBool(&bBool);
            if (SUCCEEDED(hr))
                pkDesc->m_bHidden = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.Name, "uiwidget") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uitype") == 0)
        {
            const char* pcString = NULL;
            hr = pkAnnot->AsString()->GetString(&pcString);
            if (SUCCEEDED(hr) && pcString)
            {
                if (NiStricmp(pcString, "none") == 0)
                    pkDesc->m_bHidden = true;
            }
        }
        else if (NiStricmp(kAnnotDesc.Name, "min") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uimin") == 0)
        {
            int iInt;
            hr = pkAnnot->AsScalar()->GetInt(&iInt);
            if (SUCCEEDED(hr))
                pkDesc->m_uiMinValue = (NiUInt32)iInt;
        }
        else if (NiStricmp(kAnnotDesc.Name, "max") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uimax") == 0)
        {
            int iInt;
            hr = pkAnnot->AsScalar()->GetInt(&iInt);
            if (SUCCEEDED(hr))
                pkDesc->m_uiMaxValue = (NiUInt32)iInt;
        }
    }
}
//---------------------------------------------------------------------------
void NiD3D10EffectParser::ParseTextureAnnotations(
    ID3D10EffectVariable* pkVariable, 
    const D3D10_EFFECT_VARIABLE_DESC& kVariableDesc,
    const D3D10_EFFECT_TYPE_DESC&,
    NiD3D10EffectParameterTextureDesc* pkDesc)
{
    NIASSERT(pkVariable && pkVariable->IsValid());

    pkDesc->m_uiTextureIndex = 0xFFFFFFFF;
    pkDesc->m_uiObjectType = NiShaderAttributeDesc::OT_UNDEFINED;

    // Query annotations
    const char* pcObjectType = NULL;
    NiUInt32 uiObjectIndex = 0;
    bool bFoundObjectIndex = false;
    for (NiUInt32 i = 0; i < kVariableDesc.Annotations; i++)
    {
        ID3D10EffectVariable* pkAnnot = pkVariable->GetAnnotationByIndex(i);
        D3D10_EFFECT_VARIABLE_DESC kAnnotDesc;
        HRESULT hr = pkAnnot->GetDesc(&kAnnotDesc);
        if (FAILED(hr))
            continue;

        if (NiStricmp(kAnnotDesc.Name, "desc") == 0 ||
            NiStricmp(kAnnotDesc.Name, "description") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uihelp") == 0)
        {
            const char* pcString = NULL;
            hr = pkAnnot->AsString()->GetString(&pcString);
            if (SUCCEEDED(hr) && pcString)
                pkDesc->m_kDesc = pcString;
        }
        else if (NiStricmp(kAnnotDesc.Name, "hidden") == 0)
        {
            BOOL bBool;
            hr = pkAnnot->AsScalar()->GetBool(&bBool);
            if (SUCCEEDED(hr))
                pkDesc->m_bHidden = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.Name, "uiwidget") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uitype") == 0)
        {
            const char* pcString = NULL;
            hr = pkAnnot->AsString()->GetString(&pcString);
            if (SUCCEEDED(hr) && pcString)
            {
                if (NiStricmp(pcString, "none") == 0)
                    pkDesc->m_bHidden = true;
            }
        }
        else if (NiStricmp(kAnnotDesc.Name, "name") == 0)
        {
            const char* pcString = NULL;
            hr = pkAnnot->AsString()->GetString(&pcString);
            if (SUCCEEDED(hr) && pcString)
                pkDesc->m_kTextureName = pcString;
        }
        else if (NiStricmp(kAnnotDesc.Name, "NTM") == 0)
        {
            const char* pcString = NULL;
            hr = pkAnnot->AsString()->GetString(&pcString);
            if (SUCCEEDED(hr) && pcString)
                pkDesc->m_kTextureMap = pcString;
        }
        else if (NiStricmp(kAnnotDesc.Name, "NTMIndex") == 0)
        {
            int iInt;
            hr = pkAnnot->AsScalar()->GetInt(&iInt);
            if (SUCCEEDED(hr))
                pkDesc->m_uiTextureIndex = (NiUInt32)iInt;
        }
        else if (NiStricmp(kAnnotDesc.Name, "Object") == 0)
        {
            hr = pkAnnot->AsString()->GetString(&pcObjectType);
        }
        else if (NiStricmp(kAnnotDesc.Name, "ObjectIndex") == 0)
        {
            int iInt;
            hr = pkAnnot->AsScalar()->GetInt(&iInt);
            if (SUCCEEDED(hr))
            {
                uiObjectIndex = (NiUInt32) iInt;
                bFoundObjectIndex = true;
            }
        }
    }

    if (pcObjectType)
    {
        pkDesc->m_uiObjectType = DetermineObjectType(pcObjectType,
            uiObjectIndex, bFoundObjectIndex);
        pkDesc->m_uiObjectIndex = uiObjectIndex;
        pkDesc->m_kTextureMap = NULL;
    }

    // First try and locate the texture if it is given by name
    if (pkDesc->m_kTextureName.Exists())
    {
        // Only look in current directory for now
        HANDLE hFile = CreateFile(pkDesc->m_kTextureName, GENERIC_READ, 
            FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
        if (hFile == INVALID_HANDLE_VALUE) 
            pkDesc->m_kTextureName = NULL;
        else
            CloseHandle(hFile);
    }

    // Indicate shader map if no other clues are given
    if (!pkDesc->m_kTextureName.Exists() && pcObjectType == NULL)
    {
        if (!pkDesc->m_kTextureMap.Exists())
            pkDesc->m_kTextureMap = "shader";

        if (pkDesc->m_kTextureMap.Equals("shader") &&
            pkDesc->m_uiTextureIndex == 0xFFFFFFFF)
        {
            pkDesc->m_uiTextureIndex = ms_uiShaderMapIndex++;
        }
    }
}
//---------------------------------------------------------------------------
void NiD3D10EffectParser::ParsePredefinedAnnotations(
    ID3D10EffectVariable* pkVariable, 
    const D3D10_EFFECT_VARIABLE_DESC& kVariableDesc,
    const D3D10_EFFECT_TYPE_DESC&,
    NiD3D10EffectParameterPredefinedDesc* pkDesc)
{
    NIASSERT(pkVariable && pkVariable->IsValid());

    // Query annotations
    for (NiUInt32 i = 0; i < kVariableDesc.Annotations; i++)
    {
        ID3D10EffectVariable* pkAnnot = pkVariable->GetAnnotationByIndex(i);
        D3D10_EFFECT_VARIABLE_DESC kAnnotDesc;
        HRESULT hr = pkAnnot->GetDesc(&kAnnotDesc);
        if (FAILED(hr))
            continue;

        if (NiStricmp(kAnnotDesc.Name, "NTMIndex") == 0)
        {
            int iInt;
            hr = pkAnnot->AsScalar()->GetInt(&iInt);
            if (SUCCEEDED(hr))
                pkDesc->m_uiExtraData = (NiUInt32)iInt;
        }
    }
}
//---------------------------------------------------------------------------
void NiD3D10EffectParser::ParseTechniqueAnnotations(
    ID3D10EffectTechnique* pkTechnique, 
    const D3D10_TECHNIQUE_DESC& kTechDesc,
    NiD3D10EffectTechniqueDesc* pkDesc)
{
    NIASSERT(pkTechnique && pkTechnique->IsValid());

    const char* pcNBT = NULL;

    // Get name, description, implentation, and user version annotations
    for (NiUInt32 i = 0; i < kTechDesc.Annotations; i++)
    {
        ID3D10EffectVariable* pkAnnot = pkTechnique->GetAnnotationByIndex(i);
        D3D10_EFFECT_VARIABLE_DESC kAnnotDesc;
        HRESULT hr = pkAnnot->GetDesc(&kAnnotDesc);
        if (FAILED(hr))
            continue;

        if (NiStricmp(kAnnotDesc.Name, "desc") == 0 ||
            NiStricmp(kAnnotDesc.Name, "description") == 0 ||
            NiStricmp(kAnnotDesc.Name, "uihelp") == 0)
        {
            const char* pcString = NULL;
            hr = pkAnnot->AsString()->GetString(&pcString);
            if (SUCCEEDED(hr) && pcString)
                pkDesc->m_kDesc = pcString;
        }
        else if (NiStricmp(kAnnotDesc.Name, "shadername") == 0)
        {
            const char* pcString = NULL;
            hr = pkAnnot->AsString()->GetString(&pcString);
            if (SUCCEEDED(hr) && pcString)
                pkDesc->m_kShaderName = pcString;
        }
        else if (NiStricmp(kAnnotDesc.Name, "implementation") == 0)
        {
            int iInt;
            hr = pkAnnot->AsScalar()->GetInt(&iInt);
            if (SUCCEEDED(hr))
                pkDesc->m_uiImplementation = (NiUInt32)iInt;
        }
        else if (NiStricmp(kAnnotDesc.Name, "userversion") == 0)
        {
            int iInt;
            hr = pkAnnot->AsScalar()->GetInt(&iInt);
            if (SUCCEEDED(hr))
                pkDesc->m_uiUserVersion = (NiUInt32)iInt;
        }
        else if (NiStricmp(kAnnotDesc.Name, "bonesperpartition") == 0)
        {
            int iInt;
            hr = pkAnnot->AsScalar()->GetInt(&iInt);
            if (SUCCEEDED(hr))
                pkDesc->m_uiBonesPerPartition = (NiUInt32)iInt;
        }
        else if (NiStricmp(kAnnotDesc.Name, "nbtmethod") == 0)
        {
            hr = pkAnnot->AsString()->GetString(&pcNBT);
        }
        else if (NiStricmp(kAnnotDesc.Name, "nbtsourceuv") == 0)
        {
            if (pkAnnot->AsString()->IsValid())
            {
                const char* pcString = NULL;
                hr = pkAnnot->AsString()->GetString(&pcString);
                if (SUCCEEDED(hr) && pcString && 
                    NiStricmp(pcString, "nbtuvdefault") == 0)
                {
                    pkDesc->m_uiNBTSourceUV = 
                        NiShaderDesc::BINORMALTANGENTUVSOURCEDEFAULT;
                }
            }
            else
            {
                int iInt;
                hr = pkAnnot->AsScalar()->GetInt(&iInt);
                if (SUCCEEDED(hr))
                    pkDesc->m_uiNBTSourceUV = (NiUInt32)iInt;
            }
        }
        else if (NiStricmp(kAnnotDesc.Name, "usesnirenderstate") == 0)
        {
            BOOL bBool;
            hr = pkAnnot->AsScalar()->GetBool(&bBool);
            if (SUCCEEDED(hr))
                pkDesc->m_bUsesNiRenderState = (bBool != FALSE);
        }
        else if (NiStricmp(kAnnotDesc.Name, "semanticadaptertable") == 0)
        {
            if (pkAnnot->AsString()->IsValid())
            {
                // parse the SemanticAdapterTable annotation to build a real
                // table
                const char* pcSemanticString = NULL;
                HRESULT eRes =
                    pkAnnot->AsString()->GetString(&pcSemanticString);
                if (SUCCEEDED(eRes) && pcSemanticString)
                {
                    bool bSuccess = NiSemanticAdapterTableParser::ParseString(
                        pcSemanticString, pkDesc->m_kAdapterTable);

                    if (!bSuccess)
                    {
                        NiShaderFactory::ReportError(NISHADERERR_UNKNOWN,
                            true,
                            "NiD3D10EffectParser Warning: Invalid "
                            "SemanticAdapterTable annotation specified on the"
                            "\"%s\" technique",
                            kTechDesc.Name);
                    }
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
            NiD3D10ShaderFactory::ReportError(NISHADERERR_UNKNOWN, true,
                "NiD3D10EffectParser Warning: deprecated \"MAX\" value "
                "assigned to the \"NBTMethod\" annotation specified on "
                "\"%s\" technique", kTechDesc.Name);
            pkDesc->m_eNBTMethod = NiShaderRequirementDesc::NBT_METHOD_NDL;
        }
        else if (NiStricmp(pcNBT, "ATI") == 0)
        {
            pkDesc->m_eNBTMethod = NiShaderRequirementDesc::NBT_METHOD_ATI;
        }
    }
}
//---------------------------------------------------------------------------
NiD3D10EffectParameterDesc* NiD3D10EffectParser::QueryPredefined(
    const char* pcSemantic, bool, NiUInt32 uiArrayElements)
{
    NIASSERT(pcSemantic != NULL);

    // First check the unmodified version
    NiUInt32 uiMapping = 0;
    NiUInt32 uiNumRegisters = 0;
    bool bSuccess = NiShaderConstantMap::LookUpPredefinedMapping(pcSemantic,
        uiMapping, uiNumRegisters);

    NiD3D10EffectParameterPredefinedDesc* pkDesc = NULL;

    char* pcNewSemantic = NULL;

    if (!bSuccess)
    {
        // Create new copy of semantic name for mangling
        size_t stLen = strlen(pcSemantic);
        char* pcLowerSemantic = NiAlloc(char, stLen + 1);
        NiUInt32 i = 0;
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
        NiUInt32 uiNewLen = (NiUInt32)(stLen + 1 +
            (bInverse ? 3 : 0) + (bTranspose ? 9 : 0));
        pcNewSemantic = NiAlloc(char, uiNewLen);
        NiSprintf(pcNewSemantic, uiNewLen, "%s%s%s",
            (bInverse ? "Inv" : ""),
            pcLowerSemantic,
            (bTranspose ? "Transpose" : ""));
        NiFree(pcLowerSemantic);

        bSuccess = 
            NiShaderConstantMap::LookUpPredefinedMapping(pcNewSemantic, 
            uiMapping, uiNumRegisters);

    }

    if (bSuccess)
    {
        // Semantic corresponds to a Gamebryo constant map value

        pkDesc = NiD3D10EffectParameterDesc::GetPredefinedDesc();
        pkDesc->m_kPredefined = 
            pcNewSemantic != NULL ? pcNewSemantic : pcSemantic;
        pkDesc->m_uiMapping = uiMapping;
        // Handle bone matrices
        if (uiMapping >= NiShaderConstantMap::SCM_DEF_BONE_MATRIX_3 &&
            uiMapping <= NiShaderConstantMap::SCM_DEF_SKINBONE_MATRIX_4)
        {
            pkDesc->m_uiArrayCount = uiArrayElements;
        }
        else
        {
            pkDesc->m_uiArrayCount = 1;
        }

    }

    NiFree(pcNewSemantic);

    return pkDesc;
}
//---------------------------------------------------------------------------
NiD3D10EffectParameterDesc* NiD3D10EffectParser::QueryObject(
    ID3D10EffectVariable* pkVariable,
    const D3D10_EFFECT_VARIABLE_DESC& kVariableDesc,
    const char* pcSemantic)
{
    NIASSERT(pkVariable && pkVariable->IsValid());

    // This used to assert on pcSemantic as well. However, the data previously
    // in the semantic may now be in the ObjectProperty annotation so we'll
    // try to grab that annotation before we handle the object.

    // First, look for the required "Object" annotation.
    const char* pcObjectValue = LookupObjectAnnotation(pkVariable,
        kVariableDesc, "Object");
    if (pcObjectValue == NULL)
    {
        // This parameter is not an object.
        return NULL;
    }

    const char* pcObjProp = NULL;
    ID3D10EffectVariable* pkObjectPropertyAnnot = 
        pkVariable->GetAnnotationByName("ObjectProperty");
    if (pkObjectPropertyAnnot->IsValid())
    {
        HRESULT hr = pkObjectPropertyAnnot->AsString()->GetString(&pcObjProp);
        if (SUCCEEDED(hr) && pcObjProp)
        {
            // Replace pcSemantic
            pcSemantic = pcObjProp;
        }
    }

    // We have to have the semantic value of the object now either from
    // the actual FX semantic or from the annotation.
    if (pcSemantic == NULL)
        return NULL;

    // Get the specified object index, if any.
    NiUInt32 uiObjectIndex = 0;
    bool bFoundObjectIndex = false;
    for (NiUInt32 i = 0; i < kVariableDesc.Annotations; i++)
    {
        ID3D10EffectVariable* pkAnnot = pkVariable->GetAnnotationByIndex(i);
        D3D10_EFFECT_VARIABLE_DESC kAnnotDesc;
        HRESULT hr = pkAnnot->GetDesc(&kAnnotDesc);
        if (FAILED(hr))
            continue;

        if (NiStricmp(kAnnotDesc.Name, "ObjectIndex") == 0)
        {
            int iInt;
            hr = pkAnnot->AsScalar()->GetInt(&iInt);
            if (SUCCEEDED(hr))
            {
                uiObjectIndex = iInt;
                bFoundObjectIndex = true;
                break;
            }
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
    NiUInt32 uiMapping;
    if (!NiShaderConstantMap::LookUpObjectMapping(pcSemantic, uiMapping))
    {
        return NULL;
    }

    // Verify that the mapping is appropriate for the object type. Also,
    // read any supported additional annotations for the semantic, adjusting
    // the object mapping as necessary. uiMapping may be changed by this
    // function. This function returns false if the mapping is not
    // appropriate for the object type.
    if (!VerifyAndAdjustMapping(
        pkVariable, 
        kVariableDesc, 
        eObjectType, 
        uiMapping))
    {
        return NULL;
    }

    // Create the parameter desc with the appropriate object type and mapping.
    NiD3D10EffectParameterObjectDesc* pkDesc =
        NiD3D10EffectParameterDesc::GetObjectDesc();
    pkDesc->m_uiMapping = uiMapping;
    pkDesc->m_eObjectType = eObjectType;
    pkDesc->m_uiObjectIndex = uiObjectIndex;

    return pkDesc;
}
//---------------------------------------------------------------------------
const char* NiD3D10EffectParser::LookupObjectAnnotation(
    ID3D10EffectVariable* pkVariable,
    const D3D10_EFFECT_VARIABLE_DESC& kVariableDesc,
    const char* pcAnnotation)
{
    NIASSERT(pkVariable && pkVariable->IsValid() && pcAnnotation);

    const char* pcValue = NULL;
    for (NiUInt32 i = 0; i < kVariableDesc.Annotations; i++)
    {
        ID3D10EffectVariable* pkAnnot = pkVariable->GetAnnotationByIndex(i);
        D3D10_EFFECT_VARIABLE_DESC kAnnotDesc;
        HRESULT hr = pkAnnot->GetDesc(&kAnnotDesc);
        if (FAILED(hr))
            continue;

        if (NiStricmp(kAnnotDesc.Name, pcAnnotation) == 0)
        {
            hr = pkAnnot->AsString()->GetString(&pcValue);
            if (SUCCEEDED(hr))
                break;
        }
        pcValue = NULL;
    }

    return pcValue;
}
//---------------------------------------------------------------------------
NiUInt32 NiD3D10EffectParser::DetermineObjectType(
    const char* pcObjectValue, NiUInt32& uiObjectIndex,
    bool bFoundObjectIndex)
{
    NiUInt32 uiObjectType = NiShaderAttributeDesc::OT_UNDEFINED;
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
bool NiD3D10EffectParser::VerifyAndAdjustMapping(
    ID3D10EffectVariable* pkVariable,
    const D3D10_EFFECT_VARIABLE_DESC& kVariableDesc, 
    NiUInt32 uiObjectType,
    NiUInt32& uiMapping)
{
    NIASSERT(pkVariable && pkVariable->IsValid());

    // This function reads any supported additional annotations for the input
    // mapping, adjusting the mapping as necessary. uiMapping may be changed
    // by this function. If the mapping is not appropriate for the input
    // object type, this function returns false.

    if (!NiShaderConstantMap::IsObjectMappingValidForType(
        (NiShaderConstantMap::ObjectMappings) uiMapping,
        (NiShaderAttributeDesc::ObjectType) uiObjectType))
    {
        NiD3D10ShaderFactory::ReportError(NISHADERERR_UNKNOWN, false,
            "NiD3D10EffectParser Error: Semantic specified for incompatible "
            "object type on \"%s\" parameter\n",
            kVariableDesc.Name);
        return false;
    }

    const char* pcAnnotation = NULL;
    switch (uiMapping)
    {
        case NiShaderConstantMap::SCM_OBJ_DIMMER:
            // No additional annotations supported.
            break;
        case NiShaderConstantMap::SCM_OBJ_AMBIENT:
            pcAnnotation = LookupObjectAnnotation(
                pkVariable, 
                kVariableDesc, 
                "Usage");
            if (pcAnnotation && NiStricmp(pcAnnotation, "Undimmed") == 0)
            {
                uiMapping = NiShaderConstantMap::SCM_OBJ_UNDIMMEDAMBIENT;
            }
            break;
        case NiShaderConstantMap::SCM_OBJ_DIFFUSE:
            pcAnnotation = LookupObjectAnnotation(
                pkVariable, 
                kVariableDesc, 
                "Usage");
            if (pcAnnotation && NiStricmp(pcAnnotation, "Undimmed") == 0)
            {
                uiMapping = NiShaderConstantMap::SCM_OBJ_UNDIMMEDDIFFUSE;
            }
            break;
        case NiShaderConstantMap::SCM_OBJ_SPECULAR:
            pcAnnotation = LookupObjectAnnotation(
                pkVariable, 
                kVariableDesc, 
                "Usage");
            if (pcAnnotation && NiStricmp(pcAnnotation, "Undimmed") == 0)
            {
                uiMapping = NiShaderConstantMap::SCM_OBJ_UNDIMMEDSPECULAR;
            }
            break;
        case NiShaderConstantMap::SCM_OBJ_WORLDPOSITION:
            pcAnnotation = LookupObjectAnnotation(
                pkVariable, 
                kVariableDesc, 
                "Space");
            if (pcAnnotation && NiStricmp(pcAnnotation, "Model") == 0)
            {
                uiMapping = NiShaderConstantMap::SCM_OBJ_MODELPOSITION;
            }
            break;
        case NiShaderConstantMap::SCM_OBJ_WORLDDIRECTION:
            pcAnnotation = LookupObjectAnnotation(
                pkVariable, 
                kVariableDesc, 
                "Space");
            if (pcAnnotation && NiStricmp(pcAnnotation, "Model") == 0)
            {
                uiMapping = NiShaderConstantMap::SCM_OBJ_MODELDIRECTION;
            }
            break;
        case NiShaderConstantMap::SCM_OBJ_WORLDTRANSFORM:
            pcAnnotation = LookupObjectAnnotation(
                pkVariable, 
                kVariableDesc, 
                "Space");
            if (pcAnnotation && NiStricmp(pcAnnotation, "Model") == 0)
            {
                uiMapping = NiShaderConstantMap::SCM_OBJ_MODELTRANSFORM;
            }
            break;
        case NiShaderConstantMap::SCM_OBJ_ATTENUATION:
            pcAnnotation = LookupObjectAnnotation(
                pkVariable, 
                kVariableDesc, 
                "Attenuation");
            if (pcAnnotation && NiStricmp(pcAnnotation, "Spot") == 0)
            {
                uiMapping = NiShaderConstantMap::SCM_OBJ_SPOTATTENUATION;
            }
            break;
        case NiShaderConstantMap::SCM_OBJ_WORLDPROJECTIONMATRIX:
            pcAnnotation = LookupObjectAnnotation(
                pkVariable, 
                kVariableDesc, 
                "Space");
            if (pcAnnotation && NiStricmp(pcAnnotation, "Model") == 0)
            {
                uiMapping = NiShaderConstantMap::SCM_OBJ_MODELPROJECTIONMATRIX;
            }
            break;
        case NiShaderConstantMap::SCM_OBJ_WORLDPROJECTIONTRANSLATION:
            pcAnnotation = LookupObjectAnnotation(
                pkVariable, 
                kVariableDesc, 
                "Space");
            if (pcAnnotation && NiStricmp(pcAnnotation, "Model") == 0)
            {
                uiMapping = 
                    NiShaderConstantMap::SCM_OBJ_MODELPROJECTIONTRANSLATION;
            }
            break;
        case NiShaderConstantMap::SCM_OBJ_WORLDCLIPPINGPLANE:
            pcAnnotation = LookupObjectAnnotation(
                pkVariable, 
                kVariableDesc, 
                "Space");
            if (pcAnnotation && NiStricmp(pcAnnotation, "Model") == 0)
            {
                uiMapping = NiShaderConstantMap::SCM_OBJ_MODELCLIPPINGPLANE;
            }
            break;
        case NiShaderConstantMap::SCM_OBJ_TEXCOORDGEN:
            // No additional annotations supported.
            break;
        case NiShaderConstantMap::SCM_OBJ_WORLDPROJECTIONTRANSFORM:
            pcAnnotation = LookupObjectAnnotation(
                pkVariable, 
                kVariableDesc, 
                "Space");
            if (pcAnnotation && NiStricmp(pcAnnotation, "Model") == 0)
            {
                uiMapping =
                    NiShaderConstantMap::SCM_OBJ_MODELPROJECTIONTRANSFORM;
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
