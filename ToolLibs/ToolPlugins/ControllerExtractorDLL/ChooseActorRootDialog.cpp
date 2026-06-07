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

#include "stdafx.h"
#include "ChooseActorRootDialog.h"
#include "resource.h"

//---------------------------------------------------------------------------
ChooseActorRootDialog::ChooseActorRootDialog(NiModuleRef hInstance,
    NiWindowRef hParent) : NiDialog(IDD_CHOOSEACTOR, hInstance, hParent),
    m_pkActorRoots(NULL), m_pkSelectedActorRoot(NULL)
{
}
//---------------------------------------------------------------------------
void ChooseActorRootDialog::InitDialog()
{
    NIASSERT(m_pkActorRoots && m_pkActorRoots->GetSize() >= 2);
    m_pkSelectedActorRoot = NULL;

    // Populate list box.
    for (unsigned int ui = 0; ui < m_pkActorRoots->GetSize(); ui++)
    {
        NiAVObject* pkActorRoot = m_pkActorRoots->GetAt(ui);
        int iIndex = (int)SendMessage(GetDlgItem(m_hWnd, IDC_LIST_ACTOR_ROOTS),
            LB_ADDSTRING, 0, (LPARAM) (LPCTSTR) pkActorRoot->GetName());
        NIASSERT(iIndex != LB_ERR && iIndex != LB_ERRSPACE);

#ifdef NIDEBUG
        int iRC = 
#endif
            (int)SendMessage(GetDlgItem(m_hWnd, IDC_LIST_ACTOR_ROOTS),
            LB_SETITEMDATA, (WPARAM) iIndex, (LPARAM) pkActorRoot);
        NIASSERT(iRC != LB_ERR);
    }
}
//---------------------------------------------------------------------------
BOOL ChooseActorRootDialog::OnCommand(int iWParamLow, int iWParamHigh,
    long)
{
    switch (iWParamLow)
    {
        case IDC_LIST_ACTOR_ROOTS:
            if (iWParamHigh == LBN_DBLCLK)
            {
                HandleOK();
                EndDialog(m_hWnd, IDOK);
                return TRUE;
            }
            break;
        case IDOK:
            HandleOK();
            EndDialog(m_hWnd, IDOK);
            return TRUE;
    }

    return FALSE;
}
//---------------------------------------------------------------------------
void ChooseActorRootDialog::OnClose()
{
    NIASSERT(false);
}
//---------------------------------------------------------------------------
void ChooseActorRootDialog::HandleOK()
{
    int iIndex = (int)SendMessage(GetDlgItem(m_hWnd, IDC_LIST_ACTOR_ROOTS),
        LB_GETCURSEL, 0, 0);
    NIASSERT(iIndex != LB_ERR);

    m_pkSelectedActorRoot = (NiAVObject*) SendMessage(GetDlgItem(m_hWnd,
        IDC_LIST_ACTOR_ROOTS), LB_GETITEMDATA, (WPARAM) iIndex, 0);
    NIASSERT(PtrToInt(m_pkSelectedActorRoot) != LB_ERR);
}
//---------------------------------------------------------------------------
