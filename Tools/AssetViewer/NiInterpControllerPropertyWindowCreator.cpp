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

// NiInterpConrollerPropertyWindowCreator.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiInterpControllerPropertyWindowCreator.h"
#include "NifPropertyWindow.h"
#include "NiTimeControllerInterpolatorDlg.h"
 
//---------------------------------------------------------------------------
void CNiInterpControllerPropertyWindowCreator::AddPages(NiObject* pkObj,
    CNifPropertyWindow* pkWindow)
{
    ASSERT(pkObj != NULL && pkWindow != NULL);
    AddBasicInfoDialog(pkObj, pkWindow, true);
    AddTimeControllerDialog(pkObj, pkWindow, true);
    AddInterpControllerDialog(pkObj, pkWindow, true);
    AddViewerStringsDialog(pkObj, pkWindow, true);
    pkWindow->ScrollToPage(0);
}
//---------------------------------------------------------------------------
NiRTTI* CNiInterpControllerPropertyWindowCreator::GetTargetRTTI()
{
    return (NiRTTI*) (&NiInterpController::ms_RTTI);
}
//---------------------------------------------------------------------------
int CNiInterpControllerPropertyWindowCreator::AddInterpControllerDialog(
    NiObject* pkObj, CNifPropertyWindow* pkWindow, bool bExpanded, int iIndex)
{
    CNiTimeControllerInterpolatorsDlg* pwndDlg = new 
        CNiTimeControllerInterpolatorsDlg;
    pwndDlg->SetNiObject(pkObj);
    
    pwndDlg->Create(MAKEINTRESOURCE(pwndDlg->IDD), 
        pkWindow->GetParentForPages());

    return pkWindow->InsertPage("Interpolator Info", pwndDlg, 
        bExpanded, iIndex);
}
//---------------------------------------------------------------------------
