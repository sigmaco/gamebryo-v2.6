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

// NiGeometryDataAttributesInfoDlg.h

#ifndef NIGEOMETRYDATAATTRIBUTESINFODLG_H
#define NIGEOMETRYDATAATTRIBUTESINFODLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NiObjectDlg.h"

class CNiGeometryDataAttributesInfoDlg : public CNiObjectDlg
{
// Construction
public:
// standard constructor
    CNiGeometryDataAttributesInfoDlg(CWnd* pParent = NULL); 

// Dialog Data
    //{{AFX_DATA(CNiGeometryDataAttributesInfoDlg)
    enum { IDD = IDD_GEOMETRY_DATA_ATTRIBUTES };
    CEdit   m_wndConsistencyEdit;
    CButton m_wndKeepPositions;
    CButton m_wndKeepNormals;
    CButton m_wndKeepColors;
    CButton m_wndKeepUVs;
    CButton m_wndKeepIndices;
    CButton m_wndKeepBoneData;
    CButton m_wndCompressPosition;
    CButton m_wndCompressNormals;
    CButton m_wndCompressColors;
    CButton m_wndCompressUVs;
    CButton m_wndCompressWeights;
    //}}AFX_DATA

    virtual bool DoUpdate();

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNiGeometryDataAttributesInfoDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    // Generated message map functions
    //{{AFX_MSG(CNiGeometryDataAttributesInfoDlg)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef NiGeometryDataAttributesINFODLG_H
