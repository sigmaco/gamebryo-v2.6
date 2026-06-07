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
#include "MayaPluginPCH.h"


//---------------------------------------------------------------------------
MyiCamera::MyiCamera()
{
    m_spCamera = NiNew NiCamera;
    if (m_spCamera == NULL)
        return;

    m_spCamera->SetName("(MyImmerse)Default Camera");

    NiPoint3 kTrans = NiPoint3(-30.0f, 0.0f, 0.0f);

    // Check for the current working units
    if (gExport.m_bUseCurrentWorkingUnits)
    {
        kTrans.x *= gExport.m_fLinearUnitMultiplier;
        kTrans.y *= gExport.m_fLinearUnitMultiplier;
        kTrans.z *= gExport.m_fLinearUnitMultiplier;
    }

    m_spCamera->SetTranslate( kTrans );

    m_iCameraID = -1;
    m_spCamera->Update(0.0f);
    NiMesh::CompleteSceneModifiers(m_spCamera);


    // Set the view frustum for the default camera
    float fNear = 0.1f ;
    float fFar = 1000.0f;

    // Check for the current working units
    if (gExport.m_bUseCurrentWorkingUnits)
    {
        fNear *= gExport.m_fLinearUnitMultiplier;
        fFar *= gExport.m_fLinearUnitMultiplier;
    }

    float fAspect = 1.5f;
    float fFOVY = 54.432f;
    float fHalfFOVYRadians;
    NiFrustum kFrustum;

    float fDeg2Rad = .01745329252f;
    fHalfFOVYRadians = fDeg2Rad * 0.5f * fFOVY;

    // This assumes the default view plane location in Gamebryo is (2, 0, 0)
    // and that the view plane is x = 2.

    kFrustum.m_fNear = fNear;
    kFrustum.m_fFar = fFar;
    kFrustum.m_fTop = (float)(tan(fHalfFOVYRadians));
    kFrustum.m_fBottom = (float)(-kFrustum.m_fTop);
    kFrustum.m_fRight = fAspect * (float)tan(fHalfFOVYRadians);
    kFrustum.m_fLeft = -kFrustum.m_fRight;

    // NiCamera::SetViewFrustum clamps the near value. For the time being,
    // it is more convenient to have the far value clamped, so do it before
    // calling NiCamera::SetViewFrustum.

    float fClampFar;
    fClampFar = kFrustum.m_fNear * 1e4f;
    if (fClampFar < kFrustum.m_fFar)
        kFrustum.m_fFar = fClampFar;

    m_spCamera->SetViewFrustum(kFrustum);
}
//---------------------------------------------------------------------------
MyiCamera::MyiCamera(int iCameraID)
{
    // Check for the Screen Space Camera
    MObject MTransform;
    DtExt_CameraGetTransform(iCameraID, MTransform);

    m_spCamera = NiNew NiCamera;

    if (m_spCamera == NULL)
        return;

    // GET THE CAMERAS NAME
    char *pcCameraName;
    DtCameraGetName( iCameraID, &pcCameraName );

    m_spCamera->SetName(pcCameraName);

    m_iCameraID = iCameraID;
    SetTransformation();
    SetProjection();
    m_spCamera->Update(0.0f);
    NiMesh::CompleteSceneModifiers(m_spCamera);
}
//---------------------------------------------------------------------------
MyiCamera::MyiCamera(NiCameraPtr pNiCamera)
{
    // For construction of a Ni Camera (default) that doesn't
    // have an associated maya camera.

    m_spCamera = pNiCamera;

    if (m_spCamera == NULL)
        return;


    m_iCameraID = -1;
    m_spCamera->Update(0.0f);
    NiMesh::CompleteSceneModifiers(m_spCamera);
}
//---------------------------------------------------------------------------
MyiCamera::~MyiCamera()
{
    m_spCamera = 0;
}
//---------------------------------------------------------------------------
void MyiCamera::SetProjection()
{
    float fNear, fFar, fAspect, fFOVY;
    float fHalfFOVYRadians;
    NiFrustum kFrustum;

    DtCameraGetNearClip( m_iCameraID, &fNear );
    DtCameraGetFarClip( m_iCameraID, &fFar );
    DtCameraGetAspect( m_iCameraID, &fAspect );
    DtCameraGetHeightAngle( m_iCameraID, &fFOVY );

    float fOrthoWidth;

    kFrustum.m_bOrtho = DtCameraGetHeight(m_iCameraID, &fOrthoWidth) == 1;


    if (kFrustum.m_bOrtho)
    {
        fOrthoWidth = (fOrthoWidth / 2.0f) * gExport.m_fLinearUnitMultiplier;

        kFrustum.m_fNear = fNear * gExport.m_fLinearUnitMultiplier;
        kFrustum.m_fFar = fFar * gExport.m_fLinearUnitMultiplier;
        kFrustum.m_fTop = fOrthoWidth;
        kFrustum.m_fBottom = -fOrthoWidth;
        kFrustum.m_fRight = fOrthoWidth;
        kFrustum.m_fLeft = -fOrthoWidth;
    }
    else
    {
        float fDeg2Rad = .01745329252f;
        fHalfFOVYRadians = fDeg2Rad * 0.5f * fFOVY;
        
        // This assumes the default view plane location in Gamebryo is 
        // (1, 0, 0)and that the view plane is x = 1.
        
        kFrustum.m_fNear = fNear * gExport.m_fLinearUnitMultiplier;
        kFrustum.m_fFar = fFar * gExport.m_fLinearUnitMultiplier;
        if (gExport.m_bUseOldCameraFOV)
        {
            kFrustum.m_fTop = (float)(tan(fHalfFOVYRadians));
            kFrustum.m_fBottom = (float)(-kFrustum.m_fTop);
            kFrustum.m_fRight = (float)tan(fHalfFOVYRadians) * fAspect;
            kFrustum.m_fLeft = -kFrustum.m_fRight;
        }
        else
        {
            kFrustum.m_fRight = (float)tan(fHalfFOVYRadians);
            kFrustum.m_fLeft = -kFrustum.m_fRight;
            kFrustum.m_fTop = (float)(tan(fHalfFOVYRadians)) / fAspect;
            kFrustum.m_fBottom = (float)(-kFrustum.m_fTop);
        }
    }

    m_spCamera->SetViewFrustum(kFrustum);
}
//---------------------------------------------------------------------------
void MyiCamera::SetTransformation()
{
    m_spCamera->SetTranslate(NiPoint3::ZERO);

    // Apply a special roll for the camera to convert from Maya to Gamebryo
    // Cameras
    NiMatrix3 ToNi;
    ToNi.MakeYRotation(-NI_HALF_PI);
    m_spCamera->SetRotate(ToNi);
}
//---------------------------------------------------------------------------
