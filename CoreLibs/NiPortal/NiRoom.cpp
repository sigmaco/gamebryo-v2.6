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

#include <NiCamera.h>
#include "NiPortal.h"
#include "NiRoom.h"

NiImplementRTTI(NiRoom, NiNode);

unsigned int NiRoom::ms_uiCurrentTimestamp = 0;

//---------------------------------------------------------------------------
NiRoom::NiRoom() :
    m_kWalls(0, 1),
    m_kInPortalActive(1, 1)
{
    m_uiLastRenderedTimestamp = 0;
    m_bRenderLock = false;
}
//---------------------------------------------------------------------------
NiRoom::~NiRoom()
{
    const unsigned int uiWallSize = m_kWalls.GetSize();
    for (unsigned int i = 0; i < uiWallSize; i++)
    {
        Wall* pkWall = m_kWalls.GetAt(i);
        if (pkWall)
            NiDelete pkWall;
    }
}
//---------------------------------------------------------------------------
void NiRoom::OnVisible(NiCullingProcess& kCuller)
{
    // Prevent infinite recursion if a room has already been visited.  In
    // theory, the portal visitation flags should prevent this, but it is
    // possible to have a malformed room graph that causes the cycle.
    if (m_bRenderLock)
        return;
    m_bRenderLock = true;

    // Disable all the incoming portals to prevent visiting this room again.
    NiTListIterator pkIter = m_kInPortal.GetHeadPos();
    unsigned int iPIndex = 0;
    NiPortal* pkPortal;
    while (pkIter)
    {
        pkPortal = m_kInPortal.GetNext(pkIter);
        if (pkPortal)
        {
            m_kInPortalActive.SetAtGrow(iPIndex, pkPortal->GetActive());
            pkPortal->SetActive(false);
            iPIndex++;
        }
    }

    // Propagate the graph traversal through outgoing portals.
    pkIter = m_kOutPortal.GetHeadPos();
    while (pkIter)
    {
        pkPortal = m_kOutPortal.GetNext(pkIter);
        if (pkPortal)
            pkPortal->Cull(kCuller);
    }

    // Draw the fixtures.  The default behavior is for the managing room
    // group to defer the drawing to allow building a list of unique
    // fixtures, thus avoiding drawing an object twice.
    pkIter = m_kFixture.GetHeadPos();
    while (pkIter)
    {
        NiAVObject* pkFixture = m_kFixture.GetNext(pkIter);
        if (pkFixture)
            pkFixture->Cull(kCuller);
    }

    // Restore the previous state of all the incoming portals.
    pkIter = m_kInPortal.GetHeadPos();
    iPIndex = 0;
    while (pkIter)
    {
        pkPortal = m_kInPortal.GetNext(pkIter);
        if (pkPortal)
        {
            pkPortal->SetActive(m_kInPortalActive.GetAt(iPIndex));
            iPIndex++;
        }
    }

    // Allow the room to be visited on the next drawing pass.
    m_bRenderLock = false;
}
//---------------------------------------------------------------------------
void NiRoom::UpdateWorldData()
{
    NiNode::UpdateWorldData();

    // update world plane

    // The model plane has normal N0, constant C0 and is given by
    // Dot(N0,X) = C0.  If Y = s*R*X+T where s is world scale, R is world
    // rotation, and T is world translation for current node, then
    // X = (1/s)*R^t*(Y-T) and
    //     C0 = Dot(N0,X) = Dot(N0,(1/s)*R^t*(Y-T)) = (1/s)*Dot(R*N0,Y-T)
    // so
    //     Dot(R*N0,Y) = s*c0+Dot(R*N0,T)
    // The world plane has
    //     normal N1 = R*N0
    //     constant C1 = s*C0+Dot(R*N0,T) = s*C0+Dot(N1,T)

    const unsigned int uiWallSize = m_kWalls.GetSize();
    for (unsigned int i = 0; i < uiWallSize; i++)
    {
        Wall* pkWall = m_kWalls.GetAt(i);
        if (pkWall)
        {
            const NiPlane& kModelPlane = pkWall->m_kModelPlane;
            NiPlane& kWorldPlane = pkWall->m_kWorldPlane;

            kWorldPlane.SetNormal(
                m_kWorld.m_Rotate * kModelPlane.GetNormal());
            kWorldPlane.SetConstant(
                m_kWorld.m_fScale * kModelPlane.GetConstant() 
                + kWorldPlane.GetNormal() * m_kWorld.m_Translate);
        }
    }
}
//---------------------------------------------------------------------------
void NiRoom::UpdateChildArray()
{
    // detach the current portals and fixtures
    const unsigned int uiChildCount = m_kChildren.GetSize();
    for (unsigned int i = 0; i < uiChildCount; i++)
        DetachChildAt(i);

    m_kChildren.RemoveAll();

    // attach the new portals
    NiTListIterator pkIter = m_kOutPortal.GetHeadPos();
    while (pkIter)
    {
        NiPortal* pkPortal = m_kOutPortal.GetNext(pkIter);
        if (pkPortal)
            AttachChild(pkPortal);
    }

    // attach the new fixtures
    pkIter = m_kFixture.GetHeadPos();
    while (pkIter)
    {
        NiAVObject* pkFixture = m_kFixture.GetNext(pkIter);
        if (pkFixture)
            AttachChild(pkFixture);
    }
}
//---------------------------------------------------------------------------
void NiRoom::AttachOutgoingPortal(NiPortal* pkPortal)
{
    if (!m_kOutPortal.FindPos(pkPortal))
    {
        m_kOutPortal.AddTail(pkPortal);
        UpdateChildArray();
    }
}
//---------------------------------------------------------------------------
NiPortalPtr NiRoom::DetachOutgoingPortal(NiPortal* pkPortal)
{
    if (m_kOutPortal.FindPos(pkPortal))
    {
        m_kOutPortal.Remove(pkPortal);
        UpdateChildArray();
        return pkPortal;
    }
    return 0;
}
//---------------------------------------------------------------------------
void NiRoom::AttachIncomingPortal(NiPortal* pkPortal)
{
    if (!m_kInPortal.FindPos(pkPortal))
    {
        m_kInPortal.AddTail(pkPortal);
        UpdateChildArray();
    }
}
//---------------------------------------------------------------------------
NiPortalPtr NiRoom::DetachIncomingPortal(NiPortal* pkPortal)
{
    if (m_kInPortal.FindPos(pkPortal))
    {
        m_kInPortal.Remove(pkPortal);
        UpdateChildArray();
        return pkPortal;
    }
    return 0;
}
//---------------------------------------------------------------------------
void NiRoom::AttachFixture(NiAVObject* pkFixture)
{
    NiAVObjectPtr spFixture = pkFixture;
    if (!m_kFixture.FindPos(spFixture))
    {
        m_kFixture.AddTail(spFixture);
        UpdateChildArray();
    }
}
//---------------------------------------------------------------------------
NiAVObjectPtr NiRoom::DetachFixture(NiAVObject* pkFixture)
{
    if (m_kFixture.FindPos(pkFixture))
    {
        NiAVObjectPtr spReturn = m_kFixture.Remove(pkFixture);
        UpdateChildArray();
        return spReturn;
    }
    return 0;
}
//---------------------------------------------------------------------------
bool NiRoom::ContainsPoint(const NiPoint3& kPoint) const
{
    const unsigned int uiWallSize = m_kWalls.GetSize();
    for (unsigned int i = 0; i < uiWallSize; i++)
    {
        Wall* pkWall = m_kWalls.GetAt(i);
        if (pkWall)
        {
            int iSide = pkWall->m_kWorldPlane.WhichSide(kPoint);
            if (iSide == NiPlane::NEGATIVE_SIDE)
                return false;
        }
    }
    return true;
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
#ifndef T3D_NO_NIF_CONVERSION
// For conversion of old NiWall objects only
class NIPORTAL_ENTRY  NiOldWall : public NiNode
{
    NiDeclareRTTI;
public:
    // construction
    NiOldWall()  { /* */ }
    inline virtual ~NiOldWall() { /* */ }

    inline virtual void LoadBinary(NiStream& kStream)
    {
        NiNode::LoadBinary(kStream);

        // model wall plane equation
        m_kModelPlane.LoadBinary(kStream);
    }

    NiPlane m_kModelPlane;
};

typedef NiPointer<NiOldWall> NiOldWallPtr;

NiImplementRTTI(NiOldWall,NiNode);

//---------------------------------------------------------------------------
NiObject* NiRoom::CreateOldWallObject(const char**, 
    unsigned int)
{
    return NiNew NiOldWall;
}
//---------------------------------------------------------------------------
void NiRoom::PostLinkObject(NiStream&)
{
    // Look for and replace NiWall objects with wall planes.
    while (m_kOldWalls.GetSize())
    {
        NiAVObjectPtr spAVObj = m_kOldWalls.RemoveHead();
        NiOldWallPtr spOldWall = NiDynamicCast(NiOldWall, spAVObj);

        if (!spOldWall)
            continue;

        const NiPlane& kOldPlane = spOldWall->m_kModelPlane;
        const NiTransform& kOldXform = spOldWall->GetLocalTransform();

        Wall* pkWall = NiNew Wall;

        pkWall->m_kModelPlane.SetNormal(
            kOldXform.m_Rotate * kOldPlane.GetNormal());
        pkWall->m_kModelPlane.SetConstant(
            kOldXform.m_fScale * kOldPlane.GetConstant() 
            + pkWall->m_kModelPlane.GetNormal() * kOldXform.m_Translate);
        
        m_kWalls.Add(pkWall);
    }
}
#endif
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
NiImplementCreateObject(NiRoom);
//---------------------------------------------------------------------------
void NiRoom::LoadBinary(NiStream& kStream)
{
    NiNode::LoadBinary(kStream);

    unsigned int uiWallSize = 0;
    NiStreamLoadBinary(kStream, uiWallSize);
    if (uiWallSize > 0)
    {
        m_kWalls.SetSize(uiWallSize);
        for (unsigned int i = 0; i < uiWallSize; i++)
        {
            Wall* pkWall = NiNew Wall;
            pkWall->m_kModelPlane.LoadBinary(kStream);
            m_kWalls.SetAt(i, pkWall);
        }
    }

    // load in portals
    kStream.ReadMultipleLinkIDs();   // m_kInPortal

    // load out portals
    kStream.ReadMultipleLinkIDs();   // m_kOutPortal

    // load fixtures
    kStream.ReadMultipleLinkIDs();   // m_kFixture
}
//---------------------------------------------------------------------------
void NiRoom::LinkObject(NiStream& kStream)
{
    NiNode::LinkObject(kStream);

    unsigned int uiWalls = 0;
    
    unsigned int uiInPortals = kStream.GetNumberOfLinkIDs();  // m_kInPortal
    unsigned int uiOutPortals = kStream.GetNumberOfLinkIDs();  // m_kOutPortal
    unsigned int uiFixtures = kStream.GetNumberOfLinkIDs();  // m_kFixture

    // link walls in reverse order (for IsEqual to work properly)
    while (uiWalls--)
    {
#ifndef T3D_NO_NIF_CONVERSION
        // add walls alist, to be converted later
        m_kOldWalls.AddTail((NiAVObject*)kStream.GetObjectFromLinkID());
#else
        // handle old walls, which will be ignored
        kStream.GetObjectFromLinkID();
#endif
    }

    // link in portals in reverse order (for IsEqual to work properly)
    while (uiInPortals--)
        m_kInPortal.AddTail((NiPortal*)kStream.GetObjectFromLinkID());

    // link out portals in reverse order (for IsEqual to work properly)
    while (uiOutPortals--)
        m_kOutPortal.AddTail((NiPortal*)kStream.GetObjectFromLinkID());

    // link fixtures in reverse order (for IsEqual to work properly)
    while (uiFixtures--)
        m_kFixture.AddTail((NiAVObject*)kStream.GetObjectFromLinkID());
}
//---------------------------------------------------------------------------
bool NiRoom::RegisterStreamables(NiStream& kStream)
{
    if (!NiNode::RegisterStreamables(kStream))
        return false;

    // register in portals
    NiPortal* pkPortal;
    NiTListIterator pkIter = m_kInPortal.GetHeadPos();
    while (pkIter)
    {
        pkPortal = m_kInPortal.GetNext(pkIter);
        if (pkPortal)
            pkPortal->RegisterStreamables(kStream);
    }

    // register out portals
    pkIter = m_kOutPortal.GetHeadPos();
    while (pkIter)
    {
        pkPortal = m_kOutPortal.GetNext(pkIter);
        if (pkPortal)
            pkPortal->RegisterStreamables(kStream);
    }

    // register fixtures
    pkIter = m_kFixture.GetHeadPos();
    while (pkIter)
    {
        NiAVObject* pkAVObject = m_kFixture.GetNext(pkIter);
        if (pkAVObject)
            pkAVObject->RegisterStreamables(kStream);
    }

    return true;
}
//---------------------------------------------------------------------------
void NiRoom::SaveBinary(NiStream& kStream)
{
    NiNode::SaveBinary(kStream);

    // save walls, but compress out the NULL entries
    unsigned int uiWallSize = m_kWalls.GetSize();
    unsigned int uiWallCount = 0;
    unsigned int i;
    for (i = 0; i < uiWallSize; i++)
    {
        if (m_kWalls.GetAt(i))
            uiWallCount++;
    }

    NiStreamSaveBinary(kStream, uiWallCount);
    if (uiWallCount > 0)
    {
        for (i = 0; i < uiWallSize; i++)

        {
            Wall* pkWall = m_kWalls.GetAt(i);
            if (pkWall)
                pkWall->m_kModelPlane.SaveBinary(kStream);
        }
    }

    // save in portals
    unsigned int uiListSize = m_kInPortal.GetSize();
    NiStreamSaveBinary(kStream, uiListSize);
    NiTListIterator pkIter;
    NiPortal* pkPortal;
    if (uiListSize > 0)
    {
        pkIter = m_kInPortal.GetHeadPos();
        while (pkIter)
        {
            pkPortal = m_kInPortal.GetNext(pkIter);
            if (pkPortal)
                kStream.SaveLinkID(pkPortal);
        }
    }

    // save out portals
    uiListSize = m_kOutPortal.GetSize();
    NiStreamSaveBinary(kStream, uiListSize);
    if (uiListSize > 0)
    {
        pkIter = m_kOutPortal.GetHeadPos();
        while (pkIter)
        {
            pkPortal = m_kOutPortal.GetNext(pkIter);
            if (pkPortal)
                kStream.SaveLinkID(pkPortal);
        }
    }

    // save fixtures
    uiListSize = m_kFixture.GetSize();
    NiStreamSaveBinary(kStream, uiListSize);
    if (uiListSize > 0)
    {
        pkIter = m_kFixture.GetHeadPos();
        while (pkIter)
        {
            NiAVObject* pkAVObject = m_kFixture.GetNext(pkIter);
            if (pkAVObject)
                kStream.SaveLinkID(pkAVObject);
        }
    }
}
//---------------------------------------------------------------------------
bool NiRoom::IsEqual(NiObject* pkObject)
{
    if (!NiNode::IsEqual(pkObject))
        return false;

    NiRoom* pkRoom = (NiRoom*)pkObject;

    // check walls
    unsigned int uiCount0 = m_kWalls.GetSize();
    unsigned int uiCount1 = pkRoom->m_kWalls.GetSize();
    if (uiCount0 != uiCount1)
        return false;

    if (uiCount0 > 0)
    {
        for (unsigned int i = 0; i < uiCount0; i++)
        {
            Wall* pkWall0 = m_kWalls.GetAt(i);
            Wall* pkWall1 = m_kWalls.GetAt(i);
            if ((pkWall0 && !pkWall1) || (!pkWall0 && pkWall1))
            {
                return false;
            }
            if (pkWall0 &&
                pkWall0->m_kModelPlane != pkWall1->m_kModelPlane)
            {
                return false;
            }
        }
    }

    // check in portals
    uiCount0 = m_kInPortal.GetSize();
    uiCount1 = pkRoom->m_kInPortal.GetSize();
    if (uiCount0 != uiCount1)
        return false;

    NiTListIterator pkIter0;
    NiTListIterator pkIter1;
    NiPortal* pkPortal0;
    NiPortal* pkPortal1;

    if (uiCount0 > 0)
    {
        pkIter0 = m_kInPortal.GetHeadPos();
        pkIter1 = pkRoom->m_kInPortal.GetHeadPos();
        while (pkIter0)
        {
            pkPortal0 = m_kInPortal.GetNext(pkIter0);
            pkPortal1 = m_kInPortal.GetNext(pkIter1);
            if ((pkPortal0 && !pkPortal1) || (!pkPortal0 && pkPortal1))
                return false;
            if (pkPortal0 && !pkPortal0->IsEqual(pkPortal1))
                return false;
        }
    }

    // check out portals
    uiCount0 = m_kOutPortal.GetSize();
    uiCount1 = pkRoom->m_kOutPortal.GetSize();
    if (uiCount0 != uiCount1)
        return false;

    if (uiCount0 > 0)
    {
        pkIter0 = m_kOutPortal.GetHeadPos();
        pkIter1 = pkRoom->m_kOutPortal.GetHeadPos();
        while (pkIter0)
        {
            pkPortal0 = m_kOutPortal.GetNext(pkIter0);
            pkPortal1 = m_kOutPortal.GetNext(pkIter1);
            if ((pkPortal0 && !pkPortal1) || (!pkPortal0 && pkPortal1))
                return false;
            if (pkPortal0 && !pkPortal0->IsEqual(pkPortal1))
                return false;
        }
    }

    // check fixtures
    uiCount0 = m_kFixture.GetSize();
    uiCount1 = pkRoom->m_kFixture.GetSize();
    if (uiCount0 != uiCount1)
        return false;

    if (uiCount0 > 0)
    {
        pkIter0 = m_kFixture.GetHeadPos();
        pkIter1 = pkRoom->m_kFixture.GetHeadPos();
        while (pkIter0)
        {
            NiAVObject* pkAVObject0 = m_kFixture.GetNext(pkIter0);
            NiAVObject* pkAVObject1 = m_kFixture.GetNext(pkIter1);
            if ((pkAVObject0 && !pkAVObject1) ||
                (!pkAVObject0 && pkAVObject1))
            {
                return false;
            }
            if (pkAVObject0 && !pkAVObject0->IsEqual(pkAVObject1))
                return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
