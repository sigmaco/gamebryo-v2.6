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

// NifCollisionOptionsDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NifCollisionOptionsDlg.h"
#include "NifDoc.h"
#include "NifUserPreferences.h"
#include "NifTimeManager.h"
#include "NifAnimationInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CNifCollisionOptionsDlg dialog
//---------------------------------------------------------------------------
CNifCollisionOptionsDlg::CNifCollisionOptionsDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CNifCollisionOptionsDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CNifCollisionOptionsDlg)
    m_bInitialized = false;
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
void CNifCollisionOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNifCollisionOptionsDlg)
    DDX_Control(pDX, IDC_DYNAMIC_VELOCITY, m_wndDynamicVelocity);
    DDX_Text(pDX, IDC_DYNAMIC_VELOCITY, m_fDynamicVelocity);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNifCollisionOptionsDlg, CDialog)
    //{{AFX_MSG_MAP(CNifCollisionOptionsDlg)
    ON_EN_CHANGE(IDC_DYNAMIC_VELOCITY, OnChangeDynamicVelocity)
    ON_WM_HSCROLL()
    ON_WM_CREATE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNifCollisionOptionsDlg message handlers
//---------------------------------------------------------------------------
void CNifCollisionOptionsDlg::OnOK() 
{
    CDialog::OnOK();
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    if(pkDoc)
    {
        pkDoc->Lock();
        NiCollisionLabInfo* pkLabInfo = &pkDoc->m_kCollisionLabInfo;

        CNifUserPreferences::Lock();

        pkLabInfo->SetVelocity(m_fDynamicVelocity);

        // Get the Aux collision value...
        if (IsDlgButtonChecked(IDC_AUXCALLBACKS))
        {
            pkLabInfo->SetAuxCallbacks(true);
        }
        else
        {
            pkLabInfo->SetAuxCallbacks(false);
        }

        pkLabInfo->SetAuxCallbacksOnSceneGraph(
            pkLabInfo->GetAuxCallbacks());

        CNifUserPreferences* pkPrefs = 
            CNifUserPreferences::AccessUserPreferences();
        
        CNifUserPreferences::UnLock();
        pkDoc->UpdateAllViews(NULL, MAKELPARAM(NIF_TIMINGINFOCHANGED,NULL), 
            NULL);
        pkDoc->UnLock();
    }
}
//---------------------------------------------------------------------------
void CNifCollisionOptionsDlg::OnCancel() 
{
    CDialog::OnCancel();
}
//---------------------------------------------------------------------------
BOOL CNifCollisionOptionsDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    
    CNifDoc* pkDoc = CNifDoc::GetDocument();
    if(pkDoc)
    {
        pkDoc->Lock();
        NiCollisionLabInfo* pkLabInfo = &pkDoc->m_kCollisionLabInfo;

        CNifTimeManager* pkManager = pkDoc->GetTimeManager();
        if(!pkManager) 
            return TRUE;

        char acString[256];
        //Init starting time

        NiSprintf(acString, 256, "%.3f", pkLabInfo->GetVelocity());
        m_wndDynamicVelocity.SetWindowText(acString);

        CheckDlgButton(IDC_AUXCALLBACKS,
            pkLabInfo->GetAuxCallbacks());

        //Init time slider tick rate
        CNifUserPreferences::Lock();
        CNifUserPreferences::UnLock();

        m_bInitialized = true;
        pkDoc->UnLock();
    }
    
    return TRUE;
}
//---------------------------------------------------------------------------
int CNifCollisionOptionsDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    lpCreateStruct->style &= WS_THICKFRAME | WS_MAXIMIZEBOX; 
    if (CDialog::OnCreate(lpCreateStruct) == -1)
        return -1;

    return 0;
}
//---------------------------------------------------------------------------
BOOL CNifCollisionOptionsDlg::PreCreateWindow(CREATESTRUCT& cs) 
{
    // Here we modify the construction params and call the base class

    // removes min/max boxes
    cs.style &= (WS_MAXIMIZEBOX | WS_MINIMIZEBOX);

    // makes dialog box unsizable
    cs.style &= WS_THICKFRAME;
    return CDialog::PreCreateWindow(cs);
}
//---------------------------------------------------------------------------
void CNifCollisionOptionsDlg::OnChangeDynamicVelocity() 
{
    if(m_bInitialized)
    {
        int nMaxCount = 256;
        char acString[256];
    
        m_wndDynamicVelocity.GetWindowText(acString, nMaxCount);
        double dValue = atof(acString);
        if (dValue < 1.0f)
        {
            dValue = 1.0f;
            NiSprintf(acString, 256, "%.3f", dValue);
            m_wndDynamicVelocity.SetWindowText(acString);
        }
        else if (dValue > 10000.0f)
        {
            dValue = 10000.0f;
            NiSprintf(acString, 256, "%.3f", dValue);
            m_wndDynamicVelocity.SetWindowText(acString);
        }

        m_fDynamicVelocity = (float) dValue;
    }   
}
//---------------------------------------------------------------------------
