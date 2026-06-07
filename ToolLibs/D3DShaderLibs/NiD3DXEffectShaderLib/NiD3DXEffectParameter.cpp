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
#include "NiD3DXEffectParameter.h"
#include "NiD3DXEffectAttributeTable.h"
#include "NiD3DXEffectParameterDesc.h"
#include "NiD3DUtility.h"

#include <NiAmbientLight.h>
#include <NiBooleanExtraData.h>
#include <NiColorExtraData.h>
#include <NiD3DShaderFactory.h>
#include <NiFloatExtraData.h>
#include <NiFloatsExtraData.h>
#include <NiFogProperty.h>
#include <NiIntegerExtraData.h>
#include <NiMaterialProperty.h>
#include <NiMesh.h>
#include <NiShadowGenerator.h>
#include <NiShadowMap.h>
#include <NiSkinningMeshModifier.h>
#include <NiTextureEffect.h>

#if defined(WIN32)
#include <NiDX9Renderer.h>
#include <NiDX9TextureManager.h>
#elif defined(_XENON)
#include <NiXenonRenderer.h>
#include <NiXenonTextureManager.h>
#endif

D3DXVECTOR4 NiD3DXEffectParameter::ms_akVector4Array[
    NiD3DXEffectParameterArrayDesc::MAX_ROWS];

D3DXMATRIX* NiD3DXEffectParameter::ms_pkMatrixArray = NULL;
unsigned int NiD3DXEffectParameter::ms_uiMatrixArraySize = 0;
float NiD3DXEffectParameter::ms_afObjectData[16];

//---------------------------------------------------------------------------
NiD3DXEffectParameter::NiD3DXEffectParameter() :
    m_pkParam(NULL),
    m_ePredefinedMapping(NiShaderConstantMap::SCM_DEF_INVALID),
    m_eObjectMapping(NiShaderConstantMap::SCM_OBJ_INVALID),
    m_eParameterType(NiD3DXEffectParameterDesc::PT_NONE),
    m_eTextureType(NiD3DXEffectParameterTextureDesc::TT_NONE),
    m_uiParameterCount(0),
    m_uiArrayCount(0),
    m_uiExtraData(0),
    m_uiTextureFlags(NiD3DTextureStage::TSTF_IGNORE),
    m_pkTexture(NULL),
    m_bGlobal(false),
    m_pcTextureSource(NULL),
    m_pcTextureTarget(NULL),
    m_uiWidth(0),
    m_uiHeight(0),
    m_uiDepth(0),
    m_eObjectType(NiShaderAttributeDesc::OT_UNDEFINED)
{ /* */ }
//---------------------------------------------------------------------------
NiD3DXEffectParameter::~NiD3DXEffectParameter()
{
    DestroyRendererData();

    if (m_bGlobal)
        NiD3DShaderFactory::ReleaseGlobalShaderConstant(m_kName);

    NiFree(m_pcTextureSource);
    NiFree(m_pcTextureTarget);
}
//---------------------------------------------------------------------------
bool NiD3DXEffectParameter::Initialize(NiD3DXEffectParameterDesc* pkParamDesc, 
    NiD3DXEffectAttributeTable* pkAttribTable)
{
    if (pkParamDesc == NULL)
        return false;

    m_eParameterType = pkParamDesc->m_eParamType;
    m_bGlobal = pkParamDesc->m_bGlobal;

    switch (m_eParameterType)
    {
    case NiD3DXEffectParameterDesc::PT_MATRIX:
        {
            NiD3DXEffectParameterMatrixDesc* pkMatrixDesc = 
                (NiD3DXEffectParameterMatrixDesc*)pkParamDesc;

            m_kName = pkParamDesc->m_pcName;

            m_uiWidth = pkMatrixDesc->m_uiNumCols;
            m_uiHeight = pkMatrixDesc->m_uiNumRows;
            m_uiParameterCount = m_uiWidth * m_uiHeight;

            if (pkMatrixDesc->m_uiNumRows == 3 && 
                pkMatrixDesc->m_uiNumCols == 3)
            {
                if (m_bGlobal)
                {
                    NiD3DShaderFactory::RegisterGlobalShaderConstant(m_kName, 
                        NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3,
                        12 * sizeof(float), &pkMatrixDesc->m_afInitValue);
                }
                else
                {
                    // Only add non-global attributes

                    pkAttribTable->AddAttribDesc_Matrix3(m_kName, 
                        pkMatrixDesc->m_bHidden,
                        pkMatrixDesc->m_afInitValue);
                }
            }
            else
            {
                if (m_bGlobal)
                {
                    NiD3DShaderFactory::RegisterGlobalShaderConstant(m_kName, 
                        NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4,
                        16 * sizeof(float), &pkMatrixDesc->m_afInitValue);
                }
                else
                {
                    // Only add non-global attributes

                    pkAttribTable->AddAttribDesc_Matrix4(m_kName, 
                        pkMatrixDesc->m_bHidden,
                        pkMatrixDesc->m_afInitValue);
                }
            }
            return true;
        }
        break;
    case NiD3DXEffectParameterDesc::PT_VECTOR:
        {
            NiD3DXEffectParameterVectorDesc* pkVectorDesc = 
                (NiD3DXEffectParameterVectorDesc*)pkParamDesc;

            m_kName =  pkParamDesc->m_pcName;

            NiD3DXEffectParameterVectorDesc::VectorType eVectorType =
                pkVectorDesc->GetVectorType();

            m_uiParameterCount = pkVectorDesc->m_uiNumAtomicElements;

            // Check for valid range
            float* pfMin = pkVectorDesc->m_afMinValue;
            float* pfMax = pkVectorDesc->m_afMaxValue;
            bool bEqual = true;
            unsigned int i = 0;
            for (; i < pkVectorDesc->m_uiNumAtomicElements; i++)
            {
                if (pfMin[i] > pfMax[i])
                    break;
                else if (pfMin[i] < pfMax[i])
                    bEqual = false;
            }
            if (bEqual || i < pkVectorDesc->m_uiNumAtomicElements)
            {
                pfMin = NULL;
                pfMax = NULL;
            }

            if (pkVectorDesc->m_bColor)
            {
                NIASSERT(eVectorType == 
                    NiD3DXEffectParameterVectorDesc::TYPE_POINT3 || 
                    eVectorType == 
                    NiD3DXEffectParameterVectorDesc::TYPE_POINT4);

                if (m_bGlobal)
                {
                    NiD3DShaderFactory::RegisterGlobalShaderConstant(m_kName, 
                        NiShaderAttributeDesc::ATTRIB_TYPE_COLOR,
                        pkVectorDesc->m_uiNumAtomicElements * sizeof(float), 
                        &pkVectorDesc->m_afInitValue);
                }
                else
                {
                    // Only add non-global attributes

                    if (pkVectorDesc->m_uiNumAtomicElements == 3)
                    {
                        pkAttribTable->AddAttribDesc_Color(m_kName, 
                            pkVectorDesc->m_bHidden, 
                            pkVectorDesc->m_afInitValue, pfMin, pfMax);
                    }
                    else if (pkVectorDesc->m_uiNumAtomicElements == 4)
                    {
                        pkAttribTable->AddAttribDesc_ColorA(m_kName, 
                            pkVectorDesc->m_bHidden, 
                            pkVectorDesc->m_afInitValue, pfMin, pfMax);
                    }
                }
            }
            else
            {
                NIASSERT(eVectorType == 
                    NiD3DXEffectParameterVectorDesc::TYPE_POINT2 || 
                    eVectorType == 
                    NiD3DXEffectParameterVectorDesc::TYPE_POINT3 || 
                    eVectorType == 
                    NiD3DXEffectParameterVectorDesc::TYPE_POINT4);

                if (eVectorType == 
                    NiD3DXEffectParameterVectorDesc::TYPE_POINT2)
                {
                    if (m_bGlobal)
                    {
                        NiD3DShaderFactory::RegisterGlobalShaderConstant(
                            m_kName, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_POINT2, 
                            2 * sizeof(float), 
                            &pkVectorDesc->m_afInitValue);
                    }
                    else
                    {
                        // Only add non-global attributes

                        pkAttribTable->AddAttribDesc_Point2(m_kName, 
                            pkVectorDesc->m_bHidden, 
                            pkVectorDesc->m_afInitValue, pfMin, pfMax);
                    }
                }
                else if (eVectorType == 
                    NiD3DXEffectParameterVectorDesc::TYPE_POINT3)
                {
                    if (m_bGlobal)
                    {
                        NiD3DShaderFactory::RegisterGlobalShaderConstant(
                            m_kName, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_POINT3, 
                            3 * sizeof(float), 
                            &pkVectorDesc->m_afInitValue);
                    }
                    else
                    {
                        // Only add non-global attributes

                        pkAttribTable->AddAttribDesc_Point3(m_kName, 
                            pkVectorDesc->m_bHidden, 
                            pkVectorDesc->m_afInitValue, pfMin, pfMax);
                    }
                }
                else if(eVectorType == 
                    NiD3DXEffectParameterVectorDesc::TYPE_POINT4)
                {
                    if (m_bGlobal)
                    {
                        NiD3DShaderFactory::RegisterGlobalShaderConstant(
                            m_kName, 
                            NiShaderAttributeDesc::ATTRIB_TYPE_POINT4, 
                            4 * sizeof(float), 
                            &pkVectorDesc->m_afInitValue);
                    }
                    else
                    {
                        // Only add non-global attributes

                        pkAttribTable->AddAttribDesc_Point4(m_kName, 
                            pkVectorDesc->m_bHidden, 
                            pkVectorDesc->m_afInitValue, pfMin, pfMax);
                    }
                }
            }
            return true;
        }
        break;
    case NiD3DXEffectParameterDesc::PT_FLOAT:
        {
            NiD3DXEffectParameterFloatDesc* pkFloatDesc = 
                (NiD3DXEffectParameterFloatDesc*)pkParamDesc;

            m_kName = pkParamDesc->m_pcName;

            m_uiParameterCount = 1;

            if (m_bGlobal)
            {
                NiD3DShaderFactory::RegisterGlobalShaderConstant(m_kName, 
                    NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT,
                    sizeof(pkFloatDesc->m_fInitValue), 
                    &pkFloatDesc->m_fInitValue);
            }
            else
            {
                // Only add non-global attributes

                if (pkFloatDesc->m_fMinValue == pkFloatDesc->m_fMaxValue)
                {
                    pkAttribTable->AddAttribDesc_Float(m_kName, 
                        pkFloatDesc->m_bHidden, 
                        pkFloatDesc->m_fInitValue);
                }
                else
                {
                    pkAttribTable->AddAttribDesc_Float(m_kName, 
                        pkFloatDesc->m_bHidden, 
                        pkFloatDesc->m_fInitValue, pkFloatDesc->m_fMinValue, 
                        pkFloatDesc->m_fMaxValue);
                }
            }
            return true;
        }
        break;
    case NiD3DXEffectParameterDesc::PT_BOOL:
        {
            NiD3DXEffectParameterBooleanDesc* pkBooleanDesc = 
                (NiD3DXEffectParameterBooleanDesc*)pkParamDesc;

            m_kName = pkParamDesc->m_pcName;

            m_uiParameterCount = 1;

            if (m_bGlobal)
            {
                NiD3DShaderFactory::RegisterGlobalShaderConstant(m_kName, 
                    NiShaderAttributeDesc::ATTRIB_TYPE_BOOL,
                    sizeof(pkBooleanDesc->m_bInitValue), 
                    &pkBooleanDesc->m_bInitValue);
            }
            else
            {
                // Only add non-global attributes

                pkAttribTable->AddAttribDesc_Bool(m_kName, 
                    pkBooleanDesc->m_bHidden,
                    pkBooleanDesc->m_bInitValue);
            }
            return true;
        }
        break;
    case NiD3DXEffectParameterDesc::PT_UINT:
        {
            NiD3DXEffectParameterIntegerDesc* pkIntegerDesc = 
                (NiD3DXEffectParameterIntegerDesc*)pkParamDesc;

            m_kName = pkParamDesc->m_pcName;

            m_uiParameterCount = 1;

            if (m_bGlobal)
            {
                NiD3DShaderFactory::RegisterGlobalShaderConstant(m_kName, 
                    NiShaderAttributeDesc::ATTRIB_TYPE_UNSIGNEDINT,
                    sizeof(pkIntegerDesc->m_uiInitValue), 
                    &pkIntegerDesc->m_uiInitValue);
            }
            else
            {
                // Only add non-global attributes

                if (pkIntegerDesc->m_uiMinValue == 
                    pkIntegerDesc->m_uiMaxValue)
                {
                    pkAttribTable->AddAttribDesc_UnsignedInt(m_kName, 
                        pkIntegerDesc->m_bHidden, 
                        pkIntegerDesc->m_uiInitValue);
                }
                else
                {
                    pkAttribTable->AddAttribDesc_UnsignedInt(m_kName, 
                        pkIntegerDesc->m_bHidden, 
                        pkIntegerDesc->m_uiInitValue, 
                        pkIntegerDesc->m_uiMinValue, 
                        pkIntegerDesc->m_uiMaxValue);
                }
            }
            return true;
        }
        break;
    case NiD3DXEffectParameterDesc::PT_TEXTURE:
        {
            NiD3DXEffectParameterTextureDesc* pkTextureDesc = 
                (NiD3DXEffectParameterTextureDesc*)pkParamDesc;

            m_kName = pkParamDesc->m_pcName;

            m_uiParameterCount = 1;

            if (m_bGlobal)
            {
                NiD3DShaderFactory::RegisterGlobalShaderConstant(m_kName, 
                    NiShaderAttributeDesc::ATTRIB_TYPE_TEXTURE,
                    sizeof(pkTextureDesc->m_pvInitValue), 
                    pkTextureDesc->m_pvInitValue);
            }

            m_eObjectType = (NiShaderAttributeDesc::ObjectType)
                pkTextureDesc->m_uiObjectType;
            m_uiExtraData = pkTextureDesc->m_uiObjectIndex;
            if (m_eObjectType != NiShaderAttributeDesc::OT_UNDEFINED)
            {
                return true;
            }

            // Check for Gamebryo texture request
            if (pkTextureDesc->m_pcTextureMap != NULL)
            {
                if (NiStricmp(pkTextureDesc->m_pcTextureMap, "base") == 0)
                    m_uiTextureFlags = NiD3DTextureStage::TSTF_NDL_BASE;
                else if (NiStricmp(pkTextureDesc->m_pcTextureMap, 
                    "dark") == 0)
                    m_uiTextureFlags = NiD3DTextureStage::TSTF_NDL_DARK;
                else if (NiStricmp(pkTextureDesc->m_pcTextureMap, 
                    "detail")== 0)
                    m_uiTextureFlags = NiD3DTextureStage::TSTF_NDL_DETAIL;
                else if (NiStricmp(pkTextureDesc->m_pcTextureMap, 
                    "gloss") == 0)
                    m_uiTextureFlags = NiD3DTextureStage::TSTF_NDL_GLOSS;
                else if (NiStricmp(pkTextureDesc->m_pcTextureMap, 
                    "glow") == 0)
                    m_uiTextureFlags = NiD3DTextureStage::TSTF_NDL_GLOW;
                else if (NiStricmp(pkTextureDesc->m_pcTextureMap, 
                    "bump") == 0)
                    m_uiTextureFlags = NiD3DTextureStage::TSTF_NDL_BUMP;
                else if (NiStricmp(pkTextureDesc->m_pcTextureMap, 
                    "normal") == 0)
                    m_uiTextureFlags = NiD3DTextureStage::TSTF_NDL_NORMAL;
                else if (NiStricmp(pkTextureDesc->m_pcTextureMap, 
                    "parallax") == 0)
                    m_uiTextureFlags = NiD3DTextureStage::TSTF_NDL_PARALLAX;
                else if (NiStricmp(pkTextureDesc->m_pcTextureMap, 
                    "decal") == 0)
                {
                    if (pkTextureDesc->m_uiTextureIndex < 0)
                        pkTextureDesc->m_uiTextureIndex = 0;
                    m_uiTextureFlags = NiD3DTextureStage::TSTF_MAP_DECAL | 
                        pkTextureDesc->m_uiTextureIndex;
                }
                else // shader maps
                {
                    m_uiTextureFlags = NiD3DTextureStage::TSTF_MAP_SHADER | 
                        pkTextureDesc->m_uiTextureIndex;

                    // Only add shader map attributes
                    pkAttribTable->AddAttribDesc_Texture(m_kName, 
                        pkTextureDesc->m_bHidden, 
                        pkTextureDesc->m_uiTextureIndex, 
                        pkTextureDesc->m_pcTextureName);
                }

                return true;
            }

            if (pkTextureDesc->m_uiObjectType != 
                NiShaderAttributeDesc::OT_UNDEFINED)
            {
                // If the object type references a light assume the sampler
                // references a shadow map as there is no other valid 
                // definition for a texture with a light object type.
                if (pkTextureDesc->m_uiObjectType >= 
                    NiShaderAttributeDesc::OT_EFFECT_GENERALLIGHT &&
                    pkTextureDesc->m_uiObjectType <= 
                    NiShaderAttributeDesc::OT_EFFECT_SHADOWSPOTLIGHT)
                {
                    return true;
                }
            }


            // Check for texture file
            if (pkTextureDesc->m_pcTextureName != NULL)
            {
                char* pcTextureSource = NULL;
                NiD3DUtility::SetString(pcTextureSource, 0, 
                    pkTextureDesc->m_pcTextureName);
                m_pcTextureSource = pcTextureSource;
                m_eTextureType = pkTextureDesc->m_eTextureType;

                return true;
            }
            
            // Check for procedural texture
            if (pkTextureDesc->m_pcTextureFunction != NULL)
            {
                char* pcTextureSource = NULL;
                NiD3DUtility::SetString(pcTextureSource, 0, 
                    pkTextureDesc->m_pcTextureFunction);
                m_pcTextureSource = pcTextureSource;

                char* pcTextureTarget = NULL;
                if (pkTextureDesc->m_pcTextureTarget)
                {
                    NiD3DUtility::SetString(pcTextureTarget, 0, 
                        pkTextureDesc->m_pcTextureTarget);
                }
                else
                {
                    NiD3DUtility::SetString(pcTextureTarget, 0, "tx_1_0");
                }
                m_pcTextureTarget = pcTextureTarget;
                m_eTextureType = pkTextureDesc->m_eTextureType;
                return true;
            }
        }
        break;
    case NiD3DXEffectParameterDesc::PT_PREDEFINED:
        {
            NiD3DXEffectParameterPredefinedDesc* pkPredefinedDesc = 
                (NiD3DXEffectParameterPredefinedDesc*)pkParamDesc;

           m_kName = pkParamDesc->m_pcName;

            m_uiParameterCount = 1;
            m_uiArrayCount = pkPredefinedDesc->m_uiArrayCount;

            m_ePredefinedMapping = (NiD3DShaderConstantMap::DefinedMappings)
                pkPredefinedDesc->m_uiMapping;

            m_uiExtraData = pkPredefinedDesc->m_uiExtraData;
            return true;
        }
        break;
    case NiD3DXEffectParameterDesc::PT_OBJECT:
        {
            NiD3DXEffectParameterObjectDesc* pkObjectDesc =
                (NiD3DXEffectParameterObjectDesc*) pkParamDesc;

            m_kName = pkParamDesc->m_pcName;

            m_uiParameterCount = 1;

            m_eObjectMapping = (NiD3DShaderConstantMap::ObjectMappings)
                pkObjectDesc->m_uiMapping;
            m_eObjectType = pkObjectDesc->m_eObjectType;
            m_uiExtraData = pkObjectDesc->m_uiObjectIndex;

            return true;
        }
        break;
    case NiD3DXEffectParameterDesc::PT_ARRAY:
        {
            NiD3DXEffectParameterArrayDesc* pkArrayDesc = 
                (NiD3DXEffectParameterArrayDesc*)pkParamDesc;

            m_kName = pkParamDesc->m_pcName;
            m_uiParameterCount = pkArrayDesc->m_uiNumElements;

            // Check for valid range
            float* pfMin = pkArrayDesc->m_afMinValue;
            float* pfMax = pkArrayDesc->m_afMaxValue;
            bool bEqual = true;
            unsigned int i = 0;
            unsigned int uiTotalElements = pkArrayDesc->m_uiNumElements *
                pkArrayDesc->m_uiNumCols * pkArrayDesc->m_uiNumRows;
            for (; i < uiTotalElements; i++)
            {
                if (pfMin[i] > pfMax[i])
                    break;
                else if (pfMin[i] < pfMax[i])
                    bEqual = false;
            }
            if (bEqual || i < uiTotalElements)
            {
                pfMin = NULL;
                pfMax = NULL;
            }
  
            if (m_bGlobal)
            {
                NiD3DShaderFactory::RegisterGlobalShaderConstant(
                    m_kName, 
                    NiShaderAttributeDesc::ATTRIB_TYPE_ARRAY, 
                    pkArrayDesc->m_uiNumElements * 
                    pkArrayDesc->GetElementSize(), 
                    &pkArrayDesc->m_afInitValue);
            }
            else
            {
                NiShaderAttributeDesc::AttributeType eSubType =
                    NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;

                switch(pkArrayDesc->GetArrayType())
                {
                    case NiD3DXEffectParameterArrayDesc::TYPE_FLOAT:
                        eSubType = NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
                        break;
                    case NiD3DXEffectParameterArrayDesc::TYPE_POINT2:
                        eSubType = NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
                        break;
                    case NiD3DXEffectParameterArrayDesc::TYPE_POINT3:
                        eSubType = NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
                        break;
                    case NiD3DXEffectParameterArrayDesc::TYPE_POINT4:
                        eSubType = NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
                        break;
                    case NiD3DXEffectParameterArrayDesc::TYPE_COLORA:
                        eSubType = NiShaderAttributeDesc::ATTRIB_TYPE_COLOR;
                        break;
                }

                // Only add non-global attributes
                pkAttribTable->AddAttribDesc_Array(m_kName, 
                    pkArrayDesc->m_bHidden, 
                    pkArrayDesc->m_afInitValue, 
                    pkArrayDesc->m_uiNumElements,
                    pkArrayDesc->GetElementSize(),
                    eSubType, pfMin, pfMax);
            }
            
            return true;
        }
        break;
    }
    return false;
}
//---------------------------------------------------------------------------
NiD3DError NiD3DXEffectParameter::SetParameter(
    const NiRenderCallContext& kRCC, 
    LPD3DXEFFECT pkEffect,
    bool bVertexShaderPresent)
{
    NIASSERT(pkEffect);

    const NiDynamicEffectState* pkEffects = kRCC.m_pkEffects;

    const NiPropertyState* pkState = kRCC.m_pkState;

    NiMesh* pkMesh = NiVerifyStaticCast(NiMesh, kRCC.m_pkMesh);

    NIASSERT(kRCC.m_pkWorld);

    NIASSERT(kRCC.m_pkWorldBound);

    // Check for predefined mapping.
    if (m_ePredefinedMapping != NiShaderConstantMap::SCM_DEF_INVALID)
    {
        if (FillPredefinedConstantValue(kRCC, pkEffect, bVertexShaderPresent))
        {
            return NISHADERERR_OK;
        }
        else
        {
            return NISHADERERR_SETCONSTANTFAILED;
        }
    }

    // Check for object mapping.
    if (m_eObjectMapping != NiShaderConstantMap::SCM_OBJ_INVALID)
    {
        return FillObjectConstantValue(kRCC, pkEffect);
    }

    // Check for Gamebryo texture
    if (m_eObjectType != NiShaderAttributeDesc::OT_UNDEFINED ||
        m_uiTextureFlags != NiD3DTextureStage::TSTF_IGNORE)
    {
        D3DBaseTexturePtr pkD3DTexture = NULL;

        // Check for texture from NiDynamicEffect.
        NiTextureEffect* pkTextureEffect = NULL;
        NiShadowGenerator* pkGenerator = NULL;
        switch (m_eObjectType)
        {
            case NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP:
                pkTextureEffect = pkEffects->GetEnvironmentMap();
                break;
            case NiShaderAttributeDesc::OT_EFFECT_PROJECTEDSHADOWMAP:
            {
                unsigned int uiIndex = 0;
                NiDynEffectStateIter kIter =
                    pkEffects->GetProjShadowHeadPos();
                while (kIter)
                {
                    NiTextureEffect* pkProjShadow =
                        pkEffects->GetNextProjShadow(kIter);
                    if (uiIndex++ == m_uiExtraData)
                    {
                        pkTextureEffect = pkProjShadow;
                        break;
                    }
                }
                break;
            }
            case NiShaderAttributeDesc::OT_EFFECT_PROJECTEDLIGHTMAP:
            {
                unsigned int uiIndex = 0;
                NiDynEffectStateIter kIter =
                    pkEffects->GetProjLightHeadPos();
                while (kIter)
                {
                    NiTextureEffect* pkProjLight =
                        pkEffects->GetNextProjLight(kIter);
                    if (uiIndex++ == m_uiExtraData)
                    {
                        pkTextureEffect = pkProjLight;
                        break;
                    }
                }
                break;
            }
            case NiShaderAttributeDesc::OT_EFFECT_FOGMAP:
                pkTextureEffect = pkEffects->GetFogMap();
                break;
            case NiShaderAttributeDesc::OT_EFFECT_POINTLIGHT:
            {
                NiDynamicEffect* pkDynEffect = 
                    NiShaderConstantMap::GetDynamicEffectForObject(pkEffects, 
                    NiShaderAttributeDesc::OT_EFFECT_POINTLIGHT, 
                    m_uiExtraData);

                if (pkDynEffect)
                    pkGenerator = pkDynEffect->GetShadowGenerator();
                break;
            }
            case NiShaderAttributeDesc::OT_EFFECT_SPOTLIGHT:
            {
                NiDynamicEffect* pkDynEffect = 
                    NiShaderConstantMap::GetDynamicEffectForObject(pkEffects, 
                    NiShaderAttributeDesc::OT_EFFECT_SPOTLIGHT, 
                    m_uiExtraData);

                if (pkDynEffect)
                    pkGenerator = pkDynEffect->GetShadowGenerator();
                break;
            }
            case NiShaderAttributeDesc::OT_EFFECT_DIRECTIONALLIGHT:
            {
                NiDynamicEffect* pkDynEffect = 
                    NiShaderConstantMap::GetDynamicEffectForObject(pkEffects, 
                    NiShaderAttributeDesc::OT_EFFECT_DIRECTIONALLIGHT, 
                    m_uiExtraData);

                if (pkDynEffect)
                    pkGenerator = pkDynEffect->GetShadowGenerator();
                break;
            }
            case NiShaderAttributeDesc::OT_EFFECT_SHADOWPOINTLIGHT:
            {
                NiDynamicEffect* pkDynEffect = 
                    NiShaderConstantMap::GetDynamicEffectForObject(pkEffects, 
                    NiShaderAttributeDesc::OT_EFFECT_SHADOWPOINTLIGHT, 
                    m_uiExtraData);

                if (pkDynEffect)
                    pkGenerator = pkDynEffect->GetShadowGenerator();
                break;
            }
            case NiShaderAttributeDesc::OT_EFFECT_SHADOWSPOTLIGHT:
            {
                NiDynamicEffect* pkDynEffect = 
                    NiShaderConstantMap::GetDynamicEffectForObject(pkEffects, 
                    NiShaderAttributeDesc::OT_EFFECT_SHADOWSPOTLIGHT, 
                    m_uiExtraData);

                if (pkDynEffect)
                    pkGenerator = pkDynEffect->GetShadowGenerator();
                break;
            }
            case NiShaderAttributeDesc::OT_EFFECT_SHADOWDIRECTIONALLIGHT:
            {
                NiDynamicEffect* pkDynEffect = 
                    NiShaderConstantMap::GetDynamicEffectForObject(pkEffects, 
                    NiShaderAttributeDesc::OT_EFFECT_SHADOWDIRECTIONALLIGHT, 
                    m_uiExtraData);

                if (pkDynEffect)
                    pkGenerator = pkDynEffect->GetShadowGenerator();
                break;
            }
        }
        if (pkTextureEffect)
        {
            NiTexture* pkTexture = pkTextureEffect->GetEffectTexture();
            if (pkTexture)
            {
                // This comes from NiD3DTextureStage::ApplyTexture
#if !defined(_XENON)
                bool bS0Mipmap = false;
#endif
                bool bChanged; 
#if !defined(_XENON)
                bool bNonPow2 = false;
#endif
                NiD3DRenderer* pkD3DRenderer = (NiD3DRenderer*)
                    NiRenderer::GetRenderer();
                NIASSERT(pkD3DRenderer);
                pkD3DTexture = pkD3DRenderer->GetTextureManager()->
                    PrepareTextureForRendering(pkTexture, bChanged
#if defined(_XENON)
                    );
#else
                    , bS0Mipmap, bNonPow2);
#endif
            }
        }
        else if (pkGenerator)
        {
            NiShadowMap* pkShadowMap = pkGenerator->RetrieveShadowMap(
                NiShadowGenerator::AUTO_DETERMINE_SM_INDEX, pkMesh);
            NIASSERT(pkShadowMap);

            NiTexture* pkTexture = pkShadowMap->GetTexture();

            if (pkTexture)
            {
                // This comes from NiD3DTextureStage::ApplyTexture
#if !defined(_XENON)
                bool bS0Mipmap = false;
#endif
                bool bChanged; 
#if !defined(_XENON)
                bool bNonPow2 = false;
#endif
                NiD3DRenderer* pkD3DRenderer = (NiD3DRenderer*)
                    NiRenderer::GetRenderer();
                NIASSERT(pkD3DRenderer);
                pkD3DTexture = pkD3DRenderer->GetTextureManager()->
                    PrepareTextureForRendering(pkTexture, bChanged
#if defined(_XENON)
                    );
#else
                    , bS0Mipmap, bNonPow2);
#endif
            }
        }


        // Check for texture in NiTexturingProperty.
        if (!pkD3DTexture &&
            m_uiTextureFlags != NiD3DTextureStage::TSTF_IGNORE)
        {
            NiTexturingProperty* pkTexProp = pkState->GetTexturing();
            NIASSERT(pkTexProp);
            pkD3DTexture = GetNiTexture(pkTexProp, m_uiTextureFlags);
        }

        if (pkD3DTexture)
        {
            pkEffect->SetTexture(m_pkParam, pkD3DTexture);
            return NISHADERERR_OK;
        }
        else
        {
            // Texture not found
            return NISHADERERR_ENTRYNOTFOUND;
        }
    }

    // Check for Attribute mapping
    if (m_eParameterType != NiD3DXEffectParameterDesc::PT_NONE)
    {
        // Textures first...
        if (m_eParameterType == NiD3DXEffectParameterDesc::PT_TEXTURE)
        {
            if (m_eTextureType == 
                NiD3DXEffectParameterTextureDesc::TT_3D)
            {
                if (m_pkTexture == NULL)
                    Construct3DTexture();
            }
            else if (m_eTextureType == 
                NiD3DXEffectParameterTextureDesc::TT_CUBE)
            {
                if (m_pkTexture == NULL)
                    ConstructCubeTexture();
            }
            else if (m_eTextureType == 
                NiD3DXEffectParameterTextureDesc::TT_2D)
            {
                if (m_pkTexture == NULL)
                    Construct2DTexture();
            }

            if (m_pkTexture)
            {
                pkEffect->SetTexture(m_pkParam, m_pkTexture);
                return NISHADERERR_OK;
            }
            else
            {
                // Texture not found
                return NISHADERERR_ENTRYNOTFOUND;
            }
        }
        
        // Extra Data objects next...
        if (pkMesh == NULL)
            return NISHADERERR_ENTRYNOTFOUND;

        // Grab the attribute from the geometry and set it
        if (m_bGlobal)
        {
            unsigned int uiDataSize = 0;
            const void* pvData = NULL;
            bool bSuccess = NiD3DShaderFactory::RetrieveGlobalShaderConstant(
                m_kName, uiDataSize, pvData);
            if (!bSuccess)
                return NISHADERERR_ENTRYNOTFOUND;

            if (m_eParameterType == NiD3DXEffectParameterDesc::PT_FLOAT)
            {
                float fResult = *(float*)pvData;
                if (uiDataSize == sizeof(float))
                {
                    pkEffect->SetFloat(m_pkParam, fResult);
                    return NISHADERERR_OK;
                }
            }
            else if (m_eParameterType == NiD3DXEffectParameterDesc::PT_VECTOR)
            {
                D3DXVECTOR4 kVector;
                float* pfResult = (float*)pvData;

                if (uiDataSize == 2 * sizeof(float))
                {
                    kVector.x = pfResult[0];
                    kVector.y = pfResult[1];
                    kVector.z = 0.0f;
                    kVector.w = 1.0f;
                }
                else if (uiDataSize == 3 * sizeof(float))
                {
                    kVector.x = pfResult[0];
                    kVector.y = pfResult[1];
                    kVector.z = pfResult[2];
                    kVector.w = 1.0f;
                }
                else
                {
                    NIASSERT(uiDataSize == 4 * sizeof(float));
                    kVector = (D3DXVECTOR4)pfResult;
                }
                
#ifdef NIDEBUG
                HRESULT hr = 
#endif
                    pkEffect->SetVector(m_pkParam, &kVector);
                NIASSERT(!FAILED(hr));

                return NISHADERERR_OK;
            }
            else if (m_eParameterType == NiD3DXEffectParameterDesc::PT_MATRIX)
            {
                // m_uiWidth = num columns
                // m_uiHeight = num rows
                D3DXMATRIX kMatrix;
                float* pfResult = (float*)pvData;
                if (uiDataSize == 16 * sizeof(float))
                {
                    kMatrix = (D3DXMATRIX)pfResult;
                }
                else if (m_uiWidth == 3)
                {
                    NIASSERT(uiDataSize >= 9 * sizeof(float));
                    kMatrix._11 = pfResult[0];
                    kMatrix._12 = pfResult[1];
                    kMatrix._13 = pfResult[2];
                    kMatrix._14 = 0.0f;
                    kMatrix._21 = pfResult[3];
                    kMatrix._22 = pfResult[4];
                    kMatrix._23 = pfResult[5];
                    kMatrix._24 = 0.0f;
                    kMatrix._31 = pfResult[6];
                    kMatrix._32 = pfResult[7];
                    kMatrix._33 = pfResult[8];
                    kMatrix._34 = 0.0f;
                    if (m_uiHeight == 3)
                    {
                        kMatrix._41 = 0.0f;
                        kMatrix._42 = 0.0f;
                        kMatrix._43 = 0.0f;
                        kMatrix._44 = 1.0f;
                    }
                    else
                    {
                        NIASSERT(uiDataSize >= 12 * sizeof(float));
                        kMatrix._41 = pfResult[9];
                        kMatrix._42 = pfResult[10];
                        kMatrix._43 = pfResult[11];
                        kMatrix._44 = 1.0f;
                    }
                }
                else
                {
                    NIASSERT(uiDataSize >= 12 * sizeof(float));
                    kMatrix._11 = pfResult[0];
                    kMatrix._12 = pfResult[1];
                    kMatrix._13 = pfResult[2];
                    kMatrix._14 = pfResult[3];
                    kMatrix._21 = pfResult[4];
                    kMatrix._22 = pfResult[5];
                    kMatrix._23 = pfResult[6];
                    kMatrix._24 = pfResult[7];
                    kMatrix._31 = pfResult[8];
                    kMatrix._32 = pfResult[9];
                    kMatrix._33 = pfResult[10];
                    kMatrix._34 = pfResult[11];
                    kMatrix._41 = 0.0f;
                    kMatrix._42 = 0.0f;
                    kMatrix._43 = 0.0f;
                    kMatrix._44 = 1.0f;
                }

                pkEffect->SetMatrix(m_pkParam, &kMatrix);
                return NISHADERERR_OK;
            }
            else if (m_eParameterType == NiD3DXEffectParameterDesc::PT_UINT)
            {
                if (uiDataSize == sizeof(int))
                {
                    int iResult = *(int*)pvData;
                    pkEffect->SetInt(m_pkParam, (unsigned int)iResult);
                    return NISHADERERR_OK;
                }
            }
            else if (m_eParameterType == NiD3DXEffectParameterDesc::PT_BOOL)
            {
                if (uiDataSize == sizeof(bool))
                {
                    bool bResult = *(bool*)pvData;
                    pkEffect->SetBool(m_pkParam, bResult);
                    return NISHADERERR_OK;
                }
            }
            else if (m_eParameterType == 
                NiD3DXEffectParameterDesc::PT_ARRAY)
            {
                D3DXPARAMETER_DESC kDesc;
                HRESULT hr = pkEffect->GetParameterDesc(m_pkParam,
                    &kDesc);
                NIASSERT(!FAILED(hr));
                
                if (kDesc.Type != D3DXPT_FLOAT)
                    return NISHADERERR_ENTRYNOTFOUND;

                if (kDesc.Class == D3DXPC_SCALAR)
                {
                    float* pfResult = (float*)pvData;

#ifdef NIDEBUG
                    HRESULT hr = 
#endif
                        pkEffect->SetFloatArray(m_pkParam, pfResult,
                        uiDataSize / sizeof(float));
                    NIASSERT(!FAILED(hr));
                    return NISHADERERR_OK;
                }
                else if (kDesc.Class == D3DXPC_VECTOR)
                {
                    float* pfResult = (float*)pvData;
#if defined(_XENON)         
                    // Xenon expects tightly packed data so we'll call 
                    // SetFloatArray.
                    unsigned int uiNumFloats = kDesc.Elements * kDesc.Rows *
                        kDesc.Columns;
                    NIASSERT(uiNumFloats == uiDataSize / sizeof(float));
                    hr = pkEffect->SetFloatArray(m_pkParam, pfResult, 
                        uiNumFloats);
#else
                    // We can use VECTOR4's as-is, but we need 
                    // to expand out VECTOR2 & VECTOR3's
                    if (kDesc.Columns == 4 && kDesc.Rows == 1)
                    {
                        LPD3DXVECTOR4 pkResult = (LPD3DXVECTOR4)pfResult;
                        hr = pkEffect->SetVectorArray(m_pkParam, pkResult,
                           uiDataSize / sizeof(D3DXVECTOR4));
                    }
                    else
                    {
#ifdef NIDEBUG
                        bool bFilled = 
#endif
                            NiD3DXEffectParameterArrayDesc::
                            ExpandToVector4Array(pfResult, ms_akVector4Array,
                            kDesc.Rows, kDesc.Columns, kDesc.Elements);
                        NIASSERT(bFilled);
                        hr = pkEffect->SetVectorArray(m_pkParam, 
                            ms_akVector4Array, 
                            kDesc.Rows * kDesc.Elements);
                    }
#endif
                    NIASSERT(!FAILED(hr));
                    return NISHADERERR_OK;
                }
                return NISHADERERR_ENTRYNOTFOUND;
            }
            else
            {
                // These are currently the only types we consider
                NIASSERT(false);
            }
        }
        else
        {
            NiExtraData* pkExtra = pkMesh->GetExtraData(m_kName);
            if (!pkExtra)
                return NISHADERERR_ENTRYNOTFOUND;

            if (m_eParameterType == NiD3DXEffectParameterDesc::PT_FLOAT)
            {
                NiFloatExtraData* pkFloatED = (NiFloatExtraData*)pkExtra;
            
                float fResult = pkFloatED->GetValue();
                pkEffect->SetFloat(m_pkParam, fResult);
                return NISHADERERR_OK;
            }
            else if (m_eParameterType == NiD3DXEffectParameterDesc::PT_VECTOR)
            {
                D3DXVECTOR4 kVector;
                if (NiIsKindOf(NiFloatsExtraData, pkExtra))
                {
                    NiFloatsExtraData* pkFloatsED = 
                        (NiFloatsExtraData*)pkExtra;
                
                    float* pfResult = NULL;
                    unsigned int uiNumValues = 0;
                    pkFloatsED->GetArray(uiNumValues, pfResult);
                    if (uiNumValues == 2)
                    {
                        kVector.x = pfResult[0];
                        kVector.y = pfResult[1];
                        kVector.z = 0.0f;
                        kVector.w = 1.0f;
                    }
                    else if (uiNumValues == 3)
                    {
                        kVector.x = pfResult[0];
                        kVector.y = pfResult[1];
                        kVector.z = pfResult[2];
                        kVector.w = 1.0f;
                    }
                    else
                    {
                        NIASSERT(uiNumValues == 4);
                        kVector = (D3DXVECTOR4)pfResult;
                    }
                }
                else if (NiIsKindOf(NiColorExtraData, pkExtra))
                {
                    NiColorExtraData* pkColorED = (NiColorExtraData*)pkExtra;
                
                    kVector.x = pkColorED->GetRed();
                    kVector.y = pkColorED->GetGreen();
                    kVector.z = pkColorED->GetBlue();
                    NIASSERT((m_uiParameterCount == 3) ||
                        (m_uiParameterCount == 4));
                    if (m_uiParameterCount == 3)
                        kVector.w = 1.0f;
                    else
                        kVector.w = pkColorED->GetAlpha();
                }
                pkEffect->SetVector(m_pkParam, &kVector);
                return NISHADERERR_OK;
            }
            else if (m_eParameterType == NiD3DXEffectParameterDesc::PT_MATRIX)
            {
                NiFloatsExtraData* pkFloatsED = (NiFloatsExtraData*)pkExtra;
            
                float* pfResult = NULL;
                unsigned int uiNumValues = 0;
                pkFloatsED->GetArray(uiNumValues, pfResult);

                // m_uiWidth = num columns
                // m_uiHeight = num rows
                D3DXMATRIX kMatrix;
                if (uiNumValues == 16)
                {
                    kMatrix = (D3DXMATRIX)pfResult;
                }
                else if (m_uiWidth == 3)
                {
                    kMatrix._11 = pfResult[0];
                    kMatrix._12 = pfResult[1];
                    kMatrix._13 = pfResult[2];
                    kMatrix._14 = 0.0f;
                    kMatrix._21 = pfResult[3];
                    kMatrix._22 = pfResult[4];
                    kMatrix._23 = pfResult[5];
                    kMatrix._24 = 0.0f;
                    kMatrix._31 = pfResult[6];
                    kMatrix._32 = pfResult[7];
                    kMatrix._33 = pfResult[8];
                    kMatrix._34 = 0.0f;
                    if (m_uiHeight == 3)
                    {
                        kMatrix._41 = 0.0f;
                        kMatrix._42 = 0.0f;
                        kMatrix._43 = 0.0f;
                        kMatrix._44 = 1.0f;
                    }
                    else
                    {
                        kMatrix._41 = pfResult[9];
                        kMatrix._42 = pfResult[10];
                        kMatrix._43 = pfResult[11];
                        kMatrix._44 = 1.0f;
                    }
                }
                else
                {
                    kMatrix._11 = pfResult[0];
                    kMatrix._12 = pfResult[1];
                    kMatrix._13 = pfResult[2];
                    kMatrix._14 = pfResult[3];
                    kMatrix._21 = pfResult[4];
                    kMatrix._22 = pfResult[5];
                    kMatrix._23 = pfResult[6];
                    kMatrix._24 = pfResult[7];
                    kMatrix._31 = pfResult[8];
                    kMatrix._32 = pfResult[9];
                    kMatrix._33 = pfResult[10];
                    kMatrix._34 = pfResult[11];
                    kMatrix._41 = 0.0f;
                    kMatrix._42 = 0.0f;
                    kMatrix._43 = 0.0f;
                    kMatrix._44 = 1.0f;
                }

                pkEffect->SetMatrix(m_pkParam, &kMatrix);
                return NISHADERERR_OK;
            }
            else if (m_eParameterType == NiD3DXEffectParameterDesc::PT_UINT)
            {
                // Currently only support setting 1 int.
                NiIntegerExtraData* pkIntED = (NiIntegerExtraData*)pkExtra;
            
                int iResult = pkIntED->GetValue();
                pkEffect->SetInt(m_pkParam, (unsigned int)iResult);
                return NISHADERERR_OK;
            }
            else if (m_eParameterType == NiD3DXEffectParameterDesc::PT_BOOL)
            {
                // Currently only support setting 1 bool.
                NiBooleanExtraData* pkBoolED = (NiBooleanExtraData*)pkExtra;
            
                bool bResult = pkBoolED->GetValue();
                pkEffect->SetBool(m_pkParam, bResult);
                return NISHADERERR_OK;
            }
            else if (m_eParameterType == 
                NiD3DXEffectParameterDesc::PT_ARRAY)
            {
                if (NiIsKindOf(NiFloatsExtraData, pkExtra))
                {
                    NiFloatsExtraData* pkFloatsED = 
                        (NiFloatsExtraData*)pkExtra;
                
                    float* pfResult = NULL;
                    unsigned int uiNumValues = 0;
                    pkFloatsED->GetArray(uiNumValues, pfResult);
                    HRESULT hr;
                    D3DXPARAMETER_DESC kDesc;
                    hr = pkEffect->GetParameterDesc(m_pkParam, &kDesc);
                    NIASSERT(!FAILED(hr));

                    if (kDesc.Type != D3DXPT_FLOAT)
                        return NISHADERERR_ENTRYNOTFOUND;

                    if (kDesc.Class == D3DXPC_SCALAR)
                    {
                        hr = pkEffect->SetFloatArray(m_pkParam, pfResult,
                            uiNumValues);
                        NIASSERT(!FAILED(hr));
                        return NISHADERERR_OK;
                    }
                    else if (kDesc.Class == D3DXPC_VECTOR)
                    {
#if defined(_XENON)         
                        // Xenon expects tightly packed data so we'll call 
                        // SetFloatArray.
                        unsigned int uiNumFloats = kDesc.Elements * 
                            kDesc.Rows * kDesc.Columns;
                        NIASSERT(uiNumFloats == uiNumValues);
                        hr = pkEffect->SetFloatArray(m_pkParam, pfResult, 
                            uiNumFloats);
#else
                        // We can use VECTOR4's as-is, but we need 
                        // to expand out VECTOR2 & VECTOR3's
                        if (kDesc.Columns == 4 && kDesc.Rows == 1)
                        {
                            LPD3DXVECTOR4 pkResult = (LPD3DXVECTOR4)pfResult;
                            hr = pkEffect->SetVectorArray(m_pkParam, pkResult,
                                uiNumValues / 4);
                        }
                        else
                        {
#ifdef NIDEBUG
                            bool bFilled = 
#endif
                                NiD3DXEffectParameterArrayDesc::
                                ExpandToVector4Array(pfResult, 
                                ms_akVector4Array, kDesc.Rows, kDesc.Columns,
                                kDesc.Elements);
                            NIASSERT(bFilled);
                            hr = pkEffect->SetVectorArray(m_pkParam, 
                                ms_akVector4Array, 
                                kDesc.Rows * kDesc.Elements);
                        }
#endif                        
                        NIASSERT(!FAILED(hr));
                        return NISHADERERR_OK;
                    }

                    return NISHADERERR_ENTRYNOTFOUND;
                }
                return NISHADERERR_ENTRYNOTFOUND;
            }
            else
            {
                // These are currently the only types we consider
                NIASSERT(false);
            }
        }
    }

    return NISHADERERR_ENTRYNOTFOUND;
}
//---------------------------------------------------------------------------
void NiD3DXEffectParameter::LinkHandle(LPD3DXEFFECT pkEffect)
{
    m_pkParam = pkEffect->GetParameterByName(NULL, m_kName);
}
//---------------------------------------------------------------------------
void NiD3DXEffectParameter::DestroyRendererData()
{
    if (m_pkTexture)
    {
        m_pkTexture->Release();
        m_pkTexture = NULL;
    }
}
//---------------------------------------------------------------------------
void NiD3DXEffectParameter::ReleaseBoneArray()
{
    NiDelete[] ms_pkMatrixArray;
    ms_pkMatrixArray = NULL;
}
//---------------------------------------------------------------------------
bool NiD3DXEffectParameter::FillPredefinedConstantValue(
    const NiRenderCallContext& kRCC, 
    LPD3DXEFFECT pkEffect, 
    bool bVertexShaderPresent)
{
    const NiDynamicEffectState* pkEffects = kRCC.m_pkEffects;

    const NiPropertyState* pkState = kRCC.m_pkState;

    NiMesh* pkMesh = NiVerifyStaticCast(NiMesh, kRCC.m_pkMesh);

    NIASSERT(kRCC.m_pkWorld);

    NIASSERT(kRCC.m_pkWorldBound);

    NiD3DRenderer* pkD3DRenderer = (NiD3DRenderer*)NiRenderer::GetRenderer();
    switch (m_ePredefinedMapping)
    {
    case NiShaderConstantMap::SCM_DEF_PROJ:
    case NiShaderConstantMap::SCM_DEF_INVPROJ:
    case NiShaderConstantMap::SCM_DEF_PROJ_T:
    case NiShaderConstantMap::SCM_DEF_INVPROJ_T:
        {
            bool bInv = false;
            bool bTrans = false;

            if ((m_ePredefinedMapping == 
                NiShaderConstantMap::SCM_DEF_INVPROJ) ||
                (m_ePredefinedMapping == 
                NiShaderConstantMap::SCM_DEF_INVPROJ_T))
            {
                bInv = true;
            }
            if ((m_ePredefinedMapping == 
                NiShaderConstantMap::SCM_DEF_PROJ_T) ||
                (m_ePredefinedMapping == 
                NiShaderConstantMap::SCM_DEF_INVPROJ_T))
            {
                bTrans = true;
            }
            
            D3DXALIGNEDMATRIX kD3DMat = pkD3DRenderer->GetD3DProj();

            if (bInv)
                D3DXMatrixInverse(&kD3DMat, 0, &kD3DMat);

            if (bTrans)
            {
                pkEffect->SetMatrixTranspose(m_pkParam, &kD3DMat);
            }
            else
            {
                pkEffect->SetMatrix(m_pkParam, &kD3DMat);
            }
        }
        break;
    case NiShaderConstantMap::SCM_DEF_VIEW:
    case NiShaderConstantMap::SCM_DEF_INVVIEW:
    case NiShaderConstantMap::SCM_DEF_VIEW_T:
    case NiShaderConstantMap::SCM_DEF_INVVIEW_T:
        {
            bool bInv = false;
            bool bTrans = false;

            if ((m_ePredefinedMapping == 
                    NiShaderConstantMap::SCM_DEF_INVVIEW) ||
                (m_ePredefinedMapping == 
                    NiShaderConstantMap::SCM_DEF_INVVIEW_T))
            {
                bInv = true;
            }
            if ((m_ePredefinedMapping ==
                    NiShaderConstantMap::SCM_DEF_VIEW_T) ||
                (m_ePredefinedMapping == 
                    NiShaderConstantMap::SCM_DEF_INVVIEW_T))
            {
                bTrans = true;
            }
            
            D3DXALIGNEDMATRIX kD3DMat = pkD3DRenderer->GetD3DView();

            if (bInv)
                D3DXMatrixInverse(&kD3DMat, 0, &kD3DMat);

            if (bTrans)
            {
                pkEffect->SetMatrixTranspose(m_pkParam, &kD3DMat);
            }
            else
            {
                pkEffect->SetMatrix(m_pkParam, &kD3DMat);
            }
        }
        break;
    case NiShaderConstantMap::SCM_DEF_WORLD:
    case NiShaderConstantMap::SCM_DEF_INVWORLD:
    case NiShaderConstantMap::SCM_DEF_WORLD_T:
    case NiShaderConstantMap::SCM_DEF_INVWORLD_T:
        {
            bool bInv = false;
            bool bTrans = false;

            if ((m_ePredefinedMapping ==
                    NiShaderConstantMap::SCM_DEF_INVWORLD) ||
                (m_ePredefinedMapping ==
                    NiShaderConstantMap::SCM_DEF_INVWORLD_T))
            {
                bInv = true;
            }
            if ((m_ePredefinedMapping ==
                    NiShaderConstantMap::SCM_DEF_WORLD_T) ||
                (m_ePredefinedMapping ==
                    NiShaderConstantMap::SCM_DEF_INVWORLD_T))
            {
                bTrans = true;
            }
            
            D3DXALIGNEDMATRIX kD3DMat = *(pkD3DRenderer->GetD3DWorld());

            if (bInv)
                D3DXMatrixInverse(&kD3DMat, 0, &kD3DMat);

            if (bTrans)
            {
                pkEffect->SetMatrixTranspose(m_pkParam, &kD3DMat);
            }
            else
            {
                pkEffect->SetMatrix(m_pkParam, &kD3DMat);
            }
        }
        break;
    case NiShaderConstantMap::SCM_DEF_WORLDVIEW:
    case NiShaderConstantMap::SCM_DEF_INVWORLDVIEW:
    case NiShaderConstantMap::SCM_DEF_WORLDVIEW_T:
    case NiShaderConstantMap::SCM_DEF_INVWORLDVIEW_T:
        {
            bool bInv = false;
            bool bTrans = false;

            if ((m_ePredefinedMapping ==
                    NiShaderConstantMap::SCM_DEF_INVWORLDVIEW) ||
                (m_ePredefinedMapping ==
                    NiShaderConstantMap::SCM_DEF_INVWORLDVIEW_T))
            {
                bInv = true;
            }
            if ((m_ePredefinedMapping ==
                    NiShaderConstantMap::SCM_DEF_WORLDVIEW_T) ||
                (m_ePredefinedMapping ==
                    NiShaderConstantMap::SCM_DEF_INVWORLDVIEW_T))
            {
                bTrans = true;
            }
            
            D3DXALIGNEDMATRIX kD3DWorld = *(pkD3DRenderer->GetD3DWorld());
            D3DXALIGNEDMATRIX kD3DView = pkD3DRenderer->GetD3DView();
            D3DXALIGNEDMATRIX kD3DMat = kD3DWorld * kD3DView;

            if (bInv)
                D3DXMatrixInverse(&kD3DMat, 0, &kD3DMat);

            if (bTrans)
            {
                pkEffect->SetMatrixTranspose(m_pkParam, &kD3DMat);
            }
            else
            {
                pkEffect->SetMatrix(m_pkParam, &kD3DMat);
            }
        }
        break;
    case NiShaderConstantMap::SCM_DEF_VIEWPROJ:
    case NiShaderConstantMap::SCM_DEF_INVVIEWPROJ:
    case NiShaderConstantMap::SCM_DEF_VIEWPROJ_T:
    case NiShaderConstantMap::SCM_DEF_INVVIEWPROJ_T:
        {
            bool bInv = false;
            bool bTrans = false;

            if ((m_ePredefinedMapping ==
                    NiShaderConstantMap::SCM_DEF_INVVIEWPROJ) ||
                (m_ePredefinedMapping ==
                    NiShaderConstantMap::SCM_DEF_INVVIEWPROJ_T))
            {
                bInv = true;
            }
            if ((m_ePredefinedMapping ==
                    NiShaderConstantMap::SCM_DEF_VIEWPROJ_T) ||
                (m_ePredefinedMapping ==
                    NiShaderConstantMap::SCM_DEF_INVVIEWPROJ_T))
            {
                bTrans = true;
            }


            D3DXALIGNEDMATRIX kD3DView = pkD3DRenderer->GetD3DView();
            D3DXALIGNEDMATRIX kD3DProj = pkD3DRenderer->GetD3DProj();
            D3DXALIGNEDMATRIX kD3DMat = kD3DView * kD3DProj;
#ifdef _XENON
            // The D3DMATRIX multiply is using VMX128 on Xbox 360
            // and results in slight errors. This causes the matrices
            // setup in Do_SetScreenSpaceCameraData to not have a
            // zero translation for z, thus clipping all screen
            // elements with z == 0.
            // 
            // To fight against this the value 0.000000238f, effectively 2 bits
            // of precision, is added to z-translation of the resulting matrix.
            kD3DMat._43 += 0.000000238f;
#endif


            if (bInv)
                D3DXMatrixInverse(&kD3DMat, 0, &kD3DMat);

            if (bTrans)
            {
                pkEffect->SetMatrixTranspose(m_pkParam, &kD3DMat);
            }
            else
            {
                pkEffect->SetMatrix(m_pkParam, &kD3DMat);
            }
        }
        break;
    case NiShaderConstantMap::SCM_DEF_WORLDVIEWPROJ:
    case NiShaderConstantMap::SCM_DEF_INVWORLDVIEWPROJ:
    case NiShaderConstantMap::SCM_DEF_WORLDVIEWPROJ_T:
    case NiShaderConstantMap::SCM_DEF_INVWORLDVIEWPROJ_T:
        {
            bool bInv = false;
            bool bTrans = false;

            if ((m_ePredefinedMapping ==
                    NiShaderConstantMap::SCM_DEF_INVWORLDVIEWPROJ) ||
                (m_ePredefinedMapping ==
                    NiShaderConstantMap::SCM_DEF_INVWORLDVIEWPROJ_T))
            {
                bInv = true;
            }
            if ((m_ePredefinedMapping ==
                    NiShaderConstantMap::SCM_DEF_WORLDVIEWPROJ_T) ||
                (m_ePredefinedMapping ==
                    NiShaderConstantMap::SCM_DEF_INVWORLDVIEWPROJ_T))
            {
                bTrans = true;
            }
            
            D3DXALIGNEDMATRIX kD3DWorld = *(pkD3DRenderer->GetD3DWorld());
            D3DXALIGNEDMATRIX kD3DView = pkD3DRenderer->GetD3DView();
            D3DXALIGNEDMATRIX kD3DProj = pkD3DRenderer->GetD3DProj();
            D3DXALIGNEDMATRIX kD3DWorldView = kD3DWorld * kD3DView;
            D3DXALIGNEDMATRIX kD3DMat = kD3DWorldView * kD3DProj;
#ifdef _XENON
            // The D3DMATRIX multiply is using VMX128 on Xbox 360
            // and results in slight errors. This causes the matrices
            // setup in Do_SetScreenSpaceCameraData to not have a
            // zero translation for z, thus clipping all screen
            // elements with z == 0.
            // 
            // To fight against this the value 0.000000238f, effectively 2 bits
            // of precision, is added to z-translation of the resulting matrix.
            kD3DMat._43 += 0.000000238f;
#endif

            if (bInv)
                D3DXMatrixInverse(&kD3DMat, 0, &kD3DMat);

            if (bTrans)
            {
                pkEffect->SetMatrixTranspose(m_pkParam, &kD3DMat);
            }
            else
            {
                pkEffect->SetMatrix(m_pkParam, &kD3DMat);
            }
        }
        break;
        // Bone matrices
    case NiShaderConstantMap::SCM_DEF_SKINBONE_MATRIX_3:
        {
            NiSkinningMeshModifier* pkSkin = 
                NiGetModifier(NiSkinningMeshModifier, pkMesh);

            if (!pkSkin)
                break;

            // grow static array if needed
            if (ms_uiMatrixArraySize < m_uiArrayCount)
            {
                NiExternalDelete[] ms_pkMatrixArray;
                ms_pkMatrixArray = 
                    NiExternalNew D3DXMATRIX[m_uiArrayCount];
                ms_uiMatrixArraySize = m_uiArrayCount;
            }                

            // Get the per-partition bone matrix palette
            NiDataStreamRef* pkBonePaletteStreamRef =
                pkMesh->FindStreamRef(NiCommonSemantics::BONE_PALETTE());
            NIASSERT(pkBonePaletteStreamRef != NULL);
            if (!pkBonePaletteStreamRef)
                break;

            const NiDataStream::Region& kRegion =
                pkBonePaletteStreamRef->GetRegionForSubmesh(kRCC.m_uiSubmesh);
            NiUInt32 uiRegionIdx = 
                pkBonePaletteStreamRef->GetRegionIndexForSubmesh(
                kRCC.m_uiSubmesh);

            NiDataStream* pkBonePaletteStream = 
                pkBonePaletteStreamRef->GetDataStream();

#if defined(NIDEBUG)
            // The palette indicies are assumed to be 16 bit unsigned ints,
            // packed in their own stream
            const NiDataStreamElement& kElem =
                pkBonePaletteStream->GetElementDescAt(0);
            NIASSERT(kElem.GetFormat() == NiDataStreamElement::F_UINT16_1);
            NIASSERT(pkBonePaletteStream->GetStride() == sizeof(NiUInt16));
            NIASSERT(kElem.GetOffset() == 0);
#endif
            const NiUInt32 uiBoneCount = kRegion.GetRange();

            // Get a pointer to the current region's bone palette
            const NiUInt16* puiBonePalette =
                (NiUInt16*)pkBonePaletteStream->LockRegion(uiRegionIdx, 
                NiDataStream::LOCK_READ);
            if (!puiBonePalette)
            {
                NILOG("Warning: Could not lock bone palette.\n");
                break;
            }

            NiMatrix3x4* pkBoneMatrices = pkSkin->GetBoneMatrices();
            if (!pkBoneMatrices)
            {
                NILOG("Warning: Could not find bone matrices.\n");
                break;
            }

            // move matrices to array of D3DXMATRIX
            // this requires us to pad out our 3x4 matrices
            // consider optimizing by storing our bone matrices as 4x4?
            NIASSERT(m_uiArrayCount >= uiBoneCount);
            for (unsigned int ui = 0; ui < uiBoneCount; ui++)
            {
                NIASSERT(puiBonePalette[ui] < pkSkin->GetBoneCount());

                NiMatrix3x4& kMat = 
                    pkBoneMatrices[puiBonePalette[ui]];

                ms_pkMatrixArray[ui]._11 = kMat.m_kEntry[0][0];
                ms_pkMatrixArray[ui]._12 = kMat.m_kEntry[0][1];
                ms_pkMatrixArray[ui]._13 = kMat.m_kEntry[0][2];
                ms_pkMatrixArray[ui]._14 = kMat.m_kEntry[0][3];
                ms_pkMatrixArray[ui]._21 = kMat.m_kEntry[1][0];
                ms_pkMatrixArray[ui]._22 = kMat.m_kEntry[1][1];
                ms_pkMatrixArray[ui]._23 = kMat.m_kEntry[1][2];
                ms_pkMatrixArray[ui]._24 = kMat.m_kEntry[1][3];
                ms_pkMatrixArray[ui]._31 = kMat.m_kEntry[2][0];
                ms_pkMatrixArray[ui]._32 = kMat.m_kEntry[2][1];
                ms_pkMatrixArray[ui]._33 = kMat.m_kEntry[2][2];
                ms_pkMatrixArray[ui]._34 = kMat.m_kEntry[2][3];
                ms_pkMatrixArray[ui]._41 = 0.0f;
                ms_pkMatrixArray[ui]._42 = 0.0f;
                ms_pkMatrixArray[ui]._43 = 0.0f;
                ms_pkMatrixArray[ui]._44 = 1.0f;
            }
            pkBonePaletteStream->Unlock(NiDataStream::LOCK_READ);

            // Gamebryo bone matrices are naturally transposed
            pkEffect->SetMatrixTransposeArray(
                m_pkParam, ms_pkMatrixArray, uiBoneCount);
        }
        break;
    case NiShaderConstantMap::SCM_DEF_TEXTRANSFORMBASE:
    case NiShaderConstantMap::SCM_DEF_INVTEXTRANSFORMBASE:
    case NiShaderConstantMap::SCM_DEF_TEXTRANSFORMBASE_T:
    case NiShaderConstantMap::SCM_DEF_INVTEXTRANSFORMBASE_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
            if (pkTexProp)
            {
                const NiTexturingProperty::Map* pkMap = 
                    pkTexProp->GetBaseMap();
                if (pkMap)
                {
                    const NiTextureTransform* pkTextureTransform = 
                        pkMap->GetTextureTransform();

                    if (pkTextureTransform)
                    {
                        pkMatrix = pkTextureTransform->GetMatrix();

                        if ((m_ePredefinedMapping == 
                            NiShaderConstantMap::SCM_DEF_INVTEXTRANSFORMBASE
                            ) ||
                            (m_ePredefinedMapping == NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMBASE_T))
                        {
                            bInv = true;
                        }
                        if ((m_ePredefinedMapping == 
                            NiShaderConstantMap::SCM_DEF_TEXTRANSFORMBASE_T
                                ) ||
                            (m_ePredefinedMapping == NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMBASE_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            D3DXALIGNEDMATRIX kD3DMat;
            NiD3DShaderConstantMap::SetupTextureTransformMatrix(
                kD3DMat, pkMatrix, bInv, false, 
                (bVertexShaderPresent ? 4 : 2));

            if (bTrans)
            {
                pkEffect->SetMatrixTranspose(m_pkParam, &kD3DMat);
            }
            else
            {
                pkEffect->SetMatrix(m_pkParam, &kD3DMat);
            }
        }
        break;
    case NiShaderConstantMap::SCM_DEF_TEXTRANSFORMDARK:
    case NiShaderConstantMap::SCM_DEF_INVTEXTRANSFORMDARK:
    case NiShaderConstantMap::SCM_DEF_TEXTRANSFORMDARK_T:
    case NiShaderConstantMap::SCM_DEF_INVTEXTRANSFORMDARK_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
            if (pkTexProp)
            {
                const NiTexturingProperty::Map* pkMap = 
                    pkTexProp->GetBumpMap();
                if (pkMap)
                {
                    const NiTextureTransform* pkTextureTransform = 
                        pkMap->GetTextureTransform();

                    if (pkTextureTransform)
                    {
                        pkMatrix = pkTextureTransform->GetMatrix();

                        if ((m_ePredefinedMapping == 
                            NiShaderConstantMap::SCM_DEF_INVTEXTRANSFORMDARK
                            ) ||
                            (m_ePredefinedMapping == NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMDARK_T))
                        {
                            bInv = true;
                        }
                        if ((m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_TEXTRANSFORMDARK_T) ||
                            (m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMDARK_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            D3DXALIGNEDMATRIX kD3DMat;
            NiD3DShaderConstantMap::SetupTextureTransformMatrix(kD3DMat,
                pkMatrix, bInv, false, (bVertexShaderPresent ? 4 : 2));

            if (bTrans)
            {
                pkEffect->SetMatrixTranspose(m_pkParam, &kD3DMat);
            }
            else
            {
                pkEffect->SetMatrix(m_pkParam, &kD3DMat);
            }
        }
        break;
    case NiShaderConstantMap::SCM_DEF_TEXTRANSFORMDETAIL:
    case NiShaderConstantMap::SCM_DEF_INVTEXTRANSFORMDETAIL:
    case NiShaderConstantMap::SCM_DEF_TEXTRANSFORMDETAIL_T:
    case NiShaderConstantMap::SCM_DEF_INVTEXTRANSFORMDETAIL_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
            if (pkTexProp)
            {
                const NiTexturingProperty::Map* pkMap = 
                    pkTexProp->GetDetailMap();
                if (pkMap)
                {
                    const NiTextureTransform* pkTextureTransform = 
                        pkMap->GetTextureTransform();

                    if (pkTextureTransform)
                    {
                        pkMatrix = pkTextureTransform->GetMatrix();

                        if ((m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMDETAIL) ||
                            (m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMDETAIL_T))
                        {
                            bInv = true;
                        }
                        if ((m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_TEXTRANSFORMDETAIL_T) ||
                            (m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMDETAIL_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            D3DXALIGNEDMATRIX kD3DMat;
            NiD3DShaderConstantMap::SetupTextureTransformMatrix(
                kD3DMat, pkMatrix, bInv, false, 
                (bVertexShaderPresent ? 4 : 2));

            if (bTrans)
            {
                pkEffect->SetMatrixTranspose(m_pkParam, &kD3DMat);
            }
            else
            {
                pkEffect->SetMatrix(m_pkParam, &kD3DMat);
            }
        }
        break;
    case NiShaderConstantMap::SCM_DEF_TEXTRANSFORMGLOSS:
    case NiShaderConstantMap::SCM_DEF_INVTEXTRANSFORMGLOSS:
    case NiShaderConstantMap::SCM_DEF_TEXTRANSFORMGLOSS_T:
    case NiShaderConstantMap::SCM_DEF_INVTEXTRANSFORMGLOSS_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
            if (pkTexProp)
            {
                const NiTexturingProperty::Map* pkMap = 
                    pkTexProp->GetGlossMap();
                if (pkMap)
                {
                    const NiTextureTransform* pkTextureTransform = 
                        pkMap->GetTextureTransform();

                    if (pkTextureTransform)
                    {
                        pkMatrix = pkTextureTransform->GetMatrix();

                        if ((m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMGLOSS) ||
                            (m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMGLOSS_T))
                        {
                            bInv = true;
                        }
                        if ((m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_TEXTRANSFORMGLOSS_T) ||
                            (m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMGLOSS_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            D3DXALIGNEDMATRIX kD3DMat;
            NiD3DShaderConstantMap::SetupTextureTransformMatrix(
                kD3DMat, pkMatrix, bInv, false, 
                (bVertexShaderPresent ? 4 : 2));

            if (bTrans)
            {
                pkEffect->SetMatrixTranspose(m_pkParam, &kD3DMat);
            }
            else
            {
                pkEffect->SetMatrix(m_pkParam, &kD3DMat);
            }
        }
        break;
    case NiShaderConstantMap::SCM_DEF_TEXTRANSFORMGLOW:
    case NiShaderConstantMap::SCM_DEF_INVTEXTRANSFORMGLOW:
    case NiShaderConstantMap::SCM_DEF_TEXTRANSFORMGLOW_T:
    case NiShaderConstantMap::SCM_DEF_INVTEXTRANSFORMGLOW_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
            if (pkTexProp)
            {
                const NiTexturingProperty::Map* pkMap = 
                    pkTexProp->GetGlowMap();
                if (pkMap)
                {
                    const NiTextureTransform* pkTextureTransform = 
                        pkMap->GetTextureTransform();

                    if (pkTextureTransform)
                    {
                        pkMatrix = pkTextureTransform->GetMatrix();

                        if ((m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMGLOW) ||
                            (m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMGLOW_T))
                        {
                            bInv = true;
                        }
                        if ((m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_TEXTRANSFORMGLOW_T) ||
                            (m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMGLOW_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            D3DXALIGNEDMATRIX kD3DMat;
            NiD3DShaderConstantMap::SetupTextureTransformMatrix(
                kD3DMat, pkMatrix, bInv, false,
                (bVertexShaderPresent ? 4 : 2));

            if (bTrans)
            {
                pkEffect->SetMatrixTranspose(m_pkParam, &kD3DMat);
            }
            else
            {
                pkEffect->SetMatrix(m_pkParam, &kD3DMat);
            }
        }
        break;
    case NiShaderConstantMap::SCM_DEF_TEXTRANSFORMBUMP:
    case NiShaderConstantMap::SCM_DEF_INVTEXTRANSFORMBUMP:
    case NiShaderConstantMap::SCM_DEF_TEXTRANSFORMBUMP_T:
    case NiShaderConstantMap::SCM_DEF_INVTEXTRANSFORMBUMP_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
            if (pkTexProp)
            {
                const NiTexturingProperty::Map* pkMap = 
                    pkTexProp->GetBumpMap();
                if (pkMap)
                {
                    const NiTextureTransform* pkTextureTransform = 
                        pkMap->GetTextureTransform();

                    if (pkTextureTransform)
                    {
                        pkMatrix = pkTextureTransform->GetMatrix();

                        if ((m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMBUMP) ||
                            (m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMBUMP_T))
                        {
                            bInv = true;
                        }
                        if ((m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_TEXTRANSFORMBUMP_T) ||
                            (m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMBUMP_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            D3DXALIGNEDMATRIX kD3DMat;
            NiD3DShaderConstantMap::SetupTextureTransformMatrix(
                kD3DMat, pkMatrix, bInv, false,
                (bVertexShaderPresent ? 4 : 2));

            if (bTrans)
            {
                pkEffect->SetMatrixTranspose(m_pkParam, &kD3DMat);
            }
            else
            {
                pkEffect->SetMatrix(m_pkParam, &kD3DMat);
            }
        }
        break;
    case NiShaderConstantMap::SCM_DEF_TEXTRANSFORMDECAL:
    case NiShaderConstantMap::SCM_DEF_INVTEXTRANSFORMDECAL:
    case NiShaderConstantMap::SCM_DEF_TEXTRANSFORMDECAL_T:
    case NiShaderConstantMap::SCM_DEF_INVTEXTRANSFORMDECAL_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
            if (pkTexProp)
            {
                const NiTexturingProperty::Map* pkMap = 
                    pkTexProp->GetDecalMap(m_uiExtraData);
                if (pkMap)
                {
                    const NiTextureTransform* pkTextureTransform = 
                        pkMap->GetTextureTransform();

                    if (pkTextureTransform)
                    {
                        pkMatrix = pkTextureTransform->GetMatrix();

                        if ((m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMDECAL) ||
                            (m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMDECAL_T))
                        {
                            bInv = true;
                        }
                        if ((m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_TEXTRANSFORMDECAL_T) ||
                            (m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMDECAL_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            D3DXALIGNEDMATRIX kD3DMat;
            NiD3DShaderConstantMap::SetupTextureTransformMatrix(
                kD3DMat, pkMatrix, bInv, false,
                (bVertexShaderPresent ? 4 : 2));

            if (bTrans)
            {
                pkEffect->SetMatrixTranspose(m_pkParam, &kD3DMat);
            }
            else
            {
                pkEffect->SetMatrix(m_pkParam, &kD3DMat);
            }
        }
        break;
    case NiShaderConstantMap::SCM_DEF_TEXTRANSFORMNORMAL:
    case NiShaderConstantMap::SCM_DEF_INVTEXTRANSFORMNORMAL:
    case NiShaderConstantMap::SCM_DEF_TEXTRANSFORMNORMAL_T:
    case NiShaderConstantMap::SCM_DEF_INVTEXTRANSFORMNORMAL_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
            if (pkTexProp)
            {
                const NiTexturingProperty::Map* pkMap = 
                    pkTexProp->GetNormalMap();
                if (pkMap)
                {
                    const NiTextureTransform* pkTextureTransform = 
                        pkMap->GetTextureTransform();

                    if (pkTextureTransform)
                    {
                        pkMatrix = pkTextureTransform->GetMatrix();

                        if ((m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMNORMAL) ||
                            (m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMNORMAL_T))
                        {
                            bInv = true;
                        }
                        if ((m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_TEXTRANSFORMNORMAL_T) ||
                            (m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMNORMAL_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            D3DXALIGNEDMATRIX kD3DMat;
            NiD3DShaderConstantMap::SetupTextureTransformMatrix(
                kD3DMat, pkMatrix, bInv, false,
                (bVertexShaderPresent ? 4 : 2));

            if (bTrans)
            {
                pkEffect->SetMatrixTranspose(m_pkParam, &kD3DMat);
            }
            else
            {
                pkEffect->SetMatrix(m_pkParam, &kD3DMat);
            }
        }
        break;
    case NiShaderConstantMap::SCM_DEF_TEXTRANSFORMPARALLAX:
    case NiShaderConstantMap::SCM_DEF_INVTEXTRANSFORMPARALLAX:
    case NiShaderConstantMap::SCM_DEF_TEXTRANSFORMPARALLAX_T:
    case NiShaderConstantMap::SCM_DEF_INVTEXTRANSFORMPARALLAX_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
            if (pkTexProp)
            {
                const NiTexturingProperty::Map* pkMap = 
                    pkTexProp->GetParallaxMap();
                if (pkMap)
                {
                    const NiTextureTransform* pkTextureTransform = 
                        pkMap->GetTextureTransform();

                    if (pkTextureTransform)
                    {
                        pkMatrix = pkTextureTransform->GetMatrix();

                        if ((m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMPARALLAX) ||
                            (m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMPARALLAX_T))
                        {
                            bInv = true;
                        }
                        if ((m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_TEXTRANSFORMPARALLAX_T) ||
                            (m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMPARALLAX_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            D3DXALIGNEDMATRIX kD3DMat;
            NiD3DShaderConstantMap::SetupTextureTransformMatrix(
                kD3DMat, pkMatrix, bInv, false,
                (bVertexShaderPresent ? 4 : 2));

            if (bTrans)
            {
                pkEffect->SetMatrixTranspose(m_pkParam, &kD3DMat);
            }
            else
            {
                pkEffect->SetMatrix(m_pkParam, &kD3DMat);
            }
        }
        break;
    case NiShaderConstantMap::SCM_DEF_TEXTRANSFORMSHADER:
    case NiShaderConstantMap::SCM_DEF_INVTEXTRANSFORMSHADER:
    case NiShaderConstantMap::SCM_DEF_TEXTRANSFORMSHADER_T:
    case NiShaderConstantMap::SCM_DEF_INVTEXTRANSFORMSHADER_T:
        {
            const NiMatrix3* pkMatrix = NULL;

            bool bInv = false;
            bool bTrans = false;

            NiTexturingProperty* pkTexProp = pkState->GetTexturing();
            if (pkTexProp)
            {
                NiTexturingProperty::Map* pkShaderMap = 
                    pkTexProp->GetShaderMap(m_uiExtraData);
                if (pkShaderMap)
                {
                    NiTextureTransform* pkTextureTransform = 
                        pkShaderMap->GetTextureTransform();

                    if (pkTextureTransform)
                    {
                        pkMatrix = pkTextureTransform->GetMatrix();

                        if ((m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMSHADER) ||
                            (m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMSHADER_T))
                        {
                            bInv = true;
                        }
                        if ((m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_TEXTRANSFORMSHADER_T) ||
                            (m_ePredefinedMapping == 
                                NiD3DShaderConstantMap::
                                SCM_DEF_INVTEXTRANSFORMSHADER_T))
                        {
                            bTrans = true;
                        }
                    }
                }
            }

            D3DXALIGNEDMATRIX kD3DMat;
            NiD3DShaderConstantMap::SetupTextureTransformMatrix(
                kD3DMat, pkMatrix, bInv, false,
                (bVertexShaderPresent ? 4 : 2));

            if (bTrans)
            {
                pkEffect->SetMatrixTranspose(m_pkParam, &kD3DMat);
            }
            else
            {
                pkEffect->SetMatrix(m_pkParam, &kD3DMat);
            }
        }
        break;
        // Lighting
    case NiShaderConstantMap::SCM_DEF_LIGHT_POS_WS:
    case NiShaderConstantMap::SCM_DEF_LIGHT_DIR_WS:
    case NiShaderConstantMap::SCM_DEF_LIGHT_POS_OS:
    case NiShaderConstantMap::SCM_DEF_LIGHT_DIR_OS:
        break;
        // Materials
    case NiShaderConstantMap::SCM_DEF_MATERIAL_DIFFUSE:
        {
            NiMaterialProperty* pkMaterial = pkState->GetMaterial();
            if (pkMaterial)
            {
                D3DXVECTOR4 kColor;

                kColor.x = pkMaterial->GetDiffuseColor().r;
                kColor.y = pkMaterial->GetDiffuseColor().g;
                kColor.z = pkMaterial->GetDiffuseColor().b;
                kColor.w = pkMaterial->GetAlpha();

                pkEffect->SetVector(m_pkParam, &kColor);
            }
        }
        break;
    case NiShaderConstantMap::SCM_DEF_MATERIAL_AMBIENT:
        {
            NiMaterialProperty* pkMaterial = pkState->GetMaterial();
            if (pkMaterial)
            {
                D3DXVECTOR4 kColor;

                kColor.x = pkMaterial->GetAmbientColor().r;
                kColor.y = pkMaterial->GetAmbientColor().g;
                kColor.z = pkMaterial->GetAmbientColor().b;
                kColor.w = pkMaterial->GetAlpha();

                pkEffect->SetVector(m_pkParam, &kColor);
            }
        }
        break;
    case NiShaderConstantMap::SCM_DEF_MATERIAL_SPECULAR:
        {
            NiMaterialProperty* pkMaterial = pkState->GetMaterial();
            if (pkMaterial)
            {
                D3DXVECTOR4 kColor;

                kColor.x = pkMaterial->GetSpecularColor().r;
                kColor.y = pkMaterial->GetSpecularColor().g;
                kColor.z = pkMaterial->GetSpecularColor().b;
                kColor.w = pkMaterial->GetAlpha();

                pkEffect->SetVector(m_pkParam, &kColor);
            }
        }
        break;
    case NiShaderConstantMap::SCM_DEF_MATERIAL_EMISSIVE:
        {
            NiMaterialProperty* pkMaterial = pkState->GetMaterial();
            if (pkMaterial)
            {
                D3DXVECTOR4 kColor;

                kColor.x = pkMaterial->GetEmittance().r;
                kColor.y = pkMaterial->GetEmittance().g;
                kColor.z = pkMaterial->GetEmittance().b;
                kColor.w = pkMaterial->GetAlpha();

                pkEffect->SetVector(m_pkParam, &kColor);
            }
        }
        break;
    case NiShaderConstantMap::SCM_DEF_MATERIAL_POWER:
        {
            NiMaterialProperty* pkMaterial = pkState->GetMaterial();
            if (pkMaterial)
            {
                float fPower = pkMaterial->GetShineness();

                pkEffect->SetFloat(m_pkParam, fPower);
            }
        }
        break;
        // Eye
    case NiShaderConstantMap::SCM_DEF_EYE_POS:
        {
            D3DMATRIX kViewMat = pkD3DRenderer->GetInvView();
            D3DXVECTOR4 kPos;
            kPos.x = kViewMat._41;
            kPos.y = kViewMat._42;
            kPos.z = kViewMat._43;
            kPos.w = kViewMat._44;

            pkEffect->SetVector(m_pkParam, &kPos);
        }
        break;
    case NiShaderConstantMap::SCM_DEF_EYE_DIR:
        {
            D3DMATRIX kViewMat = pkD3DRenderer->GetInvView();
            D3DXVECTOR4 kDir;
            kDir.x = kViewMat._31;
            kDir.y = kViewMat._32;
            kDir.z = kViewMat._33;
            kDir.w = kViewMat._34;
            pkEffect->SetVector(m_pkParam, &kDir);
        }
        break;
        // Constants
    case NiShaderConstantMap::SCM_DEF_CONSTS_TAYLOR_SIN:
        {
            D3DXVECTOR4 kConst(
                1.0f, -0.16161616f, 0.0083333f, -0.00019841f);
            pkEffect->SetVector(m_pkParam, &kConst);
        }
        break;
    case NiShaderConstantMap::SCM_DEF_CONSTS_TAYLOR_COS:
        {
            D3DXVECTOR4 kConst(
                -0.5f, -0.041666666f, -0.0013888889f, 0.000024801587f);
            pkEffect->SetVector(m_pkParam, &kConst);
        }
        break;
        // Time
    case NiShaderConstantMap::SCM_DEF_CONSTS_TIME:
    case NiShaderConstantMap::SCM_DEF_CONSTS_SINTIME:
    case NiShaderConstantMap::SCM_DEF_CONSTS_COSTIME:
    case NiShaderConstantMap::SCM_DEF_CONSTS_TANTIME:
    case NiShaderConstantMap::SCM_DEF_CONSTS_TIME_SINTIME_COSTIME_TANTIME:
        {
            // Grab the attribute from the geometry and set it
            NiFloatExtraData* pkFloatED = 
                (NiFloatExtraData*)pkMesh->GetExtraData(
                    NiD3DShaderConstantMap::GetTimeExtraDataName());
            if (!pkFloatED)
            {
                // Flag this error
                return false;
            }

            float fTime = pkFloatED->GetValue();

            switch (m_ePredefinedMapping)
            {
            case NiShaderConstantMap::SCM_DEF_CONSTS_TIME:
                pkEffect->SetFloat(m_pkParam, fTime);
                break;
            case NiShaderConstantMap::SCM_DEF_CONSTS_SINTIME:
                pkEffect->SetFloat(m_pkParam, sinf(fTime));
                break;
            case NiShaderConstantMap::SCM_DEF_CONSTS_COSTIME:
                pkEffect->SetFloat(m_pkParam, cosf(fTime));
                break;
            case NiShaderConstantMap::SCM_DEF_CONSTS_TANTIME:
                pkEffect->SetFloat(m_pkParam, tanf(fTime));
                break;
            case NiD3DShaderConstantMap::
                    SCM_DEF_CONSTS_TIME_SINTIME_COSTIME_TANTIME:
                {
                    D3DXVECTOR4 kTime(
                        fTime, sinf(fTime), cosf(fTime), tanf(fTime));
                    pkEffect->SetVector(m_pkParam, &kTime);
                }
                break;
            default:
                NIASSERT(!"Time set --> Invalid case!");
                return false;
            }
        }
        break;
    case NiShaderConstantMap::SCM_DEF_AMBIENTLIGHT:
        {
            D3DXVECTOR4 kAmbient(0.0f, 0.0f, 0.0f, 1.0f);
            if (pkEffects)
            {
                NiDynEffectStateIter kIter = pkEffects->GetLightHeadPos();
                while (kIter)
                {
                    NiAmbientLight* pkLight = NiDynamicCast(NiAmbientLight,
                        pkEffects->GetNextLight(kIter));
                    if (pkLight && pkLight->GetSwitch())
                    {
                        NiColor kColor = pkLight->GetAmbientColor() *
                            pkLight->GetDimmer();
                        kAmbient.x += kColor.r;
                        kAmbient.y += kColor.g;
                        kAmbient.z += kColor.b;
                    }
                }
            }
            pkEffect->SetVector(m_pkParam, &kAmbient);
        }   
        break;
    case NiShaderConstantMap::SCM_DEF_FOG_DENSITY:
        {
            NiFogProperty* pkFog = pkState->GetFog();
            NIASSERT(pkFog);

            float fNear, fFar;
            pkD3DRenderer->GetRenderState()->GetCameraNearAndFar(fNear, fFar);
            float fDensity = 1.0f / (pkFog->GetDepth() * (fFar - fNear));

            D3DXVECTOR4 kDensity(fDensity, fDensity, fDensity, fDensity);
            pkEffect->SetVector(m_pkParam, &kDensity);
        }
        break;
    case NiShaderConstantMap::SCM_DEF_FOG_NEARFAR:
        {
            NiFogProperty* pkFog = pkState->GetFog();
            NIASSERT(pkFog);

            float fNear, fFar;
            pkD3DRenderer->GetRenderState()->GetCameraNearAndFar(fNear, fFar);
            float fCameraDepthRange = fFar - fNear;

            float fWorldDepth = fCameraDepthRange * pkFog->GetDepth();
            float fFogNear = fFar - fWorldDepth;

            float fFogFar = fFar + 
                pkD3DRenderer->GetRenderState()->GetMaxFogFactor() * 
                fWorldDepth;

            D3DXVECTOR4 kNearFar(fFogNear, fFogFar, 0.0f, 0.0f);
            pkEffect->SetVector(m_pkParam, &kNearFar);

        }
        break;
    case NiShaderConstantMap::SCM_DEF_FOG_COLOR:
        {
            NiFogProperty* pkFog = pkState->GetFog();
            NIASSERT(pkFog);
            NiColor kTempColor = pkFog->GetFogColor();
            D3DXVECTOR4 kFogColor(kTempColor.r, kTempColor.g, kTempColor.b, 
                1.0f);
            pkEffect->SetVector(m_pkParam, &kFogColor);
        }
        break;
     case NiShaderConstantMap::SCM_DEF_PARALLAX_OFFSET:
         {
            NiTexturingProperty* pkTexProp = pkState->GetTexturing();
            float fOffset = 0.0f;
            if (pkTexProp)
            {
                NiTexturingProperty::ParallaxMap* pkParallaxMap = 
                    pkTexProp->GetParallaxMap();
                if (pkParallaxMap)
                {
                    fOffset = pkParallaxMap->GetOffset();
                }
            }
            pkEffect->SetFloat(m_pkParam, fOffset);
        }
        break;
    case NiShaderConstantMap::SCM_DEF_BUMP_MATRIX:
         {
            NiTexturingProperty* pkTexProp = pkState->GetTexturing();
            float afMatrix[4];
            afMatrix[0] = 1.0f;
            afMatrix[1] = 1.0f;
            afMatrix[2] = 1.0f;
            afMatrix[3] = 1.0f;

            if (pkTexProp)
            {
                NiTexturingProperty::BumpMap* pkBumpMap = 
                    pkTexProp->GetBumpMap();
                if (pkBumpMap)
                {
                    afMatrix[0] = pkBumpMap->GetBumpMat00();
                    afMatrix[1] = pkBumpMap->GetBumpMat01();
                    afMatrix[2] = pkBumpMap->GetBumpMat10();
                    afMatrix[3] = pkBumpMap->GetBumpMat11();
                }
            }
            D3DXVECTOR4 kVec(afMatrix[0], afMatrix[1], afMatrix[2], 
                afMatrix[3]);
            pkEffect->SetVector(m_pkParam, &kVec);
        }
        break;
    case NiShaderConstantMap::SCM_DEF_BUMP_LUMA_OFFSET_AND_SCALE:
         {
            NiTexturingProperty* pkTexProp = pkState->GetTexturing();
            D3DXVECTOR4 kVec(0.0f, 1.0f, 0.0f, 0.0f);
            if (pkTexProp)
            {
                NiTexturingProperty::BumpMap* pkBumpMap = 
                    pkTexProp->GetBumpMap();
                if (pkBumpMap)
                {
                    kVec.x = pkBumpMap->GetLumaOffset();
                    kVec.y = pkBumpMap->GetLumaScale();
                }
            }
            pkEffect->SetVector(m_pkParam, &kVec);
        }
        break;
    case NiShaderConstantMap::SCM_DEF_TEXSIZEBASE:
    case NiShaderConstantMap::SCM_DEF_TEXSIZEDARK:
    case NiShaderConstantMap::SCM_DEF_TEXSIZEDETAIL:
    case NiShaderConstantMap::SCM_DEF_TEXSIZEGLOSS:
    case NiShaderConstantMap::SCM_DEF_TEXSIZEGLOW:
    case NiShaderConstantMap::SCM_DEF_TEXSIZEBUMP:
    case NiShaderConstantMap::SCM_DEF_TEXSIZENORMAL:
    case NiShaderConstantMap::SCM_DEF_TEXSIZEPARALLAX:
    case NiShaderConstantMap::SCM_DEF_TEXSIZEDECAL:
    case NiShaderConstantMap::SCM_DEF_TEXSIZESHADER:
        {
            const NiTexturingProperty* pkTexProp = pkState->GetTexturing();
            if (pkTexProp)
            {
                const NiTexturingProperty::Map* pkMap = NULL;

                switch (m_ePredefinedMapping)
                {
                    case NiShaderConstantMap::SCM_DEF_TEXSIZEBASE:
                        pkMap = pkTexProp->GetBaseMap();
                        break;
                    case NiShaderConstantMap::SCM_DEF_TEXSIZEDARK:
                        pkMap = pkTexProp->GetDarkMap();
                        break;
                    case NiShaderConstantMap::SCM_DEF_TEXSIZEDETAIL:
                        pkMap = pkTexProp->GetDetailMap();
                        break;
                    case NiShaderConstantMap::SCM_DEF_TEXSIZEGLOSS:
                        pkMap = pkTexProp->GetGlossMap();
                        break;
                    case NiShaderConstantMap::SCM_DEF_TEXSIZEGLOW:
                        pkMap = pkTexProp->GetGlowMap();
                        break;
                    case NiShaderConstantMap::SCM_DEF_TEXSIZEBUMP:
                        pkMap = pkTexProp->GetBumpMap();
                        break;
                    case NiShaderConstantMap::SCM_DEF_TEXSIZENORMAL:
                        pkMap = pkTexProp->GetNormalMap();
                        break;
                    case NiShaderConstantMap::SCM_DEF_TEXSIZEPARALLAX:
                        pkMap = pkTexProp->GetParallaxMap();
                        break;
                    case NiShaderConstantMap::SCM_DEF_TEXSIZEDECAL:
                        pkMap = pkTexProp->GetDecalMap(m_uiExtraData);
                        break;
                    case NiShaderConstantMap::SCM_DEF_TEXSIZESHADER:
                        pkMap = pkTexProp->GetShaderMap(m_uiExtraData);
                        break;
                }
                    
                D3DXVECTOR4 kVec(0.0f, 0.0f, 0.0f, 0.0f);
                if (pkMap && pkMap->GetTexture())
                {
                    NiTexture* pkTex = pkMap->GetTexture();
                    kVec.x = (float) pkTex->GetWidth();
                    kVec.y = (float) pkTex->GetHeight();
                }
                pkEffect->SetVector(m_pkParam, &kVec);
            }
        }
        break;
    case NiShaderConstantMap::SCM_DEF_ALPHA_TEST_FUNC:
        {
            NiAlphaProperty* pkAlpha = pkState->GetAlpha();
            NIASSERT(pkAlpha);
            unsigned int uiTestFunc = (unsigned int)pkAlpha->GetTestMode();
            pkEffect->SetInt(m_pkParam, uiTestFunc);
        }
        break;
    case NiShaderConstantMap::SCM_DEF_ALPHA_TEST_REF:
        {
            NiAlphaProperty* pkAlpha = pkState->GetAlpha();
            NIASSERT(pkAlpha);
            float fTestRef = (float)pkAlpha->GetTestMode() / 255.0f;
            pkEffect->SetFloat(m_pkParam, fTestRef);
        }
        break;

    case NiShaderConstantMap::SCM_DEF_XENON_INST_FREQUENCY:
        {
            float fFreq = (float) 
                NiInstancingUtilities::GetInstanceIndexFrequency(pkMesh,
                kRCC.m_uiSubmesh);

            pkEffect->SetFloat(m_pkParam, fFreq);
        }
        break;

    // Deprecated shader constant map entries
    case NiShaderConstantMap::SCM_DEF_SKINWORLDVIEW:
    case NiShaderConstantMap::SCM_DEF_INVSKINWORLDVIEW:
    case NiShaderConstantMap::SCM_DEF_SKINWORLDVIEW_T:
    case NiShaderConstantMap::SCM_DEF_INVSKINWORLDVIEW_T:

    case NiShaderConstantMap::SCM_DEF_SKINWORLDVIEWPROJ:
    case NiShaderConstantMap::SCM_DEF_INVSKINWORLDVIEWPROJ:
    case NiShaderConstantMap::SCM_DEF_SKINWORLDVIEWPROJ_T:
    case NiShaderConstantMap::SCM_DEF_INVSKINWORLDVIEWPROJ_T:

    case NiShaderConstantMap::SCM_DEF_SKINWORLD:
    case NiShaderConstantMap::SCM_DEF_INVSKINWORLD:
    case NiShaderConstantMap::SCM_DEF_SKINWORLD_T:
    case NiShaderConstantMap::SCM_DEF_INVSKINWORLD_T:

    case NiShaderConstantMap::SCM_DEF_BONE_MATRIX_3:
    case NiShaderConstantMap::SCM_DEF_BONE_MATRIX_4:
    case NiShaderConstantMap::SCM_DEF_SKINBONE_MATRIX_4:
        {
            NiFixedString kName;
            NIVERIFY(NiShaderConstantMap::LookUpPredefinedMappingName(
                m_ePredefinedMapping, kName));
            NILOG("The predefined mapping %s is deprecated\n",
                (const char*)kName);
        }
        return false;
        break;

    default:
        {
            NiFixedString kName;
            NIVERIFY(NiShaderConstantMap::LookUpPredefinedMappingName(
                m_ePredefinedMapping, kName));
            NILOG("The predefined mapping %s is not supported.\n",
                (const char*)kName);
        }
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
NiD3DError NiD3DXEffectParameter::FillObjectConstantValue(
    const NiRenderCallContext& kRCC, 
    LPD3DXEFFECT pkD3DXEffect)
{
    const NiDynamicEffectState* pkEffects = kRCC.m_pkEffects;

    NiD3DError eResult = NISHADERERR_OK;

    // Get NiDynamicEffect corresponding to this object type.
    NiDynamicEffect* pkDynEffect = 
        NiD3DShaderConstantMap::GetDynamicEffectForObject(
        pkEffects, m_eObjectType, m_uiExtraData);

    // Get the register count for the mapping type.
    unsigned int uiRegCount, uiFloatCount;
    NiShaderAttributeDesc::AttributeType eType =
        NiD3DShaderConstantMap::LookUpObjectMappingType(m_eObjectMapping,
        uiRegCount, uiFloatCount);
    if (eType == NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED)
    {
        return NISHADERERR_ENTRYNOTFOUND;
    }

    // uiRegCount now contains the register count. Each register is 4 floats.
    unsigned int uiArraySize = uiRegCount * 4;
    NIASSERT(uiArraySize <= 16);

    // Get data to set.
    if (!NiD3DShaderConstantMap::GetDynamicEffectData(
        (void*) &ms_afObjectData,
        uiArraySize * sizeof(float), 
        m_eObjectMapping, 
        pkDynEffect,
        kRCC))
    {
        eResult = NISHADERERR_DYNEFFECTNOTFOUND;
    }

    // Set constant data.
    switch (uiFloatCount)
    {
        case 1:     // Single float.
        {
            pkD3DXEffect->SetFloat(m_pkParam, ms_afObjectData[0]);
            break;
        }
        case 4:     // float4 vector.
        {
            D3DXVECTOR4 kVector;
            kVector.x = ms_afObjectData[0];
            kVector.y = ms_afObjectData[1];
            kVector.z = ms_afObjectData[2];
            kVector.w = ms_afObjectData[3];
            pkD3DXEffect->SetVector(m_pkParam, &kVector);
            break;
        }
        case 12:    // float3x3 matrix.
        case 16:    // float4x4 matrix.
        {
            D3DXALIGNEDMATRIX kMatrix;
            kMatrix._11 = ms_afObjectData[0];
            kMatrix._12 = ms_afObjectData[1];
            kMatrix._13 = ms_afObjectData[2];
            kMatrix._14 = ms_afObjectData[3];
            kMatrix._21 = ms_afObjectData[4];
            kMatrix._22 = ms_afObjectData[5];
            kMatrix._23 = ms_afObjectData[6];
            kMatrix._24 = ms_afObjectData[7];
            kMatrix._31 = ms_afObjectData[8];
            kMatrix._32 = ms_afObjectData[9];
            kMatrix._33 = ms_afObjectData[10];
            kMatrix._34 = ms_afObjectData[11];
            kMatrix._41 = ms_afObjectData[12];
            kMatrix._42 = ms_afObjectData[13];
            kMatrix._43 = ms_afObjectData[14];
            kMatrix._44 = ms_afObjectData[15];
            pkD3DXEffect->SetMatrix(m_pkParam, &kMatrix);
            break;
        }
        default:
        {
            eResult = NISHADERERR_SETCONSTANTFAILED;
        }
    }

    return eResult;
}
//---------------------------------------------------------------------------
D3DBaseTexturePtr NiD3DXEffectParameter::GetNiTexture(
    NiTexturingProperty* pkTexProp, unsigned int uiTextureFlags)
{
    const NiTexturingProperty::Map* pkMap = NULL;

    if ((uiTextureFlags & NiD3DTextureStage::TSTF_MAP_MASK) == 0)
    {
        switch (uiTextureFlags & NiD3DTextureStage::TSTF_NDL_TYPEMASK)
        {
        case NiD3DTextureStage::TSTF_NONE:
            break;
        case NiD3DTextureStage::TSTF_NDL_BASE:
            pkMap = pkTexProp->GetBaseMap();
            break;
        case NiD3DTextureStage::TSTF_NDL_DARK:
            pkMap = pkTexProp->GetDarkMap();
            break;
        case NiD3DTextureStage::TSTF_NDL_DETAIL:
            pkMap = pkTexProp->GetDetailMap();
            break;
        case NiD3DTextureStage::TSTF_NDL_GLOSS:
            pkMap = pkTexProp->GetGlossMap();
            break;
        case NiD3DTextureStage::TSTF_NDL_GLOW:
            pkMap = pkTexProp->GetGlowMap();
            break;
        case NiD3DTextureStage::TSTF_NDL_BUMP:
            pkMap = pkTexProp->GetBumpMap();
            break;
        case NiD3DTextureStage::TSTF_NDL_NORMAL:
            pkMap = pkTexProp->GetNormalMap();
            break;
        case NiD3DTextureStage::TSTF_NDL_PARALLAX:
            pkMap = pkTexProp->GetParallaxMap();
            break;
        }
    }
    else if ((uiTextureFlags & NiD3DTextureStage::TSTF_MAP_MASK) == 
        NiD3DTextureStage::TSTF_MAP_DECAL)
    {
        unsigned int uiIndex = 
            uiTextureFlags & NiD3DTextureStage::TSTF_INDEX_MASK;
        if (uiIndex < pkTexProp->GetDecalArrayCount())
            pkMap = pkTexProp->GetDecalMap(uiIndex);
    }
    else if ((uiTextureFlags & NiD3DTextureStage::TSTF_MAP_MASK) == 
        NiD3DTextureStage::TSTF_MAP_SHADER)
    {
        unsigned int uiIndex = 
            uiTextureFlags & NiD3DTextureStage::TSTF_INDEX_MASK;
        if (uiIndex < pkTexProp->GetShaderArrayCount())
        {
            pkMap = pkTexProp->GetShaderMap(uiIndex);
            // Ignore texture map, since 
        }
    }

    if (!pkMap)
        return NULL;

    NiTexture* pkTexture = pkMap->GetTexture();
    if (!pkTexture)
        return NULL;

    // This comes from NiD3DTextureStage::ApplyTexture
#if !defined(_XENON)
    bool bS0Mipmap = false;
#endif
    bool bChanged; 
#if !defined(_XENON)
    bool bNonPow2 = false;
#endif
    NiD3DRenderer* pkD3DRenderer = (NiD3DRenderer*)NiRenderer::GetRenderer();
    D3DBaseTexturePtr pkD3DTexture = pkD3DRenderer->GetTextureManager()->
        PrepareTextureForRendering(pkTexture, bChanged
#if defined(_XENON)
        );
#else
        , bS0Mipmap, bNonPow2);
#endif

    return pkD3DTexture;
}
//---------------------------------------------------------------------------
void NiD3DXEffectParameter::Construct2DTexture()
{
    NIASSERT(m_eTextureType == NiD3DXEffectParameterTextureDesc::TT_2D);

    NiD3DRenderer* pkD3DRenderer = (NiD3DRenderer*)NiRenderer::GetRenderer();
    NIASSERT(pkD3DRenderer);
    D3DDevicePtr pkD3DDevice = pkD3DRenderer->GetD3DDevice();
    NIASSERT(pkD3DDevice);

    HRESULT hr = 0;
    D3DTexturePtr pk2DTex = NULL;
    if (m_pcTextureTarget != NULL)
    {
        // Currently do not procedurally generate texture
        return;
    }
    else if (m_pcTextureSource != NULL)
    {
        hr = D3DXCreateTextureFromFileEx(pkD3DDevice, m_pcTextureSource, 
            m_uiWidth, m_uiHeight, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, 
            D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, 
            &pk2DTex);
    }
    else
    {
        NIASSERT(false);
    }

    if (SUCCEEDED(hr))
        m_pkTexture = pk2DTex;
}
//---------------------------------------------------------------------------
void NiD3DXEffectParameter::Construct3DTexture()
{
    NIASSERT(m_eTextureType == NiD3DXEffectParameterTextureDesc::TT_3D);

    NiD3DRenderer* pkD3DRenderer = (NiD3DRenderer*)NiRenderer::GetRenderer();
    NIASSERT(pkD3DRenderer);
    D3DDevicePtr pkD3DDevice = pkD3DRenderer->GetD3DDevice();
    NIASSERT(pkD3DDevice);

    HRESULT hr = 0;
    LPDIRECT3DVOLUMETEXTURE9 pk3DTex = NULL;
    if (m_pcTextureTarget != NULL)
    {
        // Currently do not procedurally generate texture
        return;
    }
    else if (m_pcTextureSource != NULL)
    {
        // Load texture file
        hr = D3DXCreateVolumeTextureFromFileEx(pkD3DDevice, m_pcTextureSource, 
            m_uiWidth, m_uiHeight, m_uiDepth, 1, 0, D3DFMT_UNKNOWN, 
            D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, 
            &pk3DTex);
    }
    else
    {
        NIASSERT(false);
    }

    if (SUCCEEDED(hr))
        m_pkTexture = pk3DTex;
}
//---------------------------------------------------------------------------
void NiD3DXEffectParameter::ConstructCubeTexture()
{
    NIASSERT(m_eTextureType == NiD3DXEffectParameterTextureDesc::TT_CUBE);

    NiD3DRenderer* pkD3DRenderer = (NiD3DRenderer*)NiRenderer::GetRenderer();
    NIASSERT(pkD3DRenderer);
    D3DDevicePtr pkD3DDevice = pkD3DRenderer->GetD3DDevice();
    NIASSERT(pkD3DDevice);

    HRESULT hr = 0;
    LPDIRECT3DCUBETEXTURE9 pkCubeTex = NULL;
    if (m_pcTextureTarget != NULL)
    {
        // Currently do not procedurally generate texture
        return;
    }
    else if (m_pcTextureSource != NULL)
    {
        // Load texture file
        hr = D3DXCreateCubeTextureFromFileEx(pkD3DDevice, m_pcTextureSource, 
            m_uiWidth, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 
            D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pkCubeTex);
    }
    else
    {
        NIASSERT(false);
    }

    if (SUCCEEDED(hr))
        m_pkTexture = pkCubeTex;
}
//---------------------------------------------------------------------------
