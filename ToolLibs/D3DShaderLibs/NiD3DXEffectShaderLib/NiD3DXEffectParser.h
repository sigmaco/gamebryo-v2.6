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

#ifndef NID3DXEFFECTPARSER_H
#define NID3DXEFFECTPARSER_H

#include "NiD3DXEffectShaderLibType.h"
#include <NiMemObject.h>


class NiD3DXEffectParameterDesc;
class NiD3DXEffectParameterMatrixDesc;
class NiD3DXEffectParameterVectorDesc;
class NiD3DXEffectParameterFloatDesc;
class NiD3DXEffectParameterBooleanDesc;
class NiD3DXEffectParameterIntegerDesc;
class NiD3DXEffectParameterTextureDesc;
class NiD3DXEffectParameterPredefinedDesc;
class NiD3DXEffectTechniqueDesc;
class NiD3DXEffectParameterArrayDesc;

class NID3DXEFFECTSHADER_ENTRY NiD3DXEffectParser : public NiMemObject
{
public:
    static bool GetParametersAndTechniques(void* pkEffect, 
        unsigned int& uiNumParameters, unsigned int& uiNumTechniques);
    static NiD3DXEffectParameterDesc* GetParameter(void* pkEffect, 
        unsigned int uiIndex);
    static void GetTechnique(NiD3DXEffectTechniqueDesc& kDescriptorOut,
        void* pkEffect, 
        unsigned int uiIndex);

    static NiD3DXEffectParameterDesc* GetParameterDesc(void* pvD3DXEffect, 
        void* pvParamHandle);

    static void GetTechniqueDesc(
        NiD3DXEffectTechniqueDesc& kDescriptorOut,
        void* pvD3DXEffect, 
        void* pvParamHandle);
    
    static bool IsParameterUsed(void* pvD3DXEffect, const char* pcParamName, 
        const char* pcTechName);

    static void ResetIndices();

protected:
    static void ParseMatrixAnnotations(void* pvD3DXEffect, 
        void* pvParamHandle, void* pvParamDesc, 
        NiD3DXEffectParameterMatrixDesc* pkDesc);
    static void ParseVectorAnnotations(void* pvD3DXEffect, 
        void* pvParamHandle, void* pvParamDesc, 
        NiD3DXEffectParameterVectorDesc* pkDesc);
    static void ParseFloatAnnotations(void* pvD3DXEffect, 
        void* pvParamHandle, void* pvParamDesc, 
        NiD3DXEffectParameterFloatDesc* pkDesc);
    static void ParseBooleanAnnotations(void* pvD3DXEffect, 
        void* pvParamHandle, void* pvParamDesc, 
        NiD3DXEffectParameterBooleanDesc* pkDesc);
    static void ParseIntegerAnnotations(void* pvD3DXEffect, 
        void* pvParamHandle, void* pvParamDesc, 
        NiD3DXEffectParameterIntegerDesc* pkDesc);
    static void ParseTextureAnnotations(void* pvD3DXEffect, 
        void* pvParamHandle, void* pvParamDesc, 
        NiD3DXEffectParameterTextureDesc* pkDesc);
    static void ParsePredefinedAnnotations(void* pvD3DXEffect, 
        void* pvParamHandle, void* pvParamDesc, 
        NiD3DXEffectParameterPredefinedDesc* pkDesc);
    static void ParseTechniqueAnnotations(void* pvD3DXEffect, 
        void* pvParamHandle, void* pvParamDesc, 
        NiD3DXEffectTechniqueDesc* pkDesc);
    static void ParseArrayAnnotations(void* pvD3DXEffect, 
        void* pvParamHandle, void* pvParamDesc, 
        NiD3DXEffectParameterArrayDesc* pkDesc);
    
    static NiD3DXEffectParameterDesc* QueryPredefined(
        const char* pcSemantic, bool bColumnMatrix, 
        unsigned int uiArrayElements);

    static NiD3DXEffectParameterDesc* QueryObject(void* pvD3DXEffect,
        void* pvParamHandle, void* pvParamDesc, const char* pcSemantic);
    static const char* LookupObjectAnnotation(void* pvD3DXEffect,
        void* pvParamHandle, void* pvParamDesc, const char* pcAnnotation);
    static unsigned int DetermineObjectType(const char* pcObjectValue,
        unsigned int& uiObjectIndex, bool bFoundObjectIndex);
    static bool VerifyAndAdjustMapping(void* pvD3DXEffect,
        void* pvParamHandle, void* pvParamDesc, unsigned int uiObjectType,
        unsigned int& uiMapping);

    static unsigned int ms_uiShaderMapIndex;
    static unsigned int ms_uiGeneralLightIndex;
    static unsigned int ms_uiPointLightIndex;
    static unsigned int ms_uiDirectionalLightIndex;
    static unsigned int ms_uiSpotLightIndex;
    static unsigned int ms_uiShadowPointLightIndex;
    static unsigned int ms_uiShadowDirectionalLightIndex;
    static unsigned int ms_uiShadowSpotLightIndex;
    static unsigned int ms_uiProjShadowIndex;
    static unsigned int ms_uiProjLightIndex;
};

#endif // NID3DXEFFECTPARSER_H
