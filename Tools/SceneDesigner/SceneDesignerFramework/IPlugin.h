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

#include "IService.h"

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace PluginAPI
{
    /// <summary>
    /// Summary description for IPlugin.
    /// </summary>
    public __gc __interface IPlugin
    {
        __property String* get_Name();

        __property System::Version* get_Version();
        __property System::Version* get_ExpectedVersion();

        /// <summary>
        /// Called when a Plugin is loaded
        /// </summary>
        /// <param name="toolMajorVersion"></param>
        /// <param name="toolMinorVersion"></param>
        /// <returns>Implementors should return true upon success</returns>
        void Load(int iToolMajorVersion, int iToolMinorVersion);

        /// <summary>
        /// Provides a mechanism to allow packages to report services
        /// </summary>
        /// <returns>
        /// A list of service objects that this package provides
        /// </returns>
        IService* GetProvidedServices()[];

        /// <summary>
        /// Provides a mechanism for a plugin to interact with services.
        /// </summary>
        void Start();
    };
}}}}
