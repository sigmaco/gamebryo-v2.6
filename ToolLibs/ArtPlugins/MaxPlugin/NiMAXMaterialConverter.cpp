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
#include "NiMAXMaterialConverter.h"
#include "NiMAXAnimationConverter.h"
#include "NiMAXTextureConverter.h"
#include "NiMAXCustAttribConverter.h"
#include "NiMAXPlatformMaterial.h"
#include "NiMAXHierConverter.h"
#include "NiMAXDynamicEffectConverter.h"
#include "NiMAXNBTConverter.h"
#include <NiNBTGenerator.h>


//---------------------------------------------------------------------------
// The NiMAXMaterialConverter's static data
bool NiMAXMaterialConverter::ms_bSceneIsTrans = false;

void *NiMAXMaterialConverter::ms_pUserData = NULL;

//---------------------------------------------------------------------------
// Create an NiMaterialColorController for "pMat" using the keyframes from
// "pCont". The "eType" field specifies which color field of "pMat" to 
// animate while "fSelfIllum" is only used for sill-illumination 
// animations.
void NiMAXMaterialConverter::CreateAnimator(
    Control *pCont, 
    NiMaterialProperty *pMat,
    NiMaterialColorController::Field eType)
{
    CHECK_MEMORY();
    NiMAXAnimationConverter anim(m_animStart, m_animEnd);
    
    NiInterpolatorPtr spInterp = anim.ConvertPoint3Anim(pCont);

    if (!spInterp)
        return;
    NiKeyBasedInterpolator* pkKeyInterp = NiDynamicCast(
        NiKeyBasedInterpolator, spInterp);

    if (pkKeyInterp && pkKeyInterp->GetKeyCount(0))
    {
        NiMaterialColorController *pColorControl;

        pColorControl = NiNew NiMaterialColorController;
        NIASSERT(pColorControl);
    
        pkKeyInterp->FillDerivedValues(0);
        pColorControl->SetInterpolator(spInterp);
        pColorControl->ResetTimeExtrema();
        pColorControl->SetTarget(pMat);
        pColorControl->SetType(eType);

        NiMAXAnimationConverter::SetORT(pCont, pColorControl);
        
        if (NiMAXConverter::GetUseAppTime())
            pColorControl->SetAnimType(NiTimeController::APP_TIME);
        else
            pColorControl->SetAnimType(NiTimeController::APP_INIT);
    }
    CHECK_MEMORY();
}

//---------------------------------------------------------------------------
void NiMAXMaterialConverter::CreateSelfIllumAnimator(
    Control *pCont, 
    NiMaterialProperty *pMat,
    NiStdMat *pSM)
{
    CHECK_MEMORY();
    NiMAXAnimationConverter anim(m_animStart, m_animEnd);
    NiFloatKey *pKeys;
    NiFloatKey::KeyType eType;
    unsigned int uiNumKeys, i;
    
    NiInterpolatorPtr spInterp = anim.ConvertFloatAnim(pCont);
    NiFloatInterpolator* pkFloatInterp = NiDynamicCast(NiFloatInterpolator, 
        spInterp);
    
    if (pkFloatInterp && pkFloatInterp->GetKeyCount(0))
    {
        NiMaterialColorController *pColorControl;
        
        Color diff;
        NiPosKey* pPosKeys;

        eType = pkFloatInterp->GetKeyType(0);
        pKeys = (NiFloatKey*) pkFloatInterp->GetKeyArray(0);
        uiNumKeys = pkFloatInterp->GetKeyCount(0);
        unsigned char ucSize = pkFloatInterp->GetKeyStride(0);

        unsigned int uiPosKeySize = 0;
        NiPosKey::KeyType ePosKeyType;

        if (eType == NiFloatKey::TCBKEY)
        {
            pPosKeys = NiNew NiTCBPosKey[uiNumKeys];
            uiPosKeySize = NiPosKey::GetKeySize(NiAnimationKey::TCBKEY);
            ePosKeyType = NiAnimationKey::TCBKEY;
        }
        else
        {
            pPosKeys = NiNew NiLinPosKey[uiNumKeys];
            uiPosKeySize = NiPosKey::GetKeySize(NiAnimationKey::LINKEY);
            ePosKeyType = NiAnimationKey::LINKEY;
        }
        NIASSERT(pPosKeys);
        
        for (i = 0; i < uiNumKeys; i++)
        {
            diff = pSM->GetDiffuse(int(TICKSPERSECOND *
                pKeys->GetKeyAt(i, ucSize)->GetTime()));

            if (eType == NiFloatKey::TCBKEY)
            {
                NiTCBFloatKey *pTCB;

                pTCB = (NiTCBFloatKey *) pKeys->GetKeyAt(i, ucSize);

                NiTCBPosKey* pkTCBPosKey = (NiTCBPosKey*)
                    pPosKeys->GetKeyAt(i, (unsigned char)uiPosKeySize);
                pkTCBPosKey->SetTime(pTCB->GetTime());
                pkTCBPosKey->SetPos(NiPoint3(
                    pTCB->GetValue() * diff.r,
                    pTCB->GetValue() * diff.g,
                    pTCB->GetValue() * diff.b));
                pkTCBPosKey->SetTension(pTCB->GetTension());
                pkTCBPosKey->SetContinuity(pTCB->GetContinuity());
                pkTCBPosKey->SetBias(pTCB->GetBias());
            }
            else
            {
                NiLinPosKey* pkLinPosKey = (NiLinPosKey*)
                    pPosKeys->GetKeyAt(i, (unsigned char)uiPosKeySize);
                pkLinPosKey->SetTime(pKeys->GetKeyAt(i, ucSize)->GetTime());
                pkLinPosKey->SetPos(NiPoint3(
                    pKeys->GetKeyAt(i, ucSize)->GetValue() * diff.r,
                    pKeys->GetKeyAt(i, ucSize)->GetValue() * diff.g,
                    pKeys->GetKeyAt(i, ucSize)->GetValue() * diff.b));
            }
        }
        
        pColorControl = NiNew NiMaterialColorController;
        NIASSERT(pColorControl);
        
        // precalulate values for the splines
        NiPosKey::FillDerivedValsFunction pDeriv;
    
        pDeriv = NiPosKey::GetFillDerivedFunction(ePosKeyType);
        NIASSERT(pDeriv);

        (*pDeriv)((NiAnimationKey*)pPosKeys, uiNumKeys, (unsigned char)uiPosKeySize);

        NiPoint3Interpolator* pkInterp = NiNew NiPoint3Interpolator;
        pkInterp->SetKeys(pPosKeys, uiNumKeys, ePosKeyType);
        pColorControl->SetInterpolator(pkInterp);
        pColorControl->ResetTimeExtrema();
        pColorControl->SetTarget(pMat);
        pColorControl->SetType(NiMaterialColorController::SELF_ILLUM);
        
        NiMAXAnimationConverter::SetORT(pCont, pColorControl);
        
        if (NiMAXConverter::GetUseAppTime())
            pColorControl->SetAnimType(NiTimeController::APP_TIME);
        else
            pColorControl->SetAnimType(NiTimeController::APP_INIT);        
    }
    CHECK_MEMORY();
}

//---------------------------------------------------------------------------
void NiMAXMaterialConverter::ConvertAlphaAnim(
    Control *pCont, 
    NiMaterialProperty *pMat,
    NiAVObject *pTarget, 
    NiAlphaProperty::AlphaFunction srcBlend,
    NiAlphaProperty::AlphaFunction destBlend)
{
    CHECK_MEMORY();
    NiMAXAnimationConverter anim(m_animStart, m_animEnd);

    NiInterpolatorPtr spInterp = anim.ConvertFloatAnim(pCont);
    NiFloatInterpolator* pkFloatInterp = NiDynamicCast(NiFloatInterpolator,
        spInterp);

    if (pkFloatInterp && pkFloatInterp->GetKeyCount(0) != 0)
    {
        NiAlphaController *pAlphaControl;
        
        pAlphaControl = NiNew NiAlphaController;
        NIASSERT(pAlphaControl);        

        pkFloatInterp->FillDerivedValues(0);
        
        pAlphaControl->SetInterpolator(pkFloatInterp);
        pAlphaControl->ResetTimeExtrema();
        pAlphaControl->SetTarget(pMat);
        
        NiMAXAnimationConverter::SetORT(pCont, pAlphaControl);
        
        if (NiMAXConverter::GetUseAppTime())
            pAlphaControl->SetAnimType(NiTimeController::APP_TIME);
        else
            pAlphaControl->SetAnimType(NiTimeController::APP_INIT);
        
        CreateAlphaProp(pTarget, srcBlend, destBlend);
    }
    CHECK_MEMORY();
}

//---------------------------------------------------------------------------
// Create a simple NI material for an object that doesn't have a material
// in MAX.
int NiMAXMaterialConverter::CreateSimple(
    NiAVObject *pTarget, 
    INode *pMaxNode)
{
    CHECK_MEMORY();
    NiMaterialProperty *pMat;
    NiColor NiCol;
    int r, g, b;
    DWORD col;

    col = pMaxNode->GetWireColor();
    r = (int) GetRValue(col);
    g = (int) GetGValue(col);
    b = (int) GetBValue(col);

    pMat = NiNew NiMaterialProperty;
    NIASSERT(pMat);

    NiCol.r = r / 255.0f;
    NiCol.g = g / 255.0f;
    NiCol.b = b / 255.0f;
    pMat->SetAmbientColor(NiCol);
    pMat->SetDiffuseColor(NiCol);
    pMat->SetSpecularColor(NiCol);

    pMat->SetEmittance(NiColor(0.0f, 0.0f, 0.0f));
    pMat->SetShineness(0);
    pMat->SetAlpha(1.0);

    pTarget->AttachProperty(pMat);
    CHECK_MEMORY();

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
// Does "pDiffTm" contain an alpha channel?
bool NiMAXMaterialConverter::HasAlpha(Texmap *pDiffTm)
{
    BitmapTex* pBm;

    if (!pDiffTm)
        return(false);

    if (pDiffTm->ClassID() != Class_ID(BMTEX_CLASS_ID, 0))
        return(false);

    pBm = (BitmapTex*) pDiffTm;

    if (pBm->GetAlphaSource() != ALPHA_FILE)
        return(false);

    return(true);
}
//---------------------------------------------------------------------------
// Has the user signaled that "pDiffTm" is to be used for opacity and does
// it have an alpha channel?
bool NiMAXMaterialConverter::UseForOpacity(
    NiStdMat *pSm, 
    TimeValue animStart, 
    Texmap *pDiffTm)
{
    CHECK_MEMORY();
    Bitmap *pDiffuseBitMap, *pOpacityBitMap;
    BitmapTex *pDiffuseBMT, *pOpacityBMT;
    Texmap *pTm;

    if (!pSm->MapEnabled(ID_OP))
        return(false);

    // Check for diffuse texture map
    if (!pDiffTm)
        return(false);

    if (pDiffTm->ClassID() == Class_ID(VCOL_CLASS_ID, 0))
    {
        return(false);
    }

    if (pDiffTm->ClassID() != Class_ID(BMTEX_CLASS_ID, 0))
    {
        return(false);
    }

    pDiffuseBMT = (BitmapTex*) pDiffTm;

    pDiffuseBitMap = pDiffuseBMT->GetBitmap(animStart);
    if (pDiffuseBitMap == NULL)
        return(false);

    // Check for opacity map
    pTm = (BitmapTex*) pSm->GetSubTexmap(ID_OP);
    if (!pTm)
        return(false);

    if (pTm->ClassID() == Class_ID(VCOL_CLASS_ID, 0))
    {
        return(false);
    }

    if (pTm->ClassID() != Class_ID(BMTEX_CLASS_ID, 0))
    {
        return(false);
    }

    pOpacityBMT = (BitmapTex*) pTm;

    pOpacityBitMap = pOpacityBMT->GetBitmap(animStart);
    if (pOpacityBitMap == NULL)
        return(false);

    if (_tcscmp(pOpacityBMT->GetMapName(), pDiffuseBMT->GetMapName()))
        return(false);

    if (pOpacityBMT->GetAlphaSource() != ALPHA_FILE)
        return(false);
    CHECK_MEMORY();

    return(true);
}
//---------------------------------------------------------------------------
// Does "pSM" specify a MultiTextured material?
bool NiMAXMaterialConverter::IsMultiTextured(NiStdMat *pSM)
{
    CHECK_MEMORY();
    Texmap* pTm, *pSubTm;
    int i;

    if (!pSM->MapEnabled(ID_DI))
        return(false);

    pTm = (BitmapTex*) pSM->GetSubTexmap(ID_DI);
    if (!pTm)
        return(false);

    // There is a bitmap in the self-illum slot. It should be used as a
    // dark map
    Texmap *pSITm;

    pSITm = NULL;
    if (pSM->MapEnabled(ID_SI))
    {
        pSITm = (BitmapTex*) pSM->GetSubTexmap(ID_SI);
        if (pSITm && pSITm->ClassID() != Class_ID(BMTEX_CLASS_ID, 0))
            pSITm = NULL;
    }

    if (pSITm && pTm && pTm->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
        return(true);

    // The other types of MultiTexturing
    if (pTm->ClassID() == Class_ID(MIX_CLASS_ID, 0))
    {
        ISubMap *pMix = (ISubMap *) pTm;

        // There is a "Mix" shader in the diffuse slot. It should be
        // used to set up a decal map

        if (pMix->NumSubTexmaps() != 2 && pMix->NumSubTexmaps() != 3)
            return(false);

        for (i = 0; i < pMix->NumSubTexmaps(); i++)
        {
            pSubTm = pMix->GetSubTexmap(i);
            if (pSubTm == NULL || 
                pSubTm->ClassID() != Class_ID(BMTEX_CLASS_ID, 0))
                return(false);
        }

        return(true);
    }
    else if (pTm->ClassID() == Class_ID(COMPOSITE_CLASS_ID, 0))
    {

        ISubMap *pComp = (ISubMap *) pTm;

        // There is a "Composite" shader in the diffuse slot. It 
        // should be used to set up a glow map.

        if (pComp->NumSubTexmaps() != 2)
            return(false);

        for (i = 0; i < pComp->NumSubTexmaps(); i++)
        {
            pSubTm = pComp->GetSubTexmap(i);
            if (pSubTm == NULL || 
                pSubTm->ClassID() != Class_ID(BMTEX_CLASS_ID, 0))
                return(false);
        }

        return(true);
    }
    else if (pTm->ClassID() == Class_ID(RGBMULT_CLASS_ID, 0))
    {
        RGBMult *pMult = (RGBMult *) pTm;

        // There is an "RGB Mult" shader in the diffuse slot. It should
        // be used to set up a dark map.

        if (pMult->NumSubTexmaps() != 2)
            return(false);

        for (i = 0; i < pMult->NumSubTexmaps(); i++)
        {
            pSubTm = pMult->GetSubTexmap(i);
            if (pSubTm == NULL || 
                pSubTm->ClassID() != Class_ID(BMTEX_CLASS_ID, 0))
                return(false);
        }

        return(true);
    }
    CHECK_MEMORY();

    return(false);
}
//---------------------------------------------------------------------------
int NiMAXMaterialConverter::ConvertMultiTexture(
    NiStdMat *pSM,
    NiAVObject *pTarget,
    NiAlphaProperty::AlphaFunction srcBlend,
    NiAlphaProperty::AlphaFunction destBlend)
{
    CHECK_MEMORY();
    NiMAXTextureConverter texture(m_animStart, m_animEnd);
    bool bRequiresAlpha;
    Texmap *pTm, *pSubTm;
    unsigned int uiFlags;
    int i, iStatus;
    unsigned int uiWidth = 0;
    unsigned int uiHeight = 0;

    NIASSERT(pSM->MapEnabled(ID_DI));

    pTm = (BitmapTex*) pSM->GetSubTexmap(ID_DI);
    NIASSERT(pTm);

    NiTexture::FormatPrefs kPrefs;

    // self-illum slot --> dark map
    if (pTm->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
    {
        Texmap *pSITm;

        NIASSERT(pSM->MapEnabled(ID_SI));

        pSITm = (BitmapTex*) pSM->GetSubTexmap(ID_SI);
        NIASSERT(pSITm);

        NIASSERT(pSITm->ClassID() == Class_ID(BMTEX_CLASS_ID, 0));

        NiTexturingProperty::Map* pkDarkMap;

        NiFlipController* pkDarkFlipper;
        NiTObjectArray<NiTextureTransformControllerPtr> kDarkTTControllers;
        iStatus = texture.Convert(pSITm, pkDarkMap, 
            NiTexturingProperty::MAP_CLASS_BASE, pkDarkFlipper, kPrefs,
            uiFlags, uiWidth, uiHeight);
       
        if (iStatus)
            return(iStatus);

        m_pkNiMAXUV->AssignMapChannel(pkDarkMap, pkDarkFlipper, 
            kDarkTTControllers, pSITm);

        NiTexturingProperty::Map* pkBaseMap;

        NiFlipController* pkBaseFlipper;
        NiTObjectArray<NiTextureTransformControllerPtr> kBaseTTControllers;
        
        iStatus = texture.Convert(pTm, pkBaseMap, 
            NiTexturingProperty::MAP_CLASS_BASE, pkBaseFlipper, kPrefs, 
            uiFlags, uiWidth, uiHeight);
        if (iStatus)
            return(iStatus);

        m_pkNiMAXUV->AssignMapChannel(pkBaseMap, pkBaseFlipper, 
            kBaseTTControllers, pTm);

        bool bUseForOpacity = UseForOpacity(pSM, m_animStart, pTm);

        bool bRequiresAlpha = false;
    
        if ((uiFlags & NiMAXTextureConverter::HAS_ALPHA) && bUseForOpacity)
            bRequiresAlpha = true;

        NiTexturingProperty* pkTex = NiNew NiTexturingProperty;
        pkTex->SetApplyMode(NiTexturingProperty::APPLY_MODULATE);
        pkTex->SetBaseMap(pkBaseMap);
        pkTex->SetDarkMap(pkDarkMap);
        AssignFlipController(pkBaseFlipper, pkTex, pkBaseMap);
        AssignFlipController(pkDarkFlipper, pkTex, pkDarkMap);

        m_pkNiMAXUV->AssignTextureTransformControllers(kBaseTTControllers,
            pkTex, pkBaseMap);
        m_pkNiMAXUV->AssignTextureTransformControllers(kDarkTTControllers,
            pkTex, pkDarkMap);

        pTarget->AttachProperty(pkTex);

        if (bRequiresAlpha)
            CreateAlphaProp(pTarget, srcBlend, destBlend);

        float fSelfIllumAmt = pSM->GetTexmapAmt(ID_SI, m_animStart);

        NiVertexColorPropertyPtr spLitMode;
        
        spLitMode = NiNew NiVertexColorProperty;
        NIASSERT(spLitMode);

        if (fSelfIllumAmt > 0.8f)
        {
            spLitMode->SetSourceMode(
                NiVertexColorProperty::SOURCE_EMISSIVE);
            spLitMode->SetLightingMode(
                NiVertexColorProperty::LIGHTING_E);
        }
        else
        {
            spLitMode->SetSourceMode(
                NiVertexColorProperty::SOURCE_AMB_DIFF);
            spLitMode->SetLightingMode(
                NiVertexColorProperty::LIGHTING_E_A_D);
        }

        if (pTarget->GetProperty(NiProperty::VERTEX_COLOR) == NULL)
            pTarget->AttachProperty(spLitMode);
    }

    // mix shader in diffuse slot --> decal
    if (pTm->ClassID() == Class_ID(MIX_CLASS_ID, 0))
    {
        bool bAlphaSet;
        int iAlphaFrom = -1;

        ISubMap* pMix = (ISubMap *) pTm;

        // we allow 3 in this assertion, but it's not actually handled
        // by the converter.
        NIASSERT(pMix->NumSubTexmaps() == 2 || pMix->NumSubTexmaps() == 3);

        NiTexturingProperty::Map** ppkMaps = 
            NiAlloc(NiTexturingProperty::Map*, 2);
        NiFlipController** ppkFlipper = NiAlloc(NiFlipController*, 2);
        NiTObjectArray<NiTextureTransformControllerPtr>* pkTTCont = 
            NiNew NiTObjectArray<NiTextureTransformControllerPtr>[2];


        ppkMaps[0] = NULL;
        ppkMaps[1] = NULL;
        ppkFlipper[0] = NULL;
        ppkFlipper[1] = NULL;
        
        bAlphaSet = false;

        bRequiresAlpha = false;
        for (i = 0; i < 2; i++)
        {
            bool bUseForOpacity;

            pSubTm = pMix->GetSubTexmap(i);
            NIASSERT(pSubTm);
            NIASSERT(pSubTm->ClassID() == Class_ID(BMTEX_CLASS_ID, 0));

            bUseForOpacity = false;
            if (!bAlphaSet && UseForOpacity(pSM, m_animStart, pSubTm))
            {
                bUseForOpacity = true;
                iAlphaFrom = i;
                bAlphaSet = true;
            }

            iStatus = texture.Convert(pSubTm, ppkMaps[i], 
                NiTexturingProperty::MAP_CLASS_BASE,  ppkFlipper[i],
                kPrefs, uiFlags, uiWidth, uiHeight);

            if (iStatus)
            {
                if(ppkMaps[0])
                {
                    ppkMaps[0]->SetTexture(0);
                    NiDelete ppkMaps[0];
                }
                if(ppkMaps[1])
                {
                    ppkMaps[1]->SetTexture(0);
                    NiDelete ppkMaps[1];
                }
                NiDelete ppkFlipper[0];
                NiDelete ppkFlipper[1];
                
                NiFree(ppkMaps);
                NiFree(ppkFlipper);
                NiDelete[] pkTTCont;
                return(iStatus);
            }
            NIASSERT(ppkMaps[i]);

            m_pkNiMAXUV->AssignMapChannel(ppkMaps[i], ppkFlipper[i], 
                pkTTCont[i], pSubTm);

            if (iAlphaFrom == i &&
                ((uiFlags & NiMAXTextureConverter::HAS_ALPHA) && 
                    bUseForOpacity))
            {
                    bRequiresAlpha = true;
            }
        }

        NiTexturingProperty* pkTex = NiNew NiTexturingProperty;
        pkTex->SetApplyMode(NiTexturingProperty::APPLY_MODULATE);
        pkTex->SetBaseMap(ppkMaps[0]);
        pkTex->SetDecalMap(0, ppkMaps[1]);
        pTarget->AttachProperty(pkTex);

        AssignFlipController(ppkFlipper[0], pkTex, ppkMaps[0]);
        AssignFlipController(ppkFlipper[1], pkTex, ppkMaps[1]);
        m_pkNiMAXUV->AssignTextureTransformControllers(pkTTCont[0], 
            pkTex, ppkMaps[0]);
        m_pkNiMAXUV->AssignTextureTransformControllers(pkTTCont[1], 
            pkTex, ppkMaps[1]);

        if (bRequiresAlpha)
            CreateAlphaProp(pTarget, srcBlend, destBlend);

        NiFree(ppkMaps);
        NiFree(ppkFlipper);
        NiDelete[] pkTTCont;
    }
    
    // composite shader in diffuse slot --> glow
    else if (pTm->ClassID() == Class_ID(COMPOSITE_CLASS_ID, 0))
    {
        bool bAlphaSet;
        int iAlphaFrom = -1;

        ISubMap *pComp = (ISubMap *) pTm;

        NIASSERT(pComp->NumSubTexmaps() == 2);

        NiTexturingProperty::Map** ppkMaps = 
            NiAlloc(NiTexturingProperty::Map*, 2);
        NiFlipController** ppkFlipper = NiAlloc(NiFlipController*, 2);
        NiTObjectArray<NiTextureTransformControllerPtr>* pkTTCont = 
            NiNew NiTObjectArray<NiTextureTransformControllerPtr>[2];

        bAlphaSet = false;

        ppkMaps[0] = NULL;
        ppkMaps[1] = NULL;
        ppkFlipper[0] = NULL;
        ppkFlipper[1] = NULL;

        bRequiresAlpha = false;
        for (i = 0; i < 2; i++)
        {
            bool bUseForOpacity;

            pSubTm = pComp->GetSubTexmap(i);
            NIASSERT(pSubTm);
            NIASSERT(pSubTm->ClassID() == Class_ID(BMTEX_CLASS_ID, 0));

            bUseForOpacity = false;
            if (!bAlphaSet && UseForOpacity(pSM, m_animStart, pSubTm))
            {
                bUseForOpacity = true;
                iAlphaFrom = i;
                bAlphaSet = true;
            }

            iStatus = texture.Convert(pSubTm, ppkMaps[i], 
                NiTexturingProperty::MAP_CLASS_BASE,  ppkFlipper[i], 
                kPrefs, uiFlags, uiWidth, uiHeight);

            if (iStatus)
            {
                if(ppkMaps[0])
                {
                    ppkMaps[0]->SetTexture(0);
                    NiDelete ppkMaps[0];
                }
                if(ppkMaps[1])
                {
                    ppkMaps[1]->SetTexture(0);
                    NiDelete ppkMaps[1];
                }

                NiDelete ppkFlipper[0];
                NiDelete ppkFlipper[1];

                NiFree(ppkMaps);
                NiFree(ppkFlipper);
                NiDelete[] pkTTCont;
                return(iStatus);
            }
            NIASSERT(ppkMaps[i]);

            m_pkNiMAXUV->AssignMapChannel(ppkMaps[i], ppkFlipper[i], 
                pkTTCont[i], pSubTm);

            if (iAlphaFrom == i &&
                ((uiFlags & NiMAXTextureConverter::HAS_ALPHA) && 
                    bUseForOpacity))
            {
                    bRequiresAlpha = true;
            }
        }


        NiTexturingProperty* pkTex = NiNew NiTexturingProperty;
        pkTex->SetApplyMode(NiTexturingProperty::APPLY_MODULATE);
        pTarget->AttachProperty(pkTex);

        if (iAlphaFrom == 1)
        {
            pkTex->SetBaseMap(ppkMaps[1]);
            pkTex->SetGlowMap(ppkMaps[0]);               
        }
        else
        {
            pkTex->SetBaseMap(ppkMaps[0]);
            pkTex->SetGlowMap(ppkMaps[1]);
        }

        AssignFlipController(ppkFlipper[0], pkTex, ppkMaps[0]);
        AssignFlipController(ppkFlipper[1], pkTex, ppkMaps[1]);
        m_pkNiMAXUV->AssignTextureTransformControllers(pkTTCont[0], 
            pkTex, ppkMaps[0]);
        m_pkNiMAXUV->AssignTextureTransformControllers(pkTTCont[1], 
            pkTex, ppkMaps[1]);

        if (bRequiresAlpha)
            CreateAlphaProp(pTarget, srcBlend, destBlend);

        NiFree(ppkMaps);
        NiFree(ppkFlipper);

    }

    // RGBmultiply shader in diffuse slot --> dark map
    else if (pTm->ClassID() == Class_ID(RGBMULT_CLASS_ID, 0))
    {
        int iAlphaFrom = -1;
        RGBMult *pMult;

        pMult = (RGBMult *) pTm;

        if (pMult->alphaFrom == 0 || pMult->alphaFrom == 1)
            iAlphaFrom = pMult->alphaFrom;

        NIASSERT(pMult->NumSubTexmaps() == 2);

        NiTexturingProperty::Map** ppkMaps = 
            NiAlloc(NiTexturingProperty::Map*, 2);
        NiFlipController** ppkFlipper = NiAlloc(NiFlipController*, 2);
        NiTObjectArray<NiTextureTransformControllerPtr>* pkTTCont =
            NiNew NiTObjectArray<NiTextureTransformControllerPtr>[2];

        ppkMaps[0] = NULL;
        ppkMaps[1] = NULL;
        ppkFlipper[0] = NULL;
        ppkFlipper[1] = NULL;

        bRequiresAlpha = false;
        for (i = 0; i < 2; i++)
        {
            bool bUseForOpacity;

            pSubTm = pMult->GetSubTexmap(i);
            NIASSERT(pSubTm);
            NIASSERT(pSubTm->ClassID() == Class_ID(BMTEX_CLASS_ID, 0));

            // if this texture is where we get transparency from - save
            // the transparency
            bUseForOpacity = false;
            if (i == iAlphaFrom && UseForOpacity(pSM, m_animStart, pSubTm))
                bUseForOpacity = true;

            iStatus = texture.Convert(pSubTm, ppkMaps[i],  
                NiTexturingProperty::MAP_CLASS_BASE, ppkFlipper[i], 
                kPrefs, uiFlags, uiWidth, uiHeight);

            if (iStatus)
            {
                if(ppkMaps[0])
                {
                    ppkMaps[0]->SetTexture(0);
                    NiDelete ppkMaps[0];
                }
                if(ppkMaps[1])
                {
                    ppkMaps[1]->SetTexture(0);
                    NiDelete ppkMaps[1];
                }
                NiDelete ppkFlipper[0];
                NiDelete ppkFlipper[1];

                NiFree(ppkMaps);
                NiFree(ppkFlipper);
                return(iStatus);
            }

            NIASSERT(ppkMaps[i]);

            m_pkNiMAXUV->AssignMapChannel(ppkMaps[i], ppkFlipper[i],
                pkTTCont[i], pSubTm);

            if (iAlphaFrom == i &&
                ((uiFlags & NiMAXTextureConverter::HAS_ALPHA) && 
                    bUseForOpacity))
            {
                    bRequiresAlpha = true;
            }
        }

        NiTexturingProperty* pkTex = NiNew NiTexturingProperty;
        pkTex->SetApplyMode(NiTexturingProperty::APPLY_MODULATE);
        pTarget->AttachProperty(pkTex);

        if (iAlphaFrom == 0 || iAlphaFrom == -1)
        {
            pkTex->SetBaseMap(ppkMaps[0]);
            pkTex->SetDarkMap(ppkMaps[1]);
        }
        else
        {
            pkTex->SetBaseMap(ppkMaps[1]);
            pkTex->SetDarkMap(ppkMaps[0]);
        }
        AssignFlipController(ppkFlipper[0], pkTex, ppkMaps[0]);
        AssignFlipController(ppkFlipper[1], pkTex, ppkMaps[1]);
        m_pkNiMAXUV->AssignTextureTransformControllers(pkTTCont[0],
            pkTex, ppkMaps[0]);
        m_pkNiMAXUV->AssignTextureTransformControllers(pkTTCont[1],
            pkTex, ppkMaps[1]);


        if (bRequiresAlpha)
            CreateAlphaProp(pTarget, srcBlend, destBlend);

        NiDelete[] ppkMaps;
        NiDelete[] ppkFlipper;
    }
        
    CHECK_MEMORY();
    return(W3D_STAT_OK);
}

//---------------------------------------------------------------------------
// Simply create an NiAlphaProperty for "pTarget"
void NiMAXMaterialConverter::CreateAlphaProp(
    NiAVObject *pTarget,
    NiAlphaProperty::AlphaFunction src,
    NiAlphaProperty::AlphaFunction dest)
{
    CHECK_MEMORY();
    NiAlphaProperty *pAlpha;
    
    if (m_bMtlIsTrans)  // pTarget already has an NiAlphaProperty
        return;

    pAlpha = NiNew NiAlphaProperty;
    if (pAlpha)
    {
        m_bMtlIsTrans = ms_bSceneIsTrans = true;
        pAlpha->SetAlphaBlending(true);
        pAlpha->SetSrcBlendMode(src);
        pAlpha->SetDestBlendMode(dest);
        pTarget->AttachProperty(pAlpha);
    }
    CHECK_MEMORY();
}

//---------------------------------------------------------------------------
// Convert a single (non-Multi) material ("pkMaterial") to NI format. Attach 
// the resulting property(ies) to "pTarget".
int NiMAXMaterialConverter::ConvertSingle(
    Mtl *pkMaterial, 
    NiNode *pParent,
    NiAVObject *pTarget, 
    INode *pMaxNode)
{
    CHECK_MEMORY();
    NiAlphaProperty::AlphaFunction 
        srcBlend = NiAlphaProperty::ALPHA_ONE, 
        destBlend = NiAlphaProperty::ALPHA_ONE;
    float fShine, fSelfIllum, fOpacity, fShinStr;
    NiMaterialProperty *pMat = NULL;
    int iShading, iStatus;
    Color amb, diff, spec;
    Class_ID cID;
    NiStdMat *pSM;

    NiTexturingProperty* pkTex = NULL;

    if (pkMaterial == NULL)
    {
        iStatus = CreateSimple(pTarget, pMaxNode);
        return(iStatus);
    }

    cID = pkMaterial->ClassID();

    if (cID != Class_ID(CMTL_CLASS_ID, 0) &&
        cID != Class_ID(DMTL_CLASS_ID, 0))
    {
        // MI can only hangle CMTL & DMTL materials
        // bail out if given something else
        iStatus = CreateSimple(pTarget, pMaxNode);
        return(iStatus);
    }

    bool bParam = true;
    if (bParam)
    {
        // now to do some serious conversion
        pSM = (NiStdMat *) pkMaterial;
        Interval intForever = FOREVER;
        pSM->Update(0, intForever);
        
        // First get the simple material data out of MAX
        amb = pSM->GetAmbient(m_animStart);
        diff = pSM->GetDiffuse(m_animStart);
        spec = pSM->GetSpecular(m_animStart);
        
        fSelfIllum = pSM->GetSelfIllum(m_animStart);
        // Max's exported shine and shinestr ranges from 0.0-1.0, 
        //gamebryo expects 0-100
        fShine = 100.0f * pSM->GetShininess(m_animStart);
        fOpacity = pSM->GetOpacity(m_animStart);
        fShinStr = 100.0f * pSM->GetShinStr(m_animStart);
        
        iShading = pSM->GetShading();
        
        srcBlend  = NiAlphaProperty::ALPHA_SRCALPHA;
        destBlend = NiAlphaProperty::ALPHA_INVSRCALPHA;
        
        if (pSM->GetTransparencyType() == TRANSP_SUBTRACTIVE)
        {
            srcBlend  = NiAlphaProperty::ALPHA_ZERO;
            destBlend = NiAlphaProperty::ALPHA_SRCCOLOR;
        }
        else if (pSM->GetTransparencyType() == TRANSP_ADDITIVE)
        {
            srcBlend  = NiAlphaProperty::ALPHA_SRCALPHA;
            destBlend = NiAlphaProperty::ALPHA_ONE;
        }
        
        // Then create the NiMaterialProperty & fill in the simple data
        pMat = NiNew NiMaterialProperty;
        if (pMat == NULL)
            return(W3D_STAT_NO_MEMORY);
        
        pTarget->AttachProperty(pMat);
        
        pMat->SetAmbientColor(NiColor(amb.r, amb.g, amb.b));
        pMat->SetDiffuseColor(NiColor(diff.r, diff.g, diff.b));
        pMat->SetSpecularColor(NiColor(spec.r, spec.g, spec.b));
        pMat->SetEmittance(NiColor(fSelfIllum * diff.r,
            fSelfIllum * diff.g,
            fSelfIllum * diff.b));
        pMat->SetShineness(fShine);
        pMat->SetAlpha(fOpacity);
        
        pMat->SetName((const char*)pkMaterial->GetName());
        
        if (iShading == SHADE_CONST)
        {
            NiShadeProperty *pShade = NiNew NiShadeProperty;
            
            if (pShade)
            {
                pShade->SetSmooth(FALSE);
                pTarget->AttachProperty(pShade);
            }
        }
        
        if (fShinStr > 50.0f)
        {
            NiSpecularProperty *pSpec = NiNew NiSpecularProperty;
            
            if (pSpec)
            {
                pSpec->SetSpecular(TRUE);
                pTarget->AttachProperty(pSpec);
            }
        }
        
        if (fOpacity < 1.0f)
        {
            // create NiAlphaProperty if the material is transparent
            // and we haven't created one already
            CreateAlphaProp(pTarget, srcBlend, destBlend);
        }
        
        if (pSM->GetTransparencyType() == TRANSP_SUBTRACTIVE ||
            pSM->GetTransparencyType() == TRANSP_ADDITIVE)
        {
            // create an NiAlphaProperty if the material uses Additive
            // transparency. We need to do this since if the material is
            // black the object will be transparent even if fOpacity == 1.0f
            CreateAlphaProp(pTarget, srcBlend, destBlend);
        }
        
        if (pSM->GetWire())
        {
            NiWireframeProperty *pWire = NiNew NiWireframeProperty;
            
            if (pWire)
            {
                pWire->SetWireframe(TRUE);
                pTarget->AttachProperty(pWire);
            }
        }
        
        if (pSM->MapEnabled(ID_SI))
        {
            Texmap* pTm;
            
            pTm = (BitmapTex*) pSM->GetSubTexmap(ID_SI);
            if (pTm)
            {
                if (pTm->ClassID() == Class_ID(VCOL_CLASS_ID, 0) ||
                    pTm->ClassID() == PARTAGE_CLASSID)
                {
                    NiVertexColorPropertyPtr pLitMode;

                    pLitMode = NiNew NiVertexColorProperty;
                    NIASSERT(pLitMode);

                    float fSelfIllumAmt = 
                            pSM->GetTexmapAmt(ID_SI, m_animStart);

                    if (fSelfIllumAmt > 0.8f)
                    {
                        pLitMode->SetSourceMode(
                            NiVertexColorProperty::SOURCE_EMISSIVE);
                        pLitMode->SetLightingMode(
                            NiVertexColorProperty::LIGHTING_E);
                    }
                    else
                    {
                        pLitMode->SetSourceMode(
                            NiVertexColorProperty::SOURCE_AMB_DIFF);
                        pLitMode->SetLightingMode(
                            NiVertexColorProperty::LIGHTING_E_A_D);
                    }
                    
                    if (pTarget->GetProperty(NiProperty::VERTEX_COLOR) ==
                        NULL)
                    {
                        pTarget->AttachProperty(pLitMode);
                    }
                }
            }
        }

        // add an AlphaProperty if the particle age material is being used
        // for opacity and all the alphas are not opaque. Note that this
        // can erroneously generate an alpha property if a particle age 
        // material is in the opacity slot of a non parrticle system object
        if (pSM->MapEnabled(ID_OP))
        {
            Texmap *pTm;

            pTm = (BitmapTex*) pSM->GetSubTexmap(ID_OP);
            if (pTm)
            {
                if (pTm->ClassID() == PARTAGE_CLASSID)
                {
                    PartAgeTex *pPartAge = (PartAgeTex *) pTm;
                    float fAlpha0, fAlpha1, fAlpha2;

                    fAlpha0 = (pPartAge->col1.r + pPartAge->col1.g + 
                                                pPartAge->col1.b) / 3.0f;
                    fAlpha1 = (pPartAge->col2.r + pPartAge->col2.g + 
                                                pPartAge->col2.b) / 3.0f;
                    fAlpha2 = (pPartAge->col3.r + pPartAge->col3.g + 
                                                pPartAge->col3.b) / 3.0f;

                    if (fAlpha0 < 1.0f || fAlpha1 < 1.0f || fAlpha2 < 1.0f)
                        CreateAlphaProp(pTarget, srcBlend, destBlend);
                }
            }
        }

        if (IsMultiTextured(pSM))
        {
            ConvertMultiTexture(pSM, pTarget, srcBlend, destBlend);
        }
        else if (pSM->MapEnabled(ID_DI) || pSM->MapEnabled(ID_RL))
        {
            NiMAXTextureConverter texture(m_animStart, m_animEnd);
            bool bUseForOpacity;

            bUseForOpacity = false;
            float fSelfIllumAmt = 0.0f;

            if (pSM->MapEnabled(ID_RL))
            {
                Texmap *pTm;
                unsigned int uiEnvMapFlags;
                NiTexturingProperty::Map* pkMap;
                NiTexture::FormatPrefs kPrefs;

                pTm = (BitmapTex*) pSM->GetSubTexmap(ID_RL);
                NiFlipController* pkFlipper;
                unsigned int uiWidth, uiHeight;
                texture.Convert(pTm, pkMap,  
                    NiTexturingProperty::MAP_CLASS_BASE, pkFlipper, kPrefs, 
                    uiEnvMapFlags, uiWidth, uiHeight,false);
                // flip controller not supported in NiTextureEffects
                NiDelete pkFlipper;

                if (pkMap)
                {
                    NiTexture* pkTexture = pkMap->GetTexture();
                    if (NiIsKindOf(NiSourceCubeMap, pkTexture))
                    {
                        NiMAXDynamicEffectConverter::
                            CreateCubeMapTextureEffect(
                            (NiSourceCubeMap*) pkTexture, pParent);
                    }
                    else if (NiIsKindOf(NiSourceTexture, pkTexture))
                    {
                        NiMAXDynamicEffectConverter::
                            CreateSphereMapTextureEffect(
                            (NiSourceTexture*) pkTexture, pParent);
                    }
                 
                    // delete Map, it was only needed to hold the texture 
                    // from the converter
                    NiDelete pkMap;
                }
            }

            if (pSM->MapEnabled(ID_DI))
            {
                Texmap *pTm;
                float fDiffuseAmt;
                unsigned int uiDiffFlags;
                NiTexturingProperty::Map* pkMap;
                NiTexture::FormatPrefs kPrefs;

                pTm = (BitmapTex*) pSM->GetSubTexmap(ID_DI);
                fSelfIllumAmt = pSM->GetTexmapAmt(ID_DI, m_animStart);

                NiFlipController* pkFlipper;
                NiTObjectArray<NiTextureTransformControllerPtr> kTTCont;
                unsigned int uiWidth, uiHeight;
                texture.Convert(pTm, pkMap,  
                    NiTexturingProperty::MAP_CLASS_BASE, pkFlipper, kPrefs, 
                    uiDiffFlags, uiWidth, uiHeight);
        
                fDiffuseAmt = pSM->GetTexmapAmt(ID_DI, m_animStart);
                if (fDiffuseAmt >= .991f )
                {
                    // if the texture is fully on then the material color
                    // does not matter - so set it to white.
                    pMat->SetDiffuseColor(NiColor(1.0f, 1.0f, 1.0f));
                }
                
                bUseForOpacity = UseForOpacity(pSM, m_animStart, pTm);

                if (uiDiffFlags & NiMAXTextureConverter::VERTCOL_MAP)
                {
                    NiVertexColorPropertyPtr pLitMode;
                    pLitMode = NiNew NiVertexColorProperty;

                    if (fSelfIllumAmt > 0.8f)
                    {
                        pLitMode->SetSourceMode(
                            NiVertexColorProperty::SOURCE_EMISSIVE);
                        pLitMode->SetLightingMode(
                            NiVertexColorProperty::LIGHTING_E);
                    }
                    else
                    {
                        pLitMode->SetSourceMode(
                            NiVertexColorProperty::SOURCE_AMB_DIFF);
                        pLitMode->SetLightingMode(
                            NiVertexColorProperty::LIGHTING_E_A_D);
                    }
                    
                    if (pTarget->GetProperty(NiProperty::VERTEX_COLOR) == 
                        NULL)
                    {
                        pTarget->AttachProperty(pLitMode);
                    }
                }
                else if (pkMap)
                {

                    pkTex = NiNew NiTexturingProperty;
                    pkTex->SetApplyMode(NiTexturingProperty::APPLY_MODULATE);
                    m_pkNiMAXUV->AssignMapChannel(pkMap, pkFlipper, kTTCont, 
                        pTm);

                    pkTex->SetBaseMap(pkMap);
                    pTarget->AttachProperty(pkTex);
                    AssignFlipController(pkFlipper, pkTex, pkMap);
                    m_pkNiMAXUV->AssignTextureTransformControllers(kTTCont, 
                        pkTex, pkMap);
                    
                    if ((uiDiffFlags & NiMAXTextureConverter::HAS_ALPHA) && 
                        bUseForOpacity)
                    {
                        CreateAlphaProp(pTarget, srcBlend, destBlend);
                    }
                    else if((uiDiffFlags & NiMAXTextureConverter::HAS_ALPHA))
                    {
                        char acString[512];
                        NiSprintf(acString, 512, "Map, \"%s\", has a DIFFUSE "
                            "MAP with alpha in the image,\nbut has no OPACITY "
                            "MAP. Please add the same texture as an OPACITY "
                            "MAP to apply alpha.\n", pSM->GetName());
                        NILOGWARNING(acString);
                    }
                }
            }
        }
    }

    if (cID == Class_ID(DMTL_CLASS_ID, 0))
    {
        NiParamBlock *pBlock;
        NiStdMat *pSM = (NiStdMat *) pkMaterial;
        
        pBlock = pSM->GetParamBlockByID(std2_extended);
        
        if (pBlock)
        {
            Control *pCont;

            pCont = pBlock->GetController(STDMTL_PB_OPAC);
            if (pCont)
                ConvertAlphaAnim(pCont, pMat, pTarget, srcBlend, destBlend);
        }

        Shader *pShader;
        
        pShader = (Shader *) pSM->GetReference(SHADER_REF);
        if (pShader == NULL)
            return(ND_CB_CONTINUE);
        
        NIASSERT(pShader->SuperClassID() == SHADER_CLASS_ID);
        
        pBlock = pShader->GetParamBlockByID(shdr_params);

        if (pBlock)
        {
            Control *pCont;
            
            pCont = pBlock->GetController(STDMTL_PB_AMBIENT);
            if (pCont)
            {
                CreateAnimator(pCont, pMat,
                    NiMaterialColorController::AMB);
            }
            
            pCont = pBlock->GetController(STDMTL_PB_DIFFUSE);
            if (pCont)
            {
                CreateAnimator(pCont, pMat,
                    NiMaterialColorController::DIFF);
            }

            pCont = pBlock->GetController(STDMTL_PB_SELFI);
            if (pCont)
            {
                CreateSelfIllumAnimator(pCont, pMat, pSM);
            }
            
            pCont = pBlock->GetController(STDMTL_PB_SPECULAR);
            if (pCont)
            {
                CreateAnimator(pCont, pMat,
                    NiMaterialColorController::SPEC);
            }
        }
    }

    CHECK_MEMORY();
    return(W3D_STAT_OK);
}

//---------------------------------------------------------------------------
void NiMAXMaterialConverter::ConvertDecal(
    NiMAXShader *pShader,
    Texmap *pTm,
    unsigned int uiIndex,
    NiTexturingProperty* pkTex,
    NiTexture::FormatPrefs& kPrefs,
    bool& bAreAnyTextures)
{
    NI_UNUSED_ARG(pShader);
    CHECK_MEMORY();
    NiMAXTextureConverter texture(m_animStart, m_animEnd);

    NiTexturingProperty::Map* pkMap;
    unsigned int uiFlags;
    NiFlipController* pkFlipper;
    NiTObjectArray<NiTextureTransformControllerPtr> kTTCont;
        
    unsigned int uiWidth, uiHeight;
    texture.Convert(pTm, pkMap,  NiTexturingProperty::MAP_CLASS_BASE,
        pkFlipper, kPrefs, uiFlags, uiWidth, uiHeight);
    if (pkMap)
    {
        m_pkNiMAXUV->AssignMapChannel(pkMap, pkFlipper, kTTCont, pTm);

        if (pkMap) // may have been deleted by AssignMapChannel
        {
            pkTex->SetDecalMap(uiIndex, pkMap);

            AssignFlipController(pkFlipper, pkTex, pkMap);
            m_pkNiMAXUV->AssignTextureTransformControllers(kTTCont, 
                pkTex, pkMap);

            bAreAnyTextures = true;
        }
    }
    CHECK_MEMORY();
}
// This must match what is in NiOptimization library file: 
// NiOptimize_HelpFunc.cpp
#define NI_BONES_PER_PARTITION_KEY "NiBonesPerPartition"

//---------------------------------------------------------------------------
void NiMAXMaterialConverter::AttachShaders(
    NiMAXShader* pkShader, 
    NiMesh* pkMesh,
    INode *pMaxNode)
{
    CHECK_MEMORY();
    if (!pkShader)
        return;

    if (!pkMesh)
        return;

        NiMAXTextureConverter::GetPlatform();

    // Setup the Normal/Binormal/Tangent method
    NiShaderRequirementDesc::NBTFlags eNBTMethod = 
        NiShaderRequirementDesc::NBT_METHOD_NONE;
    unsigned int uiNBTUVSet = NiShaderDesc::BINORMALTANGENTUVSOURCEDEFAULT;

    BOOL bUsesNBT = pkShader->GetUseNormalMap();
    BOOL bOverrideNBTMethod = bUsesNBT;
    unsigned int uiMaterialNBT = pkShader->GetNormalMapTechnique();

    NiTexturingProperty* pkTexProp = NiDynamicCast(NiTexturingProperty,
        pkMesh->GetProperty(NiProperty::TEXTURING));

    // Check the shader first it has higher priority
    NiString strShaderName = pkShader->GetShaderName();
    NiMaterialToolkit* pkToolkit = NiMaterialToolkit::GetToolkit();
    NiShaderDesc* pkShaderDesc = 
        pkToolkit->GetMaterialDesc(strShaderName);

    if(pkShaderDesc)
    {
        // Determine the NBT method and UV channel
        eNBTMethod = pkShaderDesc->GetBinormalTangentMethod();
        uiNBTUVSet = pkShaderDesc->GetBinormalTangentUVSource();
        if (eNBTMethod != NiShaderRequirementDesc::NBT_METHOD_NONE)
        {
            bUsesNBT = true;
        }
    }

    // Check to see if we have a normal map or parallax map.
    if (pkTexProp)
    {
        NiTexturingProperty::Map* pkNormalMap = 
            pkTexProp->GetNormalMap();

        NiTexturingProperty::Map* pkParallaxMap = 
            pkTexProp->GetParallaxMap();
        if (pkNormalMap || pkParallaxMap)
        {
            if (eNBTMethod == NiShaderRequirementDesc::NBT_METHOD_NONE)
            {
                switch(uiMaterialNBT)
                {
                case NiMAXShader::MAX_NBT_NDL:
                    eNBTMethod = NiShaderRequirementDesc::NBT_METHOD_NDL;
                    break;
                default:
                case NiMAXShader::MAX_NBT_ATI:
                    eNBTMethod = NiShaderRequirementDesc::NBT_METHOD_ATI;
                    break;
                case NiMAXShader::MAX_NBT_MAX:
                    eNBTMethod = NiShaderRequirementDesc::NBT_METHOD_DCC;
                    break;
                }
            }

            if (uiNBTUVSet == NiShaderDesc::BINORMALTANGENTUVSOURCEDEFAULT)
            {
                if (pkParallaxMap && pkNormalMap && 
                    pkParallaxMap->GetTextureIndex() != 
                    pkNormalMap->GetTextureIndex())
                {
                    char acString[1024];
                    NiSprintf(acString, 1024,"Parallax Map and Normal Map use"
                        " different UV sets, only normal map UV set will have"
                        " its NBT frame generated.");
                    NILOGWARNING(acString);

                    uiNBTUVSet = pkNormalMap->GetTextureIndex();
                }
                else if (pkParallaxMap)
                {
                    uiNBTUVSet = pkParallaxMap->GetTextureIndex();
                }
            }

            bUsesNBT = true;
        }
    }

    // On the Wii, check to see if a bump map is applied since having 
    // one requires NBT streams (for reflection vectors in the specular
    // case and true NBTs in the diffuse case)
    if (NiSystemDesc::GetSystemDesc().GetToolModeRendererID() == 
        NiSystemDesc::RENDERER_WII)
    {
        NiTexturingProperty::Map* pkBumpMap = NULL;
        if (pkTexProp)
        {
            pkBumpMap = pkTexProp->GetBumpMap();
        }

        if (pkBumpMap)
        {
            if (eNBTMethod == NiShaderRequirementDesc::NBT_METHOD_NONE)
            {
                switch(uiMaterialNBT)
                {
                case NiMAXShader::MAX_NBT_NDL:
                    eNBTMethod = NiShaderRequirementDesc::NBT_METHOD_NDL;
                    break;
                default:
                case NiMAXShader::MAX_NBT_ATI:
                    eNBTMethod = NiShaderRequirementDesc::NBT_METHOD_ATI;
                    break;
                case NiMAXShader::MAX_NBT_MAX:
                    eNBTMethod = NiShaderRequirementDesc::NBT_METHOD_DCC;
                    break;
                }
            }

            if (uiNBTUVSet == NiShaderDesc::BINORMALTANGENTUVSOURCEDEFAULT)
            {
                if (pkBumpMap)
                {
                    uiNBTUVSet = pkBumpMap->GetTextureIndex();
                }
            }

            bUsesNBT = true;
        }
    }

    //If the user selects NBT generation,
    //we want to override any shader-specified NBT methods while still
    //using previous UV sources, so allow the conditionals above to
    //execute even though this branch may override the NBT method.
    if (bOverrideNBTMethod)
    {
        switch(uiMaterialNBT)
        {
        case NiMAXShader::MAX_NBT_NONE:
            bUsesNBT = false;
            eNBTMethod = NiShaderRequirementDesc::NBT_METHOD_NONE;
        case NiMAXShader::MAX_NBT_NDL:
            eNBTMethod = NiShaderRequirementDesc::NBT_METHOD_NDL;
            break;
        default:
        case NiMAXShader::MAX_NBT_ATI:
            eNBTMethod = NiShaderRequirementDesc::NBT_METHOD_ATI;
            break;
        case NiMAXShader::MAX_NBT_MAX:
            eNBTMethod = NiShaderRequirementDesc::NBT_METHOD_DCC;
            break;
        }
    }

    if (bUsesNBT)
    {
        // Make sure we have a valid method
        if (eNBTMethod == NiShaderRequirementDesc::NBT_METHOD_NONE)
            eNBTMethod = NiShaderRequirementDesc::NBT_METHOD_ATI;

        // Make sure the uvset exists
        if (uiNBTUVSet == NiShaderDesc::BINORMALTANGENTUVSOURCEDEFAULT)
            uiNBTUVSet = 0;

        const NiDataStreamRef* pkTexCoordStreamRef = 
            pkMesh->FindStreamRef(NiCommonSemantics::TEXCOORD(),
            uiNBTUVSet,  NiDataStreamElement::F_FLOAT32_2);

        // If there are no UV coordinates, then don't generate NBT's
        if (bUsesNBT && !(pkTexCoordStreamRef))
        {
            NILOGWARNING("No UVs present. Aborting NBT generation.\n");
            bUsesNBT = false;
        }

        // Finally generate them.
        if (bUsesNBT)
        {
            NiMAXNBTConverter::SetupBinormalTangentData(
                pkMesh,
                (unsigned short)uiNBTUVSet,
                eNBTMethod);
        }
    }

    int iPhysXType;
    bool bHasPhysX =
        pMaxNode->GetUserPropInt("PhysicsType", iPhysXType) ? true : false;
    if (bHasPhysX && iPhysXType == 6)
    {
        NiIntegerExtraData* pkED = NiNew NiIntegerExtraData(uiNBTUVSet);
        pkMesh->AddExtraData("NBTSet", pkED);
    }

    if (NiStricmp(strShaderName, NI_DEFAULT_SHADER_NAME) != 0)
    {
        unsigned int uiTechnique = 0;
        if (NiMAXConverter::GetViewerRun())
            uiTechnique = pkShader->GetViewerShaderTechnique();
        else
            uiTechnique = pkShader->GetExportShaderTechnique();

        uiTechnique = 
            pkShader->ConvertNiMAXShaderToTechnique(uiTechnique);
        
        // Attach the proxy shader
        NiMaterialPtr spMaterial = NiMaterialLibrary::CreateMaterial(
            (const char*)strShaderName);

        if (spMaterial == NULL)
        {
            char acString[1024];
            NiSprintf(acString, 1024, "Material \"%s\" not found.", 
                strShaderName);
            NILOGWARNING(acString);
        }

        if (!pkMesh->ApplyAndSetActiveMaterial(spMaterial, uiTechnique))
        {
            char acString[1024];
            NiSprintf(acString, 1024, 
                "Material \"%s\" failed ApplyAndSetActiveMaterial call.",
                strShaderName);
            NILOGWARNING(acString);               
        }

        // If the shader does matrix palette skinning, set that up
        if (pkShaderDesc)
        {
            unsigned int uiBonesPerPartition = 4;
            uiBonesPerPartition = pkShaderDesc->GetBonesPerPartition();
            NiIntegerExtraData* pkBonesPerPart = 
                NiNew NiIntegerExtraData(uiBonesPerPartition);
            pkBonesPerPart->SetName(NI_BONES_PER_PARTITION_KEY);
            pkMesh->AddExtraData(pkBonesPerPart);
        }
    }
    
    CHECK_MEMORY();
}

//---------------------------------------------------------------------------
int NiMAXMaterialConverter::ConvertCustom(
    Mtl *pkMaterial, 
    NiNode *pParent,
    NiAVObject *pTarget,
    INode *pMaxNode,
    NiNode*& pkEffectNode, 
    bool bMultiMtl)
{
    CHECK_MEMORY();
    NiStdMat *pSM = (NiStdMat *) pkMaterial;
    NiMaterialProperty *pMat;
    NiMAXShader *pShader;

    // First, extract all the data from the custom shader
    Interval intForever = FOREVER;
    pSM->Update(0, intForever);

    pShader = (NiMAXShader *) ((NiStdMtl *) pkMaterial)->GetShader();

    NIASSERT(pShader->ClassID() == NISHADER_CLASSID);
   
    Color amb = pShader->GetAmbientClr(m_animStart);
    Color diff = pShader->GetDiffuseClr(m_animStart);
    Color spec = pShader->GetSpecularClr(m_animStart);
    Color emit = pShader->GetEmittance(m_animStart);
    float fShininess = pShader->GetShininess(m_animStart);
    float fAlpha = pShader->GetAlpha(m_animStart);

    unsigned char ucTestRef = pShader->GetTestRef(m_animStart);
    NiAlphaProperty::AlphaFunction eSrcBlend = 
          (NiAlphaProperty::AlphaFunction) pShader->GetSrcBlend(m_animStart);
    NiAlphaProperty::AlphaFunction eDestBlend = 
        (NiAlphaProperty::AlphaFunction) pShader->GetDestBlend(m_animStart);
    NiAlphaProperty::TestFunction eTestMode = 
        (NiAlphaProperty::TestFunction) 
        pShader->GetAlphaTestMode(m_animStart);
    bool bAlphaTestOn = pShader->GetAlphaTestOn(m_animStart) ? true : false;

    bool bAlphaBlendOn;
    bool bForceAlphaProperty;
    int iAlphaMode = pShader->GetAlphaMode(m_animStart);

    if (iAlphaMode == 0) // automatic 
    {
        bAlphaBlendOn = true;
        bForceAlphaProperty = false;
    }
    else if (iAlphaMode == IDC_ALPHA_NONE - IDC_ALPHA_AUTOMATIC) // none
    {
        bAlphaBlendOn = false;
        bForceAlphaProperty = false;
    }
    else // standard, additive, multiplicative, or advanced
    {
        bAlphaBlendOn = true;
        bForceAlphaProperty = true;
    }

    NiTexturingProperty::ApplyMode eApplyMode = 
        (NiTexturingProperty::ApplyMode) 
        pShader->GetTextureApplyMode(m_animStart);

    bool bUseVertCols = 
        pShader->GetVertexColorsOn(m_animStart) ? true : false;
    NiVertexColorProperty::SourceVertexMode eSrcVertexMode = 
        (NiVertexColorProperty::SourceVertexMode) 
        pShader->GetSourceVertexMode(m_animStart);
    NiVertexColorProperty::LightingMode eLightingMode = 
        (NiVertexColorProperty::LightingMode) 
        pShader->GetLightingMode(m_animStart);
    bool bSpecularOn = pShader->GetSpecularOn(m_animStart) ? true : false;
    bool bNoSorter = pShader->GetNoSorter(m_animStart) ? true : false;
    int iShading = pSM->GetShading();
    int iWire = pSM->GetWire();

    // kPrefs.m_eAlphaFmt can be left as ALPHA_DEFAULT,
    // since pixel format will determine whether the
    // preference has an alpha channel.  The only exception is 
    // BINARY, which is set for the base map if alpha testing 
    // is on, but alpha blending is off.
    NiTexture::FormatPrefs kPrefs;

    // Create the necessary Gamebryo properties

    // NiMaterialProperty
    pMat = NiNew NiMaterialProperty;
    NIASSERT(pMat);

    pMat->SetName((const char*)pkMaterial->GetName());

    pMat->SetAmbientColor(NiColor(amb.r, amb.g, amb.b));
    pMat->SetDiffuseColor(NiColor(diff.r, diff.g, diff.b));
    pMat->SetSpecularColor(NiColor(spec.r, spec.g, spec.b));
    pMat->SetEmittance(NiColor(emit.r, emit.g, emit.b));
    pMat->SetShineness(fShininess);
    pMat->SetAlpha(fAlpha);

    pTarget->AttachProperty(pMat);

    // NiVertexColorProperty
    if (bUseVertCols)
    {
        NiVertexColorPropertyPtr pVertCols;

        pVertCols = NiNew NiVertexColorProperty;
        NIASSERT(pVertCols);

        pVertCols->SetSourceMode(eSrcVertexMode);
        pVertCols->SetLightingMode(eLightingMode);

        if (pTarget->GetProperty(NiProperty::VERTEX_COLOR) == NULL)
        {
            if (NiIsKindOf(NiMesh, pTarget))
            {
                NiMesh* pkMesh = (NiMesh*) pTarget;

                const NiDataStreamRef* pkColorStreamRef =
                    pkMesh->FindStreamRef(NiCommonSemantics::COLOR());

                if (pkColorStreamRef == NULL )
                {                    
                    pVertCols->SetSourceMode(
                        NiVertexColorProperty::SOURCE_IGNORE);
                }
                else if (!bForceAlphaProperty)
                {

                    NiDataStreamElementLock kLock(
                        pkMesh, 
                        NiCommonSemantics::COLOR(), 0,    
                        NiDataStreamElement::F_UNKNOWN, 
                        NiDataStream::LOCK_TOOL_READ |  
                        NiDataStream::LOCK_TOOL_WRITE); 

                    // If the lock is not valid, then vertex colors 
                    // were not present on the mesh.
                    NIASSERT(kLock.IsLocked());

                    // Check if a vertex has an alpha value smaller than
                    // one.  If so, an alpha property must be attached
                    // to the object.
                    NiUInt32 uiCount = kLock.count();
                    NiUInt32 uiStride = kLock.GetDataStreamRef()->GetStride();
                    if (uiStride == 4)
                    {
                        NiTStridedRandomAccessIterator<NiRGBA> kColorIter = 
                            kLock.begin<NiRGBA>();
                        for (NiUInt32 ui = 0; ui < uiCount; ui++)
                        {
                            if(kColorIter[ui].a() < 255)
                            {
                                bForceAlphaProperty = true;
                                break;
                            }
                        }
                    }
                    else if (uiStride == 16)
                    {
                        NiTStridedRandomAccessIterator<NiColorA> kColorIter = 
                            kLock.begin<NiColorA>();
                        for (NiUInt32 ui = 0; ui < uiCount; ui++)
                        {
                            if(kColorIter[ui].a < 1.0f)
                            {
                                bForceAlphaProperty = true;
                                break;
                            }
                        }
                    }
                    else
                    {
                        NILOG("Unsupported COLOR format found \"%s\".",
                            kLock.GetDataStreamElement().GetFormatString());
                        NIASSERT(0);
                    }
                }
                pTarget->AttachProperty(pVertCols);
            }
        }
    }

    // NiSpecularProperty
    if (bSpecularOn)
    {
        NiSpecularProperty *pSpec;

        pSpec = NiNew NiSpecularProperty;
        NIASSERT(pSpec);

        pSpec->SetSpecular(true);

        pTarget->AttachProperty(pSpec);
    }

    // NiAlphaProperty
    if (fAlpha < 1.0f ||
        bAlphaTestOn || bForceAlphaProperty)
    {
        NiAlphaProperty *pAlpha;

        pAlpha = NiNew NiAlphaProperty;
        NIASSERT(pAlpha);

        pAlpha->SetAlphaBlending(bAlphaBlendOn);
        pAlpha->SetSrcBlendMode(eSrcBlend);
        pAlpha->SetDestBlendMode(eDestBlend);

        // these next four parameters are only relevant to alpha testing
        pAlpha->SetAlphaTesting(bAlphaTestOn);
        pAlpha->SetTestRef(ucTestRef);
        pAlpha->SetTestMode(eTestMode);
        pAlpha->SetNoSorter(bNoSorter);

        m_bMtlIsTrans = ms_bSceneIsTrans = true;

        pTarget->AttachProperty(pAlpha);
    }

    // NiShadeProperty
    if (iShading == SHADE_CONST)
    {
        NiShadeProperty *pShade;

        pShade = NiNew NiShadeProperty;
        NIASSERT(pShade);
        
        pShade->SetSmooth(false);

        pTarget->AttachProperty(pShade);
    }

    // NiWireframeProperty
    if (iWire)
    {
        NiWireframeProperty *pWire;

        pWire = NiNew NiWireframeProperty;
        NIASSERT(pWire);
        
        pWire->SetWireframe(true);

        pTarget->AttachProperty(pWire);
    }

    // Now handle all the texture maps
    NiMAXTextureConverter texture(m_animStart, m_animEnd);

    bool bAreAnyTextures = false;
    unsigned int uiFlags;
    Texmap* pTm;
    NiTexturingProperty::Map* pkMap;
    NiTexturingProperty* pkTex = NiNew NiTexturingProperty;
    pkTex->SetApplyMode(eApplyMode);

    NiSourceCubeMapPtr spCubeMap = NULL;

    // Set the default texture export settings
    texture.SetLocalPixelLayout(texture.GetPixelLayout());

    // First handle the "base" or diffuse texture map
    if (pSM->MapEnabled(S_BASE))
    {
        // Set the texture export settings
        texture.SetLocalPixelLayout(ComputePixelLayout(texture, 
            pShader->GetTextureExportSetting(S_BASE)));

        pTm = pSM->GetSubTexmap(S_BASE);

        if (bAlphaTestOn && !bAlphaBlendOn)
            kPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::BINARY;

        NiFlipController* pkFlipper;
        NiTObjectArray<NiTextureTransformControllerPtr> kTTCont;

        unsigned int uiWidth, uiHeight;
        texture.Convert(pTm, pkMap, NiTexturingProperty::MAP_CLASS_BASE, 
            pkFlipper, kPrefs, uiFlags, uiWidth, uiHeight);

        kPrefs.m_eAlphaFmt = NiTexture::FormatPrefs::ALPHA_DEFAULT;
        
        if (pkMap)
        {
            NiTexture* pkTexture = pkMap->GetTexture();
            if (NiIsKindOf(NiSourceCubeMap, pkTexture))
            {
                spCubeMap = (NiSourceCubeMap*) pkTexture;
                NiDelete pkMap;
                pkMap = NULL;
            }
            
            m_pkNiMAXUV->AssignMapChannel(pkMap, pkFlipper, kTTCont, pTm);
            
            if (pkMap) // may have been deleted by AssignMapChannel
            {
                pkTex->SetBaseMap(pkMap);
                AssignFlipController(pkFlipper, pkTex, pkMap);
                m_pkNiMAXUV->AssignTextureTransformControllers(kTTCont, 
                    pkTex, pkMap);
                bAreAnyTextures = true;
            }
        
            if ((uiFlags & NiMAXTextureConverter::HAS_ALPHA) &&
                bAlphaBlendOn)
            {
                CreateAlphaProp(pTarget, eSrcBlend, eDestBlend);
            }

        }
    }

    // The Dark Map
    if (pSM->MapEnabled(S_DARK))
    {
        // Set the texture export settings
        texture.SetLocalPixelLayout(ComputePixelLayout(texture, 
                                pShader->GetTextureExportSetting(S_DARK)));
        pTm = pSM->GetSubTexmap(S_DARK);

        NiFlipController* pkFlipper;
        NiTObjectArray<NiTextureTransformControllerPtr> kTTCont;
    
        unsigned int uiWidth, uiHeight;
        texture.Convert(pTm, pkMap, NiTexturingProperty::MAP_CLASS_BASE, 
            pkFlipper, kPrefs, uiFlags, uiWidth, uiHeight);

        if (pkMap)
        {
            NiTexture* pkTexture = pkMap->GetTexture();
            if (NiIsKindOf(NiSourceCubeMap, pkTexture))
            {
                spCubeMap = (NiSourceCubeMap*) pkTexture;
                NiDelete pkMap;
                pkMap = NULL;
            }

            m_pkNiMAXUV->AssignMapChannel(pkMap, pkFlipper, kTTCont, pTm);

            if (pkMap) // may have been deleted by AssignMapChannel
            {
                pkTex->SetDarkMap(pkMap);        
                AssignFlipController(pkFlipper, pkTex, pkMap);
                m_pkNiMAXUV->AssignTextureTransformControllers(kTTCont, 
                    pkTex, pkMap);
                bAreAnyTextures = true;
            }
        }
    }

    // The Detail Map
    if (pSM->MapEnabled(S_DETAIL))
    {
        // Set the texture export settings
        texture.SetLocalPixelLayout(ComputePixelLayout(texture, 
            pShader->GetTextureExportSetting(S_DETAIL)));
        pTm = pSM->GetSubTexmap(S_DETAIL);

        NiFlipController* pkFlipper;
        NiTObjectArray<NiTextureTransformControllerPtr> kTTCont;
    
        unsigned int uiWidth, uiHeight;
        texture.Convert(pTm, pkMap, NiTexturingProperty::MAP_CLASS_BASE, 
            pkFlipper, kPrefs, uiFlags, uiWidth, uiHeight);

        if (pkMap)
        {
            NiTexture* pkTexture = pkMap->GetTexture();
            if (NiIsKindOf(NiSourceCubeMap, pkTexture))
            {
                spCubeMap = (NiSourceCubeMap*) pkTexture;
                NiDelete pkMap;
                pkMap = NULL;
            }

            m_pkNiMAXUV->AssignMapChannel(pkMap, pkFlipper, kTTCont, pTm);

            if (pkMap) // may have been deleted by AssignMapChannel
            {
                pkTex->SetDetailMap(pkMap);        
                AssignFlipController(pkFlipper, pkTex, pkMap);
                m_pkNiMAXUV->AssignTextureTransformControllers(kTTCont,
                    pkTex, pkMap);
                bAreAnyTextures = true;
            }
        }
    }

    // The Decal Map
    if (pSM->MapEnabled(S_DECAL))
    {
        // Set the texture export settings
        texture.SetLocalPixelLayout(ComputePixelLayout(texture, 
            pShader->GetTextureExportSetting(S_DECAL)));
        pTm = pSM->GetSubTexmap(S_DECAL);

        if (pTm->ClassID() == Class_ID(COMPOSITE_CLASS_ID, 0))
        {
            ISubMap *pComp = (ISubMap *) pTm;

            for (int i = 0; i < pComp->NumSubTexmaps(); i++)
            {
                Texmap *pSubTm = pComp->GetSubTexmap(i);
                if (!pSubTm)
                    continue;

                // To support the texture export settings for Decal2
                if(i == 2)
                {
                    texture.SetLocalPixelLayout(ComputePixelLayout(texture, 
                        pShader->GetTextureExportSetting(S_DECAL2)));
                }
                else if(i > 2) //All additional Decal Maps are to use the 
                    // default
                {
                    texture.SetLocalPixelLayout(texture.GetPixelLayout());
                }

                ConvertDecal(pShader, pSubTm, i, pkTex, kPrefs,
                             bAreAnyTextures);
            }
        }
        else
        {
            ConvertDecal(pShader, pTm, 0, pkTex, kPrefs, bAreAnyTextures);
        }
    }

    // The Gloss Map
    if (pSM->MapEnabled(S_GLOSS))
    {
        texture.SetLocalPixelLayout(ComputePixelLayout(texture, 
            pShader->GetTextureExportSetting(S_GLOSS)));

        pTm = pSM->GetSubTexmap(S_GLOSS);

        NiFlipController* pkFlipper; 
        NiTObjectArray<NiTextureTransformControllerPtr> kTTCont;
    
        unsigned int uiWidth, uiHeight;
        texture.Convert(pTm, pkMap, NiTexturingProperty::MAP_CLASS_BASE, 
            pkFlipper, kPrefs, uiFlags, uiWidth, uiHeight);

        if (pkMap)
        {
            NiTexture* pkTexture = pkMap->GetTexture();
            if (NiIsKindOf(NiSourceCubeMap, pkTexture))
            {
                spCubeMap = (NiSourceCubeMap*) pkTexture;
                NiDelete pkMap;
                pkMap = NULL;
            }

            m_pkNiMAXUV->AssignMapChannel(pkMap, pkFlipper, kTTCont, pTm);

            if (pkMap) // may have been deleted by AssignMapChannel
            {
                pkTex->SetGlossMap(pkMap);            
                AssignFlipController(pkFlipper, pkTex, pkMap);
                m_pkNiMAXUV->AssignTextureTransformControllers(kTTCont,
                    pkTex, pkMap);
                bAreAnyTextures = true;
            }
        }
    }

    // The Bump Map
    if (pSM->MapEnabled(S_BUMP))
    {
        pTm = pSM->GetSubTexmap(S_BUMP);

        NiFlipController* pkFlipper;
        NiTObjectArray<NiTextureTransformControllerPtr> kTTCont;
    

        kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::BUMPMAP;
        unsigned int uiWidth, uiHeight;
        texture.Convert(pTm, pkMap, NiTexturingProperty::MAP_CLASS_BUMP, 
            pkFlipper, kPrefs, uiFlags, uiWidth, uiHeight);

        if (pkMap)
        {
            NiTexture* pkTexture = pkMap->GetTexture();
            if (NiIsKindOf(NiSourceCubeMap, pkTexture))
            {
                spCubeMap = (NiSourceCubeMap*) pkTexture;
                NiDelete pkMap;
                pkMap = NULL;
            }

            m_pkNiMAXUV->AssignMapChannel(pkMap, pkFlipper, kTTCont, pTm);

            if (pkMap) // may have been deleted by AssignMapChannel
            {
                NiTexturingProperty::BumpMap* pkBump = 
                    (NiTexturingProperty::BumpMap*)pkMap;

                float fBumpMagnitude = pShader->GetBumpMagnitude();
                //float fOldBumpMagnitude = pSM->GetTexmapAmt(S_BUMP,
                //m_animStart);
                pkBump->SetBumpMat00(fBumpMagnitude);
                pkBump->SetBumpMat01(0.0f);
                pkBump->SetBumpMat10(0.0f);
                pkBump->SetBumpMat11(fBumpMagnitude);

                pkBump->SetLumaOffset(pShader->GetLumaOffset());
                pkBump->SetLumaScale(pShader->GetLumaScale());


                pkTex->SetBumpMap(pkBump);            
                AssignFlipController(pkFlipper, pkTex, pkMap);
                m_pkNiMAXUV->AssignTextureTransformControllers(kTTCont,
                    pkTex, pkMap);
                bAreAnyTextures = true;
            }
        }

        
    }
    
    
    kPrefs.m_ePixelLayout = NiTexture::FormatPrefs::PIX_DEFAULT;
    // The Glow Map
    if (pSM->MapEnabled(S_GLOW))
    {
        texture.SetLocalPixelLayout(ComputePixelLayout(texture, 
                        pShader->GetTextureExportSetting(S_GLOW)));
        pTm = pSM->GetSubTexmap(S_GLOW);

        NiFlipController* pkFlipper;
        NiTObjectArray<NiTextureTransformControllerPtr> kTTCont;
    
        
        unsigned int uiWidth, uiHeight;
        texture.Convert(pTm, pkMap, NiTexturingProperty::MAP_CLASS_BASE, 
            pkFlipper, kPrefs, uiFlags, uiWidth, uiHeight);
        if (pkMap)
        {
            m_pkNiMAXUV->AssignMapChannel(pkMap, pkFlipper, kTTCont, pTm);

            if (pkMap) // may have been deleted by AssignMapChannel
            {
                pkTex->SetGlowMap(pkMap);
            
                AssignFlipController(pkFlipper, pkTex, pkMap);
                m_pkNiMAXUV->AssignTextureTransformControllers(kTTCont, 
                    pkTex, pkMap);
                bAreAnyTextures = true;
            }
        }
    }
    
    texture.SetLocalPixelLayout(texture.GetPixelLayout());

    if (pSM->MapEnabled(S_PARTICLE_OPACITY))
    {
        pTm = pSM->GetSubTexmap(S_PARTICLE_OPACITY);

        // If the user has specified alpha values for the particle system's
        // vertex colors we must provide an alphaproperty to get the
        // transparency to work correctly. This is a bit sloppy in that
        // all the alpha values could be 1.0f but that seems very unlikely.
        if ((pTm != NULL) && (pTm->ClassID() == PARTAGE_CLASSID))
            CreateAlphaProp(pTarget, eSrcBlend, eDestBlend);
    }

    if (pSM->MapEnabled(S_REFLECTION))
    {
        unsigned int uiEnvMapFlags;
        NiTexturingProperty::Map* pkMap;
        NiTexture::FormatPrefs kPrefs;

        pTm = (BitmapTex*) pSM->GetSubTexmap(S_REFLECTION);
        NiFlipController* pkFlipper;
        unsigned int uiWidth, uiHeight;
        texture.Convert(pTm, pkMap, NiTexturingProperty::MAP_CLASS_BASE, 
            pkFlipper, kPrefs, uiEnvMapFlags, uiWidth, uiHeight,false);
        // flip controller not supported in NiTextureEffects
        NiDelete pkFlipper;

        if (pkMap)
        {
            NiTexture* pkTexture = pkMap->GetTexture();
            NiNode* pkTrueParent = pParent;
            if ((!pkEffectNode) && bMultiMtl)
            {
                pkEffectNode = NiNew NiNode;
                pParent->AttachChild(pkEffectNode);
                pkTrueParent = pkEffectNode;
            }

            if (NiIsKindOf(NiSourceCubeMap, pkTexture))
            {
                NiMAXDynamicEffectConverter::CreateCubeMapTextureEffect(
                    (NiSourceCubeMap*) pkTexture, pkTrueParent);
            }
            else if (NiIsKindOf(NiSourceTexture, pkTexture))
            {
                NiMAXDynamicEffectConverter::CreateSphereMapTextureEffect(
                    (NiSourceTexture*) pkTexture, pkTrueParent);
            }
        
            // delete Map, it was only needed to hold the texture 
            // from the converter
            NiDelete pkMap;
        }
    }

    if (pSM->MapEnabled(S_NORMALMAP))
    {
        texture.SetLocalPixelLayout(ComputePixelLayout(texture, 
            pShader->GetTextureExportSetting(S_NORMALMAP)));
        pTm = pSM->GetSubTexmap(S_NORMALMAP);

        NiFlipController* pkFlipper;
        NiTObjectArray<NiTextureTransformControllerPtr> kTTCont;


        unsigned int uiWidth, uiHeight;
        texture.Convert(pTm, pkMap, NiTexturingProperty::MAP_CLASS_BASE, 
            pkFlipper, kPrefs, uiFlags, uiWidth, uiHeight);
        if (pkMap)
        {
            m_pkNiMAXUV->AssignMapChannel(pkMap, pkFlipper, kTTCont, pTm);

            if (pkMap) // may have been deleted by AssignMapChannel
            {
                pkTex->SetNormalMap(pkMap);

                AssignFlipController(pkFlipper, pkTex, pkMap);
                m_pkNiMAXUV->AssignTextureTransformControllers(kTTCont, 
                    pkTex, pkMap);
                bAreAnyTextures = true;
            }
        }
    }

    if (pSM->MapEnabled(S_PARALLAX))
    {
        texture.SetLocalPixelLayout(ComputePixelLayout(texture, 
            pShader->GetTextureExportSetting(S_PARALLAX)));
        pTm = pSM->GetSubTexmap(S_PARALLAX);

        NiFlipController* pkFlipper;
        NiTObjectArray<NiTextureTransformControllerPtr> kTTCont;


        unsigned int uiWidth, uiHeight;
        texture.Convert(pTm, pkMap, NiTexturingProperty::MAP_CLASS_PARALLAX, 
            pkFlipper, kPrefs, uiFlags, uiWidth, uiHeight);
        if (pkMap)
        {
            m_pkNiMAXUV->AssignMapChannel(pkMap, pkFlipper, kTTCont, pTm);

            if (pkMap) // may have been deleted by AssignMapChannel
            {
                NIASSERT(pkMap->GetClassID() ==
                    NiTexturingProperty::MAP_CLASS_PARALLAX);
                pkTex->SetParallaxMap((NiTexturingProperty::ParallaxMap*)
                    pkMap);
                float fParallaxOffset = pShader->GetParallaxOffset();
                ((NiTexturingProperty::ParallaxMap*)
                    pkMap)->SetOffset(fParallaxOffset);
                AssignFlipController(pkFlipper, pkTex, pkMap);
                m_pkNiMAXUV->AssignTextureTransformControllers(kTTCont, 
                    pkTex, pkMap);
                
                bAreAnyTextures = true;
            }
        }
    }

    if (bAreAnyTextures)
    {
        pTarget->AttachProperty(pkTex);
    }
    else
    {
        // no slots set
        // delete the NiTexturingProperty
        NiDelete pkTex;
        pkTex = NULL;
    }

    // Somewhere along the line, we found a cube map, let's create
    // the dynamic texture effect
    if (spCubeMap != NULL)
    {
        NiNode* pkTrueParent = pParent;
        if ((!pkEffectNode) && bMultiMtl)
        {
            pkEffectNode = NiNew NiNode;
            pParent->AttachChild(pkEffectNode);
            pkTrueParent = pkEffectNode;
        }

        NiMAXDynamicEffectConverter::CreateCubeMapTextureEffect(spCubeMap, 
            pkTrueParent);
    }

    // Now convert any animation applied to the material
    Control *pCont;
    NiParamBlock *pBlock = pShader->GetParamBlockByID(shader_params);
            
    pCont = pBlock->GetController(sk_amb);
    if (pCont)
        CreateAnimator(pCont, pMat, NiMaterialColorController::AMB);
            
    pCont = pBlock->GetController(sk_diff);
    if (pCont)
        CreateAnimator(pCont, pMat, NiMaterialColorController::DIFF);

    pCont = pBlock->GetController(sk_spec);
    if (pCont)
        CreateAnimator(pCont, pMat, NiMaterialColorController::SPEC);

    pCont = pBlock->GetController(sk_emit);
    if (pCont)
        CreateAnimator(pCont, pMat, NiMaterialColorController::SELF_ILLUM);

    pCont = pBlock->GetController(sk_alpha);
    if (pCont)
        ConvertAlphaAnim(pCont, pMat, pTarget, eSrcBlend, eDestBlend);

    AttachShaders(pShader, NiDynamicCast(NiMesh, pTarget), pMaxNode);

    CHECK_MEMORY();
    return(W3D_STAT_OK);
}


//---------------------------------------------------------------------------
// Convert accepts a MAX material to convert ("pkMaterial") and a
// NI NiAVObect the resulting NI material needs to be attached to 
// ("pAVObject"). When "pkMaterial" is a multimaterial "iAttrId" must be
// specified to determine which sub material to convert and assign.
//
// psUVRemapping - specifies where a given MAX UV channel has wound up in NI
int NiMAXMaterialConverter::Convert(
    Mtl *pkMaterial, 
    NiNode *pParent, 
    NiAVObject *pAVObject,
    INode *pMaxNode,
    int iAttrId,
    NiMAXUV* pkNiMAXUV,
    NiNode*& pkEffectNode,
    bool bMultiMtl)
{
    CHECK_MEMORY();
    int iStatus = 0;
    
    m_bMtlIsTrans = false;
    bool bUsesNiShader = false;
    NIASSERT(pkNiMAXUV);
    m_pkNiMAXUV = pkNiMAXUV;

    
    if (pkMaterial == NULL)
    {
        iStatus = CreateSimple(pAVObject, pMaxNode);
    }
    else if (pkMaterial->ClassID() == Class_ID(CMTL_CLASS_ID, 0) || 
        pkMaterial->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
    {
        Shader *pShader = ((NiStdMat *) pkMaterial)->GetShader();

        Class_ID cID = pShader->ClassID();

        if (cID == NISHADER_CLASSID)
        {
            bUsesNiShader = true;
            // convert custom Gamebryo material
            iStatus = ConvertCustom(pkMaterial, pParent, pAVObject, 
                pMaxNode, pkEffectNode, bMultiMtl);
        }
        else
        {
            iStatus = ConvertSingle(pkMaterial, pParent, pAVObject, 
                pMaxNode);
        }
    }
    else if (pkMaterial->ClassID() == Class_ID(MULTI_CLASS_ID, 0))
    {
        if (m_pkNiMAXUV)
            m_pkNiMAXUV->SetAttrId(iAttrId);
        MultiMtl *pMM;
        Mtl *pSub;

        pMM = (MultiMtl *) pkMaterial;
        pSub = pMM->GetSubMtl(iAttrId);
        pkMaterial = pSub;

        if (pSub && (pSub->ClassID() == Class_ID(CMTL_CLASS_ID, 0) || 
            pSub->ClassID() == Class_ID(DMTL_CLASS_ID, 0)))
        {
            Shader *pShader = ((NiStdMat *) pSub)->GetShader();

            Class_ID cID = pShader->ClassID();

            if (cID == NISHADER_CLASSID)
            {
                bUsesNiShader = true;
                // convert custom Gamebryo material
                iStatus = ConvertCustom(pSub, pParent, pAVObject, pMaxNode,
                    pkEffectNode, true);
            }
            else
                iStatus = ConvertSingle(pSub, pParent, pAVObject, pMaxNode);
        }
        else
        {
            return Convert(pkMaterial, pParent, pAVObject, pMaxNode,
                iAttrId, pkNiMAXUV, pkEffectNode, bMultiMtl);
        }
    }
    else if (pkMaterial->ClassID() == Class_ID(SHELLMTL_CLASS_ID, 0))
    {
        Mtl* pSub = pkMaterial->GetSubMtl(1);
        pkMaterial = pSub;

        if (!pSub)
        {
            return 0;
        }
        else
        {
            return Convert(pSub, pParent, pAVObject, pMaxNode,
                iAttrId, pkNiMAXUV, pkEffectNode, bMultiMtl);
        }    
    }
    else if (pkMaterial->ClassID() == PLATFORM_MATERIAL_CLASS_ID)
    {
        Mtl* pSub = NULL;
        NiMAXPlatformMaterial* pkPlatformMtl = (NiMAXPlatformMaterial*) 
            pkMaterial;

        pSub = 
            pkPlatformMtl->GetDevImagePlatformMtl(
            NiMAXTextureConverter::GetPlatform());
            
        if (!pSub)
        {
            return 0;
        }
        else
        {
            return Convert(pSub, pParent, pAVObject, pMaxNode, iAttrId, 
               pkNiMAXUV, pkEffectNode, bMultiMtl);
        }
    }
    else if (pkMaterial->ClassID() == Class_ID(XREFMATERIAL_CLASS_ID))
    {
        Mtl* pkRefSource = ((IXRefMaterial*)pkMaterial)->GetSourceMaterial();
        
        if (pkRefSource)
        {
            Shader *pShader = ((NiStdMat *) pkRefSource)->GetShader();

            Class_ID cID = pShader->ClassID();

            if (cID == NISHADER_CLASSID)
            {
                bUsesNiShader = true;
                // convert custom Gamebryo material
                iStatus = ConvertCustom(pkRefSource, pParent, pAVObject, 
                    pMaxNode, pkEffectNode, bMultiMtl);
            }
            else
            {
                iStatus = ConvertSingle(pkRefSource, pParent, pAVObject, 
                    pMaxNode);
            }
        }
    }

    NiTexturingProperty* pkTex = (NiTexturingProperty*)
        pAVObject->GetProperty(NiProperty::TEXTURING);
    NiMAXCustAttribConverter kAttribConverter(m_animStart, m_animEnd);
    kAttribConverter.Convert(pkMaterial, pAVObject, true, pkTex, pkNiMAXUV);
    CHECK_MEMORY();
    return iStatus;
}

//---------------------------------------------------------------------------
// Returns the number of NI materials needed to mimic this MAX material
int NiMAXMaterialConverter::GetNumMaterials(
    Mtl* pkMaterial,
    bool& bForceMultiSub)
{
    bForceMultiSub = false;
    if (pkMaterial == NULL)
        return(1);

    if (pkMaterial->ClassID() == Class_ID(MULTI_CLASS_ID, 0))
    {
        MultiMtl *pMM = (MultiMtl *) pkMaterial;
        bForceMultiSub = true;
        return(pMM->NumSubMtls());
    }
    else if (pkMaterial->ClassID() == PLATFORM_MATERIAL_CLASS_ID)
    {
        Mtl* pSub = NULL;
        NiMAXPlatformMaterial* pkPlatformMtl = (NiMAXPlatformMaterial*) 
            pkMaterial;

        pSub = 
            pkPlatformMtl->GetDevImagePlatformMtl(
            NiMAXTextureConverter::GetPlatform());
            
        if (!pSub)
            return 1;
        else
            return GetNumMaterials(pSub, bForceMultiSub);
    }

    
    return(1);
}

//---------------------------------------------------------------------------
void NiMAXMaterialConverter::GetMultiUV(
    NiStdMat *pSM,
    Texmap *pTm, 
    NiMAXUV* pkNiMAXUV)
{
    CHECK_MEMORY();
    Texmap *pSubTm;
    int i;

    NIASSERT(pTm);

    Texmap *pSITm;

    pSITm = NULL;
    if (pSM->MapEnabled(ID_SI))
    {
        pSITm = (BitmapTex*) pSM->GetSubTexmap(ID_SI);
        if (pSITm && pSITm->ClassID() != Class_ID(BMTEX_CLASS_ID, 0))
            pSITm = NULL;
    }

    if (pSITm && pTm && pTm->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
    {
        pkNiMAXUV->Convert(pTm);
        pkNiMAXUV->Convert(pSITm);
    }
    else if (pTm->ClassID() == Class_ID(MIX_CLASS_ID, 0))
    {
        ISubMap* pMix = (ISubMap *) pTm;

        NIASSERT(pMix->NumSubTexmaps() == 2 || pMix->NumSubTexmaps() == 3);

        for (i = 0; i < pMix->NumSubTexmaps(); i++)
        {
            pSubTm = pMix->GetSubTexmap(i);
            NIASSERT(pSubTm);
            NIASSERT(pSubTm->ClassID() == Class_ID(BMTEX_CLASS_ID, 0));

            if (i == 2)
                continue;

            pkNiMAXUV->Convert(pSubTm);
        }
    }
    else if (pTm->ClassID() == Class_ID(COMPOSITE_CLASS_ID, 0))
    {
        ISubMap *pComp = (ISubMap *) pTm;

        NIASSERT(pComp->NumSubTexmaps() == 2);

        for (i = 0; i < pComp->NumSubTexmaps(); i++)
        {
            pSubTm = pComp->GetSubTexmap(i);
            NIASSERT(pSubTm);
            NIASSERT(pSubTm->ClassID() == Class_ID(BMTEX_CLASS_ID, 0));

            pkNiMAXUV->Convert(pSubTm);
        }
    }
    else if (pTm->ClassID() == Class_ID(RGBMULT_CLASS_ID, 0))
    {
        RGBMult *pMult;

        pMult = (RGBMult *) pTm;

        NIASSERT(pMult->NumSubTexmaps() == 2);

        for (i = 0; i < pMult->NumSubTexmaps(); i++)
        {
            pSubTm = pMult->GetSubTexmap(i);
            NIASSERT(pSubTm);
            NIASSERT(pSubTm->ClassID() == Class_ID(BMTEX_CLASS_ID, 0));

            pkNiMAXUV->Convert(pSubTm);
        }
    }
    CHECK_MEMORY();
}

//---------------------------------------------------------------------------
void NiMAXMaterialConverter::GetCustomUV(NiStdMat *pSM, NiMAXUV* pkNiMAXUV)
{
    CHECK_MEMORY();
    int i;

    for (i = 0; i < SHADER_NTEXMAPS; i++)
    {
        if (!pSM->MapEnabled(i))
            continue;

        Texmap *pTm = pSM->GetSubTexmap(i);

        if (i == S_DECAL && 
            pTm->ClassID() == Class_ID(COMPOSITE_CLASS_ID, 0))
        {
            ISubMap *pComp = (ISubMap *) pTm;

            for (int j = 0; j < pComp->NumSubTexmaps(); j++)
            {
                Texmap *pSubTm = pComp->GetSubTexmap(j);
                if (!pSubTm)
                    continue;

                pkNiMAXUV->Convert(pSubTm);
            }
        }
        else
            pkNiMAXUV->Convert(pTm);
    }
    CHECK_MEMORY();
}

//---------------------------------------------------------------------------
void NiMAXMaterialConverter::GetUVInfo(
    Mtl* pkMaterial,
    TimeValue kAnimStart,
    NiMAXUV*& pkNiMAXUV,
    int iAttrId,
    bool bFirstMulti)
{
    CHECK_MEMORY();
    if (!pkNiMAXUV)
        pkNiMAXUV = NiNew NiMAXUV(kAnimStart);
    
    if (pkMaterial == NULL)
        return;

    pkNiMAXUV->SetAttrId(iAttrId);

    if (pkMaterial->ClassID() == Class_ID(CMTL_CLASS_ID, 0) || 
        pkMaterial->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
    {
        NiStdMat *pSM;
        Texmap* pTm;

        pSM = (NiStdMat *) pkMaterial;

        Interval intForever = FOREVER;
        pSM->Update(0, intForever);
        
        Shader *pShader = pSM->GetShader();

        Class_ID cID = pShader->ClassID();

        if (cID == NISHADER_CLASSID)
        {
            GetCustomUV(pSM, pkNiMAXUV);
        }
        else
        {
            pTm = (BitmapTex*) pSM->GetSubTexmap(ID_DI);
            if (!pTm)
                return;

            if (IsMultiTextured(pSM))
            {
                GetMultiUV(pSM, pTm, pkNiMAXUV);
            }
            else
                pkNiMAXUV->Convert(pTm);
        }
    }
    else if (pkMaterial->ClassID() == Class_ID(MULTI_CLASS_ID, 0))
    {
        MultiMtl *pMM;
        Mtl *pSub;

        if (!bFirstMulti || !pkNiMAXUV)
            return;
        
        pMM = (MultiMtl *) pkMaterial;

        //In Max3.1 GetSubMtl(-1) would return the number of submats.  In
        //4.0 and up, it returns 0.  This conditional makes sure that 
        //pSub is 0 when needed.
        pSub = pMM->GetSubMtl(iAttrId < 0 ? 0 : iAttrId);

        GetUVInfo(pSub, kAnimStart, pkNiMAXUV, iAttrId, false);
    }
    else if (pkMaterial->ClassID() == Class_ID(SHELLMTL_CLASS_ID, 0))
    {
        Mtl *pSub;

        if (!pkNiMAXUV)
            return;
        
        pSub = pkMaterial->GetSubMtl(1);

        GetUVInfo(pSub, kAnimStart, pkNiMAXUV, iAttrId, false);
    }
    else if (pkMaterial->ClassID() == PLATFORM_MATERIAL_CLASS_ID)
    {
        Mtl* pSub = NULL;
        NiMAXPlatformMaterial* pkPlatformMtl = (NiMAXPlatformMaterial*) 
            pkMaterial;

        
        if (!pkNiMAXUV)
            return;
        
        pSub = 
            pkPlatformMtl->GetDevImagePlatformMtl(
            NiMAXTextureConverter::GetPlatform());      
        
        GetUVInfo(pSub, kAnimStart, pkNiMAXUV, iAttrId, bFirstMulti);
    }
    else if (pkMaterial->ClassID() == Class_ID(XREFMATERIAL_CLASS_ID))
    {
        Mtl* pSub;
        pSub = ((IXRefMaterial*)pkMaterial)->GetSourceMaterial();

        if (pSub)
        {
            GetUVInfo(pSub, kAnimStart, pkNiMAXUV, iAttrId, bFirstMulti);
        }
    }

    NiMAXCustAttribConverter::GetUVInfo(pkMaterial, kAnimStart, pkNiMAXUV,
        iAttrId, bFirstMulti);
    CHECK_MEMORY();
}

//---------------------------------------------------------------------------
// Is a given non-multi/sub-object material 2sided?
bool NiMAXMaterialConverter::IsTwoSidedSingle(Mtl *pkMaterial)
{

    if (pkMaterial == NULL)
        return(false);

    if (pkMaterial->ClassID() == Class_ID(CMTL_CLASS_ID, 0) || 
        pkMaterial->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
    {
        NiStdMat *pSM;

        pSM = (NiStdMat *) pkMaterial;
        Interval intForever = FOREVER;
        pSM->Update(0, intForever);

        if (pSM->GetTwoSided())
            return(true);
        return(false);
    }
    
    if (pkMaterial->ClassID() == Class_ID(DOUBLESIDED_CLASS_ID, 0))
        return(true);

    return(false);
}

//---------------------------------------------------------------------------
// Is an arbitrary material 2sided?
bool NiMAXMaterialConverter::IsTwoSided(Mtl *pkMaterial, int iAttrId)
{

    if (pkMaterial == NULL)
        return(false);

    if (pkMaterial->ClassID() == Class_ID(CMTL_CLASS_ID, 0) || 
        pkMaterial->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
        return IsTwoSidedSingle(pkMaterial);

    if (pkMaterial->ClassID() == Class_ID(MULTI_CLASS_ID, 0))
    {
        MultiMtl *pMM;
        Mtl *pSub;

        pMM = (MultiMtl *) pkMaterial;
        pSub = pMM->GetSubMtl(iAttrId);
        return IsTwoSidedSingle(pSub);
    }
    else if (pkMaterial->ClassID() == PLATFORM_MATERIAL_CLASS_ID)
    {
        Mtl* pSub = NULL;
        NiMAXPlatformMaterial* pkPlatformMtl = 
            (NiMAXPlatformMaterial*) pkMaterial;

        pSub = 
            pkPlatformMtl->GetDevImagePlatformMtl(
            NiMAXTextureConverter::GetPlatform());

        return IsTwoSided(pSub, iAttrId);
    }

    if (pkMaterial->ClassID() == Class_ID(DOUBLESIDED_CLASS_ID, 0))
        return(true);

    return(false);
}

//---------------------------------------------------------------------------
void ColorAlphaInsertionSort(
    NiColorA* pColors,
    float* pPercents, 
    unsigned int& uiNumKeys,
    float fNewAlpha,
    float fNewPercent, 
    unsigned int& uiStartIndex,
    float fNextAlpha)
{
    float fPrevPercentage = 0.0f;
    NiColorA kPrevColor = NiColorA::BLACK;
    for (unsigned int ui = uiStartIndex; ui < uiNumKeys; ui++)
    {
        if (fNewPercent == pPercents[ui])
        {
            pColors[ui].a = fNewAlpha;
            uiStartIndex = ui;
            return;
        }
        else if(fNewPercent < pPercents[ui])
        {
            uiNumKeys++;
            float fLastPercentage = fNewPercent;
            NiColorA kLastColor;
            NiColorA kOrigColor = pColors[ui];

            float fT = fNewPercent - fPrevPercentage;
            kLastColor.r = (1.0f - fT)*kPrevColor.r + (fT)*kOrigColor.r;
            kLastColor.g = (1.0f - fT)*kPrevColor.g + (fT)*kOrigColor.g;
            kLastColor.b = (1.0f - fT)*kPrevColor.b + (fT)*kOrigColor.b;
            kLastColor.a = fNewAlpha;

            pColors[ui].a = (1.0f - fT)*fNewAlpha + (fT)*fNextAlpha;
            
            for (unsigned int uj = ui; uj < uiNumKeys; uj++)
            {
                // Store the current color and position
                float fTempPercentage = pPercents[uj];
                NiColorA kTempColor = pColors[uj];

                // 
                pPercents[uj] = fLastPercentage;
                pColors[uj] = kLastColor;

                kLastColor = kTempColor;
                fLastPercentage = fTempPercentage;
            }
        }

        fPrevPercentage = pPercents[ui];
        kPrevColor = pColors[ui];
    }

    pColors[uiNumKeys].r = kPrevColor.r;
    pColors[uiNumKeys].g = kPrevColor.g;
    pColors[uiNumKeys].b = kPrevColor.b;
    pColors[uiNumKeys].a = fNewAlpha;
    pPercents[uiNumKeys] = fNewPercent;
    uiNumKeys++;
    uiStartIndex = uiNumKeys;

}

//---------------------------------------------------------------------------
#pragma warning(push)
#pragma warning(disable: 4706)
void NiMAXMaterialConverter::GetPSysColorInfo(
    Mtl *pkMaterial, 
    int iAttrId,
    NiPSKernelColorKey*& pColors,
    bool& bFound,
    unsigned int& uiNumKeys)
{
    bFound = false;

    if (pkMaterial == NULL)
        return;

    if (pkMaterial->ClassID() == Class_ID(CMTL_CLASS_ID, 0) || 
        pkMaterial->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
    {
        PartAgeTex *pPartAge, *pPartOpac;
        NiStdMat *pSM;
        Texmap* pTm;

        pSM = (NiStdMat *) pkMaterial;

        Interval intForever = FOREVER;
        pSM->Update(0, intForever);

        pPartAge = NULL;
        pPartOpac = NULL;

        NiMAXShader *pShader;
        
        pShader = (NiMAXShader *) pSM->GetShader();

        if (pShader->ClassID() == NISHADER_CLASSID)
        {
            if (pSM->MapEnabled(S_PARTICLE_COLOR) &&
                (pTm = (BitmapTex*) pSM->GetSubTexmap(S_PARTICLE_COLOR)) &&
                pTm->ClassID() == PARTAGE_CLASSID)
            {
                pPartAge = (PartAgeTex *) pTm;
            }
            else if (pSM->MapEnabled(S_BASE) &&
                (pTm = (BitmapTex*) pSM->GetSubTexmap(S_BASE)) &&
                pTm->ClassID() == PARTAGE_CLASSID)
            {
                pPartAge = (PartAgeTex *) pTm;
            }

            if (pSM->MapEnabled(S_PARTICLE_OPACITY) &&
                (pTm = (BitmapTex*) pSM->GetSubTexmap(S_PARTICLE_OPACITY)) &&
                pTm->ClassID() == PARTAGE_CLASSID)
            {
                pPartOpac = (PartAgeTex *) pTm;
            }

        }
        else
        {
            if (pSM->MapEnabled(ID_DI) &&
                (pTm = (BitmapTex*) pSM->GetSubTexmap(ID_DI)) &&
                pTm->ClassID() == PARTAGE_CLASSID)
            {
                pPartAge = (PartAgeTex *) pTm;
            }
            else if (pSM->MapEnabled(ID_SI) &&
                (pTm = (BitmapTex*) pSM->GetSubTexmap(ID_SI)) &&
                pTm->ClassID() == PARTAGE_CLASSID)
            {
                pPartAge = (PartAgeTex *) pTm;
            }

            if (pSM->MapEnabled(ID_OP) &&
                (pTm = (BitmapTex*) pSM->GetSubTexmap(ID_OP)) &&
                pTm->ClassID() == PARTAGE_CLASSID)
            {
                pPartOpac = (PartAgeTex *) pTm;
            }
        }


        if (pPartAge)
        {
            NiColorA kColor1(pPartAge->col1.r, pPartAge->col1.g,
                pPartAge->col1.b, 1.0f);
            NiColorA kColor2(pPartAge->col2.r, pPartAge->col2.g,
                pPartAge->col2.b, 1.0f);
            NiColorA kColor3(pPartAge->col3.r, pPartAge->col3.g, 
                pPartAge->col3.b, 1.0f);
            
            NiAnimationKey* pkKeys = NiNew NiLinColorKey[1];
            NiLinColorKey* pkColorKeys = (NiLinColorKey*) pkKeys;
            pkColorKeys[0].SetTime(pPartAge->p1);
            pkColorKeys[0].SetColor(kColor1);
            uiNumKeys = 1;

            NiAnimationKey::InsertFunction pfnLinColorInsert = 
                NiColorKey::GetInsertFunction(NiAnimationKey::LINKEY);
            NIASSERT(pfnLinColorInsert);
#ifdef NIDEBUG
            NiAnimationKey::DeleteFunction pfnLinColorDelete = 
#endif
                NiColorKey::GetDeleteFunction(NiAnimationKey::LINKEY);
            NIASSERT(pfnLinColorDelete);
            pfnLinColorInsert(pPartAge->p2, pkKeys, uiNumKeys);
            pfnLinColorInsert(pPartAge->p3, pkKeys, uiNumKeys);

            // Since the insertion functions will likely reassign the 
            // pointer to the array, we have to grab it again
            pkColorKeys = (NiLinColorKey*) pkKeys;
            
            for (unsigned int ui = 0; ui < uiNumKeys; ui++)
            {
                if (pkColorKeys[ui].GetTime() == pPartAge->p2)
                    pkColorKeys[ui].SetColor(kColor2);
                if (pkColorKeys[ui].GetTime() == pPartAge->p3)
                    pkColorKeys[ui].SetColor(kColor3);
            }

            bFound = true;

            if (pPartOpac)
            {
                float fAlpha1 = (pPartOpac->col1.r + pPartOpac->col1.g + 
                                                pPartOpac->col1.b) / 3.0f;
                float fAlpha2 = (pPartOpac->col2.r + pPartOpac->col2.g + 
                                                pPartOpac->col2.b) / 3.0f;
                float fAlpha3 = (pPartOpac->col3.r + pPartOpac->col3.g + 
                                                pPartOpac->col3.b) / 3.0f;

                NiAnimationKey* pkTempKeys = NiNew NiLinFloatKey[1];
                NiLinFloatKey* pkAlphaKeys = (NiLinFloatKey*) pkTempKeys;
                pkAlphaKeys[0].SetTime(pPartOpac->p1);
                pkAlphaKeys[0].SetValue(fAlpha1);
                unsigned int uiNumAlphaKeys = 1;

                NiAnimationKey::InsertFunction pfnLinFloatInsert = 
                    NiFloatKey::GetInsertFunction(NiAnimationKey::LINKEY);
                NIASSERT(pfnLinFloatInsert);
                NiAnimationKey::DeleteFunction pfnLinFloatDelete = 
                    NiFloatKey::GetDeleteFunction(
                    NiAnimationKey::LINKEY);
                NIASSERT(pfnLinFloatDelete);

                pfnLinFloatInsert(pPartOpac->p2, pkTempKeys, uiNumAlphaKeys);
                pfnLinFloatInsert(pPartOpac->p3, pkTempKeys, uiNumAlphaKeys);
                pkAlphaKeys = (NiLinFloatKey*) pkTempKeys;

                for (unsigned int ui = 0; ui < uiNumAlphaKeys; ui++)
                {
                    if (pkAlphaKeys[ui].GetTime() == pPartOpac->p2)
                        pkAlphaKeys[ui].SetValue(fAlpha2);
                    if (pkAlphaKeys[ui].GetTime() == pPartOpac->p3)
                        pkAlphaKeys[ui].SetValue(fAlpha3);
                }

                pfnLinColorInsert(pPartOpac->p1, pkKeys, uiNumKeys);
                pfnLinColorInsert(pPartOpac->p2, pkKeys, uiNumKeys);
                pfnLinColorInsert(pPartOpac->p3, pkKeys, uiNumKeys);

                //Since the insertion functions will likely reassign 
                //the pointer to the array, we have to grab it again
                pkColorKeys = (NiLinColorKey*) pkKeys;
                unsigned int uiLastIdx = 0;
                    
                for (unsigned int uj = 0; uj < uiNumKeys; uj++)
                {
                    float fTime = pkColorKeys[uj].GetTime();
                    float fAlpha = pkAlphaKeys[0].GetValue();
                    if ((fTime >= pkAlphaKeys[0].GetTime()) && 
                        (fTime <= pkAlphaKeys[uiNumAlphaKeys-1].GetTime()))
                    {
                        fAlpha = pkAlphaKeys->GenInterp(fTime, pkAlphaKeys, 
                            NiAnimationKey::LINKEY, uiNumAlphaKeys, uiLastIdx,
                            sizeof(NiLinFloatKey));
                    }
                    else if(fTime > pkAlphaKeys[uiNumAlphaKeys-1].GetTime())
                    {
                        fAlpha = pkAlphaKeys[uiNumAlphaKeys-1].GetValue();
                    }
                    NiColorA kColorA = pkColorKeys[uj].GetColor();
                    kColorA.a = fAlpha;
                    pkColorKeys[uj].SetColor(kColorA);
                }

                pfnLinFloatDelete(pkTempKeys);
            }
            
            // Ensure that we have keys at 0.0 and 1.0 times, which is a
            // requirement of particle systems.
            if (pkColorKeys[0].GetTime() > 0.0f)
            {
                NiColorA kColorA = pkColorKeys[0].GetColor();
                pfnLinColorInsert(0.0f, pkKeys, uiNumKeys);
                pkColorKeys = (NiLinColorKey*) pkKeys;
                pkColorKeys[0].SetColor(kColorA);
            }
            if (pkColorKeys[uiNumKeys - 1].GetTime() < 1.0f)
            {
                NiColorA kColorA = pkColorKeys[uiNumKeys - 1].GetColor();
                pfnLinColorInsert(1.0f, pkKeys, uiNumKeys);
                pkColorKeys = (NiLinColorKey*) pkKeys;
                pkColorKeys[uiNumKeys - 1].SetColor(kColorA);
            }

            pColors = new NiPSKernelColorKey[uiNumKeys];

            for(unsigned int i=0;i< uiNumKeys; i++)
            {
                pColors[i].m_fTime = pkColorKeys[i].GetTime();
                pColors[i].m_kColor.r() = (NiUInt8)(pkColorKeys[i].GetColor().r*255.0f);
                pColors[i].m_kColor.g() = (NiUInt8)(pkColorKeys[i].GetColor().g*255.0f);
                pColors[i].m_kColor.b() = (NiUInt8)(pkColorKeys[i].GetColor().b*255.0f);
                pColors[i].m_kColor.a() = (NiUInt8)(pkColorKeys[i].GetColor().a*255.0f);
            }
            
            NiDelete[] pkKeys;
        }
        else if (pPartOpac)
        {
            char acString[512];
            NiSprintf(acString, 512, "Mtl \"%s\" has particle opacity but no "
                "particle color.\nThis will not be exported without a "
                "particle color map.", pkMaterial->GetName());
            NILOGWARNING(acString);
        }
    }
    else if (pkMaterial->ClassID() == Class_ID(MULTI_CLASS_ID, 0))
    {
        MultiMtl *pMM;
        Mtl *pSub;

        if (iAttrId == -1)
            return;
        
        pMM = (MultiMtl *) pkMaterial;
        pSub = pMM->GetSubMtl(iAttrId);
        GetPSysColorInfo(pSub, -1, pColors, bFound, uiNumKeys);
    }
    else if (pkMaterial->ClassID() == PLATFORM_MATERIAL_CLASS_ID)
    {
        Mtl* pSub = NULL;
        NiMAXPlatformMaterial* pkPlatformMtl = (
            NiMAXPlatformMaterial*) pkMaterial;

        pSub = 
            pkPlatformMtl->GetDevImagePlatformMtl(
            NiMAXTextureConverter::GetPlatform());

        GetPSysColorInfo(pSub, iAttrId, pColors, bFound, uiNumKeys);

    }
}
#pragma warning(pop)
//---------------------------------------------------------------------------

NiTexture::FormatPrefs::PixelLayout 
NiMAXMaterialConverter::ComputePixelLayout(
    NiMAXTextureConverter converter, 
    NiMAXShader::TextureExportSetting setting)
{
    CHECK_MEMORY();

    // This is the global texture export setting
    NiTexture::FormatPrefs::PixelLayout kDefault = 
        converter.GetPixelLayout();
    // We will use this in all cases if the override checkbox has been 
    // checked
    if(converter.GetPerTextureOverride())
        return kDefault;

    // Otherwise, we use the assigned setting
    switch(setting)
    {
        case NiMAXShader::NI_IMAGE_DEFAULTS:
            return NiTexture::FormatPrefs::PIX_DEFAULT;
            break;
        case NiMAXShader::NI_HIGH_COLOR: 
            return NiTexture::FormatPrefs::HIGH_COLOR_16;
            break;
        case NiMAXShader::NI_COMPRESSED:
            return NiTexture::FormatPrefs::COMPRESSED;
            break;
        case NiMAXShader::NI_PALETTIZED:
            return NiTexture::FormatPrefs::PALETTIZED_8;
            break;
        default:
            return kDefault;
            break;
    }
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------
