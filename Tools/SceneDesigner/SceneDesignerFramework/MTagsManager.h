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
#include "MScene.h"

using namespace System::Collections::Generic;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace Framework
{
    public __gc class MTagsManager : public MDisposable
    {
    public:
        MEntity* GetEntitiesForTag(String* strTag)[];
        String* GetTags()[];

    private:
        // Event handlers.
        void OnSceneClosing(MScene* pmScene);
        void OnNewSceneLoaded(MScene* pmScene);
        void OnLayerAdded(MLayer* pmLayer, MLayer* pmParent);
        void OnEntityAddedToScene(MScene* pmScene, MEntity* pmEntity);
        void OnEntityRemovedFromScene(MScene* pmScene, MEntity* pmEntity);
        void OnEntityTagsChanged(MEntity* pmEntity, String* strOldTags);

        void AddEntitiesToMap(MEntity* amEntities[]);
        void RemoveEntitiesFromMap(MEntity* amEntities[]);
        void AddEntityToTags(String* astrTags[], MEntity* pmEntity);
        void RemoveEntityFromTags(String* astrTags[], MEntity* pmEntity);

        Dictionary<String*, List<MEntity*>*>* m_pmTagToEntityMap;

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // Singleton members.
    private public:
        static void Init();
        static void Shutdown();
        static bool InstanceIsValid();
        __property static MTagsManager* get_Instance();
    private:
        static MTagsManager* ms_pmThis = NULL;
        MTagsManager();
    };
}}}}
