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

// CollisionDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "CollisionDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CCollisionDlg dialog
//---------------------------------------------------------------------------
CCollisionDlg::CCollisionDlg(CWnd* pParent /*=NULL*/)
    : CNiObjectDlg(CCollisionDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CCollisionDlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
void CCollisionDlg::DoDataExchange(CDataExchange* pDX)
{
    CNiObjectDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CCollisionDlg)
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CCollisionDlg, CNiObjectDlg)
    //{{AFX_MSG_MAP(CCollisionDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CCollisionDlg message handlers
//---------------------------------------------------------------------------
BOOL CCollisionDlg::OnInitDialog() 
{  
    CDialog::OnInitDialog();
    ASSERT(this->m_pkObj != NULL && NiIsKindOf(NiAVObject, m_pkObj));

    DoUpdate();
    
    return TRUE;
}
//---------------------------------------------------------------------------
bool CCollisionDlg::DoUpdate()
{
    if(m_pkObj == NULL || !NiIsKindOf(NiAVObject, m_pkObj))
        return false;
    NiAVObject* pkObj = (NiAVObject*) m_pkObj;
    return true;
}
//---------------------------------------------------------------------------
