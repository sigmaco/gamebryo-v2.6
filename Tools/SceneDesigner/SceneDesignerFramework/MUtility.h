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

#pragma once

#include "IMessageService.h"

using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI::StandardServices;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MUtility
    {
    public:
        static String* GetStreamingFormatDescriptions()[];
        static String* GetStreamingFormatExtensions()[];
        static String* GetFormatFromFilename(String* filename);
        static float GetCurrentTimeInSec();
        static String* GetGamebryoVersion();
        static String* GetGamebryoBuildDate();
        static int GetVersionFromString(String* strVersion);
        static void SetImageSubFolder(String* strPath);
        static float RadiansToDegrees(float fRadians);
        static float DegreesToRadians(float fDegrees);
        static unsigned char FloatToRGB(float fColor);
        static float RGBToFloat(unsigned char ucColor);
        static void AddErrorInterfaceMessages(MessageChannelType eChannel,
            NiEntityErrorInterface* pkErrors);
        static void AddErrorInterfaceMessages(
            NiEntityErrorInterface* pkErrors);
        static Guid IDToGuid( const NiUniqueID& kID);
        static void GuidToID( Guid mGuid, NiUniqueID& kUniqueID);

    private:
        static IMessageService* ms_pmMessageService;
        __property static IMessageService* get_MessageService();
    };
}}}}
