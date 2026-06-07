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

// NiTimeControllerDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiTimeControllerDlg.h"
#include "NifPropertyWindowManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CNiTimeControllerDlg dialog
//---------------------------------------------------------------------------
CNiTimeControllerDlg::CNiTimeControllerDlg(CWnd* pParent /*=NULL*/)
    : CNiObjectDlg(CNiTimeControllerDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CNiTimeControllerDlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
void CNiTimeControllerDlg::DoDataExchange(CDataExchange* pDX)
{
    CNiObjectDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNiTimeControllerDlg)
    DDX_Control(pDX, IDC_NITIMECONTROLLER_TOTAL_KEY_COUNT_EDIT,
        m_wndTotalKeyCountEdit);
    DDX_Control(pDX, IDC_NITIMECONTROLLER_NEXT_BUTTON, m_wndNextButton);
    DDX_Control(pDX, IDC_NITIMECONTROLLER_TARGET_BUTTON, m_wndTargetButton);
    DDX_Control(pDX, IDC_NITIMECONTROLLER_PHASE_EDIT, m_wndPhaseEdit);
    DDX_Control(pDX, IDC_NITIMECONTROLLER_IS_TRANSFORM_CONTRLLER_CHECK,
        m_wndIsTransformControllerRadio);
    DDX_Control(pDX, IDC_NITIMECONTROLLER_FREQUENCY_EDIT, m_wndFrequencyEdit);
    DDX_Control(pDX, IDC_NITIMECONTROLLER_END_KEY_TIME_EDIT, 
        m_wndEndKeyTimeEdit);
    DDX_Control(pDX, IDC_NITIMECONTROLLER_CYCLE_TYPE_COMBO, 
        m_wndCycleTypeCombo);
    DDX_Control(pDX, IDC_NITIMECONTROLLER_BEGIN_KEY_TIME_EDIT,
        m_wndBeginKeyTimeEdit);
    DDX_Control(pDX, IDC_NITIMECONTROLLER_ANIM_TYPE_COMBO,
        m_wndAnimTypeCombo);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNiTimeControllerDlg, CNiObjectDlg)
    //{{AFX_MSG_MAP(CNiTimeControllerDlg)
    ON_BN_CLICKED(IDC_NITIMECONTROLLER_TARGET_BUTTON, 
        OnNitimecontrollerTargetButton)
    ON_BN_CLICKED(IDC_NITIMECONTROLLER_NEXT_BUTTON, 
        OnNitimecontrollerNextButton)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNiTimeControllerDlg message handlers
//---------------------------------------------------------------------------
bool CNiTimeControllerDlg::DoUpdate()
{
    if(m_pkObj == NULL || !NiIsKindOf(NiTimeController, m_pkObj))
        return false;

    NiTimeController* pkController = (NiTimeController*) m_pkObj;
    int iKeyframeCount = GetInfo(pkController);

    char acString[256];

    if(iKeyframeCount == 0)
        NiSprintf(acString, 256, "N/A");
    else
        NiSprintf(acString, 256, "%d", iKeyframeCount);

    m_wndTotalKeyCountEdit.SetWindowText(acString);
    
    if(pkController->GetNext())
    {
        m_wndNextButton.EnableWindow(TRUE);
        NiSprintf(acString, 256, "0x%.8x", (long)PtrToLong(pkController->GetNext()));
    }
    else
    {
        m_wndNextButton.EnableWindow(FALSE);
        NiSprintf(acString, 256, "None");
    }
    m_wndNextButton.SetWindowText(acString);

    if(pkController->GetTarget())
    {
        m_wndTargetButton.EnableWindow(TRUE);
        NiSprintf(acString, 256, "%s", pkController->GetTarget()->GetName());
    }
    else
    {
        m_wndTargetButton.EnableWindow(FALSE);
        NiSprintf(acString, 256, "None");
    }
    m_wndTargetButton.SetWindowText(acString);

    NiSprintf(acString, 256, "%.4f", pkController->GetPhase());
    m_wndPhaseEdit.SetWindowText(acString);

    m_wndIsTransformControllerRadio.SetCheck(
        pkController->IsTransformController());

    NiSprintf(acString, 256, "%.4f", pkController->GetFrequency());
    m_wndFrequencyEdit.SetWindowText(acString);

    float fTime = pkController->GetBeginKeyTime();
    
    if(fTime == -NI_INFINITY || fTime == NI_INFINITY)
        NiSprintf(acString, 256, "N/A");
    else
        NiSprintf(acString, 256, "%.4f", fTime);

    m_wndBeginKeyTimeEdit.SetWindowText(acString);

    fTime = pkController->GetEndKeyTime();
    
    if(fTime == -NI_INFINITY || fTime == NI_INFINITY)
        NiSprintf(acString, 256, "N/A");
    else
        NiSprintf(acString, 256, "%.4f", fTime);

    m_wndEndKeyTimeEdit.SetWindowText(acString);

    int iCycleType = (int) pkController->GetCycleType();
    m_wndCycleTypeCombo.SetCurSel(iCycleType);

    int iAnimType = (int) pkController->GetAnimType();
    m_wndAnimTypeCombo.SetCurSel(iAnimType);

    return true;
}
//---------------------------------------------------------------------------
BOOL CNiTimeControllerDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    
    ASSERT(m_pkObj != NULL && NiIsKindOf(NiTimeController, m_pkObj));
    if(m_pkObj == NULL || !NiIsKindOf(NiTimeController, m_pkObj))
        return FALSE;

    m_wndAnimTypeCombo.AddString("APP_TIME");
    m_wndAnimTypeCombo.AddString("APP_INIT");

    m_wndCycleTypeCombo.AddString("LOOP");
    m_wndCycleTypeCombo.AddString("REVERSE");
    m_wndCycleTypeCombo.AddString("CLAMP");

    DoUpdate();
    
    return TRUE;
}
//---------------------------------------------------------------------------
void CNiTimeControllerDlg::OnNitimecontrollerTargetButton() 
{
    if(m_pkObj == NULL || !NiIsKindOf(NiTimeController, m_pkObj))
        return;

    NiTimeController* pkController = (NiTimeController*) m_pkObj;

    if(pkController->GetTarget())
    {
        CNifPropertyWindowManager* pkManager = 
                CNifPropertyWindowManager::GetPropertyWindowManager();

        pkManager->CreatePropertyWindow(pkController->GetTarget());
    }
    
}
//---------------------------------------------------------------------------
void CNiTimeControllerDlg::OnNitimecontrollerNextButton() 
{
    if(m_pkObj == NULL || !NiIsKindOf(NiTimeController, m_pkObj))
        return;

    NiTimeController* pkController = (NiTimeController*) m_pkObj;
    if(pkController->GetNext())
    {
        CNifPropertyWindowManager* pkManager = 
                CNifPropertyWindowManager::GetPropertyWindowManager();

        pkManager->CreatePropertyWindow(pkController->GetNext());
    }
    
}
//---------------------------------------------------------------------------
int CNiTimeControllerDlg::GetInfo(NiTimeController* pkController)
{
    if(pkController == NULL)
        return 0;

    if(NiIsKindOf(NiInterpController, pkController))
        return GetInterpInfo((NiInterpController*) pkController);
    
    return 0;
}
//---------------------------------------------------------------------------
int CNiTimeControllerDlg::GetInterpInfo(NiInterpController* pkController)
{
    unsigned int uiCount = 0;
    for (unsigned int us = 0; us <  pkController->
        GetInterpolatorCount(); us++)
    {
        NiInterpolator* pkInterp = pkController->GetInterpolator(us);
        if (NiIsKindOf(NiKeyBasedInterpolator, pkInterp))
        {
            NiKeyBasedInterpolator* pkKeyInterp =
                (NiKeyBasedInterpolator*) pkInterp;
            for (unsigned short usc = 0; usc < pkKeyInterp->
                GetKeyChannelCount(); usc++)
            {
                uiCount += pkKeyInterp->GetKeyCount(usc);
            }
        }
    }
    return uiCount;    
}
//---------------------------------------------------------------------------
