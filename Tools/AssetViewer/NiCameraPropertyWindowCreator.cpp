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

// NiCameraPropertyWindowCreator.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiCameraPropertyWindowCreator.h"
#include "NifPropertyWindow.h"
#include "CameraInfoDlg.h"

//---------------------------------------------------------------------------
void CNiCameraPropertyWindowCreator::AddPages(NiObject* pkObj, 
    CNifPropertyWindow* pkWindow)
{
    ASSERT(pkObj != NULL && pkWindow != NULL);
    ASSERT(NiIsKindOf(NiAVObject, pkObj));
    AddBasicInfoDialog(pkObj, pkWindow, true);
    AddCameraDialog(pkObj, pkWindow, true);
    AddLocalTransformsDialog(pkObj, pkWindow, true);
    AddWorldTransformsDialog(pkObj, pkWindow);
    AddNiBoundDialog(pkObj, pkWindow);
    AddTimeControllerDialog(pkObj, pkWindow);
    AddPropertiesDialog(pkObj, pkWindow);
    AddSelectiveUpdateDialog(pkObj, pkWindow, true);
    AddCollisionDialog(pkObj, pkWindow);
    AddExtraDataDialog(pkObj, pkWindow);
    AddViewerStringsDialog(pkObj, pkWindow);
    pkWindow->ScrollToPage(0);
}
//---------------------------------------------------------------------------
NiRTTI* CNiCameraPropertyWindowCreator::GetTargetRTTI()
{
    return (NiRTTI*) (&NiCamera::ms_RTTI);
}
//---------------------------------------------------------------------------
int CNiCameraPropertyWindowCreator::AddCameraDialog(NiObject* pkObj,
    CNifPropertyWindow* pkWindow, bool bExpanded, int iIndex)
{
    CCameraInfoDlg* pwndDlg = new CCameraInfoDlg;
    pwndDlg->SetNiObject(pkObj);
    
    pwndDlg->Create(MAKEINTRESOURCE(pwndDlg->IDD), 
        pkWindow->GetParentForPages());

    return pkWindow->InsertPage("Camera", pwndDlg, bExpanded, iIndex);
}
//---------------------------------------------------------------------------
