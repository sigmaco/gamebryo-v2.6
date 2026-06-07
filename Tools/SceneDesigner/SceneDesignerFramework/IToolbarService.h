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

using namespace System::Drawing;
using namespace System::Windows::Forms;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace PluginAPI{ namespace StandardServices
{
    /// <summary>
    /// Summary description for ICommandPanelService.
    /// </summary>
    public __gc __interface IToolBarService : public IService
    {
        bool AddToolBar(String* strName);
        bool RemoveToolBar(String* strName);

        bool AddToolBarButton(String* strToolBarName, String* strButtonName,
            Image* pmButtonImage, UICommand* pmCommand);
        bool ReplaceToolBarButton(String* strToolBarName, 
            String* strOldButtonName, String* strNewButtonName,
            Image* pmNewButtonImage, UICommand* pmNewCommand);
        bool EnableToolBarButton(String* strToolBarName,
            String* strButtonName, bool bEnabled);
        bool CheckToolBarButton(String* strToolBarName, String* strButtonName,
            bool bChecked);
        bool RemoveToolBarButton(String* strToolBarName,
            String* strButtonName);

        void UpdateToolTips();

        __property String* get_StandardToolBarName();

    };
}}}}}
