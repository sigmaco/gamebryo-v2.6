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

// NifPropertyWindowFactory.cpp

#include "stdafx.h"
#include "NifPropertyWindowFactory.h"
#include "NifPropertyWindow.h"
#include "NiObjectPropertyWindowCreator.h"

NiTMap<NiRTTI*, CNiObjectPropertyWindowCreatorPtr>*
    CNifPropertyWindowFactory::ms_pkCreatorMap = NULL;

//---------------------------------------------------------------------------
CNifPropertyWindow* CNifPropertyWindowFactory::CreatePropertyWindow(
    NiObject* pkObj, int iStartX, int iStartY, int iWidth, int iHeight)
{
    NIASSERT(pkObj != NULL);
    if(pkObj == NULL)
        return NULL;
    
    CNifPropertyWindow* pkWindow = NULL;
    CNiObjectPropertyWindowCreatorPtr spCreator = NULL;
    bool bFoundItem = false;
    NiRTTI* pkRTTI = (NiRTTI*)pkObj->GetRTTI();

    while(!bFoundItem && pkRTTI != NULL)
    {
        bFoundItem = ms_pkCreatorMap->GetAt(pkRTTI, spCreator);
        if(bFoundItem && spCreator != NULL)
        {
            pkWindow = spCreator->CreatePropertyWindow(pkObj,
                iStartX, iStartY, iWidth, iHeight);
            NIASSERT(pkWindow != NULL);
        }
        else
        {
            pkRTTI = (NiRTTI*) pkRTTI->GetBaseRTTI();
        }
    }
    
    return pkWindow;
}
//---------------------------------------------------------------------------
void CNifPropertyWindowFactory::RegisterNiObjectPropertyCreator(
    CNiObjectPropertyWindowCreator* pkCreator)
{
    if(pkCreator == NULL)
        return;

    ms_pkCreatorMap->SetAt(pkCreator->GetTargetRTTI(), pkCreator);
}
//---------------------------------------------------------------------------
void CNifPropertyWindowFactory::RemoveAllCreators()
{
    ms_pkCreatorMap->RemoveAll();
}
//---------------------------------------------------------------------------
void CNifPropertyWindowFactory::Init()
{
    NIASSERT(ms_pkCreatorMap == NULL);
    ms_pkCreatorMap = NiNew NiTMap<NiRTTI*, 
        CNiObjectPropertyWindowCreatorPtr>(151);
}
//---------------------------------------------------------------------------
void CNifPropertyWindowFactory::Shutdown()
{
    NiDelete ms_pkCreatorMap;
    ms_pkCreatorMap = NULL;
}
//---------------------------------------------------------------------------
