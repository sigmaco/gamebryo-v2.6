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

// NiShadowGeneratorPropertyWindowCreator.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiShadowGeneratorPropertyWindowCreator.h"
#include "NifPropertyWindow.h"
#include "ShadowGeneratorDlg.h"

//---------------------------------------------------------------------------
void CNiShadowGeneratorPropertyWindowCreator::AddPages(NiObject* pkObj,
    CNifPropertyWindow* pkWindow)
{
    NIASSERT(pkObj != NULL && pkWindow != NULL);
    NIASSERT(NiIsKindOf(NiShadowGenerator, pkObj));
    AddBasicInfoDialog(pkObj, pkWindow, true);
    AddShadowGeneratorDialog(pkObj, pkWindow, true);
    AddViewerStringsDialog(pkObj, pkWindow, true);
    pkWindow->ScrollToPage(0);
}
//---------------------------------------------------------------------------
NiRTTI* CNiShadowGeneratorPropertyWindowCreator::GetTargetRTTI()
{
    return (NiRTTI*) (&NiShadowGenerator::ms_RTTI);
}
//---------------------------------------------------------------------------
int CNiShadowGeneratorPropertyWindowCreator::AddShadowGeneratorDialog(
    NiObject* pkObj, CNifPropertyWindow* pkWindow, bool bExpanded, int iIndex)
{
    CShadowGeneratorDlg* pwndDlg = new CShadowGeneratorDlg;
    pwndDlg->SetNiObject(pkObj);
    
    pwndDlg->Create(MAKEINTRESOURCE(pwndDlg->IDD), 
        pkWindow->GetParentForPages());

    return pkWindow->InsertPage("Shadow Generator Info", pwndDlg, bExpanded,
        iIndex);
}
//---------------------------------------------------------------------------
