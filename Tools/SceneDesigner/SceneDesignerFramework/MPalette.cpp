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
#include "SceneDesignerFrameworkPCH.h"

#include "MPalette.h"
#include "MSceneFactory.h"
#include "MEntityFactory.h"
#include "MEventManager.h"
#include "ServiceProvider.h"
#include "MUtility.h"

using namespace Emergent::Gamebryo::SceneDesigner::Framework;
using namespace System::Text::RegularExpressions;
using namespace System::IO;

//---------------------------------------------------------------------------
MPalette::MPalette(MScene* pmScene) :
    m_pmSentinel(new FileSystemWatcher()),
    m_bWritable(true)
{
    m_pmScene = pmScene;

    CreateFileWatcher();

    RegisterEventHandlers();
}
//---------------------------------------------------------------------------
MPalette::MPalette(String* strName, unsigned int uiInitialNumEntities) :
    m_pmSentinel(new FileSystemWatcher()),
    m_bWritable(true)
{
    const char* pcName = MStringToCharPointer(strName);
    NiScene* pkScene = NiNew NiScene(pcName, uiInitialNumEntities);
    MFreeCharPointer(pcName);
    m_pmScene = MSceneFactory::Instance->Get(pkScene);

    CreateFileWatcher();

    RegisterEventHandlers();
}
//---------------------------------------------------------------------------
void MPalette::Do_Dispose(bool bDisposing)
{
    if (bDisposing)
    {
        UnregisterEventHandlers();

        m_pmSentinel->EnableRaisingEvents = false;
        m_pmSentinel->Changed -= new FileSystemEventHandler(this,
            &MPalette::OnFileStatusChanged);
        m_pmSentinel = NULL;

        if (m_pmScene)
        {
            MScene* pmScene = MSceneFactory::Instance->Get(
                m_pmScene->GetNiScene());
            MAssert(pmScene == m_pmScene, "Palette scene does not match "
                "scene returned from factory!");
            MSceneFactory::Instance->Remove(pmScene);
        }
    }
}
//---------------------------------------------------------------------------
void MPalette::RegisterEventHandlers()
{
    MVerifyValidInstance;

    __hook(&MEventManager::PaletteWritableStatusChanged,
        MEventManager::Instance, &MPalette::OnPaletteWritableStatusChanged);
}
//---------------------------------------------------------------------------
void MPalette::UnregisterEventHandlers()
{
    MVerifyValidInstance;

    __unhook(&MEventManager::PaletteWritableStatusChanged,
        MEventManager::Instance, &MPalette::OnPaletteWritableStatusChanged);
}
//---------------------------------------------------------------------------
ICommandService* MPalette::get_CommandService()
{
    if (ms_pmCommandService == NULL)
    {
        ms_pmCommandService = MGetService(ICommandService);
        MAssert(ms_pmCommandService != NULL, "Command service not found!");
    }
    return ms_pmCommandService;
}
//---------------------------------------------------------------------------
String* MPalette::get_Name()
{
    MVerifyValidInstance;

    return m_pmScene->Name;
}
//---------------------------------------------------------------------------
void MPalette::set_Name(String* strName)
{
    MVerifyValidInstance;

    m_pmScene->Name = strName;
}
//---------------------------------------------------------------------------
MScene* MPalette::get_Scene()
{
    MVerifyValidInstance;

    return m_pmScene;
}
//---------------------------------------------------------------------------
unsigned int MPalette::get_EntityCount()
{
    MVerifyValidInstance;

    return m_pmScene->EntityCount;
}
//---------------------------------------------------------------------------
bool MPalette::get_DontSave()
{
    return m_bDontSave;
}
//---------------------------------------------------------------------------
void MPalette::set_DontSave(bool bDontSave)
{
    m_bDontSave = bDontSave;
}
//---------------------------------------------------------------------------
bool MPalette::get_Dirty()
{
    MVerifyValidInstance;

    // Search all entities in the scene for dirty flags.
    bool bDirty = m_pmScene->Dirty;
    if (!bDirty)
    {
        MEntity* amEntities[] = m_pmScene->GetEntities();
        for (int i = 0; i < amEntities->Length; ++i)
        {
            if (amEntities[i]->Dirty)
            {
                bDirty = true;
                break;
            }
        }
    }

    return bDirty;
}
//---------------------------------------------------------------------------
void MPalette::set_Dirty(bool bDirty)
{
    MVerifyValidInstance;

    m_pmScene->Dirty = bDirty;
}
//---------------------------------------------------------------------------
bool MPalette::get_Writable()
{
    MVerifyValidInstance;

    return m_bWritable;
}
//---------------------------------------------------------------------------
MEntity* MPalette::GetEntities()[]
{
    MVerifyValidInstance;

    return m_pmScene->GetEntities();
}
//---------------------------------------------------------------------------
MEntity* MPalette::GetEntityByName(String* strName)
{
    MVerifyValidInstance;

    return m_pmScene->GetEntityByName(strName);
}
//---------------------------------------------------------------------------
bool MPalette::AddEntity(MEntity* pmEntity, String* strCategory,
    bool bUndoable)
{
    MVerifyValidInstance;

    MAssert(pmEntity != NULL, "Null entity provided to function!");

    String* strFullName;
    if (strCategory->Equals(String::Empty))
    {
        strFullName = pmEntity->Name;
    }
    else
    {
        strFullName = String::Format("{0}.{1}", strCategory, pmEntity->Name);
    }

    strFullName = m_pmScene->GetUniqueEntityName(strFullName);
        
    if (m_pmScene->GetEntityByID(pmEntity->ID) == NULL)
    {
        CommandService->BeginUndoFrame(String::Format("Add \"{0}\" entity to "
            "\"{1}\" palette", pmEntity->Name, this->Name));

        pmEntity->Writable = true;
        pmEntity->Name = strFullName;
        bool bSuccess = m_pmScene->AddEntity(pmEntity, true);

        CommandService->EndUndoFrame(bUndoable);
        return bSuccess;
    }

    return false;
}
//---------------------------------------------------------------------------
void MPalette::RemoveEntity(MEntity* pmEntity, bool bUndoable)
{
    MVerifyValidInstance;

    MAssert(pmEntity != NULL, "Null entity provided to function!");

    CommandService->BeginUndoFrame(String::Format("Remove \"{0}\" entity "
        "from \"{1}\" palette", pmEntity->Name, this->Name));

    m_pmScene->RemoveEntity(pmEntity, true);
    CommandService->EndUndoFrame(bUndoable);
}
//---------------------------------------------------------------------------
void MPalette::RemoveAllEntities(bool bUndoable)
{
    MVerifyValidInstance;

    CommandService->BeginUndoFrame(String::Format("Remove all entities from "
        "\"{0}\" palette", this->Name));

    m_pmScene->RemoveAllEntities(bUndoable);
    CommandService->EndUndoFrame(bUndoable);
}
//---------------------------------------------------------------------------
void MPalette::CopyComponentProperty(MComponent* pmSource, 
   String* strPropertyName, MComponent* pmDestination)
{
    PropertyType* pmPropertyType = pmSource->GetPropertyType(
        strPropertyName);
    String* strDisplayName = pmSource->GetPropertyDisplayName(
        strPropertyName);
    String* strPrimitiveType = pmPropertyType->PrimitiveType;
    String* strSemanticType = pmPropertyType->Name;
    bool bCollection = pmSource->IsCollection(strPropertyName);
    String* strDescription = pmSource->GetDescription(
        strPropertyName);
    pmDestination->AddProperty(strPropertyName, strDisplayName,
        strPrimitiveType, strSemanticType, bCollection, strDescription, true);
    if (pmDestination->CanResetProperty(strPropertyName))
    {
        pmDestination->ResetProperty(strPropertyName);
    }
}
//---------------------------------------------------------------------------
ArrayList* MPalette::GetAffectedEntities(MEntity* pmTemplateEntity, 
    MScene* pmScenes[])
{
    ArrayList* pmEntityList = new ArrayList();
    unsigned int uiSceneCount = pmScenes->Count;
    for (unsigned int ui = 0; ui < uiSceneCount; ui++)
    {
        MScene* pmScene = pmScenes[ui];
        MEntity* pmDependentEntities[] = 
            pmScene->GetDependentEntities(pmTemplateEntity);
        pmEntityList->AddRange(static_cast<Array*>(pmDependentEntities));
    }
    return pmEntityList;
}
//---------------------------------------------------------------------------
void MPalette::AddComponentPropertyToEntities(MComponent* pmComponent,
    String* strPropertyName, MScene* pmAffectedScenes[])
{
    MEntity* pmEntities[] = m_pmScene->GetEntities();
    unsigned int uiEntityCount = pmEntities->Count;
    for (unsigned int ui = 0; ui < uiEntityCount; ui++)
    {
        MEntity* pmEntity = pmEntities[ui];
        MComponent* pmEntityComponent = pmEntity->GetComponentByTemplateID(
            pmComponent->TemplateID);
        if (pmEntityComponent != NULL)
        {
            CopyComponentProperty(pmComponent, strPropertyName, 
                pmEntityComponent);
            pmEntity->Dirty = true;
            ArrayList* pmAffectedEntities = GetAffectedEntities(pmEntity,
                pmAffectedScenes);
            
            unsigned int pmAffectedEntityCount = pmAffectedEntities->Count;

            for ( unsigned int uiIndex = 0; uiIndex < pmAffectedEntityCount;
                uiIndex++)
            {
                MEntity* pmAffectedEntity = 
                    dynamic_cast<MEntity*>
                    (pmAffectedEntities->get_Item(uiIndex));
                MComponent* pmDependentEntityComponent =
                    pmAffectedEntity->GetComponentByTemplateID(
                    pmComponent->TemplateID);
                if (pmDependentEntityComponent != NULL)
                {
                    CopyComponentProperty(pmComponent, strPropertyName, 
                        pmDependentEntityComponent);
                    pmAffectedEntity->Dirty = true;
                }
            }
        }
    }
}

//---------------------------------------------------------------------------
void MPalette::RemoveComponentPropertyFromEntities(MComponent* pmComponent,
    String* strPropertyName, MScene* pmAffectedScenes[])
{
    MEntity* pmEntities[] = m_pmScene->GetEntities();
    unsigned int uiEntityCount = pmEntities->Count;
    for (unsigned int ui = 0; ui < uiEntityCount; ui++)
    {
        MEntity* pmEntity = pmEntities[ui];
        MComponent* pmEntityComponent = pmEntity->GetComponentByTemplateID(
            pmComponent->TemplateID);
        if (pmEntityComponent != NULL &&
            pmEntityComponent->HasProperty(strPropertyName))
        {
            pmEntity->Dirty = true;
            pmEntityComponent->RemoveProperty(strPropertyName, true);
            ArrayList* pmAffectedEntities = GetAffectedEntities(pmEntity,
                pmAffectedScenes);
            
            unsigned int pmAffectedEntityCount = pmAffectedEntities->Count;

            for ( unsigned int uiIndex = 0; uiIndex < pmAffectedEntityCount;
                uiIndex++)
            {
                MEntity* pmAffectedEntity = 
                    dynamic_cast<MEntity*>
                    (pmAffectedEntities->get_Item(uiIndex));
                MComponent* pmDependentEntityComponent =
                    pmAffectedEntity->GetComponentByTemplateID(
                    pmComponent->TemplateID);
                if (pmDependentEntityComponent != NULL &&
                    pmDependentEntityComponent->HasProperty(strPropertyName))
                {
                    pmDependentEntityComponent->RemoveProperty(
                        strPropertyName, true);
                    pmAffectedEntity->Dirty = true;
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void MPalette::ReplaceComponentPropertyOnEntities(MComponent* pmComponent,
    String* strOldPropertyName, String* strNewPropertyName,
    MScene* pmAffectedScenes[])
{
    PropertyType* pmNewPropertyType = pmComponent->GetPropertyType(
        strNewPropertyName);
    bool bNewIsCollection = pmComponent->IsCollection(strNewPropertyName);

    MEntity* pmEntities[] = m_pmScene->GetEntities();
    unsigned int uiEntityCount = pmEntities->Count;
    for (unsigned int ui = 0; ui < uiEntityCount; ui++)
    {
        MEntity* pmEntity = pmEntities[ui];
        MComponent* pmEntityComponent = pmEntity->GetComponentByTemplateID(
            pmComponent->TemplateID);
        if (pmEntityComponent != NULL)
        {
            MAssert(pmEntityComponent->HasProperty(strOldPropertyName),
                "Template does not have old property!");
            MAssert(pmEntityComponent->IsPropertyUnique(strOldPropertyName),
                "Template property not unique!");

            pmEntity->Dirty = true;

            PropertyType* pmOldPropertyType = pmEntityComponent
                ->GetPropertyType(strOldPropertyName);

            // Backup old property values.
            unsigned int uiOldCount = pmEntity->GetElementCount(
                strOldPropertyName);
            Object* amOldValues[] = new Object*[uiOldCount];
            for (unsigned int uiTemp = 0; uiTemp < uiOldCount; uiTemp++)
            {
                amOldValues[uiTemp] = pmEntity->GetPropertyData(
                    strOldPropertyName, uiTemp);
            }

            // Clear old property values (to support undo).
            for (unsigned int uiTemp = uiOldCount; uiTemp > 0; uiTemp--)
            {
                pmEntity->SetPropertyData(strOldPropertyName, NULL, uiTemp - 1,
                    true);
            }

            // Remove old property.
            pmEntityComponent->RemoveProperty(strOldPropertyName, true);

            // Add new property.
            CopyComponentProperty(pmComponent, strNewPropertyName,
                pmEntityComponent);

            // Restore property values.
            if (pmOldPropertyType == pmNewPropertyType)
            {
                // Always set value at index 0.
                pmEntity->SetPropertyData(strNewPropertyName, amOldValues[0],
                    0, true);

                // Only set remaining values if the new property is a
                // collection.
                if (bNewIsCollection)
                {
                    for (unsigned int uiTemp = 1; uiTemp < uiOldCount;
                        uiTemp++)
                    {
                        pmEntity->SetPropertyData(strNewPropertyName,
                            amOldValues[uiTemp], uiTemp, true);
                    }
                }
            }

            // Iterate over affected entities in scenes.
            ArrayList* pmAffectedEntities = GetAffectedEntities(pmEntity,
                pmAffectedScenes);
            unsigned int pmAffectedEntityCount = pmAffectedEntities->Count;
            for (unsigned int uiIndex = 0; uiIndex < pmAffectedEntityCount;
                uiIndex++)
            {
                MEntity* pmAffectedEntity = dynamic_cast<MEntity*>(
                    pmAffectedEntities->Item[uiIndex]);
                MComponent* pmAffectedEntityComponent =
                    pmAffectedEntity->GetComponentByTemplateID(
                    pmComponent->TemplateID);
                if (pmAffectedEntityComponent != NULL &&
                    pmAffectedEntityComponent->HasProperty(
                        strOldPropertyName) &&
                    pmAffectedEntityComponent->IsPropertyUnique(
                        strOldPropertyName))
                {
                    pmAffectedEntity->Dirty = true;

                    pmOldPropertyType = pmAffectedEntityComponent
                        ->GetPropertyType(strOldPropertyName);

                    // Backup old property values.
                    uiOldCount = pmAffectedEntity->GetElementCount(
                        strOldPropertyName);
                    amOldValues = new Object*[uiOldCount];
                    for (unsigned int uiTemp = 0; uiTemp < uiOldCount;
                        uiTemp++)
                    {
                        amOldValues[uiTemp] = pmAffectedEntity
                            ->GetPropertyData(strOldPropertyName, uiTemp);
                    }

                    // Clear old property values (to support undo).
                    for (unsigned int uiTemp = uiOldCount; uiTemp > 0;
                        uiTemp--)
                    {
                        pmAffectedEntity->SetPropertyData(strOldPropertyName,
                            NULL, uiTemp - 1, true);
                    }

                    // Remove old property.
                    if (pmAffectedEntityComponent->CanResetProperty(
                            strOldPropertyName))
                    {
                        pmAffectedEntityComponent->ResetProperty(
                            strOldPropertyName);
                    }
                    else
                    {
                        pmAffectedEntityComponent->RemoveProperty(
                            strOldPropertyName, true);
                    }

                    // Add new property.
                    if (pmAffectedEntityComponent->HasProperty(
                        strNewPropertyName))
                    {
                        pmAffectedEntityComponent->MakePropertyUnique(
                            strNewPropertyName);
                    }
                    else
                    {
                        CopyComponentProperty(pmComponent, strNewPropertyName,
                            pmAffectedEntityComponent);
                    }

                    // Restore property values.
                    if (pmOldPropertyType == pmNewPropertyType)
                    {
                        // Always set value at index 0.
                        pmAffectedEntity->SetPropertyData(strNewPropertyName,
                            amOldValues[0], 0, true);

                        // Only set remaining values if the new property is a
                        // collection.
                        if (bNewIsCollection)
                        {
                            for (unsigned int uiTemp = 1; uiTemp < uiOldCount;
                                uiTemp++)
                            {
                                pmAffectedEntity->SetPropertyData(
                                    strNewPropertyName, amOldValues[uiTemp],
                                    uiTemp, true);
                            }
                        }
                    }
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
String* MPalette::GetEntityCategory(MEntity* pmEntity)
{
    MVerifyValidInstance;


    String* strFullName = StripPaletteName(pmEntity->Name);
    int iLastDotIndex = strFullName->LastIndexOf(".");
    if (iLastDotIndex == -1)
    {
        return String::Empty;
    }
    String* strCategoryName = strFullName->Substring(0, 
        iLastDotIndex);
    return strCategoryName;
}
//---------------------------------------------------------------------------
void MPalette::ResetEntityNames()
{
    MVerifyValidInstance;

    MEntity* pmFlatEntityArray[] = Scene->GetEntities();

    int iSize = pmFlatEntityArray->Count;
    String* strIdentifierString = String::Format("[{0}]",
        Name);

    CommandService->BeginUndoFrame("Reset template names");

    for (int i = 0; i < iSize; i++)
    {
        MEntity* pmEntity = pmFlatEntityArray[i];
        if (pmEntity->Name->StartsWith(strIdentifierString))
            continue;
        String* strNonPaletteName = MPalette::StripPaletteName(pmEntity->Name);
        String* newName = String::Format("{0}{1}", strIdentifierString,
            strNonPaletteName);
        pmEntity->Name = newName;
    }
    CommandService->EndUndoFrame(false);

}
//---------------------------------------------------------------------------
MEntity* MPalette::get_ActiveEntity()
{
    MVerifyValidInstance;

    return m_pmActiveEntity;
}
//---------------------------------------------------------------------------
void MPalette::set_ActiveEntity(MEntity* pmActiveEntity)
{
    MVerifyValidInstance;

    if (m_pmActiveEntity != pmActiveEntity)
    {
        MEntity* pmOldActiveEntity = m_pmActiveEntity;
        if (
            (pmActiveEntity == NULL) || 
            m_pmScene->IsEntityInScene(pmActiveEntity)            
            )
        {
            m_pmActiveEntity = pmActiveEntity;
            MEventManager::Instance->RaisePaletteActiveEntityChanged(this,
                pmOldActiveEntity);
        }
    }
}
//---------------------------------------------------------------------------
MEntity* MPalette::CreateTemplateFromFile(String* strFileName)
{
    MAssert(strFileName != NULL && strFileName != String::Empty, "Null or "
        "empty filename provided to function!");

    // Get filename.
    const char* pcFileName = MStringToCharPointer(strFileName);

    // Get base name.
    const char* pcBaseName = strrchr(pcFileName, '\\') + 1;

    // Copy base name to a new buffer.
    size_t stBufferSize = strlen(pcBaseName) + 1;
    char* pcEntityName = NiAlloc(char, stBufferSize);
    NiStrcpy(pcEntityName, stBufferSize, pcBaseName);

    // Clear off extension from base name.
    char* pcPtr = strrchr(pcEntityName, '.');
    *pcPtr = '\0';

    // Create new entity with name constructed above.
    NiUniqueID kTemplateID;
    MUtility::GuidToID(Guid::NewGuid(), kTemplateID);
    NiEntityInterfaceIPtr spEntity = NiNew NiGeneralEntity(pcEntityName,
        kTemplateID, 2);

    // Free entity name buffer.
    NiFree(pcEntityName);

    // Add transformation component.
    spEntity->AddComponent(NiNew NiTransformationComponent());

    // Check file extension and add additional components.
    const char* pcExtension = strrchr(pcBaseName, '.') + 1;
    bool bAddedComponent = false;
    if (_stricmp(pcExtension, "nif") == 0)
    {
        NiSceneGraphComponent* pkSceneGraphComponent = NiNew
            NiSceneGraphComponent();
        pkSceneGraphComponent->SetNifFilePath(pcFileName);
        spEntity->AddComponent(pkSceneGraphComponent);
        bAddedComponent = true;
    }
    else if (_stricmp(pcExtension, "kfm") == 0)
    {
        NiActorComponent* pkActorComponent = NiNew NiActorComponent();
        pkActorComponent->SetKfmFilePath(pcFileName);
        spEntity->AddComponent(pkActorComponent);
        bAddedComponent = true;
    }

    // Free the filename pointer.
    MFreeCharPointer(pcFileName);

    // If no component was added, the file format is unknown. The entity
    // cannot be created.
    if (!bAddedComponent)
    {
        return NULL;
    }

    // Return the managed entity.
    return MEntityFactory::Instance->Get(spEntity);
}
//---------------------------------------------------------------------------
String* MPalette::StripPaletteName(String* strName)
{
    Regex* regex = new Regex(S"[\\[][^\\]]+[\\]]");
    Match* match = regex->Match(strName);
    if (match->Success)
    {
        return strName->Substring(match->Length);
    }
    else
    {
        return strName;
    }
}

//---------------------------------------------------------------------------
void MPalette::CreateFileWatcher()
{
    MVerifyValidInstance;

    // Assign a synchronizing object (the main form) to the watcher so
    // it will marshall all events through the same thread that created
    // the UI. This prevents asynchronous access from other threads.
    if (Application::OpenForms->Count > 0)
    {
        m_pmSentinel->SynchronizingObject = Application::OpenForms->Item[0];
    }

    if (!String::IsNullOrEmpty(m_pmScene->SourceFilename))
    {
        // Initialize a FileSystemWatcher to watch this file.
        FileInfo* pmInfo = new FileInfo(m_pmScene->SourceFilename);

        m_pmSentinel->Path = pmInfo->DirectoryName;
        m_pmSentinel->Filter = pmInfo->Name;

        // Watch for changes in attributes.
        m_pmSentinel->NotifyFilter = NotifyFilters::Attributes;

        // Add event handlers.
        m_pmSentinel->Changed += new FileSystemEventHandler(this,
            &MPalette::OnFileStatusChanged);

        // Begin watching.
        m_pmSentinel->EnableRaisingEvents = true;

        // Get the writable status.
        m_bWritable = !(pmInfo->Exists && pmInfo->IsReadOnly);
        OnPaletteWritableStatusChanged(this);
    }
    else
    {
        // If the palette has not been saved (i.e., has no name), mark it
        // as writable.
        m_bWritable = true;
    }
}
//---------------------------------------------------------------------------
void MPalette::OnFileStatusChanged(Object*, FileSystemEventArgs*)
{
    MVerifyValidInstance;

    // Our attributes changed, so we need to notify our observers that our
    // writable status may have changed.
    MAssert(!String::IsNullOrEmpty(m_pmScene->SourceFilename));
    FileInfo* pmInfo = new FileInfo(m_pmScene->SourceFilename);
    MAssert(pmInfo->Exists);
    bool bWritable = !pmInfo->IsReadOnly;
    if (m_bWritable != bWritable)
    {
        m_bWritable = bWritable;
        MEventManager::Instance->RaisePaletteWritableStatusChanged(this);
    }
}
//---------------------------------------------------------------------------
void MPalette::OnPaletteWritableStatusChanged(MPalette* pmPalette)
{
    MVerifyValidInstance;

    if (pmPalette == this)
    {
        MEntity* amEntities[] = m_pmScene->GetEntities();
        for (int i = 0; i < amEntities->Length; ++i)
        {
            amEntities[i]->Writable = m_bWritable;
        }
    }
}
//---------------------------------------------------------------------------
