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

// NiLightPropertyWindowCreator.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiLightPropertyWindowCreator.h"
#include "NifPropertyWindow.h"
#include "NiLightDlg.h"

//---------------------------------------------------------------------------
void CNiLightPropertyWindowCreator::AddPages(NiObject* pkObj, 
    CNifPropertyWindow* pkWindow)
{
    ASSERT(pkObj != NULL && pkWindow != NULL);
    ASSERT(NiIsKindOf(NiAVObject, pkObj));
    AddBasicInfoDialog(pkObj, pkWindow, true);
    AddLocalTransformsDialog(pkObj, pkWindow, true);
    AddWorldTransformsDialog(pkObj, pkWindow);
    AddNiBoundDialog(pkObj, pkWindow);
    AddLightDialog(pkObj, pkWindow, true);
    AddDynamicEffectDialog(pkObj, pkWindow, true);
    AddTimeControllerDialog(pkObj, pkWindow);
    AddPropertiesDialog(pkObj, pkWindow);
    AddSelectiveUpdateDialog(pkObj, pkWindow, true);
    AddCollisionDialog(pkObj, pkWindow);
    AddExtraDataDialog(pkObj, pkWindow);
    AddViewerStringsDialog(pkObj, pkWindow);
    pkWindow->ScrollToPage(0);
}
//---------------------------------------------------------------------------
NiRTTI* CNiLightPropertyWindowCreator::GetTargetRTTI()
{
    return (NiRTTI*) (&NiLight::ms_RTTI);
}
//---------------------------------------------------------------------------
int CNiLightPropertyWindowCreator::AddLightDialog(NiObject* pkObj,
    CNifPropertyWindow* pkWindow, bool bExpanded, int iIndex)
{
    CNiLightDlg* pwndDlg = new CNiLightDlg;
    pwndDlg->SetNiObject(pkObj);
    
    pwndDlg->Create(MAKEINTRESOURCE(pwndDlg->IDD), 
        pkWindow->GetParentForPages());

    return pkWindow->InsertPage("Light Info", pwndDlg, bExpanded, iIndex);
}
//---------------------------------------------------------------------------
