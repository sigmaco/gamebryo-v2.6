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

// ChooseKFMDlg.cpp

#include "stdafx.h"
#include "SceneViewer.h"
#include "ChooseKFMDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CChooseKFMDlg dialog
//---------------------------------------------------------------------------
CChooseKFMDlg::CChooseKFMDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CChooseKFMDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CChooseKFMDlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT

    m_pkKFMRoots = NULL;
    m_pkRoot = NULL;
}
//---------------------------------------------------------------------------
void CChooseKFMDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CChooseKFMDlg)
    DDX_Control(pDX, IDC_LIST_KFM_FILES, m_kKFMFilesList);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CChooseKFMDlg, CDialog)
    //{{AFX_MSG_MAP(CChooseKFMDlg)
    ON_LBN_DBLCLK(IDC_LIST_KFM_FILES, OnDblclkListKfmFiles)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CChooseKFMDlg message handlers
//---------------------------------------------------------------------------
BOOL CChooseKFMDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();

    assert(m_pkKFMRoots);

    // Populate list box.
    for (unsigned int ui = 0; ui < m_pkKFMRoots->GetSize(); ui++)
    {
        m_kKFMFilesList.AddString(m_pkKFMRoots->GetAt(ui)->GetName());
    }
    if (m_pkKFMRoots->GetSize() > 0)
    {
        m_kKFMFilesList.SetCurSel(0);
    }

    return TRUE;
}
//---------------------------------------------------------------------------
void CChooseKFMDlg::OnDblclkListKfmFiles() 
{
    OnOK();
}
//---------------------------------------------------------------------------
void CChooseKFMDlg::OnOK() 
{
    CString strName;
    m_kKFMFilesList.GetText(m_kKFMFilesList.GetCurSel(), strName);

    for (unsigned int ui = 0; ui < m_pkKFMRoots->GetSize(); ui++)
    {
        if (strName == m_pkKFMRoots->GetAt(ui)->GetName())
        {
            m_pkRoot = m_pkKFMRoots->GetAt(ui);
            break;
        }
    }

    CDialog::OnOK();
}
//---------------------------------------------------------------------------
