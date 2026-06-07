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
#include "MUIToggleCollisionABVCommand.h"
#include "MSharedData.h"
#include "MSceneObject.h"
#include "NiOptimizeABV.h"
using namespace NiManagedToolInterface;

#define STR_COLLISION_WIREABV "SceneViewerCollisionWireABV"

//---------------------------------------------------------------------------
MUIToggleCollisionABVCommand::MUIToggleCollisionABVCommand()
{
    m_bOn = false;
    m_bEnabled = false;
}
//---------------------------------------------------------------------------
String* MUIToggleCollisionABVCommand::GetName()
{
    return "Toggle Collision Bounding Volumes";
}
//---------------------------------------------------------------------------
bool MUIToggleCollisionABVCommand::Execute(MUIState*)
{
    m_bOn = !m_bOn;
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();
    NiAVObject* pkObj = pkSharedData->GetScene(MSharedData::CHARACTER_INDEX);
    if (pkObj)
    {
        if (m_bOn)
            RecursiveSetWireABV(pkObj,false, true, true, false, true);
        else
            RecursiveSetWireABV(pkObj, true, true, true, false, false);
    }
    pkSharedData->Unlock();
    OnCommandExecuted(this);
    OnCommandStateChanged(this);
    return true;
}
//---------------------------------------------------------------------------
void MUIToggleCollisionABVCommand::DeleteContents()
{
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();
    m_bEnabled = false;
    pkSharedData->Unlock();
}
//---------------------------------------------------------------------------
void MUIToggleCollisionABVCommand::RefreshData()
{
    DeleteContents();
    AttachCollisionABV();
    OnCommandStateChanged(this);
}
//---------------------------------------------------------------------------
bool MUIToggleCollisionABVCommand::IsToggle()
{
    return true;
}
//---------------------------------------------------------------------------
bool MUIToggleCollisionABVCommand::GetActive()
{
    return m_bOn;
}
//---------------------------------------------------------------------------
bool MUIToggleCollisionABVCommand::GetEnabled()
{
    return m_bEnabled;
}
//---------------------------------------------------------------------------
void MUIToggleCollisionABVCommand::AttachCollisionABV()
{
    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();

    // Add wire frames to the node...
    NiColor kWireColor;
    kWireColor = NiColor(1.0f,1.0f,0.0f);

    NiAVObject* pkObj = pkSharedData->GetScene(MSharedData::CHARACTER_INDEX);
    
    if (pkObj)
    {
        NiOptimizeABV::CreateWireABV(pkObj, kWireColor, 1.019f,
            STR_COLLISION_WIREABV,true); 
    
        m_bEnabled = pkObj->GetObjectByName(STR_COLLISION_WIREABV) != NULL;
        pkObj->UpdateProperties();
        pkObj->UpdateEffects();
        pkObj->Update(0.0f);
        NiMesh::CompleteSceneModifiers(pkObj);

        if (m_bOn)
            RecursiveSetWireABV(pkObj,false, true, true, false, true);
        else
            RecursiveSetWireABV(pkObj, true, true, true, false, false);    
    }

    pkSharedData->Unlock();
}
//---------------------------------------------------------------------------
void MUIToggleCollisionABVCommand::RecursiveSetWireABV(NiAVObject* pkObj,
    bool bCull, bool bSelective, bool bTransform, bool bRigid, 
    bool bUpdateChild)
{
    if (NiIsKindOf(NiNode,pkObj))
    {
        // Call for each child...
        NiNode* pkNode = (NiNode*)pkObj;

        unsigned int uiTotal = pkNode->GetArrayCount();
        for(unsigned int ui=0;ui<uiTotal;ui++)
        {
            NiAVObject* pkChildObj = pkNode->GetAt(ui);

            if(pkChildObj == NULL)
                continue;

            RecursiveSetWireABV(pkChildObj,bCull,
                bSelective,bTransform, bRigid,bUpdateChild);
        }
    }

    if (pkObj->GetName())
    {
        if (strcmp(pkObj->GetName(), STR_COLLISION_WIREABV)==0)
        {
            if (!MSceneObject::IsToolOnlyObject(pkObj))
                MSceneObject::SetToolOnlyObject(pkObj, true);

            pkObj->SetAppCulled(bCull);
            pkObj->SetSelectiveUpdate(bSelective);
            pkObj->SetSelectiveUpdateTransforms(bTransform);
            pkObj->SetSelectiveUpdateRigid(bRigid);

            NIASSERT(NiIsKindOf(NiNode,pkObj));

            NiAVObject* pkChild = ((NiNode*)pkObj)->GetAt(0);
            
            NIASSERT(pkChild);
            NIASSERT(NiIsKindOf(NiMesh,pkChild));

            if (bUpdateChild)
            {
                pkChild->SetAppCulled(false);
                pkChild->SetSelectiveUpdate(true);
                pkChild->SetSelectiveUpdateTransforms(true);
                pkChild->SetSelectiveUpdateRigid(false);
            }
        }
    }
}
//---------------------------------------------------------------------------
MUICommand::CommandType MUIToggleCollisionABVCommand::GetCommandType()
{
    return MUICommand::TOGGLE_COLLISION_ABVS;
}
//---------------------------------------------------------------------------
