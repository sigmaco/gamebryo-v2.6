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

// ProgressBarDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "ProgressBarDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CProgressBarDlg dialog
//---------------------------------------------------------------------------
CProgressBarDlg::CProgressBarDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CProgressBarDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CProgressBarDlg)
    m_strMessage = _T("");
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
void CProgressBarDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CProgressBarDlg)
    DDX_Control(pDX, IDC_MESSAGE, m_kMessage);
    DDX_Control(pDX, IDC_PROGRESS_BAR, m_kProgressBar);
    DDX_Text(pDX, IDC_MESSAGE, m_strMessage);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CProgressBarDlg, CDialog)
    //{{AFX_MSG_MAP(CProgressBarDlg)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CProgressBarDlg message handlers
//---------------------------------------------------------------------------
int CProgressBarDlg::GetLowerRange()
{
    int iLower, iUpper;
    m_kProgressBar.GetRange(iLower, iUpper);
    return iLower;
}
//---------------------------------------------------------------------------
int CProgressBarDlg::GetUpperRange()
{
    int iLower, iUpper;
    m_kProgressBar.GetRange(iLower, iUpper);
    return iUpper;
}
//---------------------------------------------------------------------------
void CProgressBarDlg::SetRange(int iLower, int iUpper)
{
    NIASSERT(iLower < iUpper);
    m_kProgressBar.SetRange32(iLower, iUpper);
}
//---------------------------------------------------------------------------
void CProgressBarDlg::SetStep(int iStep)
{
    m_kProgressBar.SetStep(iStep);
}
//---------------------------------------------------------------------------
void CProgressBarDlg::StepIt()
{
    m_kProgressBar.StepIt();
}
//---------------------------------------------------------------------------
void CProgressBarDlg::SetMessage(const char* pcMessage)
{
    m_kMessage.SetWindowText(pcMessage);
}
//---------------------------------------------------------------------------
void CProgressBarDlg::SetPos(int iPos)
{
    m_kProgressBar.SetPos(iPos);
}
//---------------------------------------------------------------------------
void CProgressBarDlg::OnOK()
{
    // Do not allow OK to destroy window.
}
//---------------------------------------------------------------------------
void CProgressBarDlg::OnCancel()
{
    // Do not allow Cancel to destroy window.
}
//---------------------------------------------------------------------------
void CProgressBarDlg::PostNcDestroy() 
{
    CDialog::PostNcDestroy();

    delete this;
}
//---------------------------------------------------------------------------
