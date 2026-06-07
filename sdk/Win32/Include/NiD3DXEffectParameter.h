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

#ifndef NID3DXEFFECTPARAMETER_H
#define NID3DXEFFECTPARAMETER_H

#include <NiD3DDefines.h>

#include "NiD3DXEffectShaderLibType.h"
#include "NiD3DXEffectParameterDesc.h"

#if defined(WIN32)
    #include "NiD3DXEffectHeaders_DX9.h"
#elif defined(_XENON) 
    #include "NiD3DXEffectHeaders_Xenon.h"
#endif

#include <NiD3DShaderConstantMap.h>
#include <NiD3DTextureStage.h>
#include <NiFixedString.h>

class NiD3DXEffectAttributeTable;

class NID3DXEFFECTSHADER_ENTRY NiD3DXEffectParameter : public NiMemObject
{
public:
    NiD3DXEffectParameter();
    ~NiD3DXEffectParameter();

    bool Initialize(NiD3DXEffectParameterDesc* pkParamDesc, 
        NiD3DXEffectAttributeTable* pkAttribTable);

    NiD3DError SetParameter(
        const NiRenderCallContext& kRCC, 
        LPD3DXEFFECT pkEffect, 
        bool bVertexShaderPresent);

    // *** begin Emergent internal use only ***

    inline const NiFixedString& GetName() const;

    inline void SetParameterPtr(D3DXParameterPtr pkParam);
    inline D3DXParameterPtr GetParameterPtr() const;

    void LinkHandle(LPD3DXEFFECT pkEffect);

    inline bool RequiresTime() const;

    void DestroyRendererData();

    inline void SetUsed(bool bUsed);
    inline bool GetUsed() const;

    inline NiD3DShaderConstantMap::DefinedMappings GetPredefinedMapping() const;
    inline unsigned int GetArrayCount() const;

    static void ReleaseBoneArray();

    inline void ClearTextureReferences(LPD3DXBASEEFFECT pkEffect);

    // *** end Emergent internal use only ***

protected:
    void Construct2DTexture();
    void Construct3DTexture();
    void ConstructCubeTexture();

    bool FillPredefinedConstantValue(
        const NiRenderCallContext& kRCC, 
        LPD3DXEFFECT pkEffect, 
        bool bVertexShaderPresent);
    NiD3DError FillObjectConstantValue(
        const NiRenderCallContext& kRCC, 
        LPD3DXEFFECT pkD3DXEffect);
    D3DBaseTexturePtr GetNiTexture(NiTexturingProperty* pkTexProp, 
        unsigned int uiTextureFlags);

    D3DXParameterPtr m_pkParam;
    NiFixedString m_kName;
    bool m_bGlobal;

    NiD3DShaderConstantMap::DefinedMappings m_ePredefinedMapping;
    NiD3DShaderConstantMap::ObjectMappings m_eObjectMapping;
    NiD3DXEffectParameterDesc::ParameterType m_eParameterType;
    NiD3DXEffectParameterTextureDesc::TextureType m_eTextureType;
    unsigned int m_uiParameterCount;
    unsigned int m_uiArrayCount;

    unsigned int m_uiExtraData;

    // Texture-specific variables
    char* m_pcTextureSource;
    char* m_pcTextureTarget;
    unsigned int m_uiTextureFlags;
    D3DBaseTexturePtr m_pkTexture;
    unsigned int m_uiWidth;
    unsigned int m_uiHeight;
    unsigned int m_uiDepth;

    // Object-specific variables.
    NiShaderAttributeDesc::ObjectType m_eObjectType;

    bool m_bUsed;

    static D3DXVECTOR4 
        ms_akVector4Array[NiD3DXEffectParameterArrayDesc::MAX_ROWS];

    static D3DXMATRIX* ms_pkMatrixArray;
    static unsigned int ms_uiMatrixArraySize;
    static float ms_afObjectData[16];
};

#include "NiD3DXEffectParameter.inl"

#endif //NID3DXEFFECTPARAMETER_H
