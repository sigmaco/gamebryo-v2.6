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

// NiGeometryDataPropertyWindowCreator.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiGeometryDataPropertyWindowCreator.h"
#include "NifPropertyWindow.h"
#include "NiGeometryDataAttributesInfoDlg.h"

//---------------------------------------------------------------------------
void CNiGeometryDataPropertyWindowCreator::AddPages(NiObject* pkObj,
    CNifPropertyWindow* pkWindow)
{
    ASSERT(pkObj != NULL && pkWindow != NULL);
    ASSERT(NiIsKindOf(NiGeometryData, pkObj));
    AddBasicInfoDialog(pkObj, pkWindow, true);
    AddGeometryDataAttributesDialog(pkObj, pkWindow, true);
    AddViewerStringsDialog(pkObj, pkWindow, true);
    pkWindow->ScrollToPage(0);
}
//---------------------------------------------------------------------------
int CNiGeometryDataPropertyWindowCreator::AddGeometryDataAttributesDialog(
    NiObject* pkObj, CNifPropertyWindow* pkWindow, bool bExpanded, int iIndex)
{
    CNiGeometryDataAttributesInfoDlg* pwndDlg = 
        new CNiGeometryDataAttributesInfoDlg;
    pwndDlg->SetNiObject(pkObj);
    
    pwndDlg->Create(MAKEINTRESOURCE(pwndDlg->IDD), 
        pkWindow->GetParentForPages());

    return pkWindow->InsertPage("Geometry Data Attributes", 
        pwndDlg, bExpanded, iIndex);
}
//---------------------------------------------------------------------------
NiRTTI* CNiGeometryDataPropertyWindowCreator::GetTargetRTTI()
{
    return (NiRTTI*) (&NiGeometryData::ms_RTTI);
}
//---------------------------------------------------------------------------
