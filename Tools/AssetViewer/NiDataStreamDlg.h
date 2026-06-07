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

// NiDataStreamDlg.h

#ifndef NIDATASTREAMDLG_H
#define NIDATASTREAMDLG_H

#include <NiObject.h>
#include <NiFixedString.h>

#include "NiObjectDlg.h"

#include "afxwin.h"

class NiDataStreamObject : public NiObject
{
    NiDeclareRTTI;

public:
    NiDataStreamObject();

    NiMesh* m_pkMesh;
    NiFixedString m_kSemantic;
    NiUInt32 m_uiSemanticIndex;
};
NiSmartPointer(NiDataStreamObject);

class CNiDataStreamDlg : public CNiObjectDlg
{
// Construction
public:
    CNiDataStreamDlg(CWnd* pParent = NULL);   // standard constructor
    ~CNiDataStreamDlg();

    bool DoUpdate();

    virtual bool NeedsUpdateAfterCreation(){return true;}
    virtual void SetNiObject(NiObject* pkObj);
// Dialog Data
    //{{AFX_DATA(CNiDataStreamDlg)
    enum { IDD = IDD_NIDATASTREAM };
    CListCtrl   m_wndDataList;
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNiDataStreamDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:
    // Generated message map functions
    //{{AFX_MSG(CNiDataStreamDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private:
    NiDataStreamObjectPtr m_kDataStreamObject;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef NIDATASTREAMDLG_H
