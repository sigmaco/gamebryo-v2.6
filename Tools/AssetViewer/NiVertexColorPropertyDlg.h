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

// NiVertexColorPropertyDlg.h

#ifndef NIVERTEXCOLORPROPERTYDLG_H
#define NIVERTEXCOLORPROPERTYDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NiObjectDlg.h"

class CNiVertexColorPropertyDlg : public CNiObjectDlg
{
// Construction
public:
    CNiVertexColorPropertyDlg(CWnd* pParent = NULL);   // standard constructor
    virtual bool DoUpdate();
// Dialog Data
    //{{AFX_DATA(CNiVertexColorPropertyDlg)
    enum { IDD = IDD_VERTEXCOLORPROPERTY };
    CComboBox   m_wndVertexModeCombo;
    CComboBox   m_wndLightingModeCombo;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNiVertexColorPropertyDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CNiVertexColorPropertyDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef NIVERTEXCOLORPROPERTYDLG_H
