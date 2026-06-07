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
#include "MScene.h"
#include "MPalette.h"

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace PluginAPI{ namespace StandardServices
{
    /// <summary>
    /// Summary description for ICommandPanelService.
    /// </summary>
    public __gc __interface IEntityPathService : public IService
    {
    public:
        String* GetSimpleName(String* pmFullPath);
        String* GetFullPath(MScene* pmScene, MEntity* pmEntity);
        String* FindFullPath(MEntity* pmEntity);
        MPalette* FindPaletteContainingEntity(MEntity* pmEntity);
        MEntity* FindEntity(String* strFullPath);
    };
}}}}}
