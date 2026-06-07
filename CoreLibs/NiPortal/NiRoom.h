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

#ifndef NIROOM_H
#define NIROOM_H

#include "NiNode.h"
#include "NiPortal.h"
typedef NiTPointerList<NiAVObjectPtr> NiAVObjectList;

class NIPORTAL_ENTRY  NiRoom : public NiNode
{
    NiDeclareRTTI;
    NiDeclareStream;
public:
    // construction
    NiRoom();
    ~NiRoom();

    // The walls that bound the room.  The region enclosed by the walls is
    // necessarily convex.
    class Wall : public NiMemObject
    {
    public:
        NiPlane m_kModelPlane;
        NiPlane m_kWorldPlane;
    };
    typedef NiTPrimitiveArray<Wall*> WallArray;
    WallArray& GetWallArray();

    // The outgoing portals for the room.  These link to adjoining rooms
    // that are seen from the current room.
    inline const NiPortalList& GetOutgoingPortalList() const;
    void AttachOutgoingPortal(NiPortal* pkPortal);
    NiPortalPtr DetachOutgoingPortal(NiPortal* pkPortal);

    // The incoming portals for the room.  These link from adjoining rooms
    // that can see the current room.
    inline const NiPortalList& GetIncomingPortalList() const;
    void AttachIncomingPortal(NiPortal* pkPortal);
    NiPortalPtr DetachIncomingPortal(NiPortal* pkPortal);

    // The fixtures for a room consist of the wall geometry and any objects
    // inside the room.
    inline const NiAVObjectList& GetFixtureList() const;
    inline void AttachFixture(NiAVObject* pkFixture);
    NiAVObjectPtr DetachFixture(NiAVObject* pkFixture);

    // Test if the input point is in the room.  The point must be in world
    // coordinates and is compared to the wall planes.
    bool ContainsPoint(const NiPoint3& kPoint) const;

    // A timestamp system to allow you to track the culling and rendering
    // behavior of the room group system.
    inline unsigned int GetLastRenderedTimestamp() const;
    static void SetCurrentTimestamp(unsigned int uiTimestamp);
    static unsigned int GetCurrentTimestamp();

    // *** begin Emergent internal use only ***
    
    // culling (separate from drawing)
    virtual void OnVisible(NiCullingProcess& kCuller);

#ifndef T3D_NO_NIF_CONVERSION
    static NiObject* CreateOldWallObject(const char** pcArguments = 0,
        unsigned int uiArgCount = 0);
    virtual void PostLinkObject(NiStream& kStream);
    NiAVObjectList m_kOldWalls;
#endif

    // *** end Emergent internal use only ***

protected:
    // Whenever the portals or fixtures are attached or detached from the
    // room, the child node pointers of the room must be updated.
    void UpdateChildArray();

    // The world coordinates of the wall planes must be updated on changes
    // in transformation state.
    virtual void UpdateWorldData();

    // The bounding walls for the room.
    WallArray m_kWalls;

    // The incoming portals for the room.
    NiPortalList m_kInPortal;

    // Temporary storage for the Display(...) call to save the incoming
    // portal state before propagating the graph traversal to adjoining
    // rooms.
    NiTPrimitiveArray<bool> m_kInPortalActive;

    // The outgoing portals for the room.
    NiPortalList m_kOutPortal;

    // The wall geometry for the room and any objects inside the room.
    NiAVObjectList m_kFixture;

    // Support for tracking culling and rendering behavior.
    unsigned int m_uiLastRenderedTimestamp;

    // To prevent an infinite loop if a room occurs in a graph cycle.
    bool m_bRenderLock;

    // Support for tracking culling and rendering behavior.
    static unsigned int ms_uiCurrentTimestamp;
};

typedef NiPointer<NiRoom> NiRoomPtr;
typedef NiTPointerList<NiRoomPtr> NiRoomList;
#include "NiRoom.inl"

#endif

