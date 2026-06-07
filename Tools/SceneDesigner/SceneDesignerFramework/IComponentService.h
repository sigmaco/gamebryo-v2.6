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
#include "MComponent.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace PluginAPI{ namespace StandardServices
{
    /// <summary>
    /// Summary description for ICommandService.
    /// </summary>
    public __gc __interface IComponentService : public IService
    {
        bool RegisterComponent(MComponent* pmComponent);
        bool RegisterComponent(MComponent* pmComponent, bool bVisibleToUser);
        void UnregisterComponent(MComponent* pmComponent);
        void UnregisterAllComponents();

        String* GetComponentNames()[];
        MComponent* GetAllComponents()[];
        //MComponent* CloneComponentByName(String* strName);
        MComponent* CloneComponentByID(Guid mGuid);
        MComponent* GetComponentByID(Guid mGuid);
        bool IsComponentVisibleToUser(MComponent* pmComponent);
    };
}}}}}
