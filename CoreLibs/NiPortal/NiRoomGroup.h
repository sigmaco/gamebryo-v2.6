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

#ifndef NIROOMGROUP_H
#define NIROOMGROUP_H

#include "NiRoom.h"
#include "NiStream.h"
#include "NiFixtureCuller.h"

class NIPORTAL_ENTRY NiRoomGroup : public NiNode
{
    NiDeclareRTTI;
    NiDeclareStream;
public:
    // construction
    NiRoomGroup();
    virtual ~NiRoomGroup();

    // When the camera is outside the space occupied by the room group, a
    // shell of the group may be drawn (if specified).
    inline NiAVObjectPtr GetShell() const;
    void AttachShell(NiAVObject* pkShell);
    NiAVObjectPtr DetachShell();

    // The rooms managed by the room group.
    inline const NiRoomList& GetRoomList() const;
    void AttachRoom(NiRoom* pkRoom);
    NiRoomPtr DetachRoom(NiRoom* pkRoom);

    // culling (separate from drawing)
    virtual void OnVisible(NiCullingProcess& kCuller);

    // Determine which room contains the input point.  The point must be in
    // world-space coordinates.
    NiRoomPtr WhichRoom(const NiPoint3& kPoint) const;

    // Determine which room contains the input point.  The point must be in
    // world-space coordinates.  The room graph search is started using the
    // input room.  This room must, of course, be in the graph.
    NiRoomPtr WhichRoom(const NiPoint3& kPoint, NiRoom* pkLastRoom) const;

    // The initial room in the graph traversal for the last display pass.
    inline NiRoomPtr GetLastRoom() const;

    // Specify which room contains the camera.  This is useful for an
    // application that knows where the camera is, thereby avoiding a linear
    // search over the list of rooms to determine which one contains the
    // camera.
    inline void SetLastRoom(NiRoom* pkLastRoom);

protected:
    // Whenever the shell or rooms are attached or detached from the room
    // group, the child node pointers of the room group must be updated.
    void UpdateChildArray();

    // The object to draw when the camera is located outside the space
    // occupied by the room group.
    NiAVObjectPtr m_spShell;

    // The rooms managed by the room group.
    NiRoomList m_kRooms;

    // The initial room last visited by a display pass.
    NiRoomPtr m_spLastRoom;
};

typedef NiPointer<NiRoomGroup> NiRoomGroupPtr;
#include "NiRoomGroup.inl"

#endif

