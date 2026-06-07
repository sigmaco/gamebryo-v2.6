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

// NiAVObjectPropertyWindowCreator.cpp

#include "stdafx.h"
#include "AssetViewer.h"
#include "NiAVObjectPropertyWindowCreator.h"
#include "NifPropertyWindow.h"
#include "WorldTransformsDlg.h"
#include "LocalTransformsDlg.h"
#include "NiBoundDlg.h"
#include "SelectiveUpdateDlg.h"
#include "NiAVObjectPropertiesDlg.h"
#include "CollisionDlg.h"

//---------------------------------------------------------------------------
void CNiAVObjectPropertyWindowCreator::AddPages(NiObject* pkObj, 
    CNifPropertyWindow* pkWindow)
{
    ASSERT(pkObj != NULL && pkWindow != NULL);
    ASSERT(NiIsKindOf(NiAVObject, pkObj));
    AddBasicInfoDialog(pkObj, pkWindow, true);
    AddLocalTransformsDialog(pkObj, pkWindow, true);
    AddWorldTransformsDialog(pkObj, pkWindow);
    AddNiBoundDialog(pkObj, pkWindow);
    AddTimeControllerDialog(pkObj, pkWindow);
    AddPropertiesDialog(pkObj, pkWindow);
    AddSelectiveUpdateDialog(pkObj, pkWindow, true);
    AddCollisionDialog(pkObj, pkWindow);
    AddExtraDataDialog(pkObj, pkWindow);
    AddViewerStringsDialog(pkObj, pkWindow);
    pkWindow->ScrollToPage(0);

}
//---------------------------------------------------------------------------
int CNiAVObjectPropertyWindowCreator::AddLocalTransformsDialog(
    NiObject* pkObj, CNifPropertyWindow* pkWindow, bool bExpanded, int iIndex)
{
    CLocalTransformsDlg* pwndDlg = new CLocalTransformsDlg;
    pwndDlg->SetNiObject(pkObj);
    
    pwndDlg->Create(MAKEINTRESOURCE(pwndDlg->IDD), 
        pkWindow->GetParentForPages());

    return pkWindow->InsertPage("Local Transforms", pwndDlg, bExpanded,
        iIndex);
}
//---------------------------------------------------------------------------
NiRTTI* CNiAVObjectPropertyWindowCreator::GetTargetRTTI()
{
    return (NiRTTI*) (&NiAVObject::ms_RTTI);
}
//---------------------------------------------------------------------------
int CNiAVObjectPropertyWindowCreator::AddWorldTransformsDialog(
    NiObject* pkObj, CNifPropertyWindow* pkWindow, bool bExpanded, int iIndex)
{
    CWorldTransformsDlg* pwndDlg = new CWorldTransformsDlg;
    pwndDlg->SetNiObject(pkObj);
    
    pwndDlg->Create(MAKEINTRESOURCE(pwndDlg->IDD), 
        pkWindow->GetParentForPages());

    return pkWindow->InsertPage("World Transforms", pwndDlg, bExpanded,
        iIndex);
}
//---------------------------------------------------------------------------
int CNiAVObjectPropertyWindowCreator::AddNiBoundDialog(NiObject* pkObj,
    CNifPropertyWindow* pkWindow, bool bExpanded, int iIndex)
{
    CNiBoundDlg* pwndDlg = new CNiBoundDlg;
    pwndDlg->SetNiObject(pkObj);
    
    pwndDlg->Create(MAKEINTRESOURCE(pwndDlg->IDD), 
        pkWindow->GetParentForPages());

    return pkWindow->InsertPage("World Bounds", pwndDlg, bExpanded, iIndex);
}
//---------------------------------------------------------------------------
int CNiAVObjectPropertyWindowCreator::AddSelectiveUpdateDialog(
    NiObject* pkObj, CNifPropertyWindow* pkWindow, bool bExpanded, int iIndex)
{
    CSelectiveUpdateDlg* pwndDlg = new CSelectiveUpdateDlg;
    pwndDlg->SetNiObject(pkObj);
    
    pwndDlg->Create(MAKEINTRESOURCE(pwndDlg->IDD), 
        pkWindow->GetParentForPages());

    return pkWindow->InsertPage("Update Flags", pwndDlg, bExpanded, 
        iIndex);
}
//---------------------------------------------------------------------------
int CNiAVObjectPropertyWindowCreator::AddPropertiesDialog(NiObject* pkObj,
    CNifPropertyWindow* pkWindow, bool bExpanded, int iIndex)
{
    CNiAVObjectPropertiesDlg* pwndDlg = new CNiAVObjectPropertiesDlg;
    pwndDlg->SetNiObject(pkObj);
    
    pwndDlg->Create(MAKEINTRESOURCE(pwndDlg->IDD), 
        pkWindow->GetParentForPages());

    return pkWindow->InsertPage("NiProperties", pwndDlg, bExpanded, 
        iIndex);
}
//---------------------------------------------------------------------------
int CNiAVObjectPropertyWindowCreator::AddCollisionDialog(NiObject* pkObj,
    CNifPropertyWindow* pkWindow, bool bExpanded, int iIndex)
{
    ASSERT(NiIsKindOf(NiAVObject, pkObj));
    if (((NiAVObject*)pkObj)->GetCollisionObject() == NULL)
        return FALSE;

    return AddViewerStringsDialog(
        ((NiAVObject*)pkObj)->GetCollisionObject(), pkWindow, 0, -1,
        "Collision Info", true);
}
//---------------------------------------------------------------------------
