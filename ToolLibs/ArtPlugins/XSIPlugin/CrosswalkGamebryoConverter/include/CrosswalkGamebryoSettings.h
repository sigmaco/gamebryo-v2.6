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

#ifndef XSI_GAMEBRYO_CONVERTER_SETTINGS_H
#define XSI_GAMEBRYO_CONVERTER_SETTINGS_H

#include <string>

namespace epg
{
    //---------------------------------------------------------------------------
    /// TextureCompression setting.
    //---------------------------------------------------------------------------
    enum TextureCompression
    {
        TEXTURE_COMPRESSION_NONE
    };

    //---------------------------------------------------------------------------
    /// Texture format setting.
    //---------------------------------------------------------------------------
    enum TextureFormat
    {
        TEXTURE_FORMAT_UNKNOWN,
        TEXTURE_FORMAT_BMP,
        TEXTURE_FOTMAT_DDS,
        TEXTURE_FOTMAT_HDR,
        TEXTURE_FOTMAT_PNG,
        TEXTURE_FOTMAT_SGI,
        TEXTURE_FOTMAT_TGA
    };

    //---------------------------------------------------------------------------
    /// Target platform for conversion.
    //---------------------------------------------------------------------------
    enum TargetPlatform
    {
        UNKNOWN_PLATFORM        = -1,
        GENERIC_PLATFORM        =  0,
        DIRECTX_9_PLATFORM,
        DIRECTX_10_PLATFORM,
        XBOX_360_PLATFORM,
        PS3_PLATFORM,
        WII_PLATFORM,
    };

    //---------------------------------------------------------------------------
    /// Settings controlling how the conversion will be executed.
    //---------------------------------------------------------------------------
    class ConverterSettings
    {
    public:
        ConverterSettings();

        /// Pre-conversion and post-conversion settings.
        void SetGamebryoProcessingScript(const char * in_FileName);
        void SetGamebryoExportingScript(const char * in_FileName);
        void SetGamebryoViewingScript(const char * in_FileName);
        void SetGamebryoProcessingScriptEnc(const char * in_Enc);
        void SetGamebryoExportingScriptEnc(const char * in_Enc);
        void SetGamebryoViewingScriptEnc(const char * in_Enc);
        void SetLaunchGamebryoAssetViewer(bool in_Launch);
        void SetLogFileName(const char * in_FileName);
        void SetTargetPlatform(TargetPlatform in_TargetPlatform);
        void SetMeshProfile(const char * in_MeshProfileName);

        /// Conversion settings.
        void SetExportCamera(bool in_Export);
        void SetExportLights(bool in_Export);
        void SetExportTextures(bool in_Export);
        void SetExportShapeAnimations(bool in_Export);
        void SetExportSkinnings(bool in_Export);
        void SetEmbedTextures(bool in_Embed);
        void SetTextureFormat(TextureFormat in_Format);
        void SetTextureCompression(TextureCompression in_Compression);
        void SetKeyframeSamplingRate(float in_Rate);
        void SetIgnoreAllErrors(bool in_IgnoreErrors);

        /// Pre-conversion and post-conversion settings.
        const char*        GetGamebryoProcessingScript() const;
        const char*        GetGamebryoExportingScript() const;
        const char*        GetGamebryoViewingScript() const;
        const char*        GetGamebryoProcessingScriptEnc() const;
        const char*        GetGamebryoExportingScriptEnc() const;
        const char*        GetGamebryoViewingScriptEnc() const;
        const char*        GetMeshProfile() const;
        bool               GetLaunchGamebryoAssetViewer() const;
        const char*        GetLogFileName() const;
        TargetPlatform     GetTargetPlatform() const;

        /// Conversion settings.
        bool               GetExportCamera() const;
        bool               GetExportLights() const;
        bool               GetExportTextures() const;
        bool               GetExportShapeAnimations() const;
        bool               GetExportSkinnings() const;
        bool               GetEmbedTextures() const;
        TextureFormat      GetTextureFormat() const;
        TextureCompression GetTextureCompression() const;
        float              GetKeyframeSamplingRate() const;
        bool               GetIgnoreAllErrors() const;

    private:
        std::string         m_GamebryoProcessingScript;
        std::string         m_GamebryoExportingScript;
        std::string         m_GamebryoViewingScript;
        std::string         m_GamebryoProcessingScriptEnc;
        std::string         m_GamebryoExportingScriptEnc;
        std::string         m_GamebryoViewingScriptEnc;
        std::string         m_LogFileName;
        std::string         m_MeshProfile;

        bool                m_LaunchGamebryoAssetViewer;
        TargetPlatform      m_TargetPlatform;

        bool                m_ExportCamera;
        bool                m_ExportLights;
        bool                m_ExportTextures;
        bool                m_ExportShapeAnims;
        bool                m_ExportSkinnings;
        bool                m_EmbedTextures;
        bool                m_IgnoreAllErrors;
        TextureFormat       m_TextureFormat;
        TextureCompression  m_TextureCompression;
        float               m_KeyframeSamplingRate;
    };
}

#endif /* XSI_GAMEBRYO_CONVERTER_SETTINGS_H */
