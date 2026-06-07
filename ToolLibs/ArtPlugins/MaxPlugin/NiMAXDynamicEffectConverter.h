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

#ifndef NIDYNAMICEFFECTCONVERTER_H
#define NIDYNAMICEFFECTCONVERTER_H

#include <NiFlipController.h>
#include "NiMAX.h"
#include "NiMAXConverter.h"
#include "NiTHash.h"

//---------------------------------------------------------------------------
class NiMAXDynamicEffectConverter
{
public:
    class NiDynamicEffectInfo : public NiMemObject
    {
        public:
            NiNodePtr m_spParent;
            NiDynamicEffectPtr m_spEffect;
    };

    NiMAXDynamicEffectConverter(TimeValue kAnimStart, TimeValue kAnimEnd);
    ~NiMAXDynamicEffectConverter() {};
    
    static void Init();
    static void Shutdown();
    static void Finish(NiNode* pkNode);

   
    static NiTextureEffect* CreateEnvironmentMapTextureEffect(
        NiSourceTexture* pkMap, 
        NiNode* pkParent,
        bool bAttachToSceneRoot = true);
    static NiTextureEffect* CreateCubeMapTextureEffect(
        NiSourceCubeMap* pkCubeMap, 
        NiNode* pkParent,
        bool bAttachToSceneRoot = true);
    static NiTextureEffect* CreateSphereMapTextureEffect(
        NiSourceTexture* pkSphereMap, 
        NiNode* pkParent,
        bool bAttachToSceneRoot = true);

protected:
    static NiTPointerList<NiDynamicEffectInfo*> ms_kDynEffectList;
};

//---------------------------------------------------------------------------
//  Inline include
//#include "NiDynamicEffectConverter.inl"

//---------------------------------------------------------------------------
#endif

