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
#include "NiPortalPCH.h"

#include "NiPortal.h"
#include <NiCamera.h>
#include <NiSystem.h>
#include <NiVersion.h>

//---------------------------------------------------------------------------
// The following copyright notice may not be removed.
static char EmergentCopyright[] NI_UNUSED = 
    "Copyright (c) 1996-2008 Emergent Game Technologies.";
//---------------------------------------------------------------------------
static char acGamebryoVersion[] NI_UNUSED = 
    GAMEBRYO_MODULE_VERSION_STRING(NiPortal);
//---------------------------------------------------------------------------

NiImplementRTTI(NiPortal, NiAVObject);

//---------------------------------------------------------------------------
NiPortal::NiPortal(unsigned short usVertices, const NiPoint3* pkVertex, 
    NiAVObject* pkAdjoiner, bool bActive)
{
    m_usVertices = usVertices;
    m_pkAdjoiner = pkAdjoiner;
    m_uFlags = 0;
    m_pkModelVertex = NiNew NiPoint3[m_usVertices];
    unsigned int uiByteSize = m_usVertices * sizeof(*m_pkModelVertex);
    NiMemcpy(m_pkModelVertex, pkVertex, uiByteSize);
    m_pkWorldVertex = NiNew NiPoint3[m_usVertices];
    m_kModelBound.ComputeFromData(usVertices, m_pkModelVertex);

    SetActive(bActive);
    SetMarked(false);
}
//---------------------------------------------------------------------------
NiPortal::NiPortal()
{
    m_usVertices = 0;
    m_pkModelVertex = 0;
    m_pkAdjoiner = 0;
    m_pkWorldVertex = 0;
    m_uFlags = 0;

    SetActive(false);
    SetMarked(false);
}
//---------------------------------------------------------------------------
NiPortal::~NiPortal()
{
    NiDelete[] m_pkModelVertex;
    NiDelete[] m_pkWorldVertex;
}
//---------------------------------------------------------------------------
bool NiPortal::ComputePortalFrustum(NiCamera& kCamera, NiFrustum& kFrustum)
{
    // The portal must be at least a triangle.
    int iQuantity = (int)m_usVertices;
    NIASSERT(iQuantity >= 3);
    if (iQuantity < 3)
        return false;

    // Compute the winding order.  Only outgoing portals that are ordered
    // counterclockwise relative to the observer should be entered.
    NiPoint3 kEyeToPoly = kCamera.GetWorldLocation() - m_pkWorldVertex[0];
    NiPoint3 kEdge10 = m_pkWorldVertex[1] - m_pkWorldVertex[0];
    NiPoint3 kEdge20 = m_pkWorldVertex[2] - m_pkWorldVertex[0];
    float fOrder = kEyeToPoly.Dot(kEdge10.UnitCross(kEdge20));
    if (fOrder < 0.0f)
        return false;

    // Get world-to-camera transformation to map portal geometry into
    // camera coordinates.
    NiMatrix3 kRotScale = kCamera.GetWorldRotate().Transpose() 
        * (1.0f / kCamera.GetWorldScale());
    NiPoint3 kTrans = kCamera.GetWorldTranslate();

    NiRect<float> kRect(NI_INFINITY, -NI_INFINITY, -NI_INFINITY, NI_INFINITY);
    NiPoint3 kPos;
    int i;

    // Get the current frustum.  Determine if it can be reduced in size to
    // fit the portal.
    kFrustum = kCamera.GetViewFrustum();

    // Compute the axis-aligned bounding rectangle of the portal vertices in
    // camera coordinates.  In the case of perspective projection, special
    // care is given to those points that are behind the observer.
    if (!kFrustum.m_bOrtho)
    {
        const float EPSILON = 1.0e-07f, INV_EPSILON = 1.0f/EPSILON;
        int iFirstSign = 0, iLastSign = 0;  // in {-1,0,1}
        bool bSignChange = false;
        NiPoint3 kFirstPos = NiPoint3(0,0,0);
        NiPoint3 kLastPos = NiPoint3(0,0,0);
        NiPoint3 kDiffPos;
        float fInvX, fZDivX, fYDivX, fT;

        // Project the portion of the portal polygon which is in front of
        // the observer onto the viewing plane.  A vertex is "in front" if
        // its camera-space coordinates satisfy x > 0.  To avoid problems
        // with floating-point division, we instead use x > epsilon, for some
        // small positive epsilon.  If all vertices are in front, the
        // projections are computed and their axis-aligned bounding box is
        // stored in kRect.
        //
        // If a vertex is behind the observer (x <= epsilon), the projection
        // onto the viewing plane does not exist.  When such vertices occur,
        // we detect the polygon edges that intersect the x = epsilon plane
        // and compute the projection onto the viewing plane of the
        // edge-plane intersection.  Locating such edges efficiently requires
        // keeping track of the the "signs" of the x-values for the vertices.
        // The "sign" is 0 initially, indicating an uninitialized sign.  The
        // value 1 is assigned when x > epsilon, and the value -1 is assigned
        // when x <= epsilon.
        for (i = 0; i < iQuantity; i++)
        {
            kPos = kRotScale * (m_pkWorldVertex[i] - kTrans);

            if (kPos.x > EPSILON)
            {
                if (iFirstSign == 0)
                {
                    iFirstSign = 1;
                    kFirstPos = kPos;
                }

                fInvX = 1.0f / kPos.x;
                fZDivX = kPos.z * fInvX;
                fYDivX = kPos.y * fInvX;

                if (fZDivX < kRect.m_left)
                    kRect.m_left = fZDivX;

                if (fZDivX > kRect.m_right)
                    kRect.m_right = fZDivX;

                if (fYDivX < kRect.m_bottom)
                    kRect.m_bottom = fYDivX;

                if (fYDivX > kRect.m_top)
                    kRect.m_top = fYDivX;

                if (iLastSign < 0)
                    bSignChange = true;

                iLastSign = 1;
            }
            else
            {
                if (iFirstSign == 0)
                {
                    iFirstSign = -1;
                    kFirstPos = kPos;
                }

                if (iLastSign > 0)
                    bSignChange = true;

                iLastSign = -1;
            }

            if (bSignChange)
            {
                kDiffPos = kPos - kLastPos;
                fT = (EPSILON - kLastPos.x) / kDiffPos.x;
                fZDivX = (kLastPos.z + fT * kDiffPos.z) * INV_EPSILON;
                fYDivX = (kLastPos.y + fT * kDiffPos.y) * INV_EPSILON;

                if (fZDivX < kRect.m_left)
                    kRect.m_left = fZDivX;

                if (fZDivX > kRect.m_right)
                    kRect.m_right = fZDivX;

                if (fYDivX < kRect.m_bottom)
                    kRect.m_bottom = fYDivX;

                if (fYDivX > kRect.m_top)
                    kRect.m_top = fYDivX;

                bSignChange = false;
            }

            kLastPos = kPos;
        }

        // process the last polygon edge
        if (iFirstSign * iLastSign < 0)
        {
            kDiffPos = kFirstPos - kLastPos;
            fT = (EPSILON - kLastPos.x) / kDiffPos.x;
            fZDivX = (kLastPos.z + fT * kDiffPos.z) * INV_EPSILON;
            fYDivX = (kLastPos.y + fT * kDiffPos.y) * INV_EPSILON;

            if (fZDivX < kRect.m_left)
                kRect.m_left = fZDivX;

            if (fZDivX > kRect.m_right)
                kRect.m_right = fZDivX;

            if (fYDivX < kRect.m_bottom)
                kRect.m_bottom = fYDivX;

            if (fYDivX > kRect.m_top)
                kRect.m_top = fYDivX;
        }
    }
    else
    {
        for (i = 0; i < iQuantity; i++)
        {
            kPos = kRotScale * (m_pkWorldVertex[i] - kTrans);

            if (kPos.z < kRect.m_left)
                kRect.m_left = kPos.z;

            if (kPos.z > kRect.m_right)
                kRect.m_right = kPos.z;

            if (kPos.y < kRect.m_bottom)
                kRect.m_bottom = kPos.y;

            if (kPos.y > kRect.m_top)
                kRect.m_top = kPos.y;
        }
    }

    // If the portal cannot appear on the screen, the caller must be informed
    // not to draw the adjoiner.
    if (kFrustum.m_fLeft >= kRect.m_right ||
        kFrustum.m_fRight <= kRect.m_left ||
        kFrustum.m_fBottom >= kRect.m_top ||
        kFrustum.m_fTop <= kRect.m_bottom)
    {
        return false;
    }

    // The portal bounding rectangle intersects the current frustum.  Reduce
    // the frustum (if possible) for drawing the adjoiner.
    if (kFrustum.m_fLeft < kRect.m_left)
        kFrustum.m_fLeft = kRect.m_left;

    if (kFrustum.m_fRight > kRect.m_right)
        kFrustum.m_fRight = kRect.m_right;

    if (kFrustum.m_fBottom < kRect.m_bottom)
        kFrustum.m_fBottom = kRect.m_bottom;

    if (kFrustum.m_fTop > kRect.m_top)
        kFrustum.m_fTop = kRect.m_top;

    return true;
}
//---------------------------------------------------------------------------
bool NiPortal::UpdateFrustum(NiCullingProcess& kCuller)
{
    // The portal must be at least a triangle.
    int iQuantity = (int)m_usVertices;
    NIASSERT(iQuantity >= 3);
    if (iQuantity < 3)
        return false;

    // Compute the winding order.  Only outgoing portals that are ordered
    // counterclockwise relative to the observer should be entered.
    const NiCamera* pkCamera = kCuller.GetCamera();
    NiPoint3 kEyeToPoly = pkCamera->GetWorldLocation() - m_pkWorldVertex[0];
    NiPoint3 kEdge10 = m_pkWorldVertex[1] - m_pkWorldVertex[0];
    NiPoint3 kEdge20 = m_pkWorldVertex[2] - m_pkWorldVertex[0];
    float fOrder = kEyeToPoly.Dot(kEdge10.UnitCross(kEdge20));
    if (fOrder < 0.0f)
        return false;

    // Get world-to-camera transformation to map portal geometry into
    // camera coordinates.
    NiMatrix3 kRotScale = pkCamera->GetWorldRotate().Transpose() 
        * (1.0f / pkCamera->GetWorldScale());
    NiPoint3 kTrans = pkCamera->GetWorldTranslate();

    NiRect<float> kRect(NI_INFINITY, -NI_INFINITY, -NI_INFINITY, NI_INFINITY);
    NiPoint3 kPos;
    int i;

    // Get the current frustum.  Determine if it can be reduced in size to
    // fit the portal.
    NiFrustum kFrustum = kCuller.GetFrustum();

    // Compute the axis-aligned bounding rectangle of the portal vertices in
    // camera coordinates.  In the case of perspective projection, special
    // care is given to those points that are behind the observer.
    if (!kFrustum.m_bOrtho)
    {
        const float EPSILON = 1.0e-07f, INV_EPSILON = 1.0f/EPSILON;
        int iFirstSign = 0, iLastSign = 0;  // in {-1,0,1}
        bool bSignChange = false;
        NiPoint3 kFirstPos = NiPoint3(0,0,0);
        NiPoint3 kLastPos = NiPoint3(0,0,0);
        NiPoint3 kDiffPos;
        float fInvX, fZDivX, fYDivX, fT;

        // Project the portion of the portal polygon which is in front of
        // the observer onto the viewing plane.  A vertex is "in front" if
        // its camera-space coordinates satisfy x > 0.  To avoid problems
        // with floating-point division, we instead use x > epsilon, for some
        // small positive epsilon.  If all vertices are in front, the
        // projections are computed and their axis-aligned bounding box is
        // stored in kRect.
        //
        // If a vertex is behind the observer (x <= epsilon), the projection
        // onto the viewing plane does not exist.  When such vertices occur,
        // we detect the polygon edges that intersect the x = epsilon plane
        // and compute the projection onto the viewing plane of the
        // edge-plane intersection.  Locating such edges efficiently requires
        // keeping track of the the "signs" of the x-values for the vertices.
        // The "sign" is 0 initially, indicating an uninitialized sign.  The
        // value 1 is assigned when x > epsilon, and the value -1 is assigned
        // when x <= epsilon.
        for (i = 0; i < iQuantity; i++)
        {
            kPos = kRotScale * (m_pkWorldVertex[i] - kTrans);

            if (kPos.x > EPSILON)
            {
                if (iFirstSign == 0)
                {
                    iFirstSign = 1;
                    kFirstPos = kPos;
                }

                fInvX = 1.0f / kPos.x;
                fZDivX = kPos.z * fInvX;
                fYDivX = kPos.y * fInvX;

                if (fZDivX < kRect.m_left)
                    kRect.m_left = fZDivX;

                if (fZDivX > kRect.m_right)
                    kRect.m_right = fZDivX;

                if (fYDivX < kRect.m_bottom)
                    kRect.m_bottom = fYDivX;

                if (fYDivX > kRect.m_top)
                    kRect.m_top = fYDivX;

                if (iLastSign < 0)
                    bSignChange = true;

                iLastSign = 1;
            }
            else
            {
                if (iFirstSign == 0)
                {
                    iFirstSign = -1;
                    kFirstPos = kPos;
                }

                if (iLastSign > 0)
                    bSignChange = true;

                iLastSign = -1;
            }

            if (bSignChange)
            {
                kDiffPos = kPos - kLastPos;
                fT = (EPSILON - kLastPos.x) / kDiffPos.x;
                fZDivX = (kLastPos.z + fT * kDiffPos.z) * INV_EPSILON;
                fYDivX = (kLastPos.y + fT * kDiffPos.y) * INV_EPSILON;

                if (fZDivX < kRect.m_left)
                    kRect.m_left = fZDivX;

                if (fZDivX > kRect.m_right)
                    kRect.m_right = fZDivX;

                if (fYDivX < kRect.m_bottom)
                    kRect.m_bottom = fYDivX;

                if (fYDivX > kRect.m_top)
                    kRect.m_top = fYDivX;

                bSignChange = false;
            }

            kLastPos = kPos;
        }

        // process the last polygon edge
        if (iFirstSign * iLastSign < 0)
        {
            kDiffPos = kFirstPos - kLastPos;
            fT = (EPSILON - kLastPos.x) / kDiffPos.x;
            fZDivX = (kLastPos.z + fT * kDiffPos.z) * INV_EPSILON;
            fYDivX = (kLastPos.y + fT * kDiffPos.y) * INV_EPSILON;

            if (fZDivX < kRect.m_left)
                kRect.m_left = fZDivX;

            if (fZDivX > kRect.m_right)
                kRect.m_right = fZDivX;

            if (fYDivX < kRect.m_bottom)
                kRect.m_bottom = fYDivX;

            if (fYDivX > kRect.m_top)
                kRect.m_top = fYDivX;
        }
    }
    else
    {
        for (i = 0; i < iQuantity; i++)
        {
            kPos = kRotScale * (m_pkWorldVertex[i] - kTrans);

            if (kPos.z < kRect.m_left)
                kRect.m_left = kPos.z;

            if (kPos.z > kRect.m_right)
                kRect.m_right = kPos.z;

            if (kPos.y < kRect.m_bottom)
                kRect.m_bottom = kPos.y;

            if (kPos.y > kRect.m_top)
                kRect.m_top = kPos.y;
        }
    }

    // If the portal cannot appear on the screen, the caller must be informed
    // not to draw the adjoiner.
    if (kFrustum.m_fLeft >= kRect.m_right ||
        kFrustum.m_fRight <= kRect.m_left ||
        kFrustum.m_fBottom >= kRect.m_top ||
        kFrustum.m_fTop <= kRect.m_bottom)
    {
        return false;
    }

    // The portal bounding rectangle intersects the current frustum.  Reduce
    // the frustum (if possible) for drawing the adjoiner.
    if (kFrustum.m_fLeft < kRect.m_left)
        kFrustum.m_fLeft = kRect.m_left;

    if (kFrustum.m_fRight > kRect.m_right)
        kFrustum.m_fRight = kRect.m_right;

    if (kFrustum.m_fBottom < kRect.m_bottom)
        kFrustum.m_fBottom = kRect.m_bottom;

    if (kFrustum.m_fTop > kRect.m_top)
        kFrustum.m_fTop = kRect.m_top;

    kCuller.SetFrustum(kFrustum);
    return true;
}
//---------------------------------------------------------------------------
void NiPortal::OnVisible(NiCullingProcess& kCuller)
{
    // Only traverse through the portal if it has not been seen before (it
    // is not marked) and if it is active.
    if (GetMarked() || !GetActive())
        return;

    SetMarked(true);

    NiFrustum kOldFrustum = kCuller.GetFrustum();
    if (m_pkAdjoiner && UpdateFrustum(kCuller))
    {
        // Compute the potentially visible set of the adjoining room, but
        // using the reduced frustum implied by the portal to that room.
        m_pkAdjoiner->Cull(kCuller);

        // Restore the old frustum and culling planes.
        kCuller.SetFrustum(kOldFrustum);
    }

    SetMarked(false);
}
//---------------------------------------------------------------------------
void NiPortal::UpdateWorldData()
{
    NiAVObject::UpdateWorldData();

    // update vertices
    for (unsigned short i = 0; i < m_usVertices; i++)
    {
        m_pkWorldVertex[i] = m_kWorld.m_Translate +
            m_kWorld.m_Rotate * m_kWorld.m_fScale * m_pkModelVertex[i];
    }
}
//---------------------------------------------------------------------------
void NiPortal::UpdateWorldBound()
{
    m_kWorldBound.Update(m_kModelBound, m_kWorld);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiPortal);
//---------------------------------------------------------------------------
void NiPortal::LoadBinary(NiStream& kStream)
{
    NiAVObject::LoadBinary(kStream);

    NiStreamLoadBinary(kStream, m_uFlags);

    // model points / planes

    // Old plane count value - no longer used
    unsigned short usTemp;
    NiStreamLoadBinary(kStream, usTemp);

    NiStreamLoadBinary(kStream, m_usVertices);
    m_pkModelVertex = NiNew NiPoint3[m_usVertices];
    m_pkWorldVertex = NiNew NiPoint3[m_usVertices];
    for (unsigned int i = 0; i < m_usVertices; i++)
        m_pkModelVertex[i].LoadBinary(kStream);

    m_kModelBound.ComputeFromData(m_usVertices, m_pkModelVertex);

    // portal adjoiner
    kStream.ReadLinkID();   // m_pkAdjoiner
}
//---------------------------------------------------------------------------
void NiPortal::LinkObject(NiStream& kStream)
{
    NiAVObject::LinkObject(kStream);

    // portal adjoiner
    m_pkAdjoiner = (NiAVObject*)kStream.GetObjectFromLinkID();
}
//---------------------------------------------------------------------------
bool NiPortal::RegisterStreamables(NiStream& kStream)
{
    return NiAVObject::RegisterStreamables(kStream);

    // Adjoined objects are already registered by the objects themselves.
    // They should not be registered here.
}
//---------------------------------------------------------------------------
void NiPortal::SaveBinary(NiStream& kStream)
{
    NiAVObject::SaveBinary(kStream);

    NiStreamSaveBinary(kStream, m_uFlags);

    // model points / planes

    // Old plane count value - no longer used
    unsigned short usTemp = 0;
    NiStreamSaveBinary(kStream, usTemp);

    NiStreamSaveBinary(kStream, m_usVertices);
    for (unsigned int i = 0; i < m_usVertices; i++)
        m_pkModelVertex[i].SaveBinary(kStream);

    // portal adjoiner
    kStream.SaveLinkID(m_pkAdjoiner);
}
//---------------------------------------------------------------------------
bool NiPortal::IsEqual(NiObject* pkObject)
{
    if (!NiAVObject::IsEqual(pkObject))
        return false;

    NiPortal* pkPortal = (NiPortal*)pkObject;

    // model points / planes
    if (m_usVertices != pkPortal->m_usVertices)
        return false;

    for (unsigned int i = 0; i < m_usVertices; i++)
    {
        if (m_pkModelVertex[i] != pkPortal->m_pkModelVertex[i])
            return false;
    }

    // switch to turn portal on/off
    if (GetActive() != pkPortal->GetActive())
        return false;

    return true;
}
//---------------------------------------------------------------------------
