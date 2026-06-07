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
#include "MUIResetBackgroundHeightCommand.h"
#include "MFramework.h"
#include "MSharedData.h"
#include "MUIState.h"
using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
MUIResetBackgroundHeightCommand::MUIResetBackgroundHeightCommand()
{

}
//---------------------------------------------------------------------------
String* MUIResetBackgroundHeightCommand::GetName()
{
    return "Reset Background Height";
}
//---------------------------------------------------------------------------
bool MUIResetBackgroundHeightCommand::Execute(MUIState*)
{
    MCamera* pkMCamera = MFramework::Instance->Renderer->ActiveCamera;
    NIASSERT(pkMCamera);

    NiCamera* pkCamera = pkMCamera->GetCamera();
    if (!pkCamera)
        return false;

    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();

    NiKFMTool* pkKFM = pkSharedData->GetKFMTool();
    if (!pkKFM)
    {
        pkSharedData->Unlock();
        return false;
    }


    NiAVObject* pkBackground =
        pkSharedData->GetScene(MSharedData::BACKGROUND_INDEX);
    if (!pkBackground)
    {
        pkSharedData->Unlock();
        return false;
    }
    else
    {
        pkBackground->SetTranslate(0.0f, 0.0f, 0.0f);
        pkBackground->SetRotate(NiMatrix3::IDENTITY);
        pkBackground->SetScale(1.0f);
        pkBackground->Update(0.0f);
        NiMesh::CompleteSceneModifiers(pkBackground);
    }
    
     // If default floor, rotate it to face the up direction.
    if (pkMCamera->UpAxis == MCamera::Y_AXIS)
    {
        NiMatrix3 kRot;
        kRot.MakeXRotation(NI_PI * 0.5f);
        pkBackground->SetRotate(kRot * pkBackground->GetRotate());
    }

    NiAVObject* pkAccumRoot = NULL;
    NiAVObject* pkModelRoot = NULL;

    float fRadius = 1.0f;
    NiPoint3 kCenter = NiPoint3::ZERO;

    NiActorManager* pkActor = pkSharedData->GetActorManager();
    if (pkActor)
    {
        pkAccumRoot = pkActor->GetAccumRoot();
        pkModelRoot = pkActor->GetNIFRoot();
        NiBound kBound;
        MUtility::GetWorldBounds(pkModelRoot, kBound, false);
        kCenter = kBound.GetCenter();
        fRadius = kBound.GetRadius();
    }

    if (fRadius == 0.0f)
    {
        fRadius = 100.0f;
    }
    
    if (pkMCamera->UpAxis == MCamera::Y_AXIS)
    {
        pkBackground->SetTranslate(NiPoint3(0.0f,
            kCenter.y - fRadius +
            MFramework::Instance->Input->BackgroundHeight, 0.0f));
    }
    else if (pkMCamera->UpAxis == MCamera::Z_AXIS)
    {
        pkBackground->SetTranslate(NiPoint3(0.0f, 0.0f,
            kCenter.z - fRadius +
            MFramework::Instance->Input->BackgroundHeight));
    }

    NiBound kBackBound;
    MUtility::GetWorldBounds(pkBackground, kBackBound, true);
    float fBackRadius = kBackBound.GetRadius();
    float fFinalRadius = 100.0f * fRadius;
    fFinalRadius /= fBackRadius;
    
    pkBackground->SetScale(fFinalRadius);
    pkBackground->Update(0.0f);
    NiMesh::CompleteSceneModifiers(pkBackground);
    
#if defined (EE_PHYSX_BUILD)
    MFramework::Instance->Physics->AdjustGroundPlane();
#endif

    pkSharedData->Unlock();
    OnCommandExecuted(this);
    return true;
}
//---------------------------------------------------------------------------
void MUIResetBackgroundHeightCommand::DeleteContents()
{
}
//---------------------------------------------------------------------------
void MUIResetBackgroundHeightCommand::RefreshData()
{
    DeleteContents();
}
//---------------------------------------------------------------------------
bool MUIResetBackgroundHeightCommand::GetEnabled()
{
    return true;
}
//---------------------------------------------------------------------------
MUICommand::CommandType MUIResetBackgroundHeightCommand::GetCommandType()
{
    return MUICommand::RESET_BACKGROUND_HEIGHT;
}
//---------------------------------------------------------------------------
