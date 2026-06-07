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
#include "NiMAXDynamicEffectConverter.h"
#include "NiMAXTextureConverter.h"

NiTPointerList<NiMAXDynamicEffectConverter::NiDynamicEffectInfo *>
    NiMAXDynamicEffectConverter::ms_kDynEffectList;


//---------------------------------------------------------------------------

NiMAXDynamicEffectConverter::NiMAXDynamicEffectConverter(
    TimeValue, TimeValue)
{
}

//---------------------------------------------------------------------------
void NiMAXDynamicEffectConverter::Init()
{
    NIASSERT(ms_kDynEffectList.IsEmpty());
}
//---------------------------------------------------------------------------
void NiMAXDynamicEffectConverter::Shutdown()
{
    // Delete the list
    NiTListIterator kIter = ms_kDynEffectList.GetHeadPos();
    
    while (kIter)
    {
        NiDelete ms_kDynEffectList.RemovePos(kIter);
    }
    ms_kDynEffectList.RemoveAll();
}

//---------------------------------------------------------------------------
void NiMAXDynamicEffectConverter::Finish(NiNode* pkRoot)
{
    // Assign DynEffects
    if (!ms_kDynEffectList.IsEmpty())
    {
        NiTListIterator kIter = ms_kDynEffectList.GetHeadPos();
        
        while (kIter)
        {
            NiDynamicEffectInfo* pkDynEffectInfo = 
                ms_kDynEffectList.GetNext(kIter);
            pkRoot->AttachChild(pkDynEffectInfo->m_spEffect);
        }
    }
}

//---------------------------------------------------------------------------
NiTextureEffect* NiMAXDynamicEffectConverter::CreateSphereMapTextureEffect(
    NiSourceTexture* pkSphereMap, NiNode* pkParent, bool bAttachToSceneRoot)
{
    // Somewhere along the line, we found a spherical env map, let's create
    // the dynamic texture effect
    if (pkSphereMap != NULL)
    {
        NiTextureEffect *pTextEffect;
        pTextEffect = NiNew NiTextureEffect;
        NIASSERT(pTextEffect);

        pTextEffect->SetEffectTexture(pkSphereMap);
        pTextEffect->SetTextureFilter(NiTexturingProperty::FILTER_TRILERP);
        pTextEffect->SetTextureClamp(NiTexturingProperty::CLAMP_S_CLAMP_T);
        pTextEffect->SetTextureType(
            NiTextureEffect::ENVIRONMENT_MAP);
        pTextEffect->SetTextureCoordGen(
            NiTextureEffect::SPHERE_MAP);
        if (pkParent)
            pTextEffect->AttachAffectedNode(pkParent);
#if !defined(USE_OLD_SPHERICAL_ENVIRONMENT_MAP)
        NiMatrix3 kMatrix;
        kMatrix.SetRow(0, NiPoint3(0.0f, 0.0f, 1.0f));
        kMatrix.SetRow(1, NiPoint3(1.0f, 0.0f, 0.0f));
        kMatrix.SetRow(2, NiPoint3(0.0f, 1.0f, 0.0f));

        pTextEffect->SetRotate(kMatrix);
#endif
        if (bAttachToSceneRoot)
        {
            NiDynamicEffectInfo* pkNiDynamicEffectInfo = 
                NiNew NiDynamicEffectInfo;
            if (pkNiDynamicEffectInfo)
            {
                pkNiDynamicEffectInfo->m_spParent = pkParent;
                pkNiDynamicEffectInfo->m_spEffect = pTextEffect;
        
                ms_kDynEffectList.AddHead(pkNiDynamicEffectInfo);
            }
        }
        else if (pkParent)
        {
            pkParent->AttachChild(pTextEffect);
        }
        return (pTextEffect);
    }
    else
    {
        return (NULL);
    }
}
//---------------------------------------------------------------------------
NiTextureEffect* NiMAXDynamicEffectConverter::CreateCubeMapTextureEffect(
    NiSourceCubeMap* pkCubeMap, NiNode* pkParent, bool bAttachToSceneRoot)
{
    // Somewhere along the line, we found a cube map, let's create
    // the dynamic texture effect
    if (pkCubeMap != NULL)
    {
        NiTextureEffect *pTextEffect;
        pTextEffect = NiNew NiTextureEffect;
        NIASSERT(pTextEffect);

        pTextEffect->SetEffectTexture(pkCubeMap);
        pTextEffect->SetTextureFilter(NiTexturingProperty::FILTER_TRILERP);
        pTextEffect->SetTextureClamp(NiTexturingProperty::CLAMP_S_CLAMP_T);
        pTextEffect->SetTextureType(
            NiTextureEffect::ENVIRONMENT_MAP);

        NiIntegerExtraData* pkTextureCoordGenED = (NiIntegerExtraData*)
            pkCubeMap->GetExtraData(NI_TEXTURE_COORD_GEN_KEY);

        NiTextureEffect::CoordGenType eCoordType = 
            NiTextureEffect::SPECULAR_CUBE_MAP;
        if (pkTextureCoordGenED)
        {
            eCoordType = (NiTextureEffect::CoordGenType)
                pkTextureCoordGenED->GetValue();
            pkCubeMap->RemoveExtraData(NI_TEXTURE_COORD_GEN_KEY);
        }

        pTextEffect->SetTextureCoordGen(eCoordType);
        if (pkParent)
            pTextEffect->AttachAffectedNode(pkParent);
        pTextEffect->SetSwitch(true);

        NiMatrix3 kMatrix;
        kMatrix.SetRow(0, NiPoint3(1.0f, 0.0f, 0.0f));
        kMatrix.SetRow(1, NiPoint3(0.0f, 0.0f, 1.0f));
        kMatrix.SetRow(2, NiPoint3(0.0f, 1.0f, 0.0f));

        pTextEffect->SetModelProjectionMatrix(kMatrix);

        if (bAttachToSceneRoot)
        {
            NiDynamicEffectInfo* pkNiDynamicEffectInfo = 
                NiNew NiDynamicEffectInfo;
            if (pkNiDynamicEffectInfo)
            {
                pkNiDynamicEffectInfo->m_spParent = pkParent;
                pkNiDynamicEffectInfo->m_spEffect = pTextEffect;
        
                ms_kDynEffectList.AddHead(pkNiDynamicEffectInfo);
            }
        }
        else if (pkParent)
        {
            pkParent->AttachChild(pTextEffect);
        }
        return (pTextEffect);
    }
    else
    {
        return (NULL);
    }
}

//---------------------------------------------------------------------------
NiTextureEffect* 
NiMAXDynamicEffectConverter::CreateEnvironmentMapTextureEffect(
    NiSourceTexture* pkMap, NiNode* pkParent, bool bAttachToSceneRoot)
{
    if (NiIsExactKindOf(NiSourceCubeMap, pkMap))
    {
        NiSourceCubeMap* pkCubeMap = (NiSourceCubeMap*) pkMap;
        return CreateCubeMapTextureEffect(pkCubeMap, pkParent,
            bAttachToSceneRoot);
    }
    else if (NiIsKindOf(NiSourceTexture, pkMap))
    {
        return CreateSphereMapTextureEffect(pkMap, pkParent,
            bAttachToSceneRoot);
    }
    return NULL;
}
//---------------------------------------------------------------------------
