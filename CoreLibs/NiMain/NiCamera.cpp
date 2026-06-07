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

#include "NiMainPCH.h"
#include "NiCamera.h"
#include "NiAccumulator.h"
#include "NiBool.h"
#include "NiPoint2.h"
#include "NiNode.h"
#include "NiRenderer.h"

NiImplementRTTI(NiCamera,NiAVObject);

//---------------------------------------------------------------------------
NiCamera::NiCamera()
{
    // view frustum
    m_kViewFrustum.m_fNear = 1.0f;
    m_kViewFrustum.m_fFar = 2.0f;
    m_kViewFrustum.m_fTop = 0.5f;
    m_kViewFrustum.m_fBottom = -0.5f;
    m_kViewFrustum.m_fLeft = -0.5f;
    m_kViewFrustum.m_fRight = 0.5f;
    m_kViewFrustum.m_bOrtho = false;
    m_fMinNearPlaneDist = 0.1f;
    m_fMaxFarNearRatio = 1e4f;

    // view port:  NIASSERT(bottom < top && left < right)
    m_kPort.m_top = 1.0f;
    m_kPort.m_bottom = 0.0f;
    m_kPort.m_left = 0.0f;
    m_kPort.m_right = 1.0f;

    m_fLODAdjust = 1.0f;

    UpdateWorldData();   
    UpdateWorldBound();  
}
//---------------------------------------------------------------------------
NiCamera::~NiCamera()
{
}
//---------------------------------------------------------------------------
void NiCamera::SetViewFrustum(const NiFrustum& kFrustum)
{
    m_kViewFrustum.m_fNear = kFrustum.m_fNear;

    float fMinNear = kFrustum.m_fFar / m_fMaxFarNearRatio;
    if (m_kViewFrustum.m_fNear < fMinNear)
        m_kViewFrustum.m_fNear = fMinNear;

    if (m_kViewFrustum.m_fNear < m_fMinNearPlaneDist)
        m_kViewFrustum.m_fNear = m_fMinNearPlaneDist;

    m_kViewFrustum.m_fLeft   = kFrustum.m_fLeft;
    m_kViewFrustum.m_fRight  = kFrustum.m_fRight;
    m_kViewFrustum.m_fTop    = kFrustum.m_fTop;
    m_kViewFrustum.m_fBottom = kFrustum.m_fBottom;
    m_kViewFrustum.m_fFar    = kFrustum.m_fFar;
    m_kViewFrustum.m_bOrtho  = kFrustum.m_bOrtho;
}
//---------------------------------------------------------------------------
void NiCamera::AdjustAspectRatio(float fTargetAspectRatio, 
    bool bAdjustVerticalDims)
{
    if (bAdjustVerticalDims)
    {
        // The horizontal portion of the frustum is fixed. We'll only adjust
        // the top and bottom.
        float fHoriz = m_kViewFrustum.m_fRight - m_kViewFrustum.m_fLeft;

        // Calculate half-span of vertical dimensions.
        // Aspect = Width / Height so the span of the height is Width / Aspect
        float fSpan = fHoriz / fTargetAspectRatio * 0.5f;

        // Calculate the current center of the vertical dimensions.
        float fCenter = (m_kViewFrustum.m_fTop + m_kViewFrustum.m_fBottom) 
            * 0.5f;

        // Set the top and bottom based off the new values.
        m_kViewFrustum.m_fTop = fCenter + fSpan;
        m_kViewFrustum.m_fBottom = fCenter - fSpan;
    }
    else
    {
        // The vertical portion of the frustum is fixed. We'll only adjust
        // the right and left.
        float fVert = m_kViewFrustum.m_fTop - m_kViewFrustum.m_fBottom;

        // Calculate half-span of horizontal dimensions.
        // Aspect = Width / Height so the span of the width is Aspect * Height
        float fSpan = fTargetAspectRatio * fVert * 0.5f;

        // Calculate the current center of the horizontal dimensions.
        float fCenter = (m_kViewFrustum.m_fLeft + m_kViewFrustum.m_fRight) 
            * 0.5f;

        // Set the left and right based off the new values.
        m_kViewFrustum.m_fRight = fCenter + fSpan;
        m_kViewFrustum.m_fLeft = fCenter - fSpan;
    }
}
//---------------------------------------------------------------------------
bool NiCamera::FitNearAndFarToBound(const NiBound& kBound)
{
    const NiPoint3& kCenter = kBound.GetCenter();
    
    // these are signed distances
    float fDistToCenter = (kCenter - m_kWorld.m_Translate) *
        GetWorldDirection();

    float fMinDist = fDistToCenter - kBound.GetRadius();
    float fMaxDist = fDistToCenter + kBound.GetRadius();

    if (fMaxDist <= 0.0f)
        return false;

    float fMinMinDist = fMaxDist / m_fMaxFarNearRatio;
    if (fMinDist < fMinMinDist)
        fMinDist = fMinMinDist;

    if (fMinDist < m_fMinNearPlaneDist)
        fMinDist = m_fMinNearPlaneDist;

    m_kViewFrustum.m_fNear = fMinDist;
    m_kViewFrustum.m_fFar = fMaxDist;

    UpdateWorldData();
    UpdateWorldBound();

    return true;
}
//---------------------------------------------------------------------------
bool NiCamera::LookAtWorldPoint(const NiPoint3& kWorldPt, 
    const NiPoint3& kWorldUp)
{
    NiPoint3 kDir = GetWorldLocation() - kWorldPt;
    if (kDir.SqrLength() < 1e-10f)
        return false;

    kDir.Unitize();

    NiPoint3 kRight = kWorldUp.UnitCross(kDir);
    
    // will either be unit length or zero
    if (kRight.SqrLength() < 0.5f)
        return false;

    NiPoint3 kUp = kDir.UnitCross(kRight);

    // will either be unit length or zero
    if (kUp.SqrLength() < 0.5f)
        return false;

    // Directly create the rotation matrix using column vectors
    NiMatrix3 kRot = NiMatrix3(-kDir, kUp, kRight);
    NiNode* pkParent = GetParent();
    if (pkParent)
        kRot = pkParent->GetWorldRotate().TransposeTimes(kRot);

    SetRotate(kRot);
    UpdateWorldData();
    UpdateWorldBound();

    return true;
}
//---------------------------------------------------------------------------
bool NiCamera::WindowPointToRay(long lWx, long lWy, NiPoint3& kOrigin,
    NiPoint3& kDir, const NiRenderTargetGroup* pkTarget) const
{
    NiRenderer* pkRenderer = NiRenderer::GetRenderer();
    NIASSERT(pkRenderer);
    if (!pkRenderer)
        return false;

    // Window points (wx,wy) satisfy 0 <= wx < wxmax and 0 <= wy < wymax,
    // where wxmax is the screen width and wymax is the screen height, both
    // measured in pixels.  Buffer points (bx,by) are in [0,1]^2 with
    //   bx = wx/wxmax
    //   by = 1 - wy/wymax
    // Notice that the y values are reflected to change handedness of the
    // coordinates.  Buffer point (bx,by) is the relative location of (wx,wy)
    // on the full screen.
    float fBx, fBy;
    bool bConverted = pkRenderer->MapWindowPointToBufferPoint(
        (unsigned int)lWx, (unsigned int)lWy, fBx, fBy, pkTarget);
    if (!bConverted || fBx > m_kPort.m_right || fBx < m_kPort.m_left ||
        fBy > m_kPort.m_top || fBy < m_kPort.m_bottom)
    {
        return false;
    }

    // Viewport coordinates (px,py) are in [0,1]^2 with
    //   px = (bx - port.L)/(port.R - port.L)
    //   py = (by - port.B)/(port.T - port.B)
    // Viewport point (px,py) is the relative location of (wx,wy) in the
    // viewport, a subrectangle of the screen.
    float fPx = (fBx - m_kPort.m_left) / (m_kPort.m_right - m_kPort.m_left);
    float fPy = (fBy - m_kPort.m_bottom) / (m_kPort.m_top - m_kPort.m_bottom);

    // View plane coordinates (vx,vy) satisfy frustum.L <= vx <= frustum.R
    // and frustum.B <= vy <= frustum.T.  The mapping is
    //   vx = frustum.L + px * (frustum.R - frustum.L)
    //   vy = frustum.B + py * (frustum.T - frustum.b)
    float fVx = m_kViewFrustum.m_fLeft + fPx * 
        (m_kViewFrustum.m_fRight - m_kViewFrustum.m_fLeft);
    float fVy = m_kViewFrustum.m_fBottom + fPy * 
        (m_kViewFrustum.m_fTop - m_kViewFrustum.m_fBottom);

    // Convert view plane coordinates to a ray in world coordinates.  The
    // output kOrigin is the camera world location and kDir is the world
    // direction from the camera world location to the point on the view
    // plane.
    ViewPointToRay(fVx, fVy, kOrigin, kDir);
    return true;
}
//---------------------------------------------------------------------------
void NiCamera::ViewPointToRay(float fVx, float fVy, NiPoint3& kOrigin,
    NiPoint3& kDir) const
{
    if (m_kViewFrustum.m_bOrtho)
    {
        kDir = GetWorldDirection();
        kDir.Unitize();
        kOrigin = GetWorldLocation() + GetWorldRightVector() * fVx + 
            GetWorldUpVector() * fVy;
    }
    else
    {
        kDir = GetWorldDirection() + GetWorldRightVector() * fVx + 
            GetWorldUpVector() * fVy;
        kDir.Unitize();
        kOrigin = GetWorldLocation();
    }
}
//---------------------------------------------------------------------------
void NiCamera::ScreenSpaceBoundBound(const NiBound& kSBound, 
    NiPoint3& kMinBound, NiPoint3& kMaxBound, float fZeroTolerance) const
{
    NiPoint3 kCenter = kSBound.GetCenter();
    float fRadius = kSBound.GetRadius();
    NiPoint3 kCSmLoc = kCenter - m_kWorld.m_Translate;
    
    // Z bounds
    float fInvFmN = 1.0f / (m_kViewFrustum.m_fFar - m_kViewFrustum.m_fNear);
    float fMinZ = kCSmLoc.Dot(GetWorldDirection()) - m_kViewFrustum.m_fNear;
    float fMaxZ = fMinZ + fRadius;
    fMinZ -= fRadius;
    
    if (m_kViewFrustum.m_bOrtho)
    {
        kMinBound.z = 0.0f;
        kMaxBound.z = 0.0f;
    }
    else
    {
        kMinBound.z = fMinZ * fInvFmN;
        kMaxBound.z = fMaxZ * fInvFmN;
    }

    // min X kBound
    NiPoint3 kSphereRight = fRadius*kCSmLoc.UnitCross(GetWorldUpVector());
    NiPoint3 kTmp = kCenter - kSphereRight;
    float fW = kTmp.x * m_aafWorldToCam[3][0] +
        kTmp.y * m_aafWorldToCam[3][1] + kTmp.z * m_aafWorldToCam[3][2] +
        m_aafWorldToCam[3][3];
    if (fW > fZeroTolerance)
    {
        kMinBound.x = (kTmp.x * m_aafWorldToCam[0][0] +
            kTmp.y * m_aafWorldToCam[0][1] + kTmp.z * m_aafWorldToCam[0][2] +
            m_aafWorldToCam[0][3]) / fW;
    }
    else
    {
        kMinBound.x = kMinBound.y = -1.0f; 
        kMaxBound.x = kMaxBound.y = 1.0f; 
        
        if (kMinBound.z < 0.0f)
        {
            kMinBound.z = 0.0f;
            if (kMaxBound.z < 0.0f)
                kMaxBound.z = 0.0f;
            else if (kMaxBound.z > 1.0f)
                kMaxBound.z = 1.0f;
        }
        else if (kMaxBound.z > 1.0f)
        {
            kMaxBound.z = 1.0f;
            if (kMinBound.z > 1.0f)
                kMinBound.z = 1.0f;
        }
        return;
    }
    
    // max X kBound
    kTmp = kCenter + kSphereRight;
    fW = kTmp.x * m_aafWorldToCam[3][0] +
        kTmp.y * m_aafWorldToCam[3][1] + kTmp.z * m_aafWorldToCam[3][2] +
        m_aafWorldToCam[3][3];
    if (fW > fZeroTolerance)
    {
        kMaxBound.x = (kTmp.x * m_aafWorldToCam[0][0] +
            kTmp.y * m_aafWorldToCam[0][1] + kTmp.z * m_aafWorldToCam[0][2] +
            m_aafWorldToCam[0][3]) / fW;
    }
    else
    {
        kMinBound.x = kMinBound.y = -1.0f; 
        kMaxBound.x = kMaxBound.y = 1.0f; 
        
        if (kMinBound.z < 0.0f)
        {
            kMinBound.z = 0.0f;
            if (kMaxBound.z < 0.0f)
                kMaxBound.z = 0.0f;
            else if (kMaxBound.z > 1.0f)
                kMaxBound.z = 1.0f;
        }
        else if (kMaxBound.z > 1.0f)
        {
            kMaxBound.z = 1.0f;
            if (kMinBound.z > 1.0f)
                kMinBound.z = 1.0f;
        }
        return;
    }
    
    // min Y kBound
    NiPoint3 sphereUp = fRadius * GetWorldRightVector().UnitCross(kCSmLoc);
    kTmp = kCenter - sphereUp;
    fW = kTmp.x * m_aafWorldToCam[3][0] +
        kTmp.y * m_aafWorldToCam[3][1] + kTmp.z * m_aafWorldToCam[3][2] +
        m_aafWorldToCam[3][3];
    if (fW > fZeroTolerance)
    {
        kMinBound.y = (kTmp.x * m_aafWorldToCam[1][0] +
            kTmp.y * m_aafWorldToCam[1][1] + kTmp.z * m_aafWorldToCam[1][2] +
            m_aafWorldToCam[1][3]) / fW;
    }
    else
    {
        kMinBound.x = kMinBound.y = -1.0f; 
        kMaxBound.x = kMaxBound.y = 1.0f; 
        
        if (kMinBound.z < 0.0f)
        {
            kMinBound.z = 0.0f;
            if (kMaxBound.z < 0.0f)
                kMaxBound.z = 0.0f;
            else if (kMaxBound.z > 1.0f)
                kMaxBound.z = 1.0f;
        }
        else if (kMaxBound.z > 1.0f)
        {
            kMaxBound.z = 1.0f;
            if (kMinBound.z > 1.0f)
                kMinBound.z = 1.0f;
        }
        return;
    }
    
    // max Y kBound
    kTmp = kCenter + sphereUp;
    fW = kTmp.x * m_aafWorldToCam[3][0] +
        kTmp.y * m_aafWorldToCam[3][1] + kTmp.z * m_aafWorldToCam[3][2] +
        m_aafWorldToCam[3][3];
    if (fW > fZeroTolerance)
    {
        kMaxBound.y = (kTmp.x * m_aafWorldToCam[1][0] +
            kTmp.y * m_aafWorldToCam[1][1] + kTmp.z * m_aafWorldToCam[1][2] +
            m_aafWorldToCam[1][3] ) / fW;
    }
    else
    {
        kMinBound.x = kMinBound.y = -1.0f; 
        kMaxBound.x = kMaxBound.y = 1.0f; 
        
        if (kMinBound.z < 0.0f)
        {
            kMinBound.z = 0.0f;
            if (kMaxBound.z < 0.0f)
                kMaxBound.z = 0.0f;
            else if (kMaxBound.z > 1.0f)
                kMaxBound.z = 1.0f;
        }
        else if (kMaxBound.z > 1.0f)
        {
            kMaxBound.z = 1.0f;
            if (kMinBound.z > 1.0f)
                kMinBound.z = 1.0f;
        }
        return;
    }
    
    if (kMinBound.x < -1.0f)
    {
        kMinBound.x = -1.0f;
        if (kMaxBound.x < -1.0f)
            kMaxBound.x = -1.0f;
        else if (kMaxBound.x > 1.0f)
            kMaxBound.x = 1.0f;
    }
    else if (kMaxBound.x > 1.0f)
    {
        kMaxBound.x = 1.0f;
        if (kMinBound.x > 1.0f)
            kMinBound.x = 1.0f;
    }
    
    if (kMinBound.y < -1.0f)
    {
        kMinBound.y = -1.0f;
        if (kMaxBound.y < -1.0f)
            kMaxBound.y = -1.0f;
        else if (kMaxBound.y > 1.0f)
            kMaxBound.y = 1.0f;
    }
    else if (kMaxBound.y > 1.0f)
    {
        kMaxBound.y = 1.0f;
        if (kMinBound.y > 1.0f)
            kMinBound.y = 1.0f;
    }
    
    if (kMinBound.z < 0.0f)
    {
        kMinBound.z = 0.0f;
        if (kMaxBound.z < 0.0f)
            kMaxBound.z = 0.0f;
        else if (kMaxBound.z > 1.0f)
            kMaxBound.z = 1.0f;
    }
    else if (kMaxBound.z > 1.0f)
    {
        kMaxBound.z = 1.0f;
        if (kMinBound.z > 1.0f)
            kMinBound.z = 1.0f;
    }
}
//---------------------------------------------------------------------------
void NiCamera::ScreenSpaceBoundSize(const NiBound& kSBound, 
    NiPoint2& kBoundSize, float fZeroTolerance) const
{
    // Unlike the 3d version of this function, this one does not clip
    // the results.
    float fBoundViewDist = (kSBound.GetCenter() - m_kWorld.m_Translate) * 
        GetWorldDirection();

    if (fBoundViewDist < fZeroTolerance)
    {
        if (fBoundViewDist > -fZeroTolerance)
        {
            kBoundSize.x = FLT_MAX;
            kBoundSize.y = FLT_MAX;
            return;
        }
        fBoundViewDist = -fBoundViewDist;
    }
    
    float fRatio;

    if (m_kViewFrustum.m_bOrtho)
    {
        fRatio = kSBound.GetRadius();
    }
    else
    {
        fRatio = kSBound.GetRadius() / fBoundViewDist;
    }

    kBoundSize.x = fRatio * 2.0f /
        (m_kViewFrustum.m_fRight - m_kViewFrustum.m_fLeft);

    kBoundSize.y = fRatio * 2.0f /
        (m_kViewFrustum.m_fTop - m_kViewFrustum.m_fBottom);
}
//---------------------------------------------------------------------------
bool NiCamera::WorldPtToScreenPt(const NiPoint3& kPt, float &fBx, float &fBy,
    float fZeroTolerance) const
{
    // Same as next function, except Z is not transformed
    
    // Make sure we have a valid near plane test value. If not, we could
    // generate a divide by zero.

    fZeroTolerance = NiMax(fZeroTolerance, 0.0f);
           
    // project a world space point to screen space
    float fW = kPt.x * m_aafWorldToCam[3][0] +
        kPt.y * m_aafWorldToCam[3][1] + kPt.z * m_aafWorldToCam[3][2] +
        m_aafWorldToCam[3][3];

    // Check to see if we're on the appropriate side of the camera.
    if (fW > fZeroTolerance)
    {
        float fInvW = 1.0f / fW;

        fBx = kPt.x * m_aafWorldToCam[0][0] + kPt.y * m_aafWorldToCam[0][1] +
              kPt.z * m_aafWorldToCam[0][2] + m_aafWorldToCam[0][3];
        fBy = kPt.x * m_aafWorldToCam[1][0] + kPt.y * m_aafWorldToCam[1][1] +
              kPt.z * m_aafWorldToCam[1][2] + m_aafWorldToCam[1][3];
    
        fBx = fBx * fInvW;
        fBy = fBy * fInvW;

        fBx *= (m_kPort.m_right - m_kPort.m_left) * 0.5f;
        fBy *= (m_kPort.m_top - m_kPort.m_bottom) * 0.5f;

        fBx += (m_kPort.m_right + m_kPort.m_left) * 0.5f;
        fBy += (m_kPort.m_top + m_kPort.m_bottom) * 0.5f;

        // If on screen return true. Otherwise, we fall through to false.
        if (fBx >= m_kPort.m_left && fBx <= m_kPort.m_right && 
            fBy >= m_kPort.m_bottom && fBy <= m_kPort.m_top)
        {
            return true;
        }
    }
    else
    {
        fBx = FLT_MAX;
        fBy = FLT_MAX;
    }
    return false;
}
//---------------------------------------------------------------------------
bool NiCamera::WorldPtToScreenPt3(const NiPoint3& kPt, float &fBx, float &fBy,
    float &fBz, float fZeroTolerance) const
{
    // Same as previous function, except Z is also transformed

    // Make sure we have a valid near plane test value. If not, we could
    // generate a divide by zero.
    fZeroTolerance = NiMax(fZeroTolerance, 0.0f);
           
    // project a world space point to screen space
    float fW = kPt.x * m_aafWorldToCam[3][0] +
        kPt.y * m_aafWorldToCam[3][1] + kPt.z * m_aafWorldToCam[3][2] +
        m_aafWorldToCam[3][3];

    // Check to see if we're on the appropriate side of the camera.
    if (fW > fZeroTolerance)
    {
        float fInvW = 1.0f / fW;

        fBx = kPt.x * m_aafWorldToCam[0][0] + kPt.y * m_aafWorldToCam[0][1] +
              kPt.z * m_aafWorldToCam[0][2] + m_aafWorldToCam[0][3];
        fBy = kPt.x * m_aafWorldToCam[1][0] + kPt.y * m_aafWorldToCam[1][1] +
              kPt.z * m_aafWorldToCam[1][2] + m_aafWorldToCam[1][3];
    
        fBx = fBx * fInvW;
        fBy = fBy * fInvW;

        fBx *= (m_kPort.m_right - m_kPort.m_left) * 0.5f;
        fBy *= (m_kPort.m_top - m_kPort.m_bottom) * 0.5f;

        fBx += (m_kPort.m_right + m_kPort.m_left) * 0.5f;
        fBy += (m_kPort.m_top + m_kPort.m_bottom) * 0.5f;

        // Transform Z
        fBz = kPt.x * m_aafWorldToCam[2][0] + kPt.y * 
            m_aafWorldToCam[2][1] + kPt.z * m_aafWorldToCam[2][2] + 
            m_aafWorldToCam[2][3];

        fBz = fBz * fInvW;

        // If on screen, then return true.
        if (fBx >= m_kPort.m_left && fBx <= m_kPort.m_right && 
            fBy >= m_kPort.m_bottom && fBy <= m_kPort.m_top)
        {
            return true;
        }
    }
    else
    {
        fBx = FLT_MAX;
        fBy = FLT_MAX;
        fBz = FLT_MAX;
    }
    return false;
}
//---------------------------------------------------------------------------
void NiCamera::UpdateWorldData()
{
    NiAVObject::UpdateWorldData();
    WorldToCameraMatrix();
}
//---------------------------------------------------------------------------
void NiCamera::UpdateWorldBound()
{
    m_kWorldBound.SetCenter(m_kWorld.m_Translate);
}
//---------------------------------------------------------------------------
#ifdef _PS3
void NiCamera::ConstructOrthoMatrix(Matrix4& kResult, 
    float fLeft, float fRight, 
    float fBottom, float fTop, 
    float fNear,float fFar)
{
    kResult = Matrix4::orthographic(
        fLeft,
        fRight,
        fBottom,
        fTop,
        fNear,
        fFar);

    // Correct the Z-Scale & Z-Offset to represent DX style space.
    ((float*)&kResult)[10] = 1 /(fNear-fFar);
    ((float*)&kResult)[14] = fNear /(fNear-fFar);
}
//---------------------------------------------------------------------------
void NiCamera::ConstructProjectionMatrix(Matrix4& kResult, 
    float fLeft, float fRight, 
    float fBottom, float fTop, 
    float fNear,float fFar)
{
    // The multiplication by m_fCameraNear is required due to the
    // definition of Matrix4::frustum
    kResult = Matrix4::frustum(
        fLeft * fNear,
        fRight * fNear,
        fBottom * fNear,    // bottom
        fTop * fNear, // top
        fNear, fFar);

}
#endif
//---------------------------------------------------------------------------
void NiCamera::WorldToCameraMatrix()
{

#ifndef _PS3
    // Matrix to transform world coordinates to camera coordinates.  Let L
    // be the camera world location, and let {D,U,R} be the camera world
    // direction, up, and right vectors, respectively.  Let Z be the zero
    // vector.  For the sake of notation below, all vectors are treated as
    // row vectors.  The matrix is
    //   +-             -+
    //   | R   -Dot(R,L) |
    //   | U   -Dot(U,L) |
    //   | D   -Dot(D,L) |
    //   | Z       1     |
    //   +-             -+

    // Get the camera coordinate system.
    NiPoint3 kLoc = GetWorldLocation();
    NiPoint3 kDVector = GetWorldDirection();
    NiPoint3 kUVector = GetWorldUpVector();
    NiPoint3 kRVector = GetWorldRightVector();

    float fV03 = -(kRVector.Dot(kLoc));
    float fV13 = -(kUVector.Dot(kLoc));
    float fV23 = -(kDVector.Dot(kLoc));
    
    // various temporary quantities
    float fRpL = m_kViewFrustum.m_fRight + m_kViewFrustum.m_fLeft;
    float fTpB = m_kViewFrustum.m_fTop + m_kViewFrustum.m_fBottom;
    float fRmL = 1.0f / (m_kViewFrustum.m_fRight - m_kViewFrustum.m_fLeft);
    float fTmB = 1.0f / (m_kViewFrustum.m_fTop - m_kViewFrustum.m_fBottom);
    float fFmN = 1.0f / (m_kViewFrustum.m_fFar - m_kViewFrustum.m_fNear);

    if (m_kViewFrustum.m_bOrtho)
    {
        // Parallel projection, view-to-screen projection matrix (includes
        // shearing):
        // fP01 = fP02 = fP10 = fP12 = fP20 = fP21 = fP30 = fP31 = fP32 = 0
        // fP33 = 1
        float fP00 = 2.0f * fRmL;
        float fP03 = fRpL * fRmL;
        float fP11 = 2.0f * fTmB;
        float fP13 = fTpB * fTmB;
        float fP22 = fFmN; 
        float fP23 = -m_kViewFrustum.m_fNear * fP22; 

        // world-to-camera matrix = proj_matrix * view_matrix
        m_aafWorldToCam[0][0] = fP00 * kRVector.x;
        m_aafWorldToCam[0][1] = fP00 * kRVector.y;
        m_aafWorldToCam[0][2] = fP00 * kRVector.z;
        m_aafWorldToCam[0][3] = fP00 * fV03 + fP03;
        m_aafWorldToCam[1][0] = fP11 * kUVector.x;
        m_aafWorldToCam[1][1] = fP11 * kUVector.y;
        m_aafWorldToCam[1][2] = fP11 * kUVector.z;
        m_aafWorldToCam[1][3] = fP11 * fV13 + fP13;
        m_aafWorldToCam[2][0] = fP22 * kDVector.x;
        m_aafWorldToCam[2][1] = fP22 * kDVector.y;
        m_aafWorldToCam[2][2] = fP22 * kDVector.z;
        m_aafWorldToCam[2][3] = fP22 * fV23 + fP23;
        m_aafWorldToCam[3][0] = 0.0f;
        m_aafWorldToCam[3][1] = 0.0f;
        m_aafWorldToCam[3][2] = 0.0f;
        m_aafWorldToCam[3][3] = 1.0f;

    }
    else
    {

        // Perspective projection, view-to-screen projection matrix (includes
        // shearing):
        // fP01 = fP03 = fP10 = fP13 = fP20 = fP21 = fP30 = fP31 = 0
        // fP32 = 1
        float fP00 = 2.0f * fRmL;
        float fP02 = -fRpL * fRmL;
        float fP11 = 2.0f * fTmB;
        float fP12 = -fTpB * fTmB;
        float fP22 = m_kViewFrustum.m_fFar * fFmN;
        float fP23 = -m_kViewFrustum.m_fNear * fP22;

        // world-to-camera matrix = proj_matrix * view_matrix
        m_aafWorldToCam[0][0] = fP00 * kRVector.x + fP02 * kDVector.x;
        m_aafWorldToCam[0][1] = fP00 * kRVector.y + fP02 * kDVector.y;
        m_aafWorldToCam[0][2] = fP00 * kRVector.z + fP02 * kDVector.z;
        m_aafWorldToCam[0][3] = fP00 * fV03 + fP02 * fV23;

        m_aafWorldToCam[1][0] = fP11 * kUVector.x + fP12 * kDVector.x;
        m_aafWorldToCam[1][1] = fP11 * kUVector.y + fP12 * kDVector.y;
        m_aafWorldToCam[1][2] = fP11 * kUVector.z + fP12 * kDVector.z;
        m_aafWorldToCam[1][3] = fP11 * fV13 + fP12 * fV23;

        m_aafWorldToCam[2][0] = fP22 * kDVector.x;
        m_aafWorldToCam[2][1] = fP22 * kDVector.y;
        m_aafWorldToCam[2][2] = fP22 * kDVector.z;
        m_aafWorldToCam[2][3] = fP22 * fV23 + fP23;

        m_aafWorldToCam[3][0] = kDVector.x;
        m_aafWorldToCam[3][1] = kDVector.y;
        m_aafWorldToCam[3][2] = kDVector.z;
        m_aafWorldToCam[3][3] = fV23;
    }
#else
    Vectormath::Aos::Matrix4 kProj, kView, kViewProj;

    // Get the camera coordinate system.
    NiPoint3 kLoc = GetWorldLocation();
    NiPoint3 kDVector = GetWorldDirection();
    NiPoint3 kUVector = GetWorldUpVector();

    Vectormath::Aos::Point3 kPos = 
        Vectormath::Aos::Point3(kLoc.x, kLoc.y, kLoc.z);
    Vectormath::Aos::Point3 kLookAt = Vectormath::Aos::Point3(
        kLoc.x + kDVector.x, 
        kLoc.y + kDVector.y, 
        kLoc.z + kDVector.z);
    Vectormath::Aos::Vector3 kUp = 
        Vectormath::Aos::Vector3(kUVector.x, kUVector.y, kUVector.z);

    kView = Matrix4::lookAt(kPos, kLookAt, kUp);

    if (m_kViewFrustum.m_bOrtho)
    {
        ConstructOrthoMatrix(kProj,
                m_kViewFrustum.m_fLeft,
                m_kViewFrustum.m_fRight,
                m_kViewFrustum.m_fBottom,
                m_kViewFrustum.m_fTop,
                m_kViewFrustum.m_fNear,
                m_kViewFrustum.m_fFar);
    }
    else
    {
        ConstructProjectionMatrix(kProj,
            m_kViewFrustum.m_fLeft,
            m_kViewFrustum.m_fRight,
            m_kViewFrustum.m_fBottom,    // bottom
            m_kViewFrustum.m_fTop, // top
            m_kViewFrustum.m_fNear, m_kViewFrustum.m_fFar);
    }

    kViewProj= transpose(kProj * kView);

    NiMemcpy(m_aafWorldToCam, &kViewProj, sizeof(float) * 16);
#endif
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// cloning
//---------------------------------------------------------------------------
NiImplementCreateClone(NiCamera);
//---------------------------------------------------------------------------
void NiCamera::CopyMembers(NiCamera* pkDest,
    NiCloningProcess& kCloning)
{
    NiAVObject::CopyMembers(pkDest, kCloning);

    unsigned int uiDestSize = 16 * sizeof(float); // Dimensions 4 * 4
    NiMemcpy(pkDest->m_aafWorldToCam, m_aafWorldToCam, uiDestSize);
    pkDest->m_kWorld.m_Translate = m_kWorld.m_Translate;
    pkDest->m_kViewFrustum = m_kViewFrustum;
    pkDest->m_kPort = m_kPort;
    pkDest->m_fLODAdjust = m_fLODAdjust;
    pkDest->m_fMinNearPlaneDist = m_fMinNearPlaneDist;
    pkDest->m_fMaxFarNearRatio = m_fMaxFarNearRatio;
}
//---------------------------------------------------------------------------
#ifndef __SPU__
//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiCamera);
//---------------------------------------------------------------------------
void NiCamera::LoadBinary(NiStream& kStream)
{
    NiAVObject::LoadBinary(kStream);

    unsigned short usObsolete;  // m_uFlags obsolete
    NiStreamLoadBinary(kStream, usObsolete);

    // view frustum
    NiStreamLoadBinary(kStream, m_kViewFrustum.m_fLeft);
    NiStreamLoadBinary(kStream, m_kViewFrustum.m_fRight);
    NiStreamLoadBinary(kStream, m_kViewFrustum.m_fTop);
    NiStreamLoadBinary(kStream, m_kViewFrustum.m_fBottom);
    NiStreamLoadBinary(kStream, m_kViewFrustum.m_fNear);
    NiStreamLoadBinary(kStream, m_kViewFrustum.m_fFar);

    NiBool kValue;
    NiStreamLoadBinary(kStream, kValue);
    m_kViewFrustum.m_bOrtho = (kValue != 0);

    // view port
    NiStreamLoadBinary(kStream, m_kPort.m_left);
    NiStreamLoadBinary(kStream, m_kPort.m_right);
    NiStreamLoadBinary(kStream, m_kPort.m_top);
    NiStreamLoadBinary(kStream, m_kPort.m_bottom);

    // LOD adjustment for NiLODNode
    NiStreamLoadBinary(kStream, m_fLODAdjust);

    kStream.ReadLinkID();   // m_spScene

    // screen-space polygons
    kStream.ReadMultipleLinkIDs();  // m_kScreenPolygons

    // screen-space textures
    kStream.ReadMultipleLinkIDs();  // m_kScreenTextures
}
//---------------------------------------------------------------------------
void NiCamera::LinkObject(NiStream& kStream)
{
    NiAVObject::LinkObject(kStream);

    // The camera used to have an NiNodePtr m_spScene.  To avoid a NIF version
    // change we are writing out NULL.  At link time, a NIF with the
    // new NiCamera object should lead to spScene being NULL.  For an old
    // NIF, spScene is not NULL and is discarded.
    NiNodePtr spScene = (NiNode*) kStream.GetObjectFromLinkID();

    // The camera used to have an array of NiScreenPolygon. To avoid a NIF 
    // version change we are writing out an array quantity of zero and no
    // pointers.  At link time, an old NIF with screen polygons will have
    // these discarded.
    unsigned int i;
    unsigned int uiSize = kStream.GetNumberOfLinkIDs();
    for (i = 0; i < uiSize; i++)
    {
        // An old NIF returns an NiScreenPolygon.
        NiObjectPtr spScreenPolygon = kStream.GetObjectFromLinkID();
    }

    // The camera used to have an array of NiScreenTexture.  To avoid a NIF 
    // version change we are writing out an array quantity of zero and no
    // pointers.  At link time, an old NIF with screen textures will have
    // these discarded.
    uiSize = kStream.GetNumberOfLinkIDs();
    for (i = 0; i < uiSize; i++)
    {
        // This code should never get hit for NIF versions higher than
        // 10.1.0.0. uiSize should always be 0.
        NiObjectPtr spScreenTexture = kStream.GetObjectFromLinkID();
    }
}
//---------------------------------------------------------------------------
bool NiCamera::RegisterStreamables(NiStream& kStream)
{
    return NiAVObject::RegisterStreamables(kStream);
}
//---------------------------------------------------------------------------
void NiCamera::SaveBinary(NiStream& kStream)
{
    NiAVObject::SaveBinary(kStream);

    // Write out a dummy flag to preserve the NIF format.  
    unsigned short usObsolete = 0;
    NiStreamSaveBinary(kStream, usObsolete);

    // view frustum
    NiStreamSaveBinary(kStream, m_kViewFrustum.m_fLeft);
    NiStreamSaveBinary(kStream, m_kViewFrustum.m_fRight);
    NiStreamSaveBinary(kStream, m_kViewFrustum.m_fTop);
    NiStreamSaveBinary(kStream, m_kViewFrustum.m_fBottom);
    NiStreamSaveBinary(kStream, m_kViewFrustum.m_fNear);
    NiStreamSaveBinary(kStream, m_kViewFrustum.m_fFar);
    NiBool bOrtho = m_kViewFrustum.m_bOrtho;
    NiStreamSaveBinary(kStream, bOrtho);

    // view port
    NiStreamSaveBinary(kStream, m_kPort.m_left);
    NiStreamSaveBinary(kStream, m_kPort.m_right);
    NiStreamSaveBinary(kStream, m_kPort.m_top);
    NiStreamSaveBinary(kStream, m_kPort.m_bottom);

    // LOD adjustment for NiLODNode
    NiStreamSaveBinary(kStream, m_fLODAdjust);

    // The camera used to have an NiNodePtr m_spScene.  For now, just
    // write out NULL.  
    kStream.SaveLinkID(0);

    // Write to disk that there are zero screen polygons and zero
    // screen textures.  
    unsigned int uiSize = 0;
    NiStreamSaveBinary(kStream, uiSize);  // number of screen polygons
    NiStreamSaveBinary(kStream, uiSize);  // number of screen textures
}
//---------------------------------------------------------------------------
bool NiCamera::IsEqual(NiObject* pkObject)
{
    if (!NiAVObject::IsEqual(pkObject))
        return false;

    NiCamera* pkCamera = (NiCamera*) pkObject;

    // view frustum
    if (m_kViewFrustum.m_fLeft != pkCamera->m_kViewFrustum.m_fLeft ||
        m_kViewFrustum.m_fRight != pkCamera->m_kViewFrustum.m_fRight ||
        m_kViewFrustum.m_fTop != pkCamera->m_kViewFrustum.m_fTop ||
        m_kViewFrustum.m_fBottom != pkCamera->m_kViewFrustum.m_fBottom ||
        m_kViewFrustum.m_fNear != pkCamera->m_kViewFrustum.m_fNear ||
        m_kViewFrustum.m_fFar != pkCamera->m_kViewFrustum.m_fFar)
    {
        return false;
    }

    // view port
    if (m_kPort.m_left != pkCamera->m_kPort.m_left ||
        m_kPort.m_right != pkCamera->m_kPort.m_right ||
        m_kPort.m_top != pkCamera->m_kPort.m_top ||
        m_kPort.m_bottom != pkCamera->m_kPort.m_bottom)
    {
        return false;
    }

    // We do not stream the near/far adjust parameters, so they cannot
    // be checked in IsEqual.

    // LOD adjust
    if (m_fLODAdjust != pkCamera->m_fLODAdjust)
    {
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
void NiCamera::GetViewerStrings(NiViewerStringsArray* pkStrings)
{
    NiAVObject::GetViewerStrings(pkStrings);

    pkStrings->Add(NiGetViewerString(NiCamera::ms_RTTI.GetName()));
    pkStrings->Add(m_kViewFrustum.GetViewerString("m_kViewFrustum"));
    pkStrings->Add(m_kPort.GetViewerString("m_kPort"));
    pkStrings->Add(GetWorldDirection().GetViewerString("m_kWorldDir"));
    pkStrings->Add(GetWorldUpVector().GetViewerString("m_kWorldUp"));
    pkStrings->Add(GetWorldRightVector().GetViewerString("m_kWorldRight"));
    pkStrings->Add(NiGetViewerString("m_fMinNearPlaneDist",
        m_fMinNearPlaneDist));
    pkStrings->Add(NiGetViewerString("m_fMaxFarNearRatio",
        m_fMaxFarNearRatio));
}
//---------------------------------------------------------------------------
#endif
