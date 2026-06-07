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

#ifndef NISHADOWMAP_H
#define NISHADOWMAP_H

#include "NiTexturingProperty.h"
#include "NiRenderedTexture.h"
#include "NiShadowRenderClick.h"
#include "Ni3DRenderView.h"
#include "NiStandardMaterial.h"

class NIMAIN_ENTRY NiShadowMap : public NiMemObject
{
    NiDeclareFlags(unsigned short);
    NiDeclareRootRTTI(NiShadowMap);

public:
    enum TextureType
    {
        TT_SINGLE,
        TT_CUBE
    };
    inline NiShadowMap();
    inline virtual ~NiShadowMap();
    TextureType GetTextureType();

    static const NiFixedString& 
        GetMapNameFromType(NiStandardMaterial::LightType eType);
    static const bool GetLightTypeFromName(const NiFixedString& kName, 
        unsigned int& uiID);

    // Transformation access functions
    inline const float* GetWorldToShadowMap() const;
    
    // image access function
    inline NiRenderedTexture* GetTexture() const;

    // Render click access function.
    inline NiShadowRenderClick* GetRenderClick() const;

    // clamp mode access functions
    inline NiTexturingProperty::ClampMode GetClampMode() const;
    inline void SetClampMode(NiTexturingProperty::ClampMode eClampMode);

    // filter mode access functions
    inline NiTexturingProperty::FilterMode GetFilterMode() const;
    inline void SetFilterMode(NiTexturingProperty::FilterMode eFilterMode);

    // max anisotropy access functions
    inline unsigned short GetMaxAnisotropy() const;
    inline void SetMaxAnisotropy(unsigned short usMaxAnisotropyValue);

    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    static bool Initialize(NiShadowMap* pkShadowMap,
        unsigned int uiWidth, unsigned int uiHeight, 
        NiTexture::FormatPrefs& kPrefs, 
        const NiPixelFormat* pkDepthFormat, 
        NiTexturingProperty::ClampMode eClampMode,
        NiTexturingProperty::FilterMode eFilterMode);
    static void Destroy(NiShadowMap* pkShadowMap);
    // *** end Emergent internal use only ***

protected:
    // flags
    enum
    {
        FILTERMODE_MASK  = 0x0F00,
        FILTERMODE_POS   = 8,
        CLAMPMODE_MASK   = 0x3000,
        CLAMPMODE_POS    = 12,
        TEXTURETYPE_MASK = 0x000F,
        TEXTURETYPE_POS  = 0
    };
    inline void ClearFlags();
    inline void SetTextureType(TextureType eTextureType);

    unsigned short m_usMaxAnisotropy;
    NiRenderedTexturePtr m_spTexture;
    NiShadowRenderClickPtr m_spRenderClick;

    static NiFixedString ms_akMapName[NiStandardMaterial::LIGHT_MAX];
};

#include "NiShadowMap.inl"

#endif  // #ifndef NISHADOWMAP_H
