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

#ifndef NIMAXMATERIALCONVERTER_H
#define NIMAXMATERIALCONVERTER_H

#include "NiMaterialColorController.h"
#include "NiMAXUV.h"
#include "NiMAX.h"
#include "NiTHash.h"
#include "NiMAXConverter.h"
#include "NiMAXShader.h"
#include "NiFlipController.h"
#include "NiMAXTextureConverter.h"
#include <NiLinColorKey.h>

#define SHELLMTL_CLASS_ID 597
//---------------------------------------------------------------------------
class NiMAXMaterialConverter
{
public:
    

    NiMAXMaterialConverter(TimeValue animStart, TimeValue animEnd);

    static bool IsSceneTrans();

    int Convert(
        Mtl* pkMaterial,
        NiNode* pParent,
        NiAVObject* pAVObject, 
        INode* pMaxNode,
        int iAttrId,
        NiMAXUV* pkNiMAXUV,
        NiNode*& pkEffectNode,
        bool bMultiMtl = false);

    static void Init();
    static void Shutdown();
    static void Finish(NiNode* pkNode);
    
    // *** begin MAXImmerse internal use only ***
    static bool IsTwoSided(Mtl *pkMaterial, int iAttrId);
    static int GetNumMaterials(Mtl* pkMaterial, bool& bForceMultiSub);
    static void GetUVInfo(
        Mtl* pkMaterial,
        TimeValue kAnimStart,
        NiMAXUV*& pkNiMAXUV,
        int iAttrId,
        bool bFirstMulti = true);

    static void GetPSysColorInfo(
        Mtl *pkMaterial, 
        int iAttrId,
        NiPSKernelColorKey*& pkColors,
        bool& bFound,
        unsigned int& uiNumKeys);

    // *** end MAXImmerse internal use only ***

private:

    static void GetCustomUV(NiStdMat *pSM, NiMAXUV* pkNiMAXUV);

    static void GetMultiUV(
        NiStdMat *pSM,
        Texmap *pTm, 
        NiMAXUV* pkNiMAXUV);

    void ConvertDecal(
        NiMAXShader *pShader,
        Texmap *pTm,
        unsigned int uiIndex,
        NiTexturingProperty* pkTex,
        NiTexture::FormatPrefs& kPrefs,
        bool& bAreAnyTextures);

    bool HasAlpha(Texmap *pDiffTm);
    static bool UseForOpacity(
        NiStdMat *pSm,
        TimeValue animStart, 
        Texmap *pDiffTm);
    int CreateSimple(NiAVObject *pTarget, INode *pMaxNode);
    int ConvertSingle(
        Mtl *pkMaterial, 
        NiNode *pParent,
        NiAVObject *pTarget, 
        INode *pMaxNode);

    int ConvertCustom(
        Mtl *pkMaterial,
        NiNode *pParent,
        NiAVObject *pTarget, 
        INode *pMaxNode,
        NiNode*& pkEffectNode, 
        bool bMultiMtl);

    void AttachShaders(
        NiMAXShader* pkShader,
        NiMesh* pkMesh,
        INode *pMaxNode);

    void ConvertAlphaAnim(
        Control *pCont, 
        NiMaterialProperty *pMat,
        NiAVObject *pTarget, 
        NiAlphaProperty::AlphaFunction srcBlend,
        NiAlphaProperty::AlphaFunction destBlend);
    void CreateAnimator(
        Control *pCont, 
        NiMaterialProperty *pMat,
        NiMaterialColorController::Field eType);
    void CreateSelfIllumAnimator(
        Control *pCont, 
        NiMaterialProperty *pMat,
        NiStdMat *pSM);
    static bool IsMultiTextured(NiStdMat *pSM);
    static bool IsTwoSidedSingle(Mtl *pkMaterial);

    void CreateAlphaProp(
        NiAVObject *pTarget,
        NiAlphaProperty::AlphaFunction srcBlend,
        NiAlphaProperty::AlphaFunction destBlend);

    int ConvertMultiTexture(
        NiStdMat *pSM, 
        NiAVObject *pTarget,
        NiAlphaProperty::AlphaFunction srcBlend,
        NiAlphaProperty::AlphaFunction destBlend);

    inline void AssignFlipController(
        NiFlipController* pkFlipper,
        NiTexturingProperty* pkTex, 
        NiTexturingProperty::Map* pkMap);

    NiTexture::FormatPrefs::PixelLayout ComputePixelLayout(
        NiMAXTextureConverter converter,
        NiMAXShader::TextureExportSetting setting);
    bool m_bMtlIsTrans;
    NiMAXUV* m_pkNiMAXUV;

    TimeValue m_animStart, m_animEnd;

    static bool ms_bSceneIsTrans;

    static void *ms_pUserData;
    
};
//---------------------------------------------------------------------------
#include "NiMAXMaterialConverter.inl"

//---------------------------------------------------------------------------

#endif
