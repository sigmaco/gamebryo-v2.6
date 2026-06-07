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

// NiGeometryDlg.h

#ifndef NIGEOMETRYDLG_H
#define NIGEOMETRYDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NiObjectDlg.h"

class CNiGeometryDlg : public CNiObjectDlg
{
// Construction
public:
    CNiGeometryDlg(CWnd* pParent = NULL);   // standard constructor

    bool DoUpdate();
    virtual bool NeedsUpdateAfterCreation(){return true;}
// Dialog Data
    //{{AFX_DATA(CNiGeometryDlg)
    enum { IDD = IDD_NIGEOMETRY };
    CEdit   m_wndShaderName;
    CEdit   m_wndNumSkinPartitions;
    CButton m_wndGeometryDataButton;
    CButton m_wndSkinInstanceButton;
    CEdit   m_wndVertexCountEdit;
    CEdit   m_wndModelBoundRadiusEdit;
    CEdit   m_wndTextureSetCountEdit;
    CEdit   m_wndModelBoundCenterZEdit;
    CEdit   m_wndModelBoundCenterYEdit;
    CEdit   m_wndModelBoundCenterXEdit;
    CEdit   m_wndActiveVertCountEdit;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNiGeometryDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CNiGeometryDlg)
    afx_msg void OnNigeometrySkinInstanceButton();
    afx_msg void OnGeometryDataButton();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef NIGEOMETRYDLG_H
