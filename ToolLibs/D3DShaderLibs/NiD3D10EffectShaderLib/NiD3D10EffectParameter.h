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

#ifndef NID3D10EFFECTPARAMETER_H
#define NID3D10EFFECTPARAMETER_H

#include "NiD3D10EffectShaderLibType.h"
#include "NiD3D10EffectParameterDesc.h"

#include <NiShaderConstantMap.h>
#include <NiFixedString.h>

class NiD3D10EffectAttributeTable;
class NiRenderCallContext;
class NiTexturingProperty;

class NID3D10EFFECTSHADER_ENTRY NiD3D10EffectParameter : public NiMemObject
{
public:
    NiD3D10EffectParameter();
    ~NiD3D10EffectParameter();

    bool Initialize(NiD3D10EffectParameterDesc* pkParamDesc, 
        NiD3D10EffectAttributeTable* pkAttribTable);

    NiShaderError SetParameter(
        const NiRenderCallContext& kRCC);

    // *** begin Emergent internal use only ***

    inline const NiFixedString& GetName() const;
    inline const NiFixedString& GetConstantBufferName() inline const;


    inline void SetD3D10Parameter(ID3D10EffectVariable* pkParam);
    inline ID3D10EffectVariable* GetD3D10Parameter() const;

    inline bool RequiresTime() const;

    void DestroyRendererData();

    inline void SetUsed(bool bUsed);
    inline bool GetUsed() const;

    const char* GetKey() const;
    NiUInt32 GetFlags() const;
    NiUInt32 GetExtra() const;

    inline NiShaderConstantMap::DefinedMappings GetPredefinedMapping() const;
    inline unsigned int GetArrayCount() const;
    inline NiD3D10EffectParameterDesc::ParameterType GetType () const;

    static void ReleaseBoneArray();

    // *** end Emergent internal use only ***

protected:
    ID3D10ShaderResourceView* GetNiTexture(
        NiTexturingProperty* pkTexProp, 
        unsigned int uiTextureFlags);

    ID3D10EffectVariable* m_pkParam;
    ID3D10EffectShaderResourceVariable* m_pkEffectSRV;
    ID3D10ShaderResourceView* m_pkTextureDataRV;
	NiTexture* m_pkTexture;

    NiFixedString m_kName;
    NiFixedString m_kConstantBufferName;
    bool m_bGlobal;

    NiFixedString m_kPredefined;
    NiShaderConstantMap::DefinedMappings m_ePredefinedMapping;
    NiShaderConstantMap::ObjectMappings m_eObjectMapping;
    NiD3D10EffectParameterDesc::ParameterType m_eParameterType;
    NiUInt32 m_uiParameterCount;
    NiUInt32 m_uiArrayCount;

    NiUInt32 m_uiFlags;
    NiUInt32 m_uiExtraData;

    // Texture-specific variables
    NiFixedString m_kTextureSource;
    NiUInt32 m_uiTextureFlags;

    // Object-specific variables.
    NiShaderAttributeDesc::ObjectType m_eObjectType;

    bool m_bUsed;

    static D3DXVECTOR4 
        ms_akVector4Array[NiD3D10EffectParameterArrayDesc::MAX_ROWS];

    static D3DXMATRIXA16* ms_pkMatrixArray;
    static unsigned int ms_uiMatrixArraySize;
    static float ms_afObjectData[16];
};

#include "NiD3D10EffectParameter.inl"

#endif //NID3D10EFFECTPARAMETER_H
