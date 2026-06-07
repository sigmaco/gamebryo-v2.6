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
#include "CrosswalkGamebryoTexMap.h"
#include "CrosswalkGamebryoHelpers.h"
#include "CrosswalkGamebryoSettings.h"

#include "XSIMaterial.h"
#include "XSIShader.h"
#include "AnimatableType.h"
#include "ConnectionPoint.h"
#include "ShaderConnectionPoint.h"

#include "NiTexturingProperty.h"
#include "NiSourceTexture.h"
#include "NiSourceCubeMap.h"
#include "NiPluginHelpers.h"

//---------------------------------------------------------------------------
// Mental-ray surface shader texture conversion.
//
// Here's the mapping between XSI materials and Gamebryo texture property.
// The mappings use the following conventions:
//
//   dotXSI material or shader entry -> Gamebryo function or class.
//
//   (color) means that the dotXSI entry is a color, usually represented
//            as three entries in the form name.red, name.green, name.blue.
//
//   (shader) means the the dotXSI entry is a connection to an image,
//            thus a texture.
//
//   (image) gives the SI image name of the texture image.
//
//   (boolean) used to enable or disable a feature in dotXSI.
//
//   (name) string used to identify the texture coordinate set.
//
//   (vector) a three-value entry, in the form name.x, name.y and name.z.
//
//   (float) a real number also known as a floating-point value.
//
// Common mappings for parameter "abc" are:
//
//   abc:tex (image) ---------> NiSourceTexture.
//   abc:tspace_id (name) ----> NiTexturingProperty::Map::SetTextureIndex() -> NiDataStream.
//   abc:repeats (vector) ----> The repeat factor of the texture, used to scale UV coordinates.
//   abc:filtered (boolean) --> Enable / disable filtering the texture.
//   abc:bilinear (boolean) --> Enable / disable the use of a bi-linear filter. 
//
// The specific mappings for each texture are:
//
//   surface:diffuse (shader) ---------> NiTextureProperty::SetBaseMap().
//   surface:diffuse_inuse (boolean) --> Enable / disable diffuse texture.
//
//   normal:input (shader) ------------> NiTextureProperty::SetBumpMap().
//   normal:scale (float) -------------> NiTextureProperty::GetBumpMap().SetBumpMap00() and 11().
//   normal:inuse (boolean) -----------> Enable / disable bump mapping texture.
//
//   unsupported ----------------------> NiTextureProperty::SetDarkMap().
//
//   unsupported ----------------------> NiTextureProperty::SetDecalMap().
//
//   unsupported ----------------------> NiTextureProperty::SetDetailMap().
//
//   environment (shader)--------------> NiTextureEffect::SetTextureType(ENVIRONMENT_MAP).
//
//   surface:reflectivity (shader) ----> NiTextureProperty::SetGlossMap().
//   surface:reflect_inuse (boolean) --> Enable / disable gloss texture.
//
//   surface:incandescence (shader) ---> NiTextureProperty::SetGlowMap().
//   surface:incand_inuse (boolean) ---> Enable / disable glow texture.
//   surface:inc_inten (float) --------> must be 0 or 1, warn otherwise.
//
//   normal:input (shader) ------------> NiTextureProperty::SetNormalMap().
//   normal:inuse (boolean) -----------> Enable / disable normal mapping texture.
//
//   unsupported ----------------------> NiTextureProperty::SetParallaxMap().
//
//   unsupported ----------------------> NiTextureEffect::SetTextureType(PROJECTED_LIGHT).
//
//   unsupported ----------------------> NiTextureEffect::SetTextureType(PROJECTED_SHADOW).
//
// Note: some Gamebryo element can be taken from more than one type
//       of XSI sources. For example the bump mapping can be setup
//       in various ways in XSI. We support the most commons.
//
// Note: the XSI normal shader is used either for bump map or normal map
//       depending on the presence of an environment map.
//
// Note: the alpha channel of the bum map is used for glossiness and
//       using a bump map prevent using a gloss map. (Gamebryo limitation.)
//
//---------------------------------------------------------------------------

namespace
{
    //---------------------------------------------------------------------------
    // Convert shader property to Gamebryo texturing clamp mode.
    //---------------------------------------------------------------------------
    NiTexturingProperty::ClampMode ConvertClampMode(CSLXSIShader& in_Shader)
    {
        // So far, we've never seen any mental-ray shader that needed clamp,
        // but we've seen shaders that needed wrap and didn't set torus to true,
        // so force wrap mode all the time.
        return NiTexturingProperty::WRAP_S_WRAP_T;
    }

    //---------------------------------------------------------------------------
    // Convert shader property to Gamebryo texturing filter mode.
    //---------------------------------------------------------------------------
    NiTexturingProperty::FilterMode ConvertFilterMode(CSLXSIShader& in_Shader)
    {
        NiTexturingProperty::FilterMode mode = NiTexturingProperty::FILTER_NEAREST;

        const bool defaultValue = false;
        if (epg::GetBooleanParameter(in_Shader, "filtered", defaultValue))
            if (epg::GetBooleanParameter(in_Shader, "bilinear", defaultValue))
                mode = NiTexturingProperty::FILTER_TRILERP;

        return mode;
    }

    //---------------------------------------------------------------------------
}

namespace epg
{
    //---------------------------------------------------------------------------
    // Create the mental-ray shader texture source.
    //---------------------------------------------------------------------------
    bool CreateMentalRayTextureSource(
        Context& io_Context,
        CSLXSIMaterial& in_Material,
        CSLXSIShader& in_Shader,
        NiTexturingProperty::MapEnum in_MapIndex,
        TextureSourceInfo& out_ImageInfo)
    {
        // Find the source texture file name. Empty means the texture is disabled.
        const NiFixedString* fileName = FindTextureFilename(io_Context, in_Shader, "tex");
        if (!fileName || !fileName->GetLength())
        {
            return io_Context.Logf(
                LOG_ERROR,
                "Cannot find the image for the \"%s\" texture of material \"%s\".",
                in_Shader.Name().GetText(),
                in_Material.Name().GetText());
        }

        // Figure out the texturing mode, which influence the source texture settings.
        out_ImageInfo.m_ClampMode  = ConvertClampMode(in_Shader);
        out_ImageInfo.m_FilterMode = ConvertFilterMode(in_Shader);

        // Create the source texture.
        NiTexture::FormatPrefs prefs;
        DetectGrayscaleBumpMap(io_Context, *fileName, in_MapIndex, prefs);
        if (out_ImageInfo.m_FilterMode >= NiTexturingProperty::FILTER_NEAREST_MIPNEAREST)
            prefs.m_eMipMapped = NiTexture::FormatPrefs::YES;

        // Detect cube maps.
        out_ImageInfo.m_IsCubeMap = IsCubeMap(*fileName);

        if (out_ImageInfo.m_IsCubeMap)
            out_ImageInfo.m_Image = NiSourceCubeMap::Create(*fileName, NULL, prefs);
        else
            out_ImageInfo.m_Image = NiSourceTexture::Create(*fileName, prefs);

        if (!VerifyTextureSource(
            io_Context,
            in_Material,
            *out_ImageInfo.m_Image,
            in_Shader.Name().GetText(),
            *fileName))
        {
            return false;
        }

        // Only keep the filename, not the file path, like other conversion plugins.
        //
        // Do this *after* checking validity because when in the plugin we don't
        // necessarily have the image setup like in the Gamebryo engine and we
        // need to find the file to verify it. Once verified, we must only keep
        // the filename and let the Gamebryo engine find the image if it can.
        out_ImageInfo.m_Image->SetFilename(RemoveFolderName(*fileName));

        return true;
    }

    //---------------------------------------------------------------------------
    // Mental-ray shader texture conversion.
    //---------------------------------------------------------------------------
    bool ConvertMentalRayTexture(
        Context& io_Context,
        CSLXSIMaterial& in_Material,
        CSLXSIShader& in_Shader,
        const char* in_InUseParamName,
        const char* in_TextureParamName,
        NiTexturingProperty::MapEnum in_MapIndex)
    {
        // Verify if the texture is enabled in XSI.
        if (!GetBooleanParameter(in_Shader, in_InUseParamName, true))
            return true;

        // Find the texture shader in XSI.
        CSLShaderConnectionPoint* connPoint = FindConnectionPoint(in_Shader, in_TextureParamName);
        if (!connPoint)
            return true;
            
        CSLXSIShader* diffuseTex = connPoint->GetShader();
        if (!diffuseTex)
            return true;

        TextureSourceInfo imageInfo;
        if (!CreateMentalRayTextureSource(
            io_Context,
            in_Material,
            *diffuseTex,
            in_MapIndex,
            imageInfo))
        {
            return false;
        }

        // Find the name of the texture coordinate in XSI .
        const char* textureSpaceName = GetStringParameter(*diffuseTex, "tspace_id");
        if (!textureSpaceName || !textureSpaceName[0])
        {
            io_Context.Logf(
                LOG_WARNING,
                "\"%s\" texture of material \"%s\" lacks an explicit"
                " texture-coordinate space (UV), will use the default"
                " \"Texture_Projection\" instead.",
                in_TextureParamName,
                in_Material.Name().GetText());

            textureSpaceName = "Texture_Projection";
            // Note: when we don't have a UV-space, we assume that the filtering is wrong, too.
            imageInfo.m_FilterMode = NiTexturingProperty::FILTER_TRILERP;
        }

        TextureInfo& texInfo = io_Context.GetTextureMap().AddTextureInfo(in_Material);

        // Find the repeat values that will be used to scale the texture coordinates.
        const float factor = epg::GetFloatParameter(in_Shader, "factor", 1.0f);
        TextureInfo::RepeatUV repeatUV(
            GetFloatSubParameter(*diffuseTex, "repeats", ".x", 1.0f) / factor,
            GetFloatSubParameter(*diffuseTex, "repeats", ".y", 1.0f) / factor);

        // Reserve a stream index in Gamebryo.
        const int texCoordIndex = texInfo.AddStreamIndex(textureSpaceName, repeatUV);

        // Create the Gamebryo texture map.
        // Note: we create a map of a specific class type since
        //       bump map and parallax map have special classes.
        NiTexturingProperty& texProp = texInfo.GetProperty();
        // Note: No texture transform, but mental-ray textures
        //       are destined to be unsupported anyway.
        NiTextureTransform* pkTransform = 0;
        NiTexturingProperty::Map* pkMap;
        switch (in_MapIndex)
        {
            default:
                pkMap = NiNew NiTexturingProperty::Map(
                    imageInfo.m_Image,
                    texCoordIndex,
                    imageInfo.m_ClampMode,
                    imageInfo.m_FilterMode,
                    pkTransform);
                break;
            case NiTexturingProperty::BUMP_INDEX:
            {
                NiTexturingProperty::BumpMap* pkBumpMap = NiNew NiTexturingProperty::BumpMap(
                    imageInfo.m_Image,
                    texCoordIndex,
                    imageInfo.m_ClampMode,
                    imageInfo.m_FilterMode);
                float scale = 0.0f;
                if (FindFloatParameter(in_Shader, "scale", scale))
                {
                    pkBumpMap->SetBumpMat00(scale);
                    pkBumpMap->SetBumpMat11(scale);
                }
                pkBumpMap->SetLumaScale(1.0f);
                pkMap = pkBumpMap;
                pkMap->SetTextureTransform(pkTransform);
                break;
            }
            case NiTexturingProperty::PARALLAX_INDEX:
                pkMap = NiNew NiTexturingProperty::ParallaxMap(
                    imageInfo.m_Image,
                    texCoordIndex,
                    imageInfo.m_ClampMode,
                    imageInfo.m_FilterMode);
                pkMap->SetTextureTransform(pkTransform);
                break;
        }
        
        // Note: the NiTexturingProperty owns the map given to it.
        texProp.SetMap(in_MapIndex,pkMap);

        // Set the apply mode depending on the map type.
        //
        // Note: hard-coded, but mental-ray textures are detsined to be unsupported anyway.
        switch (in_MapIndex)
        {
            case NiTexturingProperty::GLOSS_INDEX:
            case NiTexturingProperty::GLOW_INDEX:
            case NiTexturingProperty::BUMP_INDEX:
            case NiTexturingProperty::NORMAL_INDEX:
                texProp.SetApplyMode(NiTexturingProperty::APPLY_MODULATE);
                break;
            default:
                texProp.SetApplyMode(NiTexturingProperty::APPLY_REPLACE);
                break;
        }

        return true;
    }

    //---------------------------------------------------------------------------
}
