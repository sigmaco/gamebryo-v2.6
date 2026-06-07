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

// NiObjectNETTimeControllersDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiObjectNETTimeControllersDlg.h"
#include "NifPropertyWindowManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CNiObjectNETTimeControllersDlg dialog
//---------------------------------------------------------------------------
CNiObjectNETTimeControllersDlg::CNiObjectNETTimeControllersDlg(CWnd* pParent)
    : CNiObjectDlg(CNiObjectNETTimeControllersDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CNiObjectNETTimeControllersDlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
void CNiObjectNETTimeControllersDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNiObjectNETTimeControllersDlg)
    DDX_Control(pDX, IDC_TIME_CONTROLLERS_LIST, m_wndTimeControllersList);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNiObjectNETTimeControllersDlg, CDialog)
    //{{AFX_MSG_MAP(CNiObjectNETTimeControllersDlg)
    ON_NOTIFY(NM_DBLCLK, IDC_TIME_CONTROLLERS_LIST, 
        OnDblclkTimeControllersList)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNiObjectNETTimeControllersDlg message handlers
//---------------------------------------------------------------------------
bool CNiObjectNETTimeControllersDlg::DoUpdate()
{
    int nItem = 0;
    LPCTSTR lpszItem = NULL;
    NiObjectNET* pkObj = (NiObjectNET*) m_pkObj;
    NiTimeController* pkController = pkObj->GetControllers();
    
    m_wndTimeControllersList.DeleteAllItems();
    while(pkController)
    {
        lpszItem = pkController->GetRTTI()->GetName();
        m_wndTimeControllersList.InsertItem( nItem++, lpszItem );
        pkController = pkController->GetNext();
    }

    return true;
}
//---------------------------------------------------------------------------
BOOL CNiObjectNETTimeControllersDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    
    ASSERT(this->m_pkObj != NULL && NiIsKindOf(NiObjectNET, m_pkObj));
    m_wndTimeControllersList.InsertColumn(0, "Class");
    CRect rect;
    m_wndTimeControllersList.GetWindowRect(&rect);
    int cx = rect.Width();
    m_wndTimeControllersList.SetColumnWidth(0, cx-4);
    DoUpdate();
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}
//---------------------------------------------------------------------------
void CNiObjectNETTimeControllersDlg::OnDblclkTimeControllersList(
    NMHDR* pNMHDR, LRESULT* pResult)
{
    if(m_wndTimeControllersList.GetSelectedCount() == 0)
        return;

    int iIndex = m_wndTimeControllersList.GetNextItem(-1, LVIS_SELECTED);

    if(iIndex == -1)
        return;

    int iMatchIndex = 0;
    NiAVObject* pkObj = (NiAVObject*) m_pkObj;
    NiTimeController* pkController = pkObj->GetControllers();
    while(pkController && iMatchIndex != iIndex)
    {
        pkController = pkController->GetNext();
        iMatchIndex++;
    }
    if(pkController)
    {
        CNifPropertyWindowManager* pkManager = 
            CNifPropertyWindowManager::GetPropertyWindowManager();

        pkManager->CreatePropertyWindow(pkController);
    }

    *pResult = 0;
}
//---------------------------------------------------------------------------
