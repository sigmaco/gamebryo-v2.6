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

// CameraInfoDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "CameraInfoDlg.h"
#include "NifPropertyWindowManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CCameraInfoDlg dialog
//---------------------------------------------------------------------------
CCameraInfoDlg::CCameraInfoDlg(CWnd* pParent /*=NULL*/)
    : CNiObjectDlg(CCameraInfoDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CCameraInfoDlg)
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
void CCameraInfoDlg::DoDataExchange(CDataExchange* pDX)
{
    CNiObjectDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CCameraInfoDlg)
    DDX_Control(pDX, IDC_CAMERA_FRUSTUM_ORTHO_EDIT, m_wndFrustumOrthoEdit);
    DDX_Control(pDX, IDC_CAMERA_WORLD_UP_Z_EDIT, m_wndWorldUpZEdit);
    DDX_Control(pDX, IDC_CAMERA_WORLD_UP_Y_EDIT, m_wndWorldUpYEdit);
    DDX_Control(pDX, IDC_CAMERA_WORLD_UP_X_EDIT, m_wndWorldUpXEdit);
    DDX_Control(pDX, IDC_CAMERA_WORLD_RIGHT_Z_EDIT, m_wndWorldRightZEdit);
    DDX_Control(pDX, IDC_CAMERA_WORLD_RIGHT_Y_EDIT, m_wndWorldRightYEdit);
    DDX_Control(pDX, IDC_CAMERA_WORLD_RIGHT_X_EDIT, m_wndWorldRightXEdit);
    DDX_Control(pDX, IDC_CAMERA_WORLD_DIR_Z_EDIT, m_wndWorldDirZEdit);
    DDX_Control(pDX, IDC_CAMERA_WORLD_DIR_X_EDIT, m_wndWorldDirXEdit);
    DDX_Control(pDX, IDC_CAMERA_VIEWPORT_TOPRIGHT_EDIT, 
        m_wndViewportTopRightEdit);
    DDX_Control(pDX, IDC_CAMERA_VIEWPORT_TOPLEFT_EDIT, 
        m_wndViewportTopLeftEdit);
    DDX_Control(pDX, IDC_CAMERA_VIEWPORT_BOTTOMRIGHT_EDIT, 
        m_wndViewportBottomRightEdit);
    DDX_Control(pDX, IDC_CAMERA_VIEWPORT_BOTTOMLEFT_EDIT,
        m_wndViewportBottomLeftEdit);
    DDX_Control(pDX, IDC_CAMERA_LOD_ADJUST, m_wndLODAdjust);
    DDX_Control(pDX, IDC_CAMERA_FRUSTUM_TOP_EDIT, m_wndFrustumTopEdit);
    DDX_Control(pDX, IDC_CAMERA_FRUSTUM_RIGHT_EDIT, m_wndFrustumRightEdit);
    DDX_Control(pDX, IDC_CAMERA_FRUSTUM_NEAR_EDIT, m_wndFrustumNearEdit);
    DDX_Control(pDX, IDC_CAMERA_FRUSTUM_LEFT_EDIT, m_wndFrustumLeftEdit);
    DDX_Control(pDX, IDC_CAMERA_FRUSTUM_FAR_EDIT, m_wndFrustumFarEdit);
    DDX_Control(pDX, IDC_CAMERA_FRUSTUM_BOTTOM_EDIT, m_wndFrustumBottomEdit);
    DDX_Control(pDX, IDC_CAMERA_WORLD_DIR_Y_EDIT, m_wndWorldDirYEdit);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CCameraInfoDlg, CNiObjectDlg)
    //{{AFX_MSG_MAP(CCameraInfoDlg)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CCameraInfoDlg message handlers
//---------------------------------------------------------------------------
bool CCameraInfoDlg::DoUpdate()
{
    if(!NiIsKindOf(NiCamera, m_pkObj))
        return false;

    NiCamera* pkCam = (NiCamera*) m_pkObj;
    char acString[256];
    NiPoint3 kPoint = (NiPoint3)pkCam->GetWorldUpVector();
    
    NiSprintf(acString, 256, "%.4f", kPoint.z);
    m_wndWorldUpZEdit.SetWindowText(acString);

    NiSprintf(acString, 256, "%.4f", kPoint.y);
    m_wndWorldUpYEdit.SetWindowText(acString);
    
    NiSprintf(acString, 256, "%.4f", kPoint.x);
    m_wndWorldUpXEdit.SetWindowText(acString);

    kPoint = pkCam->GetWorldRightVector();
    NiSprintf(acString, 256,"%.4f", kPoint.z);
    m_wndWorldRightZEdit.SetWindowText(acString);
    
    NiSprintf(acString, 256, "%.4f", kPoint.y);
    m_wndWorldRightYEdit.SetWindowText(acString);
    
    NiSprintf(acString, 256, "%.4f", kPoint.x);
    m_wndWorldRightXEdit.SetWindowText(acString);

    kPoint = pkCam->GetWorldDirection();
    NiSprintf(acString, 256, "%.4f", kPoint.z);
    m_wndWorldDirZEdit.SetWindowText(acString);
    
    NiSprintf(acString, 256, "%.4f", kPoint.x);
    m_wndWorldDirXEdit.SetWindowText(acString);
    
    NiSprintf(acString, 256, "%.4f", kPoint.y);
    m_wndWorldDirYEdit.SetWindowText(acString);

    NiRect<float> pkRect = pkCam->GetViewPort();
    NiSprintf(acString, 256, "%.4f", pkRect.m_right);
    m_wndViewportTopRightEdit.SetWindowText(acString);
    NiSprintf(acString, 256, "%.4f", pkRect.m_left);
    m_wndViewportTopLeftEdit.SetWindowText(acString);
    NiSprintf(acString, 256, "%.4f", pkRect.m_bottom);
    m_wndViewportBottomRightEdit.SetWindowText(acString);
    NiSprintf(acString, 256, "%.4f", pkRect.m_top);
    m_wndViewportBottomLeftEdit.SetWindowText(acString);

    NiSprintf(acString, 256, "%.4f", pkCam->GetLODAdjust());
    m_wndLODAdjust.SetWindowText(acString);
    
    NiFrustum kFrustum = pkCam->GetViewFrustum();

    NiSprintf(acString, 256, "%.4f", kFrustum.m_fTop );
    m_wndFrustumTopEdit.SetWindowText(acString);
    NiSprintf(acString, 256, "%.4f", kFrustum.m_fBottom );
    m_wndFrustumBottomEdit.SetWindowText(acString);
    NiSprintf(acString, 256, "%.4f", kFrustum.m_fLeft );
    m_wndFrustumLeftEdit.SetWindowText(acString);
    NiSprintf(acString, 256, "%.4f", kFrustum.m_fRight );
    m_wndFrustumRightEdit.SetWindowText(acString);
    NiSprintf(acString, 256, "%.4f", kFrustum.m_fNear );
    m_wndFrustumNearEdit.SetWindowText(acString);
    NiSprintf(acString, 256, "%.4f", kFrustum.m_fFar );
    m_wndFrustumFarEdit.SetWindowText(acString);
    if (kFrustum.m_bOrtho)
        m_wndFrustumOrthoEdit.SetWindowText("TRUE");
    else
        m_wndFrustumOrthoEdit.SetWindowText("FALSE");

    return true;

}
//---------------------------------------------------------------------------
