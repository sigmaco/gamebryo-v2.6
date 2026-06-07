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

#include "stdafx.h"
#include "MUIToggleSceneLightsCommand.h"
#include "MSharedData.h"
using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
MUIToggleSceneLightsCommand::MUIToggleSceneLightsCommand()
{
    m_bOn = true;
    m_uiSceneLightsIdx = (unsigned int)-1;
}
//---------------------------------------------------------------------------
String* MUIToggleSceneLightsCommand::GetName()
{
    return "Toggle Scene Lights";
}
//---------------------------------------------------------------------------
bool MUIToggleSceneLightsCommand::Execute(MUIState*)
{
    NiDirectionalLight* pkLight1 = GetDirLight1();
    NiDirectionalLight* pkLight2 = GetDirLight2();

    if (!pkLight1 || !pkLight2)
        return false;

    m_bOn = !m_bOn;
    if (m_bOn)
    {
        pkLight1->SetDimmer(1.0f);
        pkLight2->SetDimmer(1.0f);
    }
    else
    {
        pkLight1->SetDimmer(0.0f);
        pkLight2->SetDimmer(0.0f);
    }
    OnCommandExecuted(this);
    OnCommandStateChanged(this);
    return true;
}
//---------------------------------------------------------------------------
void MUIToggleSceneLightsCommand::DeleteContents()
{
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();

    NiDirectionalLight* pkLight1 = GetDirLight1();
    NiDirectionalLight* pkLight2 = GetDirLight2();

    if (!pkLight1 || !pkLight2)
        return;

    pkLight1->DetachAllAffectedNodes();
    pkLight2->DetachAllAffectedNodes();

    NiNode* pkScene = NiDynamicCast(NiNode, 
        pkSharedData->GetScene(MSharedData::CHARACTER_INDEX));
    if (pkScene)
    {
        pkScene->UpdateEffects();
    }

    NiNode* pkBackground = NiDynamicCast(NiNode, 
        pkSharedData->GetScene(MSharedData::BACKGROUND_INDEX));

    if (pkBackground)
    {
        pkBackground->UpdateEffects();
    }
    pkSharedData->Unlock();
}
//---------------------------------------------------------------------------
void MUIToggleSceneLightsCommand::RefreshData()
{
    DeleteContents();
    CreateDefaultLights();
    AttachSceneLights();
    OnCommandStateChanged(this);
}
//---------------------------------------------------------------------------
bool MUIToggleSceneLightsCommand::IsToggle()
{
    return true;
}
//---------------------------------------------------------------------------
bool MUIToggleSceneLightsCommand::GetActive()
{
    return m_bOn;
}
//---------------------------------------------------------------------------
bool MUIToggleSceneLightsCommand::GetEnabled()
{
    return true;
}
//---------------------------------------------------------------------------
NiNode* MUIToggleSceneLightsCommand::CreateDefaultLights()
{
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();

    if (m_uiSceneLightsIdx != -1)
    {
        NiAVObject* pkObj = pkSharedData->GetScene(m_uiSceneLightsIdx);
        if ( pkObj != NULL)
        {
            pkSharedData->Unlock();
            return NiDynamicCast(NiNode, pkObj);
        }
    }

    NiNode* pkLightRoot = NiNew NiNode();
    NiDirectionalLight* pkDirLight1 = NiNew NiDirectionalLight;
    pkDirLight1->SetName("DirLight1");
    pkDirLight1->SetAmbientColor(NiColor::BLACK);
    pkDirLight1->SetDiffuseColor(NiColor::WHITE);
    pkDirLight1->SetSpecularColor(NiColor::WHITE);
    NiMatrix3 kRot1(
        NiPoint3(0.44663f, 0.496292f, -0.744438f),
        NiPoint3(0.0f, -0.83205f, -0.5547f),
        NiPoint3(-0.894703f, 0.247764f, -0.371646f));
    pkDirLight1->SetRotate(kRot1);
    
    NiDirectionalLight* pkDirLight2 = NiNew NiDirectionalLight;
    pkDirLight2->SetName("DirLight2");
    pkDirLight2->SetAmbientColor(NiColor::BLACK);
    pkDirLight2->SetDiffuseColor(NiColor::WHITE);
    pkDirLight2->SetSpecularColor(NiColor::WHITE);
    NiMatrix3 kRot2(
        NiPoint3(-0.44663f, -0.496292f, 0.744438f),
        NiPoint3(0.0f, 0.83205f, 0.5547f),
        NiPoint3(-0.894703f, 0.247764f, -0.371646f));
    pkDirLight2->SetRotate(kRot2);

    pkLightRoot->AttachChild(pkDirLight1);
    pkLightRoot->AttachChild(pkDirLight2);
    if (m_bOn)
    {
        pkDirLight1->SetDimmer(1.0f);
        pkDirLight2->SetDimmer(1.0f);
    }
    else
    {
        pkDirLight1->SetDimmer(0.0f);
        pkDirLight2->SetDimmer(0.0f);
    }
    pkLightRoot->Update(0.0f);
    m_uiSceneLightsIdx = pkSharedData->AddScene(pkLightRoot);
    pkSharedData->Unlock();
            
    return pkLightRoot;
}
//---------------------------------------------------------------------------
void MUIToggleSceneLightsCommand::AttachSceneLights()
{
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();

    NiDirectionalLight* pkLight1 = GetDirLight1();
    NiDirectionalLight* pkLight2 = GetDirLight2();

    if (!pkLight1 || !pkLight2)
        return;
    pkLight1->DetachAllAffectedNodes();
    pkLight2->DetachAllAffectedNodes();

    NiNode* pkScene = NiDynamicCast(NiNode, 
        pkSharedData->GetScene(MSharedData::CHARACTER_INDEX));
    if (pkScene)
    {
        pkLight1->AttachAffectedNode(pkScene);
        pkLight2->AttachAffectedNode(pkScene);
        pkScene->UpdateEffects();
    }

    NiNode* pkBackground = NiDynamicCast(NiNode, 
        pkSharedData->GetScene(MSharedData::BACKGROUND_INDEX));

    if (pkBackground)
    {
        pkLight1->AttachAffectedNode(pkBackground);
        pkLight2->AttachAffectedNode(pkBackground);
        pkBackground->UpdateEffects();
    }
    
    pkSharedData->Unlock();
}
//---------------------------------------------------------------------------
NiDirectionalLight* MUIToggleSceneLightsCommand::GetDirLight1()
{
    NiDirectionalLight* pkLight = NULL;
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();

    if (m_uiSceneLightsIdx != -1)
    {
        NiAVObject* pkObj = pkSharedData->GetScene(m_uiSceneLightsIdx);
        if ( pkObj != NULL)
        {
           NiObject* pkTempObj = pkObj->GetObjectByName("DirLight1");
           pkLight = NiDynamicCast(NiDirectionalLight, pkTempObj);
        }
    }
    pkSharedData->Unlock();
    return pkLight;
}
//---------------------------------------------------------------------------
NiDirectionalLight* MUIToggleSceneLightsCommand::GetDirLight2()
{
    NiDirectionalLight* pkLight = NULL;
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();

    if (m_uiSceneLightsIdx != -1)
    {
        NiAVObject* pkObj = pkSharedData->GetScene(m_uiSceneLightsIdx);
        if ( pkObj != NULL)
        {
           NiObject* pkTempObj = pkObj->GetObjectByName("DirLight2");
           pkLight = NiDynamicCast(NiDirectionalLight, pkTempObj);
        }
    }
    pkSharedData->Unlock();
    return pkLight;
}
//---------------------------------------------------------------------------
MUICommand::CommandType MUIToggleSceneLightsCommand::GetCommandType()
{
    return MUICommand::TOGGLE_SCENE_LIGHTS;
}
//---------------------------------------------------------------------------
