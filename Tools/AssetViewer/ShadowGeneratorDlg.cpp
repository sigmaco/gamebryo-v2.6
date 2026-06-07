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

// ShadowGeneratorDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "ShadowGeneratorDlg.h"
#include "NifPropertyWindowManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CShadowGeneratorDlg dialog
//---------------------------------------------------------------------------
CShadowGeneratorDlg::CShadowGeneratorDlg(CWnd* pParent /*=NULL*/)
    : CNiObjectDlg(CShadowGeneratorDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CShadowGeneratorDlg)
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
void CShadowGeneratorDlg::DoDataExchange(CDataExchange* pDX)
{
    CNiObjectDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CShadowGeneratorDlg)
    DDX_Control(pDX, IDC_CHECK_ACTIVE, m_wndActiveCheckBox);
    DDX_Control(pDX, IDC_CHECK_STATIC, m_wndStaticCheckBox);
    DDX_Control(pDX, IDC_CHECK_RENDERBACKFACES, m_wndRenderBackfacesCheckBox);
    DDX_Control(pDX, IDC_CHECK_STRICTLYOBSERVESIZEHINT,
        m_wndStrictlyObserveSizeHintCheckBox);
    DDX_Control(pDX, IDC_EDIT_SHADOWTECHNIQUE, m_wndShadowTechniqueEditBox);
    DDX_Control(pDX, IDC_EDIT_DEPTHBIAS, m_wndDepthBiasEditBox);
    DDX_Control(pDX, IDC_EDIT_SIZEHINT, m_wndSizeHintEditBox);
    DDX_Control(pDX, IDC_LIST_UNAFFECTED_CASTERS, m_wndUnaffectedCastersList);
    DDX_Control(pDX, IDC_LIST_UNAFFECTED_RECEIVERS,
        m_wndUnaffectedReceiversList);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CShadowGeneratorDlg, CNiObjectDlg)
    //{{AFX_MSG_MAP(CShadowGeneratorDlg)
    ON_LBN_DBLCLK(IDC_LIST_UNAFFECTED_CASTERS,
        OnLbnDblclkListUnaffectedCasters)
    ON_LBN_DBLCLK(IDC_LIST_UNAFFECTED_RECEIVERS,
        OnLbnDblclkListUnaffectedReceivers)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CShadowGeneratorDlg message handlers
//---------------------------------------------------------------------------
bool CShadowGeneratorDlg::DoUpdate()
{
    NIASSERT(NiIsKindOf(NiShadowGenerator, m_pkObj));
    NiShadowGenerator* pkGenerator = (NiShadowGenerator*) m_pkObj;

    m_wndActiveCheckBox.SetCheck(pkGenerator->GetActive() ? BST_CHECKED :
        BST_UNCHECKED);
    m_wndStaticCheckBox.SetCheck(pkGenerator->GetStatic() ? BST_CHECKED :
        BST_UNCHECKED);
    m_wndRenderBackfacesCheckBox.SetCheck(pkGenerator->GetRenderBackfaces() ?
        BST_CHECKED : BST_UNCHECKED);

    NiShadowTechnique* pkShadowTechnique = pkGenerator->GetShadowTechnique();
    const char* pcShadowTechniqueName = pkShadowTechnique ?
        pkShadowTechnique->GetName() : "N/A";
    m_wndShadowTechniqueEditBox.SetWindowText(pcShadowTechniqueName);

    char acDepthBias[128];
    NiSprintf(acDepthBias, 128, "%f", pkGenerator->GetDepthBias());
    m_wndDepthBiasEditBox.SetWindowText(acDepthBias);

    char acSizeHint[16];
    NiSprintf(acSizeHint, 16, "%d", pkGenerator->GetSizeHint());
    m_wndSizeHintEditBox.SetWindowText(acSizeHint);

    m_wndStrictlyObserveSizeHintCheckBox.SetCheck(
        pkGenerator->GetStrictlyObserveSizeHint() ? BST_CHECKED :
        BST_UNCHECKED);

    m_wndUnaffectedCastersList.ResetContent();
    const NiNodeList& kUnaffectedCasters =
        pkGenerator->GetUnaffectedCasterNodeList();
    NiTListIterator kIter = kUnaffectedCasters.GetHeadPos();
    while (kIter)
    {
        NiNode* pkNode = kUnaffectedCasters.GetNext(kIter);
        NIASSERT(pkNode);
        int iIndex = m_wndUnaffectedCastersList.AddString(pkNode->GetName() ?
            pkNode->GetName() : "[Unnamed Node]");
        m_wndUnaffectedCastersList.SetItemData(iIndex, (DWORD_PTR) pkNode);
    }

    m_wndUnaffectedReceiversList.ResetContent();
    const NiNodeList& kUnaffectedReceivers =
        pkGenerator->GetUnaffectedReceiverNodeList();
    kIter = kUnaffectedReceivers.GetHeadPos();
    while (kIter)
    {
        NiNode* pkNode = kUnaffectedReceivers.GetNext(kIter);
        NIASSERT(pkNode);
        int iIndex = m_wndUnaffectedReceiversList.AddString(pkNode->GetName() ?
            pkNode->GetName() : "[Unnamed Node]");
        m_wndUnaffectedReceiversList.SetItemData(iIndex, (DWORD_PTR) pkNode);
    }

    return true;
}
//---------------------------------------------------------------------------
BOOL CShadowGeneratorDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();

    ASSERT(NiIsKindOf(NiShadowGenerator, m_pkObj));

    DoUpdate();
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}
//---------------------------------------------------------------------------
void CShadowGeneratorDlg::OnLbnDblclkListUnaffectedCasters()
{
    int iIndex = m_wndUnaffectedCastersList.GetCurSel();
    if (iIndex != LB_ERR)
    {
        NiNode* pkNode = (NiNode*) m_wndUnaffectedCastersList.GetItemData(
            iIndex);
        CNifPropertyWindowManager::GetPropertyWindowManager()
            ->CreatePropertyWindow(pkNode);
    }
}
//---------------------------------------------------------------------------
void CShadowGeneratorDlg::OnLbnDblclkListUnaffectedReceivers()
{
    int iIndex = m_wndUnaffectedReceiversList.GetCurSel();
    if (iIndex != LB_ERR)
    {
        NiNode* pkNode = (NiNode*) m_wndUnaffectedReceiversList.GetItemData(
            iIndex);
        CNifPropertyWindowManager::GetPropertyWindowManager()
            ->CreatePropertyWindow(pkNode);
    }
}
//---------------------------------------------------------------------------
