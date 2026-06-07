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
#include "NiDataStreamPropertyWindowCreator.h"
#include "NifPropertyWindow.h"
#include "NiDataStreamDlg.h"

//---------------------------------------------------------------------------
void CNiDataStreamPropertyWindowCreator::AddPages(NiObject* pkObj,
    CNifPropertyWindow* pkWindow)
{
    ASSERT(pkObj != NULL && pkWindow != NULL);
    ASSERT(NiIsKindOf(NiDataStreamObject, pkObj));
    AddDataStreamDialog(pkObj, pkWindow, true);
    pkWindow->ScrollToPage(0);
}
//---------------------------------------------------------------------------
NiRTTI* CNiDataStreamPropertyWindowCreator::GetTargetRTTI()
{
    return (NiRTTI*) (&NiDataStreamObject::ms_RTTI);
}
//---------------------------------------------------------------------------
int CNiDataStreamPropertyWindowCreator::AddDataStreamDialog(NiObject* pkObj,
    CNifPropertyWindow* pkWindow, bool bExpanded, int iIndex)
{
    CNiDataStreamDlg* pwndDlg = new CNiDataStreamDlg();
    pwndDlg->SetNiObject(pkObj);
    
    pwndDlg->Create(MAKEINTRESOURCE(pwndDlg->IDD), 
        pkWindow->GetParentForPages());

    ASSERT(NiIsKindOf(NiDataStreamObject, pkObj));
    NiDataStreamObject* pkDataStreamObj = (NiDataStreamObject*)pkObj;

    char acPageName[256];
    NiSprintf(acPageName, sizeof(acPageName), "%s_%d", 
        pkDataStreamObj->m_kSemantic, pkDataStreamObj->m_uiSemanticIndex);

    int iPageIndex = pkWindow->InsertPage(acPageName, pwndDlg, 
        bExpanded, iIndex);

    return iPageIndex;
}
//---------------------------------------------------------------------------
