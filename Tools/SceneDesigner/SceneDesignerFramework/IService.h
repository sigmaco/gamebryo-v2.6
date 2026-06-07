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

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace PluginAPI
{
    /// <summary>
    /// Summary description for IService.
    /// </summary>
    public __gc __interface IService
    {
        __property String* get_Name();

        /// <summary>
        /// Allows a service to perform initialization code.
        /// </summary>
        /// <returns>true upon success</returns>
        /// <remarks>A service may not assume that any other service exists
        /// at the point this is called. The order of service intitialization 
        /// is not fixed and may change when new plug-ins are added</remarks>
        bool Initialize();

        /// <summary>
        /// Allows a service to interact with other services to perform 
        /// start-up routines.
        /// </summary>
        /// <returns></returns>
        bool Start();
    };
}}}}
