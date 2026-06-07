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

// NiTimeControllerPropertyWindowCreator.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiTimeControllerPropertyWindowCreator.h"
#include "NifPropertyWindow.h"
#include "NiTimeControllerDlg.h"
 
//---------------------------------------------------------------------------
void CNiTimeControllerPropertyWindowCreator::AddPages(NiObject* pkObj,
    CNifPropertyWindow* pkWindow)
{
    ASSERT(pkObj != NULL && pkWindow != NULL);
    AddBasicInfoDialog(pkObj, pkWindow, true);
    AddTimeControllerDialog(pkObj, pkWindow, true);
    AddViewerStringsDialog(pkObj, pkWindow, true);
    pkWindow->ScrollToPage(0);
}
//---------------------------------------------------------------------------
NiRTTI* CNiTimeControllerPropertyWindowCreator::GetTargetRTTI()
{
    return (NiRTTI*) (&NiTimeController::ms_RTTI);
}
//---------------------------------------------------------------------------
int CNiTimeControllerPropertyWindowCreator::AddTimeControllerDialog(
    NiObject* pkObj, CNifPropertyWindow* pkWindow, bool bExpanded, int iIndex)
{
    CNiTimeControllerDlg* pwndDlg = new CNiTimeControllerDlg;
    pwndDlg->SetNiObject(pkObj);
    
    pwndDlg->Create(MAKEINTRESOURCE(pwndDlg->IDD), 
        pkWindow->GetParentForPages());

    return pkWindow->InsertPage("Basic Info", pwndDlg, bExpanded, iIndex);
}
//---------------------------------------------------------------------------
