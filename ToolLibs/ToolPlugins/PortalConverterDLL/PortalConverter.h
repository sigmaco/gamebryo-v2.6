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

#ifndef PORTALCONVERTER_H
#define PORTALCONVERTER_H

#include <NiSmartPointer.h>
#include <NiTStringMap.h>
#include <NiRoom.h>
#include <NiRoomGroup.h>
#include <NiPortal.h>
#include <NiString.h>
#include "HullManager.h"

class PortalConverter : public NiMemObject
{
public:
    // Construction and destruction.  The constructor does the conversion.
    PortalConverter(NiNodePtr& spScene);
    ~PortalConverter();

    // Query if the conversion was successful.
    bool Successful() const;

    // The accumulated messages from the conversion.
    const NiString& Messages() const;

protected:
    // The data structure to associate a tagged room node and the actual
    // NiRoom object.
    class RoomMapping : public NiMemObject
    {
    public:
        RoomMapping(NiNode* pkRoomNode);
        ~RoomMapping();

        NiRoomPtr m_spRoom;
        NiNodePtr m_spRoomNode;
        HullManager* m_pkHull;
        bool m_bHasTaggedWalls;
    };

    // Traverse the scene graph to locate all nodes tagged as room groups
    // (_NDLRG).  The return value is 'true' iff the room groups are not
    // nested.
    static bool FindRoomGroups(const NiAVObject* pkObject,
        NiTPrimitiveArray<NiNode*>& kRoomGroupNodes);

    // Traverse the children of the room group node looking for nodes tagged
    // as rooms (_NDLRM).  The return value is 'true' whenever at least one
    // room is found, all rooms are NiNode-derived classes, and no two rooms
    // have the same name.
    bool FindRooms(const NiNode* pkRoomGroupNode, NiNode*& pkShell);

    // This function is the workhorse of the converter.  The room geometry
    // is accumulated (or only wall geometry if _NDLWL tags were used) for
    // each room.  The rooms start out with bounding boxes to delimit the
    // space occupied by the room.  Room-room intersection testing is done
    // using the bounding boxes.  If two boxes intersect, the rooms compute
    // and use their convex hulls to delimit the space.  The walls of the
    // room are constructed from the facets of the bounding box or convex
    // hull, whichever is in use by the room.
    bool ParseRooms(NiRoomGroup* pkRoomGroup);

    // A recursive call applied to each room-tagged object to see if _NDLWL
    // tags were set.  If at least one is found, the convex hull is built from
    // the _NDLWL-tagged objects and the room geometry is ignored.  Otherwise,
    // the convex hull is built from all the room geometry, potentially
    // expensive since "interior" objects will have no influence on the hull.
    static bool HasTaggedWalls(const NiAVObject* pkObject);

    // Attach the input planes to the room object.
    static void AddWallsToRoom(int iNumPlanes, const NiPlane* akPlanes,
        NiRoom* pkRoom);

    // Portals are found (using the _NDLPR tags) and the room graph is
    // constructed a room-portal-room triple at a time.
    bool ParsePortal(const NiAVObject* pkPortalObj, NiRoom* pkRoom);

    // Portal polygon construction.  The computed polygon is a rectangle
    // that bounds the actual portal polygon.
    static bool ConstructPolygon(const NiAVObject* pkObject,
        NiPoint3 akRectangle[4]);

    // A recursive call to accumulate the points in a subtree, either rooted
    // at a tagged room node or at a tagged portal node.
    static void GetPolygonPoints(const NiAVObject* pkObject, bool bSkipMeshes,
        unsigned int& uiNumPoints, NiPoint3*& akPoints);

    // Locate the first triangle associated with meshes in a tagged portal
    // subtree.  The plane of the triangle is used as the plane of the
    // portal polygon.
    static bool GetPolygonTriangle(const NiAVObject* pkObject, NiPoint3& kP0,
        NiPoint3& kP1, NiPoint3& kP2);

    // Used to copy various fields of NiAVObject from a tagged room group
    // node to the actual NiRoomGroup and from a tagged room node to the
    // actual NiRoom.
    static void CopyData(const NiAVObject* pkSrc, NiAVObject* pkDest);

    // Get the adjoiner's name from the name tag of the portal.
    static char* GetAdjoinerName(const NiObjectNET* pkPortalObj);

    // Determine if the portal is two way from the name tag of the portal.
    static bool IsTwoWay(const NiObjectNET* pkPortalObj);

    // Find the first occurrence of the string acSub in the the string acStr.
    // If acStr is null or if acSub is not found, the function returns the
    // null pointer.  Otherwise, it returns a pointer to the substring portion
    // of acStr.
    static const char* FindSub(const char* acStr, const char* acSub);

    NiTStringMap<RoomMapping*> m_kRooms;
    NiString m_kMessages;
    char m_acTemp[512];
    bool m_bSuccessful;
}; 

#endif
