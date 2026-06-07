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

// NiKeyBasedInterpolatorKeysDlg.h

#ifndef NIKEYBASEDINTERPOLATORKEYSDLG_H
#define NIKEYBASEDINTERPOLATORKEYSDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NiObjectDlg.h"

class CNiKeyBasedInterpolatorKeysDlg : public CNiObjectDlg
{
// Construction
public:
    // standard constructor
    CNiKeyBasedInterpolatorKeysDlg(CWnd* pParent = NULL); 
    bool DoUpdate();

    unsigned int m_uiIndex;
// Dialog Data
    //{{AFX_DATA(CNiKeyBasedInterpolatorKeysDlg)
    enum { IDD = IDD_NIANIMATIONKEY };
    CListCtrl   m_wndAnimationKeysList;
    CEdit       m_wndContentEdit;
    CEdit       m_wndTypeEdit;
    CEdit       m_wndChannelEdit;
    CEdit       m_wndCountEdit;
    CEdit       m_wndPoseEdit;
    CButton     m_wndPrevButton;
    CButton     m_wndNextButton;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNiKeyBasedInterpolatorKeysDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    unsigned int GetNumberOfColumns(NiKeyBasedInterpolator* pkInterp,
        unsigned short usIndex);
    void FillInList(CListCtrl& kList, NiKeyBasedInterpolator* pkInterp, 
        unsigned short usIndex);
    CString GetContentType(NiKeyBasedInterpolator* pkInterp, 
        unsigned short usIndex);
    CString GetType(NiKeyBasedInterpolator* pkInterp, 
        unsigned short usIndex);
    CString GetCount(NiKeyBasedInterpolator* pkInterp, 
        unsigned short usIndex);
    CString GetPose(NiKeyBasedInterpolator* pkInterp, 
        unsigned short usIndex);

    void InsertAnimationKey(int iListIndex, char* pcIndexName, 
        NiAnimationKey* pkKey, NiAnimationKey::KeyContent eContent);

    // Generated message map functions
    //{{AFX_MSG(CNiKeyBasedInterpolatorKeysDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnDblclkAnimationKeyList(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnPrevButton();
    afx_msg void OnNextButton();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef NIKEYBASEDINTERPOLATORKEYSDLG_H