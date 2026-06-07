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
#include "CrosswalkGamebryoImpl.h"

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
// Real-time shader texture conversion.
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
// Common mappings for parameter "XYZ" are:
//
//   RealTime:XYZ ---------------------> NiTextureProperty::SetXYZMap().
//   RealTime:bXYZMap (boolean) -------> Enable / disable XYZ texture.
//   RealTime:XYZ:tex (image) ---------> NiSourceTexture.
//   RealTime:XYZ:tspace_id (name) ----> NiDataStream.
//   RealTime:XYZ:repeats (vector) ----> used to scale UV coordinates.
//   RealTime:XYZ:filtered (boolean) --> Enable / disable filtering the texture.
//   RealTime:XYZ:bilinear (boolean) --> Enable / disable the use of a bi-linear filter.
//
// Supported "XYZ" are:
//
//   Base
//   Dark
//   Detail
//   Decal_01
//   Decal_02
//   Decal_03
//   Normal
//   Parallax
//   Environment
//   Bump
//   Gloss
//   Glow
//   ProjectedLight_01
//   ProjectedLight_02
//   ProjectedLight_03
//   ProjectedShadow_01
//   ProjectedShadow_02
//   ProjectedShadow_03
//
// The specific mappings for each texture are:
//
//   RealTime:??? (float) -------------> NiTextureProperty::GetBumpMap().SetBumpMap00() and 11().
//
//   RealTime:Environment (shader) ----> NiTextureEffect::SetTextureType(ENVIRONMENT_MAP).
//
//   RealTime:ProjectedLight_01 -------> NiTextureEffect::SetTextureType(PROJECTED_LIGHT).
//
//   RealTime:ProjectedShadow_01 ------> NiTextureEffect::SetTextureType(PROJECTED_SHADOW).
//
//---------------------------------------------------------------------------

namespace
{
    //---------------------------------------------------------------------------
    // Convert shader property to Gamebryo texturing clamp mode.
    //---------------------------------------------------------------------------
    NiTexturingProperty::ClampMode ConvertClampMode(
        CSLXSIShader& in_Shader,
        const char* in_ParamName)
    {
        // Note: for now we assume that XSI will always bake the UV properly
        //       so we always use the wrapping mode.
        return NiTexturingProperty::WRAP_S_WRAP_T;
    }

    //---------------------------------------------------------------------------
    // Convert shader property to Gamebryo texturing filter mode.
    //---------------------------------------------------------------------------
    NiTexturingProperty::FilterMode ConvertFilterMode(
        CSLXSIShader& in_Shader,
        const char* in_ParamName)
    {
        NiTexturingProperty::FilterMode mode;

        const bool withMipMap = epg::GetBooleanParameter(
            in_Shader, epg::RealtimeParamBuilder(in_ParamName, "MipMapEnabled"));

        const int value = epg::GetIntegerParameter(
            in_Shader, epg::RealtimeParamBuilder(in_ParamName, "FilteringMode"));
        switch (value)
        {
            case 0:
                if (withMipMap)
                    mode = NiTexturingProperty::FILTER_NEAREST_MIPNEAREST;
                else
                    mode = NiTexturingProperty::FILTER_NEAREST;
                break;

            case 1:
                if (withMipMap)
                    mode = NiTexturingProperty::FILTER_BILERP_MIPNEAREST;
                else
                    mode = NiTexturingProperty::FILTER_BILERP;
                break;

            default:
            case 2:
                if (withMipMap)
                    mode = NiTexturingProperty::FILTER_BILERP_MIPNEAREST;
                else
                    mode = NiTexturingProperty::FILTER_TRILERP;
                break;
        }

        return mode;
    }

    //---------------------------------------------------------------------------
    // Convert shader properties to a Gamebryo texture transform.
    // Return null when no transform is used.
    //---------------------------------------------------------------------------
    NiTextureTransform* ConvertTextureTransform(
        CSLXSIShader& in_Shader,
        const char* in_ParamName)
    {
        if (!epg::GetBooleanParameter(in_Shader,
            epg::RealtimeParamBuilder(in_ParamName, "Transform")))
        {
            return 0;
        }

        const NiPoint2 translate(
            epg::GetFloatParameter(in_Shader, epg::RealtimeParamBuilder(in_ParamName, "trsx")),
            epg::GetFloatParameter(in_Shader, epg::RealtimeParamBuilder(in_ParamName, "trsy")));

        const float rotate = epg::DegreeToRadian(
            epg::GetFloatParameter(in_Shader, epg::RealtimeParamBuilder(in_ParamName, "rotx")));

        const NiPoint2 scale(
            epg::GetFloatParameter(
                in_Shader, epg::RealtimeParamBuilder(in_ParamName, "sclx"), 1.0f),
            epg::GetFloatParameter(
                in_Shader, epg::RealtimeParamBuilder(in_ParamName, "scly"), 1.0f));

        const NiPoint2 center(0.5f, 0.5f);

        // Note: 3dsMax transform is:
        //          Center * Scale * Rotation * Translate * Inverse(Center)
        NiTextureTransform* pTransform = NiNew NiTextureTransform(
            translate, rotate, scale, center, NiTextureTransform::MAX_TRANSFORM);

        // Optimize case when transform is identity.
        if (NiTextureTransform::IDENTITY == *pTransform)
        {
            NiDelete pTransform;
            return 0;
        }

        return pTransform;
    }

    //---------------------------------------------------------------------------
}

namespace epg
{
    //---------------------------------------------------------------------------
    // Default constructor for texture source information.
    //---------------------------------------------------------------------------
    TextureSourceInfo::TextureSourceInfo()
        : m_ClampMode(NiTexturingProperty::WRAP_S_WRAP_T)
        , m_FilterMode(NiTexturingProperty::FILTER_BILERP)
        , m_Image()
        , m_IsCubeMap(false)
    {
    }

    //---------------------------------------------------------------------------
    // Lookup the file name of a texture.
    //---------------------------------------------------------------------------
    const NiFixedString* FindTextureFilename(
        epg::Context& io_Context,
        CSLXSIShader& in_Shader,
        const char* in_FileNameParamName)
    {
        CSLShaderConnectionPoint* texConn = epg::FindConnectionPoint(
            in_Shader, in_FileNameParamName);
        if (!texConn)
            return NULL;
        SI_Char* imageName = texConn->GetImage();
        return io_Context.GetTextureMap().FindTextureFilename(imageName);
    }

    //---------------------------------------------------------------------------
    // Remove the folder from a full file name.
    //---------------------------------------------------------------------------
    NiFixedString RemoveFolderName(const NiFixedString& in_FullFileName)
    {
        NiString fileDir, fileFile, fileExt;
        NiPluginHelpers::ParseFullPath(in_FullFileName, fileDir, fileFile, fileExt);
        return NiFixedString(fileFile + fileExt);
    }

    //---------------------------------------------------------------------------
    // If the input image is grayscale then set the pixel layout to BUMPMAP
    // so that Gamebryo bakes it into a proper bump-map texture.
    //---------------------------------------------------------------------------
    void DetectGrayscaleBumpMap(
        Context& io_Context,
        const NiFixedString& in_FileName,
        NiTexturingProperty::MapEnum in_MapIndex,
        NiTexture::FormatPrefs& io_Prefs)
    {
        const bool isSpecial = (in_MapIndex == NiTexturingProperty::BUMP_INDEX ||
                                in_MapIndex == NiTexturingProperty::NORMAL_INDEX);
        if (!isSpecial)
            return;

        NiTexture::FormatPrefs prefs;
        prefs.m_eMipMapped = NiTexture::FormatPrefs::NO;
        prefs.m_eAlphaFmt = NiTexture::FormatPrefs::NONE;
        prefs.m_ePixelLayout = NiTexture::FormatPrefs::TRUE_COLOR_32;
        NiSourceTexturePtr image = NiSourceTexture::Create(in_FileName, prefs);
        if (!image)
        {
            io_Context.Logf(
                LOG_WARNING,
                "Cannot open bump image file \"%s\".",
                (const char*) in_FileName);
            return;
        }
        image->LoadPixelDataFromFile();

        NiPixelData* data = image->GetSourcePixelData();
        if (!data)
        {
            io_Context.Logf(
                LOG_WARNING,
                "Cannot get bump image \"%s\" pixel data.",
                (const char*) in_FileName);
            return;
        }

        if (!data->GetPixels())
        {
            io_Context.Logf(
                LOG_WARNING,
                "Cannot get bump image \"%s\" pixels.",
                (const char*) in_FileName);
            return;
        }

        if (data->GetPixelFormat() != NiPixelFormat::RGB24
        &&  data->GetPixelFormat() != NiPixelFormat::RGBA32)
        {
            io_Context.Logf(
                LOG_WARNING,
                "Bump image \"%s\" is not RGB24 nor RGBA32.",
                (const char*) in_FileName);
            return;
        }

        for (unsigned int x = 0; x < data->GetWidth(); ++x)
        {
            for (unsigned int y = 0; y < data->GetHeight(); ++y)
            {
                const unsigned char* pixel = (*data)(x, y);
                if (abs((int)pixel[0] - (int)pixel[1]) > 10)
                    return;
                if (abs((int)pixel[1] - (int)pixel[2]) > 10)
                    return;
            }
        }

        io_Prefs.m_ePixelLayout = NiTexture::FormatPrefs::BUMPMAP;
    }

    //---------------------------------------------------------------------------
    // Verify the validity of a texture source.
    //---------------------------------------------------------------------------
    bool VerifyTextureSource(
        Context& io_Context,
        CSLXSIMaterial& in_Material,
        NiSourceTexture& in_SourceTexture,
        const char* in_ParamName,
        const char* in_FileName)
    {
        // Load the image to verify the compatibility of its file format.
        in_SourceTexture.LoadPixelDataFromFile();

        const char* causeOfError = "unknwon cause";
        bool invalidFormat       = false;
        
        if ( !in_SourceTexture.GetSourcePixelData() )
        {
            causeOfError  = "no source pixel data";
            invalidFormat = true;
        }
        else if ( !in_SourceTexture.GetSourcePixelData()->GetPixels() )
        {
            causeOfError  = "pixel data with NULL pixels";
            invalidFormat = true;
        }
        else if ( !in_SourceTexture.GetSourcePixelData()->GetWidth() )
        {
            causeOfError  = "pixel data with no width";
            invalidFormat = true;
        }
        else if ( !in_SourceTexture.GetSourcePixelData()->GetHeight() )
        {
            causeOfError  = "pixel data with no height";
            invalidFormat = true;
        }

        if (invalidFormat)
        {
            return io_Context.Logf(
                LOG_ERROR,
                "Image file \"%s\" for the \"%s\" texture of material"
                " \"%s\" use an unsupported file format (diagnosis symptom: %s).",
                in_FileName,
                in_ParamName,
                in_Material.Name().GetText(),
                causeOfError);
        }

        // Loading the image automatically makes the NIF exporter embed it.
        // So if the settings specify not to embed them, unload the image.
        if (!io_Context.GetSettings().GetEmbedTextures())
            in_SourceTexture.DestroyAppPixelData();

        return true;
    }

    //---------------------------------------------------------------------------
    // Create the real-time shader texture source.
    //---------------------------------------------------------------------------
    bool CreateRealtimeTextureSource(
        Context& io_Context,
        CSLXSIMaterial& in_Material,
        CSLXSIShader& in_Shader,
        const char* in_ParamName,
        NiTexturingProperty::MapEnum in_MapIndex,
        TextureSourceInfo& out_ImageInfo)
    {
        // Find the source texture file name. Empty means the texture is disabled.

        const NiFixedString* fileName = FindTextureFilename(
            io_Context, in_Shader, RealtimeParamBuilder(in_ParamName));
        if (!fileName || !fileName->GetLength())
        {
            return io_Context.Logf(
                LOG_ERROR,
                "Cannot find the image for the \"%s\" texture of material \"%s\".",
                in_ParamName,
                in_Material.Name().GetText());
        }

        // Figure out the texturing mode, which influence the source texture settings.
        out_ImageInfo.m_ClampMode  = ConvertClampMode(in_Shader, in_ParamName);
        out_ImageInfo.m_FilterMode = ConvertFilterMode(in_Shader, in_ParamName);

        // Create the source texture.
        NiTexture::FormatPrefs prefs;
        DetectGrayscaleBumpMap(io_Context, *fileName, in_MapIndex, prefs);
        if (out_ImageInfo.m_FilterMode >= NiTexturingProperty::FILTER_NEAREST_MIPNEAREST)
            prefs.m_eMipMapped = NiTexture::FormatPrefs::YES;

        // Note: if the caller claims this is a cube map, believe it.
        //       Otherwise, try to detect it.
        if (!out_ImageInfo.m_IsCubeMap)
            out_ImageInfo.m_IsCubeMap = IsCubeMap(*fileName);

        if (out_ImageInfo.m_IsCubeMap)
            out_ImageInfo.m_Image = NiSourceCubeMap::Create(*fileName, NULL, prefs);
        else
            out_ImageInfo.m_Image = NiSourceTexture::Create(*fileName, prefs);

        if (!VerifyTextureSource(
            io_Context,
            in_Material,
            *out_ImageInfo.m_Image,
            in_ParamName,
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
    // Real-time shader parameter builder.
    //---------------------------------------------------------------------------
    RealtimeParamBuilder::RealtimeParamBuilder(
        const char* in_ParamBase,
        const char* in_ParamSuffix)
    {
        static const int lastIndex = sizeof(m_Buffer) - 1;
        strncpy(m_Buffer, in_ParamBase, lastIndex);
        if (in_ParamSuffix && in_ParamSuffix[0])
        {
            strncat(m_Buffer, "_", lastIndex);
            strncat(m_Buffer, in_ParamSuffix, lastIndex);
        }
        // Prefer to be paranoid... some strnXYZ functions don't put final
        // nul if the buffer would overflow...
        m_Buffer[lastIndex] = 0;
    }

    RealtimeParamBuilder::operator const char*() const
    {
        return m_Buffer;
    }

    //---------------------------------------------------------------------------
    // Real-time shader texture conversion.
    //---------------------------------------------------------------------------
    bool ConvertRealtimeTexture(
        Context& io_Context,
        CSLXSIMaterial& in_Material,
        CSLXSIShader& in_Shader,
        const char* in_ParamName,
        NiTexturingProperty::MapEnum in_MapIndex)
    {
        // Verify if the texture is enabled in XSI.
        if (!GetBooleanParameter(in_Shader, RealtimeParamBuilder(in_ParamName, "Enabled")))
            return true;

        TextureSourceInfo imageInfo;
        if (!CreateRealtimeTextureSource(
            io_Context,
            in_Material,
            in_Shader,
            in_ParamName,
            in_MapIndex,
            imageInfo))
        {
            return false;
        }

        // Find the name of the texture coordinate in XSI .
        const char* textureSpaceName = GetStringParameter(
            in_Shader, RealtimeParamBuilder(in_ParamName, "TexSpace"));
        if (!textureSpaceName || !textureSpaceName[0])
        {
            io_Context.Logf(
                LOG_WARNING,
                "\"%s\" texture of material \"%s\" lacks an explicit"
                " texture-coordinate space (UV), will use the default"
                " \"Texture_Projection\" instead.",
                in_ParamName,
                in_Material.Name().GetText());

            textureSpaceName = "*";
        }

        TextureInfo& texInfo = io_Context.GetTextureMap().AddTextureInfo(in_Material);

        TextureInfo::RepeatUV repeatUV(1.0f, 1.0f);

        // Reserve a stream index in Gamebryo.
        const int texCoordIndex = texInfo.AddStreamIndex(textureSpaceName, repeatUV);

        // Create the Gamebryo texture map.
        // Note: we create a map of a specific class type since
        // bump map and parallax map have special classes.
        NiTexturingProperty& texProp = texInfo.GetProperty();
        NiTextureTransform* pkTransform = ConvertTextureTransform(in_Shader, in_ParamName);
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
                if (FindFloatParameter(
                    in_Shader, RealtimeParamBuilder(in_ParamName, "Scale"), scale))
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

        switch (GetIntegerParameter(in_Shader, RealtimeParamBuilder(in_ParamName, "ApplyMode")))
        {
            default:
            case 0:
                texProp.SetApplyMode(NiTexturingProperty::APPLY_REPLACE);
                break;
            case 1:
                texProp.SetApplyMode(NiTexturingProperty::APPLY_DECAL);
                break;
            case 2:
                texProp.SetApplyMode(NiTexturingProperty::APPLY_MODULATE);
                break;
        }

        return true;
    }

    //---------------------------------------------------------------------------
    // Same as above but for textures used in FX shaders.
    //---------------------------------------------------------------------------
    bool ConvertRealtimeTexture_FX(
        Context& io_Context,
        CSLXSIMaterial& in_Material,
        CSLXSIShader& in_Shader,
        const char* in_ParamName,
        NiTexturingProperty::MapEnum in_MapIndex,
        int in_TextureIndex,
        int in_ShaderMapIndex,
        bool in_IsCubeMap)
    {        
        TextureSourceInfo imageInfo;
        imageInfo.m_IsCubeMap = in_IsCubeMap;
        if (!CreateRealtimeTextureSource(
            io_Context, in_Material, in_Shader, in_ParamName, in_MapIndex, imageInfo))
        {
            return false;
        }

        // Recover texure param name. 
        // Ex: "tspace_id_0","STRING","Texture_Projection1",
        char paramName[64];
        sprintf(paramName, "tspace_id_%d", in_TextureIndex);        

        // Find the name of the texture coordinate in XSI.
        const char* textureSpaceName = GetStringParameter(in_Shader, paramName);
        if (!textureSpaceName || !textureSpaceName[0])
        {
            io_Context.Logf(
                LOG_WARNING,
                "\"%s\" texture of material \"%s\" lacks an explicit"
                " texture-coordinate space (UV), will use the default"
                " \"Texture_Projection\" instead.",
                in_ParamName,
                in_Material.Name().GetText());

            textureSpaceName = "*";
        }

        // Reserve a stream index in Gamebryo.
        TextureInfo&            texInfo = io_Context.GetTextureMap().AddTextureInfo(in_Material);
        TextureInfo::RepeatUV   repeatUV(1.0f, 1.0f);        
        const int texCoordIndex = texInfo.AddStreamIndex(textureSpaceName, repeatUV);

        // Create the Gamebryo texture map.
        // Note: we create a map of a specific class type
        // since bump map and parallax map have special classes.
        NiTexturingProperty& texProp = texInfo.GetProperty();
        NiTextureTransform* pkTransform = ConvertTextureTransform(in_Shader, in_ParamName);
        
        switch (in_MapIndex)
        {
            case NiTexturingProperty::SHADER_BASE:
            {
                NiTexturingProperty::ShaderMap* pkMap = NiNew NiTexturingProperty::ShaderMap( 
                            imageInfo.m_Image,
                            texCoordIndex,
                            imageInfo.m_ClampMode,
                            imageInfo.m_FilterMode,
                            in_ShaderMapIndex);
    
                // Note: the NiTexturingProperty owns the map given to it.
                texProp.SetShaderMap(in_ShaderMapIndex, pkMap);
                break;
            }
            default:
            {
                NiTexturingProperty::Map* pkMap = NiNew NiTexturingProperty::Map(
                    imageInfo.m_Image,
                    texCoordIndex,
                    imageInfo.m_ClampMode,
                    imageInfo.m_FilterMode,
                    pkTransform);
                // Note: the NiTexturingProperty owns the map given to it.
                texProp.SetMap(in_MapIndex, pkMap);
                break;
            }
        }

        return true;
    }

    //---------------------------------------------------------------------------
}
