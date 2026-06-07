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

#include "MDisposable.h"
#include "MEntity.h"
#include "ICommandService.h"

using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI::StandardServices;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MSelectionSet : public MDisposable
    {
    public:
        static MSelectionSet* Create(String* strName);
        static MSelectionSet* Create(String* strName,
            unsigned int uiInitialSize);

        MSelectionSet(NiEntitySelectionSet* pkSelectionSet);

        virtual String* ToString();

        NiEntitySelectionSet* GetNiEntitySelectionSet();

        __property String* get_Name();
        __property void set_Name(String* strName);

        __property unsigned int get_EntityCount();
        void AddEntity(MEntity* pmEntity);
        void RemoveEntity(MEntity* pmEntity);
        void RemoveAllEntities();
        MEntity* GetEntities()[];
        MEntity* GetEntityByName(String* strName);

    private:
        NiEntitySelectionSet* m_pkSelectionSet;

        __property static ICommandService* get_CommandService();
        static ICommandService* ms_pmCommandService;

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);
    };
}}}}
