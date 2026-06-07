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

#include "MFramework.h"
#include "MLayerManager.h"
#include "MPrefabManager.h"
#include "NiLightProxyComponent.h"
#include "ServiceProvider.h"
#include "MPoint3.h"
#include "MUtility.h"
#include "MFolderLocation.h"

//The following is related to a bug in VC7.1/.Net CLR 1.1
// see http://support.microsoft.com/?id=814472
#if _MSC_VER == 1310
#pragma unmanaged
#include <windows.h>
#include <_vcclrit.h>
#pragma managed
#endif

using namespace System::Windows::Forms;
using namespace Emergent::Gamebryo::SceneDesigner::Framework;
//---------------------------------------------------------------------------
void MFramework::Init()
{
#if _MSC_VER == 1310
    __crt_dll_initialize();
#endif
    if (ms_pmThis == NULL)
    {
        ms_pmThis = new MFramework();
    }
}
//---------------------------------------------------------------------------
void MFramework::Shutdown()
{
    if (ms_pmThis != NULL)
    {
        ClearStream();
        ms_pmThis->Dispose();
        ms_pmThis = NULL;
    }
#if _MSC_VER == 1310
    __crt_dll_terminate();
#endif
}
//---------------------------------------------------------------------------
bool MFramework::InstanceIsValid()
{
    return (ms_pmThis != NULL);
}
//---------------------------------------------------------------------------
MFramework* MFramework::get_Instance()
{
    return ms_pmThis;
}
//---------------------------------------------------------------------------
MFramework::MFramework() : m_strImageSubfolder(String::Empty),
    m_bDoNotUpdate(false), m_bInitSuccessful(false), m_uiLongOperationCount(0),
    m_strCurrentFilename(String::Empty)
{
    NiInitOptions* pkInitOptions = NiExternalNew NiInitOptions(
#ifdef NI_MEMORY_DEBUGGER
        NiExternalNew NiMemTracker(NiExternalNew NiStandardAllocator(), false)
#else
        NiExternalNew NiStandardAllocator()
#endif
        );

    NiInit(pkInitOptions);
    m_bInitSuccessful = true;

    // Initialize the shadow manager. It is initially set to be inactive.
    // Rendering modes that draw shadows are responsible for activating it
    // and then deactivating when done. Rendering modes that do not draw
    // shadows need not take any action.
    NiShadowManager::Initialize();
    NiShadowManager::SetActive(false);
    NiShadowManager::SetCullingProcess(NiNew NiMeshCullingProcess(NULL, NULL));

    NiProxyComponent::_SDMInit();
    NiLightProxyComponent::_SDMInit();
    MViewport::_SDMInit();
    MEntity::_SDMInit();

    MSceneFactory::Init();
    MEntityFactory::Init();
    MSelectionSetFactory::Init();
    MComponentFactory::Init();

    MEventManager::Init();
    MRenderer::Init();
    MTimeManager::Init();
    MCameraManager::Init();
    MViewportManager::Init();
    MPaletteManager::Init();
    MProxyManager::Init();
    MLightManager::Init();
    MBoundManager::Init();
    MLayerManager::Init(MEventManager::Instance);
    MPrefabManager::Init();
    MTagsManager::Init();
    MFilterManager::Init();

    // Ensure that all registered assets clone from a pristine copy
    NiTFactory<NiExternalAssetHandler*>* pkAssetFactory = 
        NiFactories::GetAssetFactory();
    NiTMapIterator kIter = pkAssetFactory->GetFirstPos();
    while (kIter)
    {
        NiFixedString kKey;
        pkAssetFactory->GetNext(kIter, kKey);

        NiExternalAssetHandler* pkHandler = 
            pkAssetFactory->GetPersistent(kKey);

        NIVERIFY(pkHandler->SetCloneFromPristine(true));
    }

    m_pmPickUtility = new MPickUtility();

    m_pkAssetManager = NiNew NiExternalAssetManager(
        NiFactories::GetAssetFactory());

    // To make clear that error handling, if used, will be set explicitly.
    m_pkAssetManager->SetErrorHandler(NULL);

    MInitRefObject(m_pkAssetManager);

    Scenes* pmScenes = NewScene(10);
    InitNewScene(pmScenes, false);

    this->TimeManager->Enabled = true;
}
//---------------------------------------------------------------------------
void MFramework::Do_Dispose(bool bDisposing)
{
    if (!m_bInitSuccessful)
    {
        return;
    }

    MDisposeRefObject(m_pkAssetManager);

    if (bDisposing)
    {
        MProxyManager::Shutdown();

        if (m_pmPickUtility != NULL)
        {
            m_pmPickUtility->Dispose();
            m_pmPickUtility = NULL;
        }

        MFilterManager::Shutdown();
        MTagsManager::Shutdown();
        MLightManager::Shutdown();
        MPaletteManager::Shutdown();
        MViewportManager::Shutdown();
        MCameraManager::Shutdown();
        MTimeManager::Shutdown();
        MBoundManager::Shutdown();
        MLayerManager::Shutdown();
        MPrefabManager::Shutdown();

        // The factories are shut down after the managers because some of the
        // managers access them in their Dispose functions. The factories must
        // also be shutdown in the order listed here because some factories
        // depend on other factories.
        MSceneFactory::Shutdown();
        MEntityFactory::Shutdown();
        MSelectionSetFactory::Shutdown();
        MComponentFactory::Shutdown();

        // The shadow manager must be shut down prior to the renderer being
        // destroyed.
        NiShadowManager::Shutdown();

        MRenderer::Shutdown();
        MEventManager::Shutdown();
    }

    NiLightProxyComponent::_SDMShutdown();
    NiProxyComponent::_SDMShutdown();
    MEntity::_SDMShutdown();
    MViewport::_SDMShutdown();

    const NiInitOptions* pkInitOptions = NiStaticDataManager
        ::GetInitOptions();
    NiShutdown();
    NiAllocator* pkAllocator = pkInitOptions->GetAllocator();
    NiExternalDelete pkInitOptions;
    NiExternalDelete pkAllocator;
}
//---------------------------------------------------------------------------
void MFramework::Startup()
{
    MVerifyValidInstance;

    IUICommandService* pmCommand = MGetService(IUICommandService);
    pmCommand->BindCommands(MViewportManager::Instance);
    pmCommand->BindCommands(MCameraManager::Instance);

    this->ProxyManager->Startup();
    this->LightManager->Startup();
    this->CameraManager->Startup();
    this->ViewportManager->Startup();
    this->BoundManager->Startup();

    RegisterSettingsAndOptions();
}
//---------------------------------------------------------------------------
void MFramework::RegisterSettingsAndOptions()
{
    MVerifyValidInstance;

    SettingsService->RegisterSettingsObject(ms_strImageSubfolderSettingName,
        new MFolderLocation(m_strImageSubfolder), SettingsCategory::PerScene);
    SettingsService->SetChangedSettingHandler(ms_strImageSubfolderSettingName,
        SettingsCategory::PerScene, new SettingChangedHandler(this,
        &MFramework::OnImageSubfolderSettingChanged));
    OnImageSubfolderSettingChanged(NULL, NULL);

    OptionsService->AddOption(ms_strImageSubfolderOptionName,
        SettingsCategory::PerScene, ms_strImageSubfolderSettingName);
    OptionsService->SetHelpDescription(ms_strImageSubfolderOptionName,
        "The directory to use when loading external images referenced by NIF "
        "files. A blank path indicates that the same directory as the NIF "
        "file should be used.\n\nNote: this option is stored per scene and "
        "does not take effect until the scene is saved and reloaded.");
}
//---------------------------------------------------------------------------
void MFramework::OnImageSubfolderSettingChanged(Object*,
    SettingChangedEventArgs*)
{
    MVerifyValidInstance;

    MFolderLocation* pmImageSubfolder = dynamic_cast<MFolderLocation*>(
        SettingsService->GetSettingsObject(ms_strImageSubfolderSettingName,
        SettingsCategory::PerScene));
    if (pmImageSubfolder != NULL)
    {
        // Store new image subfolder.
        m_strImageSubfolder = pmImageSubfolder->Path;

        // Set platform-specific subdirectory with new image subfolder.
        const char* pcImageSubfolder = MStringToCharPointer(
            m_strImageSubfolder);
        NiDevImageConverter::SetPlatformSpecificSubdirectory(
            pcImageSubfolder);
        MFreeCharPointer(pcImageSubfolder);
    }
}
//---------------------------------------------------------------------------
MSceneFactory* MFramework::get_SceneFactory()
{
    MVerifyValidInstance;

    return MSceneFactory::Instance;
}
//---------------------------------------------------------------------------
MEntityFactory* MFramework::get_EntityFactory()
{
    MVerifyValidInstance;

    return MEntityFactory::Instance;
}
//---------------------------------------------------------------------------
MSelectionSetFactory* MFramework::get_SelectionSetFactory()
{
    MVerifyValidInstance;

    return MSelectionSetFactory::Instance;
}
//---------------------------------------------------------------------------
MComponentFactory* MFramework::get_ComponentFactory()
{
    MVerifyValidInstance;

    return MComponentFactory::Instance;
}
//---------------------------------------------------------------------------
MRenderer* MFramework::get_Renderer()
{
    MVerifyValidInstance;

    return MRenderer::Instance;
}
//---------------------------------------------------------------------------
MEventManager* MFramework::get_EventManager()
{
    MVerifyValidInstance;

    return MEventManager::Instance;
}
//---------------------------------------------------------------------------
MTimeManager* MFramework::get_TimeManager()
{
    MVerifyValidInstance;

    return MTimeManager::Instance;
}
//---------------------------------------------------------------------------
MCameraManager* MFramework::get_CameraManager()
{
    MVerifyValidInstance;

    return MCameraManager::Instance;
}
//---------------------------------------------------------------------------
MViewportManager* MFramework::get_ViewportManager()
{
    MVerifyValidInstance;

    return MViewportManager::Instance;
}
//---------------------------------------------------------------------------
MPaletteManager* MFramework::get_PaletteManager()
{
    MVerifyValidInstance;

    return MPaletteManager::Instance;
}
//---------------------------------------------------------------------------
MProxyManager* MFramework::get_ProxyManager()
{
    MVerifyValidInstance;

    return MProxyManager::Instance;
}
//---------------------------------------------------------------------------
MLightManager* MFramework::get_LightManager()
{
    MVerifyValidInstance;

    return MLightManager::Instance;
}
//---------------------------------------------------------------------------
MBoundManager* MFramework::get_BoundManager()
{
    MVerifyValidInstance;

    return MBoundManager::Instance;
}
//---------------------------------------------------------------------------
MLayerManager* MFramework::get_LayerManager()
{
    MVerifyValidInstance;

    return MLayerManager::Instance;
}
//---------------------------------------------------------------------------
MPrefabManager* MFramework::get_PrefabManager()
{
    MVerifyValidInstance;

    return MPrefabManager::Instance;
}
//---------------------------------------------------------------------------
MTagsManager* MFramework::get_TagsManager()
{
    MVerifyValidInstance;

    return MTagsManager::Instance;
}
//---------------------------------------------------------------------------
MFilterManager* MFramework::get_FilterManager()
{
    MVerifyValidInstance;

    return MFilterManager::Instance;
}
//---------------------------------------------------------------------------
String* MFramework::get_AppStartupPath()
{
    MVerifyValidInstance;

    return String::Format(S"{0}{1}",Application::StartupPath, S"\\");
}
//---------------------------------------------------------------------------
MScene* MFramework::get_Scene()
{
    MVerifyValidInstance;

    return m_pmScene;
}
//---------------------------------------------------------------------------
MPickUtility* MFramework::get_PickUtility()
{
    MVerifyValidInstance;

    return m_pmPickUtility;
}
//---------------------------------------------------------------------------
NiExternalAssetManager* MFramework::get_ExternalAssetManager()
{
    MVerifyValidInstance;

    return m_pkAssetManager;
}
//---------------------------------------------------------------------------
String* MFramework::get_ApplicationName()
{
    MVerifyValidInstance;

    return Application::ProductName;
}
//---------------------------------------------------------------------------
String* MFramework::get_ImageSubfolder()
{
    MVerifyValidInstance;

    return m_strImageSubfolder;
}
//---------------------------------------------------------------------------
void MFramework::ClearStream()
{
    // Clear out our stream
    NiTObjectArray<NiFixedString> kKeys;
    NiFactories::GetStreamingFactory()->GetKeys(kKeys);
    const unsigned int uiKeyCount = kKeys.GetSize();
    for (unsigned int ui = 0; ui < uiKeyCount; ++ui)
    {
        NiEntityStreaming* pkEntityStreaming =
            NiFactories::GetStreamingFactory()->GetPersistent(kKeys.GetAt(ui));
        if (pkEntityStreaming)
        {
            pkEntityStreaming->Reset();
        }
    }
}
//---------------------------------------------------------------------------
MFramework::Scenes* MFramework::NewScene(unsigned int uiEntityArraySize)
{
    MVerifyValidInstance;

    ClearStream();

    Scenes* scenes = new Scenes;
    scenes->Add(SceneFactory->Get(NiNew NiScene("Main Scene",
        uiEntityArraySize)));
    return scenes;
}
//---------------------------------------------------------------------------
MFramework::Scenes* MFramework::LoadScene(
    String* strFilename,
    String* strFormat)
{
    return LoadScene(strFilename, strFormat, true);
}
//---------------------------------------------------------------------------
MFramework::Scenes* MFramework::LoadScene(
    String* strFilename,
    String* strFormat,
    bool bResetStreamingObject)
{
    MVerifyValidInstance;

    // Create the appropriate file format handler based on strFormat
    const char* pcFormat = MStringToCharPointer(strFormat);
    NiEntityStreaming* pkEntityStreaming =
        NiFactories::GetStreamingFactory()->GetPersistent(pcFormat);
    MFreeCharPointer(pcFormat);

    if (bResetStreamingObject)
    {
        // Clear out our stream
        pkEntityStreaming->Reset();
    }

    // Create error handler.
    NiDefaultErrorHandlerPtr spErrors = NiNew NiDefaultErrorHandler();
    pkEntityStreaming->SetErrorHandler(spErrors);

    // Check for an autosave file.
    bool bAutoSave = false;
    String* strOriginalFilename = strFilename;
    String* strAutoSaveFilename = String::Concat(strFilename, ".autosave");
    FileInfo* pmAutoSaveFile = new FileInfo(strAutoSaveFilename);
    if (pmAutoSaveFile->Exists)
    {
        if (MessageBox::Show(String::Format("It appears that the following "
            "file was not properly saved:\n\n{0}\n\nDo you want to attempt "
            "to restore the backed up version?", strFilename),
            "AutoSave File Found", MessageBoxButtons::YesNo,
            MessageBoxIcon::Question) == DialogResult::Yes)
        {
            strFilename = strAutoSaveFilename;
            bAutoSave = true;
        }
        else
        {
            // Delete autosave and backup files for the main scene file.
            pmAutoSaveFile->IsReadOnly = false;
            pmAutoSaveFile->Delete();
            EventManager->RaiseDeletedAutoSave(strFilename);

            FileInfo* pmBackupFile = new FileInfo(String::Concat(strFilename,
                ".backup"));
            if (pmBackupFile->Exists)
            {
                pmBackupFile->IsReadOnly = false;
                pmBackupFile->Delete();
            }
        }
    }

    // Load the scene
    const char* pcFilename = MStringToCharPointer(strFilename);
    NiBool bSuccess = pkEntityStreaming->Load(pcFilename, false, true);
    MFreeCharPointer(pcFilename);

    // Report errors.
    bool bErrorsInLoading = (spErrors->GetErrorCount() > 0);
    // Send messages to Error Interface, but do not specify that they
    // are errors so that the utility can figure it out.  This is because
    // some of the messages may be conflicts while others may be errors.
    MUtility::AddErrorInterfaceMessages(spErrors);
    pkEntityStreaming->SetErrorHandler(NULL);

    // Return if loading failure.
    if (!bSuccess)
    {
        return NULL;
    }

    // Detect all layer autosave files.
    List<String*>* pmAutoSaveFiles = new List<String*>();
    const unsigned int uiNumScenes = pkEntityStreaming->GetSceneCount();
    for (unsigned int ui = 1; ui < uiNumScenes; ++ui)
    {
        NiScene* pkScene = pkEntityStreaming->GetSceneAt(ui);
        String* strDependentFile = pkScene->GetSourceFilename();
        String* strDependentAutoSave = String::Concat(strDependentFile,
            ".autosave");
        pmAutoSaveFile = new FileInfo(strDependentAutoSave);
        if (pmAutoSaveFile->Exists)
        {
            pmAutoSaveFiles->Add(strDependentFile);
        }
    }

    // If any layer autosave files exist, prompt the user to restore them.
    if (pmAutoSaveFiles->Count > 0)
    {
        String* strMessage = "The following layer files were not properly "
            "saved:\n\n";
        List<String*>::Enumerator mEnum = pmAutoSaveFiles->GetEnumerator();
        while (mEnum.MoveNext())
        {
            String* strAutoSaveFile = mEnum.Current;
            strMessage = String::Concat(strMessage, strAutoSaveFile, "\n");
        }
        strMessage = String::Concat(strMessage, "\nDo you want to restore "
            "these files?");
        if (MessageBox::Show(strMessage, "AutoSave Files Found",
            MessageBoxButtons::YesNo, MessageBoxIcon::Question) ==
            DialogResult::Yes)
        {
            // Backup and restore layer files.
            for (unsigned int ui = 1; ui < uiNumScenes; ++ui)
            {
                NiScene* pkScene = pkEntityStreaming->GetSceneAt(ui);
                String* strDependentFile = pkScene->GetSourceFilename();
                String* strDependentAutoSave = String::Concat(strDependentFile,
                    ".autosave");
                pmAutoSaveFile = new FileInfo(strDependentAutoSave);
                if (pmAutoSaveFile->Exists)
                {
                    FileInfo* pmDependentFile = new FileInfo(strDependentFile);
                    if (pmDependentFile->Exists)
                    {
                        pmDependentFile->IsReadOnly = false;
                        pmDependentFile->CopyTo(String::Concat(
                            strDependentFile, ".backup"), true);
                    }

                    pmAutoSaveFile->IsReadOnly = false;
                    pmAutoSaveFile->CopyTo(strDependentFile, true);
                }
            }

            bAutoSave = true;
        }
        else
        {
            // Delete autosave and backup files for layers.
            for (unsigned int ui = 1; ui < uiNumScenes; ++ui)
            {
                NiScene* pkScene = pkEntityStreaming->GetSceneAt(ui);
                String* strDependentFile = pkScene->GetSourceFilename();
                String* strDependentAutoSave = String::Concat(strDependentFile,
                    ".autosave");
                String* strDependentBackup = String::Concat(strDependentFile,
                    ".backup");
                pmAutoSaveFile = new FileInfo(strDependentAutoSave);
                if (pmAutoSaveFile->Exists)
                {
                    pmAutoSaveFile->IsReadOnly = false;
                    pmAutoSaveFile->Delete();
                    EventManager->RaiseDeletedAutoSave(strDependentFile);

                    FileInfo* pmBackupFile = new FileInfo(strDependentBackup);
                    if (pmBackupFile->Exists)
                    {
                        pmBackupFile->IsReadOnly = false;
                        pmBackupFile->Delete();
                    }
                }
            }
        }
    }

    if (!strFilename->Equals(strOriginalFilename))
    {
        // Backup and restore main scene file.
        pmAutoSaveFile = new FileInfo(strAutoSaveFilename);
        if (pmAutoSaveFile->Exists)
        {
            FileInfo* pmMainFile = new FileInfo(strOriginalFilename);
            if (pmMainFile->Exists)
            {
                pmMainFile->IsReadOnly = false;
                pmMainFile->CopyTo(String::Concat(strOriginalFilename,
                    ".backup"), true);
            }

            pmAutoSaveFile->IsReadOnly = false;
            pmAutoSaveFile->CopyTo(strOriginalFilename, true);
        }

        strFilename = strOriginalFilename;
    }

    if (bAutoSave)
    {
        // Clear out the streaming object.
        for (unsigned int ui = 0; ui < uiNumScenes; ++ui)
        {
            NiScene* pkScene = pkEntityStreaming->GetSceneAt(ui);
            pkEntityStreaming->RemoveLoadedScene(pkScene->GetSourceFilename());
        }

        // Clear out the error handler.
        spErrors->ClearErrors();
        pkEntityStreaming->SetErrorHandler(spErrors);

        // Re-load the scene
        const char* pcFilename = MStringToCharPointer(strFilename);
        bSuccess = pkEntityStreaming->Load(pcFilename, false, true);
        MFreeCharPointer(pcFilename);

        // Report errors.
        bErrorsInLoading = (spErrors->GetErrorCount() > 0);
        MUtility::AddErrorInterfaceMessages(spErrors);
        pkEntityStreaming->SetErrorHandler(NULL);

        if (!bSuccess)
        {
            return NULL;
        }
    }

    Scenes* scenes = new Scenes;

    // Collect all loaded scenes.
    bool bSceneFilesConverted = false;
    const int numScenes = pkEntityStreaming->GetSceneCount();
    List<String*>* pmROFilesWithMods = new List<String*>();
    for (int i = 0; i != numScenes; ++i)
    {
        NiScene* scene = pkEntityStreaming->GetSceneAt(i);
        MScene* pmScene = MSceneFactory::Instance->Get(scene);
        scenes->Add(pmScene);
        String* strBackupFilename = String::Concat(
            new String(scene->GetSourceFilename()), ".backup");
        FileInfo* pmBackupFile = new FileInfo(strBackupFilename);

        bool bHasBeenConverted = NIBOOL_IS_TRUE(
            pkEntityStreaming->HasBeenConverted(pmScene->GetNiScene()));
        bSceneFilesConverted = bSceneFilesConverted || bHasBeenConverted;

        if (bHasBeenConverted ||
            pkEntityStreaming->HasBeenModified(pmScene->GetNiScene()) ||
            pmBackupFile->Exists)
        {
            pmScene->Dirty = true;
            FileInfo* fileInfo = new FileInfo(scene->GetSourceFilename());
            if (fileInfo->IsReadOnly)
            {
                pmROFilesWithMods->Add(scene->GetSourceFilename());
            }
        }
    }

    // Find the palette directory
    MPaletteManager* pkPaletteManager = MPaletteManager::Instance;
    const int numLoadedScenes = pkEntityStreaming->GetLoadedSceneCount();
    for (int i = 0; i != numLoadedScenes; i++)
    {
        NiScene* pkScene = pkEntityStreaming->GetLoadedSceneAt(i);
        String* strFileName = pkScene->GetSourceFilename();
        if (strFileName->EndsWith(pkPaletteManager->PaletteFileExtension))
        {
            int iSlashIndex = strFileName->LastIndexOf(NI_PATH_DELIMITER_STR);
            String* strPath = strFileName->Substring(0, iSlashIndex+1);
            pkPaletteManager->PaletteFolder = strPath;
            break;
        }
    }

    pkEntityStreaming->RemoveAllScenes(); // Because static instance

    String* alertMsg = "";
    if (pmROFilesWithMods->Count > 0)
    {
        alertMsg = "\n\nNote that the following read-only files have "
            "been modified:\n";
        List<String*>::Enumerator mEnum = pmROFilesWithMods->GetEnumerator(); 
        while (mEnum.MoveNext())
        {
            String* roFile = mEnum.Current;
            alertMsg = String::Concat(alertMsg, "\n", roFile);
        }        
    }

    if (bErrorsInLoading)
    {
        MessageBox::Show(String::Concat("Errors and/or conflicts occurred "
            "when loading the scene or one of its layers. The scene may\n"
            "have been modified as a result. Check the Messages Panel for "
            "more information\nbefore saving.", alertMsg),
            "Errors/Conflicts Found When Loading Scene", MessageBoxButtons::OK,
            MessageBoxIcon::Information);
    }

    if (bSceneFilesConverted)
    {
        MessageBox::Show(String::Concat("The scene or one of its layers has "
            "been converted from a previous file version.\nThe files should "
            "be saved to retain the changes and prevent mismatches\nbetween "
            "files.", alertMsg),
            "Scene Files Converted", MessageBoxButtons::OK,
            MessageBoxIcon::Information);
    }

    return scenes;
}
//---------------------------------------------------------------------------
void MFramework::InitNewScene(Scenes* pmNewScene)
{
    MVerifyValidInstance;

    InitNewScene(pmNewScene, true);
}
//---------------------------------------------------------------------------
void MFramework::InitNewScene(Scenes* pmNewScenes, bool bRaiseEvent)
{
    MVerifyValidInstance;

    MAssert(pmNewScenes != NULL, "Null scene provided to function!");

    m_bDoNotUpdate = true;
    BeginLongOperation();

    if (m_pmScene != NULL)
    {
        // change the active layer to NULL so that event comes before
        // the scene closing event does
        LayerManager->SetActiveLayer(NULL, bRaiseEvent);
        this->EventManager->RaiseSceneClosing(m_pmScene);
        LayerManager->RemoveAllLayers();
        MSceneFactory::Instance->Remove(m_pmScene);
        m_pmScene = NULL;
    }
    m_pkAssetManager->RemoveAll();
    this->TimeManager->ResetTime(0.0f);

    // Total up all entities in all scenes so we can size our uber scene 
    // correctly.
    int totalEntities = 0;
    const int numScenes = pmNewScenes->Count;
    for (int i = 0; i != numScenes; ++i)
    {
        totalEntities += pmNewScenes->Item[i]->EntityCount;
    }

    NiScene* uberScene = NiNew NiScene("Main Scene", totalEntities);
    m_pmScene = SceneFactory->Get(uberScene);

    BeginLongOperation();

    for (int i = 0; i != numScenes; ++i)
    {
        LayerManager->AddLayer(
            new MLayer(pmNewScenes->Item[i], uberScene), NULL,
            MLayerManager::DisableEvents);
    }

    EndLongOperation();

    m_pmScene->Update(TimeManager->CurrentTime, m_pkAssetManager);

    if (bRaiseEvent)
    {
        this->EventManager->RaiseNewSceneLoaded(m_pmScene);
    }

    EndLongOperation();
    m_bDoNotUpdate = false;
}
//---------------------------------------------------------------------------
bool MFramework::SaveScene(
    String* strFilename,
    String*,
    bool bAutoSave)
{
    MVerifyValidInstance;

    if (!m_pmScene)
        return false;

    if (!m_pmScene->GetNiScene())
        return false;

    m_bDoNotUpdate = true;

    bool bSuccess = LayerManager->SaveAll(strFilename, bAutoSave);

    m_bDoNotUpdate = false;

    return bSuccess;
}
//---------------------------------------------------------------------------
void MFramework::Update()
{
    MVerifyValidInstance;

    if (m_bDoNotUpdate)
    {
        return;
    }

    m_bDoNotUpdate = true;

    this->TimeManager->UpdateTime();
    PerformUpdate();
    this->Renderer->Render();

    m_bDoNotUpdate = false;
}
//---------------------------------------------------------------------------
void MFramework::PerformUpdate()
{
    MVerifyValidInstance;

    // Continuous time never pauses and is used to update UI elements. Current
    // time is used to update the main scene.
    float fContinuousTime = this->TimeManager->ContinuousTime;
    float fCurrentTime = this->TimeManager->CurrentTime;

    InteractionModeService->Update(fContinuousTime);
    LightManager->Update(fContinuousTime);
    CameraManager->Update(fContinuousTime);

    if (m_pmScene != NULL)
    {
        m_pmScene->Update(fCurrentTime, m_pkAssetManager);
    }
    if (this->ProxyManager->ProxyScene != NULL)
    {
        this->ProxyManager->ProxyScene->Update(fContinuousTime,
            m_pkAssetManager);
    }
    for (unsigned int ui = 0; ui < this->ViewportManager->ViewportCount; ui++)
    {
        this->ViewportManager->GetViewport(ui)->ToolScene->Update(
            fContinuousTime, m_pkAssetManager);
    }

    BoundManager->Update(fContinuousTime);
}
//---------------------------------------------------------------------------
bool MFramework::get_PerformingLongOperation()
{
    MVerifyValidInstance;

    return (m_uiLongOperationCount > 0);
}
//---------------------------------------------------------------------------
void MFramework::BeginLongOperation()
{
    MVerifyValidInstance;

    if (m_uiLongOperationCount == 0)
    {
        this->EventManager->RaiseLongOperationStarted();
    }
    m_uiLongOperationCount++;
}
//---------------------------------------------------------------------------
void MFramework::EndLongOperation()
{
    MVerifyValidInstance;

    if (m_uiLongOperationCount > 0)
    {
        m_uiLongOperationCount--;

        if (m_uiLongOperationCount == 0)
        {
            this->EventManager->RaiseLongOperationCompleted();
        }
    }
}
//---------------------------------------------------------------------------
void MFramework::RestartAnimation()
{
    MEntity* pmEntities[] = m_pmScene->GetEntities();
    int iEntityCount = pmEntities->Count;
    NiFixedString kActorClassName = "NiActorComponent";
    for (int iEntityIndex = 0; iEntityIndex < iEntityCount; iEntityIndex++)
    {
        MComponent* pmComponents[] = pmEntities[iEntityIndex]->GetComponents();
        int iComponentCount = pmComponents->Count;
        for (int iComponentIndex = 0; iComponentIndex < iComponentCount; 
            iComponentIndex++)
        {
            MComponent* pmComponent = pmComponents[iComponentIndex];
            NiEntityComponentInterface* pkComponent = 
                pmComponent->GetNiEntityComponentInterface();
            NiFixedString kClassName = pkComponent->GetClassName();
            if (kClassName == kActorClassName)
            {
                NiActorComponent* pkActor = static_cast<NiActorComponent*>
                    (pkComponent);
                pkActor->ResetAnimation();
            }            
        }
    }
}
//---------------------------------------------------------------------------
String* MFramework::get_CurrentFilename()
{
    MLayer* defLayer = LayerManager->DefaultLayer;
    if (defLayer != NULL)
    {
        return defLayer->Name;
    }

    return String::Empty;
}
//---------------------------------------------------------------------------
IInteractionModeService* MFramework::get_InteractionModeService()
{
    if (ms_pmInteractionModeService == NULL)
    {
        ms_pmInteractionModeService = MGetService(IInteractionModeService);
        MAssert(ms_pmInteractionModeService != NULL, "Interaction mode "
            "service not found!");
    }
    return ms_pmInteractionModeService;
}
//---------------------------------------------------------------------------
ISelectionService* MFramework::get_SelectionService()
{
    if (ms_pmSelectionService == NULL)
    {
        ms_pmSelectionService = MGetService(ISelectionService);
        MAssert(ms_pmSelectionService != NULL, "Selection service not "
            "found!");
    }
    return ms_pmSelectionService;
}
//---------------------------------------------------------------------------
ISettingsService* MFramework::get_SettingsService()
{
    if (ms_pmSettingsService == NULL)
    {
        ms_pmSettingsService = MGetService(ISettingsService);
        MAssert(ms_pmSettingsService != NULL, "Settings service not "
            "found!");
    }
    return ms_pmSettingsService;
}
//---------------------------------------------------------------------------
IOptionsService* MFramework::get_OptionsService()
{
    if (ms_pmOptionsService == NULL)
    {
        ms_pmOptionsService = MGetService(IOptionsService);
        MAssert(ms_pmOptionsService != NULL, "Options service not "
            "found!");
    }
    return ms_pmOptionsService;
}
//---------------------------------------------------------------------------
