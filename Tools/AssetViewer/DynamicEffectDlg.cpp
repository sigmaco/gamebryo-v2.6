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

// DynamicEffectDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "DynamicEffectDlg.h"
#include "NifPropertyWindowManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CDynamicEffectDlg dialog
//---------------------------------------------------------------------------
CDynamicEffectDlg::CDynamicEffectDlg(CWnd* pParent /*=NULL*/)
    : CNiObjectDlg(CDynamicEffectDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CDynamicEffectDlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
void CDynamicEffectDlg::DoDataExchange(CDataExchange* pDX)
{
    CNiObjectDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDynamicEffectDlg)
    DDX_Control(pDX, IDC_NIDYNAMICEFFECT_EFFECT_TYPE_COMBO,
        m_wndEffectTypeCombo);
    DDX_Control(pDX, IDC_NIDYNAMICEFFECT_AFFECTEDNODE_LIST, 
        m_wndAffectedNodeList);
    DDX_Control(pDX, IDC_DYNAMIC_EFFECT_SWITCH_CHECK, m_wndSwitchCheck);
    DDX_Control(pDX, IDC_BUTTON_SHADOWGENERATOR, m_wndShadowGeneratorButton);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CDynamicEffectDlg, CNiObjectDlg)
    //{{AFX_MSG_MAP(CDynamicEffectDlg)
    ON_NOTIFY(NM_DBLCLK, IDC_NIDYNAMICEFFECT_AFFECTEDNODE_LIST,
        OnDblclkNidynamiceffectAffectednodeList)
    ON_BN_CLICKED(IDC_BUTTON_SHADOWGENERATOR, OnBnClickedButtonShadowgenerator)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CDynamicEffectDlg message handlers
//---------------------------------------------------------------------------
bool CDynamicEffectDlg::DoUpdate()
{
    if(!NiIsKindOf(NiDynamicEffect, m_pkObj))
        return false;

    NiDynamicEffect* pkEffect = (NiDynamicEffect*) m_pkObj;

    const NiNodeList* pkList = &pkEffect->GetAffectedNodeList();

    char acString[256];
    int nItem = 0;
    m_wndAffectedNodeList.DeleteAllItems();
    NiTListIterator kIter = pkList->GetHeadPos();
    while(kIter)
    {       
        NiNode* pkNode = pkList->GetNext(kIter);
        if(pkNode)
        {
            NiSprintf(acString, 256, "%s", pkNode->GetName());
            m_wndAffectedNodeList.InsertItem(nItem++, acString);
        }
    }

    m_wndEffectTypeCombo.SetCurSel((int)pkEffect->GetEffectType());
    m_wndSwitchCheck.SetCheck(pkEffect->GetSwitch());

    NiShadowGenerator* pkGenerator = pkEffect->GetShadowGenerator();
    if (pkGenerator)
    {
        NiSprintf(acString, 256, "%#x", (size_t) pkGenerator);
        m_wndShadowGeneratorButton.SetWindowText(acString);
    }

    return true;
}
//---------------------------------------------------------------------------
BOOL CDynamicEffectDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    ASSERT(NiIsKindOf(NiDynamicEffect, m_pkObj));

    m_wndAffectedNodeList.InsertColumn(0, "Name");
    CRect rect;
    m_wndAffectedNodeList.GetWindowRect(&rect);
    int cx = rect.Width();
    m_wndAffectedNodeList.SetColumnWidth(0, cx-4);
    m_wndEffectTypeCombo.AddString("AMBIENT_LIGHT");
    m_wndEffectTypeCombo.AddString("UNKNOWN");
    m_wndEffectTypeCombo.AddString("POINT_LIGHT");
    m_wndEffectTypeCombo.AddString("DIR_LIGHT");
    m_wndEffectTypeCombo.AddString("SPOT_LIGHT");
    m_wndEffectTypeCombo.AddString("SHADOWPOINT_LIGHT");
    m_wndEffectTypeCombo.AddString("SHADOWDIR_LIGHT");
    m_wndEffectTypeCombo.AddString("SHADOWSPOT_LIGHT");
    m_wndEffectTypeCombo.AddString("TEXTURE_EFFECT");

    NiDynamicEffect* pkDynEffect = (NiDynamicEffect*) m_pkObj;
    if (pkDynEffect->GetShadowGenerator())
    {
        m_wndShadowGeneratorButton.EnableWindow();
    }

    DoUpdate();
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}
//---------------------------------------------------------------------------
void CDynamicEffectDlg::OnDblclkNidynamiceffectAffectednodeList(
    NMHDR* pNMHDR, LRESULT* pResult) 
{
    *pResult = 0;
    
    if(m_wndAffectedNodeList.GetSelectedCount() == 0)
        return;

    int iIndex = m_wndAffectedNodeList.GetNextItem(-1, LVIS_SELECTED);

    if(iIndex == -1)
        return;

    int iMatchIndex = 0;
    NiDynamicEffect* pkEffect = (NiDynamicEffect*) m_pkObj;
    const NiNodeList* pkList = &pkEffect->GetAffectedNodeList();
    NiNode* pkNode = NULL;
    NiTListIterator kIter = pkList->GetHeadPos();
    while(kIter)
    {       
        if(iMatchIndex == iIndex)
        {
            pkNode = pkList->GetNext(kIter);
            if(pkNode)
            {
                CNifPropertyWindowManager* pkManager = 
                CNifPropertyWindowManager::GetPropertyWindowManager();

                pkManager->CreatePropertyWindow(pkNode);
                return;
            }
        }
        iMatchIndex++;
    }
}
//---------------------------------------------------------------------------
void CDynamicEffectDlg::OnBnClickedButtonShadowgenerator()
{
    NiDynamicEffect* pkDynEffect = (NiDynamicEffect*) m_pkObj;
    NiShadowGenerator* pkGenerator = pkDynEffect->GetShadowGenerator();
    NIASSERT(pkGenerator);

    CNifPropertyWindowManager* pkManager = 
        CNifPropertyWindowManager::GetPropertyWindowManager();
    if (pkManager)
    {
        pkManager->CreatePropertyWindow(pkGenerator);
    }
}
//---------------------------------------------------------------------------
