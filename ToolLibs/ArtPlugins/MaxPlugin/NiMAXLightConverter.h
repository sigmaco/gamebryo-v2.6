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

#ifndef NIMAXLIGHTCONVERTER_H
#define NIMAXLIGHTCONVERTER_H

#include "NiMAX.h"
#include "NiTHash.h"
#include "NiMAXConverter.h"

#define NON_SHADOW_CASTER_KEY "NonShadowCaster"
#define NON_SHADOW_RECEIVER_KEY "NonShadowReceiver"
#define SHADOW_TECHNIQUE_NAME_KEY "NiShadowTechnique"
#define SHADOW_STATIC_KEY "NiShadowStatic"
#define SHADOW_BACKFACING_KEY "NiShadowBackfacing"
#define SHADOW_FORCE_SIZE_KEY "NiShadowForceSize"
#define SHADOW_AUTOBIAS_KEY "NiShadowAutoBias"

//---------------------------------------------------------------------------
class NiMAXLightConverter
{
public:
    NiMAXLightConverter(TimeValue kAnimStart, TimeValue kAnimEnd);
    ~NiMAXLightConverter() {};

    int Convert(Object* pkObj, 
        NiNode* pkCurNode, 
        INode* pkMaxNode,
        ScaleInfo* pkParentScale);
    int ConvertAmbientLight(Interface* pkIntf, NiNode* pkRoot);

    inline static void SetConvertLights(bool bConvertLights)
                    { ms_bConvertLights = bConvertLights; }
    inline static bool GetConvertLights() { return ms_bConvertLights; }

    int Finish(Interface *pIntf, NiNode *pRoot);

    static void Init();
    static void Shutdown();

private:
    static int ConvertDefaultLights(NiNode* pkRoot);
    NiTextureEffect* CreateNiTextureEffect(
        TimeValue kAnimStart,
        TimeValue kAnimEnd,
        GenLight* pkGL,
        LightState& kLState,
        Texmap* pkTexMap,
        NiTextureEffect::TextureType eType,
        BOOL bClip = FALSE);

    int ReallyConvert(Object* pkObj, 
        NiNode* pkCurNode, 
        INode* pkMaxNode,
        Interface* pkIntf,
        TimeValue kAnimStart,
        TimeValue kAnimEnd,
        NiNode* pkRoot,
        ScaleInfo* pkParentScale);

    int ConvertShadows(NiNode* pkCurNode, NiLight* piLight, GenLight* pkGL);
    void RecursiveFindShadowFlags(NiNode* pkNode, 
        NiShadowGenerator* pkShadowGenerator);
    int CleanExtraData(NiObjectNET* pkObject);

    class LightInfo : public NiMemObject
    {
    public:
        Object* pkObj;
        NiNode* pkCurNode;
        INode* pkMaxNode;
        ScaleInfo kScale;
    };

    TimeValue m_kAnimStart, m_kAnimEnd;

    enum         { NUM_DEF_LIGHTS = 2 };

    static const NiPoint3 ms_akDefaultLightDirs[NUM_DEF_LIGHTS];
    static bool ms_bConvertLights;
    static NiTPointerList<LightInfo *> ms_kLightList;

    static void *ms_pvUserData;
};
//---------------------------------------------------------------------------

#endif
