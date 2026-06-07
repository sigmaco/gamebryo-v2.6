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

// NiObjectBasicInfoDlg.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiObjectBasicInfoDlg.h"
#include "NifPropertyWindowManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//---------------------------------------------------------------------------
// CNiObjectBasicInfoDlg dialog
//---------------------------------------------------------------------------
CNiObjectBasicInfoDlg::CNiObjectBasicInfoDlg(CWnd* pParent /*=NULL*/)
    : CNiObjectDlg(CNiObjectBasicInfoDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CNiObjectBasicInfoDlg)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
}
//---------------------------------------------------------------------------
void CNiObjectBasicInfoDlg::DoDataExchange(CDataExchange* pDX)
{
    CNiObjectDlg::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CNiObjectBasicInfoDlg)
    DDX_Control(pDX, IDC_PARENT_BUTTON, m_wndParentButton);
    DDX_Control(pDX, IDC_NIOBJECT_REFCOUNT_EDIT, m_wndRefCountEdit);
    DDX_Control(pDX, IDC_NIOBJECT_POINTER_EDIT, m_wndPointerEdit);
    DDX_Control(pDX, IDC_NIOBJECT_NAME_EDIT, m_wndNameEdit);
    DDX_Control(pDX, IDC_NIOBJECT_CLASS_EDIT, m_wndClassEdit);
    //}}AFX_DATA_MAP
}
//---------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CNiObjectBasicInfoDlg, CNiObjectDlg)
    //{{AFX_MSG_MAP(CNiObjectBasicInfoDlg)
    ON_BN_CLICKED(IDC_PARENT_BUTTON, OnParentButton)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
//---------------------------------------------------------------------------
// CNiObjectBasicInfoDlg message handlers
//---------------------------------------------------------------------------
BOOL CNiObjectBasicInfoDlg::OnInitDialog() 
{
    CNiObjectDlg::OnInitDialog();
    
    if(NiIsKindOf(NiAVObject,m_pkObj) && ((NiAVObject*)m_pkObj)->GetParent())
        m_wndParentButton.EnableWindow();
    DoUpdate();

    return TRUE;
}
//---------------------------------------------------------------------------
bool CNiObjectBasicInfoDlg::DoUpdate()
{
    if(m_pkObj == NULL)
        return false;

    char acString[256];

    NiSprintf(acString, 256, "%d", m_pkObj->GetRefCount());
    m_wndRefCountEdit.SetWindowText(acString);

    NiSprintf(acString, 256, "0x%.8x", (long) PtrToLong(m_pkObj));
    m_wndPointerEdit.SetWindowText(acString);

    if(NiIsKindOf(NiObjectNET, m_pkObj))
        NiSprintf(acString, 256, "%s", ((NiObjectNET*)m_pkObj)->GetName());
    else
        acString[0] = '\0';

    m_wndNameEdit.SetWindowText(acString);

    NiSprintf(acString, 256, "%s", m_pkObj->GetRTTI()->GetName());
    m_wndClassEdit.SetWindowText(acString);

    if(NiIsKindOf(NiAVObject,m_pkObj))
    {
        NiAVObject* pkObj = ((NiAVObject*) m_pkObj)->GetParent();
        
        if(pkObj)
        {
            const char* pcName = pkObj->GetName();
            if(pcName)
                NiSprintf(acString, 256, "%s", pcName);
            else
            {
                NiSprintf(acString, 256, "%s(0x%.8x)",
                    pkObj->GetRTTI()->GetName(),
                    (long) PtrToLong(pkObj->GetParent()));
            }
        }
        else
        {
            NiSprintf(acString, 256, "No Parent");
        }
        

        m_wndParentButton.SetWindowText(acString);
    }

    return true;

}
//---------------------------------------------------------------------------
void CNiObjectBasicInfoDlg::OnParentButton() 
{
    if(NiIsKindOf(NiAVObject,m_pkObj))
    {
        NiAVObject* pkObj = ((NiAVObject*) m_pkObj)->GetParent();
        
        if(pkObj)
        {
            CNifPropertyWindowManager* pkManager = 
                CNifPropertyWindowManager::GetPropertyWindowManager();
            if(pkManager)
                pkManager->CreatePropertyWindow(pkObj);
        }
    }
}
//---------------------------------------------------------------------------
