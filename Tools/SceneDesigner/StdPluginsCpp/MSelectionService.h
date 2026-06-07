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

using namespace System::Collections;

namespace Emergent{ namespace Gamebryo{ namespace SceneDesigner{
    namespace StdPluginsCpp
{
    public __gc class MSelectionService : public MDisposable,
        public ISelectionService
    {
    public:
        MSelectionService();

        void RegisterSettings();

    private:
        void OnSettingsChanged(Object* pmSender,
            SettingChangedEventArgs* pmEventArgs);

        [UICommandHandlerAttribute("DeleteSelectedEntities")]
        void OnDeleteSelectedEntities(Object* pmSender,
            EventArgs* pmEventArgs);
        [UICommandValidatorAttribute("DeleteSelectedEntities")]
        void OnValidateDeleteSelectedEntities(Object* pmSender,
            UIState* pmState);

        __property static ICommandService* get_CommandService();
        static ICommandService* ms_pmCommandService;

        __property static ISettingsService* get_SettingsService();
        static ISettingsService* ms_pmSettingsService;

        __property static IOptionsService* get_OptionsService();
        static IOptionsService* ms_pmOptionsService;

        __property static IMessageService* get_MessageService();
        static IMessageService* ms_pmMessageService;

        void OnSceneClosing(MScene* pmScene);
        void OnNewSceneLoaded(MScene* pmScene);
        void OnEntityRemovedFromScene(MScene* pmScene, MEntity* pmEntity);
        void OnEntityHiddenStateChanged(MEntity* pmEntity, bool bHidden);
        void OnEntityFrozenStateChanged(MEntity* pmEntity, bool bFrozen);
        void OnEntityPropertyChanged(MEntity* pmEntity,
            String* strPropertyName, unsigned int uiPropertyIndex,
            bool bInBatch);
        void OnSelectedEntitiesChanged();
        void OnLongOperationCompleted();

        void RecalculateSelectionCenter();
        void AddToSelectionCenter(MEntity* pmEntity);
        void RemoveFromSelectionCenter(MEntity* pmEntity);
        void RebuildHashtable();

        MSelectionSet* m_pmSelectedEntities;
        Hashtable* m_pmEntityHashtable;
        MPoint3* m_pmCenter;
        bool m_bCommandsAreUndoable;
        bool m_bProcessSelectedEntitiesChanged;
        bool m_bRecalculateSelectionCenter;

        static String* ms_strTranslationName = "Translation";

    // MDisposable members.
    protected:
        virtual void Do_Dispose(bool bDisposing);

    // IService members.
    public:
        __property String* get_Name();
        bool Initialize();
        bool Start();

    // ISelectionService members.
    public:
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
        __property MEntity* get_SelectedEntity();
        __property void set_SelectedEntity(MEntity* pmEntity);
        __property MPoint3* get_SelectionCenter();
        __property void set_SelectionCenter(MPoint3* pmCenter);
        bool IsEntitySelected(MEntity* pmEntity);
    };
}}}}
