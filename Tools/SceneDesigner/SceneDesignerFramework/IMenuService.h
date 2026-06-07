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
#include "UICommand.h"

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace PluginAPI{ namespace StandardServices
{
    /// <summary>
    /// Provides a means for plug-ins to interact with the main menu.
    /// </summary>
    /// <remarks>
    /// All menu names are 'dot delimited'. And the service will merge 
    /// the items into the existing menu structure
    /// </remarks>
    /// <example>
    /// <code>
    /// IMenuService service = //get service
    /// service.AddMenu("View.Customize", new EventHandler(Some_handler);
    /// </code>
    /// This will add to the existing View menu, if it exists and add
    /// a Customize menu that will call the delegate
    /// </example>
    public __gc __interface IMenuService : public IService
    {
        /// <summary>
        /// Adds a "leaf" menu to the main menu
        /// </summary>
        /// <param name="menu">The 'dot' delimited name of the menu</param>
        /// <param name="command">A UICommand object that will be called 
        /// when the menu is clicked</param>
        /// <remarks></remarks>
        void AddMenu(String* strMenu, UICommand* pmCommand);
        void RemoveMenu(String* strMenu, UICommand* pmCommand, 
            bool bPruneEmpty);
        bool IsMenuEnabled(String* strMenu);
        void EnableMenu(String* strMenu, bool bEnable);
        ///<summary>
        ///Intended for editor internal use
        ///</summary>
        String* GetAssignedMenus(UICommand* pmCommand)[];
    };
}}}}}
