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
#include "IInteractionMode.h"

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace PluginAPI{ namespace StandardServices
{
    /// <summary>
    /// Summary description for IInteractionModeService.
    /// </summary>
    public __gc __interface IInteractionModeService : public IService
    {
        void AddInteractionMode(IInteractionMode* pmMode);
        IInteractionMode* GetInteractionModeByName(String* strName);
        IInteractionMode* GetInteractionModes()[];

        __property IInteractionMode* get_ActiveMode();
        __property void set_ActiveMode(IInteractionMode* pmActiveMode);

        void Update(float fTime);
    };
}}}}}
