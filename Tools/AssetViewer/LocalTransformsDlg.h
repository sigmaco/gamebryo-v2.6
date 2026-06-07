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

// LocalTransformsDlg.h

#ifndef LOCALTRANSFORMSDLG_H
#define LOCALTRANSFORMSDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NiObjectDlg.h"

class CLocalTransformsDlg : public CNiObjectDlg
{
// Construction
public:
    CLocalTransformsDlg(CWnd* pParent = NULL);   // standard constructor
    virtual bool DoUpdate();
    virtual bool NeedsUpdateAfterCreation(){return true;}
// Dialog Data
    //{{AFX_DATA(CLocalTransformsDlg)
    enum { IDD = IDD_LOCAL_TRANSFORMS };
    CEdit   m_wndTranslateZEdit;
    CEdit   m_wndTranslateYEdit;
    CEdit   m_wndTranslateXEdit;
    CEdit   m_wndScaleEdit;
    CEdit   m_wndRotation3x3Edit;
    CEdit   m_wndRotation3x2Edit;
    CEdit   m_wndRotation3x1Edit;
    CEdit   m_wndRotation2x3Edit;
    CEdit   m_wndRotation2x2Edit;
    CEdit   m_wndRotation2x1Edit;
    CEdit   m_wndRotation1x3Edit;
    CEdit   m_wndRotation1x2Edit;
    CEdit   m_wndRotation1x1Edit;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CLocalTransformsDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CLocalTransformsDlg)
        // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef LOCALTRANSFORMSDLG_H
