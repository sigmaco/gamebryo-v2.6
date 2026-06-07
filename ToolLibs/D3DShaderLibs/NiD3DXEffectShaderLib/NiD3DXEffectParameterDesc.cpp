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

#include "NiD3DXEffectParameterDesc.h"
#include <NiRTLib.h>

#include <NiD3DDefines.h>

NiD3DXEffectParameterMatrixDesc NiD3DXEffectParameterDesc::ms_kMatrixDesc;
NiD3DXEffectParameterVectorDesc NiD3DXEffectParameterDesc::ms_kVectorDesc;
NiD3DXEffectParameterFloatDesc NiD3DXEffectParameterDesc::ms_kFloatDesc;
NiD3DXEffectParameterBooleanDesc NiD3DXEffectParameterDesc::ms_kBooleanDesc;
NiD3DXEffectParameterIntegerDesc NiD3DXEffectParameterDesc::ms_kIntegerDesc;
NiD3DXEffectParameterTextureDesc NiD3DXEffectParameterDesc::ms_kTextureDesc;
NiD3DXEffectParameterPredefinedDesc
    NiD3DXEffectParameterDesc::ms_kPredefinedDesc;
NiD3DXEffectParameterObjectDesc NiD3DXEffectParameterDesc::ms_kObjectDesc;
NiD3DXEffectParameterArrayDesc NiD3DXEffectParameterDesc::ms_kArrayDesc;

//---------------------------------------------------------------------------
NiD3DXEffectParameterMatrixDesc* NiD3DXEffectParameterDesc::GetMatrixDesc()
{
    memset(&ms_kMatrixDesc, 0, sizeof(ms_kMatrixDesc));
    ms_kMatrixDesc.m_eParamType = PT_MATRIX;
    return &ms_kMatrixDesc;
}
//---------------------------------------------------------------------------
NiD3DXEffectParameterVectorDesc* NiD3DXEffectParameterDesc::GetVectorDesc()
{
    memset(&ms_kVectorDesc, 0, sizeof(ms_kVectorDesc));
    ms_kVectorDesc.m_eParamType = PT_VECTOR;
    return &ms_kVectorDesc;
}
//---------------------------------------------------------------------------
NiD3DXEffectParameterFloatDesc* NiD3DXEffectParameterDesc::GetFloatDesc()
{
    memset(&ms_kFloatDesc, 0, sizeof(ms_kFloatDesc));
    ms_kFloatDesc.m_eParamType = PT_FLOAT;
    return &ms_kFloatDesc;
}
//---------------------------------------------------------------------------
NiD3DXEffectParameterBooleanDesc* NiD3DXEffectParameterDesc::GetBooleanDesc()
{
    memset(&ms_kBooleanDesc, 0, sizeof(ms_kBooleanDesc));
    ms_kBooleanDesc.m_eParamType = PT_BOOL;
    return &ms_kBooleanDesc;
}
//---------------------------------------------------------------------------
NiD3DXEffectParameterIntegerDesc* NiD3DXEffectParameterDesc::GetIntegerDesc()
{
    memset(&ms_kIntegerDesc, 0, sizeof(ms_kIntegerDesc));
    ms_kIntegerDesc.m_eParamType = PT_UINT;
    return &ms_kIntegerDesc;
}
//---------------------------------------------------------------------------
NiD3DXEffectParameterTextureDesc* NiD3DXEffectParameterDesc::GetTextureDesc()
{
    memset(&ms_kTextureDesc, 0, sizeof(ms_kTextureDesc));
    ms_kTextureDesc.m_eParamType = PT_TEXTURE;
    return &ms_kTextureDesc;
}
//---------------------------------------------------------------------------
NiD3DXEffectParameterPredefinedDesc*
    NiD3DXEffectParameterDesc::GetPredefinedDesc()
{
    memset(&ms_kPredefinedDesc, 0, sizeof(ms_kPredefinedDesc));
    ms_kPredefinedDesc.m_eParamType = PT_PREDEFINED;
    return &ms_kPredefinedDesc;
}
//---------------------------------------------------------------------------
NiD3DXEffectParameterObjectDesc* NiD3DXEffectParameterDesc::GetObjectDesc()
{
    memset(&ms_kObjectDesc, 0, sizeof(ms_kObjectDesc));
    ms_kObjectDesc.m_eParamType = PT_OBJECT;
    return &ms_kObjectDesc;
}
//---------------------------------------------------------------------------
NiD3DXEffectParameterArrayDesc* 
NiD3DXEffectParameterDesc::GetArrayDesc()
{
    memset(&ms_kArrayDesc, 0, sizeof(ms_kArrayDesc));
    ms_kArrayDesc.m_eParamType = PT_ARRAY;
    return &ms_kArrayDesc;
}
//---------------------------------------------------------------------------
NiD3DXEffectParameterVectorDesc::VectorType 
    NiD3DXEffectParameterVectorDesc::GetVectorType()
{
    NIASSERT(m_uiNumAtomicElements != 1); // Single float
    if (m_uiNumRows == 1 && m_uiNumCols == 2 && m_uiNumAtomicElements 
        == 2)
    {
        return TYPE_POINT2;
    }
    else if (m_uiNumRows == 1 && m_uiNumCols == 3 && m_uiNumAtomicElements 
        == 3)
    {
        return TYPE_POINT3;
    }
    else
    {
        NIASSERT(m_uiNumRows == 1 && m_uiNumCols == 4 && m_uiNumAtomicElements 
            == 4);
        return TYPE_POINT4;
    }

}
//---------------------------------------------------------------------------
NiD3DXEffectParameterArrayDesc::ArrayType 
    NiD3DXEffectParameterArrayDesc::GetArrayType()
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
unsigned int NiD3DXEffectParameterArrayDesc::GetElementSize()
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
bool NiD3DXEffectParameterArrayDesc::ExpandToVector4Array(
    const float* pfSrcArray, D3DXVECTOR4* pfDestArray,
    unsigned int uiRows, unsigned int uiColumns, 
    unsigned int uiNumElements)
{
    if (uiRows == 1 && uiColumns == 2)
    {
        unsigned int uiArrayIdx = 0;
        for (unsigned int ui = 0; ui < uiNumElements; ui++)
        {
            unsigned int uj = 0;
            float* pfDest = (float*) pfDestArray[ui];
            pfDest[uj++] = pfSrcArray[uiArrayIdx++];
            pfDest[uj++] = pfSrcArray[uiArrayIdx++];
            pfDest[uj++] = 0.0f;
            pfDest[uj++] = 0.0f;
        }
        return true;
    }
    else if (uiRows == 1 && uiColumns == 3)
    {
        unsigned int uiArrayIdx = 0;
        for (unsigned int ui = 0; ui < uiNumElements; ui++)
        {
            unsigned int uj = 0;
            float* pfDest = (float*) pfDestArray[ui];
            pfDest[uj++] = pfSrcArray[uiArrayIdx++];
            pfDest[uj++] = pfSrcArray[uiArrayIdx++];
            pfDest[uj++] = pfSrcArray[uiArrayIdx++];
            pfDest[uj++] = 0.0f;
        }
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiD3DXEffectParameterArrayDesc::CompactFromVector4Array(
    const D3DXVECTOR4* pfSrcArray, float* pfDestArray,
    unsigned int uiRows, unsigned int uiColumns, 
    unsigned int uiNumElements)
{
    if (uiRows == 1 && uiColumns == 2)
    {
        unsigned int uiArrayIdx = 0;
        for (unsigned int ui = 0; ui < uiNumElements; ui++)
        {
            pfDestArray[uiArrayIdx++] = pfSrcArray[ui].x;
            pfDestArray[uiArrayIdx++] = pfSrcArray[ui].y;
        }
        return true;
    }
    else if (uiRows == 1 && uiColumns == 3)
    {
        unsigned int uiArrayIdx = 0;
        for (unsigned int ui = 0; ui < uiNumElements; ui++)
        {
            pfDestArray[uiArrayIdx++] = pfSrcArray[ui].x;
            pfDestArray[uiArrayIdx++] = pfSrcArray[ui].y;
            pfDestArray[uiArrayIdx++] = pfSrcArray[ui].z;
        }
        return true;
    }
    else if (uiRows == 1 && uiColumns == 4)
    {
        unsigned int uiArrayIdx = 0;
        for (unsigned int ui = 0; ui < uiNumElements; ui++)
        {
            pfDestArray[uiArrayIdx++] = pfSrcArray[ui].x;
            pfDestArray[uiArrayIdx++] = pfSrcArray[ui].y;
            pfDestArray[uiArrayIdx++] = pfSrcArray[ui].z;
            pfDestArray[uiArrayIdx++] = pfSrcArray[ui].w;
        }
        return true;
    }
    return false;
}
//---------------------------------------------------------------------------
