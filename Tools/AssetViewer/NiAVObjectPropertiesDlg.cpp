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

// NiAVObjectPropertiesDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiAVObjectPropertiesDlg.h"
#include "NifPropertyWindowManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CNiAVObjectPropertiesDlg dialog
//---------------------------------------------------------------------------
CNiAVObjectPropertiesDlg::CNiAVObjectPropertiesDlg(CWnd* pParent /*=NULL*/)
    : CNiObjectDlg(CNiAVObjectPropertiesDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CNiAVObjectPropertiesDlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
void CNiAVObjectPropertiesDlg::DoDataExchange(CDataExchange* pDX)
{
    CNiObjectDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNiAVObjectPropertiesDlg)
    DDX_Control(pDX, IDC_NIPROPERTIES_LIST, m_wndPropertiesList);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNiAVObjectPropertiesDlg, CNiObjectDlg)
    //{{AFX_MSG_MAP(CNiAVObjectPropertiesDlg)
    ON_NOTIFY(NM_DBLCLK, IDC_NIPROPERTIES_LIST, OnDblclkNipropertiesList)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNiAVObjectPropertiesDlg message handlers
//---------------------------------------------------------------------------
void CNiAVObjectPropertiesDlg::OnDblclkNipropertiesList(NMHDR* pNMHDR,
    LRESULT* pResult) 
{
    LV_DISPINFO* pkInfo = (LV_DISPINFO*) pNMHDR;
    if(m_wndPropertiesList.GetSelectedCount() == 0)
        return;

    int iIndex = m_wndPropertiesList.GetNextItem(-1, LVIS_SELECTED);

    if(iIndex == -1)
        return;

    int iMatchIndex = 0;
    NiAVObject* pkObj = (NiAVObject*) m_pkObj;
    NiPropertyList* pkList = &pkObj->GetPropertyList();
    NiProperty* pkProperty = NULL;
    
    NiTListIterator kIter = pkList->GetHeadPos();
    while(kIter && iMatchIndex != iIndex)
    {
        pkProperty = pkList->GetNext(kIter); 
        if(pkProperty)
            iMatchIndex++;
    }

    if(kIter && iMatchIndex == iIndex)
        pkProperty = pkList->GetNext(kIter); 
    else
        pkProperty = NULL;

    if(pkProperty)
    {
        CNifPropertyWindowManager* pkManager = 
            CNifPropertyWindowManager::GetPropertyWindowManager();

        pkManager->CreatePropertyWindow(pkProperty);
    }

    *pResult = 0;
}
//---------------------------------------------------------------------------
bool  CNiAVObjectPropertiesDlg::DoUpdate()
{
    int nItem = 0;
    LPCTSTR lpszItem = NULL;
    NiAVObject* pkObj = (NiAVObject*) m_pkObj;
    NiPropertyList* pkList = &pkObj->GetPropertyList();

    m_wndPropertiesList.DeleteAllItems();
    NiTListIterator kIter = pkList->GetHeadPos();
    while(kIter)
    {
        NiProperty* pkProperty = pkList->GetNext(kIter); 
        if(pkProperty)
        {
            lpszItem = pkProperty->GetRTTI()->GetName();
            const char* pcName  = pkProperty->GetName();
            int i = m_wndPropertiesList.InsertItem( nItem++, lpszItem );
            m_wndPropertiesList.SetItemText(i, 1, pcName);
            
        }
    }

    return true;
}
//---------------------------------------------------------------------------
BOOL CNiAVObjectPropertiesDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    
    ASSERT(this->m_pkObj != NULL && NiIsKindOf(NiAVObject, m_pkObj));
    m_wndPropertiesList.InsertColumn(0, "Class");
    m_wndPropertiesList.InsertColumn(01, "Name");
    CRect rect;
    m_wndPropertiesList.GetWindowRect(&rect);
    int cx = rect.Width()/2;
    m_wndPropertiesList.SetColumnWidth(0, cx - 4);
    m_wndPropertiesList.SetColumnWidth(1, cx);

    DoUpdate();
    return TRUE;
}
//---------------------------------------------------------------------------
