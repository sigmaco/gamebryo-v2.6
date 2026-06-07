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

#include "NiD3D10EffectParameter.h"
#include "NiD3D10EffectAttributeTable.h"
#include "NiD3D10EffectParameterDesc.h"

#include <NiD3D10Pass.h>
#include <NiD3D10Shader.h>
#include <NiD3D10TextureData.h>
#include <NiRenderer.h>
#include <NiShaderFactory.h>

D3DXVECTOR4 NiD3D10EffectParameter::ms_akVector4Array[
    NiD3D10EffectParameterArrayDesc::MAX_ROWS];

D3DXMATRIXA16* NiD3D10EffectParameter::ms_pkMatrixArray = NULL;
unsigned int NiD3D10EffectParameter::ms_uiMatrixArraySize = 0;
float NiD3D10EffectParameter::ms_afObjectData[16];

//---------------------------------------------------------------------------
NiD3D10EffectParameter::NiD3D10EffectParameter() :
    m_pkParam(NULL),
    m_pkTexture(NULL),
    m_pkEffectSRV(NULL),
    m_pkTextureDataRV(NULL),
    m_ePredefinedMapping(NiShaderConstantMap::SCM_DEF_INVALID),
    m_eObjectMapping(NiShaderConstantMap::SCM_OBJ_INVALID),
    m_eParameterType(NiD3D10EffectParameterDesc::PT_NONE),
    m_uiParameterCount(0),
    m_uiArrayCount(0),
    m_uiFlags(0),
    m_uiExtraData(0),
    m_uiTextureFlags(NiD3D10Pass::GB_MAP_IGNORE),
    m_bGlobal(false),
    m_eObjectType(NiShaderAttributeDesc::OT_UNDEFINED)
{ /* */ }
//---------------------------------------------------------------------------
NiD3D10EffectParameter::~NiD3D10EffectParameter()
{
    DestroyRendererData();

    if (m_bGlobal)
        NiShaderFactory::ReleaseGlobalShaderConstant(m_kName);
}
//---------------------------------------------------------------------------
bool NiD3D10EffectParameter::Initialize(
    NiD3D10EffectParameterDesc* pkParamDesc, 
    NiD3D10EffectAttributeTable* pkAttribTable)
{
    if (pkParamDesc == NULL)
        return false;
    NIASSERT(pkAttribTable);

    m_kConstantBufferName = pkParamDesc->m_kConstantBuffer;
    m_uiFlags = pkParamDesc->m_uiFlags;

    m_eParameterType = pkParamDesc->m_eParamType;
    m_bGlobal = pkParamDesc->m_bGlobal;

    switch (m_eParameterType)
    {
    case NiD3D10EffectParameterDesc::PT_MATRIX:
    {
        NiD3D10EffectParameterMatrixDesc* pkMatrixDesc = 
            (NiD3D10EffectParameterMatrixDesc*)pkParamDesc;

        m_kName = pkParamDesc->m_kName;

        NiUInt32 uiWidth = pkMatrixDesc->m_uiNumCols;
        NiUInt32 uiHeight = pkMatrixDesc->m_uiNumRows;
        m_uiParameterCount = uiWidth * uiHeight;

        if (pkMatrixDesc->m_uiNumRows == 3 && 
            pkMatrixDesc->m_uiNumCols == 3)
        {
            if (m_bGlobal)
            {
                NiShaderFactory::RegisterGlobalShaderConstant(m_kName, 
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
                NiShaderFactory::RegisterGlobalShaderConstant(m_kName, 
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
    case NiD3D10EffectParameterDesc::PT_VECTOR:
    {
        NiD3D10EffectParameterVectorDesc* pkVectorDesc = 
            (NiD3D10EffectParameterVectorDesc*)pkParamDesc;

        m_kName =  pkParamDesc->m_kName;

        NiD3D10EffectParameterVectorDesc::VectorType eVectorType =
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
                NiD3D10EffectParameterVectorDesc::TYPE_POINT3 || 
                eVectorType == 
                NiD3D10EffectParameterVectorDesc::TYPE_POINT4);

            if (m_bGlobal)
            {
                NiShaderFactory::RegisterGlobalShaderConstant(m_kName, 
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
                NiD3D10EffectParameterVectorDesc::TYPE_POINT2 || 
                eVectorType == 
                NiD3D10EffectParameterVectorDesc::TYPE_POINT3 || 
                eVectorType == 
                NiD3D10EffectParameterVectorDesc::TYPE_POINT4);

            if (eVectorType == 
                NiD3D10EffectParameterVectorDesc::TYPE_POINT2)
            {
                if (m_bGlobal)
                {
                    NiShaderFactory::RegisterGlobalShaderConstant(
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
                NiD3D10EffectParameterVectorDesc::TYPE_POINT3)
            {
                if (m_bGlobal)
                {
                    NiShaderFactory::RegisterGlobalShaderConstant(
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
                NiD3D10EffectParameterVectorDesc::TYPE_POINT4)
            {
                if (m_bGlobal)
                {
                    NiShaderFactory::RegisterGlobalShaderConstant(
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
    case NiD3D10EffectParameterDesc::PT_FLOAT:
    {
        NiD3D10EffectParameterFloatDesc* pkFloatDesc = 
            (NiD3D10EffectParameterFloatDesc*)pkParamDesc;

        m_kName = pkParamDesc->m_kName;

        m_uiParameterCount = 1;

        if (m_bGlobal)
        {
            NiShaderFactory::RegisterGlobalShaderConstant(m_kName, 
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
    case NiD3D10EffectParameterDesc::PT_BOOL:
    {
        NiD3D10EffectParameterBooleanDesc* pkBooleanDesc = 
            (NiD3D10EffectParameterBooleanDesc*)pkParamDesc;

        m_kName = pkParamDesc->m_kName;

        m_uiParameterCount = 1;

        if (m_bGlobal)
        {
            NiShaderFactory::RegisterGlobalShaderConstant(m_kName, 
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
    case NiD3D10EffectParameterDesc::PT_UINT:
    {
        NiD3D10EffectParameterIntegerDesc* pkIntegerDesc = 
            (NiD3D10EffectParameterIntegerDesc*)pkParamDesc;

        m_kName = pkParamDesc->m_kName;

        m_uiParameterCount = 1;

        if (m_bGlobal)
        {
            NiShaderFactory::RegisterGlobalShaderConstant(m_kName, 
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
    case NiD3D10EffectParameterDesc::PT_TEXTURE:
    {
        NiD3D10EffectParameterTextureDesc* pkTextureDesc = 
            (NiD3D10EffectParameterTextureDesc*)pkParamDesc;

        m_kName = pkParamDesc->m_kName;

        m_uiParameterCount = 1;

        if (m_bGlobal)
        {
            NiShaderFactory::RegisterGlobalShaderConstant(m_kName, 
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
        if (pkTextureDesc->m_kTextureMap.Exists())
        {
            if (pkTextureDesc->m_kTextureMap.EqualsNoCase("base"))
                m_uiTextureFlags = NiD3D10Pass::GB_MAP_BASE;
            else if (pkTextureDesc->m_kTextureMap.EqualsNoCase("dark"))
                m_uiTextureFlags = NiD3D10Pass::GB_MAP_DARK;
            else if (pkTextureDesc->m_kTextureMap.EqualsNoCase("detail"))
                m_uiTextureFlags = NiD3D10Pass::GB_MAP_DETAIL;
            else if (pkTextureDesc->m_kTextureMap.EqualsNoCase("gloss"))
                m_uiTextureFlags = NiD3D10Pass::GB_MAP_GLOSS;
            else if (pkTextureDesc->m_kTextureMap.EqualsNoCase("glow"))
                m_uiTextureFlags = NiD3D10Pass::GB_MAP_GLOW;
            else if (pkTextureDesc->m_kTextureMap.EqualsNoCase("bump"))
                m_uiTextureFlags = NiD3D10Pass::GB_MAP_BUMP;
            else if (pkTextureDesc->m_kTextureMap.EqualsNoCase("normal"))
                m_uiTextureFlags = NiD3D10Pass::GB_MAP_NORMAL;
            else if (pkTextureDesc->m_kTextureMap.EqualsNoCase("parallax"))
                m_uiTextureFlags = NiD3D10Pass::GB_MAP_PARALLAX;
            else if (pkTextureDesc->m_kTextureMap.EqualsNoCase("decal"))
            {
                if (pkTextureDesc->m_uiTextureIndex < 0)
                    pkTextureDesc->m_uiTextureIndex = 0;
                m_uiTextureFlags = NiD3D10Pass::GB_MAP_DECAL | 
                    pkTextureDesc->m_uiTextureIndex;
            }
            else // shader maps
            {
                m_uiTextureFlags = NiD3D10Pass::GB_MAP_SHADER | 
                    pkTextureDesc->m_uiTextureIndex;

                // Only add shader map attributes
                pkAttribTable->AddAttribDesc_Texture(m_kName, 
                    pkTextureDesc->m_bHidden, 
                    pkTextureDesc->m_uiTextureIndex, 
                    pkTextureDesc->m_kTextureName);
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
        if (pkTextureDesc->m_kTextureName.Exists())
        {
            m_kTextureSource = pkTextureDesc->m_kTextureName;

            return true;
        }

        // Procedural textures don't exist in D3D10.
    }
    case NiD3D10EffectParameterDesc::PT_PREDEFINED:
    {
        NiD3D10EffectParameterPredefinedDesc* pkPredefinedDesc = 
            (NiD3D10EffectParameterPredefinedDesc*)pkParamDesc;

        m_kName = pkParamDesc->m_kName;
        m_kPredefined = pkPredefinedDesc->m_kPredefined;

        m_uiParameterCount = 1;
        m_uiArrayCount = pkPredefinedDesc->m_uiArrayCount;

        m_ePredefinedMapping = (NiShaderConstantMap::DefinedMappings)
            pkPredefinedDesc->m_uiMapping;

        m_uiExtraData = pkPredefinedDesc->m_uiExtraData;
        // Encode bone count if necessary
        if (m_ePredefinedMapping == 
            NiShaderConstantMap::SCM_DEF_SKINBONE_MATRIX_3)
        {
            m_uiExtraData |= m_uiArrayCount << 16;
        }
        // Other bone predefines are deprecated...
        NIASSERT(
            m_ePredefinedMapping != 
            NiShaderConstantMap::SCM_DEF_BONE_MATRIX_3 &&
            m_ePredefinedMapping != 
            NiShaderConstantMap::SCM_DEF_BONE_MATRIX_4 &&
            m_ePredefinedMapping != 
            NiShaderConstantMap::SCM_DEF_SKINBONE_MATRIX_4);
        return true;
    }
    case NiD3D10EffectParameterDesc::PT_OBJECT:
    {
        NiD3D10EffectParameterObjectDesc* pkObjectDesc =
            (NiD3D10EffectParameterObjectDesc*) pkParamDesc;

        m_kName = pkParamDesc->m_kName;

        m_uiParameterCount = 1;

        m_eObjectMapping = (NiShaderConstantMap::ObjectMappings)
            pkObjectDesc->m_uiMapping;
        m_eObjectType = pkObjectDesc->m_eObjectType;
        m_uiExtraData = pkObjectDesc->m_uiObjectIndex;

        return true;
    }
    case NiD3D10EffectParameterDesc::PT_ARRAY:
    {
        NiD3D10EffectParameterArrayDesc* pkArrayDesc = 
            (NiD3D10EffectParameterArrayDesc*)pkParamDesc;

        m_kName = pkParamDesc->m_kName;
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
            NiShaderFactory::RegisterGlobalShaderConstant(
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
                case NiD3D10EffectParameterArrayDesc::TYPE_FLOAT:
                    eSubType = NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
                    break;
                case NiD3D10EffectParameterArrayDesc::TYPE_POINT2:
                    eSubType = NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
                    break;
                case NiD3D10EffectParameterArrayDesc::TYPE_POINT3:
                    eSubType = NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;
                    break;
                case NiD3D10EffectParameterArrayDesc::TYPE_POINT4:
                    eSubType = NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
                    break;
                case NiD3D10EffectParameterArrayDesc::TYPE_COLORA:
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
    }
    return false;
}
//---------------------------------------------------------------------------
NiShaderError NiD3D10EffectParameter::SetParameter(
    const NiRenderCallContext& kRCC)
{
    NIASSERT(m_pkParam);

    // Only textures are set here!
    if (m_eParameterType != NiD3D10EffectParameterDesc::PT_TEXTURE)
    {
        // Skip out - not an error
        return NISHADERERR_OK;
    }

 
    if (m_pkEffectSRV == NULL)
    {
        m_pkEffectSRV = m_pkParam->AsShaderResource();
        NIASSERT(m_pkEffectSRV->IsValid());
    }
    
    // Check for Gamebryo texture
    NiTexturingProperty::ClampMode eClampMode; 
    NiTexturingProperty::FilterMode eFilterMode;
    unsigned short usMaxAnisotropy = 1;

    // Save the last texture.
    NiTexture* pkLastTexture = m_pkTexture;
                                  
    // Check for Gamebryo texture
    NiD3D10Shader::ObtainTexture(
        m_uiTextureFlags, 

        (NiUInt16)m_eObjectType,
        kRCC,
        m_pkTexture,
        eClampMode,
        eFilterMode,
        usMaxAnisotropy);

    //if the texture is new or changed get new m_pkTextureDataRV.
    if (m_pkTexture && (pkLastTexture == NULL || 
        m_pkTexture != pkLastTexture))
    {
        // This comes from NiD3D10Pass::ApplyTextures
        NiD3D10TextureData* pkTextureData = (NiD3D10TextureData*)
            m_pkTexture->GetRendererData();
        if (pkTextureData == NULL)
        {
            // Pack texture if necessary
            NiRenderer::GetRenderer()->PrecacheTexture(m_pkTexture);
            pkTextureData = (NiD3D10TextureData*)
                m_pkTexture->GetRendererData();
        }

        if (pkTextureData)
            m_pkTextureDataRV = pkTextureData->GetResourceView();
    }

    // whether the texture changed or not we can now set the 
    // resource view of the parameter from the texture resource
    // view.
    if (m_pkTextureDataRV)
    {
        m_pkEffectSRV->SetResource(m_pkTextureDataRV);
        return NISHADERERR_OK;
    }

    return NISHADERERR_ENTRYNOTFOUND;
}
//---------------------------------------------------------------------------
void NiD3D10EffectParameter::DestroyRendererData()
{
    m_pkParam = NULL;
    m_pkTexture = NULL;
    m_pkEffectSRV = NULL;
    m_pkTextureDataRV = NULL;
}
//---------------------------------------------------------------------------
const char* NiD3D10EffectParameter::GetKey() const
{
    NiFixedString kKey;
    if (m_kPredefined.Exists())
    {
        kKey = m_kPredefined;
    }
    else if (m_eObjectMapping != NiShaderConstantMap::SCM_OBJ_INVALID &&
        NiShaderConstantMap::LookUpObjectMappingName(
        m_eObjectMapping, kKey))
    {
        /* kKey already = kKey */
    }
    else
    {
        kKey = m_kName;
    }

    return (const char*)kKey;
}
//---------------------------------------------------------------------------
NiUInt32 NiD3D10EffectParameter::GetFlags() const
{
    return m_uiFlags;
}
//---------------------------------------------------------------------------
NiUInt32 NiD3D10EffectParameter::GetExtra() const
{
    return m_uiExtraData;
}
//---------------------------------------------------------------------------
void NiD3D10EffectParameter::ReleaseBoneArray()
{
    NiDelete[] ms_pkMatrixArray;
    ms_pkMatrixArray = NULL;
}
//---------------------------------------------------------------------------
