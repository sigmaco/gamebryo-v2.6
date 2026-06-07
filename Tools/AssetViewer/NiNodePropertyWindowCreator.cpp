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

// NiNodePropertyWindowCreator.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiNodePropertyWindowCreator.h"
#include "NifPropertyWindow.h"
#include "ChildrenDlg.h"
#include "NiNodeEffectsDlg.h"

//---------------------------------------------------------------------------
void CNiNodePropertyWindowCreator::AddPages(NiObject* pkObj,
    CNifPropertyWindow* pkWindow)
{
    ASSERT(pkObj != NULL && pkWindow != NULL);
    ASSERT(NiIsKindOf(NiNode, pkObj));
    AddBasicInfoDialog(pkObj, pkWindow, true);
    AddLocalTransformsDialog(pkObj, pkWindow, true);
    AddWorldTransformsDialog(pkObj, pkWindow);
    AddNiBoundDialog(pkObj, pkWindow);
    AddChildrenDialog(pkObj, pkWindow, true);
    AddSelectiveUpdateDialog(pkObj, pkWindow, true);
    AddTimeControllerDialog(pkObj, pkWindow);
    AddPropertiesDialog(pkObj, pkWindow);
    AddEffectsDialog(pkObj, pkWindow);
    AddCollisionDialog(pkObj, pkWindow);
    AddExtraDataDialog(pkObj, pkWindow);
    AddViewerStringsDialog(pkObj, pkWindow);
    pkWindow->ScrollToPage(0);
}
//---------------------------------------------------------------------------
NiRTTI* CNiNodePropertyWindowCreator::GetTargetRTTI()
{
    return (NiRTTI*) (&NiNode::ms_RTTI);
}
//---------------------------------------------------------------------------
int CNiNodePropertyWindowCreator::AddChildrenDialog(NiObject* pkObj,
    CNifPropertyWindow* pkWindow, bool bExpanded, int iIndex)
{
    CChildrenDlg* pwndDlg = new CChildrenDlg;
    pwndDlg->SetNiObject(pkObj);
    
    pwndDlg->Create(MAKEINTRESOURCE(pwndDlg->IDD), 
        pkWindow->GetParentForPages());

    return pkWindow->InsertPage("Children", pwndDlg, bExpanded, iIndex);
}
//---------------------------------------------------------------------------
int CNiNodePropertyWindowCreator::AddEffectsDialog(NiObject* pkObj,
    CNifPropertyWindow* pkWindow, bool bExpanded, int iIndex)
{
    CNiNodeEffectsDlg* pwndDlg = new CNiNodeEffectsDlg;
    pwndDlg->SetNiObject(pkObj);
    
    pwndDlg->Create(MAKEINTRESOURCE(pwndDlg->IDD), 
        pkWindow->GetParentForPages());

    return pkWindow->InsertPage("Dynamic Effects", pwndDlg, bExpanded,
        iIndex);
}
//---------------------------------------------------------------------------
