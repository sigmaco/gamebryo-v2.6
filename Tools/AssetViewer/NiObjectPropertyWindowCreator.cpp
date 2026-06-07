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

// NiObjectPropertyWindowCreator.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiObjectPropertyWindowCreator.h"
#include "NifPropertyWindow.h"
#include "ViewerStringsDlg.h"
#include "NiObjectBasicInfoDlg.h"

int CNiObjectPropertyWindowCreator::ms_iWindowID =
    AFX_IDW_CONTROLBAR_FIRST + 100;

//---------------------------------------------------------------------------
CNiObjectPropertyWindowCreator::~CNiObjectPropertyWindowCreator()
{

}
//---------------------------------------------------------------------------
CNifPropertyWindow* CNiObjectPropertyWindowCreator::CreatePropertyWindow(
    NiObject* pkObj, int iStartX, int iStartY, int iWidth, int iHeight)
{
    CWinApp* pkApp = AfxGetApp();
    if(!pkApp)
        return NULL;

    CWnd* pkParentWnd = pkApp->GetMainWnd();
    if (!pkParentWnd)
        return NULL;

    CNifPropertyWindow* pkWindow = new CNifPropertyWindow();
    char acName[256];
    acName[0] = '\0';
    if(NiIsKindOf(NiObjectNET, pkObj))
    {
        NiObjectNET* pkObjNET = (NiObjectNET*) pkObj;
        NiSprintf(acName,  256, "%s", pkObjNET->GetName());
    }
    NiSprintf(acName, 256, "%s (%s)", acName, pkObj->GetRTTI()->GetName());
    
    LPCTSTR lpszClassName = (LPCTSTR) pkWindow->GetRegisteredClassName();
    LPCTSTR lpszWindowName = acName; 

    BOOL bSucceeded =  
        pkWindow->CreateEx(0, lpszClassName, lpszWindowName, 
            WS_OVERLAPPEDWINDOW, iStartX, iStartY,  iWidth, iHeight, 
            pkParentWnd->m_hWnd, NULL);

    if(bSucceeded)
    {
        AddPages(pkObj, pkWindow);
        if(pkWindow)
        {
            // Make sure that this window is always on top of the main frame
            pkWindow->SetWindowPos(&pkWindow->wndTop, 0, 0, 0, 0, 
                SWP_NOSIZE | SWP_NOMOVE);
            pkWindow->SetCreator(this);
            pkWindow->ShowWindow(SW_SHOW);
            pkWindow->UpdateWindow();
        }
        return pkWindow;
    }
    else
    {
        delete pkWindow;
        return NULL;
    }
}
//---------------------------------------------------------------------------
void CNiObjectPropertyWindowCreator::AddPages(NiObject* pkObj,
    CNifPropertyWindow* pkWindow)
{
    ASSERT(pkObj != NULL && pkWindow != NULL);
    AddBasicInfoDialog(pkObj, pkWindow, true);
    AddViewerStringsDialog(pkObj, pkWindow, true);
    pkWindow->ScrollToPage(0);
}
//---------------------------------------------------------------------------
int CNiObjectPropertyWindowCreator::AddViewerStringsDialog(NiObject* pkObj,
    CNifPropertyWindow* pkWindow, bool bExpanded, int iIndex,
    char* pcName, bool bLimitedText)
{
    CViewerStringsDlg* pwndDlg = new CViewerStringsDlg;
    pwndDlg->SetNiObject(pkObj);
    
    if (bLimitedText)
        pwndDlg->SetShowType(CViewerStringsDlg::LIMITED);

    pwndDlg->Create(MAKEINTRESOURCE(pwndDlg->IDD), 
        pkWindow->GetParentForPages());

    return pkWindow->InsertPage(pcName, pwndDlg, bExpanded,
        iIndex);
}
//---------------------------------------------------------------------------
NiRTTI* CNiObjectPropertyWindowCreator::GetTargetRTTI()
{
    return (NiRTTI*) (&NiObject::ms_RTTI);
}
//---------------------------------------------------------------------------
int CNiObjectPropertyWindowCreator::AddBasicInfoDialog(NiObject* pkObj,
    CNifPropertyWindow* pkWindow, bool bExpanded, int iIndex)
{
    CNiObjectBasicInfoDlg* pwndDlg = new CNiObjectBasicInfoDlg;
    pwndDlg->SetNiObject(pkObj);
    
    pwndDlg->Create(MAKEINTRESOURCE(pwndDlg->IDD), 
        pkWindow->GetParentForPages());

    return pkWindow->InsertPage("Basic Info", pwndDlg, bExpanded, iIndex);
}
//---------------------------------------------------------------------------
