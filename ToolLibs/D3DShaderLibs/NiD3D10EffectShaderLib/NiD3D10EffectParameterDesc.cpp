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

#include "NiD3D10EffectParameterDesc.h"
#include <NiRTLib.h>

NiD3D10EffectParameterMatrixDesc NiD3D10EffectParameterDesc::ms_kMatrixDesc;
NiD3D10EffectParameterVectorDesc NiD3D10EffectParameterDesc::ms_kVectorDesc;
NiD3D10EffectParameterFloatDesc NiD3D10EffectParameterDesc::ms_kFloatDesc;
NiD3D10EffectParameterBooleanDesc NiD3D10EffectParameterDesc::ms_kBooleanDesc;
NiD3D10EffectParameterIntegerDesc NiD3D10EffectParameterDesc::ms_kIntegerDesc;
NiD3D10EffectParameterTextureDesc NiD3D10EffectParameterDesc::ms_kTextureDesc;
NiD3D10EffectParameterPredefinedDesc
    NiD3D10EffectParameterDesc::ms_kPredefinedDesc;
NiD3D10EffectParameterObjectDesc NiD3D10EffectParameterDesc::ms_kObjectDesc;
NiD3D10EffectParameterArrayDesc NiD3D10EffectParameterDesc::ms_kArrayDesc;

//---------------------------------------------------------------------------
NiD3D10EffectParameterMatrixDesc* NiD3D10EffectParameterDesc::GetMatrixDesc()
{
    ms_kMatrixDesc.m_kName = NULL;
    ms_kMatrixDesc.m_kConstantBuffer = NULL;
    ms_kMatrixDesc.m_kDesc = NULL;
    memset(&ms_kMatrixDesc, 0, sizeof(ms_kMatrixDesc));
    ms_kMatrixDesc.m_eParamType = PT_MATRIX;
    return &ms_kMatrixDesc;
}
//---------------------------------------------------------------------------
NiD3D10EffectParameterVectorDesc* NiD3D10EffectParameterDesc::GetVectorDesc()
{
    ms_kVectorDesc.m_kName = NULL;
    ms_kVectorDesc.m_kConstantBuffer = NULL;
    ms_kVectorDesc.m_kDesc = NULL;
    memset(&ms_kVectorDesc, 0, sizeof(ms_kVectorDesc));
    ms_kVectorDesc.m_eParamType = PT_VECTOR;
    return &ms_kVectorDesc;
}
//---------------------------------------------------------------------------
NiD3D10EffectParameterFloatDesc* NiD3D10EffectParameterDesc::GetFloatDesc()
{
    ms_kFloatDesc.m_kName = NULL;
    ms_kFloatDesc.m_kConstantBuffer = NULL;
    ms_kFloatDesc.m_kDesc = NULL;
    memset(&ms_kFloatDesc, 0, sizeof(ms_kFloatDesc));
    ms_kFloatDesc.m_eParamType = PT_FLOAT;
    return &ms_kFloatDesc;
}
//---------------------------------------------------------------------------
NiD3D10EffectParameterBooleanDesc* NiD3D10EffectParameterDesc::GetBooleanDesc()
{
    ms_kBooleanDesc.m_kName = NULL;
    ms_kBooleanDesc.m_kConstantBuffer = NULL;
    ms_kBooleanDesc.m_kDesc = NULL;
    memset(&ms_kBooleanDesc, 0, sizeof(ms_kBooleanDesc));
    ms_kBooleanDesc.m_eParamType = PT_BOOL;
    return &ms_kBooleanDesc;
}
//---------------------------------------------------------------------------
NiD3D10EffectParameterIntegerDesc* NiD3D10EffectParameterDesc::GetIntegerDesc()
{
    ms_kIntegerDesc.m_kName = NULL;
    ms_kIntegerDesc.m_kConstantBuffer = NULL;
    ms_kIntegerDesc.m_kDesc = NULL;
    memset(&ms_kIntegerDesc, 0, sizeof(ms_kIntegerDesc));
    ms_kIntegerDesc.m_eParamType = PT_UINT;
    return &ms_kIntegerDesc;
}
//---------------------------------------------------------------------------
NiD3D10EffectParameterTextureDesc* NiD3D10EffectParameterDesc::GetTextureDesc()
{
    ms_kTextureDesc.m_kName = NULL;
    ms_kTextureDesc.m_kDesc = NULL;
    ms_kTextureDesc.m_kTextureName = NULL;
    ms_kTextureDesc.m_kTextureMap = NULL;
    memset(&ms_kTextureDesc, 0, sizeof(ms_kTextureDesc));
    ms_kTextureDesc.m_eParamType = PT_TEXTURE;
    return &ms_kTextureDesc;
}
//---------------------------------------------------------------------------
NiD3D10EffectParameterPredefinedDesc*
    NiD3D10EffectParameterDesc::GetPredefinedDesc()
{
    ms_kPredefinedDesc.m_kName = NULL;
    ms_kPredefinedDesc.m_kConstantBuffer = NULL;
    ms_kPredefinedDesc.m_kPredefined = NULL;
    memset(&ms_kPredefinedDesc, 0, sizeof(ms_kPredefinedDesc));
    ms_kPredefinedDesc.m_eParamType = PT_PREDEFINED;
    return &ms_kPredefinedDesc;
}
//---------------------------------------------------------------------------
NiD3D10EffectParameterObjectDesc* NiD3D10EffectParameterDesc::GetObjectDesc()
{
    ms_kObjectDesc.m_kName = NULL;
    ms_kObjectDesc.m_kConstantBuffer = NULL;
    memset(&ms_kObjectDesc, 0, sizeof(ms_kObjectDesc));
    ms_kObjectDesc.m_eParamType = PT_OBJECT;
    return &ms_kObjectDesc;
}
//---------------------------------------------------------------------------
NiD3D10EffectParameterArrayDesc* 
NiD3D10EffectParameterDesc::GetArrayDesc()
{
    ms_kArrayDesc.m_kName = NULL;
    ms_kArrayDesc.m_kConstantBuffer = NULL;
    ms_kArrayDesc.m_kDesc = NULL;
    memset(&ms_kArrayDesc, 0, sizeof(ms_kArrayDesc));
    ms_kArrayDesc.m_eParamType = PT_ARRAY;
    return &ms_kArrayDesc;
}
//---------------------------------------------------------------------------
NiD3D10EffectParameterVectorDesc::VectorType 
    NiD3D10EffectParameterVectorDesc::GetVectorType()
{
    NIASSERT(m_uiNumAtomicElements != 1); // Single float
    if (m_uiNumRows == 1 && 
        m_uiNumCols == 2 && 
        m_uiNumAtomicElements == 2)
    {
        return TYPE_POINT2;
    }
    else if (m_uiNumRows == 1 && 
        m_uiNumCols == 3 && 
        m_uiNumAtomicElements == 3)
    {
        return TYPE_POINT3;
    }
    else
    {
        NIASSERT(m_uiNumRows == 1 && 
            m_uiNumCols == 4 && 
            m_uiNumAtomicElements == 4);
        return TYPE_POINT4;
    }
}
//---------------------------------------------------------------------------
NiD3D10EffectParameterArrayDesc::ArrayType 
    NiD3D10EffectParameterArrayDesc::GetArrayType()
{
    if (m_uiNumRows == 1 && m_uiNumCols == 1)
    {
        return TYPE_FLOAT;
    }
    else if (m_uiNumRows == 1 && m_uiNumCols == 2)
    {
        return TYPE_POINT2;
    }
    else if (m_uiNumRows == 1 && m_uiNumCols == 3)
    {
        return TYPE_POINT3;
    }
    else if (m_uiNumRows == 1 && m_uiNumCols == 4 && m_bColor)
    {
        return TYPE_COLORA;
    }
    else
    {
        NIASSERT(m_uiNumRows == 1 && m_uiNumCols == 4);
        return TYPE_POINT4;
    }
}
//---------------------------------------------------------------------------
NiUInt32 NiD3D10EffectParameterArrayDesc::GetElementSize()
{
    if (m_uiNumRows == 1 && m_uiNumCols == 1)
    {
        return sizeof(float);
    }
    else if (m_uiNumRows == 1 && m_uiNumCols == 2)
    {
        return 2 * sizeof(float);
    }
    else if (m_uiNumRows == 1 && m_uiNumCols == 3)
    {
        return 3 * sizeof(float);
    }
    else
    {
        NIASSERT(m_uiNumRows == 1 && m_uiNumCols == 4);
        return 4 * sizeof(float);
    }
}
//---------------------------------------------------------------------------
bool NiD3D10EffectParameterArrayDesc::ExpandToVector4Array(
    const float* pfSrcArray, D3DXVECTOR4* pkDestArray,
    NiUInt32 uiRows, NiUInt32 uiColumns, 
    NiUInt32 uiNumElements)
{
    if (uiRows == 1 && uiColumns == 2)
    {
        NiUInt32 uiArrayIdx = 0;
        for (NiUInt32 i = 0; i < uiNumElements; i++)
        {
            NiUInt32 j = 0;
            float* pfDest = (float*) pkDestArray[i];
            pfDest[j++] = pfSrcArray[uiArrayIdx++];
            pfDest[j++] = pfSrcArray[uiArrayIdx++];
            pfDest[j++] = 0.0f;
            pfDest[j++] = 0.0f;
        }
        return true;
    }
    else if (uiRows == 1 && uiColumns == 3)
    {
        NiUInt32 uiArrayIdx = 0;
        for (NiUInt32 i = 0; i < uiNumElements; i++)
        {
            NiUInt32 j = 0;
            float* pfDest = (float*) pkDestArray[i];
            pfDest[j++] = pfSrcArray[uiArrayIdx++];
            pfDest[j++] = pfSrcArray[uiArrayIdx++];
            pfDest[j++] = pfSrcArray[uiArrayIdx++];
            pfDest[j++] = 0.0f;
        }
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiD3D10EffectParameterArrayDesc::CompactFromVector4Array(
    const D3DXVECTOR4* pkSrcArray, float* pfDestArray,
    NiUInt32 uiRows, NiUInt32 uiColumns, 
    NiUInt32 uiNumElements)
{
    if (uiRows == 1 && uiColumns == 2)
    {
        NiUInt32 uiArrayIdx = 0;
        for (NiUInt32 i = 0; i < uiNumElements; i++)
        {
            pfDestArray[uiArrayIdx++] = pkSrcArray[i].x;
            pfDestArray[uiArrayIdx++] = pkSrcArray[i].y;
        }
        return true;
    }
    else if (uiRows == 1 && uiColumns == 3)
    {
        NiUInt32 uiArrayIdx = 0;
        for (NiUInt32 i = 0; i < uiNumElements; i++)
        {
            pfDestArray[uiArrayIdx++] = pkSrcArray[i].x;
            pfDestArray[uiArrayIdx++] = pkSrcArray[i].y;
            pfDestArray[uiArrayIdx++] = pkSrcArray[i].z;
        }
        return true;
    }
    else if (uiRows == 1 && uiColumns == 4)
    {
        NiUInt32 uiArrayIdx = 0;
        for (NiUInt32 i = 0; i < uiNumElements; i++)
        {
            pfDestArray[uiArrayIdx++] = pkSrcArray[i].x;
            pfDestArray[uiArrayIdx++] = pkSrcArray[i].y;
            pfDestArray[uiArrayIdx++] = pkSrcArray[i].z;
            pfDestArray[uiArrayIdx++] = pkSrcArray[i].w;
        }
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
