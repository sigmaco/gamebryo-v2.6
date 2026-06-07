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
#include "ICommand.h"
#include "MEntity.h"

using namespace System::Collections::Generic;
using namespace Emergent::Gamebryo::SceneDesigner::PluginAPI;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MLayer;
    public __gc class MEntity;

    public __gc class MMoveEntitiesToLayerCommand : public MDisposable,
        public ICommand
    {
    public:
        MMoveEntitiesToLayerCommand(MLayer* pmDestination);

        void AddEntity(MEntity* pmEntity);

    private:
        typedef List<MEntity*> Entities;
        typedef Dictionary<MLayer*, Entities*> Layers;

        MLayer* m_pmDestination;
        Layers* m_pmLayers;

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // ICommand members.
    public:
        __property String* get_Name();
        NiEntityCommandInterface* GetNiEntityCommandInterface();
        void DoCommand(bool bInBatch, bool bUndoable);
        void UndoCommand(bool bInBatch);
    };
}}}}
