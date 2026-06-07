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

#ifndef NID3D10EFFECTPARSER_H
#define NID3D10EFFECTPARSER_H

#include "NiD3D10EffectShaderLibType.h"
#include <NiD3D10Headers.h>
#include <NiMemObject.h>
#include <NiShaderConstantMap.h>
#include <NiTPtrSet.h>

class NiD3D10EffectParameterDesc;
class NiD3D10EffectParameterMatrixDesc;
class NiD3D10EffectParameterVectorDesc;
class NiD3D10EffectParameterFloatDesc;
class NiD3D10EffectParameterBooleanDesc;
class NiD3D10EffectParameterIntegerDesc;
class NiD3D10EffectParameterTextureDesc;
class NiD3D10EffectParameterPredefinedDesc;
class NiD3D10EffectTechniqueDesc;
class NiD3D10EffectParameterArrayDesc;

class NID3D10EFFECTSHADER_ENTRY NiD3D10EffectParser : public NiMemObject
{
public:
    static NiD3D10EffectParameterDesc* GetParameter(
        ID3D10Effect* pkEffect, 
        NiUInt32 uiIndex);
    static void GetTechnique(
        NiD3D10EffectTechniqueDesc& kDescriptorOut,
        ID3D10Effect* pkEffect, 
        NiUInt32 uiIndex);

    static NiD3D10EffectParameterDesc* GetParameterDesc(
        ID3D10EffectVariable* pkVariable);

    static void GetTechniqueDesc(
        NiD3D10EffectTechniqueDesc& kDescriptorOut,
        ID3D10EffectTechnique* pkTechnique);
    
    static bool IsParameterUsed(
        ID3D10Effect* pkEffect, 
        const char* pcParamName, 
        const char* pcTechName);

    static void ResetIndices();

    // *** begin Emergent internal use only ***
    static void _SDMShutdown();
    // *** end Emergent internal use only ***

protected:
    static void ParseMatrixAnnotations(
        ID3D10EffectVariable* pkVariable, 
        const D3D10_EFFECT_VARIABLE_DESC& kVariableDesc,
        const D3D10_EFFECT_TYPE_DESC& kTypeDesc,
        NiD3D10EffectParameterMatrixDesc* pkDesc);
    static void ParseVectorAnnotations(
        ID3D10EffectVariable* pkVariable, 
        const D3D10_EFFECT_VARIABLE_DESC& kVariableDesc,
        const D3D10_EFFECT_TYPE_DESC& kTypeDesc,
        NiD3D10EffectParameterVectorDesc* pkDesc);
    static void ParseArrayAnnotations(
        ID3D10EffectVariable* pkVariable, 
        const D3D10_EFFECT_VARIABLE_DESC& kVariableDesc,
        const D3D10_EFFECT_TYPE_DESC& kTypeDesc,
        NiD3D10EffectParameterArrayDesc* pkDesc);
    static void ParseFloatAnnotations(
        ID3D10EffectVariable* pkVariable, 
        const D3D10_EFFECT_VARIABLE_DESC& kVariableDesc,
        const D3D10_EFFECT_TYPE_DESC& kTypeDesc,
        NiD3D10EffectParameterFloatDesc* pkDesc);
    static void ParseBooleanAnnotations(
        ID3D10EffectVariable* pkVariable, 
        const D3D10_EFFECT_VARIABLE_DESC& kVariableDesc,
        const D3D10_EFFECT_TYPE_DESC& kTypeDesc,
        NiD3D10EffectParameterBooleanDesc* pkDesc);
    static void ParseIntegerAnnotations(
        ID3D10EffectVariable* pkVariable, 
        const D3D10_EFFECT_VARIABLE_DESC& kVariableDesc,
        const D3D10_EFFECT_TYPE_DESC& kTypeDesc,
        NiD3D10EffectParameterIntegerDesc* pkDesc);
    static void ParseTextureAnnotations(
        ID3D10EffectVariable* pkVariable, 
        const D3D10_EFFECT_VARIABLE_DESC& kVariableDesc,
        const D3D10_EFFECT_TYPE_DESC& kTypeDesc,
        NiD3D10EffectParameterTextureDesc* pkDesc);
    static void ParsePredefinedAnnotations(
        ID3D10EffectVariable* pkVariable, 
        const D3D10_EFFECT_VARIABLE_DESC& kVariableDesc,
        const D3D10_EFFECT_TYPE_DESC& kTypeDesc,
        NiD3D10EffectParameterPredefinedDesc* pkDesc);

    static void ParseTechniqueAnnotations(
        ID3D10EffectTechnique* pkTechnique, 
        const D3D10_TECHNIQUE_DESC& pkTechDesc,
        NiD3D10EffectTechniqueDesc* pkDesc);
    
    static NiD3D10EffectParameterDesc* QueryPredefined(
        const char* pcSemantic, 
        bool bColumnMatrix, 
        NiUInt32 uiArrayElements);

    static NiD3D10EffectParameterDesc* QueryObject(
        ID3D10EffectVariable* pkVariable,
        const D3D10_EFFECT_VARIABLE_DESC& kVariableDesc,
        const char* pcSemantic);
    static const char* LookupObjectAnnotation(
        ID3D10EffectVariable* pkVariable,
        const D3D10_EFFECT_VARIABLE_DESC& kVariableDesc,
        const char* pcAnnotation);
    static NiUInt32 DetermineObjectType(
        const char* pcObjectValue,
        NiUInt32& uiObjectIndex, 
        bool bFoundObjectIndex);
    static bool VerifyAndAdjustMapping(
        ID3D10EffectVariable* pkVariable,
        const D3D10_EFFECT_VARIABLE_DESC& kVariableDesc, 
        NiUInt32 uiObjectType,
        NiUInt32& uiMapping);

    static NiUInt32 ms_uiShaderMapIndex;
    static NiUInt32 ms_uiGeneralLightIndex;
    static NiUInt32 ms_uiPointLightIndex;
    static NiUInt32 ms_uiDirectionalLightIndex;
    static NiUInt32 ms_uiSpotLightIndex;
    static NiUInt32 ms_uiShadowPointLightIndex;
    static NiUInt32 ms_uiShadowDirectionalLightIndex;
    static NiUInt32 ms_uiShadowSpotLightIndex;
    static NiUInt32 ms_uiProjShadowIndex;
    static NiUInt32 ms_uiProjLightIndex;

    static NiShaderConstantMap::PredefinedMapping* ms_pkObjectSemantics;
    static const NiUInt32 ms_uiNumObjectSemantics;

};

#endif // NID3D10EFFECTPARSER_H
