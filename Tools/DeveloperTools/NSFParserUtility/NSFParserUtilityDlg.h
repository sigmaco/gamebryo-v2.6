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

// NSFParserUtilityDlg.h : header file
//

#if !defined(NSFPARSERUTILITYDLG_H)
#define NSFPARSERUTILITYDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// NSFParserUtilityDlg dialog

class NSFParserUtilityDlg : public CDialog
{
// Construction
public:
    NSFParserUtilityDlg(CWnd* pParent = NULL);  // standard constructor

// Dialog Data
    //{{AFX_DATA(NSFParserUtilityDlg)
    enum { IDD = IDD_NSFPARSERUTILITY_DIALOG };
    CListBox    m_CtrlList_Status;
    CEdit   m_CtrlEdit_FileOrFolder;
    CButton m_CtrlBtn_Recurse;
    CButton m_CtrlBtn_FileSelect;
    CButton m_CtrlBtn_ClearStatus;
    CStatic m_CtrlStatic_FileOrFolderName;
    CString m_strFileOrFolderString;
    BOOL    m_bRecurse;
    CString m_strFileOrFolder;
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(NSFParserUtilityDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
public:
    bool RemoveFileNameFromPath(CString& strFullPath);
    void RunParserUtility(void);

    static unsigned int ShaderErrorCallback(const char* pacError, 
        NiD3DError eError, bool bRecoverable);

protected:
    static NSFParserUtilityDlg* ms_pkDlg;
    HICON m_hIcon;

    // Generated message map functions
    //{{AFX_MSG(NSFParserUtilityDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnButtonClearList();
    afx_msg void OnButtonSelect();
    afx_msg void OnCheckRecurse();
    afx_msg void OnChangeEditFileOrFolder();
    virtual void OnCancel();
    virtual void OnOK();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before
// the previous line.

#endif // !defined(NSFPARSERUTILITYDLG_H)
