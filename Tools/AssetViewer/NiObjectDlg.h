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

// NiObjectDlg.h

#ifndef NIOBJECTDLG_H
#define NIOBJECTDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNiObjectDlg : public CDialog
{
// Construction
public:
    CNiObjectDlg( LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL );
    CNiObjectDlg( UINT nIDTemplate, CWnd* pParentWnd = NULL );
    CNiObjectDlg( );

// Dialog Data
    //{{AFX_DATA(CNiObjectDlg)
    
    //}}AFX_DATA

    virtual bool DoUpdate() = 0;

    virtual void SetNiObject(NiObject* pkObj);
    virtual bool NeedsUpdateAfterCreation() {return false;}

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNiObjectDlg)
    //}}AFX_VIRTUAL

// Implementation
protected:
    NiObject* m_pkObj;

    virtual BOOL OnInitDialog();
    virtual void OnOK() {};
    virtual void OnCancel(){};

    // Generated message map functions
    //{{AFX_MSG(CNiObjectDlg)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef NIOBJECTDLG_H
