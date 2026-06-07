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

// NiObjectNETPropertyWindowCreator.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiObjectNETPropertyWindowCreator.h"
#include "NifPropertyWindow.h"
#include "ExtraDataDlg.h"
#include "NiObjectNETTimeControllersDlg.h"

//---------------------------------------------------------------------------
void CNiObjectNETPropertyWindowCreator::AddPages(NiObject* pkObj,
    CNifPropertyWindow* pkWindow)
{
    ASSERT(pkObj != NULL && pkWindow != NULL);
    AddBasicInfoDialog(pkObj, pkWindow, true);
    AddExtraDataDialog(pkObj, pkWindow,true);
    AddTimeControllerDialog(pkObj, pkWindow,true);
    AddViewerStringsDialog(pkObj, pkWindow,true);
    pkWindow->ScrollToPage(0);
}
//---------------------------------------------------------------------------
int CNiObjectNETPropertyWindowCreator::AddExtraDataDialog(NiObject* pkObj,
    CNifPropertyWindow* pkWindow, bool bExpanded, int iIndex)
{
    CExtraDataDlg* pwndDlg = new CExtraDataDlg;
    pwndDlg->SetNiObject(pkObj);
    
    pwndDlg->Create(MAKEINTRESOURCE(pwndDlg->IDD), 
        pkWindow->GetParentForPages());

    return pkWindow->InsertPage("Extra Data", pwndDlg, bExpanded, iIndex);
}
//---------------------------------------------------------------------------
NiRTTI* CNiObjectNETPropertyWindowCreator::GetTargetRTTI()
{
    return (NiRTTI*) (&NiObjectNET::ms_RTTI);
}
//---------------------------------------------------------------------------
int CNiObjectNETPropertyWindowCreator::AddTimeControllerDialog(
    NiObject* pkObj, CNifPropertyWindow* pkWindow, bool bExpanded, 
    int iIndex)
{
    CNiObjectNETTimeControllersDlg* pwndDlg = 
        new CNiObjectNETTimeControllersDlg;
    pwndDlg->SetNiObject(pkObj);
    
    pwndDlg->Create(MAKEINTRESOURCE(pwndDlg->IDD), 
        pkWindow->GetParentForPages());

    return pkWindow->InsertPage("Time Controller", pwndDlg, bExpanded,
        iIndex);
}
//---------------------------------------------------------------------------
