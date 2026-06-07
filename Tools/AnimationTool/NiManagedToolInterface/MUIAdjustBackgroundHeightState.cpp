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
#include "MUIAdjustBackgroundHeightState.h"
#include "MSharedData.h"
#include "MFramework.h"
#include "MUIManager.h"

using namespace NiManagedToolInterface;
//---------------------------------------------------------------------------
MUIAdjustBackgroundHeightState::MUIAdjustBackgroundHeightState()
{
    m_bProcessMouseMove = false;
}
//---------------------------------------------------------------------------
MUIState::UIType MUIAdjustBackgroundHeightState::GetUIType()
{
    return MUIState::BACKGROUNDHEIGHT;
}
//---------------------------------------------------------------------------
String* MUIAdjustBackgroundHeightState::GetName()
{
    return "Adjust Background Height";
}
//---------------------------------------------------------------------------
void MUIAdjustBackgroundHeightState::MouseEnter()
{
    MUIState::MouseEnter();
    
}
//---------------------------------------------------------------------------
void MUIAdjustBackgroundHeightState::MouseLeave()
{
    MUIState::MouseLeave();
    
}
//---------------------------------------------------------------------------
void MUIAdjustBackgroundHeightState::MouseHover()
{
    MUIState::MouseHover();
    
}
//---------------------------------------------------------------------------
void MUIAdjustBackgroundHeightState::DoubleClick()
{
    MUIState::DoubleClick();
    
}
//---------------------------------------------------------------------------
void MUIAdjustBackgroundHeightState::MouseMove(int x, int y)
{
    MUIState::MouseMove(x,y);
    if (m_bProcessMouseMove)
    {
        POINT point;
        point.x = x;
        point.y = y;

        GetPositionInScreenSpace(point);

        MRenderer* pkRenderer = MFramework::Instance->Renderer;
        MCamera* pkCamera = pkRenderer->ActiveCamera;
        int iVertRes = pkRenderer->Height;

        MSharedData* pkSharedData = MSharedData::Instance;
        pkSharedData->Lock();
        NiActorManager* pkActorManager = pkSharedData->GetActorManager();
        if (!pkActorManager)
        {
            pkSharedData->Unlock();
            return;
        }

        NiAVObject* pkModelRoot = NULL;
        pkModelRoot = pkActorManager->GetNIFRoot();
            
        if (!pkModelRoot)
        {
            pkSharedData->Unlock();
            return;
        }

        NiBound kBound;
        MUtility::GetWorldBounds(pkModelRoot, kBound, false);
        float fSceneRadius = kBound.GetRadius();
        NiPoint3 kSceneCenter = kBound.GetCenter();

        // Get vert translation.
        float fVert = (float) (m_ptClickPoint.y - point.y);
        float fVertTrans = (fVert / iVertRes) * 5.0f * fSceneRadius;

        NiAVObject* pkBackground =
            pkSharedData->GetScene(MSharedData::BACKGROUND_INDEX);

        if (!pkBackground)
        {
            pkSharedData->Unlock();
            return;
        }

        MFramework::Instance->Input->BackgroundHeight += fVertTrans;
        
        if (pkCamera->UpAxis == MCamera::Y_AXIS)
        {
            pkBackground->SetTranslate(NiPoint3(0.0f,
                kSceneCenter.y - fSceneRadius +
                MFramework::Instance->Input->BackgroundHeight, 0.0f));
        }
        else if (pkCamera->UpAxis == MCamera::Z_AXIS)
        {
            pkBackground->SetTranslate(NiPoint3(0.0f, 0.0f,
                kSceneCenter.z - fSceneRadius +
                MFramework::Instance->Input->BackgroundHeight));
        }
        pkBackground->Update(0.0f);
        NiMesh::CompleteSceneModifiers(pkBackground);

#if defined (EE_PHYSX_BUILD)
        MFramework::Instance->Physics->AdjustGroundPlane();
#endif

        pkSharedData->Unlock();
        m_ptClickPoint.x = point.x;   
        m_ptClickPoint.y = point.y;   
    }
}
//---------------------------------------------------------------------------
void MUIAdjustBackgroundHeightState::MouseUp(MouseButtonType eType, int x, 
    int y)
{
    MUIState::MouseUp(eType, x, y);
    if (eType == LMB)
    {
        m_bProcessMouseMove = false;
        ReleaseCapture();
        EndPause();
        ShowCursor(TRUE);
    }
}
//---------------------------------------------------------------------------
void MUIAdjustBackgroundHeightState::MouseDown(MouseButtonType eType, int x, 
    int y)
{
    MUIState::MouseDown(eType, x, y);

    if (eType == LMB)
    {
        POINT point;
        point.x = x;
        point.y = y;

        BeginPause();
        ShowCursor(FALSE);
        GetPositionInScreenSpace(point);
        m_ptClickPoint.x = point.x;
        m_ptClickPoint.y = point.y;
        m_bProcessMouseMove = true;
        MRenderer* pkRenderer = MFramework::Instance->Renderer;
        SetCapture(pkRenderer->GetRenderWindowHandle()); 
    }
    
}
//---------------------------------------------------------------------------
void MUIAdjustBackgroundHeightState::MouseWheel(int iDelta)
{
    WheelZoom(iDelta);
}
//---------------------------------------------------------------------------
void MUIAdjustBackgroundHeightState::Update()
{
    MUIState::Update();
}
//---------------------------------------------------------------------------
void MUIAdjustBackgroundHeightState::RefreshData()
{
    MUIState::RefreshData();
}
//---------------------------------------------------------------------------
void MUIAdjustBackgroundHeightState::DeleteContents()
{
    MUIState::DeleteContents();
    
}
//---------------------------------------------------------------------------
void MUIAdjustBackgroundHeightState::Activate(bool)
{
    
}
//---------------------------------------------------------------------------
void MUIAdjustBackgroundHeightState::Deactivate()
{
   
}
//---------------------------------------------------------------------------
