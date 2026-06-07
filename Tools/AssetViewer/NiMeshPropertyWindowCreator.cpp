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

// NiMeshPropertyWindowCreator.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiMeshPropertyWindowCreator.h"
#include "NifPropertyWindow.h"
#include "NiMeshDlg.h"
#include "NiMeshModifierDlg.h"

//---------------------------------------------------------------------------
void CNiMeshPropertyWindowCreator::AddPages(NiObject* pkObj,
    CNifPropertyWindow* pkWindow)
{
    ASSERT(pkObj != NULL && pkWindow != NULL);
    ASSERT(NiIsKindOf(NiMesh, pkObj));
    AddBasicInfoDialog(pkObj, pkWindow, true);
    AddLocalTransformsDialog(pkObj, pkWindow, true);
    AddWorldTransformsDialog(pkObj, pkWindow);
    AddNiBoundDialog(pkObj, pkWindow);
    AddMeshDialog(pkObj, pkWindow, true);
    AddTimeControllerDialog(pkObj, pkWindow);
    AddPropertiesDialog(pkObj, pkWindow);
    AddSelectiveUpdateDialog(pkObj, pkWindow, true);
    AddCollisionDialog(pkObj, pkWindow);
    AddExtraDataDialog(pkObj, pkWindow);
    AddViewerStringsDialog(pkObj, pkWindow);
    pkWindow->ScrollToPage(0);
}
//---------------------------------------------------------------------------
NiRTTI* CNiMeshPropertyWindowCreator::GetTargetRTTI()
{
    return (NiRTTI*) (&NiMesh::ms_RTTI);
}
//---------------------------------------------------------------------------
int CNiMeshPropertyWindowCreator::AddMeshDialog(NiObject* pkObj,
    CNifPropertyWindow* pkWindow, bool bExpanded, int iIndex)
{
    CNiMeshDlg* pwndDlg = new CNiMeshDlg;
    pwndDlg->SetNiObject(pkObj);
    
    pwndDlg->Create(MAKEINTRESOURCE(pwndDlg->IDD), 
        pkWindow->GetParentForPages());

    int iPageIndex = pkWindow->InsertPage("Mesh", pwndDlg, bExpanded, iIndex);

    return iPageIndex;
}
//---------------------------------------------------------------------------
