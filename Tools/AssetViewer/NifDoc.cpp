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

// NifDoc.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NifDoc.h"
#include "FileDialog.h"
#include "RemoveFileDlg.h"
#include "NifUserPreferences.h"
#include "PreferencesDlg.h"
#include "NifPropertyWindowManager.h"
#include "ProgressBarDlg.h"
#include "NifTimeManager.h"
#include "NifAnimationInfo.h"
#include "DiscardOpenFilesDlg.h"
#include <NiOptimizeABV.h>

#include <NiSystem.h>
#include <NiMain.h>
#include <NiSourceTexture.h>
#include <NiPixelData.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CNifDoc* CNifDoc::ms_pkThis = NULL;

//---------------------------------------------------------------------------
// CNifDoc
//---------------------------------------------------------------------------
IMPLEMENT_DYNCREATE(CNifDoc, CDocument)
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNifDoc, CDocument)
    //{{AFX_MSG_MAP(CNifDoc)
    ON_COMMAND(ID_OPEN_FILE, OnOpenFile)
    ON_COMMAND(ID_ADD_FILE, OnAddFile)
    ON_COMMAND(ID_REMOVE_FILE, OnRemoveFile)
    ON_UPDATE_COMMAND_UI(ID_REMOVE_FILE, OnUpdateRemoveFile)
    ON_COMMAND(ID_PREFERENCES, OnPreferences)
    ON_COMMAND(ID_SET_IMAGE_SUBFOLDER, OnSetImageSubfolder)
    ON_COMMAND(ID_LIST_OPTIONS, OnListOptions)
    ON_COMMAND(ID_TOGGLE_ABVS, OnToggleABVs)
    ON_UPDATE_COMMAND_UI(ID_TOGGLE_ABVS, OnUpdateToggleABVs)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNifDoc construction/destruction
//---------------------------------------------------------------------------
CNifDoc::CNifDoc()
    :
    m_kScreenElements(5, 5)
{
    m_bWorking = false;
    m_ulThreadId = 0;
    m_spScene = 0;
    m_pkDefaultNode = NULL;
    m_uiNumRoots = 0;
    m_pkCameraList = NULL;
    m_pkTimeManager = NULL;
    m_fLastUpdateTime = -1.0f;
    m_pkAnimInfo = NULL;
    ms_pkThis = this;
    m_spBoundShape = 0;
    m_pkBoundShapeRoot = NULL;
    m_pkCollisionLabGrp = NULL;
    m_fMinStartTime = NI_INFINITY;
    m_bRendererCreated = false;
    m_bRendererCreationFailed = false;
}
//---------------------------------------------------------------------------
CNifDoc::~CNifDoc()
{
    CNifUserPreferences::Lock();
    CNifUserPreferences::Save();
    CNifUserPreferences::UnLock();

    if(m_ulThreadId != NULL)
    {
        ::MessageBox(NULL, "Someone is holding onto the Critical Section at "
            "~CNifDoc()\n", "Threading Error", MB_OK | MB_ICONERROR);
    }
    ms_pkThis = NULL;
    NiImageConverter::SetPlatformSpecificSubdirectory(NULL);

    delete m_pkCollisionLabGrp;
}
//---------------------------------------------------------------------------
// CNifDoc serialization
//---------------------------------------------------------------------------
void CNifDoc::Serialize(CArchive& ar)
{
    // if additional saving/loading code is needed, it should be here
}
//---------------------------------------------------------------------------
// CNifDoc diagnostics
//---------------------------------------------------------------------------
#ifdef _DEBUG
void CNifDoc::AssertValid() const
{
    CDocument::AssertValid();
}
//---------------------------------------------------------------------------
void CNifDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG
//---------------------------------------------------------------------------
// CNifDoc commands
//---------------------------------------------------------------------------
// Thread access methods.
//---------------------------------------------------------------------------
void CNifDoc::Lock()
{
    m_bWorking = m_kCriticalSection.Lock() ? true : false;
    m_ulThreadId = GetCurrentThreadId();
}
//---------------------------------------------------------------------------
void CNifDoc::UnLock(bool bSceneChanged)
{
    ASSERT(m_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == m_ulThreadId);
    if(m_kCriticalSection.m_sect.LockCount == 0)
        m_ulThreadId = NULL;
    m_bWorking = !(m_kCriticalSection.Unlock() ? true : false);
    NIASSERT(!m_bWorking);
    if(bSceneChanged)
        UpdateAllViews(NULL, MAKELPARAM(NIF_SCENECHANGED, 0), NULL);
}
//---------------------------------------------------------------------------
// Member data accessors.
//---------------------------------------------------------------------------
NiNode* CNifDoc::GetSceneGraph()
{
    ASSERT(m_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == m_ulThreadId);
    return m_spScene;
}
//---------------------------------------------------------------------------
NiNode* CNifDoc::GetDefaultNode()
{
    ASSERT(m_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == m_ulThreadId);
    return m_pkDefaultNode;
}
//---------------------------------------------------------------------------
bool CNifDoc::IsAvailableNow()
{
    return !m_bWorking;
}
//---------------------------------------------------------------------------
void CNifDoc::SetRendererCreated(bool bCreated)
{
    m_bRendererCreated = bCreated;
}
//---------------------------------------------------------------------------
void CNifDoc::SetRendererCreationFailed(bool bFailed)
{
    m_bRendererCreationFailed = bFailed;
}
//---------------------------------------------------------------------------
// Creation and deletion.
//---------------------------------------------------------------------------
void CNifDoc::OnCloseDocument() 
{
    if (m_spScene)
    {
        DestroyScene();
    }

    CDocument::OnCloseDocument();
}
//---------------------------------------------------------------------------
void CNifDoc::DeleteContents() 
{
    if (m_spScene)
    {
        DestroyScene();
    }
    CNifStatisticsManager::Lock();
    CNifStatisticsManager::AccessStatisticsManager()->ResetTimers();
    CNifStatisticsManager::UnLock();

    CDocument::DeleteContents();
}
//---------------------------------------------------------------------------
void CNifDoc::CreateScene()
{
    Lock();

    NiImageConverter::SetPlatformSpecificSubdirectory(
        "");

    CreateBoundNode();

    m_kFileStream.RemoveAllObjects();
    m_pkCameraList = NiTCreate1<CNifCameraList, CNifDoc*>(this);
    m_pkTimeManager = NiTCreate<CNifTimeManager>();
    m_fLastUpdateTime = -1.0f;
    m_pkAnimInfo = NiTCreate<CNifAnimationInfo>();

    m_spScene = NiTCreate<NiNode>();
    m_spScene->SetSelectiveUpdate(true);
    m_spScene->SetSelectiveUpdateTransforms(true);
    m_spScene->SetSelectiveUpdatePropertyControllers(true);
    m_spScene->SetSelectiveUpdateRigid(false);
    m_spScene->SetName("AssetViewer Root");
    
    NiCamera* pkCamera = NiTCreate<NiCamera>();
    NiFrustum kFrustum = pkCamera->GetViewFrustum();
    pkCamera->SetMinNearPlaneDist(0.01f);
    pkCamera->SetMaxFarNearRatio(16384);
    kFrustum.m_fNear = 0.1f;
    kFrustum.m_fFar = 5.0f;
    pkCamera->SetViewFrustum(kFrustum);
    pkCamera->SetName(NiCameraInfo::ms_kDefaultCameraName);
    pkCamera->SetAppCulled(true);
    m_pkDefaultNode = NiTCreate<NiNode>();
    m_pkDefaultNode->SetName("Default Node");
    m_pkDefaultNode->AttachChild(pkCamera, true);
    m_pkDefaultNode->SetSelectiveUpdate(true);
    m_pkDefaultNode->SetSelectiveUpdateTransforms(true);
    m_pkDefaultNode->SetSelectiveUpdatePropertyControllers(true);
    m_pkDefaultNode->SetSelectiveUpdateRigid(false);
    AddRoot(m_pkDefaultNode);
    m_pkTimeManager->SetTimeMode(CNifTimeManager::LOOP);

    m_pkBoundShapeRoot = NiTCreate<NiNode>();
    m_pkBoundShapeRoot->SetName((const char*)g_strBoundShapeRootName);
    m_pkDefaultNode->AttachChild(m_pkBoundShapeRoot);

    m_spScene->Update(0.0f);
    NiMesh::CompleteSceneModifiers(m_spScene);
    m_spScene->UpdateProperties();
    m_spScene->UpdateEffects();
    m_spScene->UpdateNodeBound();

    m_pkCameraList->AddCamerasForRoot(m_pkDefaultNode);

    // Empty any shadow maps that may be left around from a previous scene.
    if (NiShadowManager::GetShadowManager())
        NiShadowManager::GetShadowManager()->DestroyAllShadowMaps();

    UpdateAllViews(NULL, MAKELPARAM(NIF_CREATESCENE, 0), NULL);

    UnLock();
}
//---------------------------------------------------------------------------
void CNifDoc::DestroyScene()
{
    Lock();

    m_kFileStream.SetTexturePalette(NiTCreate<NiDefaultTexturePalette>());

    m_kCollisionLabInfo.ResetCollisionIndicator(false);
    m_kCollisionLabInfo.ResetCollisionIndicator(true);

    NiDelete m_pkCollisionLabGrp;
    m_pkCollisionLabGrp = NULL;

    NiDelete m_pkCameraList;
    m_pkCameraList = NULL;
    NiDelete m_pkTimeManager;
    m_pkTimeManager = NULL;
    NiDelete m_pkAnimInfo;
    m_pkAnimInfo = NULL;
    m_uiNumRoots = 0;
    m_spScene = 0;
    m_spBoundShape = 0;
    m_kBoundShapeMap.RemoveAll();

    m_kScreenElements.RemoveAll();

    CNifStatisticsManager::Lock();
    CNifStatisticsManager::AccessStatisticsManager()->ResetRoots();
    CNifStatisticsManager::UnLock();

    UpdateAllViews(NULL, MAKELPARAM(NIF_DESTROYSCENE, 0), NULL);   

    UnLock();
}
//---------------------------------------------------------------------------
// Adding and removing files.
//---------------------------------------------------------------------------
BOOL CNifDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    // (SDI documents will reuse this document)
    Lock();

    CreateScene();
    m_spScene->Update(0.0f);
    NiMesh::CompleteSceneModifiers(m_spScene);
    m_spScene->UpdateProperties();
    m_spScene->UpdateEffects();
    m_spScene->UpdateNodeBound();

    UnLock();

    return TRUE;
}
//---------------------------------------------------------------------------
BOOL CNifDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
    // Base class function intentionally not called.
//    if (!CDocument::OnOpenDocument(lpszPathName))
//        return FALSE;

    CNifUserPreferences::Lock();
    bool bPromptOnDiscard = CNifUserPreferences::AccessUserPreferences()
        ->GetPromptOnDiscard();
    CNifUserPreferences::UnLock();

    if (g_bNeedPromptOnDiscard && bPromptOnDiscard && m_uiNumRoots > 1)
    {
        CDiscardOpenFilesDlg dlg;
        if (dlg.DoModal() == IDOK)
        {
            if (dlg.m_bDontPromptAgain)
            {
                CNifUserPreferences::Lock();
                CNifUserPreferences::AccessUserPreferences()
                    ->SetPromptOnDiscard(false);
                CNifUserPreferences::UnLock();
            }
        }
        else
        {
            return FALSE;
        }
    }

    const char* pcExt = strrchr(lpszPathName, '.');
    if (pcExt && toupper(pcExt[1]) == 'N' && toupper(pcExt[2]) == 'I' &&
        toupper(pcExt[3]) == 'F')
    {
        if (!OpenNifFile(lpszPathName))
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
    
    SetModifiedFlag(FALSE);

    return TRUE;
}
//---------------------------------------------------------------------------
void CNifDoc::OnOpenFile() 
{
    CNifUserPreferences::Lock();
    bool bPromptOnDiscard = CNifUserPreferences::AccessUserPreferences()
        ->GetPromptOnDiscard();
    CNifUserPreferences::UnLock();

    if (g_bNeedPromptOnDiscard && bPromptOnDiscard && m_uiNumRoots > 1)
    {
        CDiscardOpenFilesDlg dlg;
        if (dlg.DoModal() == IDOK)
        {
            if (dlg.m_bDontPromptAgain)
            {
                CNifUserPreferences::Lock();
                CNifUserPreferences::AccessUserPreferences()
                    ->SetPromptOnDiscard(false);
                CNifUserPreferences::UnLock();
            }
        }
        else
        {
            return;
        }
    }

    bool bDone = false;
    while (!bDone)
    {
        CString strPathName = FileDialog::GetFilename(
            AfxGetMainWnd()->m_hWnd, "Open File",
            "AssetViewer Files (*.nif)\0*.nif\0\0");
        if (strPathName.IsEmpty())
        {
            bDone = true;
        }
        else
        {
            const char* pcExt = strrchr(strPathName, '.');
            if (toupper(pcExt[1]) == 'N' && toupper(pcExt[2]) == 'I' &&
                toupper(pcExt[3]) == 'F')
            {
                if (OpenNifFile(strPathName))
                {
                    bDone = true;
                    SetPathName(strPathName);
                }
            }
            else
            {
                ::MessageBox(NULL, "Please select a valid "
                    "AssetViewer file type.", "Invalid File Type", MB_OK |
                    MB_ICONERROR);
            }
        }
    }

    SetModifiedFlag(FALSE);
}
//---------------------------------------------------------------------------
void CNifDoc::OnAddFile()
{
    CStringArray astrPathNames;
    if (FileDialog::GetMultipleFilenames(astrPathNames,
        AfxGetMainWnd()->m_hWnd,
        "Add File", "AssetViewer Files (*.nif)\0*.nif\0\0"))
    {
        for (int i = 0; i < astrPathNames.GetSize(); i++)
        {
            CString strPathName = astrPathNames.GetAt(i);

            const char* pcExt = strrchr(strPathName, '.');
            if (toupper(pcExt[1]) == 'N' && toupper(pcExt[2]) == 'I' &&
                toupper(pcExt[3]) == 'F')
            {
                AddNifFile(strPathName);
                SetPathName(strPathName);
            }
            else
            {
                CString strMsg;
                strMsg.Format("%s\nis not a valid AssetViewer file. It will "
                    "not be added.", strPathName);
                ::MessageBox(NULL, strMsg,
                    "Invalid File Type", MB_OK | MB_ICONERROR);
            }
        }
    }
}
//---------------------------------------------------------------------------
bool CNifDoc::OpenNifFile(CString strPathName)
{
    while (!m_bRendererCreated && !m_bRendererCreationFailed)
    {
        Sleep(10);
    }

    Lock();
    DeleteContents();
    CreateScene();
    bool bSuccess = AddNifFile(strPathName);
    UnLock();

    return bSuccess;
}
//---------------------------------------------------------------------------
bool CNifDoc::AddNifFile(CString strPathName)
{
    CNifUserPreferences::Lock();
    CString pcSubfolder = CNifUserPreferences::AccessUserPreferences()
        ->GetImageSubfolderPath();
    NiImageConverter::SetPlatformSpecificSubdirectory(
           pcSubfolder);
    CNifUserPreferences::UnLock();
    
    // Load NIF file.
    CProgressBarDlg* pProgressBar = NiExternalNew CProgressBarDlg;
    pProgressBar->m_strMessage = "Loading NIF File\nPlease Wait...";
    pProgressBar->Create(IDD_PROGRESS_BAR);
    pProgressBar->ShowWindow(SW_SHOW);
    pProgressBar->UpdateWindow();
    pProgressBar->SetRange(0, 1);
    pProgressBar->SetPos(1);
    
    if (!m_kFileStream.Load(strPathName))
    {
        CString strMsg;
        strMsg.Format("Unable to open the specified NIF file: %s. "
            "Please try again.", m_kFileStream.GetLastErrorMessage());
        ::MessageBox(NULL, strMsg,
            "Error Loading NIF File", MB_OK | MB_ICONERROR);
        pProgressBar->DestroyWindow();
        return false;
    }

    if(m_kFileStream.GetObjectCount() == 0)
    {
        ::MessageBox(NULL,
            "This file contains no top-level objects. It will "
            "not be loaded", "Invalid Object Error", MB_OK |
            MB_ICONEXCLAMATION);
        m_kFileStream.RemoveAllObjects();
        pProgressBar->DestroyWindow();
        return false;
    }

    for (unsigned int ui = 0; ui < m_kFileStream.GetObjectCount(); ui++)
    {
        NiObject* pkBaseObj = m_kFileStream.GetObjectAt(ui);
        NiAVObject* pkObject = NiDynamicCast(NiAVObject, pkBaseObj);
        if (pkObject && !pkObject->GetParent())
        {
            AddRootObject(pkObject, strPathName);
            float fStartTime = NiTimeController::GetMinBeginKeyTime(pkObject);
            if (fStartTime < m_fMinStartTime)
            {
                m_fMinStartTime = fStartTime;
            }
        }
        else if(NiIsKindOf(NiPixelData, pkBaseObj))
        {
            NiTexture* pkTexture = (NiTexture*)
                (NiSourceTexture::Create((NiPixelData*)pkBaseObj));

            AddRootTextureObject(pkTexture, strPathName);
        }
        else if(ui == 0)
        {
            ::MessageBox(NULL, "This file contains an invalid top-level "
                "object. The object will not be loaded",
                "Invalid Object Error", MB_OK | MB_ICONEXCLAMATION);
        }
    }

    // Perform particle system run up.
    RunUpScene();

    m_kFileStream.RemoveAllObjects();
    AfxGetApp()->AddToRecentFileList(strPathName);

    pProgressBar->DestroyWindow();

    return true;
}
//---------------------------------------------------------------------------
void CNifDoc::OnUpdateRemoveFile(CCmdUI* pCmdUI) 
{
    if (m_uiNumRoots <= 1)
    {
        pCmdUI->Enable(FALSE);
    }
    else
    {
        pCmdUI->Enable(TRUE);
    }
}
//---------------------------------------------------------------------------
void CNifDoc::OnRemoveFile() 
{
    if (m_uiNumRoots <= 1)
    {
        return;
    }

    Lock();
    NiTStringMap<NiNode*> kNameToRoot;
    for (unsigned int uiRoot = 1; uiRoot < m_uiNumRoots; uiRoot++)
    {
        NiNode* pkRoot = GetRoot(uiRoot);
        const char* pcName = pkRoot->GetName();
        NIASSERT(pcName);
        kNameToRoot.SetAt(pcName, pkRoot);
    }
    UnLock();

    CRemoveFileDlg dlg;
    dlg.m_pkNameToRoot = &kNameToRoot;
    if (dlg.DoModal() == IDOK)
    {
        RemoveFile(dlg.m_pkNodeToRemove);
        if (m_uiNumRoots == 1)
        {
            g_bNeedPromptOnDiscard = false;
        }
    }
}
//---------------------------------------------------------------------------
void CNifDoc::RemoveFileByName(char* pcName)
{
    NiNode* pkNode = 
        (NiNode*)(m_spScene->GetObjectByName(pcName));
    RemoveFile(pkNode);
}
//---------------------------------------------------------------------------
void CNifDoc::RemoveFile(NiNode* pkNode)
{
    if (pkNode == NULL)
        return;

    Lock();

    if (strcmp(pkNode->GetName(),STR_COLLISION_LAB)==0)
    {
        // This is a special node. We will allow it's deletion,
        // but the associated collision groups need to be destroyed
        // also.
        delete m_pkCollisionLabGrp; 
        m_pkCollisionLabGrp = NULL;           
    }

    unsigned int uiIndex = GetRootId(pkNode);
    m_pkCameraList->RemoveCamerasForRoot(pkNode);
    RemoveRoot(pkNode);
    CNifStatisticsManager::Lock();
    CNifStatisticsManager::AccessStatisticsManager()->RemoveRoot(pkNode);
    CNifStatisticsManager::UnLock();

    m_spScene->Update(m_fLastUpdateTime);
    NiMesh::CompleteSceneModifiers(m_spScene);
    m_spScene->UpdateNodeBound();

    UpdateAllViews(NULL, MAKELPARAM(NIF_REMOVENIF, uiIndex), NULL);
    UpdateAllViews(NULL, MAKELPARAM(NIF_SCENECHANGED, 0), NULL);

    CString strAppTitle;
    strAppTitle.LoadString(AFX_IDS_APP_TITLE);
    if (m_uiNumRoots == 1)
    {
        AfxGetMainWnd()->SetWindowText(strAppTitle);
    }
    else if (m_uiNumRoots == 2)
    {
        AfxGetMainWnd()->SetWindowText(strAppTitle + CString(" - ") +
            GetRoot(1)->GetName());
    }

    SetPathName("C:\\", FALSE);

    m_fMinStartTime = NiTimeController::GetMinBeginKeyTime(m_spScene);

    UnLock();
}
//---------------------------------------------------------------------------
// Other document functions.
//---------------------------------------------------------------------------
bool CNifDoc::CreateBoundNode()
{
    if (m_bRendererCreated)
    {
        if (!m_kFileStream.Load(theApp.GetApplicationDirectory() + "..\\" + 
            g_strBoundShapePath))
        {
            CString strMsg;
            strMsg.Format(
                "AssetViewer was unable to load the bounding volume shape "
                "used\nto highlight selected objects. Please ensure that the "
                "following file\nexists in the main application directory:\n"
                "\n%s", g_strBoundShapePath);
            ::MessageBox(NULL, strMsg, "Error Loading Required File",
                MB_OK | MB_ICONERROR);
            m_kFileStream.RemoveAllObjects();
            exit(-1);
        }
        NiNode* pkBoundShapeRoot = NiDynamicCast(NiNode, 
            m_kFileStream.GetObjectAt(0));
        if (pkBoundShapeRoot)
        {
            m_spBoundShape = pkBoundShapeRoot->GetObjectByName("BoundShape");
        }
        if (!m_spBoundShape)
        {
            CString strMsg;
            strMsg.Format("The following required AssetViewer file has been "
                "changed\nand is no longer in the correct format:\n\n%s\n\n"
                "You may need to reinstall the application.",
                g_strBoundShapePath);
            ::MessageBox(NULL, strMsg, "Error Loading Required File",
                MB_OK | MB_ICONERROR);
            m_kFileStream.RemoveAllObjects();
            exit(-1);
        }
        
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------
void CNifDoc::RunUpScene()
{
    Lock();
    float fCurrentTime = m_pkTimeManager->GetCurrentTime();
    if (m_fMinStartTime < fCurrentTime)
    {
        // Update the scene at 30 FPS until the current time.
        for (float fStepTime = m_fMinStartTime; fStepTime < fCurrentTime;
            fStepTime += 0.0333f)
        {
            m_spScene->UpdateSelected(fStepTime);
        }
    }
    UnLock();
}
//---------------------------------------------------------------------------
void CNifDoc::AddRoot(NiNode* pkRoot)
{
    ASSERT(m_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == m_ulThreadId);
    m_spScene->AttachChild(pkRoot);
    m_uiNumRoots++;
    float fEndTime = m_pkAnimInfo->GetMaxTime();
    m_pkAnimInfo->CollectData(m_spScene);

    m_pkTimeManager->SetStartTime(m_pkAnimInfo->GetMinTime());
    m_pkTimeManager->SetEndTime(m_pkAnimInfo->GetMaxTime());
    if (m_pkTimeManager->GetStartTime() == m_pkTimeManager->GetEndTime())
    {
        m_pkTimeManager->SetTimeMode(CNifTimeManager::CONTINUOUS);
    }
    UpdateAllViews(NULL, MAKELPARAM(NIF_TIMINGINFOCHANGED, 0), NULL);
}
//---------------------------------------------------------------------------
void CNifDoc::RemoveRoot(NiNode* pkRoot)
{
    ASSERT(m_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == m_ulThreadId);
    m_spScene->DetachChild(pkRoot);
    m_uiNumRoots--;
    float fEndTime = m_pkAnimInfo->GetMaxTime();
    m_pkAnimInfo->CollectData(m_spScene);
    if(fEndTime == m_pkTimeManager->GetEndTime())
    {
        m_pkTimeManager->SetEndTime(m_pkAnimInfo->GetMaxTime());
        UpdateAllViews(NULL, MAKELPARAM(NIF_TIMINGINFOCHANGED, 0), NULL);
    }
}
//---------------------------------------------------------------------------
float CNifDoc::UpdateScene(bool bForceUpdate)
{
    ASSERT(m_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == m_ulThreadId);
    float fTime = 0.0f;
    if(m_spScene && m_pkTimeManager)
    {
        fTime = m_pkTimeManager->GetCurrentTime();
        if (m_fLastUpdateTime > fTime)
        {
            RunUpScene();
        }
        
        if(bForceUpdate || fTime != m_fLastUpdateTime)
        {
            m_spScene->UpdateSelected(fTime);
            UpdateBoundShapes(fTime);
            m_fLastUpdateTime = fTime;
            MainThreadUpdateAllViews(MAKELPARAM(NIF_TIMECHANGED, fTime));
        }
    }

    return fTime;
}
//---------------------------------------------------------------------------
unsigned int CNifDoc::GetRootId(NiNode* pkRoot)
{
    ASSERT(m_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == m_ulThreadId);
    if(m_spScene == NULL)
        return (unsigned int) -1;

    for(unsigned int ui = 0; ui < m_spScene->GetArrayCount(); ui++)
    {
        NiAVObject* pkObj = m_spScene->GetAt(ui);
        if(pkObj && NiIsKindOf(NiNode, pkObj))
        {
            NiNode* pkNode = (NiNode*) pkObj;
            if(pkRoot == pkNode)
                return ui;
        }
    }

    return (unsigned int) -1;
}
//---------------------------------------------------------------------------
unsigned int CNifDoc::GetRecursiveRootId(NiAVObject* pkObject)
{
    ASSERT(m_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == m_ulThreadId);
    
    if(m_spScene == NULL)
        return (unsigned int) -1;

    NiAVObject* pkPreviousObj = NULL;
    while(pkObject && pkObject != m_spScene)
    {
        pkPreviousObj = pkObject;
        pkObject = pkObject->GetParent();
    }

    if(pkObject == m_spScene && NiIsKindOf(NiNode, pkPreviousObj))
        return GetRootId((NiNode*) pkPreviousObj);
    else
        return (unsigned int) -1;
}
//---------------------------------------------------------------------------
NiNode* CNifDoc::GetRoot(unsigned int uiWhichRoot)
{
    ASSERT(m_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == m_ulThreadId);

    unsigned int uiRootAccum = 0;
    if(m_spScene && uiWhichRoot < GetNumberOfRoots())
    {
        for(unsigned int ui = 0; ui < m_spScene->GetArrayCount(); ui++)
        {
            NiAVObject* pkObj = m_spScene->GetAt(ui);
            if(NiIsKindOf(NiNode, pkObj))
            {
                
                if(uiRootAccum == uiWhichRoot)
                    return (NiNode*) pkObj;
                uiRootAccum++;
            }
        }
    }
    return NULL;
}
//---------------------------------------------------------------------------
unsigned int CNifDoc::GetNumberOfRoots()
{
    ASSERT(m_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == m_ulThreadId);
    return m_uiNumRoots;
}
//---------------------------------------------------------------------------
bool CNifDoc::IsAnimated(NiAVObject* pkObj)
{
    if(pkObj == NULL)
    {
        return false;
    }

    NiTimeController* pkCtlr = pkObj->GetControllers();
    while (pkCtlr)
    {
        if (pkCtlr->IsTransformController())
        {
            return true;
        }

        pkCtlr = pkCtlr->GetNext();
    }

    return IsAnimated(pkObj->GetParent());
}
//---------------------------------------------------------------------------
CNifCameraList* CNifDoc::GetCameraList()
{
    ASSERT(m_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == m_ulThreadId);
    return m_pkCameraList;
}
//---------------------------------------------------------------------------
NiCamera* CNifDoc::GetCamera(NiCameraInfoIndex kIndex)
{
    ASSERT(m_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == m_ulThreadId);
    if(m_pkCameraList)
        return m_pkCameraList->GetCamera(kIndex);
    else
        return NULL;
}
//---------------------------------------------------------------------------
NiCameraInfo* CNifDoc::GetCameraInfo(NiCameraInfoIndex kIndex)
{
    ASSERT(m_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == m_ulThreadId);
    if(m_pkCameraList)
        return m_pkCameraList->GetCameraInfo(kIndex);
    else
        return NULL;
}
//---------------------------------------------------------------------------
CNifDoc* CNifDoc::GetDocument()
{
    return ms_pkThis;
}
//---------------------------------------------------------------------------
void CNifDoc::AddDefaultLights(NiNode* pkRoot)
{
    ASSERT(m_ulThreadId != NULL);
    ASSERT(GetCurrentThreadId() == m_ulThreadId);

    NIASSERT(m_spScene && m_pkDefaultNode);
    bool bDefaultLightsFound = false;
    for (unsigned int ui = 0; ui < m_pkDefaultNode->GetArrayCount(); ui++)
    {
        NiLight* pkLight = NiDynamicCast(NiLight, 
            m_pkDefaultNode->GetAt(ui));

        if (pkLight)
        {
            bDefaultLightsFound = true;
        }
    }

    if (!bDefaultLightsFound && !FindLights(pkRoot))
    {
        NiDirectionalLight* pkDirLight1 = NiTCreate<NiDirectionalLight>();
        pkDirLight1->SetAmbientColor(NiColor::BLACK);
        pkDirLight1->SetDiffuseColor(NiColor::WHITE);
        pkDirLight1->SetSpecularColor(NiColor::WHITE);
        NiMatrix3 kRot1(
            NiPoint3(0.44663f, 0.496292f, -0.744438f),
            NiPoint3(0.0f, -0.83205f, -0.5547f),
            NiPoint3(-0.894703f, 0.247764f, -0.371646f));
        pkDirLight1->SetRotate(kRot1);
        pkDirLight1->SetName("Default Light");
        pkDirLight1->SetAppCulled(true);
    
        NiDirectionalLight* pkDirLight2 = NiTCreate<NiDirectionalLight>();
        pkDirLight2->SetAmbientColor(NiColor::BLACK);
        pkDirLight2->SetDiffuseColor(NiColor::WHITE);
        pkDirLight2->SetSpecularColor(NiColor::WHITE);
        NiMatrix3 kRot2(
            NiPoint3(-0.44663f, -0.496292f, 0.744438f),
            NiPoint3(0.0f, 0.83205f, 0.5547f),
            NiPoint3(-0.894703f, 0.247764f, -0.371646f));
        pkDirLight2->SetRotate(kRot2);
        pkDirLight2->SetName("Default Light");
        pkDirLight2->SetAppCulled(true);

        pkDirLight1->AttachAffectedNode(m_spScene);
        pkDirLight2->AttachAffectedNode(m_spScene);

        m_pkDefaultNode->AttachChild(pkDirLight1);
        m_pkDefaultNode->AttachChild(pkDirLight2);

        CNifUserPreferences::Lock();
        EnableDefaultLights(CNifUserPreferences::AccessUserPreferences()
            ->GetUseDefaultLights());
        CNifUserPreferences::UnLock();
    }
}
//---------------------------------------------------------------------------
bool CNifDoc::FindLights(NiNode* pkNode)
{
    for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild)
        {
            if (NiIsKindOf(NiLight, pkChild))
            {
                return true;
            }
            else if (NiIsKindOf(NiNode, pkChild))
            {
                if (FindLights((NiNode*) pkChild))
                {
                    return true;
                }
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void CNifDoc::EnableDefaultLights(bool bEnable)
{
    if (!m_pkDefaultNode)
    {
        return;
    }

    Lock();

    for (unsigned int ui = 0; ui < m_pkDefaultNode->GetArrayCount(); ui++)
    {
        NiLight* pkLight = NiDynamicCast(NiLight, 
            m_pkDefaultNode->GetAt(ui));
        if (pkLight && pkLight->GetName() &&
            strcmp(pkLight->GetName(), "Default Light") == 0)
        {
            pkLight->SetSwitch(bEnable);
        }
    }

    UnLock();
}
//---------------------------------------------------------------------------
CNifTimeManager* CNifDoc::GetTimeManager()
{
    return m_pkTimeManager;
}
//---------------------------------------------------------------------------
float CNifDoc::GetLastTimeUpdated()
{
    return m_fLastUpdateTime;
}
//---------------------------------------------------------------------------
CNifAnimationInfo* CNifDoc::GetAnimationInfo()
{
    return m_pkAnimInfo;
}
//---------------------------------------------------------------------------
void CNifDoc::OnPreferences() 
{
    const float fRadToDeg = 360.0f / (2.0f * NI_PI);
    const float fDegToRad = 1.0f / fRadToDeg;

    CNifUserPreferences::Lock();

    CNifUserPreferences* pkPrefs =
        CNifUserPreferences::AccessUserPreferences();
    CPreferencesDlg dlg;
    dlg.m_bDefaultLights = pkPrefs->GetUseDefaultLights();
    NiPoint3 kTranslateSpeed = pkPrefs->GetTranslateSpeed();
    dlg.m_uiTranslateSpeedX = (unsigned int) (kTranslateSpeed.x * 100.0f);
    dlg.m_uiTranslateSpeedY = (unsigned int) (kTranslateSpeed.y * 100.0f);
    dlg.m_uiTranslateSpeedZ = (unsigned int) (kTranslateSpeed.z * 100.0f);
    dlg.m_uiPitchSpeed = (unsigned int) (pkPrefs->GetPitchSpeed() *
        fRadToDeg);
    dlg.m_uiYawSpeed = (unsigned int) (pkPrefs->GetYawSpeed() * fRadToDeg);
    dlg.m_kBackgroundColor = pkPrefs->GetBackgroundColor();
    dlg.m_bPromptOnDiscard = pkPrefs->GetPromptOnDiscard();
    dlg.m_bHighlightSelected = pkPrefs->GetHighlightSelected();
    dlg.m_bShowABVs = pkPrefs->GetShowABVs();
    dlg.m_iCameraControlMode =
        static_cast<int>(pkPrefs->GetCameraControlMode());

    CNifUserPreferences::UnLock();

    if (dlg.DoModal() == IDOK)
    {
        CNifUserPreferences::Lock();

        pkPrefs = CNifUserPreferences::AccessUserPreferences();
        pkPrefs->SetUseDefaultLights(dlg.m_bDefaultLights ? true : false);
        kTranslateSpeed.x = dlg.m_uiTranslateSpeedX / 100.0f;
        kTranslateSpeed.y = dlg.m_uiTranslateSpeedY / 100.0f;
        kTranslateSpeed.z = dlg.m_uiTranslateSpeedZ / 100.0f;
        pkPrefs->SetTranslateSpeed(kTranslateSpeed);
        pkPrefs->SetPitchSpeed(dlg.m_uiPitchSpeed * fDegToRad);
        pkPrefs->SetYawSpeed(dlg.m_uiYawSpeed * fDegToRad);
        pkPrefs->SetBackgroundColor(dlg.m_kBackgroundColor);
        pkPrefs->SetPromptOnDiscard(dlg.m_bPromptOnDiscard ? true : false);
        pkPrefs->SetHighlightSelected(
            dlg.m_bHighlightSelected ? true : false);
        pkPrefs->SetShowABVs(dlg.m_bShowABVs ? true : false);
        pkPrefs->SetCameraControlMode(
            static_cast<CNifUserPreferences::CameraControlMode>(
                dlg.m_iCameraControlMode));

        CNifUserPreferences::UnLock();

        // Responding on changes
        if (!dlg.m_bHighlightSelected)
        {
            HighlightObject(NULL);
        }

        if (dlg.m_bCameraControlModeChanged)
        {
            UpdateAllViews(
                NULL,
                MAKELPARAM(NIF_CAMERAMODECHANGED, NULL),
                NULL);
        }

        bool bShowABVs = dlg.m_bShowABVs ? true : false;
        ShowABVsForRoot(m_spScene, bShowABVs, true);
    }
}
//---------------------------------------------------------------------------
void CNifDoc::UpdateAllViews( CView* pSender, LPARAM lHint, 
                             CObject* pHint)
{
    CDocument::UpdateAllViews(pSender, lHint, pHint);
    CNifPropertyWindowManager::SendUpdateMessage(pSender, lHint, pHint);
}
//---------------------------------------------------------------------------
void CNifDoc::OnSetImageSubfolder() 
{
    CNifUserPreferences::Lock();
    CString strSubfolder = CNifUserPreferences::AccessUserPreferences()
        ->GetImageSubfolderPath();
    strSubfolder = FileDialog::GetFolderName(AfxGetMainWnd()->GetSafeHwnd(),
        "Choose image subfolder:", strSubfolder);
    if (!strSubfolder.IsEmpty())
    {
        CNifUserPreferences::AccessUserPreferences()->SetImageSubfolderPath(
           strSubfolder);
        NiImageConverter::SetPlatformSpecificSubdirectory(
           strSubfolder);
    }

    CNifUserPreferences::UnLock();

}
//---------------------------------------------------------------------------
void CNifDoc::HighlightObject(NiAVObject* pkObject)
{
    if (!m_spBoundShape && !CreateBoundNode())
        return;
    
    NIASSERT(m_spBoundShape && m_pkBoundShapeRoot);

    // Ensure that this object is not already highlighted.
    NiTMapIterator kIter = m_kBoundShapeMap.GetFirstPos();
    while (kIter)
    {
        NiAVObject* pkBTemp;
        NiAVObject* pkOTemp;
        m_kBoundShapeMap.GetNext(kIter, pkBTemp, pkOTemp);
        if (pkOTemp == pkObject)
        {
            return;
        }
    }

    CNifUserPreferences::Lock();
    bool bHighlight = CNifUserPreferences::AccessUserPreferences()
        ->GetHighlightSelected();
    CNifUserPreferences::UnLock();

    Lock();

    // Remove previous bound shapes.
    m_pkBoundShapeRoot->RemoveAllChildren();
    m_kBoundShapeMap.RemoveAll();

    if (bHighlight && pkObject)
    {
        // Add bound shape.
        NiAVObject* pkBoundShape = (NiAVObject*) m_spBoundShape->Clone();
        m_pkBoundShapeRoot->AttachChild(pkBoundShape);
        m_kBoundShapeMap.SetAt(pkBoundShape, pkObject);

        m_pkBoundShapeRoot->UpdateProperties();
        UpdateBoundShapes(m_pkTimeManager->GetCurrentTime());
    }

    UnLock();

    UpdateAllViews(NULL, MAKELPARAM(NIF_SELECTEDOBJECTCHANGED, NULL),
        (CObject*) pkObject);
}
//---------------------------------------------------------------------------
void CNifDoc::UpdateBoundShapes(float fTime)
{
    NIASSERT(m_pkBoundShapeRoot);

    Lock();

    for (unsigned int ui = 0; ui < m_pkBoundShapeRoot->GetArrayCount(); ui++)
    {
        NiAVObject* pkBoundShape = m_pkBoundShapeRoot->GetAt(ui);
        if (pkBoundShape)
        {
            NiAVObject* pkTarget;
            m_kBoundShapeMap.GetAt(pkBoundShape, pkTarget);

            NiBound kBound = pkTarget->GetWorldBound();
            NiPoint3 kBoundCenter = kBound.GetCenter();
            float fBoundRadius = kBound.GetRadius();
            if (fBoundRadius < 0.0f)
            {
                fBoundRadius = 0.0f;
            }

            pkBoundShape->SetTranslate(kBoundCenter);
            pkBoundShape->SetScale(2.0f * fBoundRadius);
        }
    }

    m_pkBoundShapeRoot->Update(fTime);
    NiMesh::CompleteSceneModifiers(m_pkBoundShapeRoot);

    UnLock();
}
//---------------------------------------------------------------------------
bool CNifDoc::GetSelectionParams(NiPoint3& kCenter, float& fRadius)
{
    if (!m_pkBoundShapeRoot || m_kBoundShapeMap.IsEmpty())
        return false;

    Lock();
    kCenter = m_pkBoundShapeRoot->GetWorldBound().GetCenter();
    fRadius = m_pkBoundShapeRoot->GetWorldBound().GetRadius();
    UnLock();

    return true;
}
//---------------------------------------------------------------------------
void CNifDoc::OnListOptions() 
{
    UpdateAllViews(NULL, MAKELPARAM(ID_LIST_OPTIONS, NULL), NULL);
}
//---------------------------------------------------------------------------
void CNifDoc::ShowABVsForRoot(NiNode* pkNode, bool bShowABVs,
    bool bUpdate)
{
    for (unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
    {
        NiAVObject* pkChild = pkNode->GetAt(ui);
        if (pkChild)
        {
            const char* pcName = pkChild->GetName();
            if (!pcName || (strcmp(pcName, "Default Node") != 0 &&
                strcmp(pcName, "Gamebryo Collision Lab") != 0))
            {
                ShowABVs(pkChild, bShowABVs, bUpdate);
            }
        }
    }
}
//---------------------------------------------------------------------------
void CNifDoc::ShowABVs(NiAVObject* pkRoot, bool bShowABVs, bool bUpdate)
{
    if (bShowABVs)
    {
        Lock();
        m_spScene->Update(m_pkTimeManager->GetCurrentTime());
        NiMesh::CompleteSceneModifiers(m_spScene);
        NiOptimizeABV::CreateWireABV(pkRoot, NiColor(0.0f, 0.0f, 1.0f),
            1.0f, "WireABVTemp", false);
        bool bSelectiveUpdate, bRigid;
        m_spScene->SetSelectiveUpdateFlags(bSelectiveUpdate, false, bRigid);
        if (bUpdate)
        {
            m_spScene->Update(m_pkTimeManager->GetCurrentTime());
            NiMesh::CompleteSceneModifiers(m_spScene);
            m_spScene->UpdateProperties();
            m_spScene->UpdateEffects();
            m_spScene->UpdateNodeBound();
        }
        UnLock();
    }
    else
    {
        Lock();
        NiOptimizeABV::RemoveByFirstToken(pkRoot, "WireABVTemp");
        m_pkBoundShapeRoot->RemoveAllChildren();
        m_kBoundShapeMap.RemoveAll();
        bool bSelectiveUpdate, bRigid;
        m_spScene->SetSelectiveUpdateFlags(bSelectiveUpdate, false, bRigid);
        if (bUpdate)
        {
            m_spScene->Update(m_pkTimeManager->GetCurrentTime());
            NiMesh::CompleteSceneModifiers(m_spScene);
            m_spScene->UpdateProperties();
            m_spScene->UpdateEffects();
            m_spScene->UpdateNodeBound();
        }
        UnLock();
    }

    if (bUpdate)
    {
        UpdateAllViews(NULL, MAKELPARAM(NIF_SCENECHANGED, NULL), NULL);
    }
}
//---------------------------------------------------------------------------
void CNifDoc::OnToggleABVs() 
{
    CNifUserPreferences::Lock();
    CNifUserPreferences* pkPrefs =
        CNifUserPreferences::AccessUserPreferences();
    bool bShowABVs = !pkPrefs->GetShowABVs();
    pkPrefs->SetShowABVs(bShowABVs);
    CNifUserPreferences::UnLock();

    ShowABVsForRoot(m_spScene, bShowABVs, true);
}
//---------------------------------------------------------------------------
void CNifDoc::OnUpdateToggleABVs(CCmdUI* pCmdUI) 
{
    CNifUserPreferences::Lock();
    bool bShowABVs = CNifUserPreferences::AccessUserPreferences()
        ->GetShowABVs();
    CNifUserPreferences::UnLock();

    pCmdUI->SetCheck(bShowABVs);
}
//---------------------------------------------------------------------------
NiNode* CNifDoc::AddRootTextureObject(NiTexture* pkRootObject,
    const char* pcFilePath)
{
    NiMeshScreenElements* pkScreenQuad = 
        NiTCreate3<NiMeshScreenElements>(false, false, 1);

    // Apply Transforms
    NiMatrix3 kMat;
    kMat.MakeXRotation(NI_HALF_PI);
    pkScreenQuad->SetRotate(kMat);

    int iPolygon = pkScreenQuad->Insert(4);
    pkScreenQuad->SetRectangle(iPolygon, 0.0f, 0.0f, 1.0f, 1.0f);
    pkScreenQuad->UpdateBound();
    pkScreenQuad->SetTextures(iPolygon, 0, 0.0f, 0.0f, 1.0f, 1.0f);

    // Create and add texturing property.
    NiTexturingProperty* pkTexProp = NiTCreate<NiTexturingProperty>();
    pkTexProp->SetApplyMode(NiTexturingProperty::APPLY_REPLACE);
    pkTexProp->SetBaseMap(NiTCreate<NiTexturingProperty::Map>());
    pkTexProp->SetBaseClampMode(NiTexturingProperty::CLAMP_S_CLAMP_T);
    pkScreenQuad->AttachProperty(pkTexProp);

    // Create and add z-buffer property.
    NiZBufferProperty* pkZBufferProp = NiTCreate<NiZBufferProperty>();
    pkZBufferProp->SetZBufferTest(false);
    pkZBufferProp->SetZBufferWrite(false);
    pkScreenQuad->AttachProperty(pkZBufferProp);

    // Perform initial update.
    pkScreenQuad->Update(0.0f);
    NiMesh::CompleteSceneModifiers(pkScreenQuad);
    pkScreenQuad->UpdateNodeBound();
    pkScreenQuad->UpdateProperties();
    pkScreenQuad->UpdateEffects();

    pkTexProp->SetBaseTexture(pkRootObject);

    return AddRootObject(pkScreenQuad, pcFilePath);
}
//---------------------------------------------------------------------------
NiNode* CNifDoc::AddRootObject(NiAVObject* pkRootObject,
    const char* pcFilePath)
{
    // Start animations for APP_INIT controllers.
    NiTimeController::StartAnimations(pkRootObject, 0.0f);

    NiNode* pkRoot = NiTCreate<NiNode>(); 
    pkRoot->SetSelectiveUpdate(true);
    pkRoot->SetSelectiveUpdateTransforms(true);
    pkRoot->SetSelectiveUpdatePropertyControllers(true);
    pkRoot->SetSelectiveUpdateRigid(false);
    const char* pcNifName = strrchr(pcFilePath, '\\') + 1;
    pkRoot->SetName(pcNifName);
    pkRoot->AttachChild(pkRootObject);

    Lock();

    unsigned int uiIndex = m_spScene->GetChildCount();
    AddRoot(pkRoot);

    CNifStatisticsManager::Lock();
    CNifStatisticsManager::AccessStatisticsManager()->AddRoot(pkRoot);
    CNifStatisticsManager::UnLock();
    AddDefaultLights(pkRoot);

    CNifUserPreferences::Lock();
    bool bShowABVs = CNifUserPreferences::AccessUserPreferences()
        ->GetShowABVs();
    CNifUserPreferences::UnLock();
    if (bShowABVs)
    {
        ShowABVs(pkRoot);
    }

    m_spScene->Update(0.0f);
    NiMesh::CompleteSceneModifiers(m_spScene);
    m_spScene->UpdateProperties();
    m_spScene->UpdateEffects();
    m_spScene->UpdateNodeBound();

    m_pkCameraList->AddCamerasForRoot(pkRoot);

    UpdateAllViews(NULL, MAKELPARAM(NIF_ADDNIF, uiIndex), NULL);
    UpdateAllViews(NULL, MAKELPARAM(NIF_UPDATEDEFAULTNODE, 0), NULL);

    UnLock();

    if (AfxGetMainWnd())
    {
        CString strAppTitle;
        strAppTitle.LoadString(AFX_IDS_APP_TITLE);
        if (m_uiNumRoots > 2)
        {
            AfxGetMainWnd()->SetWindowText(strAppTitle +
                " - Multiple Files");
        }
        else
        {
            AfxGetMainWnd()->SetWindowText(strAppTitle + CString(" - ") +
                pcNifName);
        }
    }

    g_bNeedPromptOnDiscard = true;

    return pkRoot;
}
//---------------------------------------------------------------------------
NiMeshScreenElementsArray& CNifDoc::GetScreenElementsArray()
{
    return m_kScreenElements;
}
//---------------------------------------------------------------------------
bool CNifDoc::GetDataStreamInfo(NiDataStream* pkStream,
    NiUInt8& uiAccessMask)
{
    return m_kDataStreamToAccessMaskMap.GetAt(pkStream, uiAccessMask);
}
//---------------------------------------------------------------------------
void CNifDoc::SetDataStreamInfo(NiDataStream* pkStream,
    NiUInt8 uiAccessMask)
{
    if (!pkStream)
        return;

    m_kDataStreamToAccessMaskMap.SetAt(pkStream, uiAccessMask);
}
//---------------------------------------------------------------------------
void CNifDoc::ClearDataStreamInfo()
{
    m_kDataStreamToAccessMaskMap.RemoveAll();
}
//---------------------------------------------------------------------------
