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

// RemoveFileDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "RemoveFileDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CRemoveFileDlg dialog
//---------------------------------------------------------------------------
CRemoveFileDlg::CRemoveFileDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CRemoveFileDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CRemoveFileDlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT

    m_pkNameToRoot = NULL;
    m_pkNodeToRemove = NULL;
}
//---------------------------------------------------------------------------
void CRemoveFileDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CRemoveFileDlg)
    DDX_Control(pDX, IDC_LIST_FILES, m_kFilesEdit);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CRemoveFileDlg, CDialog)
    //{{AFX_MSG_MAP(CRemoveFileDlg)
    ON_LBN_DBLCLK(IDC_LIST_FILES, OnDblclkListFiles)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CRemoveFileDlg message handlers
//---------------------------------------------------------------------------
BOOL CRemoveFileDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    NIASSERT(m_pkNameToRoot);


    CDC* pDC = m_kFilesEdit.GetDC();
    CSize TextSize;
    int iMaxWidth = 0;

    // Initialize the list box.
    NiTMapIterator kIter = m_pkNameToRoot->GetFirstPos();
    while (kIter)
    {
        const char* pcName;
        NiNode* pkNode;
        m_pkNameToRoot->GetNext(kIter, pcName, pkNode);

        m_kFilesEdit.AddString(pcName);

        // Get the Extent of the String
        TextSize = pDC->GetTextExtent(pcName);

        // Increase the max width if it is larger
        if (iMaxWidth < TextSize.cx)
            iMaxWidth = TextSize.cx;

    }

    // Get the Text Metrics
    TEXTMETRIC   tm;
    pDC->GetTextMetrics(&tm);

    // Add the avg width to prevent clipping
    iMaxWidth += tm.tmAveCharWidth;

    // Set the Horizontal Extent of the List Box
    m_kFilesEdit.SetHorizontalExtent(iMaxWidth);

    if (m_kFilesEdit.GetCount() > 0)
    {
        m_kFilesEdit.SetCurSel(0);
    }

    return TRUE;
}
//---------------------------------------------------------------------------
void CRemoveFileDlg::OnDblclkListFiles() 
{
    OnOK();
}
//---------------------------------------------------------------------------
void CRemoveFileDlg::OnOK() 
{
    CString strName;
    m_kFilesEdit.GetText(m_kFilesEdit.GetCurSel(), strName);
    NIVERIFY(m_pkNameToRoot->GetAt(strName, m_pkNodeToRemove));

    CDialog::OnOK();
}
//---------------------------------------------------------------------------
