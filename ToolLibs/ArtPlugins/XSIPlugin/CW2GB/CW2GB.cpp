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

#include <stdio.h>
#include <tchar.h>
#include <direct.h>

#include "CrosswalkGamebryoConverter.h"
#include "CrosswalkGamebryoSimpleLoggers.h"
#include "CrosswalkGamebryoSimpleProgress.h"
#include "CrosswalkGamebryoHelpers.h"
#include "CrosswalkGamebryoInit.h"

#include <Scene.h>

#include "NiStaticDataManager.h"
#include "NiAVObject.h"
#include "NiMaterialProperty.h"
#include "NiTexturingProperty.h"
#include "NiDynamicEffect.h"
#include "NiTransformController.h"
#include "NiSourceTexture.h"
#include "NiXMLLogger.h"
#include "NiExporterOptionsSharedData.h"
#include "NiPluginToolkit.h"
#include "NiMain.h"
#include "NiMesh.h"
#include "NiFilename.h"
#include "NiMorphWeightsController.h"

namespace
{
    //---------------------------------------------------------------------------
    struct ConversionArguments
    {
        bool parse(int argc, _TCHAR* argv[], epg::StdoutLogger& logger)
        {
            ignoreAllErrors = false;
            platform = epg::UNKNOWN_PLATFORM;

            for (int i = 1; i < argc; ++i)
            {
                if (strcmp(argv[i],"--ignore_errors") == 0)
                    ignoreAllErrors = true;
                else if (strcmp(argv[i],"--verbose") == 0 || strcmp(argv[i],"-v") == 0)
                    logger.SetMinPrintableLevel(epg::LOG_DEBUG);
                else if (strcmp(argv[i],"--silent") == 0 || strcmp(argv[i],"-s") == 0)
                    logger.SetMinPrintableLevel(epg::LOG_ERROR);
                else if (strcmp(argv[i],"--platform") == 0 || strcmp(argv[i],"-p") == 0)
                    i += parsePlatform(argc, argv, i+i, logger);
                else if (!inputFileName.size())
                    inputFileName = argv[i];
                else if (!scriptFileName.size())
                    scriptFileName = argv[i];
            }

            if (!inputFileName.size())
            {
                return logger.Log(
                    epg::LOG_ERROR,
                    "Usage: CW2GB <dotXSI filename> [Gamebryo plugin script] [options]\n"
                    "Options:\n"
                    "   --ignore_errors      ignore all errors and continue processing.\n"
                    "   --verbose or -v      print all messages.\n"
                    "   --silent or -s       only print error messages.\n"
                    "   --platform or -p X   use platform X when processing the input.\n"
                    "                        X can be: xbox, ps3, dx9, dx10, wii or generic.");
            }

            outputFileName = inputFileName;
            if (outputFileName.size() > 4
            && outputFileName.substr(outputFileName.size() - 4).compare(".xsi") == 0)
            {
                outputFileName.resize(outputFileName.size() - 4);
            }
            outputFileName += ".nif";

            return true;
        }

        std::string inputFileName;
        std::string outputFileName;
        std::string scriptFileName;
        bool ignoreAllErrors;
        epg::TargetPlatform platform;

    private:
        int parsePlatform(int argc, _TCHAR* argv[], int index, epg::StdoutLogger& logger)
        {
            if (index >= argc)
            {
                logger.Logf(epg::LOG_ERROR, "Missing platform command-line parameter.");
                return 0;
            }

            const char* arg = argv[index];

            if (0 == strcmp(arg, "xbox"))
            {
                platform = epg::XBOX_360_PLATFORM;
                return 1;
            }

            if (0 == strcmp(arg, "ps3"))
            {
                platform = epg::PS3_PLATFORM;
                return 1;
            }

            if (0 == strcmp(arg, "dx9"))
            {
                platform = epg::DIRECTX_9_PLATFORM;
                return 1;
            }

            if (0 == strcmp(arg, "dx10"))
            {
                platform = epg::DIRECTX_10_PLATFORM;
                return 1;
            }

            if (0 == strcmp(arg, "wii"))
            {
                platform = epg::WII_PLATFORM;
                return 1;
            }

            if (0 == strcmp(arg, "generic"))
            {
                platform = epg::GENERIC_PLATFORM;
                return 1;
            }

            logger.Logf(epg::LOG_ERROR, "Unsupported platform \"%s\".", arg);
            return 1;
        }
    };

    //---------------------------------------------------------------------------
    int convert(int argc, _TCHAR* argv[], epg::StdoutLogger& logger)
    {
        ConversionArguments args;
        if (!args.parse(argc, argv, logger))
            return 1;

        const char* inName  = args.inputFileName.c_str();
        const char* outName = args.outputFileName.c_str();

        if (args.scriptFileName.size() > 0)
        {
            epg::ConverterSettings settings;
            settings.SetGamebryoProcessingScript(args.scriptFileName.c_str());
            settings.SetIgnoreAllErrors(args.ignoreAllErrors);

            if (!epg::GamebryoScriptedExport(inName, outName, settings, logger))
            {
                return 1;
            }
        }
        else
        {
            // Control the progress silence by checking if logger was silenced.
            epg::StdoutProgress progress(logger.GetMinPrintableLevel() == epg::LOG_ERROR);
            if (!epg::GamebryoDirectExport(
                inName, outName, args.ignoreAllErrors, args.platform, logger, progress))
            {
                return 1;
            }
        }

        logger.Logf(
            epg::LOG_INFO,
            "Saved objects to Gamebryo file \"%s\".",
            args.outputFileName.c_str());

        return 0;
    }
}

//---------------------------------------------------------------------------
int _tmain(int argc, _TCHAR* argv[])
{
    epg::InitializeGamebryo("CW2GB");

    epg::StdoutLogger logger(epg::LOG_DEBUG);

    int result = convert(argc, argv, logger);

    epg::ShutdownGamebryo("CW2GB");

    if (logger.GetInfoMessageCount() > 0 )
        printf(
            "Got %d informational message%s.\n",
            logger.GetInfoMessageCount(), logger.GetInfoMessageCount() > 1 ? "s" : "");
    if (logger.GetWarningMessageCount() > 0)
        printf(
            "Got %d warning%s.\n",
            logger.GetWarningMessageCount(), logger.GetWarningMessageCount() > 1 ? "s" : "");
    if (logger.GetErrorMessageCount() > 0)
        printf(
            "Got %d error%s.\n",
            logger.GetErrorMessageCount(), logger.GetErrorMessageCount() > 1 ? "s" : "");

    return result;
}
//---------------------------------------------------------------------------
