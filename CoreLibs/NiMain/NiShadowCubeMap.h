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

#ifndef NISHADOWCUBEMAP_H
#define NISHADOWCUBEMAP_H

#include "NiShadowMap.h"

class NIMAIN_ENTRY NiShadowCubeMap : public NiShadowMap
{
    NiDeclareRTTI;

public:
    inline virtual ~NiShadowCubeMap() {};
    static const NiFixedString& GetMapName();

    inline const float* GetWorldToShadowMap(unsigned char ucIndex) const;

    inline NiRenderedCubeMap* GetCubeMapTexture() const;

    static unsigned char GetNumRenderClicks();
    inline NiShadowRenderClick* GetRenderClick(unsigned char ucIndex) const;

    // *** begin Emergent internal use only ***
    static void _SDMInit();
    static void _SDMShutdown();
    static bool Initialize(NiShadowCubeMap* pkShadowCubeMap,
        unsigned int uiSize, NiTexture::FormatPrefs& kPrefs,
        const NiPixelFormat* pkDepthFormat, 
        NiTexturingProperty::ClampMode eClampMode,
        NiTexturingProperty::FilterMode eFilterMode);
    static void Destroy(NiShadowCubeMap* pkShadowCubeMap);
    // *** end Emergent internal use only ***

protected:
    NiShadowRenderClickPtr m_aspRenderClicks[6];

    static NiFixedString ms_kMapName;
};

#include "NiShadowCubeMap.inl"

#endif  // #ifndef NISHADOWCUBEMAP_H
