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

// NiDynamicEffectPropertyWindowCreator.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiDynamicEffectPropertyWindowCreator.h"
#include "NifPropertyWindow.h"
#include "DynamicEffectDlg.h"

//---------------------------------------------------------------------------
void CNiDynamicEffectPropertyWindowCreator::AddPages(NiObject* pkObj,
    CNifPropertyWindow* pkWindow)
{
    ASSERT(pkObj != NULL && pkWindow != NULL);
    ASSERT(NiIsKindOf(NiAVObject, pkObj));
    AddBasicInfoDialog(pkObj, pkWindow, true);
    AddLocalTransformsDialog(pkObj, pkWindow, true);
    AddWorldTransformsDialog(pkObj, pkWindow);
    AddNiBoundDialog(pkObj, pkWindow);
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
NiRTTI* CNiDynamicEffectPropertyWindowCreator::GetTargetRTTI()
{
    return (NiRTTI*) (&NiDynamicEffect::ms_RTTI);
}
//---------------------------------------------------------------------------
int CNiDynamicEffectPropertyWindowCreator::AddDynamicEffectDialog(
    NiObject* pkObj, CNifPropertyWindow* pkWindow, bool bExpanded, int iIndex)
{
    CDynamicEffectDlg* pwndDlg = new CDynamicEffectDlg;
    pwndDlg->SetNiObject(pkObj);
    
    pwndDlg->Create(MAKEINTRESOURCE(pwndDlg->IDD), 
        pkWindow->GetParentForPages());

    return pkWindow->InsertPage("Dynamic Effect Info", pwndDlg, bExpanded,
        iIndex);
}
//---------------------------------------------------------------------------
