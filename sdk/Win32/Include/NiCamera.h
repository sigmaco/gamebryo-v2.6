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

#ifndef NICAMERA_H
#define NICAMERA_H

#include "NiAVObject.h"
#include "NiFrustum.h"
#include "NiRect.h"

#ifdef _PS3
#include <vectormath/cpp/vectormath_aos.h>
using namespace Vectormath::Aos;
#endif

class NiPoint2;
class NiRenderTargetGroup;

class NIMAIN_ENTRY NiCamera : public NiAVObject
{
    NiDeclareRTTI;
    NiDeclareClone(NiCamera);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    // Construction and destruction.
    NiCamera();
    virtual ~NiCamera();

    // The default camera model coordinate system is
    //   origin    = (0,0,0)
    //   direction = (1,0,0)
    //   up        = (0,1,0)
    //   right     = (0,0,1)

    // Camera world coordinates.  The last three functions return copies
    // because the vectors are extracted from the columns of the world
    // rotation matrix, each column not of the type NiPoint3.
    inline const NiPoint3& GetWorldLocation() const;
    inline NiPoint3 GetWorldDirection() const;
    inline NiPoint3 GetWorldUpVector() const;
    inline NiPoint3 GetWorldRightVector() const;

    // The 4x4 matrix that maps world coordinates to camera coordinates.  The
    // matrix is stored in row-major order.  The matrix is used in the member
    // function WorldPtToScreenPt.  The renderers do not use this.
    inline const float* GetWorldToCameraMatrix() const;

    // The view frustum and supporting functions.
    void SetViewFrustum(const NiFrustum& kFrustum);
    inline const NiFrustum& GetViewFrustum() const;
    inline void SetMinNearPlaneDist(float fMin);
    inline float GetMinNearPlaneDist() const;
    inline void SetMaxFarNearRatio(float fMax);
    inline float GetMaxFarNearRatio() const;
    void AdjustAspectRatio(float fTargetAspectRatio, 
        bool bAdjustVerticalDims = false);

    // The viewport, which is contained in the unit square [0,1]^2.
    inline void SetViewPort(const NiRect<float>& kPort);
    inline const NiRect<float>& GetViewPort() const;

    // Adjust the near and far planes of the view frustum so that the input
    // bound is between the two planes.
    bool FitNearAndFarToBound(const NiBound& kBound);

    // Rotate the camera so that its world direction points to the input
    // point.  Once a camera faces that direction, there are infinitely many
    // rotations about that direction, each one a valid "look at" orientation.
    // The input up vector is used to select the unique orientation for the
    // given direction and up vectors.
    bool LookAtWorldPoint(const NiPoint3& kWorldPt, const NiPoint3& kWorldUp);

    // The input point (lWx,lWy) is in screen coordinates, measured in pixels.
    // The input NiRenderTargetGroup indicates which render target group to 
    // whose primary surface the screen coordinates (lWx,lWy) refer.
    // The output is the ray, in world coordinates, whose origin is the
    // camera location and whose direction is from the camera origin to the
    // world point corresponding to (lWx,lWy).
    bool WindowPointToRay(long lWx, long lWy, NiPoint3& kOrigin,
        NiPoint3& kDir, const NiRenderTargetGroup* pkTarget = NULL) const;

    // The input point (fVx,fVy) is in frustum coordinates, with
    // frustum.L <= fVx <= frustum.R and frustum.B <= fVy <= frustum.T.  The
    // output is the ray, in world coordinates, whose origin is the camera
    // location and whose direction is from the camera origin to the world
    // point corresponding to (fVx,fVy).
    void ViewPointToRay(float fVx, float fVy, NiPoint3& kOrigin,
        NiPoint3& kDir) const;

    // Compute the axis-aligned bounding box, in normalized projection
    // coordinates (near = 0, far = 1, left = bottom = -1, right = top = 1),
    // of the input world bounding sphere.
    void ScreenSpaceBoundBound(const NiBound& kSBound, NiPoint3& kMinBound,
        NiPoint3& kMaxBound, float fZeroTolerance = 1e-5f) const;

    // Compute the screen-space axis-aligned bounding box of the projection
    // of the input world bounding sphere.
    void ScreenSpaceBoundSize(const NiBound& kSBound, NiPoint2& kBoundSize,
        float fZeroTolerance = 1e-5f) const;

    // Compute the projection of the input world point to a viewport point
    // (fBx,fBy), with port.L <= fBx <= port.R and port.B <= fBy <= port.T.
    bool WorldPtToScreenPt(const NiPoint3& kPt, float& fBx, float& fBy,
        float fZeroTolerance = 1e-5f) const;

    // Compute the projection of the input world point to a viewport point
    // (fBx,fBy,fBz), with port.L <= fBx <= port.R and port.B <= fBy <= port.T.
    // Same as WorldPtToScreen but Z is also transformed.
    bool WorldPtToScreenPt3(const NiPoint3& kPt, float& fBx, float& fBy,
        float& fBz, float fZeroTolerance = 1e-5f) const;

    // A LOD adjustment for NiLODNode objects.
    inline void SetLODAdjust(float fLODAdjust);
    inline float GetLODAdjust() const;

#ifdef _PS3
    static void ConstructOrthoMatrix(Matrix4& kResult, 
        float fLeft, float fRight, 
        float fBottom, float fTop, 
        float fNear,float fFar);

    static void ConstructProjectionMatrix(Matrix4& kResult, 
        float fLeft, float fRight, 
        float fBottom, float fTop, 
        float fNear,float fFar);
#endif

protected:
    // Support for geometric updates.
    virtual void UpdateWorldData();
    virtual void UpdateWorldBound();

    // The world-to-camera transformation matrix M, stored so that an
    // application to a vector V is accomplished by M*V.
    float m_aafWorldToCam[4][4];
    void WorldToCameraMatrix();

    // The view frustum and associated parameters.
    NiFrustum m_kViewFrustum;
    float m_fMinNearPlaneDist;
    float m_fMaxFarNearRatio;

    // The viewport, which is contained in the unit square [0,1]^2.
    NiRect<float> m_kPort;

    // A LOD adjustment for NiLODNode.
    float m_fLODAdjust;
};

typedef NiPointer<NiCamera> NiCameraPtr;
#include "NiCamera.inl"

#endif
