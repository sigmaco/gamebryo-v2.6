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

// NiMeshModifierPropertyWindowCreator.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiMeshModifierPropertyWindowCreator.h"
#include "NifPropertyWindow.h"
#include "NiMeshModifierDlg.h"

//---------------------------------------------------------------------------
void CNiMeshModifierPropertyWindowCreator::AddPages(NiObject* pkObj,
    CNifPropertyWindow* pkWindow)
{
    ASSERT(pkObj != NULL && pkWindow != NULL);
    ASSERT(NiIsKindOf(NiMeshModifier, pkObj));
    AddBasicInfoDialog(pkObj, pkWindow, true);
    AddMeshModifierDialog(pkObj, pkWindow, true);
    AddViewerStringsDialog(pkObj, pkWindow);
    pkWindow->ScrollToPage(0);
}
//---------------------------------------------------------------------------
NiRTTI* CNiMeshModifierPropertyWindowCreator::GetTargetRTTI()
{
    return (NiRTTI*) (&NiMeshModifier::ms_RTTI);
}
//---------------------------------------------------------------------------
int CNiMeshModifierPropertyWindowCreator::AddMeshModifierDialog(
    NiObject* pkObj, CNifPropertyWindow* pkWindow, bool bExpanded, int iIndex)
{
    CNiMeshModifierDlg* pwndDlg = new CNiMeshModifierDlg;
    pwndDlg->SetNiObject(pkObj);
    
    pwndDlg->Create(MAKEINTRESOURCE(pwndDlg->IDD), 
        pkWindow->GetParentForPages());

    int iPageIndex = pkWindow->InsertPage("Mesh Modifier", pwndDlg, bExpanded, 
        iIndex);

    return iPageIndex;
}
//---------------------------------------------------------------------------
