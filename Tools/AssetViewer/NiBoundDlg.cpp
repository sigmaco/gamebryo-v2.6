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

// NiBoundDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiBoundDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CNiBoundDlg dialog
//---------------------------------------------------------------------------
CNiBoundDlg::CNiBoundDlg(CWnd* pParent /*=NULL*/)
    : CNiObjectDlg(CNiBoundDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CNiBoundDlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
void CNiBoundDlg::DoDataExchange(CDataExchange* pDX)
{
    CNiObjectDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNiBoundDlg)
    DDX_Control(pDX, IDC_BOUND_RADIUS_EDIT, m_wndRadiusEdit);
    DDX_Control(pDX, IDC_BOUND_CENTER_Z_EDIT_WORLD, m_wndCenterZEdit);
    DDX_Control(pDX, IDC_BOUND_CENTER_Y_EDIT_WORLD, m_wndCenterYEdit);
    DDX_Control(pDX, IDC_BOUND_CENTER_X_EDIT_WORLD, m_wndCenterXEdit);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNiBoundDlg, CNiObjectDlg)
    //{{AFX_MSG_MAP(CNiBoundDlg)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNiBoundDlg message handlers
//---------------------------------------------------------------------------
bool CNiBoundDlg::DoUpdate()
{
    if(!NiIsKindOf(NiAVObject, m_pkObj))
        return false;

    NiAVObject* pkObj = (NiAVObject*) m_pkObj;
    
    NiBound kBound = pkObj->GetWorldBound();
    char acString[256];
    NiPoint3 kPoint = kBound.GetCenter();

    NiSprintf(acString, 256, "%.4f", kBound.GetRadius());
    m_wndRadiusEdit.SetWindowText(acString);

    NiSprintf(acString, 256, "%.4f", kPoint.z);
    m_wndCenterZEdit.SetWindowText(acString);
    
    NiSprintf(acString, 256, "%.4f", kPoint.y);
    m_wndCenterYEdit.SetWindowText(acString);
    
    NiSprintf(acString, 256, "%.4f", kPoint.x);
    m_wndCenterXEdit.SetWindowText(acString);

    return true;

}
//---------------------------------------------------------------------------
