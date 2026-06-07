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

// Precompiled Header
#include "StdPluginsCppPCH.h"

#include "MSelectionService.h"
#include "MSettingsHelper.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

//---------------------------------------------------------------------------
MSelectionService::MSelectionService() : m_bCommandsAreUndoable(true),
    m_bProcessSelectedEntitiesChanged(true),
    m_bRecalculateSelectionCenter(false)
{
    __hook(&MEventManager::SceneClosing, MFramework::Instance->EventManager,
        &MSelectionService::OnSceneClosing);
    __hook(&MEventManager::NewSceneLoaded, MFramework::Instance->EventManager,
        &MSelectionService::OnNewSceneLoaded);
    __hook(&MEventManager::EntityRemovedFromScene,
        MFramework::Instance->EventManager,
        &MSelectionService::OnEntityRemovedFromScene);
    __hook(&MEventManager::EntityHiddenStateChanged,
        MFramework::Instance->EventManager,
        &MSelectionService::OnEntityHiddenStateChanged);
    __hook(&MEventManager::EntityFrozenStateChanged,
        MFramework::Instance->EventManager,
        &MSelectionService::OnEntityFrozenStateChanged);
    __hook(&MEventManager::EntityPropertyChanged,
        MFramework::Instance->EventManager,
        &MSelectionService::OnEntityPropertyChanged);
    __hook(&MEventManager::SelectedEntitiesChanged,
        MFramework::Instance->EventManager,
        &MSelectionService::OnSelectedEntitiesChanged);
    __hook(&MEventManager::LongOperationCompleted,
        MFramework::Instance->EventManager,
        &MSelectionService::OnLongOperationCompleted);

    m_pmSelectedEntities = MSelectionSet::Create("Main");
    m_pmEntityHashtable = new Hashtable();
    m_pmCenter = new MPoint3(NiPoint3(0.0f, 0.0f, 0.0f));
}
//---------------------------------------------------------------------------
void MSelectionService::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        __unhook(&MEventManager::SceneClosing,
            MFramework::Instance->EventManager,
            &MSelectionService::OnSceneClosing);
        __unhook(&MEventManager::NewSceneLoaded,
            MFramework::Instance->EventManager,
            &MSelectionService::OnNewSceneLoaded);
        __unhook(&MEventManager::EntityRemovedFromScene,
            MFramework::Instance->EventManager,
            &MSelectionService::OnEntityRemovedFromScene);
        __unhook(&MEventManager::EntityHiddenStateChanged,
            MFramework::Instance->EventManager,
            &MSelectionService::OnEntityHiddenStateChanged);
        __unhook(&MEventManager::EntityFrozenStateChanged,
            MFramework::Instance->EventManager,
            &MSelectionService::OnEntityFrozenStateChanged);
        __unhook(&MEventManager::EntityPropertyChanged,
            MFramework::Instance->EventManager,
            &MSelectionService::OnEntityPropertyChanged);
        __unhook(&MEventManager::SelectedEntitiesChanged,
            MFramework::Instance->EventManager,
            &MSelectionService::OnSelectedEntitiesChanged);
        __unhook(&MEventManager::LongOperationCompleted,
            MFramework::Instance->EventManager,
            &MSelectionService::OnLongOperationCompleted);

        m_pmSelectedEntities->Dispose();
        m_pmSelectedEntities = NULL;
    }
}
//---------------------------------------------------------------------------
void MSelectionService::RegisterSettings()
{
    MVerifyValidInstance;

    SettingChangedHandler* pmHandler = new SettingChangedHandler(this, 
        &MSelectionService::OnSettingsChanged);

    MSettingsHelper::GetStandardSetting(MSettingsHelper::SELECTION_UNDOABLE,
        m_bCommandsAreUndoable, pmHandler);
}
//---------------------------------------------------------------------------
void MSelectionService::OnSettingsChanged(Object*,
    SettingChangedEventArgs* pmEventArgs)
{
    MVerifyValidInstance;

    String* strSetting = pmEventArgs->Name;
    SettingsCategory eCategory = pmEventArgs->Category;

    if (MSettingsHelper::IsArgStandardSetting(pmEventArgs, 
        MSettingsHelper::SELECTION_UNDOABLE))
    {
        Object* pmObj;
        pmObj = SettingsService->GetSettingsObject(strSetting, eCategory);
        __box bool* pbVal = dynamic_cast<__box bool*>(pmObj);
        if (pbVal != NULL)
        {
            m_bCommandsAreUndoable = *pbVal;
        }
    }
}
//---------------------------------------------------------------------------
void MSelectionService::OnDeleteSelectedEntities(Object*,
    EventArgs*)
{
    MVerifyValidInstance;

    CommandService->BeginUndoFrame(String::Format("Delete {0} entities",
        this->NumSelectedEntities.ToString()));

    MEntity* amSelectedEntities[] = GetSelectedEntities();
    for (int i = 0; i < amSelectedEntities->Length; i++)
    {
        MEntity* pmEntity = amSelectedEntities[i];
        MFramework::Instance->Scene->RemoveEntity(pmEntity, true);
    }

    CommandService->EndUndoFrame(true);
}
//---------------------------------------------------------------------------
void MSelectionService::OnValidateDeleteSelectedEntities(Object*,
    UIState* pmState)
{
    if (this->NumSelectedEntities > 0)
    {
        pmState->Enabled = true;
    }
    else
    {
        pmState->Enabled = false;
    }
}
//---------------------------------------------------------------------------
ICommandService* MSelectionService::get_CommandService()
{
    if (ms_pmCommandService == NULL)
    {
        ms_pmCommandService = MGetService(ICommandService);
        MAssert(ms_pmCommandService != NULL, "Command service not found!");
    }
    return ms_pmCommandService;
}
//---------------------------------------------------------------------------
ISettingsService* MSelectionService::get_SettingsService()
{
    if (ms_pmSettingsService == NULL)
    {
        ms_pmSettingsService = MGetService(ISettingsService);
        MAssert(ms_pmSettingsService != NULL, "Settings service not found.");
    }
    return ms_pmSettingsService;
}
//---------------------------------------------------------------------------
IOptionsService* MSelectionService::get_OptionsService()
{
    if (ms_pmOptionsService == NULL)
    {
        ms_pmOptionsService = MGetService(IOptionsService);
        MAssert(ms_pmOptionsService != NULL, "Options service not found.");
    }
    return ms_pmOptionsService;
}
//---------------------------------------------------------------------------
IMessageService* MSelectionService::get_MessageService()
{
    if (ms_pmMessageService == NULL)
    {
        ms_pmMessageService = MGetService(IMessageService);
        MAssert(ms_pmMessageService != NULL, "Message service not found.");
    }
    return ms_pmMessageService;
}
//---------------------------------------------------------------------------
void MSelectionService::OnSceneClosing(MScene*)
{
    MVerifyValidInstance;

    m_pmSelectedEntities->RemoveAllEntities();
    m_pmEntityHashtable->Clear();
}
//---------------------------------------------------------------------------
void MSelectionService::OnNewSceneLoaded(MScene*)
{
    MVerifyValidInstance;

    RecalculateSelectionCenter();
}
//---------------------------------------------------------------------------
void MSelectionService::OnEntityRemovedFromScene(MScene*,
    MEntity* pmEntity)
{
    MVerifyValidInstance;

    if (m_pmEntityHashtable->Contains(pmEntity))
    {
        RemoveEntityFromSelection(pmEntity);
    }
}
//---------------------------------------------------------------------------
void MSelectionService::OnEntityHiddenStateChanged(MEntity* pmEntity,
    bool bHidden)
{
    MVerifyValidInstance;

    if (bHidden && m_pmEntityHashtable->Contains(pmEntity))
    {
        RemoveEntityFromSelection(pmEntity);
    }
}
//---------------------------------------------------------------------------
void MSelectionService::OnEntityFrozenStateChanged(MEntity* pmEntity,
    bool bFrozen)
{
    MVerifyValidInstance;

    if (bFrozen && m_pmEntityHashtable->Contains(pmEntity))
    {
        RemoveEntityFromSelection(pmEntity);
    }
}
//---------------------------------------------------------------------------
void MSelectionService::OnEntityPropertyChanged(MEntity*,
    String* strPropertyName, unsigned int, bool)
{
    MVerifyValidInstance;

    if (strPropertyName->Equals(ms_strTranslationName))
    {
        if (!MFramework::Instance->PerformingLongOperation)
        {
            RecalculateSelectionCenter();
        }
        else
        {
            m_bRecalculateSelectionCenter = true;
        }
    }
}
//---------------------------------------------------------------------------
void MSelectionService::OnSelectedEntitiesChanged()
{
    MVerifyValidInstance;

    if (m_bProcessSelectedEntitiesChanged)
    {
        RebuildHashtable();
    }
}
//---------------------------------------------------------------------------
void MSelectionService::OnLongOperationCompleted()
{
    MVerifyValidInstance;

    if (m_bRecalculateSelectionCenter)
    {
        RecalculateSelectionCenter();
    }

    m_bRecalculateSelectionCenter = false;
}
//---------------------------------------------------------------------------
void MSelectionService::RecalculateSelectionCenter()
{
    MVerifyValidInstance;

    NiPoint3 kNewCenter = NiPoint3::ZERO;

    MEntity* amEntities[] = m_pmSelectedEntities->GetEntities();
    for (int i = 0; i < amEntities->Length; i++)
    {
        MEntity* pmEntity = amEntities[i];
        if (pmEntity->HasProperty(ms_strTranslationName))
        {
            MPoint3* pmEntityTranslation = dynamic_cast<MPoint3*>(pmEntity->
                GetPropertyData(ms_strTranslationName));
            NiPoint3 kEntityTranslation;
            pmEntityTranslation->ToNiPoint3(kEntityTranslation);
            kNewCenter += kEntityTranslation;
        }
    }

    if (amEntities->Length > 0)
    {
        kNewCenter = kNewCenter / ((float) amEntities->Length);
    }
    m_pmCenter->SetData(kNewCenter);
}
//---------------------------------------------------------------------------
void MSelectionService::AddToSelectionCenter(MEntity* pmEntity)
{
    MVerifyValidInstance;

    if (pmEntity->HasProperty(ms_strTranslationName))
    {
        float fEntityCount = (float) m_pmSelectedEntities->EntityCount;

        NiPoint3 kNewCenter;
        m_pmCenter->ToNiPoint3(kNewCenter);

        MPoint3* pmEntityTranslation = dynamic_cast<MPoint3*>(pmEntity->
            GetPropertyData(ms_strTranslationName));
        NiPoint3 kEntityTranslation;
        pmEntityTranslation->ToNiPoint3(kEntityTranslation);

        kNewCenter = ((fEntityCount - 1.0f) / fEntityCount) * 
            kNewCenter + (1.0f / fEntityCount) * kEntityTranslation;
        m_pmCenter->SetData(kNewCenter);
    }
}
//---------------------------------------------------------------------------
void MSelectionService::RemoveFromSelectionCenter(MEntity* pmEntity)
{
    MVerifyValidInstance;

    if (pmEntity->HasProperty(ms_strTranslationName))
    {
        float fEntityCount = (float) m_pmSelectedEntities->EntityCount;

        NiPoint3 kNewCenter;
        m_pmCenter->ToNiPoint3(kNewCenter);

        MPoint3* pmEntityTranslation = dynamic_cast<MPoint3*>(pmEntity->
            GetPropertyData(ms_strTranslationName));
        NiPoint3 kEntityTranslation;
        pmEntityTranslation->ToNiPoint3(kEntityTranslation);

        kNewCenter -= (1.0f / (fEntityCount + 1.0f)) * kEntityTranslation;
        kNewCenter *= ((fEntityCount + 1.0f) / fEntityCount);
        m_pmCenter->SetData(kNewCenter);
    }
}
//---------------------------------------------------------------------------
void MSelectionService::RebuildHashtable()
{
    MVerifyValidInstance;

    m_pmEntityHashtable->Clear();

    MEntity* amSelectedEntities[] = m_pmSelectedEntities->GetEntities();
    for (int i = 0; i < amSelectedEntities->Length; i++)
    {
        MEntity* pmSelectedEntity = amSelectedEntities[i];
        m_pmEntityHashtable->Add(pmSelectedEntity, NULL);
    }
}
//---------------------------------------------------------------------------
// IService members.
//---------------------------------------------------------------------------
String* MSelectionService::get_Name()
{
    MVerifyValidInstance;

    return "Selection Service";
}
//---------------------------------------------------------------------------
bool MSelectionService::Initialize()
{
    MVerifyValidInstance;

    return true;
}
//---------------------------------------------------------------------------
bool MSelectionService::Start()
{
    MVerifyValidInstance;

    IUICommandService* pmUICommandService = MGetService(IUICommandService);
    MAssert(pmUICommandService != NULL, "UI Command service not found!");
    pmUICommandService->BindCommands(this);

    return true;
}
//---------------------------------------------------------------------------
// ISelectionService members.
//---------------------------------------------------------------------------
void MSelectionService::AddEntitiesToSelection(MEntity* amEntities[])
{
    MVerifyValidInstance;

    MAssert(amEntities != NULL, "Null entity array provided to function!");

    if (amEntities->Length > 0)
    {
        String* strCommandName = String::Format("Selection: Add {0} entities "
            "to main selection", amEntities->Length.ToString());

        const char* pcCommandName = MStringToCharPointer(strCommandName);
        NiSelectEntitiesCommand* pkCommand = NiNew NiSelectEntitiesCommand(
            pcCommandName, m_pmSelectedEntities->GetNiEntitySelectionSet(),
            true, amEntities->Length);
        MFreeCharPointer(pcCommandName);

        int iEntitiesAdded = 0;

        for (int i = 0; i < amEntities->Length; i++)
        {
            MEntity* pmEntity = amEntities[i];
            if (pmEntity->Frozen || pmEntity->Hidden)
            {
                String* strStateText = pmEntity->Frozen ?
                    "frozen" : "hidden";
                MessageService->AddMessage(MessageChannelType::General,
                    String::Format("Entity '{0}' is {1} and can't be selected",
                    pmEntity->Name, strStateText));
                continue;
            }
            if (!m_pmEntityHashtable->Contains(pmEntity))
            {
                iEntitiesAdded++;
                m_pmEntityHashtable->Add(pmEntity, NULL);
                pkCommand->AddEntityToArray(pmEntity->GetNiEntityInterface());
            }
        }

        if (iEntitiesAdded > 0)
        {
            m_bProcessSelectedEntitiesChanged = false;       
            CommandService->ExecuteCommand(
                new MSelectEntitiesCommand(pkCommand), m_bCommandsAreUndoable);
            m_bProcessSelectedEntitiesChanged = true;

            if (!MFramework::Instance->PerformingLongOperation)
            {
                RecalculateSelectionCenter();
            }
            else
            {
                m_bRecalculateSelectionCenter = true;
            }
        }
    }
}
//---------------------------------------------------------------------------
void MSelectionService::AddEntityToSelection(MEntity* pmEntity)
{
    MVerifyValidInstance;

    MAssert(pmEntity != NULL, "Null entity provided to function!");

    CommandService->BeginUndoFrame(String::Format("Selection: Add \"{0}\" "
        "entity to main selection", pmEntity->Name));

    MEntity* amEntities[] = new MEntity*[1];
    amEntities[0] = pmEntity;
    AddEntitiesToSelection(amEntities);

    CommandService->EndUndoFrame(m_bCommandsAreUndoable);
}
//---------------------------------------------------------------------------
void MSelectionService::AddSelectionSetToSelection(
    MSelectionSet* pmSelectionSet)
{
    MVerifyValidInstance;

    MAssert(pmSelectionSet != NULL, "Null selection set provided to "
        "function!");

    CommandService->BeginUndoFrame(String::Format("Selection: Add \"{0}\" "
        "selection set to main selection", pmSelectionSet->Name));

    AddEntitiesToSelection(pmSelectionSet->GetEntities());

    CommandService->EndUndoFrame(m_bCommandsAreUndoable);
}
//---------------------------------------------------------------------------
void MSelectionService::RemoveEntitiesFromSelection(MEntity* amEntities[])
{
    MVerifyValidInstance;

    MAssert(amEntities != NULL, "Null entity array provided to function!");

    if (amEntities->Length > 0)
    {
        String* strCommandName = String::Format("Selection: Remove {0} "
            "entities from main selection", amEntities->Length.ToString());

        const char* pcCommandName = MStringToCharPointer(strCommandName);
        NiSelectEntitiesCommand* pkCommand = NiNew NiSelectEntitiesCommand(
            pcCommandName, m_pmSelectedEntities->GetNiEntitySelectionSet(),
            false, amEntities->Length);
        MFreeCharPointer(pcCommandName);

        for (int i = 0; i < amEntities->Length; i++)
        {
            MEntity* pmEntity = amEntities[i];
            if (m_pmEntityHashtable->Contains(pmEntity))
            {
                m_pmEntityHashtable->Remove(pmEntity);
                pkCommand->AddEntityToArray(pmEntity->GetNiEntityInterface());
            }
        }

        m_bProcessSelectedEntitiesChanged = false;
        CommandService->ExecuteCommand(new MSelectEntitiesCommand(pkCommand),
            m_bCommandsAreUndoable);
        m_bProcessSelectedEntitiesChanged = true;

        if (!MFramework::Instance->PerformingLongOperation)
        {
            RecalculateSelectionCenter();
        }
        else
        {
            m_bRecalculateSelectionCenter = true;
        }
    }
}
//---------------------------------------------------------------------------
void MSelectionService::RemoveEntityFromSelection(MEntity* pmEntity)
{
    MVerifyValidInstance;

    MAssert(pmEntity != NULL, "Null entity provided to function!");

    CommandService->BeginUndoFrame(String::Format("Selection: Remove \"{0}\" "
        "entity from main selection", pmEntity->Name));

    MEntity* amEntities[] = new MEntity*[1];
    amEntities[0] = pmEntity;
    RemoveEntitiesFromSelection(amEntities);

    CommandService->EndUndoFrame(m_bCommandsAreUndoable);
}
//---------------------------------------------------------------------------
void MSelectionService::RemoveSelectionSetFromSelection(
    MSelectionSet* pmSelectionSet)
{
    MVerifyValidInstance;

    MAssert(pmSelectionSet != NULL, "Null selection set provided to "
        "function!");

    CommandService->BeginUndoFrame(String::Format("Selection: Remove \"{0}\" "
        "selection set from main selection", pmSelectionSet->Name));

    RemoveEntitiesFromSelection(pmSelectionSet->GetEntities());

    CommandService->EndUndoFrame(m_bCommandsAreUndoable);
}
//---------------------------------------------------------------------------
void MSelectionService::ReplaceSelection(MEntity* amEntities[])
{
    MVerifyValidInstance;

    MAssert(amEntities != NULL, "Null entity array provided to function!");

    CommandService->BeginUndoFrame(String::Format("Selection: Replace main "
        "selection with {0} entities", amEntities->Length.ToString()));

    ClearSelectedEntities();
    AddEntitiesToSelection(amEntities);

    CommandService->EndUndoFrame(m_bCommandsAreUndoable);
}
//---------------------------------------------------------------------------
void MSelectionService::ReplaceSelection(MSelectionSet* pmSelectionSet)
{
    MVerifyValidInstance;

    CommandService->BeginUndoFrame(String::Format("Selection: Replace main "
        "selection with \"{0}\" selection set", pmSelectionSet->Name));

    ClearSelectedEntities();
    AddSelectionSetToSelection(pmSelectionSet);

    CommandService->EndUndoFrame(m_bCommandsAreUndoable);
}
//---------------------------------------------------------------------------
void MSelectionService::ClearSelectedEntities()
{
    MVerifyValidInstance;

    if (m_pmSelectedEntities->EntityCount == 0)
    {
        return;
    }

    MEntity* amEntities[] = m_pmSelectedEntities->GetEntities();

    NiSelectEntitiesCommand* pkCommand = NiNew NiSelectEntitiesCommand(
        "Selection: Clear main selection",
        m_pmSelectedEntities->GetNiEntitySelectionSet(), false,
        amEntities->Length);

    for (int i = 0; i < amEntities->Length; i++)
    {
        pkCommand->AddEntityToArray(amEntities[i]->GetNiEntityInterface());
    }

    m_pmEntityHashtable->Clear();

    m_bProcessSelectedEntitiesChanged = false;
    CommandService->ExecuteCommand(new MSelectEntitiesCommand(pkCommand),
        m_bCommandsAreUndoable);
    m_bProcessSelectedEntitiesChanged = true;

    if (!MFramework::Instance->PerformingLongOperation)
    {
        RecalculateSelectionCenter();
    }
    else
    {
        m_bRecalculateSelectionCenter = true;
    }
}
//---------------------------------------------------------------------------
int MSelectionService::get_NumSelectedEntities()
{
    MVerifyValidInstance;

    return m_pmSelectedEntities->EntityCount;
}
//---------------------------------------------------------------------------
MEntity* MSelectionService::GetSelectedEntities()[]
{
    MVerifyValidInstance;

    return m_pmSelectedEntities->GetEntities();
}
//---------------------------------------------------------------------------
bool MSelectionService::get_CommandsAreUndoable()
{
    MVerifyValidInstance;

    return m_bCommandsAreUndoable;
}
//---------------------------------------------------------------------------
MEntity* MSelectionService::get_SelectedEntity()
{
    MVerifyValidInstance;

    if (m_pmSelectedEntities->EntityCount == 1)
    {
        MEntity* amEntities[] = m_pmSelectedEntities->GetEntities();
        return amEntities[0];
    }
    else
    {
        return NULL;
    }
}
//---------------------------------------------------------------------------
void MSelectionService::set_SelectedEntity(MEntity* pmEntity)
{
    MVerifyValidInstance;

    if (pmEntity != NULL)
    {
        CommandService->BeginUndoFrame(String::Format("Selection: Replace "
            "main selection with \"{0}\" entity", pmEntity->Name));

        ClearSelectedEntities();
        AddEntityToSelection(pmEntity);

        CommandService->EndUndoFrame(m_bCommandsAreUndoable);
    }
    else
    {
        ClearSelectedEntities();
    }
}
//---------------------------------------------------------------------------
MPoint3* MSelectionService::get_SelectionCenter()
{
    MVerifyValidInstance;
    
    return m_pmCenter;
}
//---------------------------------------------------------------------------
void MSelectionService::set_SelectionCenter(MPoint3* pmCenter)
{
    MVerifyValidInstance;

    m_pmCenter = pmCenter;
}
//---------------------------------------------------------------------------
bool MSelectionService::IsEntitySelected(MEntity* pmEntity)
{
    MVerifyValidInstance;

    return m_pmEntityHashtable->Contains(pmEntity);
}
//---------------------------------------------------------------------------
