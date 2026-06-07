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

using namespace System::Windows::Forms;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace PluginAPI{ namespace StandardServices
{
    /// <summary>
    /// Summary description for ICommandPanelService.
    /// </summary>
    public __gc __interface ICommandPanelService : public IService
    {
        void RegisterPanel(Form* pmForm);
        void ShowAllPanels();
        /// <summary>
        /// Shows or hides a panel
        /// </summary>
        /// <param name="name">name of panel based on the Form.Test
        /// property</param>
        /// <param name="show">'true' shows the panel,
        /// 'false' hides it</param>
        void ShowPanel(String* strName, bool bShow);
        Form* GetPanel(String* pmName);
    };
}}}}}
