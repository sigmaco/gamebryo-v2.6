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

// NiKeyBasedInterpolatorPropertyWindowCreator.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiKeyBasedInterpolatorPropertyWindowCreator.h"
#include "NifPropertyWindow.h"
#include "NiKeyBasedInterpolatorKeysDlg.h"

//---------------------------------------------------------------------------
void CNiKeyBasedInterpolatorPropertyWindowCreator::AddPages(NiObject* pkObj,
    CNifPropertyWindow* pkWindow)
{
    ASSERT(pkObj != NULL && pkWindow != NULL);
    AddBasicInfoDialog(pkObj, pkWindow, true);
    AddAnimationKeyDialog(pkObj, pkWindow, true);
    AddViewerStringsDialog(pkObj, pkWindow,true);
    pkWindow->ScrollToPage(1);
}
//---------------------------------------------------------------------------
NiRTTI* CNiKeyBasedInterpolatorPropertyWindowCreator::GetTargetRTTI()
{
    return (NiRTTI*) (&NiKeyBasedInterpolator::ms_RTTI);
}
//---------------------------------------------------------------------------
int CNiKeyBasedInterpolatorPropertyWindowCreator::AddAnimationKeyDialog(
    NiObject* pkObj, CNifPropertyWindow* pkWindow, bool bExpanded, int iIndex)
{
    CNiKeyBasedInterpolatorKeysDlg* pwndDlg = 
        new CNiKeyBasedInterpolatorKeysDlg;
    pwndDlg->SetNiObject(pkObj);
    
    pwndDlg->Create(MAKEINTRESOURCE(pwndDlg->IDD), 
        pkWindow->GetParentForPages());

    return pkWindow->InsertPage("Animation Keys", pwndDlg, bExpanded,
        iIndex);
}
//---------------------------------------------------------------------------
