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

// NiTimeControllerInterpolatorsDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiTimeControllerInterpolatorDlg.h"
#include "NifPropertyWindowManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CNiTimeControllerInterpolatorsDlg dialog
//---------------------------------------------------------------------------
CNiTimeControllerInterpolatorsDlg::CNiTimeControllerInterpolatorsDlg(
    CWnd* pParent /*=NULL*/): 
    CNiObjectDlg(CNiTimeControllerInterpolatorsDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CNiTimeControllerInterpolatorsDlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
void CNiTimeControllerInterpolatorsDlg::DoDataExchange(CDataExchange* pDX)
{
    CNiObjectDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNiTimeControllerInterpolatorsDlg)
    DDX_Control(pDX, IDC_NIINTERPOLATORS_LIST, m_wndInterpolatorsList);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNiTimeControllerInterpolatorsDlg, CNiObjectDlg)
    //{{AFX_MSG_MAP(CNiTimeControllerInterpolatorsDlg)
    ON_NOTIFY(NM_DBLCLK, IDC_NIINTERPOLATORS_LIST, 
        OnDblclkNiInterpolatorList)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNiTimeControllerInterpolatorsDlg message handlers
//---------------------------------------------------------------------------
BOOL CNiTimeControllerInterpolatorsDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    
    ASSERT(m_pkObj != NULL);
    ASSERT(NiIsKindOf(NiInterpController,m_pkObj ));
    m_wndInterpolatorsList.InsertColumn(0, "Class");
    m_wndInterpolatorsList.InsertColumn(1, "Name");
    CRect rect;
    m_wndInterpolatorsList.GetWindowRect(&rect);
    int cx = rect.Width()/2;
    m_wndInterpolatorsList.SetColumnWidth(0, cx - 4);
    m_wndInterpolatorsList.SetColumnWidth(1, cx);

    DoUpdate();
    return TRUE;
}
//---------------------------------------------------------------------------
void CNiTimeControllerInterpolatorsDlg::OnDblclkNiInterpolatorList(
    NMHDR* pNMHDR, LRESULT* pResult)
{

    *pResult = 0;
    
    if(m_wndInterpolatorsList.GetSelectedCount() == 0)
        return;

    int iIndex = m_wndInterpolatorsList.GetNextItem(-1, LVIS_SELECTED);

    if(iIndex == -1)
        return;

    int iMatchIndex = 0;
    NiInterpController* pkObj = (NiInterpController*) m_pkObj;
    NiInterpolator* pkInterp = NULL;
    
    pkInterp = pkObj->GetInterpolator(iIndex);

    if(pkInterp)
    {
        CNifPropertyWindowManager* pkManager = 
            CNifPropertyWindowManager::GetPropertyWindowManager();

        pkManager->CreatePropertyWindow(pkInterp);
    }
}
//---------------------------------------------------------------------------
bool CNiTimeControllerInterpolatorsDlg::DoUpdate()
{
    int nItem = 0;
    LPCTSTR lpszItem = NULL;
    NiInterpController* pkObj = (NiInterpController*) m_pkObj;

    m_wndInterpolatorsList.DeleteAllItems();
    for (unsigned short us = 0; us < pkObj->GetInterpolatorCount(); us++)
    {
        NiInterpolator* pkInterp =pkObj->GetInterpolator(us); 
        if(pkInterp)
        {
            lpszItem = pkInterp->GetRTTI()->GetName();
            const char* pcName  = pkObj->GetInterpolatorID(us);
            int i = m_wndInterpolatorsList.InsertItem( nItem++, lpszItem );
            m_wndInterpolatorsList.SetItemText(i, 1, pcName);
        }
    }

    return true;
}
//---------------------------------------------------------------------------
