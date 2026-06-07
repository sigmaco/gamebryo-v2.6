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

// NiObjectDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiObjectDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CNiObjectDlg dialog
//---------------------------------------------------------------------------
CNiObjectDlg::CNiObjectDlg(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
    : CDialog(lpszTemplateName, pParentWnd)
{
    m_pkObj = NULL;
}
//---------------------------------------------------------------------------
CNiObjectDlg::CNiObjectDlg(UINT nIDTemplate, CWnd* pParentWnd)
    : CDialog(nIDTemplate, pParentWnd)
{
    m_pkObj = NULL;
}
//---------------------------------------------------------------------------
CNiObjectDlg::CNiObjectDlg()
    : CDialog()
{
    m_pkObj = NULL;
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNiObjectDlg, CDialog)
    //{{AFX_MSG_MAP(CNiObjectDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNiObjectDlg message handlers
//---------------------------------------------------------------------------
void CNiObjectDlg::SetNiObject(NiObject* pkObj)
{
    ASSERT(pkObj != NULL); 
    m_pkObj = pkObj;
}
//---------------------------------------------------------------------------
BOOL CNiObjectDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    
    ASSERT(m_pkObj != NULL);
    DoUpdate();

    return TRUE;
}
//---------------------------------------------------------------------------
