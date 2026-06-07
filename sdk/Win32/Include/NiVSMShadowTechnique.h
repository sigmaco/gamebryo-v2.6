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

#ifndef NIVSMSHADOWTECHNIQUE_H
#define NIVSMSHADOWTECHNIQUE_H

#include "NiShadowTechnique.h"

class NiSingleShaderMaterial;

class NIMAIN_ENTRY NiVSMShadowTechnique : public NiShadowTechnique
{
    NiDeclareRTTI;
public:
    NiVSMShadowTechnique(const NiFixedString& kName, 
        const NiFixedString kReadFragmentName, 
        const NiFixedString kWriteFragmentName, 
        bool bUseCubeMapForPointLight = false,
        bool bWriteBatchable = false);

    NiVSMShadowTechnique(const NiFixedString& kName, 
        const NiFixedString kDirReadFragmentName, 
        const NiFixedString kPointReadFragmentName, 
        const NiFixedString kSpotReadFragmentName, 
        const NiFixedString kDirWriteFragmentName, 
        const NiFixedString kPointWriteFragmentName, 
        const NiFixedString kSpotWriteFragmentName, 
        bool bUseCubeMapForPointLight,
        bool bWriteBatchable);

    virtual ~NiVSMShadowTechnique();

    virtual void AppendPostProcessRenderClicks(NiShadowMap* pkShadowMap,
        NiShadowClickGenerator* pkClickGenerator);
    virtual void PrepareShadowMap(NiShadowMap* pkShadowMap);
    
    void SetBlurKernelSize(unsigned int uiKernelSize);
    unsigned int GetBlurKernelSize();
    
protected:

    bool PreparePostProcessData(NiShadowMap* pkShadowMap);

    bool m_bCanNotFindShaders;
    bool m_bBlurKernelSizeChanged;
    unsigned int m_uiBlurKernelSize;
    float m_fMaxWidth;
    float m_fMaxHeight;

    NiRenderedTexturePtr m_spPostProcessTexture;
    NiRenderTargetGroupPtr m_spPostProcessRTG;
    
    NiMaterialNodeLibraryPtr m_spVSMBlurNodeLibrary;
    NiFragmentMaterialPtr m_spGaussianBlur;
};


#endif // NIVSMSHADOWTECHNIQUE_H
