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

// DiscardOpenFilesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AssetViewer.h"
#include "DiscardOpenFilesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDiscardOpenFilesDlg dialog


CDiscardOpenFilesDlg::CDiscardOpenFilesDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CDiscardOpenFilesDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CDiscardOpenFilesDlg)
    m_bDontPromptAgain = FALSE;
    //}}AFX_DATA_INIT
}


void CDiscardOpenFilesDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDiscardOpenFilesDlg)
    DDX_Check(pDX, IDC_CHECK_DONTPROMPTAGAIN, m_bDontPromptAgain);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDiscardOpenFilesDlg, CDialog)
    //{{AFX_MSG_MAP(CDiscardOpenFilesDlg)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiscardOpenFilesDlg message handlers
