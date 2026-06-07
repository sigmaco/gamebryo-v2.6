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

#ifndef XSI_GAMEBRYO_CONVERTER_TEX_HELPERS_H
#define XSI_GAMEBRYO_CONVERTER_TEX_HELPERS_H

#include "SLTypes.h"

#include "NiMainLibType.h"
#include "NiTexturingProperty.h"

//---------------------------------------------------------------------------
// External references.
//---------------------------------------------------------------------------

class XSIEXPORT CSLXSIMaterial;
class XSIEXPORT CSLXSIShader;

class NIMAIN_ENTRY NiMaterialProperty;
class NIMAIN_ENTRY NiSourceTexture;
template <class T> class NiPointer;
typedef NiPointer<NiSourceTexture> NiSourceTexturePtr;

namespace epg
{
    class Context;

    //---------------------------------------------------------------------------
    // @name Texture conversion helpers.
    // @{
    //---------------------------------------------------------------------------

    /** Convert the surface textures to a Gamebryo texturing property.
        @return true on success. Doing nothing is considered successful.
    */
    bool ConvertSurfaceTextures(
        Context& io_Context,
        CSLXSIMaterial& in_Material);

    /** Convert the global texture effect to a Gamebryo texture effect.
        For example: environment map, projected lights, projected shadows.
        @return true on success. Doing nothing is considered successful.
    */
    bool ConvertTextureEffects(
        Context& io_Context,
        CSLXSIMaterial& in_Material);

    /*! Convert FX and CgFX programs.
        \return true on success. Doing nothing is considered successful.
    */
    bool ConvertFXShader(
        Context& io_Context,
        CSLXSIMaterial& in_Material,
        CSLXSIShader& in_GamebryoFXShader);

    /*! Lookup a specified named shader.
        \return the shader. Returns null if not found.
    */
    CSLXSIShader* FindShader(
        CSLXSIMaterial& in_MaterialXSI,
        const char * in_ParamName);

    /** Lookup a specified named shader.
        @return the shader. Returns null if not found.
    */
    CSLXSIShader* FindShader(
        CSLXSIShader& in_ShaderXSI,
        const char * in_ParamName);
    

    //---------------------------------------------------------------------------
    // @name Texture image source helpers.
    // @{
    //---------------------------------------------------------------------------

    struct TextureSourceInfo
    {
        TextureSourceInfo();

        NiTexturingProperty::ClampMode m_ClampMode;
        NiTexturingProperty::FilterMode m_FilterMode;
        NiSourceTexturePtr m_Image;
        bool m_IsCubeMap;
    };

    /** Convert a real-time shader texture source corresponding to the specified texture map.
    */
    bool CreateRealtimeTextureSource(
        Context& io_Context,
        CSLXSIMaterial& in_Material,
        CSLXSIShader& in_Shader,
        const char* in_ParamName,
        NiTexturingProperty::MapEnum in_MapIndex,
        TextureSourceInfo& out_ImageInfo);

    /** Convert a mental-ray shader texture source corresponding to the specified texture map.
    */
    bool CreateMentalRayTextureSource(
        Context& io_Context,
        CSLXSIMaterial& in_Material,
        CSLXSIShader& in_Shader,
        NiTexturingProperty::MapEnum in_MapIndex,
        TextureSourceInfo& out_ImageInfo);

    /** Verify the validity of a texture source.
    */
    bool VerifyTextureSource(
        Context& io_Context,
        CSLXSIMaterial& in_Material,
        NiSourceTexture& in_SourceTexture,
        const char* in_ParamName,
        const char* in_FileName);

    /** Lookup the file name of a texture.
    */
    const NiFixedString* FindTextureFilename(
        epg::Context& io_Context,
        CSLXSIShader& in_Shader,
        const char* in_FileNameParamName);

    /** If the input image is grayscale then set the pixel layout to BUMPMAP
        so that Gamebryo bakes it into a proper bump-map texture.
    */
    void DetectGrayscaleBumpMap(
        Context& io_Context,
        const NiFixedString& in_FileName,
        NiTexturingProperty::MapEnum in_MapIndex,
        NiTexture::FormatPrefs& io_Prefs);

    /** If the input image contains 6 images, then we assume it is a cube map.
        @param in_FileName the file to inspect.
        @return true if the file contains a cube map.
    */
    bool IsCubeMap(const char* in_FileName);

    /** Remove the folder from a full file name.
    */
    NiFixedString RemoveFolderName(const NiFixedString& in_FullFileName);
    // @}

    //---------------------------------------------------------------------------
    // @name Shader texture conversion helpers.
    // @{
    //---------------------------------------------------------------------------

    /** Convert a real-time shader texture corresponding to the specified Gamebryo texture map.
    */
    bool ConvertRealtimeTexture(
        Context& io_Context,
        CSLXSIMaterial& in_Material,
        CSLXSIShader& in_Shader,
        const char* in_ParamName,
        NiTexturingProperty::MapEnum in_MapIndex);

    /** Same as above but for textures used in FX shaders.
    */
    bool ConvertRealtimeTexture_FX(
        Context& io_Context,
        CSLXSIMaterial& in_Material,
        CSLXSIShader& in_Shader,
        const char* in_ParamName,
        NiTexturingProperty::MapEnum in_MapIndex,
        int in_TextureIndex,
        int in_ShaderMapIndex,
        bool in_IsCubeMap);

    /** Convert a mental-ray shader texture corresponding to the specified Gamebryo texture map.
    */
    bool ConvertMentalRayTexture(
        Context& io_Context,
        CSLXSIMaterial& in_Material,
        CSLXSIShader& in_Shader,
        const char* in_InUseParamName,
        const char* in_TextureParamName,
        NiTexturingProperty::MapEnum in_MapIndex);
    // @}

    //---------------------------------------------------------------------------
    /// Real-time parameter builder.
    //---------------------------------------------------------------------------

    class RealtimeParamBuilder
    {
    public:
        RealtimeParamBuilder(const char* in_ParamBase, const char* in_ParamSuffix = 0);
        operator const char*() const;

    private:
        char m_Buffer[256];
    };
}

#endif /* XSI_GAMEBRYO_CONVERTER_TEX_HELPERS_H */
