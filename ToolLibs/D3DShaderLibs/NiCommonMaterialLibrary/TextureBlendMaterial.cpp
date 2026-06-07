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

#include "TextureBlendMaterial.h"

#include <NiStandardMaterialDescriptor.h>
#include <NiStandardMaterialNodeLibrary.h>
#include <NiStandardPixelProgramDescriptor.h>
#include <NiStandardVertexProgramDescriptor.h>
#include <NiRenderer.h>
#include "NiCommonMaterialLib.h"
#include <NiStandardMaterialNodeLibrary.h>

NiImplementRTTI(TextureBlendMaterial, NiStandardMaterial);
//---------------------------------------------------------------------------
TextureBlendMaterial::TextureBlendMaterial(bool bAutoCreateCaches) : 
    NiStandardMaterial("TextureBlendMaterial", NULL, 
    NITEXTUREBLENDMATERIAL_VERTEX_VERSION, 
    NITEXTUREBLENDMATERIAL_GEOMETRY_VERSION, 
    NITEXTUREBLENDMATERIAL_PIXEL_VERSION, bAutoCreateCaches)
{
    m_kLibraries.Add(
        NiStandardMaterialNodeLibrary::CreateMaterialNodeLibrary());
    AddDefaultFallbacks();
}
//---------------------------------------------------------------------------
bool TextureBlendMaterial::HandleCustomMaps(Context& kContext, 
    NiStandardPixelProgramDescriptor* pkPixDesc, 
    unsigned int&, NiMaterialResource** apkUVSets,
    unsigned int,
    NiMaterialResource*&,
    NiMaterialResource*&,
    NiMaterialResource*&,
    NiMaterialResource*&,
    NiMaterialResource*&,
    NiMaterialResource*&,
    NiMaterialResource*&,
    NiMaterialResource*&,
    NiMaterialResource*&,
    NiMaterialResource*&,
    NiMaterialResource*& pkDiffuseTexAccum,
    NiMaterialResource*&)
{
    NiMaterialResource* apkColors[TEXTURE_COUNT];
    memset(apkColors, 0, sizeof(NiMaterialResource*) * TEXTURE_COUNT);

    // Implement the following equation:
    // DiffuseTexAccum.rgb *= ShaderMap0.rgb * ShaderMap4.r + 
    //      ShaderMap1.rgb * ShaderMap4.g + ShaderMap2.rgb * ShaderMap4.b +
    //      ShaderMap3.rgb * ShaderMap4.a

    // Create and sample all of the relevant textures.
    for (unsigned int ui = 0; ui < TEXTURE_COUNT; ui++)
    {
        bool bExists = false;
        switch (ui)
        {
        case 0:
            bExists = NiBitfieldGetIndexedField(
                NiStandardPixelProgramDescriptor::CUSTOMMAP00COUNT,
                unsigned int, pkPixDesc->m_auiBitArray) ? true : false;
            break;
        case 1:
            bExists = NiBitfieldGetIndexedField(
                NiStandardPixelProgramDescriptor::CUSTOMMAP01COUNT,
                unsigned int, pkPixDesc->m_auiBitArray) ? true : false;
            break;
        case 2:
            bExists = NiBitfieldGetIndexedField(
                NiStandardPixelProgramDescriptor::CUSTOMMAP02COUNT,
                unsigned int, pkPixDesc->m_auiBitArray) ? true : false;
            break;
        case 3:
            bExists = NiBitfieldGetIndexedField(
                NiStandardPixelProgramDescriptor::CUSTOMMAP03COUNT,
                unsigned int, pkPixDesc->m_auiBitArray) ? true : false;
            break;
        case 4:
            bExists = NiBitfieldGetIndexedField(
                NiStandardPixelProgramDescriptor::CUSTOMMAP04COUNT,
                unsigned int, pkPixDesc->m_auiBitArray) ? true : false;
            break;
        default:
            bExists = false;
            break;
        }

        if (bExists)
        {
            NiMaterialResource* pkUVRes = FindUVSetIndexForTextureEnum( 
                (TextureMap)(MAP_CUSTOM00 + ui), 
                pkPixDesc, apkUVSets);

            if (!pkUVRes)
                return false;

            NiFixedString kSamplerName;
            if (!GetTextureNameFromTextureEnum((TextureMap)(MAP_CUSTOM00 + ui),
                kSamplerName, ui))
            {
                return false;
            }

            NiMaterialResource* pkSamplerRes = InsertTextureSampler(kContext,
                kSamplerName, TEXTURE_SAMPLER_2D, ui);

            if (!pkSamplerRes)
                return false;

            NiMaterialNode* pkNode = GetAttachableNodeFromLibrary(
                "TextureRGBASample");

            if (!pkNode)
                return false;

            kContext.m_spConfigurator->AddNode(pkNode);
            bool bSuccess = kContext.m_spConfigurator->AddBinding(
                pkUVRes, pkNode->GetInputResourceByVariableName("TexCoord"));
            bSuccess &= kContext.m_spConfigurator->AddBinding(pkSamplerRes, 
                pkNode->GetInputResourceByVariableName("Sampler"));
            bSuccess &= kContext.m_spConfigurator->AddBinding(
                kContext.m_spStatics->AddOutputConstant("bool", "(false)"),
                "Saturate", pkNode);

            if (!bSuccess)
            {
                NIASSERT(!"Error in fragment");
                return false;
            }

            apkColors[ui] = pkNode->GetOutputResourceByVariableName(
                "ColorOut");
        }
    }

    // Determine if the control map was created. If so, split into independent
    // channels. If not, use the fixed scalar of 0.25f per texture.
    NiMaterialResource* apkScalars[BLEND_TEXTURE_COUNT];
    if (apkColors[CONTROL_MAP_INDEX] == NULL)
    {
        NiMaterialResource* pkInputConstant = 
            kContext.m_spStatics->AddOutputConstant("float3", 
            "(0.25, 0.25, 0.25)");

        for (unsigned int ui = 0; ui < BLEND_TEXTURE_COUNT; ui++)
        {
            apkScalars[ui] = pkInputConstant;
        }
    }
    else
    {
        if (!SplitRGBA(kContext, apkColors[CONTROL_MAP_INDEX], 
            apkScalars[RED_MAP_INDEX], apkScalars[GREEN_MAP_INDEX], 
            apkScalars[BLUE_MAP_INDEX], apkScalars[ALPHA_MAP_INDEX]))
        {
            return false;
        }
    }

    // Accumulate the scaled colors into a temporary variable 
    NiMaterialResource* pkAccumColor = 
        kContext.m_spStatics->AddOutputConstant("float3", "(0.0, 0.0, 0.0)");

    for (unsigned int ui = 0; ui < BLEND_TEXTURE_COUNT; ui++)
    {
        if (apkColors[ui] && !MultiplyAddVector(kContext, apkColors[ui], 
            apkScalars[ui], pkAccumColor, pkAccumColor))
        {
            return false;
        }
    }

    return MultiplyVector(kContext, pkDiffuseTexAccum, pkAccumColor, 
        pkDiffuseTexAccum);
}
//---------------------------------------------------------------------------
bool TextureBlendMaterial::SplitBlendTextureMaps(
    NiMaterialDescriptor* pkMaterialDescriptor, ReturnCode eFailedRC,
    unsigned int uiFailedPass, RenderPassDescriptor* pkRenderPasses, 
    unsigned int uiMaxCount, unsigned int& uiCount, 
    unsigned int& uiFunctionData)
{
    NiOutputDebugString("Trying to split the blend maps"
        " across multiple passes\n");
    NiStandardMaterial::DropParallaxMap(pkMaterialDescriptor, eFailedRC,
        uiFailedPass, pkRenderPasses, uiMaxCount, uiCount, uiFunctionData);

    NiStandardPixelProgramDescriptor* pkInvalidPixelDesc = 
        (NiStandardPixelProgramDescriptor*)
        &pkRenderPasses[uiFailedPass].m_kPixelDesc;
    NiStandardVertexProgramDescriptor* pkInvalidVertexDesc = 
        (NiStandardVertexProgramDescriptor*)
        &pkRenderPasses[uiFailedPass].m_kVertexDesc;
    
    // Determine the number of blend maps needed
    unsigned int uiBlendMaps = 0;
    bool bMapsExist[4];
    for (unsigned int ui = NiStandardMaterial::MAP_CUSTOM00; ui < 
        NiStandardMaterial::MAP_CUSTOM04; ui++)
    {
        NiStandardMaterial::TextureMap eMap = 
            (NiStandardMaterial::TextureMap) ui;

        if (IsMapEnabled(eMap, pkInvalidPixelDesc))
        {
            uiBlendMaps++;
            bMapsExist[ui - NiStandardMaterial::MAP_CUSTOM00] = true;
        }
        else
        {
            bMapsExist[ui - NiStandardMaterial::MAP_CUSTOM00] = false;
        }
    }

    unsigned int uiInsertedPasses = uiBlendMaps - 1;

    if (uiInsertedPasses > uiMaxCount)
        return false;

    for (unsigned int ui = uiCount - 1; ui > uiFailedPass; ui--)
        pkRenderPasses[ui + uiInsertedPasses] = pkRenderPasses[ui];
    
    // copy all values from the failed pass over to the new passes
    for (unsigned int ui = uiFailedPass + 1; 
        ui < (uiFailedPass + uiBlendMaps); ui++)
    {
        pkRenderPasses[ui] = pkRenderPasses[uiFailedPass];
        pkRenderPasses[ui].m_bAlphaOverride = true;
        pkRenderPasses[ui].m_bAlphaBlend = true;    
        pkRenderPasses[ui].m_bUsePreviousSrcBlendMode = true;
        pkRenderPasses[ui].m_bUsePreviousDestBlendMode = false;
        pkRenderPasses[ui].m_eSrcBlendMode = 
            NiAlphaProperty::ALPHA_ONE;
        pkRenderPasses[ui].m_eDestBlendMode = 
            NiAlphaProperty::ALPHA_ONE;
    }

    // Split the blend maps one per pass
    unsigned int uiWorkingPass = uiFailedPass;
    for (unsigned int ui = 0; ui < 4; ui++)
    {
        if (!bMapsExist[ui])
            continue;

        pkInvalidPixelDesc = (NiStandardPixelProgramDescriptor*)
            &pkRenderPasses[uiWorkingPass].m_kPixelDesc;
        pkInvalidVertexDesc = (NiStandardVertexProgramDescriptor*)
            &pkRenderPasses[uiWorkingPass].m_kVertexDesc;

        for (unsigned int uiCustomMapIdx = 0; uiCustomMapIdx < 4;
            uiCustomMapIdx++)
        {
            NiStandardMaterial::TextureMap eMap = (TextureMap)
                (MAP_CUSTOM00 + uiCustomMapIdx);

            if (uiCustomMapIdx != ui)
            {
                RemoveTextureMap(eMap, pkInvalidVertexDesc, 
                    pkInvalidPixelDesc);
            }
        }

        // The first pass will have all specular, emissive, and ambient 
        // lighting. The first pass will also have all glow/gloss/environment
        // maps.
        if (ui != 0)
        {
            RemoveTextureMap(NiStandardMaterial::MAP_GLOW, pkInvalidVertexDesc,
                pkInvalidPixelDesc);
            RemoveTextureMap(NiStandardMaterial::MAP_GLOSS, 
                pkInvalidVertexDesc, pkInvalidPixelDesc);
            RemoveTextureMap(NiStandardMaterial::MAP_BUMP, pkInvalidVertexDesc,
                pkInvalidPixelDesc);
            
            NiBitfieldSetIndexedField(
                NiStandardPixelProgramDescriptor::ENVMAPTYPE,
                TEXEFFECT_NONE, unsigned int, 
                pkInvalidPixelDesc->m_auiBitArray);
            NiBitfieldSetIndexedField(
                NiStandardPixelProgramDescriptor::SPECULAR,
                0, unsigned int, pkInvalidPixelDesc->m_auiBitArray);
            NiBitfieldSetIndexedField(
                NiStandardVertexProgramDescriptor::SPECULAR,
                0, unsigned int, pkInvalidVertexDesc->m_auiBitArray);
            
            NiBitfieldSetIndexedField(
                NiStandardVertexProgramDescriptor::OUTPUTWORLDVIEW,
                0, unsigned int, pkInvalidVertexDesc->m_auiBitArray);
            NiBitfieldSetIndexedField(
                NiStandardPixelProgramDescriptor::WORLDVIEW,
                0, unsigned int, pkInvalidPixelDesc->m_auiBitArray);

            NiBitfieldSetIndexedField(
                NiStandardPixelProgramDescriptor::APPLYAMBIENT,
                0, unsigned int, pkInvalidPixelDesc->m_auiBitArray);
            NiBitfieldSetIndexedField(
                NiStandardPixelProgramDescriptor::APPLYEMISSIVE,
                0, unsigned int, pkInvalidPixelDesc->m_auiBitArray);

        }

        uiWorkingPass++;
    }

    NIASSERT(uiWorkingPass - uiFailedPass == uiBlendMaps);
    uiCount += uiInsertedPasses;
    return true;
}
//---------------------------------------------------------------------------
void TextureBlendMaterial::AddDefaultFallbacks()
{
    AddShaderFallbackFunc(SplitBlendTextureMaps);
    AddShaderFallbackFunc(SplitPerPixelLights);
    AddShaderFallbackFunc(SplitPerVertexLights);
    AddShaderFallbackFunc(DropParallaxMap);
    AddShaderFallbackFunc(DropParallaxMapThenSplitLights);

    NiFragmentMaterial::AddDefaultFallbacks();
}
//---------------------------------------------------------------------------
