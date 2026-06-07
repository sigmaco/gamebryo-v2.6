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

// NifListView.h

#ifndef NIFLISTVIEW_H
#define NIFLISTVIEW_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNifDoc;

class CNifListView : public CView
{
protected:
    CNifListView(); // protected constructor used by dynamic creation
    DECLARE_DYNCREATE(CNifListView)

// Attributes
public:

// Operations
public:
    CNifDoc* GetDocument();

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNifListView)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
    virtual void OnDraw(CDC* pDC);      // overridden to draw this view
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    //}}AFX_VIRTUAL

// Implementation
protected:
    virtual ~CNifListView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    // Generated message map functions
protected:
    void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
    void AddNifAtIndex(unsigned int uiIndex);
    void CNifListView::AddToListRecursive(NiAVObject* pkObject);
    void RePopulateSceneGraphList();
    void HighlightObject(NiAVObject* pkObject);

    CButton m_kOptionsButton;
    CComboBox m_kSceneGraphList;
    CFont* m_pkFont;
    bool m_bListExists;

    bool m_bDisplayNodes;
    bool m_bDisplayLights;
    bool m_bDisplayCameras;
    bool m_bDisplayGeometry;
    bool m_bDisplayCustom;
    CString m_strCustomString;
    bool m_bDisplayTimeControllers;
    bool m_bDisplayProperties;
    bool m_bDisplayDynamicEffects;
    bool m_bDisplayVisible;
    bool m_bDisplayHidden;

    //{{AFX_MSG(CNifListView)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnSelChange();
    afx_msg void OnDblClick();
    afx_msg void OnOptionsButtonClicked();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in NifListView.cpp
inline CNifDoc* CNifListView::GetDocument()
   { return (CNifDoc*)m_pDocument; }
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif // #ifndef NIFLISTVIEW_H
