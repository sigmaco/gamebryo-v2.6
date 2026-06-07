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

// LocalTransformsDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "LocalTransformsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CLocalTransformsDlg dialog
//---------------------------------------------------------------------------
CLocalTransformsDlg::CLocalTransformsDlg(CWnd* pParent /*=NULL*/)
    : CNiObjectDlg(CLocalTransformsDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CLocalTransformsDlg)
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
void CLocalTransformsDlg::DoDataExchange(CDataExchange* pDX)
{
    CNiObjectDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CLocalTransformsDlg)
    DDX_Control(pDX, IDC_TRANSFORMS_TRANSLATE_Z_EDIT, m_wndTranslateZEdit);
    DDX_Control(pDX, IDC_TRANSFORMS_TRANSLATE_Y_EDIT, m_wndTranslateYEdit);
    DDX_Control(pDX, IDC_TRANSFORMS_TRANSLATE_X_EDIT, m_wndTranslateXEdit);
    DDX_Control(pDX, IDC_TRANSFORMS_SCALE_LOCAL_EDIT, m_wndScaleEdit);
    DDX_Control(pDX, IDC_TRANSFORMS_3x3_EDIT, m_wndRotation3x3Edit);
    DDX_Control(pDX, IDC_TRANSFORMS_3x2_EDIT, m_wndRotation3x2Edit);
    DDX_Control(pDX, IDC_TRANSFORMS_3x1_EDIT, m_wndRotation3x1Edit);
    DDX_Control(pDX, IDC_TRANSFORMS_2x3_EDIT, m_wndRotation2x3Edit);
    DDX_Control(pDX, IDC_TRANSFORMS_2x2_EDIT, m_wndRotation2x2Edit);
    DDX_Control(pDX, IDC_TRANSFORMS_2x1_EDIT, m_wndRotation2x1Edit);
    DDX_Control(pDX, IDC_TRANSFORMS_1x3_EDIT, m_wndRotation1x3Edit);
    DDX_Control(pDX, IDC_TRANSFORMS_1x2_EDIT, m_wndRotation1x2Edit);
    DDX_Control(pDX, IDC_TRANSFORMS_1x1_EDIT, m_wndRotation1x1Edit);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CLocalTransformsDlg, CNiObjectDlg)
    //{{AFX_MSG_MAP(CLocalTransformsDlg)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CLocalTransformsDlg message handlers
//---------------------------------------------------------------------------
bool CLocalTransformsDlg::DoUpdate()
{
    if(m_pkObj == NULL)
        return false;

    ASSERT(NiIsKindOf(NiAVObject, m_pkObj));

    NiAVObject* pkObj = (NiAVObject*) m_pkObj;
    char acString[256];

    NiPoint3 kPoint = pkObj->GetTranslate();
    NiSprintf(acString, 256, "%.4f", kPoint.z);
    m_wndTranslateZEdit.SetWindowText(acString);

    NiSprintf(acString, 256, "%.4f", kPoint.y);
    m_wndTranslateYEdit.SetWindowText(acString);
    
    NiSprintf(acString, 256, "%.4f", kPoint.x);
    m_wndTranslateXEdit.SetWindowText(acString);

    NiSprintf(acString, 256, "%.4f", pkObj->GetScale());
    m_wndScaleEdit.SetWindowText(acString);

    NiMatrix3 kMatrix = pkObj->GetRotate();
    kMatrix.GetRow(2, kPoint);

    NiSprintf(acString, 256, "%.4f", kPoint.z);
    m_wndRotation3x3Edit.SetWindowText(acString);

    NiSprintf(acString, 256, "%.4f", kPoint.y);
    m_wndRotation3x2Edit.SetWindowText(acString);
    
    NiSprintf(acString, 256, "%.4f", kPoint.x);
    m_wndRotation3x1Edit.SetWindowText(acString);

    kMatrix.GetRow(1, kPoint);
    NiSprintf(acString, 256, "%.4f", kPoint.z);
    m_wndRotation2x3Edit.SetWindowText(acString);

    NiSprintf(acString, 256, "%.4f", kPoint.y);
    m_wndRotation2x2Edit.SetWindowText(acString);

    NiSprintf(acString, 256, "%.4f", kPoint.x);
    m_wndRotation2x1Edit.SetWindowText(acString);

    kMatrix.GetRow(0, kPoint);
    NiSprintf(acString, 256, "%.4f", kPoint.z);
    m_wndRotation1x3Edit.SetWindowText(acString);
    
    NiSprintf(acString, 256, "%.4f", kPoint.y);
    m_wndRotation1x2Edit.SetWindowText(acString);

    NiSprintf(acString, 256, "%.4f", kPoint.x);
    m_wndRotation1x1Edit.SetWindowText(acString);

    return true;
}
//---------------------------------------------------------------------------
