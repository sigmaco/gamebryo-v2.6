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

// CameraInfoDlg.h

#ifndef CAMERAINFODLG_H
#define CAMERAINFODLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NiObjectDlg.h"

class CCameraInfoDlg : public CNiObjectDlg
{
// Construction
public:
    CCameraInfoDlg(CWnd* pParent = NULL);   // standard constructor
    virtual bool DoUpdate();
    virtual bool NeedsUpdateAfterCreation(){return true;}
// Dialog Data
    //{{AFX_DATA(CCameraInfoDlg)
    enum { IDD = IDD_CAMERA_INFO };
    CEdit   m_wndFrustumOrthoEdit;
    CEdit   m_wndWorldUpZEdit;
    CEdit   m_wndWorldUpYEdit;
    CEdit   m_wndWorldUpXEdit;
    CEdit   m_wndWorldRightZEdit;
    CEdit   m_wndWorldRightYEdit;
    CEdit   m_wndWorldRightXEdit;
    CEdit   m_wndWorldDirZEdit;
    CEdit   m_wndWorldDirXEdit;
    CEdit   m_wndViewportTopRightEdit;
    CEdit   m_wndViewportTopLeftEdit;
    CEdit   m_wndViewportBottomRightEdit;
    CEdit   m_wndViewportBottomLeftEdit;
    CEdit   m_wndLODAdjust;
    CEdit   m_wndFrustumTopEdit;
    CEdit   m_wndFrustumRightEdit;
    CEdit   m_wndFrustumNearEdit;
    CEdit   m_wndFrustumLeftEdit;
    CEdit   m_wndFrustumFarEdit;
    CEdit   m_wndFrustumBottomEdit;
    CEdit   m_wndWorldDirYEdit;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCameraInfoDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CCameraInfoDlg)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef CAMERAINFODLG_H
