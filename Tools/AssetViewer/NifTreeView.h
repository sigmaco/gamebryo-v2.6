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

// NifTreeView.h

#ifndef NIFTREEVIEW_H
#define NIFTREEVIEW_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNifDoc;

class CNifTreeView : public CTreeView
{
protected:
    CNifTreeView(); // protected constructor used by dynamic creation
    DECLARE_DYNCREATE(CNifTreeView)

    enum
    {
        IM_WORLD,
        IM_NODE,
        IM_LIGHT,
        IM_CAMERA,
        IM_GEOMETRY,
        IM_GEOMDATA,
        IM_ATTRIBUTES,
        IM_ATTRIBITEM
    };

// Attributes
public:
    CNifDoc* GetDocument();

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNifTreeView)
    public:
    virtual void OnInitialUpdate();
    protected:
    virtual void OnDraw(CDC* pDC);      // overridden to draw this view
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    //}}AFX_VIRTUAL

// Implementation
protected:
    virtual ~CNifTreeView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    // Generated message map functions
protected:
    bool HighlightObject(HTREEITEM hItem, NiAVObject* pkObject);
    void ExpandRecursive(HTREEITEM hItem, UINT nCode);
    CImageList* m_pilImages;
    void RemoveNifAtIndex(unsigned int uiIndex);
    void AddNifAtIndex(unsigned int uiIndex);
    void UpdateDefaultNode();
    HTREEITEM m_hSceneRoot;
    void PopulateTreeCtrl();
    void RepopulateTreeCtrl();
    void FillTreeRecursive(HTREEITEM hParent, NiNode* pkNode,
        HTREEITEM hInsertAfter = TVI_LAST);
    //{{AFX_MSG(CNifTreeView)
    afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnRemoveNif();
    afx_msg void OnExpandAll();
    afx_msg void OnCollapseAll();
    afx_msg void OnProperties();
    afx_msg void OnToggleAppCulled();
    afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnReturn(NMHDR* pNMHDR, LRESULT* pResult);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in NifTreeView.cpp
inline CNifDoc* CNifTreeView::GetDocument()
   { return (CNifDoc*)m_pDocument; }
#endif

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef NIFTREEVIEW_H
