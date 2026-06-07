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

// ProgressBarDlg.h

#ifndef PROGRESSBARDLG_H
#define PROGRESSBARDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CProgressBarDlg : public CDialog
{
// Construction
public:
    int GetUpperRange();
    int GetLowerRange();
    void SetPos(int iPos);
    void SetMessage(const char* pcMessage);
    void StepIt();
    void SetStep(int iStep);
    void SetRange(int iLower, int iUpper);
    CProgressBarDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
    //{{AFX_DATA(CProgressBarDlg)
    enum { IDD = IDD_PROGRESS_BAR };
    CStatic m_kMessage;
    CProgressCtrl   m_kProgressBar;
    CString m_strMessage;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CProgressBarDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual void PostNcDestroy();
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CProgressBarDlg)
        // NOTE: the ClassWizard will add member functions here
    virtual void OnOK();
    virtual void OnCancel();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef PROGRESSBARDLG_H
