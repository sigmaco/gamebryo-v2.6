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
    /// Summary description for ServiceProvider.
    /// </summary>
    public __gc __abstract class ServiceProvider 
    {
    public:
        /// <summary>
        /// Returns the first service that implements the supplied interface
        /// </summary>
        /// <param name="serviceInterface"></param>
        /// <returns></returns>
        virtual IService* GetService(Type* pmServiceInterface) = 0;

        virtual void Shutdown() = 0;


    // Singleton members.
    public:
        __property static ServiceProvider* get_Instance();
    protected:
        ServiceProvider();
        static ServiceProvider* ms_pmThis;
    };
}}}}
