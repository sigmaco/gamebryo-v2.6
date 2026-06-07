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

#ifndef NID3D10EFFECTPARAMETERDESC_H
#define NID3D10EFFECTPARAMETERDESC_H

#include "NiD3D10EffectShaderLibType.h"
#include <NiD3D10Headers.h>
#include <NiShaderAttributeDesc.h>

class NiD3D10EffectParameterMatrixDesc;
class NiD3D10EffectParameterVectorDesc;
class NiD3D10EffectParameterFloatDesc;
class NiD3D10EffectParameterBooleanDesc;
class NiD3D10EffectParameterIntegerDesc;
class NiD3D10EffectParameterTextureDesc;
class NiD3D10EffectParameterPredefinedDesc;
class NiD3D10EffectParameterObjectDesc;
class NiD3D10EffectParameterArrayDesc;

//---------------------------------------------------------------------------
class NID3D10EFFECTSHADER_ENTRY NiD3D10EffectParameterDesc : public NiMemObject
{
public:
    static NiD3D10EffectParameterMatrixDesc* GetMatrixDesc();
    static NiD3D10EffectParameterVectorDesc* GetVectorDesc();
    static NiD3D10EffectParameterFloatDesc* GetFloatDesc();
    static NiD3D10EffectParameterBooleanDesc* GetBooleanDesc();
    static NiD3D10EffectParameterIntegerDesc* GetIntegerDesc();
    static NiD3D10EffectParameterTextureDesc* GetTextureDesc();
    static NiD3D10EffectParameterPredefinedDesc* GetPredefinedDesc();
    static NiD3D10EffectParameterObjectDesc* GetObjectDesc();
    static NiD3D10EffectParameterArrayDesc* GetArrayDesc();

    enum ParameterType
    {
        PT_NONE = 0,
        PT_MATRIX,
        PT_VECTOR,
        PT_FLOAT,
        PT_BOOL,
        PT_UINT,
        PT_TEXTURE,
        PT_PREDEFINED,
        PT_OBJECT,
        PT_ARRAY,
        PT_NUMBER_TYPES,
        PT_INVALID
    };

    ParameterType m_eParamType;
    NiUInt32 m_uiFlags;
    bool m_bGlobal;

    NiFixedString m_kName;
    NiFixedString m_kConstantBuffer;
protected:
    static NiD3D10EffectParameterMatrixDesc ms_kMatrixDesc;
    static NiD3D10EffectParameterVectorDesc ms_kVectorDesc;
    static NiD3D10EffectParameterFloatDesc ms_kFloatDesc;
    static NiD3D10EffectParameterBooleanDesc ms_kBooleanDesc;
    static NiD3D10EffectParameterIntegerDesc ms_kIntegerDesc;
    static NiD3D10EffectParameterTextureDesc ms_kTextureDesc;
    static NiD3D10EffectParameterPredefinedDesc ms_kPredefinedDesc;
    static NiD3D10EffectParameterObjectDesc ms_kObjectDesc;
    static NiD3D10EffectParameterArrayDesc ms_kArrayDesc;
};
//---------------------------------------------------------------------------
class NID3D10EFFECTSHADER_ENTRY NiD3D10EffectParameterMatrixDesc : 
    public NiD3D10EffectParameterDesc
{
public:
    NiFixedString m_kDesc;

    float m_afInitValue[16];
    NiUInt32 m_uiNumRows;
    NiUInt32 m_uiNumCols;

    bool m_bHidden;
};
//---------------------------------------------------------------------------
class NID3D10EFFECTSHADER_ENTRY NiD3D10EffectParameterVectorDesc : 
    public NiD3D10EffectParameterDesc
{
public:
    NiFixedString m_kDesc;

    enum 
    {
        MAX_ROWS = 1,
        MAX_COLUMNS = 4,
        MAX_ENTRIES = MAX_ROWS * MAX_COLUMNS
    };

    enum VectorType
    {
        TYPE_POINT2 = 0,
        TYPE_POINT3,
        TYPE_POINT4,
        TYPE_MAX
    };

    VectorType GetVectorType();

    float m_afInitValue[MAX_ENTRIES];
    float m_afMaxValue[MAX_ENTRIES];
    float m_afMinValue[MAX_ENTRIES];

    NiUInt32 m_uiNumRows;
    NiUInt32 m_uiNumCols;
    NiUInt32 m_uiNumAtomicElements;

    bool m_bHidden;
    bool m_bColor;
};
//---------------------------------------------------------------------------
class NID3D10EFFECTSHADER_ENTRY NiD3D10EffectParameterArrayDesc : 
    public NiD3D10EffectParameterDesc
{
public:
    NiFixedString m_kDesc;

    enum ArrayType
    {
        TYPE_FLOAT = 0,
        TYPE_POINT2,
        TYPE_POINT3,
        TYPE_POINT4,
        TYPE_COLORA,
        TYPE_MAX
    };

    enum 
    {
        MAX_ROWS = 256,
        MAX_COLUMNS = 4,
        MAX_ENTRIES = MAX_ROWS*MAX_COLUMNS
    };

    ArrayType GetArrayType();

    float m_afInitValue[MAX_ENTRIES];
    float m_afMaxValue[MAX_ENTRIES];
    float m_afMinValue[MAX_ENTRIES];
    float m_afTempValues[MAX_ENTRIES];

    NiUInt32 m_uiNumRows;
    NiUInt32 m_uiNumCols;
    NiUInt32 m_uiNumElements;
    NiUInt32 GetElementSize();

    bool m_bHidden;
    bool m_bColor;

    static bool ExpandToVector4Array(
        const float* pfSrcArray, 
        D3DXVECTOR4* pkDestArray,
        NiUInt32 uiRows, 
        NiUInt32 uiColumns, 
        NiUInt32 uiNumElements);
    static bool CompactFromVector4Array( 
        const D3DXVECTOR4* pkSrcArray, 
        float* pfDestArray,
        NiUInt32 uiRows, 
        NiUInt32 uiColumns, 
        NiUInt32 uiNumElements);
};
//---------------------------------------------------------------------------
class NID3D10EFFECTSHADER_ENTRY NiD3D10EffectParameterFloatDesc : 
    public NiD3D10EffectParameterDesc
{
public:
    NiFixedString m_kDesc;

    float m_fInitValue;
    float m_fMaxValue;
    float m_fMinValue;

    bool m_bHidden;
};
//---------------------------------------------------------------------------
class NID3D10EFFECTSHADER_ENTRY NiD3D10EffectParameterBooleanDesc : 
    public NiD3D10EffectParameterDesc
{
public:
    NiFixedString m_kDesc;

    bool m_bInitValue;

    bool m_bHidden;
};
//---------------------------------------------------------------------------
class NID3D10EFFECTSHADER_ENTRY NiD3D10EffectParameterIntegerDesc : 
    public NiD3D10EffectParameterDesc
{
public:
    NiFixedString m_kDesc;

    NiUInt32 m_uiInitValue;
    NiUInt32 m_uiMaxValue;
    NiUInt32 m_uiMinValue;

    bool m_bHidden;
};
//---------------------------------------------------------------------------
class NID3D10EFFECTSHADER_ENTRY NiD3D10EffectParameterTextureDesc : 
    public NiD3D10EffectParameterDesc
{
public:
    NiFixedString m_kDesc;

    void* m_pvInitValue;

    bool m_bHidden;

    NiFixedString m_kTextureName;

    NiFixedString m_kTextureMap;
    NiUInt32 m_uiTextureIndex;

    NiUInt32 m_uiObjectType;
    NiUInt32 m_uiObjectIndex;
};
//---------------------------------------------------------------------------
class NID3D10EFFECTSHADER_ENTRY NiD3D10EffectParameterPredefinedDesc : 
    public NiD3D10EffectParameterDesc
{
public:
    NiFixedString m_kPredefined;
    NiUInt32 m_uiMapping;
    NiUInt32 m_uiArrayCount;
    NiUInt32 m_uiExtraData;
};
//---------------------------------------------------------------------------
class NID3D10EFFECTSHADER_ENTRY NiD3D10EffectParameterObjectDesc :
    public NiD3D10EffectParameterDesc
{
public:
    NiUInt32 m_uiMapping;
    NiShaderAttributeDesc::ObjectType m_eObjectType;
    NiUInt32 m_uiObjectIndex;
};
//---------------------------------------------------------------------------

#endif //NID3D10EFFECTPARAMETERDESC_H
