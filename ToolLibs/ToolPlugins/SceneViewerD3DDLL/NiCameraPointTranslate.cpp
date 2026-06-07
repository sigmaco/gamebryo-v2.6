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
#include "NiCameraPointTranslate.h"
#include "NiSceneMouseCommandInfo.h"
#include "NiSceneMouse.h"

NiCommandID NiCameraPointTranslateCommand::ms_kID = 0;
                  
//---------------------------------------------------------------------------
NiCameraPointTranslateCommand::NiCameraPointTranslateCommand(NiPoint2 kPoint)
{
    m_kWindowPt = kPoint;
}
//---------------------------------------------------------------------------
bool NiCameraPointTranslateCommand::Apply(NiSceneViewer* pkViewer)
{

    unsigned int uiCameraID = pkViewer->GetCurrentCameraID();
    NiCamera* pkCamera = pkViewer->GetCamera(uiCameraID, true);
 
    NiViewerCamera* pkViewerCam = pkViewer->GetCurrentCameraOrientation();

    long lWx = (long)m_kWindowPt.x;
    long lWy = (long)m_kWindowPt.y; 
    NiPoint3 kOrigin; 
    NiPoint3 kDir;

    NiPoint3 kTranslateVector;
    NiNode* pkNode = pkViewer->GetScene();
    if (pkCamera->WindowPointToRay(lWx,lWy, kOrigin, kDir))
    {
        NiPoint3 kCoords = pkNode->GetWorldBound().GetCenter();
        NiPoint3 kDistance = pkCamera->GetWorldTranslate() + kCoords;
        float fDist = kDistance.x*kDistance.x + kDistance.y*kDistance.y +
            kDistance.z*kDistance.z;
        fDist = (float) sqrt(fDist);

        kTranslateVector = kOrigin - pkCamera->GetWorldTranslate();
        //kTranslateVector = kTranslateVector * fDist;
        //NiPoint3 kWorldNormal = pkCamera->GetWorldDirection()*-1.0f;
        /*NiPoint3 kCoords = pkNode->GetWorldBound().GetCenter();
        float fConstant = kWorldNormal.x * kCoords.x + 
            kWorldNormal.y*kCoords.y + kWorldNormal.z*kCoords.z;
        float fScale = 
            (fConstant - kOrigin.Dot(kWorldNormal))/(kDir*kWorldNormal);
        NiPoint3 kIntersection = kOrigin + kDir*fScale;
        kTranslateVector = (kIntersection - pkCamera->GetWorldTranslate());*/
        
       /* // now compute the projection of the translate vector onto the camera
        NiPoint3 kCamRightVector = pkCamera->GetWorldRightVector();
        kTranslateVector = ((kTranslateVector.Dot(kCamRightVector)) / 
            (kCamRightVector.Dot(kCamRightVector)))*(kCamRightVector);*/
    }

    pkViewerCam->IncrementTranslate(kTranslateVector);
    pkViewerCam->UpdateCamera();

    return true;
}
//---------------------------------------------------------------------------
NiCommandID NiCameraPointTranslateCommand::GetClassID()
{
     return ms_kID;
}
//---------------------------------------------------------------------------
NiCameraPointTranslateCommand::~NiCameraPointTranslateCommand()
{

}
//---------------------------------------------------------------------------
NiSceneCommand* NiCameraPointTranslateCommand::Create(
    NiSceneCommandInfo* pkInfo)
{
    if (pkInfo == NULL)
    {
        return NiNew NiCameraPointTranslateCommand(NiPoint2(0,0));
    }
    if (pkInfo->m_kParamList.GetSize() != 2)
    {
        NiOutputDebugString("Invalid Number of Arguments for "
            "CameraPtTranslate\n");
        return NULL;
    }

    NiPoint2 kTranslateVector(pkInfo->m_kParamList.GetAt(0),
        pkInfo->m_kParamList.GetAt(1));

    return NiNew NiCameraPointTranslateCommand(kTranslateVector);
}
//---------------------------------------------------------------------------
unsigned int NiCameraPointTranslateCommand::GetParameterCount()
{
    return 2;
}
//---------------------------------------------------------------------------
char* NiCameraPointTranslateCommand::GetParameterName(
    unsigned int uiWhichParam) 
{
    char* pcString = NULL;
    switch(uiWhichParam)
    {
        case 0:
            pcString = NiAlloc(char, 256);
            NiStrcpy(pcString, 256, "X");
            break;
        case 1:
            pcString = NiAlloc(char, 256);
            NiStrcpy(pcString, 256, "Y");
            break;
        default:
            break;
    }
    return pcString;
}
//---------------------------------------------------------------------------
NiTPrimitiveArray<NiSceneCommandInfo*>* 
    NiCameraPointTranslateCommand::GetDefaultCommandParamaterizations()
{
    return NULL;
}
//---------------------------------------------------------------------------
void NiCameraPointTranslateCommand::Register()
{
    ms_kID = NiSceneCommandRegistry::RegisterCommand("CAMERA_MOUSE_TRANSLATE",
        NiCameraPointTranslateCommand::Create);
}
//---------------------------------------------------------------------------
