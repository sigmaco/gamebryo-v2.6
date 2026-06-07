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

#include "NiCommonMaterialLibPCH.h"

#include "AdditiveLightMapsMaterial.h"

#include <NiStandardMaterialDescriptor.h>
#include <NiStandardMaterialNodeLibrary.h>
#include <NiStandardPixelProgramDescriptor.h>
#include <NiStandardVertexProgramDescriptor.h>
#include <NiRenderer.h>
#include "NiCommonMaterialLib.h"
#include <NiStandardMaterialNodeLibrary.h>

NiImplementRTTI(AdditiveLightMapsMaterial, NiStandardMaterial);

//---------------------------------------------------------------------------
AdditiveLightMapsMaterial::AdditiveLightMapsMaterial(bool bAutoCreateCaches)
    : NiStandardMaterial("AdditiveLightMapsMaterial", NULL, 
    NIADDITIVELIGHTMAPSMATERIAL_VERTEX_VERSION, 
    NIADDITIVELIGHTMAPSMATERIAL_GEOMETRY_VERSION, 
    NIADDITIVELIGHTMAPSMATERIAL_PIXEL_VERSION, bAutoCreateCaches)
{
    m_kLibraries.Add(
        NiStandardMaterialNodeLibrary::CreateMaterialNodeLibrary());
    AddDefaultFallbacks();
}
//---------------------------------------------------------------------------
bool AdditiveLightMapsMaterial::HandlePreLightTextureApplication(
    Context& kContext, NiStandardPixelProgramDescriptor* pkPixelDesc,
    NiMaterialResource*& pkWorldPos, NiMaterialResource*& pkWorldNormal,
    NiMaterialResource*& pkWorldBinormal, NiMaterialResource*& pkWorldTangent,
    NiMaterialResource*&,
    NiMaterialResource*& pkTangentViewVector,
    NiMaterialResource*& pkMatDiffuseColor,
    NiMaterialResource*& pkMatSpecularColor,
    NiMaterialResource*& pkMatSpecularPower,
    NiMaterialResource*& pkMatGlossiness,
    NiMaterialResource*& pkMatAmbientColor,
    NiMaterialResource*& pkMatEmissiveColor,
    NiMaterialResource*& pkOpacityAccum,
    NiMaterialResource*& pkAmbientLightAccum,
    NiMaterialResource*& pkDiffuseLightAccum,
    NiMaterialResource*& pkSpecularLightAccum,
    NiMaterialResource*& pkDiffuseTexAccum,
    NiMaterialResource*& pkSpecularTexAccum, unsigned int& uiTexturesApplied,
    NiMaterialResource** apkUVSets, unsigned int uiNumStandardUVs,
    unsigned int)
{
    unsigned int uiWhichTexture = 0;

    if (pkPixelDesc->GetPARALLAXMAPCOUNT()  == 1)
    {
        unsigned int uiUVSet = FindUVSetIndexForTextureEnum( 
            MAP_PARALLAX, pkPixelDesc);

        NiMaterialResource* pkUVSet = apkUVSets[uiUVSet];

        if (!HandleParallaxMap(kContext, pkUVSet, pkTangentViewVector))
        {
            return false;
        }
        uiWhichTexture++;

        apkUVSets[uiUVSet] = pkUVSet;
    }

    bool bNormalMap = pkPixelDesc->GetNORMALMAPCOUNT() != 0;
    if (bNormalMap)
    {
        NormalMapType eNormMapType = (NormalMapType)
            pkPixelDesc->GetNORMALMAPTYPE();

        if (!HandleNormalMap(kContext, 
            FindUVSetIndexForTextureEnum(MAP_NORMAL, 
            pkPixelDesc, apkUVSets), eNormMapType, pkWorldNormal, 
            pkWorldBinormal, pkWorldTangent))
        {
            return false;
        }
        uiWhichTexture++;
    }
    
    if (pkPixelDesc->GetDARKMAPCOUNT() == 1)
    {
        if (!HandleDarkMap(kContext, 
            FindUVSetIndexForTextureEnum( MAP_DARK, 
            pkPixelDesc, apkUVSets), pkDiffuseTexAccum,
            pkSpecularTexAccum))
        {
            return false;
        }

        uiWhichTexture++;
    }
    unsigned int uiTexEffectOffset = 0;

    if (pkPixelDesc->GetENVMAPTYPE() != TEXEFFECT_NONE && 
        pkPixelDesc->GetNORMALMAPCOUNT() == 0)
    {
        uiTexEffectOffset++;
    }

    unsigned int uiProjLightMapCount = pkPixelDesc->GetPROJLIGHTMAPCOUNT();

    // For this material, projected light maps are additive, so process the
    // base map before the projected light maps.
    if (pkPixelDesc->GetBASEMAPCOUNT() == 1)
    {
         bool bAlphaOnly = pkPixelDesc->GetBASEMAPALPHAONLY() == 1;

         if (!HandleBaseMap(kContext, 
             FindUVSetIndexForTextureEnum( MAP_BASE, 
             pkPixelDesc, apkUVSets), pkDiffuseTexAccum, pkOpacityAccum, 
             bAlphaOnly))
         {
             return false;
         }
         uiWhichTexture++;
    }
    else if (pkDiffuseTexAccum == NULL)
    {
        // If there's no base map, must use a constant instead.
        pkDiffuseTexAccum = kContext.m_spStatics->AddOutputConstant(
            "float3", "(1.0f, 1.0f, 1.0f)");
    }

    if (uiProjLightMapCount > 0)
    {
        unsigned int uiMask = 1;
        for (unsigned int ui = 0; ui < uiProjLightMapCount; ui++)
        {

            bool bPerspective = (pkPixelDesc->GetPROJLIGHTMAPTYPES() & uiMask)
                != 0;

            bool bClipped = (pkPixelDesc->GetPROJLIGHTMAPCLIPPED() & uiMask)
                != 0;

            if (!HandleProjLightMap(kContext, 
                apkUVSets[uiNumStandardUVs + uiTexEffectOffset], ui,
                bPerspective, bClipped, pkWorldPos, pkDiffuseTexAccum,
                pkSpecularTexAccum))
            {
                return false;
            }
            uiTexEffectOffset++;
            uiMask *= 2;
        }
    }

    if (pkPixelDesc->GetDETAILMAPCOUNT() == 1)
    {
        if (!HandleDetailMap(kContext, 
            FindUVSetIndexForTextureEnum( MAP_DETAIL, 
            pkPixelDesc, apkUVSets), pkDiffuseTexAccum,
            pkSpecularTexAccum))
        {
            return false;
        }

        uiWhichTexture++;
    }

    
    unsigned int uiDecalMapCount = pkPixelDesc->GetDECALMAPCOUNT();
    if (uiDecalMapCount > 0)
    {
        for (unsigned int ui = 0; ui < uiDecalMapCount; ui++)
        {
            if (!HandleDecalMap(kContext, 
                FindUVSetIndexForTextureEnum( 
                (TextureMap)(MAP_DECAL00 + ui), 
                pkPixelDesc, apkUVSets), ui, pkDiffuseTexAccum,
                pkSpecularTexAccum))
            {
                return false;
            }

            uiWhichTexture++;
        }
    }


    if (!HandleCustomMaps(kContext, pkPixelDesc, uiWhichTexture, apkUVSets,
        uiNumStandardUVs, pkMatDiffuseColor, pkMatSpecularColor,
        pkMatSpecularPower, pkMatGlossiness, pkMatAmbientColor,
        pkMatEmissiveColor, pkOpacityAccum, pkAmbientLightAccum,
        pkDiffuseLightAccum, pkSpecularLightAccum, pkDiffuseTexAccum,
        pkSpecularTexAccum))
    {
        return false;
    }
    
    unsigned int uiProjShadowMapCount = pkPixelDesc->GetPROJSHADOWMAPCOUNT();

    if (uiProjShadowMapCount > 0)
    {
        unsigned int uiMask = 1;
        for (unsigned int ui = 0; ui < uiProjShadowMapCount; ui++)
        {
            bool bPerspective = (pkPixelDesc->GetPROJSHADOWMAPTYPES() & uiMask)
                != 0;

            bool bClipped = (pkPixelDesc->GetPROJSHADOWMAPCLIPPED() & uiMask)
                != 0;

            if (!HandleProjShadowMap(kContext, 
                apkUVSets[uiNumStandardUVs + uiTexEffectOffset], ui,
                bPerspective, bClipped, pkWorldPos, pkDiffuseTexAccum,
                pkSpecularTexAccum))
            {
                return false;
            }
            uiTexEffectOffset++;
            uiMask *= 2;
        }
    }

    if (pkPixelDesc->GetGLOSSMAPCOUNT() == 1)
    {
        if (!HandleGlossMap(kContext, 
            FindUVSetIndexForTextureEnum( MAP_GLOSS, 
            pkPixelDesc, apkUVSets), pkMatGlossiness))
        {
            return false;
        }
        uiWhichTexture++;
    }

    uiTexturesApplied = uiWhichTexture;
    return true;
}
//---------------------------------------------------------------------------
