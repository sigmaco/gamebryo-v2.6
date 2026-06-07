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

#ifndef NID3DXEFFECTPARAMETERDESC_H
#define NID3DXEFFECTPARAMETERDESC_H

#include "NiD3DXEffectShaderLibType.h"
#include <NiShaderAttributeDesc.h>

class NiD3DXEffectParameterMatrixDesc;
class NiD3DXEffectParameterVectorDesc;
class NiD3DXEffectParameterFloatDesc;
class NiD3DXEffectParameterBooleanDesc;
class NiD3DXEffectParameterIntegerDesc;
class NiD3DXEffectParameterTextureDesc;
class NiD3DXEffectParameterPredefinedDesc;
class NiD3DXEffectParameterObjectDesc;
class NiD3DXEffectParameterArrayDesc;
struct D3DXVECTOR4;

//---------------------------------------------------------------------------
class NID3DXEFFECTSHADER_ENTRY NiD3DXEffectParameterDesc : public NiMemObject
{
public:
    static NiD3DXEffectParameterMatrixDesc* GetMatrixDesc();
    static NiD3DXEffectParameterVectorDesc* GetVectorDesc();
    static NiD3DXEffectParameterFloatDesc* GetFloatDesc();
    static NiD3DXEffectParameterBooleanDesc* GetBooleanDesc();
    static NiD3DXEffectParameterIntegerDesc* GetIntegerDesc();
    static NiD3DXEffectParameterTextureDesc* GetTextureDesc();
    static NiD3DXEffectParameterPredefinedDesc* GetPredefinedDesc();
    static NiD3DXEffectParameterObjectDesc* GetObjectDesc();
    static NiD3DXEffectParameterArrayDesc* GetArrayDesc();

    enum ParameterType
    {
        PT_NONE,
        PT_MATRIX,
        PT_VECTOR,
        PT_FLOAT,
        PT_BOOL,
        PT_UINT,
        PT_TEXTURE,
        PT_PREDEFINED,
        PT_OBJECT,
        PT_ARRAY
    };

    ParameterType m_eParamType;
    bool m_bGlobal;

    const char* m_pcName;
protected:
    static NiD3DXEffectParameterMatrixDesc ms_kMatrixDesc;
    static NiD3DXEffectParameterVectorDesc ms_kVectorDesc;
    static NiD3DXEffectParameterFloatDesc ms_kFloatDesc;
    static NiD3DXEffectParameterBooleanDesc ms_kBooleanDesc;
    static NiD3DXEffectParameterIntegerDesc ms_kIntegerDesc;
    static NiD3DXEffectParameterTextureDesc ms_kTextureDesc;
    static NiD3DXEffectParameterPredefinedDesc ms_kPredefinedDesc;
    static NiD3DXEffectParameterObjectDesc ms_kObjectDesc;
    static NiD3DXEffectParameterArrayDesc ms_kArrayDesc;
};
//---------------------------------------------------------------------------
class NID3DXEFFECTSHADER_ENTRY NiD3DXEffectParameterMatrixDesc : 
    public NiD3DXEffectParameterDesc
{
public:
    const char* m_pcDesc;

    float m_afInitValue[16];
    unsigned int m_uiNumRows;
    unsigned int m_uiNumCols;

    bool m_bHidden;
};
//---------------------------------------------------------------------------
class NID3DXEFFECTSHADER_ENTRY NiD3DXEffectParameterVectorDesc : 
    public NiD3DXEffectParameterDesc
{
public:
    const char* m_pcDesc;

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

    unsigned int m_uiNumRows;
    unsigned int m_uiNumCols;
    unsigned int m_uiNumAtomicElements;

    bool m_bHidden;
    bool m_bColor;
};
//---------------------------------------------------------------------------
class NID3DXEFFECTSHADER_ENTRY NiD3DXEffectParameterArrayDesc : 
    public NiD3DXEffectParameterDesc
{
public:
    const char* m_pcDesc;

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

    unsigned int m_uiNumRows;
    unsigned int m_uiNumCols;
    unsigned int m_uiNumElements;
    unsigned int GetElementSize();

    bool m_bHidden;
    bool m_bColor;

    static bool ExpandToVector4Array(const float* pfSrcArray, 
        D3DXVECTOR4* pfDestArray,
        unsigned int uiRows, unsigned int uiColumns, 
        unsigned int uiNumElements);
    static bool CompactFromVector4Array( 
        const D3DXVECTOR4* pfSrcArray, float* pfDestArray,
        unsigned int uiRows, unsigned int uiColumns, 
        unsigned int uiNumElements);
};
//---------------------------------------------------------------------------
class NID3DXEFFECTSHADER_ENTRY NiD3DXEffectParameterFloatDesc : 
    public NiD3DXEffectParameterDesc
{
public:
    const char* m_pcDesc;

    float m_fInitValue;
    float m_fMaxValue;
    float m_fMinValue;

    bool m_bHidden;
};
//---------------------------------------------------------------------------
class NID3DXEFFECTSHADER_ENTRY NiD3DXEffectParameterBooleanDesc : 
    public NiD3DXEffectParameterDesc
{
public:
    const char* m_pcDesc;

    bool m_bInitValue;

    bool m_bHidden;
};
//---------------------------------------------------------------------------
class NID3DXEFFECTSHADER_ENTRY NiD3DXEffectParameterIntegerDesc : 
    public NiD3DXEffectParameterDesc
{
public:
    const char* m_pcDesc;

    unsigned int m_uiInitValue;
    unsigned int m_uiMaxValue;
    unsigned int m_uiMinValue;

    bool m_bHidden;
};
//---------------------------------------------------------------------------
class NID3DXEFFECTSHADER_ENTRY NiD3DXEffectParameterTextureDesc : 
    public NiD3DXEffectParameterDesc
{
public:
    const char* m_pcDesc;

    void* m_pvInitValue;

    bool m_bHidden;

    enum TextureType
    {
        TT_NONE,
        TT_2D,
        TT_3D,
        TT_CUBE
    };

    TextureType m_eTextureType;

    const char* m_pcTextureName;
    const char* m_pcTextureFunction;
    const char* m_pcTextureTarget;

    const char* m_pcTextureType;
    unsigned int m_uiTextureWidth;
    unsigned int m_uiTextureHeight;
    unsigned int m_uiTextureDepth;
    const char* m_pcTextureMap;
    unsigned int m_uiTextureIndex;

    unsigned int m_uiObjectType;
    unsigned int m_uiObjectIndex;
};
//---------------------------------------------------------------------------
class NID3DXEFFECTSHADER_ENTRY NiD3DXEffectParameterPredefinedDesc : 
    public NiD3DXEffectParameterDesc
{
public:
    const char* m_pcPredefined;
    unsigned int m_uiMapping;
    unsigned int m_uiArrayCount;
    unsigned int m_uiExtraData;
};
//---------------------------------------------------------------------------
class NID3DXEFFECTSHADER_ENTRY NiD3DXEffectParameterObjectDesc :
    public NiD3DXEffectParameterDesc
{
public:
    unsigned int m_uiMapping;
    NiShaderAttributeDesc::ObjectType m_eObjectType;
    unsigned int m_uiObjectIndex;
};
//---------------------------------------------------------------------------

#endif //NID3DXEFFECTPARAMETERDESC_H
