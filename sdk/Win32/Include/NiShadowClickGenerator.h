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

#ifndef NISHADOWCLICKGENERATOR_H
#define NISHADOWCLICKGENERATOR_H

#include "NiRefObject.h"
#include "NiRTTI.h"
#include "NiTPointerList.h"
#include "NiSmartPointer.h"
#include "NiFixedString.h"
#include "NiShadowManager.h"

class NiShadowRenderClick;
class NiShadowGenerator;
class NiCamera;
class NiCullingProcess;
class NiSpotLight;
class NiPointLight;
class NiDirectionalLight;
class NiTexture;

class NIMAIN_ENTRY NiShadowClickGenerator : public NiRefObject
{
    NiDeclareRootRTTI(NiShadowClickGenerator);

public:
    NiShadowClickGenerator();
    virtual ~NiShadowClickGenerator();

    virtual bool GenerateRenderClicks(
        const NiTPointerList<NiShadowGeneratorPtr>& kGenerators) = 0;

    virtual void ReorganizeActiveShadowGenerators(
        NiTPointerList<NiShadowGeneratorPtr>& kActiveGenerators, 
        NiTPointerList<NiShadowGeneratorPtr>& kAllGenerators) = 0;

    inline const NiFixedString& GetName() const;

    NiViewRenderClick* PreparePostProcessClick(NiTexture* pkSourceTexture,
        NiRenderTargetGroup* pkDestRTG, NiMaterial* pkPostProcessMat);

    inline void AddRenderClick(NiRenderClick* pkRenderClick);

    void CleanAllPostProcessClicks();

    // *** begin Emergent internal use only ***
    void PrepareShadowGenerators(
        const NiTPointerList<NiShadowGeneratorPtr>& kGenerators);
    void PrepareShadowGenerator(NiShadowGenerator* pkGenerator);
    // *** end Emergent internal use only ***

protected:
    NiShadowClickGenerator(const NiFixedString& kName);
    
    virtual bool PrepareSpotLightShadowGenerator(
        NiShadowGenerator* pkGenerator) = 0;
    virtual bool PreparePointLightShadowGenerator(
        NiShadowGenerator* pkGenerator) = 0;
    virtual bool PrepareDirectionalLightShadowGenerator(
        NiShadowGenerator* pkGenerator) = 0;

    NiTObjectPtrSet<NiViewRenderClickPtr> kPostProcessClicks;
    unsigned int m_uiActivePostProcessClicks;

    const NiFixedString m_kName;
};

NiSmartPointer(NiShadowClickGenerator);

#include "NiShadowClickGenerator.inl"

#endif  // #ifndef NISHADOWCLICKGENERATOR_H
