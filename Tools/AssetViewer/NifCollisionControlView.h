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

// NifCollisionControlView.h

#ifndef NIFCOLLISIONCONTROLLER
#define NIFCOLLISIONCONTROLLER

#if !defined(NIFCOLLISIONCONTROLVIEW_H_2005_01_24)
#define NIFCOLLISIONCONTROLVIEW_H_2005_01_24

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NifCollisionControls.h"

class CNifCollisionControlView : public CScrollView  
{
    DECLARE_DYNCREATE(CNifCollisionControlView)

protected:
    CNifCollisionControlView();
    //virtual ~CNifCollisionControlView();

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCNifCollisionControlView)
    public:
    virtual void OnInitialUpdate();
    protected:
    virtual void OnDraw(CDC* pDC);      // overridden to draw this view
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
    //}}AFX_VIRTUAL

// Implementation
protected:
    virtual ~CNifCollisionControlView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    // Generated message map functions
protected:
    CNifCollisionControls m_wndDialog;

    //{{AFX_MSG(CCNifCollisionControlView)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif // !defined(NIFCOLLISIONCONTROLVIEW_H_2005_01_24)

#endif