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

// NiMaterialPropPropertyWindowCreator.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiMaterialPropertyPropertyWindowCreator.h"
#include "NifPropertyWindow.h"
#include "ExtraDataDlg.h"
#include "NiMaterialPropertyDlg.h"

//---------------------------------------------------------------------------
void CNiMaterialPropertyPropertyWindowCreator::AddPages(NiObject* pkObj,
    CNifPropertyWindow* pkWindow)
{
    ASSERT(pkObj != NULL && pkWindow != NULL);
    AddBasicInfoDialog(pkObj, pkWindow, true);
    AddMaterialPropertyDialog(pkObj, pkWindow,true);
    AddTimeControllerDialog(pkObj, pkWindow,true);
    AddExtraDataDialog(pkObj, pkWindow,false);
    AddViewerStringsDialog(pkObj, pkWindow,false);
    pkWindow->ScrollToPage(0);
}
//---------------------------------------------------------------------------
NiRTTI* CNiMaterialPropertyPropertyWindowCreator::GetTargetRTTI()
{
    return (NiRTTI*) (&NiMaterialProperty::ms_RTTI);
}
//---------------------------------------------------------------------------
int CNiMaterialPropertyPropertyWindowCreator::AddMaterialPropertyDialog(
    NiObject* pkObj, CNifPropertyWindow* pkWindow, bool bExpanded, int iIndex)
{
    CNiMaterialPropertyDlg* pwndDlg = new CNiMaterialPropertyDlg;
    pwndDlg->SetNiObject(pkObj);
    
    pwndDlg->Create(MAKEINTRESOURCE(pwndDlg->IDD), 
        pkWindow->GetParentForPages());

    return pkWindow->InsertPage("Material Properties", pwndDlg, bExpanded,
        iIndex);
}
//---------------------------------------------------------------------------
