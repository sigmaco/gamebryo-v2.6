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

// NiNodeEffectsDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiNodeEffectsDlg.h"
#include "NifPropertyWindowManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CNiNodeEffectsDlg dialog
//---------------------------------------------------------------------------
CNiNodeEffectsDlg::CNiNodeEffectsDlg(CWnd* pParent /*=NULL*/)
    : CNiObjectDlg(CNiNodeEffectsDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CNiNodeEffectsDlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
void CNiNodeEffectsDlg::DoDataExchange(CDataExchange* pDX)
{
    CNiObjectDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNiNodeEffectsDlg)
    DDX_Control(pDX, IDC_NIEFFECTS_LIST, m_wndDynamicEffectsList);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNiNodeEffectsDlg, CNiObjectDlg)
    //{{AFX_MSG_MAP(CNiNodeEffectsDlg)
    ON_NOTIFY(NM_DBLCLK, IDC_NIEFFECTS_LIST, OnDblclkNieffectsList)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNiNodeEffectsDlg message handlers
//---------------------------------------------------------------------------
BOOL CNiNodeEffectsDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    
    ASSERT(m_pkObj != NULL);
    ASSERT(NiIsKindOf(NiNode,m_pkObj ));
    m_wndDynamicEffectsList.InsertColumn(0, "Class");
    m_wndDynamicEffectsList.InsertColumn(1, "Name");
    CRect rect;
    m_wndDynamicEffectsList.GetWindowRect(&rect);
    int cx = rect.Width()/2;
    m_wndDynamicEffectsList.SetColumnWidth(0, cx - 4);
    m_wndDynamicEffectsList.SetColumnWidth(1, cx);

    DoUpdate();
    return TRUE;
}
//---------------------------------------------------------------------------
void CNiNodeEffectsDlg::OnDblclkNieffectsList(NMHDR* pNMHDR, LRESULT* pResult)
{

    *pResult = 0;
    
    if(m_wndDynamicEffectsList.GetSelectedCount() == 0)
        return;

    int iIndex = m_wndDynamicEffectsList.GetNextItem(-1, LVIS_SELECTED);

    if(iIndex == -1)
        return;

    int iMatchIndex = 0;
    NiNode* pkObj = (NiNode*) m_pkObj;
    const NiDynamicEffectList* pkList = &pkObj->GetEffectList();
    NiDynamicEffect* pkDynamicEffect = NULL;
    
    NiTListIterator kIter = pkList->GetHeadPos();
    while(kIter && iMatchIndex != iIndex)
    {
        pkDynamicEffect = pkList->GetNext(kIter); 
        if(pkDynamicEffect)
            iMatchIndex++;
    }

    if(kIter && iMatchIndex == iIndex)
        pkDynamicEffect = pkList->GetNext(kIter); 
    else
        pkDynamicEffect = NULL;

    if(pkDynamicEffect)
    {
        CNifPropertyWindowManager* pkManager = 
            CNifPropertyWindowManager::GetPropertyWindowManager();

        pkManager->CreatePropertyWindow(pkDynamicEffect);
    }
}
//---------------------------------------------------------------------------
bool CNiNodeEffectsDlg::DoUpdate()
{
    int nItem = 0;
    LPCTSTR lpszItem = NULL;
    NiNode* pkObj = (NiNode*) m_pkObj;
    const NiDynamicEffectList* pkList = &pkObj->GetEffectList();

    NiTListIterator kIter = pkList->GetHeadPos();
    m_wndDynamicEffectsList.DeleteAllItems();
    while(kIter )
    {
        NiDynamicEffect* pkDynamicEffect = pkList->GetNext(kIter); 
        if(pkDynamicEffect)
        {
            lpszItem = pkDynamicEffect->GetRTTI()->GetName();
            const char* pcName  = pkDynamicEffect->GetName();
            int i = m_wndDynamicEffectsList.InsertItem( nItem++, lpszItem );
            m_wndDynamicEffectsList.SetItemText(i, 1, pcName);
            
        }
    }

    return true;

}
//---------------------------------------------------------------------------
