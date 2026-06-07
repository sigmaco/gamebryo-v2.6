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

// WorldTransformsDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "WorldTransformsDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CWorldTransformsDlg dialog
//---------------------------------------------------------------------------
CWorldTransformsDlg::CWorldTransformsDlg(CWnd* pParent /*=NULL*/)
    : CNiObjectDlg(CWorldTransformsDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CWorldTransformsDlg)
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
void CWorldTransformsDlg::DoDataExchange(CDataExchange* pDX)
{
    CNiObjectDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CWorldTransformsDlg)
    DDX_Control(pDX, IDC_TRANSFORMS_SCALE_WORLD_EDIT, m_wndScaleEdit);
    DDX_Control(pDX, IDC_TRANSFORMS_TRANSLATE_Z_EDIT_WORLD,
        m_wndTranslateZEdit);
    DDX_Control(pDX, IDC_TRANSFORMS_TRANSLATE_Y_EDIT_WORLD, 
        m_wndTranslateYEdit);
    DDX_Control(pDX, IDC_TRANSFORMS_TRANSLATE_X_EDIT_WORLD, 
        m_wndTranslateXEdit);
    DDX_Control(pDX, IDC_TRANSFORMS_3x3_EDIT_WORLD, m_wndRotate3x3Edit);
    DDX_Control(pDX, IDC_TRANSFORMS_3x2_EDIT_WORLD, m_wndRotate3x2Edit);
    DDX_Control(pDX, IDC_TRANSFORMS_3x1_EDIT_WORLD, m_wndRotate3x1Edit);
    DDX_Control(pDX, IDC_TRANSFORMS_2x3_EDIT_WORLD, m_wndRotate2x3Edit);
    DDX_Control(pDX, IDC_TRANSFORMS_2x2_EDIT_WORLD, m_wndRotate2x2Edit);
    DDX_Control(pDX, IDC_TRANSFORMS_2x1_EDIT_WORLD, m_wndRotate2x1Edit);
    DDX_Control(pDX, IDC_TRANSFORMS_1x3_EDIT_WORLD, m_wndRotate1x3Edit);
    DDX_Control(pDX, IDC_TRANSFORMS_1x2_EDIT_WORLD, m_wndRotate1x2Edit);
    DDX_Control(pDX, IDC_TRANSFORMS_1x1_EDIT_WORLD, m_wndRotate1x1Edit);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CWorldTransformsDlg, CNiObjectDlg)
    //{{AFX_MSG_MAP(CWorldTransformsDlg)
        // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CWorldTransformsDlg message handlers
//---------------------------------------------------------------------------
bool CWorldTransformsDlg::DoUpdate()
{
    if(m_pkObj == NULL)
        return false;

    ASSERT(NiIsKindOf(NiAVObject, m_pkObj));

    NiAVObject* pkObj = (NiAVObject*) m_pkObj;
    char acString[256];

    NiPoint3 kPoint = pkObj->GetWorldTranslate();
    NiSprintf(acString, 256, "%.4f", kPoint.z);
    m_wndTranslateZEdit.SetWindowText(acString);

    NiSprintf(acString, 256, "%.4f", kPoint.y);
    m_wndTranslateYEdit.SetWindowText(acString);
    
    NiSprintf(acString, 256, "%.4f", kPoint.x);
    m_wndTranslateXEdit.SetWindowText(acString);

    NiSprintf(acString, 256, "%.4f", pkObj->GetWorldScale());
    m_wndScaleEdit.SetWindowText(acString);

    NiMatrix3 kMatrix = pkObj->GetWorldRotate();
    kMatrix.GetRow(2, kPoint);

    NiSprintf(acString, 256, "%.4f", kPoint.z);
    m_wndRotate3x3Edit.SetWindowText(acString);

    NiSprintf(acString, 256, "%.4f", kPoint.y);
    m_wndRotate3x2Edit.SetWindowText(acString);
    
    NiSprintf(acString, 256, "%.4f", kPoint.x);
    m_wndRotate3x1Edit.SetWindowText(acString);

    kMatrix.GetRow(1, kPoint);
    NiSprintf(acString, 256, "%.4f", kPoint.z);
    m_wndRotate2x3Edit.SetWindowText(acString);

    NiSprintf(acString, 256, "%.4f", kPoint.y);
    m_wndRotate2x2Edit.SetWindowText(acString);

    NiSprintf(acString, 256, "%.4f", kPoint.x);
    m_wndRotate2x1Edit.SetWindowText(acString);

    kMatrix.GetRow(0, kPoint);
    NiSprintf(acString, 256, "%.4f", kPoint.z);
    m_wndRotate1x3Edit.SetWindowText(acString);
    
    NiSprintf(acString, 256, "%.4f", kPoint.y);
    m_wndRotate1x2Edit.SetWindowText(acString);

    NiSprintf(acString, 256, "%.4f", kPoint.x);
    m_wndRotate1x1Edit.SetWindowText(acString);

    return true;
}
//---------------------------------------------------------------------------
