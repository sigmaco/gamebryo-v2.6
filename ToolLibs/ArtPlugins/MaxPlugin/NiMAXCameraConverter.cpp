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

#include "MaxImmerse.h"
#include "NiMAXCameraConverter.h"

bool NiMAXCameraConverter::m_bConvertCameras = true;

NiTPointerList<NiMAXCameraConverter::CameraInfo *> 
NiMAXCameraConverter::m_camList;

void *NiMAXCameraConverter::m_pUserData = NULL;

//---------------------------------------------------------------------------
NiMAXCameraConverter::CameraInfo::CameraInfo() : spCam(NULL), spCamNode(NULL)
{
    bCameraFixed = false;
    bIsCurrent = false;
}

//---------------------------------------------------------------------------
NiMAXCameraConverter::CameraInfo::~CameraInfo()
{
    spCam = NULL;
    spCamNode = NULL;
}
//---------------------------------------------------------------------------
NiMAXCameraConverter::NiMAXCameraConverter(TimeValue animStart,
    TimeValue animEnd)
{
    m_animStart = animStart;
    m_animEnd = animEnd;
}
//---------------------------------------------------------------------------
// ConvertDefaultCamera just fits a camera to the
// scene graph rooted at "pRoot". Note that "pRoot"
// must be completely filled out to get a good camera.
// If elements are added after the default camera is 
// created they can be left off screen.
int NiMAXCameraConverter::ConvertDefaultCamera(Interface* pIntf, 
    NiNode* pRoot, CameraInfo* pResult, float fAnimStart, NiPoint3& offset)
{    
    CHECK_MEMORY();
    NiNode* pCamNode;
    pResult->spCamNode = pCamNode = NiNew NiNode;
    if (pCamNode == NULL)
        return(W3D_STAT_NO_MEMORY);
    
    NiCamera* pNIcam;
    pResult->spCam = pNIcam = NiNew NiCamera;
    if (pNIcam == NULL)
    {
        NiDelete pCamNode;
        return(W3D_STAT_NO_MEMORY);
    }
    
    NiBound sph;
    sph = pRoot->GetWorldBound();
    
    pNIcam->SetRotate(-NI_PI/2.0f, 0.0f, 1.0f, 0.0f);
    
    pNIcam->SetLODAdjust(1.0f);
    
    ViewExp* pView = pIntf->GetActiveViewport();
    float fInvAspectRatio = 1.0f / pIntf->GetRendImageAspect();
    NiFrustum f;
    if (pView)
    {
        Matrix3 WorldToView, ViewToWorld;
        float fFOV, fTanFOV;
        bool bIsPersp;        

        // The affine TM transforms from world coords to view coords
        // so we need the inverse of this matrix
        pView->GetAffineTM(WorldToView);
        ViewToWorld = Inverse(WorldToView);
        // The Z axis of this matrix is the view direction.
        Point3 viewUp = ViewToWorld.GetRow(0);
        Point3 viewRight = ViewToWorld.GetRow(1);
        Point3 viewDir = ViewToWorld.GetRow(2);
        Point3 viewPt = ViewToWorld.GetRow(3); 
        fFOV = pView->GetFOV();
        bIsPersp = pView->IsPerspView() ? true : false;

        pIntf->ReleaseViewport(pView);

        if (bIsPersp)
        {
            fTanFOV = (float) tan(fFOV * 0.5f);
            f.m_fLeft = -fTanFOV;          
            f.m_fRight = fTanFOV;          
            f.m_fTop = fInvAspectRatio * fTanFOV; 
            f.m_fBottom = -fInvAspectRatio * fTanFOV;
            f.m_fNear = 1.0f;
            //here we set the far plane to the distance from viewPt to
            //sph.GetCenter() plus 2*sph.GetRadius().  This insures all
            //geometry is drawn and the user can translate away some
            //without immediately clipping the geometry
            NiPoint3 kViewPoint = NiPoint3(viewPt.x, viewPt.y, viewPt.z);
            NiPoint3 kTemp = kViewPoint - sph.GetCenter();
            f.m_fFar = kTemp.Length() + 6 * sph.GetRadius();
            if (f.m_fFar < 5000.0f)
                f.m_fFar = 5000.0f;
            pCamNode->SetTranslate(viewPt.x + offset.x,
                viewPt.y + offset.y, viewPt.z + offset.z);
        }
        else
        {
            // NiCamera does not support orthographic projections
            // Build a standard frustum.
            f.m_fLeft = -0.5f;
            f.m_fRight = 0.5f;
            f.m_fTop = fInvAspectRatio * 0.5f;
            f.m_fBottom = -fInvAspectRatio * 0.5f;
            f.m_fNear = 1.0f;
            //since the location of the camera will be approximately 4X the
            //radius away along the -viewVector, we set the far plane to 6X
            //so all geometry is shown
            f.m_fFar = 6* sph.GetRadius();

            if (f.m_fFar < 5000.0f)
                f.m_fFar = 5000.0f;
            
            // MAX won't tell us the view point for orthographic
            // views, so we make up something reasonable using the
            // world bounding sphere.
            pCamNode->SetTranslate(
                sph.GetCenter().x + 4.0f * sph.GetRadius() * viewDir.x,
                sph.GetCenter().y + 4.0f * sph.GetRadius() * viewDir.y,
                sph.GetCenter().z + 4.0f * sph.GetRadius() * viewDir.z);
        }
        NiMatrix3 rot;
        rot.SetRow(0,viewUp.x, viewRight.x, viewDir.x);
        rot.SetRow(1,viewUp.y, viewRight.y, viewDir.y);
        rot.SetRow(2,viewUp.z, viewRight.z, viewDir.z);

        pCamNode->SetRotate(rot);
    }
    else
    {   
        // not sure when this case will occur; this assert will
        // alert me when/if it does.
        NIASSERT(0);

        f.m_fLeft = -0.5f;
        f.m_fRight = 0.5f;
        f.m_fTop = fInvAspectRatio * 0.5f;
        f.m_fBottom = -fInvAspectRatio * 0.5f;
        
        if (sph.GetRadius() < 0.0001f)
        {
            f.m_fNear = 0.1f;
            f.m_fFar = 11.0f;
        }
        else
        {
            f.m_fNear = 0.1f * sph.GetRadius();
            f.m_fFar = 11.0f * sph.GetRadius();
        }
        
        pCamNode->SetTranslate(sph.GetCenter().x,
            sph.GetCenter().y,
            sph.GetCenter().z + 2.0f * sph.GetRadius());
    }
        
    pNIcam->SetViewFrustum(f);
    
    pNIcam->SetViewPort(NiRect<float>(0.0f, 1.0f, 1.0f, 0.0f));
    
    pResult->bCameraFixed = false;
    pResult->bIsCurrent = true;    

    pCamNode->AttachChild(pNIcam);
    pCamNode->Update(fAnimStart);
    CHECK_MEMORY();

    return(W3D_STAT_OK);
}

//---------------------------------------------------------------------------
int NiMAXCameraConverter::Convert(Interface* pIntf, INode* pNode, 
    Object* pObj, NiNode* pCurNode, TimeValue animStart)
{
    CHECK_MEMORY();
    if (!m_bConvertCameras)
        return(W3D_STAT_OK);

    CameraObject* pCam = (CameraObject *) pObj;
  
    Interval itvl;
    CameraState cs;
    pCam->UpdateTargDistance(0, pNode);
    pCam->EvalCameraState(animStart, itvl, &cs);

    float fFOV = cs.fov;
    BOOL bIsOrtho = cs.isOrtho;

    // create a camera
    NiCamera* pNIcam = NiNew NiCamera;
    if (pNIcam == NULL)
        return(W3D_STAT_NO_MEMORY);

    NiNode* pCamNode = NiNew NiNode;
    if (pCamNode == NULL)
    {
        NiDelete pNIcam;
        return(W3D_STAT_NO_MEMORY);
    }

    pNIcam->SetRotate(-NI_PI/2.0f, 0.0f, 1.0f, 0.0f);

    bool bCameraFixed;
    if (pCam->GetManualClip())
        bCameraFixed = true;
    else
        bCameraFixed = false;

    // set camera attributes
    pNIcam->SetLODAdjust(1.0f);


    NiFrustum f;
    float fTanFOV = (float) tan(fFOV * 0.5f);
    float fInvAspectRatio = 1.0f / pIntf->GetRendImageAspect();

    float fTargetScalar = 1.0f;
    if (bIsOrtho)
    {
        fTargetScalar = pCam->GetTDist(0);
        f.m_bOrtho = true;
    }
    
    f.m_fLeft = -fTanFOV * fTargetScalar;          
    f.m_fRight = fTanFOV * fTargetScalar;          
    f.m_fTop = fInvAspectRatio * fTanFOV * fTargetScalar; 
    f.m_fBottom = -fInvAspectRatio * fTanFOV * fTargetScalar;
        
    if (bCameraFixed)
    {
        f.m_fNear = cs.hither;
        f.m_fFar = cs.yon;
    }
    else
    {
        f.m_fNear = 1.0f;
        f.m_fFar = 5000.0f;
    }

    
    pNIcam->SetViewFrustum(f);

    pNIcam->SetViewPort(NiRect<float>(0.0f, 1.0f, 1.0f, 0.0f));

    pCamNode->AttachChild(pNIcam);
    pCurNode->AttachChild(pCamNode);

    // Set the camera name (so that it is not null).
    const char* pcParentName = pCurNode->GetName();
    if (pcParentName)
    {
        const size_t stBufSize = 256;
        char acCameraName[stBufSize];
        NiSprintf(acCameraName, stBufSize, "%s Camera", pcParentName);
        pCamNode->SetName(acCameraName);
        NiSprintf(acCameraName, stBufSize, "%s NiCamera", pcParentName);
        pNIcam->SetName(acCameraName);
    }

    // check if this camera is used in the current view

    bool bIsCurrent = false;
    ViewExp* pView = pIntf->GetActiveViewport();
    if (pView)
    {
        pView->GetViewCamera();

        if (pView->GetViewCamera() == pNode)
            bIsCurrent = true;

        pIntf->ReleaseViewport(pView);
    }

    // create the CameraInfo
    NiMAXCameraConverter::CameraInfo *pResult;
    
    pResult = NiNew CameraInfo;

    if (pResult)
    {
        pResult->spCam = pNIcam;
        pResult->spCamNode = pCamNode;
        pResult->bCameraFixed = bCameraFixed;
        pResult->bIsCurrent = bIsCurrent;

        m_camList.AddHead(pResult);            
    }
    CHECK_MEMORY();

    return(W3D_STAT_OK);
}
//---------------------------------------------------------------------------
void NiMAXCameraConverter::Init()
{
    CHECK_MEMORY();
    NIASSERT(m_bConvertCameras);
    NIASSERT(m_camList.IsEmpty());
}
//---------------------------------------------------------------------------
void NiMAXCameraConverter::Shutdown()
{
    CHECK_MEMORY();
    m_bConvertCameras = true;

    NiTListIterator pCam = m_camList.GetHeadPos();
    while (pCam)
    {
        NiDelete m_camList.RemovePos(pCam);
    }

    m_camList.RemoveAll();
    CHECK_MEMORY();
}
//---------------------------------------------------------------------------

