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
#include "SettingChangedEventArgs.h"

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace PluginAPI{ namespace StandardServices
{
    public __delegate void SettingChangedHandler(Object* pmSender,
        SettingChangedEventArgs* pmEventArgs);

    /// <summary>
    /// Summary description for ISettingsService.
    /// </summary>
    public __gc __interface ISettingsService : public IService
    {
        /// <summary>
        /// This will set the value of a setting only if it does not 
        /// already exist. It's a way of setting a default.
        /// </summary>
        /// <param name="name">The unique key to be used to store the 
        /// object</param>
        /// <param name="obj">The default object to store if one does not
        ///  exist</param>
        /// <param name="category">The location in which to store
        ///  the settings object</param>
        void RegisterSettingsObject(String* strName, Object* pmObject,
            SettingsCategory eCategory);

        /// <summary>
        /// Retrieves and object form the system settings 
        /// </summary>
        /// <param name="name">Unique key string to 
        /// retieve the settigns object</param>
        /// <param name="category">The location from which to retrieve 
        /// the settings object</param>
        /// <returns>The settings object</returns>
        Object* GetSettingsObject(String* strName,
            SettingsCategory eCategory);

        /// <summary>
        /// Sets an object in the system settings
        /// </summary>
        /// <param name="name">The unique key to be used to store the 
        /// object</param>
        /// <param name="obj">The object to store</param>
        /// <param name="category">The location in which to store
        ///  the settings object</param>
        void SetSettingsObject(String* strName, Object* pmObject, 
            SettingsCategory eCategory);

        void RemoveSettingsObject(String* strName, SettingsCategory eCategory);

        __property String* get_ScenePath();
        __property void set_ScenePath(String* strPath);

        __property String* get_SceneFileName();
        __property void set_SceneFileName(String* strFileName);

        /// <summary>
        /// Streams the current settings infrastructure to persistent storage
        /// </summary>
        /// <param name="category"></param>
        void SaveSettings(SettingsCategory eCategory);

        /// <summary>
        /// Reads the current settings infrastructure from persistent storage
        /// </summary>
        /// <param name="category"></param>
        void LoadSettings(SettingsCategory eCategory);

        /// <summary>
        /// Empties the in-memory cache of settings for the indicated
        /// settings store
        /// </summary>
        /// <param name="category"></param>
        void ClearSettings(SettingsCategory eCategory);

        void SetChangedSettingHandler(String* strName,
            SettingsCategory eCategory, SettingChangedHandler* pmHandler);
    };
}}}}}
