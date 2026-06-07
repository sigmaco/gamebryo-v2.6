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
#include "MEntity.h"
#include "MSelectionSet.h"
#include "MPoint3.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace PluginAPI{ namespace StandardServices
{
    /// <summary>
    /// Summary description for ISelectionService.
    /// </summary>
    public __gc __interface ISelectionService : public IService
    {
        void AddEntitiesToSelection(MEntity* amEntities[]);
        void AddEntityToSelection(MEntity* pmEntity);
        void AddSelectionSetToSelection(MSelectionSet* pmSelectionSet);

        void RemoveEntitiesFromSelection(MEntity* amEntities[]);
        void RemoveEntityFromSelection(MEntity* pmEntity);
        void RemoveSelectionSetFromSelection(MSelectionSet* pmSelectionSet);

        void ReplaceSelection(MEntity* amEntities[]);
        void ReplaceSelection(MSelectionSet* pmSelectionSet);

        void ClearSelectedEntities();

        __property int get_NumSelectedEntities();
        MEntity* GetSelectedEntities()[];

        __property bool get_CommandsAreUndoable();

        // This property will return NULL if more than one entity is selected.
        // Setting this property will result in the selection set being
        // cleared and only one selected property remaining.
        __property MEntity* get_SelectedEntity();
        __property void set_SelectedEntity(MEntity* pmEntity);

        __property MPoint3* get_SelectionCenter();
        __property void set_SelectionCenter(MPoint3* pmCenter);

        bool IsEntitySelected(MEntity* pmEntity);
    };
}}}}}
