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

#ifndef XSI_GAMEBRYO_CONVERTER_H
#define XSI_GAMEBRYO_CONVERTER_H

#include "CrosswalkGamebryoSettings.h"
#include "CrosswalkGamebryoNodeMap.h"
#include "CrosswalkGamebryoMatMap.h"
#include "CrosswalkGamebryoTexMap.h"
#include "CrosswalkGamebryoEffectMap.h"
#include "CrosswalkGamebryoAnimMap.h"

#include "SLTypes.h"

//---------------------------------------------------------------------------
// External references.
//---------------------------------------------------------------------------

class XSIEXPORT CSLScene;

namespace epg
{
    class Logger;
    class ProgressMonitor;

    //---------------------------------------------------------------------------
    /// Main conversion entry-point.
    //---------------------------------------------------------------------------

    class Converter
    {
    public:
        /** Create a converter with the specified settings.
            @param ConverterSettings& the settings to be used for conversion.
        */
        Converter(const ConverterSettings & in_Settings);

        /// Reset the converter. Keeps the settings.
        void Clear();

        /** Set the logger to be used. The object must exists as long
            as it it used by the converter.
            @param Logger* the logger to be used.
        */
        void SetLogger(Logger& in_Logger);

        /// Set the default accumulating logger.
        void SetDefaultLogger();

        /** Set the progress monitor. The object must exists as long
            as it is used by the converter.
        */
        void SetProgressMonitor(ProgressMonitor& in_Monitor);

        /// Set the default (do-nothing) progress monitor.
        void SetDefaultProgressMonitor();

        /** Retrieve the current logger.
            @return the current logger.
        */
        Logger& GetLogger() const;

        /** Convert an open dotXSI scene to the Gamebryo format.
            @param CSLScene the scene to be converted.
        */
        bool Convert(CSLScene& in_Scene);

        /// Retrieve the node map.
        NodeMap & GetNodeMap();

        /// Retrieve the material map.
        MaterialMap& GetMaterialMap();

        /// Retrieve the texture map.
        TextureMap& GetTextureMap();

        /// Retrieve the effect map.
        EffectMap& GetEffectMap();

        /// Retrieve the settings.
        ConverterSettings& GetSettings();

    private:
        // Disable copying.
        Converter(const Converter &);
        void operator=(const Converter &);
        void BuildTimerSharedData(float fStartTime, float fEndTime);

        ConverterSettings       m_Settings;
        Logger*                 m_pLogger;
        ProgressMonitor*        m_pProgress;
        NodeMap                 m_NodeMap;
        MaterialMap             m_MaterialMap;
        TextureMap              m_TextureMap;
        EffectMap               m_EffectMap;
        AnimationMap            m_AnimationMap;
    };

    //---------------------------------------------------------------------------
    /// Export a dotXSI file to a NIF using Gamebryo pipeline scripts.
    ///
    /// @param in_dotXSIFileName  the name of the input dotXSI file to export.
    /// @param in_OutputFileName  the name of the NIF file to produce.
    /// @param in_Logger          the object receiving export log messages.
    //---------------------------------------------------------------------------
    bool GamebryoScriptedExport(
        const char* in_dotXSIFileName,
        const char* in_OutputFileName,
        const ConverterSettings& in_Settings,
        Logger& in_Logger);

    //---------------------------------------------------------------------------
    /// Export a dotXSI file to a NIF directly.
    ///
    /// @param in_dotXSIFileName  the name of the input dotXSI file to export.
    /// @param in_OutputFileName  the name of the NIF file to produce.
    /// @param in_IgnoreAllErrors if true then ignore all errors.
    /// @param in_Platform        selects the target platform.
    /// @param in_Logger          the object receiving export log messages.
    //---------------------------------------------------------------------------
    bool GamebryoDirectExport(
        const char* in_dotXSIFileName,
        const char* in_OutputFileName,
        bool in_IgnoreAllErrors,
        epg::TargetPlatform in_Platform,
        Logger& in_Logger,
        ProgressMonitor& in_Progress);

    void BuildRendererSharedData();
    void DeleteAllSharedData();
}

#endif /* XSI_GAMEBRYO_CONVERTER_H */
