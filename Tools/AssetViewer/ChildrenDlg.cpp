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

// ChildrenDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "ChildrenDlg.h"
#include "NifPropertyWindowManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CChildrenDlg dialog
//---------------------------------------------------------------------------
CChildrenDlg::CChildrenDlg(CWnd* pParent /*=NULL*/)
    : CNiObjectDlg(CChildrenDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CChildrenDlg)
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
void CChildrenDlg::DoDataExchange(CDataExchange* pDX)
{
    CNiObjectDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CChildrenDlg)
    DDX_Control(pDX, IDC_SUBTREE_OBJECT_DEPTH_COMPLEXITY_EDIT,
        m_wndSubTreeDepthComplexityEdit);
    DDX_Control(pDX, IDC_SUBTREE_OBJECT_COUNT_EDIT, 
        m_wndSubTreeObjectCountEdit);
    DDX_Control(pDX, IDC_CHILD_COUNT_EDIT, m_wndChildCountEdit);
    DDX_Control(pDX, IDC_CHILDREN_LIST, m_wndChildrenList);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CChildrenDlg, CNiObjectDlg)
    //{{AFX_MSG_MAP(CChildrenDlg)
    ON_NOTIFY(NM_DBLCLK, IDC_CHILDREN_LIST, OnDblclkChildrenList)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CChildrenDlg message handlers
//---------------------------------------------------------------------------
void CChildrenDlg::OnDblclkChildrenList(NMHDR* pNMHDR, LRESULT* pResult) 
{
    *pResult = 0;
    if( m_wndChildrenList.GetSelectedCount() == 0)
        return;

    int iIndex =  m_wndChildrenList.GetNextItem(-1, LVIS_SELECTED);

    if(iIndex == -1)
        return;

    NiNode* pkNode = (NiNode*) m_pkObj;
    NiAVObject* pkChild = NULL;

    int iMatchIndex = 0;

    for(unsigned int ui =0; ui < pkNode->GetArrayCount() ; ui++)
    {
        pkChild = pkNode->GetAt(ui);
        if(pkChild)
        {
            if(iMatchIndex == iIndex)
            {
                CNifPropertyWindowManager* pkManager = 
                    CNifPropertyWindowManager::GetPropertyWindowManager();

                if(pkManager)
                {
                    pkManager->CreatePropertyWindow(pkChild);
                    return;
                }
            }
            iMatchIndex++;
        }
    }
    
}
//---------------------------------------------------------------------------
bool CChildrenDlg::DoUpdate()
{
    ASSERT(m_pkObj != NULL);
    LPCTSTR lpszItem = NULL;

    int nItem = 0;
    NiNode* pkNode = (NiNode*) m_pkObj;
    NiAVObject* pkChild = NULL;

    int iChildCount = pkNode->GetChildCount();
    char acString[256];

    m_wndChildrenList.DeleteAllItems();
    for(unsigned int ui =0; ui < pkNode->GetArrayCount() ; ui++)
    {
        pkChild = pkNode->GetAt(ui);
        if(pkChild)
        {
            lpszItem = pkChild->GetRTTI()->GetName();
            const char* pcName  = pkChild->GetName();
            NiSprintf(acString, 256, "%s", pcName);
            int i = m_wndChildrenList.InsertItem( nItem++, acString  );
            m_wndChildrenList.SetItemText(i, 1, lpszItem);
        }
    }
    NiSprintf(acString, 256, "%d", iChildCount);
    m_wndChildCountEdit.SetWindowText(acString);

    int iDepth = -1;
    int iTotalChildCount = 0;
    GatherRecursiveInfo(pkNode, iDepth, iTotalChildCount);
    
    NiSprintf(acString, 256, "%d", iDepth);
    m_wndSubTreeDepthComplexityEdit.SetWindowText(acString);

    NiSprintf(acString, 256, "%d", iTotalChildCount);
    m_wndSubTreeObjectCountEdit.SetWindowText(acString);

    return true;

}
//---------------------------------------------------------------------------
BOOL CChildrenDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    ASSERT(m_pkObj != NULL);
    ASSERT(NiIsKindOf(NiNode,m_pkObj ));
    m_wndChildrenList.InsertColumn(0, "Name");
    m_wndChildrenList.InsertColumn(1, "Class");
    CRect rect;
    m_wndChildrenList.GetWindowRect(&rect);
    int cx = rect.Width()/2;
    m_wndChildrenList.SetColumnWidth(0, cx - 4);
    m_wndChildrenList.SetColumnWidth(1, cx);

    DoUpdate();
    
    return TRUE;
}
//---------------------------------------------------------------------------
void CChildrenDlg::GatherRecursiveInfo(NiAVObject* pkObj,
    int& iDepthComplexity, int& iSubTreeItemCount)
{
    if(pkObj == NULL)
    {
        return;
    }
    
    iDepthComplexity++;
    iSubTreeItemCount++;

    if(NiIsKindOf(NiNode, pkObj))
    {
        int iMaxDepth = 0;
        NiNode* pkNode = (NiNode*) pkObj;
        for(unsigned int ui = 0; ui < pkNode->GetArrayCount(); ui++)
        {
            int iDepth = 0;
            GatherRecursiveInfo(pkNode->GetAt(ui), iDepth, iSubTreeItemCount);
            if(iDepth > iMaxDepth)
                iMaxDepth = iDepth;
        }

        iDepthComplexity += iMaxDepth;
    }
}
//---------------------------------------------------------------------------
