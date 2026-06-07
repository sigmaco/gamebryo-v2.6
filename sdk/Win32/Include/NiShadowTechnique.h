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

#ifndef NISHADOWTECHNIQUE_H
#define NISHADOWTECHNIQUE_H

#include "NiFixedString.h"
#include "NiShadowManager.h"
#include "NiStandardMaterial.h"
#include "NiPixelFormat.h"

class NiMaterialFragmentNode;

class NIMAIN_ENTRY NiShadowTechnique : public NiMemObject
{
    NiDeclareRootRTTI(NiShadowTechnique);
public:
    enum
    {
        INVALID_SHADOWTECHNIQUE_ID = 16
    };

    NiShadowTechnique(const NiFixedString& kName, 
        const NiFixedString kReadFragmentName, 
        const NiFixedString kWriteFragmentName, 
        bool bUseCubeMapForPointLight = false,
        bool bWriteBatchable = false);

    NiShadowTechnique(const NiFixedString& kName, 
        const NiFixedString kDirReadFragmentName, 
        const NiFixedString kPointReadFragmentName, 
        const NiFixedString kSpotReadFragmentName, 
        const NiFixedString kDirWriteFragmentName, 
        const NiFixedString kPointWriteFragmentName, 
        const NiFixedString kSpotWriteFragmentName, 
        bool bUseCubeMapForPointLight,
        bool bWriteBatchable);

    inline virtual ~NiShadowTechnique(){};

    inline const NiFixedString& GetName() const;

    inline const NiFixedString& GetReadFragmentName(
        NiStandardMaterial::LightType eType) const;
    inline const NiFixedString& GetWriteFragmentName(
        NiStandardMaterial::LightType eType) const;
    
    inline unsigned short GetActiveTechniqueSlot();
    inline void SetActiveTechniqueSlot(unsigned short usSlot);
    
    inline unsigned short GetTechniqueID();
    inline void SetTechniqueID(unsigned short usID);

    inline bool IsWriteBatchable();
    inline void SetWriteBatchable(bool bBatchable);

    inline bool GetUseCubeMapForPointLight();
    inline void SetUseCubeMapForPointLight(bool bUseCubeMap);

    inline NiTexture::FormatPrefs& GetTextureFormatPrefs();
    inline void SetTextureFormatPrefs(NiTexture::FormatPrefs& pkPrefs);

    virtual void AppendPostProcessRenderClicks(NiShadowMap* pkShadowMap,
        NiShadowClickGenerator* pkClickGenerator);
    virtual void PrepareShadowMap(NiShadowMap* pkShadowMap);

    inline void SetDefaultDepthBias(NiStandardMaterial::LightType eType,
        bool bRenderBackfaces, float fDepthBias);
    inline float GetDefaultDepthBias(NiStandardMaterial::LightType eType,
        bool bRenderBackfaces);

    void SetDepthFormat(const NiPixelFormat* pkDepthFormat);
    const NiPixelFormat* GetDepthFormat() const;

protected:
    NiFixedString m_kName;

    NiFixedString m_kReadFragmentName[NiStandardMaterial::LIGHT_MAX];
    NiFixedString m_kWriteFragmentName[NiStandardMaterial::LIGHT_MAX];
    float m_afDefaultDepthBias[NiStandardMaterial::LIGHT_MAX * 2];

    NiTexture::FormatPrefs m_kFormatPrefs;

    unsigned short m_uiActiveTechniqueSlot;
    unsigned short m_usTechniqueID;
    bool m_bWriteBatchable;
    bool m_bUseCubeMapForPointLight;

    const NiPixelFormat* m_pkDepthFormat;
};


#include "NiShadowTechnique.inl"


#endif // NISHADOWTECHNIQUE_H
