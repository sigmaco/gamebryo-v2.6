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
#include "MUIZoomExtentsCommand.h"
#include "MFramework.h"
#include "MSharedData.h"
using namespace NiManagedToolInterface;

//---------------------------------------------------------------------------
MUIZoomExtentsCommand::MUIZoomExtentsCommand()
{
    m_pkOrbitPt = NULL;
}
//---------------------------------------------------------------------------
String* MUIZoomExtentsCommand::GetName()
{
    return "Zoom Extents";
}
//---------------------------------------------------------------------------
void MUIZoomExtentsCommand::SetOrbitPoint(MOrbitPoint* pkOrbitPt)
{
    m_pkOrbitPt = pkOrbitPt;
}
//---------------------------------------------------------------------------
bool MUIZoomExtentsCommand::Execute(MUIState*)
{
    MCamera* pkMCamera = MFramework::Instance->Renderer->ActiveCamera;
    NIASSERT(pkMCamera);

    NiCamera* pkCamera = pkMCamera->GetCamera();
    if (!pkCamera)
        return false;

    MSharedData* pkSharedData = MSharedData::Instance;
    pkSharedData->Lock();

    NiAVObject* pkScene = pkSharedData->GetScene(MSharedData::CHARACTER_INDEX);
    bool bZoomExtents = true;
    
    if (!pkScene)
    {
        pkScene = pkSharedData->GetScene(MSharedData::BACKGROUND_INDEX);
        bZoomExtents = false;
    }

    if (!pkScene)
    {
        pkSharedData->Unlock();
        return false;
    }

    if (m_pkOrbitPt)
    {
        m_pkOrbitPt->ResetOrbitPoint();
    }

    pkMCamera->CalcCameraFrustum(pkScene);
    if (bZoomExtents)
        pkMCamera->ZoomExtents();
    else
        pkMCamera->CenterToScene(pkScene);
    
    pkSharedData->Unlock();
    OnCommandExecuted(this);
    return true;
}
//---------------------------------------------------------------------------
void MUIZoomExtentsCommand::DeleteContents()
{
}
//---------------------------------------------------------------------------
void MUIZoomExtentsCommand::RefreshData()
{
    DeleteContents();
}
//---------------------------------------------------------------------------
bool MUIZoomExtentsCommand::GetEnabled()
{
    return m_pkOrbitPt != NULL;
}
//---------------------------------------------------------------------------
MUICommand::CommandType MUIZoomExtentsCommand::GetCommandType()
{
    return MUICommand::ZOOM_EXTENTS;
}
//---------------------------------------------------------------------------
