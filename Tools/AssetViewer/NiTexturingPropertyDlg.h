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

// NiTexturingPropertyDlg.h

#ifndef NITEXTURINGPROPERTYDLG_H
#define NITEXTURINGPROPERTYDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NiObjectDlg.h"

class NiTexturingProperty::Map;

class CNiTexturingPropertyDlg : public CNiObjectDlg
{
// Construction
public:
    CNiTexturingPropertyDlg(CWnd* pParent = NULL);   // standard constructor
    virtual bool DoUpdate();
    virtual bool NeedsUpdateAfterCreation(){return true;}
    void RebuildMapList();

// Dialog Data
    //{{AFX_DATA(CNiTexturingPropertyDlg)
    enum { IDD = IDD_NITEXTURINGPROPERTY };
    
    // Basic info
    CEdit   m_wndTextureNameEdit;
    CEdit   m_wndTextureClassEdit;
    CEdit   m_wndWidthEdit;
    CEdit   m_wndHeightEdit;
    CEdit   m_wndTextureSetEdit;

    // Map list box
    CListCtrl   m_wndMapList;
    
    // PS 2 MipMapping
    CEdit   m_wndLEdit;
    CEdit   m_wndKEdit;
    
    // Map settings
    CComboBox   m_wndPixelLayoutCombo;
    CComboBox   m_wndMipMapCombo;
    CComboBox   m_wndFilterModeCombo;
    CComboBox   m_wndClampModeCombo;
    CComboBox   m_wndApplyModeCombo;
    CComboBox   m_wndPlatformCombo;
    CComboBox   m_wndAlphaFormatCombo;

    //Bump map specific 
    CEdit   m_wndMatrix11Edit;
    CEdit   m_wndMatrix10Edit;
    CEdit   m_wndMatrix01Edit;
    CEdit   m_wndMatrix00Edit;
    CEdit   m_wndLumaOffsetEdit;
    CEdit   m_wndLumaScaleEdit;
    
    //Texture transforms settings
    CComboBox   m_wndTransformTypeCombo;
    CEdit   m_wndCenterUOffsetEdit;
    CEdit   m_wndCenterVOffsetEdit;
    CEdit   m_wndTilingUEdit;
    CEdit   m_wndTilingVEdit;
    CEdit   m_wndTranslateUEdit;
    CEdit   m_wndTranslateVEdit;
    CEdit   m_wndWRotationEdit;


    
    //}}AFX_DATA


// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CNiTexturingPropertyDlg)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation
protected:

    NiTPrimitiveArray<NiTexturingProperty::Map*> m_kMapArray;
    int m_iBumpMapIndex;
    // Generated message map functions
    //{{AFX_MSG(CNiTexturingPropertyDlg)
    virtual BOOL OnInitDialog();
    afx_msg void OnClkMapList(NMHDR* pNMHDR, LRESULT* pResult);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately
// before the previous line.

#endif  // #ifndef NITEXTURINGPROPERTYDLG_H
