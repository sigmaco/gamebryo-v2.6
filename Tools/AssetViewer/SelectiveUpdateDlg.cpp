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

// SelectiveUpdateDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "SelectiveUpdateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CSelectiveUpdateDlg dialog
//---------------------------------------------------------------------------
CSelectiveUpdateDlg::CSelectiveUpdateDlg(CWnd* pParent /*=NULL*/)
    : CNiObjectDlg(CSelectiveUpdateDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CSelectiveUpdateDlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
void CSelectiveUpdateDlg::DoDataExchange(CDataExchange* pDX)
{
    CNiObjectDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSelectiveUpdateDlg)
    DDX_Control(pDX, IDC_APPCULLED, m_wndAppCulled);
    DDX_Control(pDX, IDC_SELECTIVE_UPDATE_TRANSFORMS, m_wndTransformsCheck);
    DDX_Control(pDX, IDC_SELECTIVE_UPDATE_RIGID, m_wndRigidCheck);
    DDX_Control(pDX, IDC_SELECTIVE_UPDATE_PROPERTY_CONTROLLERS, 
        m_wndPropertyControllersCheck);
    DDX_Control(pDX, IDC_SELECTIVE_UPDATE, m_wndSelectiveUpdateCheck);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CSelectiveUpdateDlg, CNiObjectDlg)
    //{{AFX_MSG_MAP(CSelectiveUpdateDlg)
    ON_BN_CLICKED(IDC_APPCULLED, OnAppculled)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CSelectiveUpdateDlg message handlers
//---------------------------------------------------------------------------
bool CSelectiveUpdateDlg::DoUpdate()
{
    if(!NiIsKindOf(NiAVObject, m_pkObj))
        return false;

    NiAVObject* pkObj = (NiAVObject*) m_pkObj;

    m_wndAppCulled.SetCheck(pkObj->GetAppCulled());
    m_wndTransformsCheck.SetCheck(pkObj->GetSelectiveUpdateTransforms());
    m_wndRigidCheck.SetCheck(pkObj->GetSelectiveUpdateRigid());
    m_wndPropertyControllersCheck.SetCheck(
        pkObj->GetSelectiveUpdatePropertyControllers());
    m_wndSelectiveUpdateCheck.SetCheck(pkObj->GetSelectiveUpdate());

    return true;
}
//---------------------------------------------------------------------------

void CSelectiveUpdateDlg::OnAppculled() 
{
    NiAVObject* pkObj = (NiAVObject*) m_pkObj;
    if (pkObj)
        pkObj->SetAppCulled(!pkObj->GetAppCulled());
}
//---------------------------------------------------------------------------
