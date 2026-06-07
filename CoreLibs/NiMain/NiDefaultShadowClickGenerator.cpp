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
#include "NiMainPCH.h"

#include "NiDefaultShadowClickGenerator.h"
#include "NiShadowGenerator.h"
#include "NiSpotLight.h"
#include "NiPointLight.h"
#include "NiDirectionalLight.h"
#include "NiNode.h"
#include "NiRenderedCubeMap.h"
#include "NiShadowTechnique.h"

NiImplementRTTI(NiDefaultShadowClickGenerator, NiShadowClickGenerator);

//---------------------------------------------------------------------------
NiDefaultShadowClickGenerator::NiDefaultShadowClickGenerator() :
    NiShadowClickGenerator(ms_RTTI.GetName())
{
    kVSMTechniqueName = "NiVSMShadowTechnique";
}
//---------------------------------------------------------------------------
NiDefaultShadowClickGenerator::NiDefaultShadowClickGenerator(
    NiFixedString kName) : NiShadowClickGenerator(kName)
{
    kVSMTechniqueName = "NiVSMShadowTechnique";
}
//---------------------------------------------------------------------------
NiDefaultShadowClickGenerator::~NiDefaultShadowClickGenerator()
{
}
//---------------------------------------------------------------------------
bool NiDefaultShadowClickGenerator::GenerateRenderClicks(
    const NiTPointerList<NiShadowGeneratorPtr>& kGenerators)
{
    m_uiActivePostProcessClicks = 0;
    NiShadowManager::ReorganizeActiveShadowGenerators();

    NiTListIterator kIter = kGenerators.GetHeadPos();
    while (kIter)
    {
        NiShadowGenerator* pkGenerator = kGenerators.GetNext(kIter);
        
        if (!pkGenerator)
            continue;

        bool bRegenerateRenderClicks = false;
        bool bUpdateShadowMaps = false;
        NiDynamicEffect* pkDynEffect = 
            pkGenerator->GetAssignedDynamicEffect();

        if (pkGenerator->GetActive() && pkDynEffect->GetSwitch())
        {
            if (pkGenerator->GetStatic())
            {
                if (pkGenerator->GetRenderViewsDirty() )
                {
                    bRegenerateRenderClicks = true;
                    bUpdateShadowMaps = true;
                }
                else if (pkGenerator->AreShadowMapsDirty())
                {
                    bUpdateShadowMaps = true;
                }
            }
            else
            {
                bUpdateShadowMaps = true;
                if (pkGenerator->GetRenderViewsDirty() )
                    bRegenerateRenderClicks = true;
            }
            
            if (bRegenerateRenderClicks || bUpdateShadowMaps)
            {
                // Render Clicks need to be regenerated. So regenerate the
                // render clicks and add them to the list of clicks to be
                // rendered so their shadow maps are up to date.
                bool bResults = false; 
                switch (pkDynEffect->GetEffectType())
                {
                case NiDynamicEffect::SPOT_LIGHT:
                case NiDynamicEffect::SHADOWSPOT_LIGHT:
                    bResults = HandleSpotLight(pkGenerator, 
                        (NiSpotLight*)pkDynEffect, 
                        bRegenerateRenderClicks, bUpdateShadowMaps);
                    break;
                case NiDynamicEffect::POINT_LIGHT:
                case NiDynamicEffect::SHADOWPOINT_LIGHT:
                    bResults = HandlePointLight(pkGenerator, 
                        (NiPointLight*)pkDynEffect, 
                        bRegenerateRenderClicks, bUpdateShadowMaps);
                    break;
                case NiDynamicEffect::DIR_LIGHT:
                case NiDynamicEffect::SHADOWDIR_LIGHT:
                    bResults = HandleDirectionalLight(pkGenerator,
                        (NiDirectionalLight*)pkDynEffect, 
                        bRegenerateRenderClicks, bUpdateShadowMaps);
                    break;
                default:
                    // Unsupported dynamic effect; do nothing.
                    NIASSERT(!"Unsupported NiDynamicEffect type");
                    break;
                }

                if (!bResults)
                {
                    // Failed to correctly produce the shadow render clicks 
                    // for the generator, so de-activate the generator.
                    pkGenerator->SetActive(false, true);
                }

            }

        }
    }

    return true;
}
//---------------------------------------------------------------------------
NiShadowMap* NiDefaultShadowClickGenerator::ObtainAndAssignShadowMap(
    NiShadowGenerator* pkGenerator, unsigned int uiSMIndex, 
    NiRenderObject* pkGeometry)
{
    NiShadowMap* pkShadowMap = 
        pkGenerator->RetrieveShadowMap(uiSMIndex, pkGeometry);

    // A shadow map has already been assigned.
    if (pkShadowMap)
        return pkShadowMap;

    // Get size hint from the shadow generator.
    unsigned short usSizeHint = pkGenerator->GetSizeHint();
    bool bExactSizeOnly = pkGenerator->GetStrictlyObserveSizeHint();

    NIASSERT(pkGenerator->GetShadowTechnique());
    NiShadowTechnique* pkTechnique = pkGenerator->GetShadowTechnique();
    NiTexture::FormatPrefs& kFormatPrefs =
        pkTechnique->GetTextureFormatPrefs();

    // The generator does not yet have a shadow map, so request
    // one from the shadow manager.
    NiTListIterator kSMIter = NULL;
        
    // First see if a shadow map matching the exact size if available.
    kSMIter = NiShadowManager::RequestClosestShadowMap(usSizeHint, usSizeHint, 
        kFormatPrefs, pkShadowMap, true);
    
    if (!kSMIter)
    {
        // Attempt to create a new shadow map of the desired size.
        pkShadowMap = NiShadowManager::CreateShadowMap(
            usSizeHint, usSizeHint, kFormatPrefs, 
            pkTechnique->GetDepthFormat());

        if (!pkShadowMap && bExactSizeOnly)
        {
            // Failed to create shadow map. This is most likely because 
            // the requested shadow map required more texture memory that
            // is allowed. This shadow generator will only accept shadow maps 
            // that exactly fit its request size. So exit out instead of trying
            // to find a shadow map of a different size.
            return NULL;
        }

        kSMIter = NiShadowManager::RequestClosestShadowMap(usSizeHint, 
            usSizeHint, kFormatPrefs, pkShadowMap, bExactSizeOnly);

        // Failed to find an available shadow map of any size.
        if (!kSMIter)
            return NULL;
    }
    NIASSERT(kSMIter);
    NIASSERT(pkShadowMap);

    // Assign the map to the generator.
    pkGenerator->AssignShadowMap(pkShadowMap, uiSMIndex, kSMIter, pkGeometry);

    return pkShadowMap;
}
//---------------------------------------------------------------------------
NiShadowCubeMap* NiDefaultShadowClickGenerator::ObtainAndAssignShadowCubeMap(
    NiShadowGenerator* pkGenerator, unsigned int uiSMIndex, 
    NiRenderObject* pkGeometry)
{
    NiShadowCubeMap* pkShadowMap = 
        NiDynamicCast(NiShadowCubeMap, 
        pkGenerator->RetrieveShadowMap(uiSMIndex, pkGeometry) );

    // A shadow map has already been assigned.
    if (pkShadowMap)
        return pkShadowMap;

    // Get size hint from the shadow generator.
    unsigned short usSizeHint = pkGenerator->GetSizeHint();
    bool bExactSizeOnly = pkGenerator->GetStrictlyObserveSizeHint();

    // Make sure we have a shadow technique
    NiShadowTechnique* pkTechnique = pkGenerator->GetShadowTechnique();
    if (!pkTechnique)
    {
        NILOG("Failed to find shadow technique.\n");
        return NULL;
    }

    NiTexture::FormatPrefs& kFormatPrefs =
        pkTechnique->GetTextureFormatPrefs();

    // The generator does not yet have a shadow map, so request
    // one from the shadow manager.
    NiTListIterator kSMIter = NULL;

    // First see if a shadow map matching the exact size if available.
    kSMIter = NiShadowManager::RequestClosestShadowCubeMap(
        usSizeHint, kFormatPrefs, pkShadowMap, true);
    
    // If requested map is not found, create a new map.
    if (!kSMIter)
    {
        // Attempt to create a new shadow map of the desired size.
        pkShadowMap = NiShadowManager::CreateShadowCubeMap(
            usSizeHint, kFormatPrefs, pkTechnique->GetDepthFormat());

        if (!pkShadowMap && bExactSizeOnly)
        {
            // Failed to create shadow map. This is most likely because 
            // the requested shadow map required more texture memory that
            // is allowed. This shadow generator will only accept shadow maps 
            // that exactly fit its request size. So exit out instead of trying
            // to find a shadow map of a different size.
            return NULL;
        }

        kSMIter = NiShadowManager::RequestClosestShadowCubeMap(
            usSizeHint, kFormatPrefs, pkShadowMap, bExactSizeOnly);

        // Failed to find an available shadow map of any size.
        if (!kSMIter)
            return NULL;
    }
    NIASSERT(kSMIter);
    NIASSERT(pkShadowMap);

    // Assign the map to the generator.
    pkGenerator->AssignShadowMap(pkShadowMap, uiSMIndex, kSMIter, pkGeometry);

    return pkShadowMap;
}
//---------------------------------------------------------------------------
bool NiDefaultShadowClickGenerator::PrepareSpotLightShadowGenerator(
    NiShadowGenerator* pkGenerator)
{
    pkGenerator->SetSMTextureAssignmentCallbackFunc(
        NiShadowGenerator::SingleShadowMapAssignmentCallback);
    pkGenerator->SetSMTextureRetrievalCallbackFunc(
        NiShadowGenerator::SingleShadowMapRetrievalCallback);
    pkGenerator->SetOwnedSMCallbackFunc(
        NiShadowGenerator::SingleShadowMapGetOwnedCallback);
    pkGenerator->SetSMCountCallbackFunc(
        NiShadowGenerator::SingleShadowMapCountCallback);
    pkGenerator->SetSMConstantAssignmentCallbackFunc(NULL);
    pkGenerator->SetSMConstantRetrievalCallbackFunc(
        NiShadowGenerator::DefaultGetShaderConstantCallback);
    pkGenerator->SetExtraDataCleanupCallbackFunc(
        NiShadowGenerator::SingleShadowMapCleanupCallback);

    return true;
}
//---------------------------------------------------------------------------
bool NiDefaultShadowClickGenerator::PreparePointLightShadowGenerator(
    NiShadowGenerator* pkGenerator)
{
    pkGenerator->SetSMTextureAssignmentCallbackFunc(
        NiShadowGenerator::SingleShadowMapAssignmentCallback);
    pkGenerator->SetSMTextureRetrievalCallbackFunc(
        NiShadowGenerator::SingleShadowMapRetrievalCallback);
    pkGenerator->SetOwnedSMCallbackFunc(
        NiShadowGenerator::SingleShadowMapGetOwnedCallback);
    pkGenerator->SetSMCountCallbackFunc(
        NiShadowGenerator::SingleShadowMapCountCallback);
    pkGenerator->SetSMConstantAssignmentCallbackFunc(NULL);
    pkGenerator->SetSMConstantRetrievalCallbackFunc(
        NiShadowGenerator::DefaultGetShaderConstantCallback);
    pkGenerator->SetExtraDataCleanupCallbackFunc(
        NiShadowGenerator::SingleShadowMapCleanupCallback);

    return false;
}
//---------------------------------------------------------------------------
bool NiDefaultShadowClickGenerator::PrepareDirectionalLightShadowGenerator(
    NiShadowGenerator* pkGenerator)
{
    pkGenerator->SetSMTextureAssignmentCallbackFunc(
        NiShadowGenerator::SingleShadowMapAssignmentCallback);
    pkGenerator->SetSMTextureRetrievalCallbackFunc(
        NiShadowGenerator::SingleShadowMapRetrievalCallback);
    pkGenerator->SetOwnedSMCallbackFunc(
        NiShadowGenerator::SingleShadowMapGetOwnedCallback);
    pkGenerator->SetSMCountCallbackFunc(
        NiShadowGenerator::SingleShadowMapCountCallback);
    pkGenerator->SetSMConstantAssignmentCallbackFunc(NULL);
    pkGenerator->SetSMConstantRetrievalCallbackFunc(
        NiShadowGenerator::DefaultGetShaderConstantCallback);
    pkGenerator->SetExtraDataCleanupCallbackFunc(
        NiShadowGenerator::SingleShadowMapCleanupCallback);

    return true;
}
//---------------------------------------------------------------------------
bool NiDefaultShadowClickGenerator::HandleSpotLight(
    NiShadowGenerator* pkGenerator, NiSpotLight* pkSpotLight,  
    bool bRegenerateViews, bool bUpdateShadowMaps)
{
    NIASSERT(pkSpotLight);

    // Get a shadow map for the generator.
    NiShadowMap* pkShadowMap = ObtainAndAssignShadowMap(pkGenerator, 0);

    if (!pkShadowMap)
        return false;
    
    NIASSERT(pkShadowMap->GetTextureType() == NiShadowMap::TT_SINGLE);

    // Get the render click for the shadow map.
    NiShadowRenderClick* pkShadowClick = pkShadowMap->GetRenderClick();
    NIASSERT(pkShadowClick);

    // Get the render view from the render click.
    NIASSERT(pkShadowClick->GetRenderViews().GetSize() > 0);
    Ni3DRenderView* pkShadowView = NiDynamicCast(Ni3DRenderView,
        pkShadowClick->GetRenderViews().GetHead());
    NIASSERT(pkShadowView);

    NiCamera* pkShadowCamera = pkShadowView->GetCamera();
    NIASSERT(pkShadowCamera);


    if (bRegenerateViews)
    {
        // Clear all scenes from the render view.
        pkShadowView->GetScenes().RemoveAll();

        // Iterate through affected nodes list, adding all affected
        // nodes to the render view.
        pkGenerator->GetCasterGeometryList(m_kGeometryList);
        NiTListIterator kCasterNodeIter =  m_kGeometryList.GetHeadPos();

        float fNearClippingDist = pkGenerator->GetNearClippingDist();
        float fFarClippingDist = pkGenerator->GetFarClippingDist();

        if (pkGenerator->GetFlags() & 
            NiShadowGenerator::AUTO_CALC_NEARFAR_MASK)
        {
            const NiPoint3& kLightPosition = pkSpotLight->GetWorldTranslate();
            float fMinDistance = 0.0f;
            float fMaxDistance = 0.0f;

            if (kCasterNodeIter)
            {
                // Use the near and far dist from the first geometry object
                // to get the min and max distance computation started.
                NiRenderObject* pkGeometry = m_kGeometryList.GetNext(
                    kCasterNodeIter);
                pkShadowView->AppendScene((NiAVObject*)pkGeometry);

                const NiBound& kFirstBound = pkGeometry->GetWorldBound();
                fMinDistance = (kFirstBound.GetCenter() -
                    kLightPosition).Length() - kFirstBound.GetRadius();
                fMaxDistance = (kFirstBound.GetCenter() -
                    kLightPosition).Length() + kFirstBound.GetRadius();

                while (kCasterNodeIter)
                {
                    NiRenderObject* pkGeom = 
                        m_kGeometryList.GetNext(kCasterNodeIter);
                    const NiBound& kBound = pkGeom->GetWorldBound();

                    const float fNearDistance = (kBound.GetCenter() -
                        kLightPosition).Length() - kBound.GetRadius();
                    const float fFarDistance = (kBound.GetCenter() -
                        kLightPosition).Length() + kBound.GetRadius();

                    fMinDistance = NiMin(fMinDistance, fNearDistance);
                    fMaxDistance = NiMax(fMaxDistance, fFarDistance);

                    pkShadowView->AppendScene((NiAVObject*)pkGeom);
                }

                if (fMinDistance < 1.0f)
                {
                    fMinDistance = 1.0f;
                }
            }

            if (pkGenerator->GetAutoCalcNearClippingDist())
            {
                fNearClippingDist = fMinDistance * 0.85f;
                pkGenerator->SetNearClippingDist(fNearClippingDist);
            }
            if (pkGenerator->GetAutoCalcFarClippingDist())
            {
                fFarClippingDist = fMaxDistance * 1.10f;
                pkGenerator->SetFarClippingDist(fFarClippingDist);
            }
        }
        else
        {
            while (kCasterNodeIter)
            {
                NiRenderObject* pkGeometry = 
                    m_kGeometryList.GetNext(kCasterNodeIter);
                pkShadowView->AppendScene((NiAVObject*)pkGeometry);
            }
        }

        // Update the camera frustum for the render view.
        float fAngle = pkSpotLight->GetSpotAngle();
        fAngle = fAngle * NI_PI / 180.0f;
        float fOffset = tanf(fAngle);

        NiFrustum kFrustum(-fOffset, fOffset, fOffset, -fOffset,
            fNearClippingDist, fFarClippingDist);
        pkShadowCamera->SetViewFrustum(kFrustum);
    }

    // Update the camera data for the render view.
    pkShadowCamera->SetTranslate(pkSpotLight->GetWorldTranslate());
    pkShadowCamera->SetRotate(pkSpotLight->GetWorldRotate());
    pkShadowCamera->SetScale(pkSpotLight->GetWorldScale());
    pkShadowCamera->Update(0.0f);

    pkShadowClick->SetProcessor(NiShadowManager::GetMaterialSwapProcessor(
        NiStandardMaterial::LIGHT_SPOT));

    if (bUpdateShadowMaps)
    {
        AddRenderClick(pkShadowClick);

        NiShadowTechnique* pkTechnique = pkGenerator->GetShadowTechnique();
        pkTechnique->PrepareShadowMap(pkShadowMap);
        pkTechnique->AppendPostProcessRenderClicks(pkShadowMap, this);
    }

    pkGenerator->SetRenderViewsDirty(false);
    return true;
}
//---------------------------------------------------------------------------
bool NiDefaultShadowClickGenerator::HandlePointLight(
    NiShadowGenerator* pkGenerator, NiPointLight* pkPointLight,
     bool bRegenerateViews, bool bUpdateShadowMaps)
{
    NIASSERT(pkPointLight);

    // Get a shadow map for the generator.
    NiShadowCubeMap* pkShadowCubeMap = 
        ObtainAndAssignShadowCubeMap(pkGenerator, 0);

    if (!pkShadowCubeMap)
        return false;

    NIASSERT(pkShadowCubeMap->GetTextureType() == NiShadowMap::TT_CUBE);

    float fNearClippingDist = pkGenerator->GetNearClippingDist();
    float fFarClippingDist = pkGenerator->GetFarClippingDist();

    for (unsigned int uiFace = 0; uiFace < 6; uiFace++)
    {
        // Get the render click for the shadow map.
        NiShadowRenderClick* pkShadowClick =
            pkShadowCubeMap->GetRenderClick((unsigned char)uiFace);
        NIASSERT(pkShadowClick);

        // Get the render view from the render click.
        NIASSERT(pkShadowClick->GetRenderViews().GetSize() > 0);
        Ni3DRenderView* pkShadowView = NiDynamicCast(Ni3DRenderView,
            pkShadowClick->GetRenderViews().GetHead());
        NIASSERT(pkShadowView);

        // Update the camera data for the render view.
        NiCamera* pkShadowCamera = pkShadowView->GetCamera();
        NIASSERT(pkShadowCamera);

        if (bRegenerateViews)
        {
            // Set all the faces of the cube map to be cleared to zero. Zero
            // is handled has a special case by the ShadowCubeMap material 
            // fragment in NiStandardMaterial. This is needed since depth 
            // squared is written to the cube map instead of a normalized 
            // depth and there is no way to clear the cubemaps color buffer 
            // to a value greater than 1.0f
            pkShadowClick->SetBackgroundColor(NiColorA(0, 0, 0, 0));

            // Clear all scenes from the render view.
            pkShadowView->GetScenes().RemoveAll();

            // Iterate through affected nodes list, adding all affected
            // nodes to the render view.
            pkGenerator->GetCasterGeometryList(m_kGeometryList);
            NiTListIterator kCasterNodeIter =  m_kGeometryList.GetHeadPos();

            if (pkGenerator->GetFlags() & 
                NiShadowGenerator::AUTO_CALC_NEARFAR_MASK)
            {
                const NiPoint3& kLightPosition =
                    pkPointLight->GetWorldTranslate();
                float fMaxDistance = 0;

                if (kCasterNodeIter)
                {
                    // Use the near and far dist from the first geometry object
                    // to get the min and max distance computation started.
                    NiRenderObject* pkGeometry = m_kGeometryList.GetNext(
                        kCasterNodeIter);
                    pkShadowView->AppendScene((NiAVObject*)pkGeometry);
                    const NiBound& kFirstBound = pkGeometry->GetWorldBound();

                    fMaxDistance = (kFirstBound.GetCenter() -
                        kLightPosition).Length() + kFirstBound.GetRadius();
               
                    while (kCasterNodeIter)
                    {
                        pkGeometry = m_kGeometryList.GetNext(kCasterNodeIter);
                        const NiBound& kBound = pkGeometry->GetWorldBound();
                        float fFarDistance = (kBound.GetCenter() -
                            kLightPosition).Length() + kBound.GetRadius();

                        fMaxDistance = NiMax(fFarDistance, fMaxDistance);

                        pkShadowView->AppendScene((NiAVObject*)pkGeometry);
                    }

                }

                if (uiFace == 0)
                {
                    if (pkGenerator->GetAutoCalcNearClippingDist())
                        fNearClippingDist = 1.0f;
                    if (pkGenerator->GetAutoCalcFarClippingDist())
                        fFarClippingDist = fMaxDistance * 1.10f;
                }
                else
                {
                    if (pkGenerator->GetAutoCalcFarClippingDist())
                    {
                        fFarClippingDist = 
                            NiMax(fFarClippingDist, 1.10f * fMaxDistance);
                    }
                }

            }
            else
            {
                while (kCasterNodeIter)
                {
                    NiRenderObject* pkGeometry = m_kGeometryList.GetNext(
                        kCasterNodeIter);
                    pkShadowView->AppendScene((NiAVObject*)pkGeometry);
                }
            }
        }

        pkShadowCamera->SetTranslate(pkPointLight->GetWorldTranslate());
        pkShadowCamera->Update(0.0f);

        // LookAtWorldPoint
        NiPoint3 kLookAtPoint = pkPointLight->GetWorldTranslate();
        NiPoint3 kUp;

        switch ((NiRenderedCubeMap::FaceID)uiFace)
        {
        case NiRenderedCubeMap::FACE_POS_X:
                kLookAtPoint += NiPoint3(1, 0, 0);
                kUp = NiPoint3(0, 1, 0);
            break;
        case NiRenderedCubeMap::FACE_NEG_X:
                kLookAtPoint += NiPoint3(-1, 0, 0);
                kUp = NiPoint3(0, 1, 0);
            break;
        case NiRenderedCubeMap::FACE_POS_Y:
                kLookAtPoint += NiPoint3(0, 1, 0);
                kUp = NiPoint3(0, 0, 1);
            break;
        case NiRenderedCubeMap::FACE_NEG_Y:
            kLookAtPoint += NiPoint3(0, -1, 0);
            kUp = NiPoint3(0, 0, -1);
            break;
        case NiRenderedCubeMap::FACE_POS_Z:
                kLookAtPoint += NiPoint3(0, 0, -1);
                kUp = NiPoint3(0, 1, 0);
            break;
        case NiRenderedCubeMap::FACE_NEG_Z:
                kLookAtPoint += NiPoint3(0, 0, 1);
                kUp = NiPoint3(0, 1, 0);
            break;
        default:
            NIASSERT(!"Unknown face");
            break;
        };

        pkShadowCamera->LookAtWorldPoint(kLookAtPoint, kUp);


        pkShadowCamera->SetScale(pkPointLight->GetWorldScale());
        pkShadowCamera->Update(0.0f);

        pkShadowClick->SetProcessor(NiShadowManager::GetMaterialSwapProcessor(
            NiStandardMaterial::LIGHT_POINT));

        if (bUpdateShadowMaps)
            AddRenderClick(pkShadowClick); 
    }

    if (bRegenerateViews)
    {
        if (pkGenerator->GetAutoCalcNearClippingDist())
        {
            pkGenerator->SetNearClippingDist(fNearClippingDist);
        }

        if (pkGenerator->GetAutoCalcFarClippingDist())
        {
            pkGenerator->SetFarClippingDist(fFarClippingDist);
        }

        // Now that we have a uniform set of near/far clipping planes apply
        // them to the view frustums to all the render views.
        for (unsigned int uiFace = 0; uiFace < 6; uiFace++)
        {
            // Get the render click for the shadow map.
            NiShadowRenderClick* pkShadowClick =
                pkShadowCubeMap->GetRenderClick((unsigned char)uiFace);
            NIASSERT(pkShadowClick);

            // Get the render view from the render click.
            NIASSERT(pkShadowClick->GetRenderViews().GetSize() > 0);
            Ni3DRenderView* pkShadowView = NiDynamicCast(Ni3DRenderView,
                pkShadowClick->GetRenderViews().GetHead());
            NIASSERT(pkShadowView);

            // Update the camera data for the render view.
            NiCamera* pkShadowCamera = pkShadowView->GetCamera();
            NIASSERT(pkShadowCamera);

            NiFrustum kFrustum(-1.0f, 1.0f, 1.0f, -1.0f,
                fNearClippingDist, fFarClippingDist);
            pkShadowCamera->SetViewFrustum(kFrustum);
        }
    }

    if (bUpdateShadowMaps)
    {
        NiShadowTechnique* pkTechnique = pkGenerator->GetShadowTechnique();
        pkTechnique->PrepareShadowMap(pkShadowCubeMap);
        pkTechnique->AppendPostProcessRenderClicks(pkShadowCubeMap, this);
    }

    pkGenerator->SetRenderViewsDirty(false);
    return true;
}
//---------------------------------------------------------------------------
bool NiDefaultShadowClickGenerator::HandleDirectionalLight(
    NiShadowGenerator* pkGenerator, NiDirectionalLight* pkDirLight,
    bool bRegenerateViews, bool bUpdateShadowMaps)
{
    NIASSERT(pkDirLight);

    // Get a shadow map for the generator.
    NiShadowMap* pkShadowMap = ObtainAndAssignShadowMap(pkGenerator, 0);
    
    if  (!pkShadowMap)
        return false;

    NIASSERT(pkShadowMap->GetTextureType() == NiShadowMap::TT_SINGLE);

    // Get the render click for the shadow map.
    NiShadowRenderClick* pkShadowClick =
        pkShadowMap->GetRenderClick();
    NIASSERT(pkShadowClick);

    // Get the render view from the render click.
    NIASSERT(pkShadowClick->GetRenderViews().GetSize() > 0);
    Ni3DRenderView* pkShadowView = NiDynamicCast(Ni3DRenderView,
        pkShadowClick->GetRenderViews().GetHead());
    NIASSERT(pkShadowView);

    NiCamera* pkShadowCamera = pkShadowView->GetCamera();
    NIASSERT(pkShadowCamera);

    if (bRegenerateViews)
    {
        // Clear all scenes from the render view.
        pkShadowView->GetScenes().RemoveAll();

        // Iterate through affected nodes list, adding all affected
        // nodes to the render view and merging the bounds of all the affected 
        // nodes into a single bound. The view frustum will then be computed 
        // from this bound.
        pkGenerator->GetCasterGeometryList(m_kGeometryList);
        NiTListIterator kCasterNodeIter =  m_kGeometryList.GetHeadPos();
        NiPoint3 kCameraPos;

        float fNearClippingDist = pkGenerator->GetNearClippingDist();
        float fFarClippingDist = pkGenerator->GetFarClippingDist();
        float fDirLightFrustumWidth = pkGenerator->GetDirLightFrusturmWidth();

        if (pkGenerator->GetFlags() & NiShadowGenerator::AUTO_CALC_FULL_MASK)
        {
            NiBound kBound;

            if (kCasterNodeIter)
            {
                // Start by using the bounds from the first node in the list.
                NiRenderObject* pkGeometry = 
                    m_kGeometryList.GetNext(kCasterNodeIter);
                pkShadowView->AppendScene((NiAVObject*)pkGeometry);
                kBound = pkGeometry->GetWorldBound();

                while (kCasterNodeIter)
                {
                    pkGeometry = m_kGeometryList.GetNext(kCasterNodeIter);

                    kBound.Merge(&pkGeometry->GetWorldBound());
                    pkShadowView->AppendScene((NiAVObject*)pkGeometry);
                }
            }           

            UpdateDirLightFrustum(&kBound, pkGenerator, pkDirLight, 
                &fNearClippingDist, &fFarClippingDist, 
                &fDirLightFrustumWidth, &kCameraPos);
        }
        else
        {
            while (kCasterNodeIter)
            {
                NiRenderObject* pkGeometry = 
                    m_kGeometryList.GetNext(kCasterNodeIter);
                pkShadowView->AppendScene((NiAVObject*)pkGeometry);
            }
            
            kCameraPos = pkDirLight->GetWorldTranslate();
        }

        NiFrustum kFrustum(-fDirLightFrustumWidth, fDirLightFrustumWidth, 
            fDirLightFrustumWidth, -fDirLightFrustumWidth, 
            fNearClippingDist, fFarClippingDist, true);

        pkShadowCamera->SetViewFrustum(kFrustum);
        pkShadowCamera->SetTranslate(kCameraPos);
    }
    else
    {
        if (pkGenerator->GetFlags() & NiShadowGenerator::AUTO_CALC_FULL_MASK)
        {

            // Determine if the direction of the directional light has changed.
            // If so force the render views to be regenerated.
            const float fEpsilon = 1e-06f;
            float fPackedOldDirection = 0;
            float fPackedCurrentDirection = 
                (pkDirLight->GetWorldDirection().x * 3) +
                (pkDirLight->GetWorldDirection().y * 2) +
                (pkDirLight->GetWorldDirection().z * 0.5f);
            if (!m_kDirLightMap.GetAt(pkDirLight, fPackedOldDirection) ||
                NiAbs(fPackedOldDirection - fPackedCurrentDirection) >
                fEpsilon)
            {
                m_kDirLightMap.SetAt(pkDirLight, fPackedCurrentDirection);

                NiBound kBound;
                NiPoint3 kCameraPos;
                float fNearClippingDist = pkGenerator->GetNearClippingDist();
                float fFarClippingDist = pkGenerator->GetFarClippingDist();
                float fDirLightFrustumWidth = 
                    pkGenerator->GetDirLightFrusturmWidth();

                NiTPointerList<NiAVObjectPtr>& kScenes = 
                    pkShadowView->GetScenes();

                NiTListIterator kIter = kScenes.GetHeadPos();
                if (kIter)
                {
                    // Start by using the bounds from the first node in 
                    // the list.
                    NiAVObjectPtr spObject = 
                        m_kGeometryList.GetNext(kIter);
                    kBound = spObject->GetWorldBound();

                    while (kIter)
                    {
                        spObject = m_kGeometryList.GetNext(kIter);
                        kBound.Merge(&spObject->GetWorldBound());
                    }
                }

                UpdateDirLightFrustum(&kBound, pkGenerator, pkDirLight, 
                    &fNearClippingDist, &fFarClippingDist, 
                    &fDirLightFrustumWidth, &kCameraPos);

                NiFrustum kFrustum(-fDirLightFrustumWidth, 
                    fDirLightFrustumWidth, fDirLightFrustumWidth, 
                    -fDirLightFrustumWidth, fNearClippingDist, 
                    fFarClippingDist, true);

                pkShadowCamera->SetViewFrustum(kFrustum);
                pkShadowCamera->SetTranslate(kCameraPos);

            }
        }
        else
        {
            pkShadowCamera->SetTranslate(pkDirLight->GetWorldTranslate());
        }
    }

    pkShadowCamera->SetRotate(pkDirLight->GetWorldRotate());
    pkShadowCamera->SetScale(pkDirLight->GetWorldScale());
    pkShadowCamera->Update(0.0f);

    pkShadowClick->SetProcessor(NiShadowManager::GetMaterialSwapProcessor(
        NiStandardMaterial::LIGHT_DIR));

    if (bUpdateShadowMaps)
    {
        AddRenderClick(pkShadowClick);

        NiShadowTechnique* pkTechnique = pkGenerator->GetShadowTechnique();
        pkTechnique->PrepareShadowMap(pkShadowMap);
        pkTechnique->AppendPostProcessRenderClicks(pkShadowMap, this);
    }

    pkGenerator->SetRenderViewsDirty(false);
    return true;
}
//---------------------------------------------------------------------------
void NiDefaultShadowClickGenerator::ReorganizeActiveShadowGenerators(
    NiTPointerList<NiShadowGeneratorPtr>& kActiveGenerators, 
    NiTPointerList<NiShadowGeneratorPtr>& kAllGenerators)
{
    if (kActiveGenerators.GetSize() == kAllGenerators.GetSize())
    {
        // If both lists are the same size there is no need to re-organize the
        // active shadow generators since all the shadow generators are active.
        return;
    }
    else if (NiShadowManager::GetMaxActiveShadowGenerators() >= 
        kAllGenerators.GetSize())
    {
        // If there is room to activate all of unactivated generators simply
        // activate them all and exit.
        NiTListIterator kIter = kAllGenerators.GetHeadPos();
        while (kIter)
        {
            kAllGenerators.GetNext(kIter)->SetActive(true);
        }
        return;
    }

    NiCamera* pkCamera = NiShadowManager::GetSceneCamera();
    if (!pkCamera)
        return;

    // Ensure the arrays are large enough.
    m_kShadowGeneratorsDistSquared.SetSize(kAllGenerators.GetSize());
    m_kSortedShadowGenerators.SetSize(kAllGenerators.GetSize());

    // Copy list into sorted array and compute dist-square of all 
    // shadow generators from the camera
    NiTListIterator kIter = kAllGenerators.GetHeadPos();
    NiPoint3 kCameraWorldPos = pkCamera->GetWorldLocation();
    unsigned int uiCounter = 0;
    while (kIter)
    {
        NiShadowGenerator* pkGenerator = kAllGenerators.GetNext(kIter);
        m_kSortedShadowGenerators.SetAt(uiCounter, pkGenerator);

        // Since directional lights have no position handle them as a special
        // case and give them priority over all other lights.
        if (pkGenerator->GetAssignedDynamicEffect()->GetEffectType() == 
            NiDynamicEffect::DIR_LIGHT ||
            pkGenerator->GetAssignedDynamicEffect()->GetEffectType() == 
            NiDynamicEffect::SHADOWDIR_LIGHT)
        {
            m_kShadowGeneratorsDistSquared.SetAt(uiCounter, 0);
        }
        else
        {
            NiPoint3 pkWorldPos = 
                pkGenerator->GetAssignedDynamicEffect()->GetWorldTranslate();

            float fDistSqrd = (pkWorldPos - kCameraWorldPos).SqrLength();

            m_kShadowGeneratorsDistSquared.SetAt(uiCounter, fDistSqrd);
        }

        uiCounter++;
    }

    // Perform simple bubble sort on array of shadow generators
    unsigned int uiGeneratorCount = m_kSortedShadowGenerators.GetSize();
    for (unsigned int uiBase = 0; uiBase < uiGeneratorCount-1; uiBase++)
    {
        for (unsigned int uiBubble = 0; uiBubble < uiGeneratorCount-1-uiBase;
            uiBubble++)
        {
            if (m_kShadowGeneratorsDistSquared.GetAt(uiBubble+1) <
                m_kShadowGeneratorsDistSquared.GetAt(uiBubble))
            {
                // Swap DistSqrd and shadow generator
                float fTmp = m_kShadowGeneratorsDistSquared.GetAt(uiBubble);
                m_kShadowGeneratorsDistSquared.SetAt(uiBubble, 
                    m_kShadowGeneratorsDistSquared.GetAt(uiBubble+1) );
                m_kShadowGeneratorsDistSquared.SetAt(uiBubble+1, fTmp);

                NiShadowGenerator* pkTmp = 
                    m_kSortedShadowGenerators.GetAt(uiBubble);
                m_kSortedShadowGenerators.SetAt(uiBubble, 
                    m_kSortedShadowGenerators.GetAt(uiBubble+1) );
                m_kSortedShadowGenerators.SetAt(uiBubble+1, pkTmp);
            }
        }
    }

    // Deactivate all shadow generators beyond the max active shadow generator
    // threshold.
    for (uiCounter = NiShadowManager::GetMaxActiveShadowGenerators(); 
        uiCounter < m_kSortedShadowGenerators.GetSize();
        uiCounter++)
    {
        m_kSortedShadowGenerators.GetAt(uiCounter)->SetActive(false, true);
    }

    // Now ensure all shadow generators < max active shadow generators are
    // active.
    for (uiCounter = 0; 
        uiCounter <  NiShadowManager::GetMaxActiveShadowGenerators();
        uiCounter++)
    {
        m_kSortedShadowGenerators.GetAt(uiCounter)->SetActive(true, true);
    }

}
//---------------------------------------------------------------------------
void NiDefaultShadowClickGenerator::UpdateDirLightFrustum(NiBound* pkBound, 
    NiShadowGenerator* pkGenerator, NiDirectionalLight* pkDirLight,
    float* pfNearClippingDist, float* pfFarClippingDist, float* pfFrustumWidth,
    NiPoint3* pkPosition)
{
    if (pkGenerator->GetAutoCalcNearClippingDist())
    {
        *pfNearClippingDist = pkBound->GetRadius();
        pkGenerator->SetNearClippingDist(*pfNearClippingDist);
    }
    if (pkGenerator->GetAutoCalcFarClippingDist())
    {
        *pfFarClippingDist = pkBound->GetRadius() * 3.0f;
        pkGenerator->SetFarClippingDist(*pfFarClippingDist);
    }

    if (pkGenerator->GetAutoCalcDirLightFrusturmWidth())
    {
        *pfFrustumWidth = pkBound->GetRadius();
        pkGenerator->SetDirLightFrusturmWidth(*pfFrustumWidth);
    }

    if (pkGenerator->GetAutoCalcDirLightFrusturmPosition())
    {
        float fCameraDistFromBoundCenter = pkBound->GetRadius() * 2.0f;
        *pkPosition = pkBound->GetCenter() - 
            (pkDirLight->GetWorldDirection() * fCameraDistFromBoundCenter);
    }
    else
    {
        *pkPosition = pkDirLight->GetWorldTranslate();
    }
}
//---------------------------------------------------------------------------
