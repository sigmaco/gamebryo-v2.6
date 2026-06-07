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

// NifAnimationControls.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NifCollisionControls.h"
#include "NifCollisionOptionsDlg.h"
#include "NifDoc.h"    
#include "NifUserPreferences.h"
#include "NifTimeManager.h"
#include "NiWin32FileFinder.h"
#include "FileDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CNifCollisionControls property page
//---------------------------------------------------------------------------
IMPLEMENT_DYNCREATE(CNifCollisionControls, CDialog)
//---------------------------------------------------------------------------
CNifCollisionControls::CNifCollisionControls() :
    CDialog(CNifCollisionControls::IDD)
{
    m_acPath[0] = '\0';

    CNifUserPreferences::Lock();
    CString strPath = CNifUserPreferences::AccessUserPreferences()
        ->GetCollisionLabPath();
    NiStrcpy(m_acPath, MAX_PATH, strPath);

    CNifUserPreferences::UnLock();

    //{{AFX_DATA_INIT(CNifCollisionControls)
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
CNifCollisionControls::~CNifCollisionControls()
{
}
//---------------------------------------------------------------------------
void CNifCollisionControls::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNifCollisionControls)
    DDX_Control(pDX, IDC_COLLIDEE_LIST, m_kCollideeList);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNifCollisionControls, CDialog)
    //{{AFX_MSG_MAP(CNifCollisionControls)
    ON_LBN_DBLCLK(IDC_COLLIDEE_LIST, OnDblclkListCollideeFiles)
    ON_BN_CLICKED(IDC_CHOOSE_COLLIDER, OnAddCollider)
    ON_BN_CLICKED(IDC_SET_COLLIDEE_PATH, OnSetCollideePath)
    ON_BN_CLICKED(IDC_COLLIDE_REMOVEALL, OnRemoveCollisionLab)
    ON_BN_CLICKED(IDC_COLLISIONLAB_OPTIONS, OnCollisionLabOptions)
    ON_WM_CREATE()
    ON_WM_HSCROLL()
    ON_WM_DESTROY()
    ON_WM_CLOSE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

//---------------------------------------------------------------------------
// CNifCollisionControls message handlers
//---------------------------------------------------------------------------
void CNifCollisionControls::OnSetCollideePath()
{
    CNifUserPreferences::Lock();
    CString strInitialFolder = CNifUserPreferences::AccessUserPreferences()
        ->GetCollisionLabPath();
    CNifUserPreferences::UnLock();

    CString strSubfolder = FileDialog::GetFolderName(
        AfxGetMainWnd()->GetSafeHwnd(), "Choose collidee subfolder:",
        strInitialFolder);
    if (!strSubfolder.IsEmpty())
    {
        // Set path in IDC_COLLIDEE_PATH
        NiStrcpy(m_acPath, MAX_PATH, strSubfolder);
        SetDlgItemText(IDC_COLLIDEE_PATH, m_acPath);

        CNifUserPreferences::Lock();
        CNifUserPreferences::AccessUserPreferences()
            ->SetCollisionLabPath(m_acPath);
        CNifUserPreferences::UnLock();

        // Refresh IDC_COLLIDEE_LIST list
        OnInitDialog();
    }
}
//---------------------------------------------------------------------------
void CNifCollisionControls::OnAddCollider()
{
    CString strColliderPath = FileDialog::GetFilename(
        AfxGetMainWnd()->GetSafeHwnd(), "Choose Collider",
        "AssetViewer Files (*.nif)\0*.nif\0\0");
    if (!strColliderPath.IsEmpty())
    {
        const char* pcExt = strrchr(strColliderPath, '.');
        if (toupper(pcExt[1]) == 'N' && toupper(pcExt[2]) == 'I' &&
            toupper(pcExt[3]) == 'F')
        {
            CNifDoc* pkDoc = CNifDoc::GetDocument();
            pkDoc->Lock();
            pkDoc->m_kCollisionLabInfo.AddCollisionLabNifFile(
                strColliderPath, COLLIDER_POS);
            NiCollisionLabInfo* pkLabInfo = &pkDoc->m_kCollisionLabInfo;
            pkLabInfo->SetAuxCallbacksOnSceneGraph(
                pkLabInfo->GetAuxCallbacks());
            pkDoc->UnLock();
        }
        else
        {
            CString strMsg;
            strMsg.Format("%s\nis not a valid AssetViewer file. It will "
                "not be added.", strColliderPath);
            MessageBox(strMsg, "Invalid File Type", MB_OK | MB_ICONERROR);
        }
    }
}
//---------------------------------------------------------------------------
void CNifCollisionControls::OnCollisionLabOptions()
{
    CNifCollisionOptionsDlg kDialog;
    kDialog.DoModal();
}
//---------------------------------------------------------------------------
void CNifCollisionControls::OnRemoveCollisionLab()
{
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    pkDoc->RemoveFileByName(STR_COLLISION_LAB);
}
//---------------------------------------------------------------------------
int CNifCollisionControls::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CDialog::OnCreate(lpCreateStruct) == -1)
        return -1;

    return 0;
}
//---------------------------------------------------------------------------
void CNifCollisionControls::OnUpdate(CView* pSender, LPARAM lHint,
    CObject* pHint)
{
    WORD wLoWord = LOWORD(lHint);
    CNifDoc* pkDoc = CNifDoc::GetDocument();
       
    if (wLoWord == NIF_TIMINGINFOCHANGED)
    {
        pkDoc->Lock();
        RefreshValues();
        pkDoc->UnLock();
    }
    else if(wLoWord == NIF_TIMECHANGED && pkDoc)
    {
        float fTime = pkDoc->GetLastTimeUpdated();
        char acValue [256];
        NiSprintf(acValue,  256,"%f", fTime);
    }
}
//---------------------------------------------------------------------------
BOOL CNifCollisionControls::OnInitDialog() 
{
    CDialog::OnInitDialog();

    SetDlgItemText(IDC_COLLIDEE_PATH,m_acPath);

    // Open our path...
    // Look for nif files and populate collidee/collider boxes
    
    // The directory for now:
    m_kCollideeList.ResetContent();
    m_kCollideeList.AddString("<NO SELECTION>");
    
    NiWin32FileFinder kFinder(m_acPath, false,".nif");

    CDC* pDC = m_kCollideeList.GetDC();
    CSize TextSize;
    int iMaxWidth = 0;


    while(kFinder.HasMoreFiles())
    {
        NiWin32FoundFile* pkFoundFile = kFinder.GetNextFile();
        if (pkFoundFile)
        {
            char acFile[256];
            NiStrcpy(acFile, 256, 
                pkFoundFile->m_strPath + strlen(m_acPath) + 1);
            CString strListBoxEntry = pkFoundFile->m_strPath;
            m_kCollideeList.AddString(acFile);

            // Get the Extent of the String
            TextSize = pDC->GetTextExtent(acFile);

            // Increase the max width if it is larger
            if (iMaxWidth < TextSize.cx)
                iMaxWidth = TextSize.cx;
        }
    }

    // Get the Text Metrics
    TEXTMETRIC   tm;
    pDC->GetTextMetrics(&tm);

    // Add the avg width to prevent clipping
    iMaxWidth += tm.tmAveCharWidth;

    // Set the Horizontal Extent of the List Box
    m_kCollideeList.SetHorizontalExtent(iMaxWidth);

    m_kCollideeList.SetCurSel(0);

    RefreshValues();
    
    return TRUE;
}
//---------------------------------------------------------------------------
void CNifCollisionControls::OnHScroll(UINT nSBCode, UINT nPos, 
    CScrollBar* pScrollBar) 
{
    CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
//---------------------------------------------------------------------------
void CNifCollisionControls::OnDestroy() 
{
    CDialog::OnDestroy();
}
//---------------------------------------------------------------------------
void CNifCollisionControls::OnClose() 
{

    CDialog::OnClose();
}
//---------------------------------------------------------------------------
BOOL CNifCollisionControls::DestroyWindow() 
{
    return CDialog::DestroyWindow();
}
//---------------------------------------------------------------------------
void CNifCollisionControls::RefreshValues()
{
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    if(!pkDoc) 
        return;

    pkDoc->Lock();
    CNifTimeManager* pkTimeManager = pkDoc->GetTimeManager();
    pkDoc->UnLock();  
}
//---------------------------------------------------------------------------
void CNifCollisionControls::OnDblclkListCollideeFiles() 
{
    int iSel = m_kCollideeList.GetCurSel();
    
    if(iSel <= 0)
        return;

    char szBuffer[256];
    m_kCollideeList.GetText(iSel, szBuffer);

    // Load Nif
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    CString strListBoxEntry;
    m_kCollideeList.GetText(iSel, strListBoxEntry);

    // Append path to nif file
    CString strSelection = "\\" + strListBoxEntry;
    CString strFullPath = m_acPath + strSelection;

    pkDoc->m_kCollisionLabInfo.AddCollisionLabNifFile(
        strFullPath,COLLIDEE_POS);
}
//---------------------------------------------------------------------------

