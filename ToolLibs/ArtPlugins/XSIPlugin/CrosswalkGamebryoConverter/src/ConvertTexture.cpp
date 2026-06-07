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

#include "CrosswalkGamebryoContext.h"
#include "CrosswalkGamebryoTexHelpers.h"
#include "CrosswalkGamebryoEffectMap.h"
#include "CrosswalkGamebryoNodeMap.h"
#include "CrosswalkGamebryoHelpers.h"

#include "XSIMaterial.h"
#include "XSIShader.h"
#include "ConnectionPoint.h"
#include "ShaderConnectionPoint.h"

#include "NiTexturingProperty.h"
#include "NiTextureEffect.h"
#include "NiSourceTexture.h"
#include "NiSourceCubeMap.h"
#include "NiPath.h"
#include "NiFilename.h"
#include "NiDDSReader.h"

namespace
{
    //---------------------------------------------------------------------------
    // Add a new effect to the effect map.
    //---------------------------------------------------------------------------
    void AddEffect(
        epg::Context& io_Context,
        CSLXSIMaterial& in_Material,
        const epg::TextureSourceInfo& in_TextureSourceInfo,
        NiTextureEffect::TextureType in_TextureType,
        NiTextureEffect::CoordGenType in_CoordGenType)
    {
        NiTextureEffectPtr pEffect = NiNew NiTextureEffect;
        pEffect->SetTextureFilter(in_TextureSourceInfo.m_FilterMode);
        pEffect->SetTextureClamp(NiTexturingProperty::CLAMP_S_CLAMP_T);
        pEffect->SetTextureType(in_TextureType);
        pEffect->SetTextureCoordGen(in_CoordGenType);
        pEffect->SetEffectTexture(in_TextureSourceInfo.m_Image);

        io_Context.GetEffectMap().AddEffect(in_Material, *pEffect);
        io_Context.GetNodeMap().AddRoot(*pEffect);
    }

    //---------------------------------------------------------------------------
    // Find a real-time shader of a particular type.
    //---------------------------------------------------------------------------
    CSLXSIShader* FindRealtimeShader(CSLXSIShader& in_Shader, const char* in_ShaderProgID)
    {
        if (!in_ShaderProgID)
            return 0;

        for (CSLXSIShader* pkShader = &in_Shader;
             pkShader;
             pkShader = epg::FindShader(*pkShader, "previous"))
        {
            const SI_Char* progID = pkShader->GetProgID();
            if (!progID)
                continue;

            if (strcmp(progID, in_ShaderProgID) == 0)
                return pkShader;
        }

        return 0;
    }

    //---------------------------------------------------------------------------
    // Find a Gamebryo real-time shader.
    // Try both Softimage and Gamebryo prefixes since the code has changed with
    // time and we better be safe than sorry.
    //---------------------------------------------------------------------------
    CSLXSIShader* FindGamebryoRealtimeShader(CSLXSIShader& in_Shader, const char* in_BaseProgID)
    {
        NiString gamebryoName("GamebryoMaterial.");
        gamebryoName.Concatenate(in_BaseProgID);
        CSLXSIShader* pkShader = FindRealtimeShader(in_Shader, gamebryoName);
        if (!pkShader)
        {
            NiString gamebryoName("Softimage.");
            gamebryoName.Concatenate(in_BaseProgID);
            CSLXSIShader* pkShader = FindRealtimeShader(in_Shader, gamebryoName);
        }

        return pkShader;
    }

    //---------------------------------------------------------------------------
    // Convert the cube-map texture UV wrapping mode.
    //---------------------------------------------------------------------------
    NiTexturingProperty::ClampMode ConvertCubeMapClampMode(CSLXSIMaterial& in_Material)
    {
        CSLXSIShader* pkRTShader = epg::FindShader(in_Material, "RealTime");
        if (!pkRTShader)
            return NiTexturingProperty::WRAP_S_WRAP_T;

        CSLXSIShader* pkShader = FindRealtimeShader(
            *pkRTShader, "Softimage.OGL13TexCoord.1");
        if (!pkShader)
            return NiTexturingProperty::WRAP_S_WRAP_T;

        // Note: we could use magic assumptions instead, but this avoids
        //       said magic assumptions about clamp mode enum values.
        NiTexturingProperty::ClampMode modes[] =
        {
            NiTexturingProperty::CLAMP_S_CLAMP_T,
            NiTexturingProperty::CLAMP_S_WRAP_T,
            NiTexturingProperty::WRAP_S_CLAMP_T,
            NiTexturingProperty::WRAP_S_WRAP_T
        };

        int index = 0;
        index += epg::GetBooleanParameter(*pkShader, "wrap_u") ? 2 : 0;
        index += epg::GetBooleanParameter(*pkShader, "wrap_v") ? 1 : 0;
        return modes[index];
    }

    //---------------------------------------------------------------------------
    // Convert the cube-map filtering.
    //---------------------------------------------------------------------------
    NiTexturingProperty::FilterMode ConvertCubeMapFilterMode(CSLXSIShader& in_Shader)
    {
        // Note: the XSI values for the filter are not declared anywhere... we will
        //       have to trust that they won't change.
        const int filter = std::max( epg::GetIntegerParameter(in_Shader, "magfilter"),
                                     epg::GetIntegerParameter(in_Shader, "minfilter") );
        switch (filter)
        {
            case 0: return NiTexturingProperty::FILTER_NEAREST;
            case 1: return NiTexturingProperty::FILTER_BILERP;
            case 2: return NiTexturingProperty::FILTER_NEAREST_MIPNEAREST;
            case 3: return NiTexturingProperty::FILTER_NEAREST_MIPLERP;
            default:
            case 4: return NiTexturingProperty::FILTER_BILERP_MIPNEAREST;
            // XSI want linear + linear mipmap, but Gamebryo doesn't support it...
            case 5: return NiTexturingProperty::FILTER_BILERP_MIPNEAREST;
        }
    }

    //---------------------------------------------------------------------------
    // Create a cub-map texture source.
    //---------------------------------------------------------------------------
    bool CreateCubeMapTextureSource(
        epg::Context& io_Context,
        CSLXSIMaterial& in_Material,
        CSLXSIShader& in_Shader,
        epg::TextureSourceInfo& out_ImageInfo)
    {
        NiImageConverter* pConverter = NiImageConverter::GetImageConverter();
        if (!pConverter)
        {
            return io_Context.Logf(
                epg::LOG_ERROR,
                "No image converter found, cannot create cube map"
                " for the \"%s\" texture of material \"%s\".",
                in_Shader.Name().GetText(),
                in_Material.Name().GetText());
        }

        // Figure out the texturing mode, which influence the source texture settings.
        out_ImageInfo.m_ClampMode = ConvertCubeMapClampMode(in_Material);
        out_ImageInfo.m_FilterMode = ConvertCubeMapFilterMode(in_Shader);

        // Extract the 6 texture pixel data and their file names.
        enum { CUBE_MAP_SIDE_COUNT = 6 };
        NiPixelData* pPixelDatas[CUBE_MAP_SIDE_COUNT] = { 0, 0, 0, 0, 0, 0 };
        NiString fileNames;
        for (int i = 0; i < CUBE_MAP_SIDE_COUNT; ++i)
        {
            static const char* cubeSideParamNames[CUBE_MAP_SIDE_COUNT] =
            {
                "right",    // +X
                "left",     // -X
                "bottom",   // +Y
                "top",      // -Y
                "back",     // +Z
                "front"     // -Z
            };

            const NiFixedString* pFileName = FindTextureFilename(
                io_Context, in_Shader, cubeSideParamNames[i]);
            if (!pFileName)
            {
                return io_Context.Logf(
                    epg::LOG_ERROR,
                    "Empty %s side parameter for the cube map"
                    " for the \"%s\" texture of material \"%s\".",
                    cubeSideParamNames[i],
                    in_Shader.Name().GetText(),
                    in_Material.Name().GetText());
            }

            pPixelDatas[i] = pConverter->ReadImageFile(*pFileName, NULL);
            if (!pPixelDatas[i])
            {
                return io_Context.Logf(
                    epg::LOG_ERROR,
                    "Cannot read the image file \"%s\" for the %s side"
                    " of the cube map for the \"%s\" texture of material \"%s\".",
                    (const char*) *pFileName,
                    cubeSideParamNames[i],
                    in_Shader.Name().GetText(),
                    in_Material.Name().GetText());
            }

            if (fileNames.Length() > 0)
                fileNames += " + ";
            fileNames += *pFileName;
        }

        // Create the cub map texture source and verify it.
        //
        // Note: we don't use the filename-based constructor because
        //       if it cannot load the images it crashes...
        out_ImageInfo.m_Image = NiSourceCubeMap::Create(
            pPixelDatas[0], pPixelDatas[1], pPixelDatas[2],
            pPixelDatas[3], pPixelDatas[4], pPixelDatas[5],
            NULL);

        // Note: we don't call VerifyTextureSource() because the validity
        //       of the image has already been checked and we must always
        //       embed the images of the cube map or the cube mapping doesn't
        //       seem to work in the asset viewer.    

        return true;
    }

    //---------------------------------------------------------------------------
    // Convert a real-time shader cube-map texture.
    //---------------------------------------------------------------------------
    bool ConvertRealtimeCubeMapEffect(
        epg::Context& io_Context,
        CSLXSIMaterial& in_Material,
        CSLXSIShader& in_Shader)
    {
        // Try to find a cubic map real-time texture in the real-time shader tree.
        // Finding nothing is OK.
        CSLXSIShader* pkShader = FindRealtimeShader(in_Shader, "Softimage.OGL13CubicTexture.1");
        if (!pkShader)
            return true;

        epg::TextureSourceInfo imageInfo;
        if (!CreateCubeMapTextureSource(io_Context, in_Material, *pkShader, imageInfo))
            return false;

        // Note: specular cube map corresponds to the XSI OpenGL cube map:
        //       it acts to make the object appear like a mirror reflecting
        //       an environment, like in XSI. The diffuse cube-map is "static"
        //       on the object, which is different from XSI.
        AddEffect(
            io_Context,
            in_Material,
            imageInfo,
            NiTextureEffect::ENVIRONMENT_MAP,
            NiTextureEffect::SPECULAR_CUBE_MAP);

        return true;
    }

    //---------------------------------------------------------------------------
    // Convert the coordinate type to the proper NiTextureEffect::CoordGenType.
    //---------------------------------------------------------------------------
    NiTextureEffect::CoordGenType ConvertCoordinateType(
        epg::Context& io_Context,
        CSLXSIShader& in_Shader,
        const char* in_ParamName,
        bool isProjectedLightOrShadow)
    {
        int mode = epg::GetIntegerParameter(
            in_Shader, epg::RealtimeParamBuilder(in_ParamName, "CoordinateType"), -1);

        if (isProjectedLightOrShadow)
        {
            switch (mode)
            {
                case 0:  return NiTextureEffect::WORLD_PARALLEL;
                case 1:  return NiTextureEffect::WORLD_PERSPECTIVE;
            }
        }
        else
        {
            switch (mode)
            {
                case 2:  return NiTextureEffect::SPHERE_MAP;
                case 3:  return NiTextureEffect::SPECULAR_CUBE_MAP;
                case 4:  return NiTextureEffect::DIFFUSE_CUBE_MAP;
            }
        }

        io_Context.Logf(
            epg::LOG_WARNING,
            "Unknown coordinate type used in the %s map of \"%s\","
            " will determine proper coordinate by analyzing the texture.",
            in_ParamName,
            in_Shader.Name().GetText());
        return NiTextureEffect::NUM_COORD_GEN;
    }

    //---------------------------------------------------------------------------
}

namespace epg
{
    //---------------------------------------------------------------------------
    // Convert the surface textures to a Gamebryo texturing property.
    // Returns true on success. Doing nothing is considered successful.
    //---------------------------------------------------------------------------
    bool ConvertSurfaceTextures(
        Context& io_Context,
        CSLXSIMaterial& in_Material)
    {
        if (CSLXSIShader* pkRTShader = FindShader(in_Material, "RealTime"))
        {
            if (CSLXSIShader* pkGBRTShader = FindGamebryoRealtimeShader(
                *pkRTShader, "GamebryoMaterial.1"))
            {
                typedef NiTexturingProperty::MapEnum MapEnum;

                struct TextureConversion
                {
                    const char* paramName;
                    MapEnum mapIndex;
                };

                static const TextureConversion textureConversions[] =
                {
                    { "Base",     NiTexturingProperty::BASE_INDEX },
                    { "Dark",     NiTexturingProperty::DARK_INDEX },
                    { "Detail",   NiTexturingProperty::DETAIL_INDEX },
                    { "Normal",   NiTexturingProperty::NORMAL_INDEX },
                    { "Parallax", NiTexturingProperty::PARALLAX_INDEX },
                    { "Bump",     NiTexturingProperty::BUMP_INDEX },
                    { "Gloss",    NiTexturingProperty::GLOSS_INDEX },
                    { "Glow",     NiTexturingProperty::GLOW_INDEX },
                    { "Decal_01", (MapEnum) (NiTexturingProperty::DECAL_BASE + 0) },
                    { "Decal_02", (MapEnum) (NiTexturingProperty::DECAL_BASE + 1) },
                    { "Decal_03", (MapEnum) (NiTexturingProperty::DECAL_BASE + 2) },
                    { 0, NiTexturingProperty::INDEX_MAX }
                };

                for (int i = 0; textureConversions[i].paramName; ++i)
                {
                    const TextureConversion& tex = textureConversions[i];
                    if (!ConvertRealtimeTexture(
                        io_Context, in_Material, *pkGBRTShader, tex.paramName, tex.mapIndex))
                    {
                        return false;
                    }
                }
            }
        }
        else
        {
            if (CSLXSIShader* pkShader = FindShader(in_Material, "surface"))
            {
                io_Context.Logf(
                    LOG_WARNING,
                    "Didn't find a real-time shader in \"%s\" material,"
                    " using surface shader instead.",
                    in_Material.Name().GetText());

                if (!ConvertMentalRayTexture(
                    io_Context,
                    in_Material,
                    *pkShader,
                    "diffuse_inuse",
                    "diffuse",
                    NiTexturingProperty::BASE_INDEX))
                {
                    return false;
                }

                if (!ConvertMentalRayTexture(
                    io_Context,
                    in_Material,
                    *pkShader,
                    "reflect_inuse",
                    "reflectivity",
                    NiTexturingProperty::GLOSS_INDEX))
                {
                    return false;
                }

                const float incandescenceIntensity = GetFloatParameter(*pkShader, "inc_inten");
                if (incandescenceIntensity >= 0.99f)
                {
                    if (!ConvertMentalRayTexture(
                        io_Context,
                        in_Material,
                        *pkShader,
                        "incand_inuse",
                        "incandescence",
                        NiTexturingProperty::GLOW_INDEX))
                    {
                        return false;
                    }
                }
                else if (incandescenceIntensity >= 0.1f)
                {
                    return io_Context.Logf(
                        LOG_ERROR,
                        "Material \"%s\" surface incandescence intensity is not 0 or 1.",
                        in_Material.Name().GetText());
                }
            }

            if (CSLXSIShader* pkNormalShader = FindShader(in_Material, "normal"))
            {
                // Note: meshes look ugly when both a bump-map and normal map are used...
                //       so we choose one based on the presence of an environment map
                //       since Gamebryo bump-map work only with an environment map.
                CSLXSIShader* envShader = FindShader(in_Material, "environment");
                NiTexturingProperty::MapEnum whichMap = envShader
                                                      ? NiTexturingProperty::BUMP_INDEX
                                                      : NiTexturingProperty::NORMAL_INDEX;
                if (!ConvertMentalRayTexture(
                    io_Context,
                    in_Material,
                    *pkNormalShader,
                    "inuse",
                    "input",
                    whichMap))
                {
                    return false;
                }
            }
        }

        return true;
    }

    //---------------------------------------------------------------------------
    // Convert the global texture effect to a Gamebryo texture effect.
    // For example: environment map, projected lights, projected shadows.
    // Return true on success. Doing nothing is considered successful.
    //---------------------------------------------------------------------------
    bool ConvertTextureEffects(
        Context& io_Context,
        CSLXSIMaterial& in_Material)
    {
        if (CSLXSIShader* pkRTShader = FindShader(in_Material, "RealTime"))
        {
            if (CSLXSIShader* pkGBRTShader = FindGamebryoRealtimeShader(
                *pkRTShader, "GamebryoMaterial.1"))
            {
                struct EffectConversion
                {
                    const char* paramName;
                    NiTextureEffect::TextureType textureType;
                };

                static const EffectConversion effectConversions[] =
                {
                    { "Environment",        NiTextureEffect::ENVIRONMENT_MAP  },
                    { "ProjectedLight_01",  NiTextureEffect::PROJECTED_LIGHT  },
                    { "ProjectedLight_02",  NiTextureEffect::PROJECTED_LIGHT  },
                    { "ProjectedLight_03",  NiTextureEffect::PROJECTED_LIGHT  },
                    { "ProjectedShadow_01", NiTextureEffect::PROJECTED_SHADOW },
                    { "ProjectedShadow_02", NiTextureEffect::PROJECTED_SHADOW },
                    { "ProjectedShadow_03", NiTextureEffect::PROJECTED_SHADOW },
                    { 0, NiTextureEffect::TEXTURE_TYPE_MAX }
                };

                for (int i = 0; effectConversions[i].paramName; ++i)
                {
                    const EffectConversion& effect = effectConversions[i];

                    if (!GetBooleanParameter(
                        *pkGBRTShader, RealtimeParamBuilder(effect.paramName, "Enabled")))
                    {
                        continue;
                    }

                    const bool isProjectedLightOrShadow = (i != 0);
                    NiTextureEffect::CoordGenType coordGen = ConvertCoordinateType(
                        io_Context, *pkGBRTShader, effect.paramName, isProjectedLightOrShadow);

                    TextureSourceInfo imageInfo;
                    if (NiTextureEffect::SPECULAR_CUBE_MAP == coordGen
                    ||  NiTextureEffect::DIFFUSE_CUBE_MAP  == coordGen)
                    {
                        imageInfo.m_IsCubeMap = true;
                    }

                    // Note: BASE_INDEX parameter will be ignored, provided for API compatibility.
                    if (!CreateRealtimeTextureSource(
                        io_Context,
                        in_Material,
                        *pkGBRTShader,
                        effect.paramName,
                        NiTexturingProperty::BASE_INDEX,
                        imageInfo))
                    {
                        return false;
                    }

                    // If the coordinate type could not be determined, then use a proper default
                    // based on if the texture was a cube map or not.
                    if (NiTextureEffect::NUM_COORD_GEN == coordGen)
                    {
                        const char* coordGenName;
                        if (isProjectedLightOrShadow)
                        {
                            coordGen     = NiTextureEffect::WORLD_PARALLEL;
                            coordGenName = "world parallel";
                        }
                        else if (imageInfo.m_IsCubeMap)
                        {
                            coordGen     = NiTextureEffect::SPECULAR_CUBE_MAP;
                            coordGenName = "specular cube map";
                        }
                        else
                        {
                            coordGen     = NiTextureEffect::SPHERE_MAP;
                            coordGenName = "sphere map";
                        }

                        io_Context.Logf(
                            epg::LOG_WARNING,
                            "Using %s coordinate type for the %s map"
                            " of \"%s\" based on texture analysis.",
                            coordGenName,
                            effect.paramName,
                            pkGBRTShader->Name().GetText());
                    }

                    AddEffect(io_Context, in_Material, imageInfo, effect.textureType, coordGen);
                }
            }
            else if (CSLXSIShader* pkGBFXShader = FindGamebryoRealtimeShader(
                *pkRTShader, "GamebryoFX.1")) 
            {
               if (!ConvertFXShader(io_Context, in_Material, *pkGBFXShader))
                    return false;
            }
            
            // Convert possible cube map.
            if (!ConvertRealtimeCubeMapEffect(io_Context, in_Material, *pkRTShader))
                return false;
        }
        else if (CSLXSIShader* shader = FindShader(in_Material, "environment"))
        {
            io_Context.Logf(
                LOG_WARNING,
                "Didn't find a real-time shader in \"%s\" material,"
                " using environment mental-ray shader instead.",
                in_Material.Name().GetText());

            TextureSourceInfo imageInfo;
            // Note: BASE_INDEX parameter will be ignored, provided only for API compatibility.
            if (!CreateMentalRayTextureSource(
                io_Context, in_Material, *shader, NiTexturingProperty::BASE_INDEX, imageInfo))
            {
                return false;
            }

            NiTextureEffect::CoordGenType coordGen = imageInfo.m_IsCubeMap
                                                   ? NiTextureEffect::DIFFUSE_CUBE_MAP
                                                   : NiTextureEffect::SPHERE_MAP;
            AddEffect(
                io_Context,
                in_Material,
                imageInfo,
                NiTextureEffect::ENVIRONMENT_MAP,
                coordGen);
        }

        return  true;
    }
    
    //---------------------------------------------------------------------------
    // Lookup a specified named shader.
    // Returns the shader. Returns null if not found.
    //---------------------------------------------------------------------------
    CSLXSIShader* FindShader(CSLXSIMaterial& in_MaterialXSI, const char * in_ParamName)
    {
        CSLConnectionPoint* connPoint = FindConnectionPoint(in_MaterialXSI, in_ParamName);
        if (connPoint)
            return connPoint->GetShader();

        return 0;
    }

    CSLXSIShader* FindShader(CSLXSIShader& in_ShaderXSI, const char * in_ParamName)
    {
        CSLShaderConnectionPoint* connPoint = FindConnectionPoint(in_ShaderXSI, in_ParamName);
        if (connPoint)
            return connPoint->GetShader();

        return 0;
    }

    //---------------------------------------------------------------------------
    // If the input image contains 6 images, then we assume it is a cube map.
    //---------------------------------------------------------------------------
    bool IsCubeMap(const char* in_FileName)
    {
        // Get filename extension.
        char acStandardFilename[NI_MAX_PATH];
        NiStrcpy(acStandardFilename, NI_MAX_PATH, in_FileName);
        NiPath::Standardize(acStandardFilename);
        NiFilename kFileName(acStandardFilename);
        const char* pcExt = kFileName.GetExt();

        // Verify if the extension corresponds to a DDS file.
        NiDDSReader ddsReader;
        if (ddsReader.CanReadFile(pcExt))
        {
            NiFile* pkFile = NiFile::GetFile(
                acStandardFilename,
                NiFile::READ_ONLY);
            unsigned int uiW;
            unsigned int uiH;
            bool bMipMap;
            NiPixelFormat kFormat;
            unsigned int uiFaceCount = 1;
            ddsReader.ReadHeader(*pkFile, uiW, uiH, kFormat,
                bMipMap, uiFaceCount);
            NiDelete pkFile;
            return uiFaceCount == 6;
        }

        return false;
    }

    //---------------------------------------------------------------------------
}
