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

#include "CrosswalkGamebryoSettings.h"

#include <stdlib.h>
#include <string.h>

namespace epg
{
    //---------------------------------------------------------------------------
    // Settings controlling how the conversion will be executed.
    //---------------------------------------------------------------------------

    ConverterSettings::ConverterSettings()
        : m_GamebryoProcessingScript()
        , m_GamebryoExportingScript()
        , m_GamebryoViewingScript()
        , m_ExportCamera( true )
        , m_ExportLights( true )
        , m_ExportTextures( true )
        , m_ExportShapeAnims( true )
        , m_ExportSkinnings( true )
        , m_LaunchGamebryoAssetViewer( false )
        , m_EmbedTextures( false )
        , m_IgnoreAllErrors( false )
        , m_TextureFormat( TEXTURE_FORMAT_UNKNOWN )
        , m_TextureCompression( TEXTURE_COMPRESSION_NONE )
        , m_KeyframeSamplingRate( 0.0 )
        , m_LogFileName()
        , m_TargetPlatform(GENERIC_PLATFORM)
    {
    }

    //---------------------------------------------------------------------------
    void ConverterSettings::SetGamebryoProcessingScript(const char* in_FileName)
    {
        m_GamebryoProcessingScript = in_FileName ? in_FileName : "";
    }

    //---------------------------------------------------------------------------
    void ConverterSettings::SetGamebryoExportingScript(const char* in_FileName)
    {
        m_GamebryoExportingScript = in_FileName ? in_FileName : "";
    }

    //---------------------------------------------------------------------------
    void ConverterSettings::SetGamebryoViewingScript(const char* in_FileName)
    {
        m_GamebryoViewingScript = in_FileName ? in_FileName : "";
    }
    //---------------------------------------------------------------------------
    void ConverterSettings::SetGamebryoProcessingScriptEnc(const char* in_Enc)
    {
        m_GamebryoProcessingScriptEnc = in_Enc ? in_Enc : "";
    }

    //---------------------------------------------------------------------------
    void ConverterSettings::SetGamebryoExportingScriptEnc(const char* in_Enc)
    {
        m_GamebryoExportingScriptEnc = in_Enc ? in_Enc : "";
    }

    //---------------------------------------------------------------------------
    void ConverterSettings::SetGamebryoViewingScriptEnc(const char* in_Enc)
    {
        m_GamebryoViewingScriptEnc = in_Enc ? in_Enc : "";
    }
    //---------------------------------------------------------------------------
    void ConverterSettings::SetExportCamera(bool in_Export)
    {
        m_ExportCamera = in_Export;
    }

    //---------------------------------------------------------------------------
    void ConverterSettings::SetLogFileName(const char * in_FileName)
    {
        m_LogFileName = in_FileName ? in_FileName : "";
    }

    //---------------------------------------------------------------------------
    void ConverterSettings::SetLaunchGamebryoAssetViewer(bool in_Launch)
    {
        m_LaunchGamebryoAssetViewer = in_Launch;
    }

    //---------------------------------------------------------------------------
    void ConverterSettings::SetExportLights(bool in_Export)
    {
        m_ExportLights = in_Export;
    }

    //---------------------------------------------------------------------------
    void ConverterSettings::SetExportShapeAnimations(bool in_Export)
    {
        m_ExportShapeAnims = in_Export;
    }

    //---------------------------------------------------------------------------
    void ConverterSettings::SetExportSkinnings(bool in_Export)
    {
        m_ExportSkinnings = in_Export;
    }

    //---------------------------------------------------------------------------
    void ConverterSettings::SetEmbedTextures(bool in_Embed)
    {
        m_EmbedTextures = in_Embed;
    }

    //---------------------------------------------------------------------------
    void ConverterSettings::SetTextureCompression(TextureCompression in_Compression)
    {
        m_TextureCompression = in_Compression;
    }

    //---------------------------------------------------------------------------
    void ConverterSettings::SetTextureFormat(TextureFormat in_Format)
    {
        m_TextureFormat = in_Format;
    }

    //---------------------------------------------------------------------------
    void ConverterSettings::SetKeyframeSamplingRate(float in_Rate)
    {
        m_KeyframeSamplingRate = in_Rate;
    }

    //---------------------------------------------------------------------------
    void ConverterSettings::SetExportTextures(bool in_Export)
    {
        m_ExportTextures = in_Export;
    }

    //---------------------------------------------------------------------------
    void ConverterSettings::SetIgnoreAllErrors(bool in_IgnoreErrors)
    {
        m_IgnoreAllErrors = in_IgnoreErrors;
    }

    //---------------------------------------------------------------------------
    void ConverterSettings::SetTargetPlatform(TargetPlatform in_TargetPlatform)
    {
        m_TargetPlatform = in_TargetPlatform;
    }

    //---------------------------------------------------------------------------
    void ConverterSettings::SetMeshProfile(const char * in_MeshProfileName)
    {
        m_MeshProfile = in_MeshProfileName;
    }

    //---------------------------------------------------------------------------
    const char* ConverterSettings::GetGamebryoProcessingScript() const
    {
        return m_GamebryoProcessingScript.c_str();
    }

    //---------------------------------------------------------------------------
    const char* ConverterSettings::GetGamebryoExportingScript() const
    {
        return m_GamebryoExportingScript.c_str();
    }

    //---------------------------------------------------------------------------
    const char* ConverterSettings::GetGamebryoViewingScript() const
    {
        return m_GamebryoViewingScript.c_str();
    }
    //---------------------------------------------------------------------------
    const char* ConverterSettings::GetGamebryoProcessingScriptEnc() const
    {
        return m_GamebryoProcessingScriptEnc.c_str();
    }

    //---------------------------------------------------------------------------
    const char* ConverterSettings::GetGamebryoExportingScriptEnc() const
    {
        return m_GamebryoExportingScriptEnc.c_str();
    }

    //---------------------------------------------------------------------------
    const char* ConverterSettings::GetGamebryoViewingScriptEnc() const
    {
        return m_GamebryoViewingScriptEnc.c_str();
    }
    //---------------------------------------------------------------------------
    bool ConverterSettings::GetExportCamera() const
    {
        return m_ExportCamera;
    }

    //---------------------------------------------------------------------------
    bool ConverterSettings::GetExportLights() const
    {
        return m_ExportLights;
    }

    //---------------------------------------------------------------------------
    bool ConverterSettings::GetExportTextures() const
    {
        return m_ExportTextures;
    }

    //---------------------------------------------------------------------------
    bool ConverterSettings::GetExportShapeAnimations() const
    {
        return m_ExportShapeAnims;
    }

    //---------------------------------------------------------------------------
    bool ConverterSettings::GetExportSkinnings() const
    {
        return m_ExportSkinnings;
    }

    //---------------------------------------------------------------------------
    bool ConverterSettings::GetLaunchGamebryoAssetViewer() const
    {
        return m_LaunchGamebryoAssetViewer;
    }

    //---------------------------------------------------------------------------
    bool ConverterSettings::GetEmbedTextures() const
    {
        return m_EmbedTextures;
    }

    //---------------------------------------------------------------------------
    TextureFormat ConverterSettings::GetTextureFormat() const
    {
        return m_TextureFormat;
    }

    //---------------------------------------------------------------------------
    TextureCompression ConverterSettings::GetTextureCompression() const
    {
        return m_TextureCompression;
    }

    //---------------------------------------------------------------------------
    float ConverterSettings::GetKeyframeSamplingRate() const
    {
        return m_KeyframeSamplingRate;
    }

    //---------------------------------------------------------------------------
    const char* ConverterSettings::GetLogFileName() const
    {
        return m_LogFileName.c_str();
    }

    //---------------------------------------------------------------------------
    const char* ConverterSettings::GetMeshProfile() const
    {
        return m_MeshProfile.c_str();
    }

    //---------------------------------------------------------------------------
    bool ConverterSettings::GetIgnoreAllErrors() const
    {
        return m_IgnoreAllErrors;
    }

    //---------------------------------------------------------------------------
    TargetPlatform ConverterSettings::GetTargetPlatform() const
    {
        return m_TargetPlatform;
    }

    //---------------------------------------------------------------------------
}
