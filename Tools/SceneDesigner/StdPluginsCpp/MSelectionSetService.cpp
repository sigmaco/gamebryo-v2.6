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

// Precompiled Header
#include "StdPluginsCppPCH.h"

#include "MSelectionSetService.h"

using namespace Emergent::Gamebryo::SceneDesigner::StdPluginsCpp;

//---------------------------------------------------------------------------
MSelectionSetService::MSelectionSetService()
{
}
//---------------------------------------------------------------------------
void MSelectionSetService::Do_Dispose(bool)
{
}
//---------------------------------------------------------------------------
// IService members.
//---------------------------------------------------------------------------
String* MSelectionSetService::get_Name()
{
    MVerifyValidInstance;

    return "Selection Set Service";
}
//---------------------------------------------------------------------------
bool MSelectionSetService::Initialize()
{
    MVerifyValidInstance;

    return true;
}
//---------------------------------------------------------------------------
bool MSelectionSetService::Start()
{
    MVerifyValidInstance;

    return true;
}
//---------------------------------------------------------------------------
// ISelectionSetService members.
//---------------------------------------------------------------------------
unsigned int MSelectionSetService::get_SelectionSetCount()
{
    MVerifyValidInstance;

    MLayer* pmLayer = MFramework::Instance->LayerManager->ActiveLayer;
    return pmLayer ? pmLayer->SelectionSetCount : 0;
}
//---------------------------------------------------------------------------
MSelectionSet* MSelectionSetService::GetSelectionSets()[]
{
    MVerifyValidInstance;

    MLayer* pmLayer = MFramework::Instance->LayerManager->ActiveLayer;
    return pmLayer ? 
        MFramework::Instance->LayerManager->ActiveLayer->GetSelectionSets()
        : NULL;
}
//---------------------------------------------------------------------------
MSelectionSet* MSelectionSetService::GetSelectionSetByName(String* strName)
{
    MVerifyValidInstance;

    MLayer* pmLayer = MFramework::Instance->LayerManager->ActiveLayer;
    return pmLayer ? MFramework::Instance->LayerManager->ActiveLayer->
        GetSelectionSetByName(strName) : NULL;
}
//---------------------------------------------------------------------------
bool MSelectionSetService::AddSelectionSet(MSelectionSet* pmSelectionSet)
{
    MVerifyValidInstance;

    MLayer* pmLayer = MFramework::Instance->LayerManager->ActiveLayer;
    return pmLayer ? MFramework::Instance->LayerManager->ActiveLayer->
        AddSelectionSet(pmSelectionSet) : false;
}
//---------------------------------------------------------------------------
void MSelectionSetService::RemoveSelectionSet(MSelectionSet* pmSelectionSet)
{
    MVerifyValidInstance;

    MLayer* pmLayer = MFramework::Instance->LayerManager->ActiveLayer;
    if (pmLayer)
        pmLayer->RemoveSelectionSet(pmSelectionSet);
}
//---------------------------------------------------------------------------
void MSelectionSetService::RemoveAllSelectionSets()
{
    MVerifyValidInstance;

    MLayer* pmLayer = MFramework::Instance->LayerManager->ActiveLayer;
    if (pmLayer)
        pmLayer->RemoveAllSelectionSets();
}
//---------------------------------------------------------------------------
