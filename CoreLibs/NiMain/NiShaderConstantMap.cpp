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
#include "NiShaderConstantMap.h"
#include "NiAmbientLight.h"
#include "NiDirectionalLight.h"
#include "NiPointLight.h"
#include "NiSpotLight.h"
#include <ctype.h>
#include "NiDynamicEffectState.h"
#include "NiTextureEffect.h"

NiShaderConstantMap::PredefinedMapping* 
    NiShaderConstantMap::ms_pkPredefinedMappings = NULL;
unsigned int NiShaderConstantMap::ms_uiPredefinedMappingCount = 0;
NiShaderConstantMap::PredefinedMapping* 
    NiShaderConstantMap::ms_pkObjectMappings = NULL;
unsigned int NiShaderConstantMap::ms_uiObjectMappingCount = 0;
NiShaderConstantMap::MappingTypeInfo 
    NiShaderConstantMap::ms_akObjectMappingTypeInfo[
    NiShaderConstantMap::SCM_OBJ_COUNT];

const char* NiShaderConstantMap::ms_pcTimeExtraDataName = "NDL_UpdateTime";

//---------------------------------------------------------------------------
void NiShaderConstantMap::_SDMInit()
{
    // Handle Predefined Mapping
    ms_uiPredefinedMappingCount = SCM_DEF_COUNT + 20; // 20 Duplicates
    ms_pkPredefinedMappings = NiNew 
        PredefinedMapping[ms_uiPredefinedMappingCount];

    unsigned int ui = 0;
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_INVALID,
        "Invalid");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_PROJ,
        "Proj");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_PROJ,
        "Projection");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_INVPROJ,
        "InvProj");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_INVPROJ,
        "InvProjection");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_VIEW, 
        "View");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_INVVIEW,
        "InvView");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_WORLD,
        "World");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_INVWORLD,
        "InvWorld");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_WORLDVIEW,
        "WorldView");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_INVWORLDVIEW,
        "InvWorldView");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_VIEWPROJ,
        "ViewProj");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_VIEWPROJ,
        "ViewProjection");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_INVVIEWPROJ,
        "InvViewProj");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_INVVIEWPROJ,
        "InvViewProjection");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_WORLDVIEWPROJ,
        "WorldViewProj");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_WORLDVIEWPROJ, 
        "WorldViewProjection");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_INVWORLDVIEWPROJ,
        "InvWorldViewProj");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_INVWORLDVIEWPROJ,
        "InvWorldViewProjection");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_PROJ_T,
        "ProjTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_PROJ_T,
        "ProjectionTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_INVPROJ_T,
        "InvProjTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_INVPROJ_T,
        "InvProjectionTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_VIEW_T, 
        "ViewTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_INVVIEW_T,
        "InvViewTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_WORLD_T,
        "WorldTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_INVWORLD_T,
        "InvWorldTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_WORLDVIEW_T,
        "WorldViewTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_INVWORLDVIEW_T,
        "InvWorldViewTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_VIEWPROJ_T,
        "ViewProjTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_VIEWPROJ_T,
        "ViewProjectionTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_INVVIEWPROJ_T,
        "InvViewProjTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_INVVIEWPROJ_T,
        "InvViewProjectionTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_WORLDVIEWPROJ_T, 
        "WorldViewProjTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_WORLDVIEWPROJ_T, 
        "WorldViewProjectionTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVWORLDVIEWPROJ_T, "InvWorldViewProjTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVWORLDVIEWPROJ_T, "InvWorldViewProjectionTranspose");
    // Matrix palette skinning values
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_SKINWORLDVIEW, 
        "SkinWorldView"); 
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_INVSKINWORLDVIEW,
        "InvSkinWorldView"); 
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_SKINWORLDVIEW_T, 
        "SkinWorldViewTranspose"); 
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVSKINWORLDVIEW_T, "InvSkinWorldViewTranspose"); 
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_SKINWORLDVIEWPROJ, "SkinWorldViewProj"); 
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_SKINWORLDVIEWPROJ, "SkinWorldViewProjection"); 
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVSKINWORLDVIEWPROJ, "InvSkinWorldViewProj"); 
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVSKINWORLDVIEWPROJ, "InvSkinWorldViewProjection"); 
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_SKINWORLDVIEWPROJ_T, "SkinWorldViewProjTranspose"); 
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_SKINWORLDVIEWPROJ_T, "SkinWorldViewProjectionTranspose"); 
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVSKINWORLDVIEWPROJ_T, "InvSkinWorldViewProjTranspose"); 
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVSKINWORLDVIEWPROJ_T, "InvSkinWorldViewProjectionTranspose");
    
    // Bone matrices
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_BONE_MATRIX_3, "BoneMatrix3");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_BONE_MATRIX_4, "BoneMatrix4");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_SKINBONE_MATRIX_3, "SkinBoneMatrix3");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_SKINBONE_MATRIX_4, "SkinBoneMatrix4");
    
    // Texture transform matrices
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_TEXTRANSFORMBASE,
        "TexTransformBase");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVTEXTRANSFORMBASE, "InvTexTransformBase");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_TEXTRANSFORMBASE_T, "TexTransformBaseTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVTEXTRANSFORMBASE_T, "InvTexTransformBaseTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_TEXTRANSFORMDARK,
        "TexTransformDark");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVTEXTRANSFORMDARK, "InvTexTransformDark");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_TEXTRANSFORMDARK_T, "TexTransformDarkTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVTEXTRANSFORMDARK_T, "InvTexTransformDarkTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_TEXTRANSFORMDETAIL, "TexTransformDetail");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVTEXTRANSFORMDETAIL, "InvTexTransformDetail");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_TEXTRANSFORMDETAIL_T, "TexTransformDetailTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVTEXTRANSFORMDETAIL_T, "InvTexTransformDetailTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_TEXTRANSFORMGLOSS,"TexTransformGloss");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVTEXTRANSFORMGLOSS, "InvTexTransformGloss");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_TEXTRANSFORMGLOSS_T, "TexTransformGlossTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVTEXTRANSFORMGLOSS_T, "InvTexTransformGlossTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_TEXTRANSFORMGLOW,
        "TexTransformGlow");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVTEXTRANSFORMGLOW, "InvTexTransformGlow");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_TEXTRANSFORMGLOW_T, "TexTransformGlowTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVTEXTRANSFORMGLOW_T, "InvTexTransformGlowTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_TEXTRANSFORMBUMP,
        "TexTransformBump");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVTEXTRANSFORMBUMP, "InvTexTransformBump");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_TEXTRANSFORMBUMP_T, "TexTransformBumpTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVTEXTRANSFORMBUMP_T, "InvTexTransformBumpTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_TEXTRANSFORMDECAL, "TexTransformDecal");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVTEXTRANSFORMDECAL, "InvTexTransformDecal");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_TEXTRANSFORMDECAL_T, "TexTransformDecalTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVTEXTRANSFORMDECAL_T, "InvTexTransformDecalTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_TEXTRANSFORMSHADER, "TexTransformShader");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVTEXTRANSFORMSHADER, "InvTexTransformShader");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_TEXTRANSFORMSHADER_T, "TexTransformShaderTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVTEXTRANSFORMSHADER_T, "InvTexTransformShaderTranspose");

    // Lights
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_LIGHT_POS_WS, 
        "LightPosWS");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_LIGHT_DIR_WS,
        "LightDirWS");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_LIGHT_POS_OS, 
        "LightPosOS");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_LIGHT_DIR_OS, 
        "LightDirOS");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_MATERIAL_DIFFUSE,
        "MaterialDiffuse");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_MATERIAL_DIFFUSE,
        "Diffuse");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_MATERIAL_AMBIENT,
        "MaterialAmbient");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_MATERIAL_AMBIENT,
        "Ambient");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_MATERIAL_SPECULAR, "MaterialSpecular");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_MATERIAL_SPECULAR, "Specular");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_MATERIAL_EMISSIVE, "MaterialEmissive");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_MATERIAL_POWER,
        "MaterialPower");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_MATERIAL_POWER,
        "SpecularPower");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_EYE_POS, 
        "EyePos");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_EYE_DIR, 
        "EyeDir");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_CONSTS_TAYLOR_SIN, "TaylorSin");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_CONSTS_TAYLOR_COS, "TaylorCos");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_CONSTS_TIME,
        "time");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_CONSTS_SINTIME, 
        "sin_time");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_CONSTS_COSTIME, 
        "cos_time");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_CONSTS_TANTIME, 
        "tan_time");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_CONSTS_TIME_SINTIME_COSTIME_TANTIME, "time_sin_cos_tan");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_AMBIENTLIGHT, 
        "AmbientLight");
    // Additional NiTexturingProperty::Maps Texture Transforms
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_TEXTRANSFORMPARALLAX, "TexTransformParallax");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVTEXTRANSFORMPARALLAX, "InvTexTransformParallax");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_TEXTRANSFORMPARALLAX_T, "TexTransformParallaxTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVTEXTRANSFORMPARALLAX_T, "InvTexTransformParallaxTranspose");

    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_TEXTRANSFORMNORMAL, "TexTransformNormal");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVTEXTRANSFORMNORMAL, "InvTexTransformNormal");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_TEXTRANSFORMNORMAL_T, "TexTransformNormalTranspose");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_INVTEXTRANSFORMNORMAL_T, "InvTexTransformNormalTranspose");

    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_SKINWORLD, 
        "SkinWorld"); 
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_INVSKINWORLD,
        "InvSkinWorld"); 
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_SKINWORLD_T, 
        "SkinWorldTranspose"); 
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_INVSKINWORLD_T, 
        "InvSkinWorldTranspose"); 

    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_FOG_DENSITY, 
        "FogDensity"); 
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_FOG_NEARFAR,
        "FogNearFar"); 
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_FOG_COLOR, 
        "FogColor"); 

    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_BUMP_MATRIX,
        "BumpMatrix");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_BUMP_LUMA_OFFSET_AND_SCALE,
        "BumpLumaAndScale");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_PARALLAX_OFFSET,
        "ParallaxOffset");

    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_TEXSIZEBASE,
        "TexSizeBase");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_TEXSIZEDARK,
        "TexSizeDark");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_TEXSIZEDETAIL,
        "TexSizeDetail");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_TEXSIZEGLOSS,
        "TexSizeGloss");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_TEXSIZEGLOW,
        "TexSizeGlow");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_TEXSIZEBUMP,
        "TexSizeBump");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_TEXSIZENORMAL,
        "TexSizeNormal");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_TEXSIZEPARALLAX,
        "TexSizeParallax");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_TEXSIZEDECAL,
        "TexSizeDecal");
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_TEXSIZESHADER,
        "TexSizeShader");

    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_ALPHA_TEST_FUNC, 
        "AlphaTestFunction"); 
    ms_pkPredefinedMappings[ui++] = PredefinedMapping(SCM_DEF_ALPHA_TEST_REF, 
        "AlphaTestRef"); 

    ms_pkPredefinedMappings[ui++] = PredefinedMapping(
        SCM_DEF_XENON_INST_FREQUENCY, "XenonInstanceFrequency"); 

    NIASSERT(ui == ms_uiPredefinedMappingCount);

    // Handle Object Mappings
    // Because there are some cases of multiple strings mapping to a single
    // value, the total mapping count will exceed the object type count.
    ms_uiObjectMappingCount = SCM_OBJ_COUNT + 6;
    ms_pkObjectMappings = NiNew PredefinedMapping[ms_uiObjectMappingCount];

    ui = 0;
    ms_pkObjectMappings[ui++] = PredefinedMapping(SCM_OBJ_INVALID, "Invalid");
    ms_pkObjectMappings[ui++] = PredefinedMapping(SCM_OBJ_DIMMER, "Dimmer");
    ms_pkObjectMappings[ui++] = PredefinedMapping(SCM_OBJ_UNDIMMEDAMBIENT, 
        "UndimmedAmbient");
    ms_pkObjectMappings[ui++] = PredefinedMapping(SCM_OBJ_UNDIMMEDDIFFUSE, 
        "UndimmedDiffuse");
    ms_pkObjectMappings[ui++] = PredefinedMapping(SCM_OBJ_UNDIMMEDSPECULAR, 
        "UndimmedSpecular");
    ms_pkObjectMappings[ui++] = PredefinedMapping(SCM_OBJ_AMBIENT, 
        "Ambient");
    ms_pkObjectMappings[ui++] = PredefinedMapping(SCM_OBJ_DIFFUSE, 
        "Diffuse");
    ms_pkObjectMappings[ui++] = PredefinedMapping(SCM_OBJ_SPECULAR, 
        "Specular");
    ms_pkObjectMappings[ui++] = PredefinedMapping(SCM_OBJ_WORLDPOSITION, 
        "WorldPosition");
    ms_pkObjectMappings[ui++] = PredefinedMapping(SCM_OBJ_WORLDPOSITION, 
        "Position");
    ms_pkObjectMappings[ui++] = PredefinedMapping(SCM_OBJ_MODELPOSITION, 
        "ModelPosition");
    ms_pkObjectMappings[ui++] = PredefinedMapping(SCM_OBJ_WORLDDIRECTION, 
        "WorldDirection");
    ms_pkObjectMappings[ui++] = PredefinedMapping(SCM_OBJ_WORLDDIRECTION, 
        "Direction");
    ms_pkObjectMappings[ui++] = PredefinedMapping(SCM_OBJ_MODELDIRECTION, 
        "ModelDirection");
    ms_pkObjectMappings[ui++] = PredefinedMapping(SCM_OBJ_WORLDTRANSFORM, 
        "WorldTransform");
    ms_pkObjectMappings[ui++] = PredefinedMapping(SCM_OBJ_WORLDTRANSFORM, 
        "World");
    ms_pkObjectMappings[ui++] = PredefinedMapping(SCM_OBJ_MODELTRANSFORM, 
        "ModelTransform");
    ms_pkObjectMappings[ui++] = PredefinedMapping(SCM_OBJ_SPOTATTENUATION, 
        "SpotAttenuation");
    ms_pkObjectMappings[ui++] = PredefinedMapping(SCM_OBJ_ATTENUATION, 
        "Attenuation");
    ms_pkObjectMappings[ui++] = PredefinedMapping(
        SCM_OBJ_WORLDPROJECTIONMATRIX, "WorldProjectionMatrix");
    ms_pkObjectMappings[ui++] = PredefinedMapping(
        SCM_OBJ_WORLDPROJECTIONMATRIX, "Projection");
   ms_pkObjectMappings[ui++] = PredefinedMapping(
        SCM_OBJ_MODELPROJECTIONMATRIX, "ModelProjectionMatrix");
    ms_pkObjectMappings[ui++] = PredefinedMapping(
        SCM_OBJ_WORLDPROJECTIONTRANSLATION, "WorldProjectionTranslation");
    ms_pkObjectMappings[ui++] = PredefinedMapping(
        SCM_OBJ_WORLDPROJECTIONTRANSLATION, "ProjectionTranslation");
    ms_pkObjectMappings[ui++] = PredefinedMapping(
        SCM_OBJ_MODELPROJECTIONTRANSLATION, "ModelProjectionTranslation");
    ms_pkObjectMappings[ui++] = PredefinedMapping(SCM_OBJ_WORLDCLIPPINGPLANE, 
        "WorldClippingPlane");
    ms_pkObjectMappings[ui++] = PredefinedMapping(SCM_OBJ_WORLDCLIPPINGPLANE, 
        "ClippingPlane");
    ms_pkObjectMappings[ui++] = PredefinedMapping(SCM_OBJ_MODELCLIPPINGPLANE, 
        "ModelClippingPlane");
    ms_pkObjectMappings[ui++] = PredefinedMapping(SCM_OBJ_TEXCOORDGEN, 
        "TexCoordGen");
    ms_pkObjectMappings[ui++] = PredefinedMapping(
        SCM_OBJ_WORLDPROJECTIONTRANSFORM, "WorldProjectionTransform");
    ms_pkObjectMappings[ui++] = PredefinedMapping(
        SCM_OBJ_MODELPROJECTIONTRANSFORM, "ModelProjectionTransform");
    ms_pkObjectMappings[ui++] = PredefinedMapping(
        SCM_OBJ_WORLDTOSHADOWMAPMATRIX, "WorldToShadowMapMatrix");
    ms_pkObjectMappings[ui++] = PredefinedMapping(
        SCM_OBJ_SHADOWMAP, "ShadowMap");
    ms_pkObjectMappings[ui++] = PredefinedMapping(
        SCM_OBJ_SHADOWMAPTEXSIZE, "TexSizeShadow");
    ms_pkObjectMappings[ui++] = PredefinedMapping(
        SCM_OBJ_SHADOWBIAS, "ShadowBias");
    ms_pkObjectMappings[ui++] = PredefinedMapping(
        SCM_OBJ_SHADOW_VSM_POWER_EPSILON, "ShadowVSMPowerEpsilon");

    NIASSERT(ui == ms_uiObjectMappingCount);

    InitializeObjectMappingTypeInfo();
}
//---------------------------------------------------------------------------
void NiShaderConstantMap::_SDMShutdown()
{
    NiDelete [] ms_pkObjectMappings;
    ms_pkObjectMappings = NULL;

    NiDelete [] ms_pkPredefinedMappings;
    ms_pkPredefinedMappings = NULL;
}
//---------------------------------------------------------------------------
void NiShaderConstantMap::InitializeObjectMappingTypeInfo()
{
    for (unsigned int ui = 0; ui < SCM_OBJ_COUNT; ++ui)
    {
        unsigned int uiRegCount = 1;
        unsigned int uiFloatCount = 4;
        NiShaderAttributeDesc::AttributeType eAttribType = 
            NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;

        switch (ui)
        {
        case SCM_OBJ_DIMMER:
            uiFloatCount = 1;
            eAttribType = NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            break;
        case SCM_OBJ_UNDIMMEDAMBIENT:
        case SCM_OBJ_UNDIMMEDDIFFUSE:
        case SCM_OBJ_UNDIMMEDSPECULAR:
        case SCM_OBJ_AMBIENT:
        case SCM_OBJ_DIFFUSE:
        case SCM_OBJ_SPECULAR:
        case SCM_OBJ_WORLDPOSITION:
        case SCM_OBJ_MODELPOSITION:
        case SCM_OBJ_WORLDDIRECTION:
        case SCM_OBJ_MODELDIRECTION:
            eAttribType = NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            break;
        case SCM_OBJ_WORLDTRANSFORM:
        case SCM_OBJ_MODELTRANSFORM:
        case SCM_OBJ_WORLDPROJECTIONTRANSFORM:
        case SCM_OBJ_MODELPROJECTIONTRANSFORM:
            uiRegCount = 4;
            uiFloatCount = 16;
            eAttribType = NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
            break;
        case SCM_OBJ_SPOTATTENUATION:
        case SCM_OBJ_ATTENUATION:
            eAttribType = NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            break;
        case SCM_OBJ_WORLDPROJECTIONMATRIX:
        case SCM_OBJ_MODELPROJECTIONMATRIX:
            uiRegCount = 3;
            uiFloatCount = 12;
            eAttribType = NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT12;
            break;
        case SCM_OBJ_WORLDPROJECTIONTRANSLATION:
        case SCM_OBJ_MODELPROJECTIONTRANSLATION:
        case SCM_OBJ_WORLDCLIPPINGPLANE:
        case SCM_OBJ_MODELCLIPPINGPLANE:
            eAttribType = NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
            break;
        case SCM_OBJ_TEXCOORDGEN:
            uiFloatCount = 1;
            eAttribType = NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            break;
        case SCM_OBJ_WORLDTOSHADOWMAPMATRIX:
            uiRegCount = 4;
            uiFloatCount = 16;
            eAttribType = NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;
            break;
        case SCM_OBJ_SHADOWMAPTEXSIZE:
            eAttribType = NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
            break;
        case SCM_OBJ_SHADOWBIAS:
            eAttribType = NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
            break;
        case SCM_OBJ_SHADOW_VSM_POWER_EPSILON:
            eAttribType = NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
            break;
        default:
            break;
        };
        ms_akObjectMappingTypeInfo[ui].m_uiFloatCount = uiFloatCount;
        ms_akObjectMappingTypeInfo[ui].m_uiRegCount = uiRegCount;
        ms_akObjectMappingTypeInfo[ui].m_eAttribType = eAttribType;
    }
}
//---------------------------------------------------------------------------
NiShaderConstantMap::PredefinedMapping::PredefinedMapping(
    unsigned int uiMappingID, const NiFixedString& kMappingName)
{
    m_uiMappingID = uiMappingID;
    m_kMappingName = kMappingName;
}
//---------------------------------------------------------------------------
NiShaderConstantMap::PredefinedMapping::PredefinedMapping()
{
    m_uiMappingID = SCM_DEF_INVALID;
    m_kMappingName = NULL;
}
//---------------------------------------------------------------------------
NiShaderConstantMap::PredefinedMapping& 
NiShaderConstantMap::PredefinedMapping::operator=(
    const NiShaderConstantMap::PredefinedMapping& kOther)
{
    m_uiMappingID = kOther.m_uiMappingID;
    m_kMappingName = kOther.m_kMappingName;
    return *this;
}
//---------------------------------------------------------------------------
NiShaderConstantMap::NiShaderConstantMap(NiGPUProgram::ProgramType eType) :
    m_eProgramType(eType)
{
}
//---------------------------------------------------------------------------
bool NiShaderConstantMap::ParseKeyName(const char* pszOrigName, 
    char* pszTrueName, unsigned int uiMaxLen, unsigned int& uiRegisterCount)
{
    if (uiMaxLen + 1 < NI_MAX_PATH)
        return false;

    char acFullKey[NI_MAX_PATH];
    NiStrcpy(acFullKey, NI_MAX_PATH, pszOrigName);
    NiStrcpy(pszTrueName, NI_MAX_PATH, pszOrigName);

    char* pcKey = strchr(acFullKey, '_');
    if (pcKey)
    {
        // Continue only if the key is of the form "XXXX_#"
        // This allows the existing defines that contain '_' to be used
        if (!isdigit(pcKey[1]) || pcKey[2] != '\0')
            return true;

        char* pcContext;
        pcKey = NiStrtok(acFullKey, "_", &pcContext);

        char* pcToken = NiStrtok(NULL, "_", &pcContext);
        if (pcToken)
        {
            NIASSERT(isdigit(pcToken[0]));

            uiRegisterCount = atoi(pcToken);
            // Reset the key - removing the entry count
            NIASSERT((strlen(pcKey) + 1) < uiMaxLen);
            NiStrcpy(pszTrueName, uiMaxLen, pcKey);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
bool NiShaderConstantMap::LookUpPredefinedMapping(const char* pszMapping,
    unsigned int& uiMappingID, unsigned int& uiNumRegisters)
{
    char szTrueName[NI_MAX_PATH];

    if (!ParseKeyName(pszMapping, szTrueName, NI_MAX_PATH, uiNumRegisters))
        return false;

    for (unsigned int ui = 0; ui < ms_uiPredefinedMappingCount; ui++)
    {
        const PredefinedMapping* pkMapping = 
            &ms_pkPredefinedMappings[ui];

        if (NiStricmp(pkMapping->m_kMappingName, szTrueName) == 0)
        {
            uiMappingID = pkMapping->m_uiMappingID;
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiShaderConstantMap::LookUpPredefinedMappingName(
    unsigned int uiMappingID, NiFixedString& kName)
{
    NIASSERT(ms_pkPredefinedMappings);
    for (unsigned int ui = 0; ui < ms_uiPredefinedMappingCount; ui++)
    {
        if (ms_pkPredefinedMappings[ui].m_uiMappingID == uiMappingID)
        {
            kName = ms_pkPredefinedMappings[ui].m_kMappingName;
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
NiShaderAttributeDesc::AttributeType 
NiShaderConstantMap::LookUpPredefinedMappingType(unsigned int uiMappingID, 
    unsigned int uiNumRegisters)
{
    switch (uiMappingID)
    {
    case SCM_DEF_PROJ:
    case SCM_DEF_INVPROJ:
    case SCM_DEF_VIEW:
    case SCM_DEF_INVVIEW:
    case SCM_DEF_WORLD:
    case SCM_DEF_INVWORLD:
    case SCM_DEF_WORLDVIEW:
    case SCM_DEF_INVWORLDVIEW:
    case SCM_DEF_VIEWPROJ:
    case SCM_DEF_INVVIEWPROJ:
    case SCM_DEF_WORLDVIEWPROJ:
    case SCM_DEF_INVWORLDVIEWPROJ:
    case SCM_DEF_PROJ_T:
    case SCM_DEF_INVPROJ_T:
    case SCM_DEF_VIEW_T:
    case SCM_DEF_INVVIEW_T:
    case SCM_DEF_WORLD_T:
    case SCM_DEF_INVWORLD_T:
    case SCM_DEF_WORLDVIEW_T:
    case SCM_DEF_INVWORLDVIEW_T:
    case SCM_DEF_VIEWPROJ_T:
    case SCM_DEF_INVVIEWPROJ_T:
    case SCM_DEF_WORLDVIEWPROJ_T:
    case SCM_DEF_INVWORLDVIEWPROJ_T:
        if (uiNumRegisters == 3)
            return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
        else
            return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;

    case SCM_DEF_SKINBONE_MATRIX_3:
        return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;

    case SCM_DEF_TEXTRANSFORMBASE:
    case SCM_DEF_INVTEXTRANSFORMBASE:
    case SCM_DEF_TEXTRANSFORMBASE_T:
    case SCM_DEF_INVTEXTRANSFORMBASE_T:
    case SCM_DEF_TEXTRANSFORMDARK:
    case SCM_DEF_INVTEXTRANSFORMDARK:
    case SCM_DEF_TEXTRANSFORMDARK_T:
    case SCM_DEF_INVTEXTRANSFORMDARK_T:
    case SCM_DEF_TEXTRANSFORMDETAIL:
    case SCM_DEF_INVTEXTRANSFORMDETAIL:
    case SCM_DEF_TEXTRANSFORMDETAIL_T:
    case SCM_DEF_INVTEXTRANSFORMDETAIL_T:
    case SCM_DEF_TEXTRANSFORMGLOSS:
    case SCM_DEF_INVTEXTRANSFORMGLOSS:
    case SCM_DEF_TEXTRANSFORMGLOSS_T:
    case SCM_DEF_INVTEXTRANSFORMGLOSS_T:
    case SCM_DEF_TEXTRANSFORMGLOW:
    case SCM_DEF_INVTEXTRANSFORMGLOW:
    case SCM_DEF_TEXTRANSFORMGLOW_T:
    case SCM_DEF_INVTEXTRANSFORMGLOW_T:
    case SCM_DEF_TEXTRANSFORMBUMP:
    case SCM_DEF_INVTEXTRANSFORMBUMP:
    case SCM_DEF_TEXTRANSFORMBUMP_T:
    case SCM_DEF_INVTEXTRANSFORMBUMP_T:
    case SCM_DEF_TEXTRANSFORMDECAL:
    case SCM_DEF_INVTEXTRANSFORMDECAL:
    case SCM_DEF_TEXTRANSFORMDECAL_T:
    case SCM_DEF_INVTEXTRANSFORMDECAL_T:
    case SCM_DEF_TEXTRANSFORMSHADER:
    case SCM_DEF_INVTEXTRANSFORMSHADER:
    case SCM_DEF_TEXTRANSFORMSHADER_T:
    case SCM_DEF_INVTEXTRANSFORMSHADER_T:
    case SCM_DEF_TEXTRANSFORMNORMAL:
    case SCM_DEF_INVTEXTRANSFORMNORMAL:
    case SCM_DEF_TEXTRANSFORMNORMAL_T:
    case SCM_DEF_INVTEXTRANSFORMNORMAL_T:
    case SCM_DEF_TEXTRANSFORMPARALLAX:
    case SCM_DEF_INVTEXTRANSFORMPARALLAX:
    case SCM_DEF_TEXTRANSFORMPARALLAX_T:
    case SCM_DEF_INVTEXTRANSFORMPARALLAX_T:
        if (uiNumRegisters == 3)
            return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX3;
        else
            return NiShaderAttributeDesc::ATTRIB_TYPE_MATRIX4;

    case SCM_DEF_LIGHT_POS_WS:
    case SCM_DEF_LIGHT_DIR_WS:
    case SCM_DEF_LIGHT_POS_OS:
    case SCM_DEF_LIGHT_DIR_OS:
        return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;

    case SCM_DEF_MATERIAL_DIFFUSE:
    case SCM_DEF_MATERIAL_AMBIENT:
    case SCM_DEF_MATERIAL_SPECULAR:
    case SCM_DEF_MATERIAL_EMISSIVE:
    case SCM_DEF_MATERIAL_POWER:
        return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;

    case SCM_DEF_EYE_POS:
    case SCM_DEF_EYE_DIR:
        return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;

    case SCM_DEF_CONSTS_TAYLOR_SIN:
    case SCM_DEF_CONSTS_TAYLOR_COS:
    case SCM_DEF_CONSTS_TIME:
    case SCM_DEF_CONSTS_SINTIME:
    case SCM_DEF_CONSTS_COSTIME:
    case SCM_DEF_CONSTS_TANTIME:
    case SCM_DEF_CONSTS_TIME_SINTIME_COSTIME_TANTIME:
        return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;

    case SCM_DEF_AMBIENTLIGHT:
        return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;

    case SCM_DEF_FOG_DENSITY:
    case SCM_DEF_FOG_NEARFAR:
    case SCM_DEF_FOG_COLOR:
        return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
    case SCM_DEF_PARALLAX_OFFSET:
        return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;
    case SCM_DEF_BUMP_MATRIX:
        return NiShaderAttributeDesc::ATTRIB_TYPE_POINT4;
    case SCM_DEF_BUMP_LUMA_OFFSET_AND_SCALE:
        return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;
    
    case SCM_DEF_TEXSIZEBASE:
    case SCM_DEF_TEXSIZEDARK:
    case SCM_DEF_TEXSIZEDETAIL:
    case SCM_DEF_TEXSIZEGLOSS:
    case SCM_DEF_TEXSIZEGLOW:
    case SCM_DEF_TEXSIZEBUMP:
    case SCM_DEF_TEXSIZENORMAL:
    case SCM_DEF_TEXSIZEPARALLAX:
    case SCM_DEF_TEXSIZEDECAL:
    case SCM_DEF_TEXSIZESHADER:
        return NiShaderAttributeDesc::ATTRIB_TYPE_POINT2;

    case SCM_DEF_ALPHA_TEST_FUNC:
        return NiShaderAttributeDesc::ATTRIB_TYPE_POINT3;

    case SCM_DEF_ALPHA_TEST_REF:
        return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;

    case SCM_DEF_XENON_INST_FREQUENCY:
        return NiShaderAttributeDesc::ATTRIB_TYPE_FLOAT;

    // Deprecated predefined mappings
    case SCM_DEF_SKINWORLDVIEW:
    case SCM_DEF_INVSKINWORLDVIEW:
    case SCM_DEF_SKINWORLDVIEW_T:
    case SCM_DEF_INVSKINWORLDVIEW_T:
    case SCM_DEF_SKINWORLDVIEWPROJ:
    case SCM_DEF_INVSKINWORLDVIEWPROJ:
    case SCM_DEF_SKINWORLDVIEWPROJ_T:
    case SCM_DEF_INVSKINWORLDVIEWPROJ_T:
    case SCM_DEF_SKINWORLD:
    case SCM_DEF_INVSKINWORLD:
    case SCM_DEF_SKINWORLD_T:
    case SCM_DEF_INVSKINWORLD_T:
    case SCM_DEF_BONE_MATRIX_3:
    case SCM_DEF_BONE_MATRIX_4:
    case SCM_DEF_SKINBONE_MATRIX_4:
        return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
    }

    return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
}
//---------------------------------------------------------------------------
NiShaderAttributeDesc::AttributeType 
NiShaderConstantMap::LookUpPredefinedMappingType(const char* pszMapping)
{
    unsigned int uiMappingID;
    unsigned int uiNumRegisters = 0;
    if (LookUpPredefinedMapping(pszMapping, uiMappingID, uiNumRegisters) &&
        uiMappingID != 0)
    {
        return LookUpPredefinedMappingType(uiMappingID, uiNumRegisters);
    }
    return NiShaderAttributeDesc::ATTRIB_TYPE_UNDEFINED;
}
//---------------------------------------------------------------------------
bool NiShaderConstantMap::LookUpObjectMapping(const char* pcMapping,
    unsigned int& uiMappingID)
{
    for (unsigned int ui = 0; ui < ms_uiObjectMappingCount; ui++)
    {
        const PredefinedMapping* pkMapping = 
            &ms_pkObjectMappings[ui];

        if (NiStricmp(pkMapping->m_kMappingName, pcMapping) == 0)
        {
            uiMappingID = pkMapping->m_uiMappingID;
            return true;
        }
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiShaderConstantMap::LookUpObjectMappingName(
    unsigned int uiMappingID, NiFixedString& kName)
{
    NIASSERT(ms_pkObjectMappings);
    for (unsigned int ui = 0; ui < ms_uiObjectMappingCount; ui++)
    {
        if (ms_pkObjectMappings[ui].m_uiMappingID == uiMappingID)
        {
            kName = ms_pkObjectMappings[ui].m_kMappingName;
            return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
bool NiShaderConstantMap::IsObjectMappingValidForType(
    ObjectMappings eMapping, NiShaderAttributeDesc::ObjectType eType)
{
    // The code in this function relies on the order of the
    // NiShaderAttributeDesc::ObjectType enumeration. No changes should be
    // made to the order of existing values

    bool bValid = true;

    switch (eMapping)
    {
    case SCM_OBJ_DIMMER:
    case SCM_OBJ_UNDIMMEDAMBIENT:
    case SCM_OBJ_UNDIMMEDDIFFUSE:
    case SCM_OBJ_UNDIMMEDSPECULAR:
    case SCM_OBJ_AMBIENT:
    case SCM_OBJ_DIFFUSE:
    case SCM_OBJ_SPECULAR:
    case SCM_OBJ_WORLDDIRECTION:
    case SCM_OBJ_MODELDIRECTION:
    case SCM_OBJ_SPOTATTENUATION:
    case SCM_OBJ_ATTENUATION:
        // Only valid for lights.
        if (eType < NiShaderAttributeDesc::OT_EFFECT_GENERALLIGHT ||
            eType > NiShaderAttributeDesc::OT_EFFECT_SHADOWSPOTLIGHT)
        {
            bValid = false;
        }
        break;
    case SCM_OBJ_WORLDPOSITION:
    case SCM_OBJ_MODELPOSITION:
    case SCM_OBJ_WORLDTRANSFORM:
    case SCM_OBJ_MODELTRANSFORM:
        // Only valid for dynamic effects.
        if (eType < NiShaderAttributeDesc::OT_EFFECT_GENERALLIGHT ||
            eType > NiShaderAttributeDesc::OT_EFFECT_FOGMAP)
        {
            bValid = false;
        }
        break;
    case SCM_OBJ_WORLDPROJECTIONMATRIX:
    case SCM_OBJ_MODELPROJECTIONMATRIX:
    case SCM_OBJ_WORLDPROJECTIONTRANSLATION:
    case SCM_OBJ_MODELPROJECTIONTRANSLATION:
    case SCM_OBJ_WORLDCLIPPINGPLANE:
    case SCM_OBJ_MODELCLIPPINGPLANE:
    case SCM_OBJ_TEXCOORDGEN:
    case SCM_OBJ_WORLDPROJECTIONTRANSFORM:
    case SCM_OBJ_MODELPROJECTIONTRANSFORM:
        // Only valid for texture effects.
        if (eType < NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP ||
            eType > NiShaderAttributeDesc::OT_EFFECT_FOGMAP)
        {
            bValid = false;
        }
        break;
    case SCM_OBJ_WORLDTOSHADOWMAPMATRIX:
    case SCM_OBJ_SHADOWMAPTEXSIZE:
    case SCM_OBJ_SHADOWBIAS:
    case SCM_OBJ_SHADOW_VSM_POWER_EPSILON:
        // Only valid for lights.
        if (eType < NiShaderAttributeDesc::OT_EFFECT_GENERALLIGHT ||
            eType > NiShaderAttributeDesc::OT_EFFECT_SHADOWSPOTLIGHT)
        {
            bValid = false;
        }
        break;
    default:
        // Unknown mapping.
        bValid = false;
        break;
    }

    return bValid;
}
//---------------------------------------------------------------------------
NiDynamicEffect* NiShaderConstantMap::GetDynamicEffectForObject(
    const NiDynamicEffectState* pkEffectState,
    NiShaderAttributeDesc::ObjectType eObjectType, unsigned int uiObjectIndex)
{
    NiDynamicEffect* pkDynEffect = NULL;
    if (pkEffectState)
    {
        switch (eObjectType)
        {
        case NiShaderAttributeDesc::OT_EFFECT_GENERALLIGHT:
        case NiShaderAttributeDesc::OT_EFFECT_POINTLIGHT:
        case NiShaderAttributeDesc::OT_EFFECT_DIRECTIONALLIGHT:
        case NiShaderAttributeDesc::OT_EFFECT_SPOTLIGHT:
            {
                unsigned int uiNonShadowedLights = 0;
                unsigned int uiIndex = 0;
                NiDynEffectStateIter kIter =
                    pkEffectState->GetLightHeadPos();

                while (kIter)
                {
                    NiLight* pkLight = pkEffectState->GetNextLight(kIter);
                    if (IsLightCorrectType(pkLight, eObjectType))
                    {
                        uiNonShadowedLights++;
                        if (uiIndex++ == uiObjectIndex)
                            return pkLight;
                    }
                }

                // If we could not find a valid non-shadowed light search for 
                // a shadowed light of the same type. This is done for geometry
                // objects that are un-affected shadow receivers. They will
                // be searching for none shadowed lights when in fact the need
                // a shadowed light. 
                // Note: This is not a full proof solution and can cause 
                // in-correct light with combinations of shadowed and non-
                // shadowed lights applied to the same geometry object if one
                // of the non-shadowed lights is turned off.

                // Adjust the object index to account for the number of
                // non-shadow casting lights.
                if (uiObjectIndex < uiNonShadowedLights)
                    uiObjectIndex = 0;
                else
                    uiObjectIndex -= uiNonShadowedLights;

                // Reset index to zero.
                uiIndex = 0;

                if (eObjectType == 
                    NiShaderAttributeDesc::OT_EFFECT_GENERALLIGHT)
                {
                    // There is not an shadow equivalent to the GENERALLIGHT, 
                    // so instead we check the whole range of shadow light
                    // types.
                    kIter = pkEffectState->GetLightHeadPos();
                    while (kIter)
                    {
                        NiLight* pkLight = pkEffectState->GetNextLight(kIter);
                        unsigned int uiEffectType = pkLight->GetEffectType();

                        if ((uiEffectType >= 
                            NiShaderAttributeDesc::OT_EFFECT_SHADOWPOINTLIGHT
                            && uiEffectType <=
                            NiShaderAttributeDesc::OT_EFFECT_SHADOWSPOTLIGHT)
                            && uiIndex++ == uiObjectIndex)
                        {
                            pkDynEffect = pkLight;
                            break;
                        }
                    }

                    break;
                }

                // Adjust object type to a shadowing light object.
                eObjectType = (NiShaderAttributeDesc::ObjectType)
                    ((unsigned int)eObjectType + 
                    (NiShaderAttributeDesc::OT_EFFECT_SPOTLIGHT - 
                    NiShaderAttributeDesc::OT_EFFECT_GENERALLIGHT));

                kIter = pkEffectState->GetLightHeadPos();
                while (kIter)
                {
                    NiLight* pkLight = pkEffectState->GetNextLight(kIter);
                    if (IsLightCorrectType(pkLight, eObjectType)
                        && uiIndex++ == uiObjectIndex)
                    {
                        pkDynEffect = pkLight;
                        break;
                    }
                }
                break;
            }

        case NiShaderAttributeDesc::OT_EFFECT_SHADOWPOINTLIGHT:
        case NiShaderAttributeDesc::OT_EFFECT_SHADOWDIRECTIONALLIGHT:
        case NiShaderAttributeDesc::OT_EFFECT_SHADOWSPOTLIGHT:
            {
                unsigned int uiIndex = 0;
                NiDynEffectStateIter kIter =
                    pkEffectState->GetLightHeadPos();
                while (kIter)
                {
                    NiLight* pkLight = pkEffectState->GetNextLight(kIter);
                    if (IsLightCorrectType(pkLight, eObjectType)
                        && uiIndex++ == uiObjectIndex)
                    {
                        pkDynEffect = pkLight;
                        break;
                    }
                }
                break;
            }
        case NiShaderAttributeDesc::OT_EFFECT_ENVIRONMENTMAP:
            pkDynEffect = pkEffectState->GetEnvironmentMap();
            if (pkDynEffect && !pkDynEffect->GetSwitch())
                pkDynEffect = NULL;
            break;
        case NiShaderAttributeDesc::OT_EFFECT_PROJECTEDSHADOWMAP:
            {
                unsigned int uiIndex = 0;
                NiDynEffectStateIter kIter =
                    pkEffectState->GetProjShadowHeadPos();
                while (kIter)
                {
                    NiTextureEffect* pkProjShadow =
                        pkEffectState->GetNextProjShadow(kIter);

                    if (pkProjShadow && pkProjShadow->GetSwitch())
                    {
                        if (uiIndex++ == uiObjectIndex)
                        {
                            pkDynEffect = pkProjShadow;
                            break;
                        }
                    }
                }
                break;
            }
        case NiShaderAttributeDesc::OT_EFFECT_PROJECTEDLIGHTMAP:
            {
                unsigned int uiIndex = 0;
                NiDynEffectStateIter kIter =
                    pkEffectState->GetProjLightHeadPos();
                while (kIter)
                {
                    NiTextureEffect* pkProjLight =
                        pkEffectState->GetNextProjLight(kIter);
                    if (pkProjLight && pkProjLight->GetSwitch())
                    {
                        if (uiIndex++ == uiObjectIndex)
                        {
                            pkDynEffect = pkProjLight;
                            break;
                        }
                    }
                }
                break;
            }
        case NiShaderAttributeDesc::OT_EFFECT_FOGMAP:
            pkDynEffect = pkEffectState->GetFogMap();
            if (pkDynEffect && !pkDynEffect->GetSwitch())
                pkDynEffect = NULL;
            break;
        default:
            break;
        }
    }

    return pkDynEffect;
}
//---------------------------------------------------------------------------
NiShaderError NiShaderConstantMap::AddEntry(const char*, 
    unsigned int, unsigned int, unsigned int, 
    unsigned int, const char*, 
    unsigned int, unsigned int, 
    const void*, bool)
{
    return NISHADERERR_ENTRYNOTADDED;
}
//---------------------------------------------------------------------------
NiShaderError NiShaderConstantMap::AddPredefinedEntry(const char*, 
    unsigned int, unsigned int, 
    const char*)
{
    return NISHADERERR_ENTRYNOTADDED;
}
//---------------------------------------------------------------------------

NiShaderError NiShaderConstantMap::AddAttributeEntry(const char*, 
    unsigned int, unsigned int, 
    unsigned int, unsigned int, 
    const char*, unsigned int, 
    unsigned int, const void*, 
    bool)
{
    return NISHADERERR_ENTRYNOTADDED;
}
//---------------------------------------------------------------------------
NiShaderError NiShaderConstantMap::AddConstantEntry(const char*, 
    unsigned int, unsigned int, 
    unsigned int, unsigned int, 
    const char*, unsigned int, 
    unsigned int, const void*, 
    bool)
{
    return NISHADERERR_ENTRYNOTADDED;
}
//---------------------------------------------------------------------------
NiShaderError NiShaderConstantMap::AddGlobalEntry(const char*, 
    unsigned int, unsigned int, 
    unsigned int, unsigned int, 
    const char*, unsigned int, 
    unsigned int, const void*, 
    bool)
{
    return NISHADERERR_ENTRYNOTADDED;
}
//---------------------------------------------------------------------------
NiShaderError NiShaderConstantMap::AddOperatorEntry(const char*, 
    unsigned int, unsigned int, 
    unsigned int, unsigned int, 
    const char*)
{
    return NISHADERERR_ENTRYNOTADDED;
}
//---------------------------------------------------------------------------
NiShaderError NiShaderConstantMap::AddObjectEntry(const char*,
    unsigned int, const char*,
    unsigned int, NiShaderAttributeDesc::ObjectType)
{
    return NISHADERERR_ENTRYNOTADDED;
}
//---------------------------------------------------------------------------
