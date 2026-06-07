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

// NiTexturingPropPropertyWindowCreator.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiTexturingPropertyPropertyWindowCreator.h"
#include "NifPropertyWindow.h"
#include "ExtraDataDlg.h"
#include "NiTexturingPropertyDlg.h"

//---------------------------------------------------------------------------
void CNiTexturingPropertyPropertyWindowCreator::AddPages(NiObject* pkObj,
    CNifPropertyWindow* pkWindow)
{
    ASSERT(pkObj != NULL && pkWindow != NULL);
    AddBasicInfoDialog(pkObj, pkWindow, true);
    AddTexturingPropertyDialog(pkObj, pkWindow,true);
    AddTimeControllerDialog(pkObj, pkWindow,true);
    AddExtraDataDialog(pkObj, pkWindow,false);
    AddViewerStringsDialog(pkObj, pkWindow,false);
    pkWindow->ScrollToPage(0);
}
//---------------------------------------------------------------------------
NiRTTI* CNiTexturingPropertyPropertyWindowCreator::GetTargetRTTI()
{
    return (NiRTTI*) (&NiTexturingProperty::ms_RTTI);
}
//---------------------------------------------------------------------------
int CNiTexturingPropertyPropertyWindowCreator::AddTexturingPropertyDialog(
    NiObject* pkObj, CNifPropertyWindow* pkWindow, bool bExpanded, int iIndex)
{
    CNiTexturingPropertyDlg* pwndDlg = new CNiTexturingPropertyDlg;
    pwndDlg->SetNiObject(pkObj);
    
    pwndDlg->Create(MAKEINTRESOURCE(pwndDlg->IDD), 
        pkWindow->GetParentForPages());

    return pkWindow->InsertPage("Texture Maps", pwndDlg, bExpanded, iIndex);
}
//---------------------------------------------------------------------------
