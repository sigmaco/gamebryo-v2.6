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

#include <NiAccumulator.h>
#include <NiCamera.h>
#include "NiPortal.h"
#include "NiPortalMetrics.h"
#include "NiRoomGroup.h"

NiImplementRTTI(NiRoomGroup, NiNode);

//---------------------------------------------------------------------------
NiRoomGroup::NiRoomGroup()
{
}
//---------------------------------------------------------------------------
NiRoomGroup::~NiRoomGroup()
{
    m_kRooms.RemoveAll();
}
//---------------------------------------------------------------------------
void NiRoomGroup::OnVisible(NiCullingProcess& kCuller)
{
    NIMETRICS_PORTAL_SCOPETIMER(ROOMGROUP_ONVISIBLE);

    // Locate the room which contains the camera (or NULL if outside).
    const NiCamera* pkCamera = kCuller.GetCamera();
    NiRoom* pkRoom = WhichRoom(pkCamera->GetWorldLocation(), m_spLastRoom);
    if (pkRoom)
    {
        // The camera is in pkRoom.  Remember this room for a fast lookup
        // in the next call to WhichRoom.
        m_spLastRoom = pkRoom;

        // Compute the potentially visible set of the room.  Use the fixture
        // culler to obtain a unique list of objects, and then append that
        // list to the input culler.
        NiFixtureCuller kFixtureCuller(kCuller.GetWorkflowManager());
        kFixtureCuller.Process(pkCamera, pkRoom, kCuller.GetVisibleSet());
    }
    else if (m_spShell)
    {
        // The camera is outside the room group, process the shell.
        m_spLastRoom = pkRoom;
        m_spShell->Cull(kCuller);
    }
    else
    {
        // The camera is outside the room group, but there is no shell to
        // process.  Do no change the m_spLastRoom in this case.  When there
        // is no shell, we just continue processing the last room.  This
        // behavior limits the issues with rooms that do not quite overlap.
        if (m_spLastRoom)
            m_spLastRoom->Cull(kCuller);
    }
}
//---------------------------------------------------------------------------
void NiRoomGroup::UpdateChildArray()
{
    // detach the current shell and rooms
    for (unsigned int i = 0; i < GetArrayCount(); i++)
        DetachChildAt(i);

    m_kChildren.RemoveAll();

    // attach the new shell and rooms
    if (m_spShell)
        AttachChild(m_spShell);

    NiTListIterator pkIter = m_kRooms.GetHeadPos();
    while (pkIter)
        AttachChild(m_kRooms.GetNext(pkIter));
}
//---------------------------------------------------------------------------
void NiRoomGroup::AttachShell(NiAVObject* pkShell)
{
    m_spShell = pkShell;
    UpdateChildArray();
}
//---------------------------------------------------------------------------
NiAVObjectPtr NiRoomGroup::DetachShell()
{
    NiAVObjectPtr pkShell = m_spShell;
    m_spShell = 0;
    UpdateChildArray();
    return pkShell;
}
//---------------------------------------------------------------------------
void NiRoomGroup::AttachRoom(NiRoom* pkRoom)
{
    if (!m_kRooms.FindPos(pkRoom))
    {
        m_kRooms.AddHead(pkRoom);
        UpdateChildArray();
    }
}
//---------------------------------------------------------------------------
NiRoomPtr NiRoomGroup::DetachRoom(NiRoom* pkRoom)
{
    if (m_kRooms.FindPos(pkRoom))
    {
        m_kRooms.Remove(pkRoom);
        UpdateChildArray();
        return pkRoom;
    }
    return 0;
}
//---------------------------------------------------------------------------
NiRoomPtr NiRoomGroup::WhichRoom(const NiPoint3& kPoint) const
{
    NiTListIterator pkIter = m_kRooms.GetHeadPos();
    while (pkIter)
    {
        NiRoom* pkRoom = m_kRooms.GetNext(pkIter);
        if (pkRoom && pkRoom->ContainsPoint(kPoint))
            return pkRoom;
    }
    return 0;
}
//---------------------------------------------------------------------------
NiRoomPtr NiRoomGroup::WhichRoom(const NiPoint3& kPoint,
    NiRoom* pkLastRoom) const
{
    // The input point is the camera location.  Based on time coherency, the
    // probability that the last room still contains the camera is large, so
    // the last room is tested first to reduce the search time.  If the
    // camera is not in the last room, a linear search is made through the
    // other rooms of the group.

    if (pkLastRoom && pkLastRoom->ContainsPoint(kPoint))
        return pkLastRoom;

    return WhichRoom(kPoint);
}
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// streaming
//---------------------------------------------------------------------------
NiImplementCreateObject(NiRoomGroup);
//---------------------------------------------------------------------------
void NiRoomGroup::LoadBinary(NiStream& kStream)
{
    NiNode::LoadBinary(kStream);

    kStream.ReadLinkID();            // m_spShell
    kStream.ReadMultipleLinkIDs();   // m_kRooms
}
//---------------------------------------------------------------------------
void NiRoomGroup::LinkObject(NiStream& kStream)
{
    NiNode::LinkObject(kStream);

    // link contained objects - shell objects
    m_spShell = (NiAVObject*)kStream.GetObjectFromLinkID();

    // link contained objects - room objects
    // link room objects in reverse order (for IsEqual to work properly)
    unsigned int uiRooms = kStream.GetNumberOfLinkIDs();

    while (uiRooms--)
        m_kRooms.AddHead((NiRoom*)kStream.GetObjectFromLinkID());
}
//---------------------------------------------------------------------------
bool NiRoomGroup::RegisterStreamables(NiStream& kStream)
{
    if (!NiNode::RegisterStreamables(kStream))
        return false;

    // register contained objects - shell objects
    if (m_spShell)
        m_spShell->RegisterStreamables(kStream);

    // register contained objects - room objects
    NiTListIterator pkList = m_kRooms.GetHeadPos();
    while (pkList)
    {
        NiRoom* pkRoom = m_kRooms.GetNext(pkList);
        if (pkRoom)
            pkRoom->RegisterStreamables(kStream);

    }

    return true;
}
//---------------------------------------------------------------------------
void NiRoomGroup::SaveBinary(NiStream& kStream)
{
    NiNode::SaveBinary(kStream);

    // pointer types

    // contained objects - shell objects
    kStream.SaveLinkID(m_spShell);

    // contained objects - room objects
    int iListSize = m_kRooms.GetSize();
    NiStreamSaveBinary(kStream, iListSize);
    if (iListSize > 0)
    {
        // save in reverse order because rooms will link faster that way
        NiRoom** ppkRoom = NiAlloc(NiRoom*, iListSize);
        int i = 0;
        NiTListIterator pkList = m_kRooms.GetHeadPos();
        while (pkList)
            ppkRoom[i++] = m_kRooms.GetNext(pkList);

        for (i = iListSize - 1; i >= 0; i--)
            kStream.SaveLinkID(ppkRoom[i]);
        NiFree(ppkRoom);
    }
}
//---------------------------------------------------------------------------
bool NiRoomGroup::IsEqual(NiObject* pkObject)
{
    if (!NiNode::IsEqual(pkObject))
        return false;

    NiRoomGroup* pkRoomGroup = (NiRoomGroup*)pkObject;

    // contained objects - shell objects
    NiAVObject* pkShell0 = m_spShell;
    NiAVObject* pkShell1 = pkRoomGroup->m_spShell;
    if ((pkShell0 && !pkShell1) || (!pkShell0 && pkShell1))
        return false;
    if (pkShell0 && !pkShell0->IsEqual(pkShell1))
        return false;

    // contained objects - room objects
    unsigned int uiCount0 = m_kRooms.GetSize();
    unsigned int uiCount1 = pkRoomGroup->m_kRooms.GetSize();
    if (uiCount0 != uiCount1)
        return false;

    if (uiCount0 > 0)
    {
        NiTListIterator pkList0 = m_kRooms.GetHeadPos();
        NiTListIterator pkList1 = pkRoomGroup->m_kRooms.GetHeadPos();
        while (pkList0)
        {
            NiRoom* pkRoom0 = m_kRooms.GetNext(pkList0);
            NiRoom* pkRoom1 = pkRoomGroup->m_kRooms.GetNext(pkList1);
            if ((pkRoom0 && !pkRoom1) || (!pkRoom0 && pkRoom1))
                return false;
            if (pkRoom0 && !pkRoom0->IsEqual(pkRoom1))
                return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
