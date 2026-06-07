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

#include "MaxImmerse.h"
#include "NiMAXLightConverter.h"
#include "NiMAXHierConverter.h"
#include "NiMAXAnimationConverter.h"
#include "NiMAXTextureConverter.h"

// Note: Make sure to verify these values when adding support for 
// a new version of 3ds Max.
#define PB_INTENSITY    1 // from maxsdk/samples/objects/light.cpp 
#define PBLOCK_REF      0 // from maxsdk/samples/objects/light.h

const NiPoint3 NiMAXLightConverter::ms_akDefaultLightDirs[NUM_DEF_LIGHTS] = 
{
    NiPoint3(0.9f,  1.0f, -1.5f),
    NiPoint3(-.9f, -1.0f,  1.5f)
};

bool NiMAXLightConverter::ms_bConvertLights = true;

NiTPointerList<NiMAXLightConverter::LightInfo *> 
NiMAXLightConverter::ms_kLightList;

void* NiMAXLightConverter::ms_pvUserData = NULL;

const char* gpcDefaultLightName = "__MAX_Default_Light";

//---------------------------------------------------------------------------
// Is "uiSize" a power of two
static bool NiIsPowerOfTwo(unsigned short usSize)
{
    unsigned short i;

    if (usSize != 1 && usSize % 2)     // powers of two are always even
        return(false);

    for (i = 1; usSize >= i; i *= 2)
    {
        if (usSize == i)
            return(true);
    }

    return(false);
}
//---------------------------------------------------------------------------
NiMAXLightConverter::NiMAXLightConverter(
    TimeValue kAnimStart,
    TimeValue kAnimEnd)
{
    m_kAnimStart = kAnimStart;
    m_kAnimEnd = kAnimEnd;
}
    
//---------------------------------------------------------------------------
int NiMAXLightConverter::Convert(
    Object* pkObj, 
    NiNode* pkCurNode, 
    INode* pkMaxNode, 
    ScaleInfo* pkParentScale)
{
    CHECK_MEMORY();
    LightInfo* pkLightInfo = NiNew LightInfo;

    if (pkLightInfo)
    {
        pkLightInfo->pkObj = pkObj;
        pkLightInfo->pkCurNode = pkCurNode;
        pkLightInfo->pkMaxNode = pkMaxNode;
        pkLightInfo->kScale = *pkParentScale;
        
        ms_kLightList.AddHead(pkLightInfo);
    }
    CHECK_MEMORY();

    return(W3D_STAT_OK);
}


//---------------------------------------------------------------------------
NiTextureEffect *NiMAXLightConverter::CreateNiTextureEffect(
    TimeValue kAnimStart,
    TimeValue kAnimEnd,
    GenLight* pkGL,
    LightState &kLState, 
    Texmap *pkTexMap, 
    NiTextureEffect::TextureType eType,
    BOOL bClip)
{
    CHECK_MEMORY();

    NIASSERT(eType == NiTextureEffect::PROJECTED_LIGHT ||
            eType == NiTextureEffect::PROJECTED_SHADOW ||
            eType == NiTextureEffect::ENVIRONMENT_MAP);

    unsigned int uiFlags;
    NiTexturingProperty::Map* pkMap;
    NiTexture::FormatPrefs kPrefs;
    NiFlipController* pkFlipper;

    NiMAXTextureConverter texture(kAnimStart, kAnimEnd);

    unsigned int uiWidth, uiHeight;
    texture.Convert(pkTexMap, pkMap, NiTexturingProperty::MAP_CLASS_BASE, 
        pkFlipper, kPrefs, uiFlags, uiWidth, uiHeight);

    // flip controller not supported on nitextureeffects
    NiDelete pkFlipper;
    
    NiTextureEffect* pkTE = 0;

    if (pkMap)
    {
        pkTE = NiNew NiTextureEffect;

        NiTexture* pkTexture = pkMap->GetTexture();

        pkTE->SetEffectTexture(pkTexture);
        pkTE->SetTextureType(eType);
        pkTE->SetTextureClamp(NiTexturingProperty::CLAMP_S_CLAMP_T);

        if (eType == NiTextureEffect::ENVIRONMENT_MAP)
        {
            pkTE->SetTextureFilter(NiTexturingProperty::FILTER_TRILERP);
            if (pkTexMap->ClassID() == Class_ID(ACUBIC_CLASS_ID,0))
            {
                pkTE->SetTextureCoordGen(NiTextureEffect::SPECULAR_CUBE_MAP);
            }
            else
            {
                pkTE->SetTextureCoordGen(NiTextureEffect::SPHERE_MAP);
            }
#if !defined(USE_OLD_SPHERICAL_ENVIRONMENT_MAP)
            NiMatrix3 kMatrix;
            kMatrix.SetRow(0, NiPoint3(0.0f, 0.0f, 1.0f));
            kMatrix.SetRow(1, NiPoint3(1.0f, 0.0f, 0.0f));
            kMatrix.SetRow(2, NiPoint3(0.0f, 1.0f, 0.0f));
            pkTE->SetRotate(kMatrix);
#endif
            bClip = FALSE;
        }
        else if (pkGL->IsDir())
        {
            pkTE->SetTextureFilter(NiTexturingProperty::FILTER_BILERP);
            NiMatrix3 kDirMat(NiMatrix3::ZERO);
            float fEntry = 0.5f/kLState.fallsize;
            kDirMat.SetEntry(0,0, fEntry);
            kDirMat.SetEntry(1,1, -fEntry);
            pkTE->SetModelProjectionMatrix(kDirMat);

            NiPoint3 kDirTrans(0.5f, 0.5f, 0.0f);
            pkTE->SetModelProjectionTranslation(kDirTrans);

            pkTE->SetTextureCoordGen(NiTextureEffect::WORLD_PARALLEL);
        }
        else if (pkGL->IsSpot())
        {
            pkTE->SetTextureFilter(NiTexturingProperty::FILTER_BILERP);
            // use fallsize as angle of projection
            float fEntry = 0.5f/(float)tan((NI_PI/360.0f)*kLState.fallsize);

            NiMatrix3 kSpotMat;
            kSpotMat.SetCol( 0, -fEntry, 0.0f, 0.0f);
            kSpotMat.SetCol( 1, 0.0f, fEntry, 0.0f);
            kSpotMat.SetCol( 2, 0.5f, 0.5f, 1.0f);

            pkTE->SetModelProjectionMatrix(kSpotMat);

            pkTE->SetModelProjectionTranslation(NiPoint3::ZERO);
            pkTE->SetTextureCoordGen(NiTextureEffect::WORLD_PERSPECTIVE);
        }
        else // omni projection not supported
        {
            NIASSERT(0);
            NiDelete pkTE;
            pkTE = 0;
        }
        // delete Map, it was only needed to hold the texture from the 
        // converter
        NiDelete pkMap;

        if (bClip == TRUE && pkTE != NULL)
        {
            // Directional and spot lights face down their negative z axis
            // in local space. Therefore the normal should match this direction
            NiPoint3 kNormal(0.0, 0.0, -1.0);
            NiPlane kPlane(kNormal, NiPoint3::ZERO);
            pkTE->SetClippingPlaneEnable(true);
            pkTE->SetModelClippingPlane(kPlane);
        }
    }
    CHECK_MEMORY();
    return pkTE;

}

//---------------------------------------------------------------------------
int NiMAXLightConverter::ReallyConvert(
    Object* pkObj,
    NiNode* pkCurNode, 
    INode* pkMaxNode,
    Interface*,
    TimeValue kAnimStart,
    TimeValue kAnimEnd,
    NiNode* pkRoot,
    ScaleInfo*)
{
    CHECK_MEMORY();
    LightState kLState;

    if (!ms_bConvertLights)
        return(W3D_STAT_OK);

    NiDynamicEffect* pkDynEffect = NULL;

    // first extract all the light information
    GenLight* pkGL = (GenLight *) pkObj;

    Interval int00 = Interval(0,0);
    pkGL->EvalLightState(kAnimStart, int00, &kLState);

    // see if this should be converted to a dynamic texture effect

    Texmap* pkLightMap = pkGL->GetProjMap();
    Texmap* pkShadowMap = pkGL->GetShadowProjMap();

    BOOL bClip = FALSE;
    pkMaxNode->GetUserPropBool("NiClipPlane", bClip);

    if (pkLightMap && kLState.type != OMNI_LIGHT)
    {
        // create an nitextureeffect
        pkDynEffect = CreateNiTextureEffect(kAnimStart, kAnimEnd, pkGL,
            kLState, pkLightMap, NiTextureEffect::PROJECTED_LIGHT, bClip);
    }
    // if there are both light and shadow maps on the same light,
    // just do lightmap
    else if (pkShadowMap)
    {
        if (pkGL->GetLightAffectsShadow())
        {
            // environment map
            pkDynEffect = CreateNiTextureEffect(kAnimStart, kAnimEnd, pkGL,
                kLState, pkShadowMap, NiTextureEffect::ENVIRONMENT_MAP);
        }
        else if (kLState.type != OMNI_LIGHT)
        {
            // shadow map
            pkDynEffect = CreateNiTextureEffect(kAnimStart, kAnimEnd, pkGL,
                kLState, pkShadowMap, NiTextureEffect::PROJECTED_SHADOW,
                bClip);
        }
    }
    else
    {
        // build a NI light
        switch (kLState.type)
        {
        case OMNI_LIGHT:
            pkDynEffect = NiNew NiPointLight;
            break;
        case FSPOT_LIGHT:
        case TSPOT_LIGHT:
            pkDynEffect = NiNew NiSpotLight;
            break;
        case DIR_LIGHT:
            pkDynEffect = NiNew NiDirectionalLight;
            break;
        default:
            break;
        }

        if (pkDynEffect == NULL)
            return(W3D_STAT_NO_MEMORY);


        NiColor kLightAmb;
        kLightAmb.r = kLightAmb.g = kLightAmb.b = 0.0f;
        NiLight* pkLight = (NiLight *)pkDynEffect;
        pkLight->SetAmbientColor(kLightAmb);
    
        NiColor kLightCol;
        kLightCol.r = kLState.color.r;
        kLightCol.g = kLState.color.g;
        kLightCol.b = kLState.color.b;
        pkLight->SetDiffuseColor(kLightCol);
        pkLight->SetSpecularColor(kLightCol);
    
        float fIntens = kLState.intens;
        pkLight->SetDimmer(fIntens);

        NiPointLight* pkPointLight = NiDynamicCast(NiPointLight, 
            pkDynEffect);
        if(pkPointLight)
        {
            int iDecayType = pkGL->GetDecayType();

            // attenuation
            if (iDecayType == 0)  // constant "none"
            {
                pkPointLight->SetConstantAttenuation(1.0f);
                pkPointLight->SetLinearAttenuation(0.0f);
                pkPointLight->SetQuadraticAttenuation(0.0f);
            }
            else if (iDecayType == 1) // linear "inverse"
            {
                pkPointLight->SetConstantAttenuation(0.0f);
                pkPointLight->SetLinearAttenuation(4.0f/kLState.attenEnd);
                pkPointLight->SetQuadraticAttenuation(0.0f);
            }
            else // quadratic "inverse square"
            {
                pkPointLight->SetConstantAttenuation(0.0f);
                pkPointLight->SetLinearAttenuation(0.0f);
                pkPointLight->SetQuadraticAttenuation(
                    16.0f/(kLState.attenEnd*kLState.attenEnd));
            }

            NiSpotLight* pkSpotLight = NiDynamicCast(NiSpotLight,
                pkPointLight);
            // NiSpotLight is derived from NiPointLight
            if(pkSpotLight)
            {
                float fHotAngle = kLState.hotsize;
                float fFallAngle = kLState.fallsize;
            
                pkSpotLight->SetSpotAngle(fFallAngle/2.0f);
                pkSpotLight->SetInnerSpotAngle(fHotAngle/2.0f);
            }
        }

        // Must rotate the model direction <0,0,-1>
        NiMatrix3 kExtraRot(NiMatrix3::ZERO);

        kExtraRot.SetEntry(2,0,-1.0f);
        kExtraRot.SetEntry(0,1,1.0f);
        kExtraRot.SetEntry(1,2,-1.0f);

        pkDynEffect->SetRotate(kExtraRot);

    }

    if (pkDynEffect == NULL)
        return(W3D_STAT_NO_MEMORY);

    bool bLightOn = true;
    if (!kLState.on)
        bLightOn = false;
    pkDynEffect->SetSwitch(bLightOn);

    int iNumEntries = pkGL->GetExclusionList().Count();
    if (iNumEntries == 0) 
    {
        // attach scene to pkDynEffect for scope
        pkDynEffect->AttachAffectedNode(pkRoot);
    }
    else
    {
        if (pkGL->GetExclusionList().TestFlag(NT_AFFECT_SHADOWCAST) &&
            !pkGL->GetExclusionList().TestFlag(NT_AFFECT_ILLUM))
        {
            NiString kWarning;
            kWarning = "The light ";
            kWarning += (const char*)pkCurNode->GetName();
            kWarning += " uses an include/exclude list for shadow casting.\n"
                "This is not supported; use of the include/exclude " 
                "list will only apply to Illumination.\nUse object properties "
                "to set shadow casting/receiving flags.\n";
            NILOGWARNING((const char*)kWarning);
        }

        bool bIncludeList = 
            pkGL->GetExclusionList().TestFlag(NT_INCLUDE) ? true : false;
        if (!bIncludeList)
            pkDynEffect->AttachAffectedNode(pkRoot);
    
        int i;

        for (i = 0; i < iNumEntries; i++)
        {
            INode* pLitNode;
            pLitNode = (*pkGL->GetExclList())[i];

            if (!pLitNode)
                continue;

            NiNode* pkRetNode = NiMAXHierConverter::Lookup(pLitNode, NULL);
            if (pkRetNode)
            {
                /*
                 * We have already built this node - so just refer to it
                 */
                if (bIncludeList)
                {
                    pkDynEffect->AttachAffectedNode(pkRetNode);
                }
                else
                {
                    pkDynEffect->AttachUnaffectedNode(pkRetNode);
                }
                continue;
            }
        }
    }

    Control *pkCont;

    pkCont = pkGL->GetColorControl();
    NiLight* pkLight = NiDynamicCast(NiLight, pkDynEffect);

    if (pkCont && pkLight)
    {
        NiMAXAnimationConverter anim(kAnimStart, kAnimEnd);
    
        NiInterpolatorPtr spInterp = anim.ConvertPoint3Anim(pkCont);

        if (spInterp)
        {
            NiKeyBasedInterpolator* pkKeyInterp = NiDynamicCast(
                NiKeyBasedInterpolator, spInterp);

            if (pkKeyInterp && pkKeyInterp->GetKeyCount(0) != 0)
            {
                NiLightColorController *pkColorControl;
                pkColorControl = NiNew NiLightColorController;
                pkKeyInterp->FillDerivedValues(0);
                pkColorControl->SetInterpolator(spInterp);
                pkColorControl->ResetTimeExtrema();
                pkColorControl->SetTarget(pkLight);
            
                NiMAXAnimationConverter::SetORT(pkCont, pkColorControl);
            
                if (NiMAXConverter::GetUseAppTime())
                    pkColorControl->SetAnimType(NiTimeController::APP_TIME);
                else
                    pkColorControl->SetAnimType(NiTimeController::APP_INIT);
            }
        }
    }

    if (pkLight)
    {
        NiMAXAnimationConverter anim(kAnimStart, kAnimEnd);
        Animatable* pkPBAnimatable = pkGL->SubAnim(PBLOCK_REF);
        NIASSERT(pkPBAnimatable);
        if (PARAMETER_BLOCK_CLASS_ID == pkPBAnimatable->SuperClassID())
        {
            IParamBlock* pkBlock = (IParamBlock*)pkPBAnimatable;
            pkCont = pkBlock->GetController(PB_INTENSITY);
        
            if (pkCont)
            {
                NiInterpolatorPtr spInterp = anim.ConvertFloatAnim(pkCont);

                if (spInterp)
                {
                    NiKeyBasedInterpolator* pkKeyInterp = NiDynamicCast(
                        NiKeyBasedInterpolator, spInterp);

                    if (pkKeyInterp && pkKeyInterp->GetKeyCount(0) != 0)
                    {
                        NiLightDimmerController *pkDimmerControl;
                        pkDimmerControl = NiNew NiLightDimmerController;
                        pkKeyInterp->FillDerivedValues(0);
                        pkDimmerControl->SetInterpolator(spInterp);
                        pkDimmerControl->ResetTimeExtrema();
                        pkDimmerControl->SetTarget(pkLight);
                    
                        NiMAXAnimationConverter::SetORT(pkCont, 
                            pkDimmerControl);
                    
                        if (NiMAXConverter::GetUseAppTime())
                        {
                            pkDimmerControl->SetAnimType(
                                NiTimeController::APP_TIME);
                        }
                        else
                        {
                            pkDimmerControl->SetAnimType(
                                NiTimeController::APP_INIT);
                        }
                    }
                }
            }
        }
    }

    // get the shadow casting parameters
    // do not export shadow information for photometric lights
    if ((pkLight) && 
        (pkGL->GetShadow() != 0) && 
        !pkObj->IsSubClassOf(LIGHTSCAPE_LIGHT_CLASS))
    {
        ConvertShadows(pkCurNode, pkLight, pkGL);
    }

    pkCurNode->AttachChild(pkDynEffect);
    CHECK_MEMORY();

    return(W3D_STAT_OK);
}

//---------------------------------------------------------------------------
int NiMAXLightConverter::ConvertShadows(
    NiNode* pkCurNode, 
    NiLight* pkLight, 
    GenLight* pkGL)
{
    NiShadowGenerator* pkShadowGenerator = NiNew NiShadowGenerator(pkLight);

    // Get the map size suggestion
    unsigned short usMapSize = (unsigned short)pkGL->GetMapSize(0);
    if (NiIsPowerOfTwo(usMapSize))
    {
        pkShadowGenerator->SetSizeHint(usMapSize);
    }

    // find the extra data on this node for shadow technique
    NiShadowTechnique* pkTechnique = NULL;
    NiStringExtraData* pkTechniqueData = NiDynamicCast(NiStringExtraData, 
        pkCurNode->GetExtraData(SHADOW_TECHNIQUE_NAME_KEY));
    if (pkTechniqueData)
    {
        // find a matching technique from the shadow manager
        pkTechnique = NiShadowManager::GetActiveShadowTechnique(
            pkTechniqueData->GetValue());
    }

    if (pkTechnique != NULL)
    {
        pkShadowGenerator->SetShadowTechnique(pkTechnique);
    }
    else
    {
        // the chosen shadow technique doesn't exist
        NiString kWarning;
        if (pkTechniqueData)
        {
            kWarning = "The shadow technique \"";
            kWarning += (const char*)pkTechniqueData->GetValue();
            kWarning += "\" selected for the light \"";
            kWarning += (const char*)pkCurNode->GetName();
            kWarning += "\" does not exist in the shadow manager. ";
        }
        else
        {
            kWarning = "The shadow technique ";
            kWarning += "was not selected for the light \"";
            kWarning += (const char*)pkCurNode->GetName();
            kWarning += "\"";
        }

        // try to use the first technique instead
        pkTechnique = NiShadowManager::GetKnownShadowTechnique(0);
        if (pkTechnique)
        {
            pkShadowGenerator->SetShadowTechnique(pkTechnique);

            kWarning += "Using technique \"";
            kWarning += (const char*)pkTechnique->GetName();
            kWarning += "\" instead.";
        }
        else
        {
            kWarning += "\nNo alternative techniques found. No shadow ";
            kWarning += "generator will be created.";

            NiDelete pkShadowGenerator;
            return(W3D_STAT_OK);
        }

        NILOGWARNING((const char*)kWarning);
    }

    // find the extra data on this node for static
    NiBooleanExtraData* pkStaticData = NiDynamicCast(NiBooleanExtraData, 
        pkCurNode->GetExtraData(SHADOW_STATIC_KEY));
    if (pkStaticData)
    {
        pkShadowGenerator->SetStatic(pkStaticData->GetValue());
    }

    // find the extra data on this node for strictly observing size hint
    NiBooleanExtraData* pkForceSizeData = NiDynamicCast(NiBooleanExtraData, 
        pkCurNode->GetExtraData(SHADOW_FORCE_SIZE_KEY));
    if (pkForceSizeData)
    {
        pkShadowGenerator->SetStrictlyObserveSizeHint(
            pkForceSizeData->GetValue());
    }

    // find the extra data on this node for using back faces
    NiBooleanExtraData* pkBacfacingData = NiDynamicCast(NiBooleanExtraData, 
        pkCurNode->GetExtraData(SHADOW_BACKFACING_KEY));
    if (pkBacfacingData)
    {
        pkShadowGenerator->SetRenderBackfaces(
            pkBacfacingData->GetValue());
    }

    // find the extra data on this node for bias
    NiBooleanExtraData* pkAutoBiasData = NiDynamicCast(NiBooleanExtraData, 
        pkCurNode->GetExtraData(SHADOW_AUTOBIAS_KEY));
    if (pkAutoBiasData)
    {
        if (pkAutoBiasData->GetValue())
        {
            pkShadowGenerator->SetDepthBiasToDefault();
        }
        else
        {
            // if auto bias if off, grab max's bias parameter
            float fBias = pkGL->GetMapBias(0);
            pkShadowGenerator->SetDepthBias(fBias);
        }
    }

    // find the affected nodes that aren't receivers and/or casters
    const NiNodeList* pkAffectedList;
    pkAffectedList = &pkLight->GetAffectedNodeList();
    NiTListIterator kPos;
    kPos = pkAffectedList->GetHeadPos();
    while (kPos != NULL)
    {
        NiNode* pkAffectedNode;
        pkAffectedNode = pkAffectedList->GetNext(kPos);
        // traverse the sub-scene and add exclusions to the generator
        RecursiveFindShadowFlags(pkAffectedNode, pkShadowGenerator);
    }

    NiShadowManager::AddShadowGenerator(pkShadowGenerator);

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
void NiMAXLightConverter::RecursiveFindShadowFlags(
    NiNode* pkNode,
    NiShadowGenerator* pkShadowGenerator)
{
    // this function searches the scene graph for flags indicating that an 
    // object doesn't cast or receive shadows. When it finds them, it adds
    // them to the appropriate exclusion list of the shadow generator

    // non-casters own a boolean extra data 'NonShadowCaster'
    NiBooleanExtraData* pkNonCaster;
    pkNonCaster = NiDynamicCast(NiBooleanExtraData, pkNode->GetExtraData(
        NON_SHADOW_CASTER_KEY));
    if (pkNonCaster != NULL)
    {
        pkShadowGenerator->AttachUnaffectedCasterNode(pkNode);
    }
    // non-receivers own a boolean extra data 'NonShadowReceiver'
    NiBooleanExtraData* pkNonReceiver;
    pkNonReceiver = NiDynamicCast(NiBooleanExtraData, pkNode->GetExtraData(
        NON_SHADOW_RECEIVER_KEY));
    if (pkNonReceiver != NULL)
    {
        pkShadowGenerator->AttachUnaffectedReceiverNode(pkNode);
    }

    // recurse over children
    unsigned int ui;
    for (ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (NiIsKindOf(NiNode, pkChild))
        {
            RecursiveFindShadowFlags((NiNode*)pkChild, pkShadowGenerator);
        }
    }
}
//---------------------------------------------------------------------------
int NiMAXLightConverter::ConvertAmbientLight(
    Interface* pkIntf, 
    NiNode* pkRoot)
{
    CHECK_MEMORY();
    if (!ms_bConvertLights)
        return(W3D_STAT_OK);

    NiColor kLightCol;
    NiLight* pkLight;
    Interval kTmpValid = FOREVER;
    NiLightColorController *pkColorControl = NULL;

    Point3 kAmbCol = pkIntf->GetAmbient(m_kAnimStart, kTmpValid);

    pkLight = NiNew NiAmbientLight;
    if (pkLight == NULL)
        return(W3D_STAT_NO_MEMORY);

    kLightCol.r = kAmbCol.x;
    kLightCol.g = kAmbCol.y;
    kLightCol.b = kAmbCol.z;
    pkLight->SetAmbientColor(kLightCol);

    pkLight->SetDiffuseColor(NiColor(0.0f, 0.0f, 0.0f));
    pkLight->SetSpecularColor(NiColor(0.0f, 0.0f, 0.0f));
    pkLight->SetDimmer(1.0);
    pkLight->SetSwitch(true);

    // attach scene to pkLight for scope
    pkLight->AttachAffectedNode(pkRoot);

    pkRoot->AttachChild(pkLight);

    Control* pkCont = pkIntf->GetAmbientController();
    if (pkCont)
    {
        NiMAXAnimationConverter anim(m_kAnimStart, m_kAnimEnd);
        
        NiInterpolatorPtr spInterp = anim.ConvertPoint3Anim(pkCont);

        if (spInterp)
        {
            NiKeyBasedInterpolator* pkKeyInterp = NiDynamicCast(
                NiKeyBasedInterpolator, spInterp);

            if (pkKeyInterp && pkKeyInterp->GetKeyCount(0) != 0)
            {            
                pkColorControl = NiNew NiLightColorController;

                pkKeyInterp->FillDerivedValues(0);
                pkColorControl->SetInterpolator(spInterp);
                pkColorControl->ResetTimeExtrema();
                pkColorControl->SetTarget(pkLight);
                pkColorControl->SetAmbient(true);

                NiMAXAnimationConverter::SetORT(pkCont, pkColorControl);

                if (NiMAXConverter::GetUseAppTime())
                    pkColorControl->SetAnimType(NiTimeController::APP_TIME);
                else
                    pkColorControl->SetAnimType(NiTimeController::APP_INIT);
            }
        }
    }

    // if ambient light is black and non-animated
    if (!pkColorControl && 
        kLightCol.r == 0.0f && kLightCol.g == 0.0f && kLightCol.b == 0.0f)
    {
        // just delete it
        pkLight->DetachAllAffectedNodes();
        pkLight->GetParent()->DetachChild(pkLight);
    }
    CHECK_MEMORY();

    return(W3D_STAT_OK);
}

//---------------------------------------------------------------------------
int NiMAXLightConverter::ConvertDefaultLights(NiNode* pkRoot)
{
    CHECK_MEMORY();
    NiLight* pkLight;
    int i;

    if (!ms_bConvertLights)
        return(W3D_STAT_OK);

    for (i = 0; i < NUM_DEF_LIGHTS; i++)
    {
        // build the default lights
        pkLight = NiNew NiDirectionalLight;
        if (pkLight == NULL)
            return(W3D_STAT_NO_MEMORY);

        // Create matrix to rotate <1,0,0> to default light dir
        NiPoint3 kXVec = ms_akDefaultLightDirs[i];
        kXVec.Unitize();
        NiPoint3 kYVec = kXVec.Perpendicular();
        NiPoint3 kZVec = kXVec.UnitCross(kYVec);
        NiMatrix3 kExtraRot(kXVec,kYVec,kZVec);
        
        pkLight->SetRotate(kExtraRot);

        pkLight->SetAmbientColor(NiColor(0.0f, 0.0f, 0.0f));
        pkLight->SetDiffuseColor(NiColor(1.0f, 1.0f, 1.0f));
        pkLight->SetSpecularColor(NiColor(1.0f, 1.0f, 1.0f));
        pkLight->SetDimmer(1.0f);
        pkLight->SetSwitch(true);
        
        // attach scene to pkLight for scope
        pkLight->AttachAffectedNode(pkRoot);
        
        pkRoot->AttachChild(pkLight);

        // name the light for nioptimize
        pkLight->SetName(gpcDefaultLightName);
    }
    CHECK_MEMORY();

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
int NiMAXLightConverter::Finish(Interface* pkIntf, NiNode* pkRoot)
{
    CHECK_MEMORY();
    int iStatus;

    if (ms_kLightList.IsEmpty())
    {
        iStatus = ConvertDefaultLights(pkRoot); // m_spScene
        return(iStatus);
    }
    else
    {
        NiTPointerList<LightInfo *>* pkList = &ms_kLightList;

        NiTListIterator kIter = pkList->GetHeadPos();
        while (kIter)
        {
            LightInfo* pkLightInfo = pkList->GetNext(kIter);
            
            ReallyConvert(pkLightInfo->pkObj, pkLightInfo->pkCurNode,
                          pkLightInfo->pkMaxNode, pkIntf, m_kAnimStart,
                          m_kAnimEnd,
                          pkRoot, &pkLightInfo->kScale);
            
        }
    }

    CleanExtraData(pkRoot);
    CHECK_MEMORY();

    return(W3D_STAT_OK);
}

//---------------------------------------------------------------------------
int NiMAXLightConverter::CleanExtraData(NiObjectNET* pkObject)
{
    // this recursive function removes all of the extra data flags from the 
    // scene that were added by the plug-in to indicate that an object doesn't 
    // cast or receive shadows.

    // non-casters own a boolean extra data 'NonShadowCaster'
    NiBooleanExtraData* pkNonCaster;
    pkNonCaster = NiDynamicCast(NiBooleanExtraData, pkObject->GetExtraData(
        NON_SHADOW_CASTER_KEY));
    if (pkNonCaster != NULL)
    {
        pkObject->RemoveExtraData(NON_SHADOW_CASTER_KEY);
    }
    // non-receivers own a boolean extra data 'NonShadowReceiver'
    NiBooleanExtraData* pkNonReceiver;
    pkNonReceiver = NiDynamicCast(NiBooleanExtraData, pkObject->GetExtraData(
        NON_SHADOW_RECEIVER_KEY));
    if (pkNonReceiver != NULL)
    {
        pkObject->RemoveExtraData(NON_SHADOW_RECEIVER_KEY);
    }
    // lights own a boolean extra data 'NiShadowStatic'
    NiBooleanExtraData* pkStatic;
    pkStatic = NiDynamicCast(NiBooleanExtraData, pkObject->GetExtraData(
        SHADOW_STATIC_KEY));
    if (pkStatic != NULL)
    {
        pkObject->RemoveExtraData(SHADOW_STATIC_KEY);
    }
    // lights own a string extra data 'NiShadowTechnique'
    NiStringExtraData* pkTechnique;
    pkTechnique = NiDynamicCast(NiStringExtraData, pkObject->GetExtraData(
        SHADOW_TECHNIQUE_NAME_KEY));
    if (pkTechnique != NULL)
    {
        pkObject->RemoveExtraData(SHADOW_TECHNIQUE_NAME_KEY);
    }
    // lights own a boolean extra data 'NiShadowBackfacing'
    NiBooleanExtraData* pkBackfacingData;
    pkBackfacingData = NiDynamicCast(NiBooleanExtraData, 
        pkObject->GetExtraData(SHADOW_BACKFACING_KEY));
    if (pkBackfacingData != NULL)
    {
        pkObject->RemoveExtraData(SHADOW_BACKFACING_KEY);
    }
    // lights own a boolean extra data 'NiShadowForceSize'
    NiBooleanExtraData* pkForceSizeData;
    pkForceSizeData = NiDynamicCast(NiBooleanExtraData, pkObject->GetExtraData(
        SHADOW_FORCE_SIZE_KEY));
    if (pkForceSizeData != NULL)
    {
        pkObject->RemoveExtraData(SHADOW_FORCE_SIZE_KEY);
    }
    // lights own a boolean extra data 'NiShadowAutoBias'
    NiBooleanExtraData* pkAutoBiasData;
    pkAutoBiasData = NiDynamicCast(NiBooleanExtraData, pkObject->GetExtraData(
        SHADOW_AUTOBIAS_KEY));
    if (pkAutoBiasData != NULL)
    {
        pkObject->RemoveExtraData(SHADOW_AUTOBIAS_KEY);
    }

    // recurse over children
    if (NiIsKindOf(NiNode, pkObject))
    {
        NiNode* pkNode = (NiNode*)pkObject;
        for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            NiAVObject* pkChild = pkNode->GetAt(ui);
            CleanExtraData(pkChild);
        }
    }

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
void NiMAXLightConverter::Init()
{
    NiShadowManager::Initialize();
    NIASSERT(ms_kLightList.IsEmpty());
    NIASSERT(ms_bConvertLights);
}
//---------------------------------------------------------------------------
void NiMAXLightConverter::Shutdown()
{ 
    NiTPointerList<LightInfo *>* pkList;

    ms_bConvertLights = true;

    pkList = &ms_kLightList;
    NiTListIterator kIter = pkList->GetHeadPos();
    while (kIter)
    {
        NiDelete pkList->RemovePos(kIter);
    }
    
    ms_kLightList.RemoveAll();

    NiShadowManager::Shutdown();
}
//---------------------------------------------------------------------------


