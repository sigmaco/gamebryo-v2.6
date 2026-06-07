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

#include "PortalConverter.h"
#include <NiInfoDialog.h>
#include <NiStringExtraData.h>
#include <NiMesh.h>
#include <NiDataStreamPrimitiveLock.h>
#include <NiMeshAlgorithms.h>
#include <NiDataStreamElementLock.h>
#include <NiPoint2.h>

typedef NiTStridedRandomAccessIterator<NiPoint3> NiVertIter;

//---------------------------------------------------------------------------
// Functor class / operator() definition for used with NiMeshAlgorithms
//---------------------------------------------------------------------------
class FirstNonDegenerate
{
public:
    NiPoint3 m_kV0;
    NiPoint3 m_kV1;
    NiPoint3 m_kV2;
    bool m_bFoundFirst;

    NiVertIter& m_kVertIter;

    FirstNonDegenerate(NiVertIter& kVertIter) : m_kVertIter(kVertIter), 
        m_bFoundFirst(false) {}

#ifdef NIDEBUG
    inline bool operator ()(const NiUInt32* pIndices, NiUInt32, 
        NiUInt32, NiUInt16 uiSubMesh)
#else
    inline bool operator ()(const NiUInt32* pIndices, NiUInt32, 
        NiUInt32, NiUInt16)
#endif
    {
        // This function is only designed to work with a single sub-mesh.
        // The vertex iterator access may give incorrect results if used
        // with more than one sub-mesh because the iterator has not been
        // initialized with the specific region for anything other than the
        // first mesh.
        NIASSERT(uiSubMesh == 0);
        
        NiUInt32 ui0 = *pIndices++;
        NiUInt32 ui1 = *pIndices++;
        NiUInt32 ui2 = *pIndices;

        if (ui0 == ui1 || ui0 == ui2 || ui1 == ui2)
            return true; // degenerate, so continue

        m_bFoundFirst = true;

        m_kV0 = m_kVertIter[ui0];
        m_kV1 = m_kVertIter[ui1];
        m_kV2 = m_kVertIter[ui2];

        return false; // break;
    }
private:
    FirstNonDegenerate & operator=( const FirstNonDegenerate & );
};

//---------------------------------------------------------------------------
PortalConverter::PortalConverter(NiNodePtr& spScene)
{
    m_kMessages.Empty();
    m_bSuccessful = true;

    // This function should be called on scene graph roots only.
    if (spScene->GetParent())
    {
        NiSprintf(m_acTemp, 512, "The input scene must be a root node.\n");
        m_kMessages += m_acTemp;
        m_bSuccessful = false;
        return;
    }

    // Ensure all world data is up to date since we will need to compute
    // convex hulls using vertices in the coordinate systems of the room
    // groups.
    spScene->Update(0.0f);
    NiMesh::CompleteSceneModifiers(spScene);

    // Locate all the room groups in the scene.
    NiTPrimitiveArray<NiNode*> kRoomGroupNodes(32, 32);
    bool bNotNested = FindRoomGroups(spScene, kRoomGroupNodes);
    const unsigned int uiSize = kRoomGroupNodes.GetSize();
    if (uiSize == 0)
    {
        NiSprintf(m_acTemp, 512, "The scene has no room groups.\n");
        m_kMessages += m_acTemp;
        m_bSuccessful = false;
        return;
    }
    if (!bNotNested)
    {
        NiSprintf(m_acTemp, 512, "Room groups cannot be nested.\n");
        m_kMessages += m_acTemp;
        m_bSuccessful = false;
        return;
    }

    if (uiSize == 1)
    {
        NiSprintf(m_acTemp, 512, "The scene has 1 room group.\n\n");
    }
    else
    {
        NiSprintf(m_acTemp, 512, "The scene has %u room groups.\n\n",
            uiSize);
    }

    m_kMessages += m_acTemp;

    for (unsigned int i = 0; i < uiSize; i++)
    {
        NiSprintf(m_acTemp, 512, "Processing room group %u.\n", i);
        m_kMessages += m_acTemp;

        // Find all of the tagged room nodes.  Any nontagged nodes will form
        // the shell of the room group.
        NiNode* pkRoomGroupNode = kRoomGroupNodes.GetAt(i);
        NiNode* pkShell = 0;
        if (FindRooms(pkRoomGroupNode, pkShell))
        {
            // Parse the rooms and their contents.
            NiRoomGroup* pkRoomGroup = NiNew NiRoomGroup;
            CopyData(pkRoomGroupNode, pkRoomGroup);
            if (ParseRooms(pkRoomGroup))
            {
                // Attach the shell (if any).
                if (pkShell)
                {
                    NiSprintf(m_acTemp, 512,
                        "The room group has a shell.\n");
                    m_kMessages += m_acTemp;
                    pkRoomGroup->AttachShell(pkShell);
                    NiStringExtraData* pkData =
                        NiNew NiStringExtraData("NiOptimizeKeep");
                    pkShell->AddExtraData(pkData);
                }

                // Replace the tagged node by the room group node.
                NiNode* pkParent = pkRoomGroupNode->GetParent();
                if (pkParent)
                {
                    pkParent->DetachChild(pkRoomGroupNode); 
                    pkParent->AttachChild(pkRoomGroup);
                }
                else
                {
                    spScene = pkRoomGroup;
                }
            }
            else
            {
                NiDelete pkRoomGroup;
                NiDelete pkShell;
                m_bSuccessful = false;
            }
        }
        else
        {
            NiDelete pkShell;
            m_bSuccessful = false;
        }

        // Clear the room mappings.
        NiTMapIterator kIter = m_kRooms.GetFirstPos();
        while (kIter)
        {
            RoomMapping* pkRoomMap;
            const char* pcKey;
            m_kRooms.GetNext(kIter, pcKey, pkRoomMap);
            NiDelete pkRoomMap;
        }
        m_kRooms.RemoveAll();

        if (m_bSuccessful)
        {
            NiSprintf(m_acTemp, 512,
                "Room group %u was converted successfully.\n\n", i);
            m_kMessages += m_acTemp;
        }
    }
}
//---------------------------------------------------------------------------
PortalConverter::~PortalConverter()
{
}
//---------------------------------------------------------------------------
bool PortalConverter::Successful() const
{
    return m_bSuccessful;
}
//---------------------------------------------------------------------------
const NiString& PortalConverter::Messages() const
{
    return m_kMessages;
}
//---------------------------------------------------------------------------
PortalConverter::RoomMapping::RoomMapping(NiNode* pkRoomNode) :
    m_spRoom(NiNew NiRoom),
    m_spRoomNode(pkRoomNode)
{
    m_pkHull = 0;
    m_bHasTaggedWalls = false;
}
//---------------------------------------------------------------------------
PortalConverter::RoomMapping::~RoomMapping()
{
    NiDelete m_pkHull;
}
//---------------------------------------------------------------------------
bool PortalConverter::FindRoomGroups(const NiAVObject* pkObject,
    NiTPrimitiveArray<NiNode*>& kRoomGroupNodes)
{
    bool bIsRoomGroup = false, bSubTreeHasRoomGroup = false;

    // Recursively search the scene graph and collect all the tagged room
    // group nodes for portalizing.
    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        const char* acName = pkNode->GetName();
        if (FindSub(acName, "*NDLRG") || FindSub(acName, "_NDLRG"))
        {
            kRoomGroupNodes.Add(pkNode);
            bIsRoomGroup = true;
        }

        const unsigned int uiCount = pkNode->GetArrayCount();
        for (unsigned int i = 0; i < uiCount; i++)
        {
            NiAVObject* pkChild = pkNode->GetAt(i);
            if (pkChild)
            {
                if (FindRoomGroups(pkChild, kRoomGroupNodes))
                    bSubTreeHasRoomGroup = true;
            }
        }
    }

    return !bIsRoomGroup || !bSubTreeHasRoomGroup;
}
//---------------------------------------------------------------------------
bool PortalConverter::FindRooms(const NiNode* pkRoomGroupNode,
    NiNode*& pkShell)
{
    // Traverse the children of each room group node to locate tagged room
    // nodes.

    bool bRooms = false;
    const unsigned int uiArrayCount = pkRoomGroupNode->GetArrayCount();
    for (unsigned int i = 0; i < uiArrayCount; i++)
    {
        NiAVObject* pkChild = pkRoomGroupNode->GetAt(i);
        if (pkChild)
        {
            const char* acName = pkChild->GetName();
            if (FindSub(acName, "*NDLRM") || FindSub(acName, "_NDLRM"))
            {
                // Found a node tagged as a room for the room group.
                NiNode* pkNode = NiDynamicCast(NiNode, pkChild);
                if (!pkNode)
                {
                    NiSprintf(m_acTemp, 512,
                        "The nonnode object \"%s\" is tagged as a room.",
                        acName);
                    m_kMessages += m_acTemp;
                    return false;
                }

                // Keep track of the room/roomnode pair for later processing.
                // The room names must be unique.
                RoomMapping* pkMap = NiNew RoomMapping(pkNode);
                RoomMapping* pkMapTmp;
                if (m_kRooms.GetAt(acName, pkMapTmp))
                {
                    NiSprintf(m_acTemp, 512, 
                        "A tagged room already exists with the "
                        "name \"%s\".\n", acName);
                    m_kMessages += m_acTemp;
                    return false;
                }
                pkMap->m_bHasTaggedWalls = HasTaggedWalls(pkNode);
                if (pkMap->m_bHasTaggedWalls)
                {
                    NiSprintf(m_acTemp, 512,
                        "Room \"%s\" has tagged walls.\n", acName);
                }

                m_kRooms.SetAt(acName, pkMap);

                CopyData(pkMap->m_spRoomNode, pkMap->m_spRoom);
                bRooms = true;
            }
            else
            {
                // The node is not a room node, so make it part of the shell
                // for the room group.
                if (!pkShell)
                    pkShell = NiNew NiNode;

                pkShell->AttachChild(pkChild);
            }
        }
    }

    if (!bRooms)
    {
        NiStrcpy(m_acTemp, 512, "No tagged rooms were found.");
        m_kMessages += m_acTemp;
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool PortalConverter::ParseRooms(NiRoomGroup* pkRoomGroup)
{
    unsigned int i;

    NiTMapIterator kIter = m_kRooms.GetFirstPos();
    while (kIter)
    {
        // Get a room and roomnode pair to process.
        RoomMapping* pkRoomMap;
        const char* pcKey;
        m_kRooms.GetNext(kIter, pcKey, pkRoomMap);
        NiNode* pkRoomNode = pkRoomMap->m_spRoomNode;
        NiRoom* pkRoom = pkRoomMap->m_spRoom;
        bool bSkipMeshes = pkRoomMap->m_bHasTaggedWalls;
        
        // Recursively find all vertices in this room so that we may compute
        // the convex hull for them.
        unsigned int uiNumPoints = 0;
        NiPoint3* akPoints = 0;
        GetPolygonPoints(pkRoomNode, bSkipMeshes, uiNumPoints, akPoints); 
        NiSprintf(m_acTemp, 512,
            "Room \"%s\" has %u vertices in all its children.\n",
            pkRoomNode->GetName(), uiNumPoints);
        m_kMessages += m_acTemp;
        if (!akPoints)
        {
            NiSprintf(m_acTemp, 512,
                "A room with 0 vertices is a problem.\n");
            m_kMessages += NiString(m_acTemp);
            return false;
        }
        
        // Create the convex hull of the points.
        pkRoomMap->m_pkHull = NiNew HullManager(uiNumPoints, akPoints);

        // Process the children to obtain portals and fixtures for the room.
        const unsigned int uiArrayCount = pkRoomNode->GetArrayCount();
        for (i = 0; i < uiArrayCount; i++)
        {
            NiAVObject* pkChild = pkRoomNode->GetAt(i);
            if (pkChild)
            {
                const char* acName = pkChild->GetName();
                if (FindSub(acName, "*NDLPR") || FindSub(acName, "_NDLPR"))
                {
                    if (!ParsePortal(pkChild, pkRoom))
                        return false;
                }
                else
                {
                    // The object is not a portal, so it must be a fixture.
                    pkRoom->AttachFixture(pkChild);

                    if (NiIsKindOf(NiDynamicEffect, pkChild))
                    {
                        // Since the room node has been copied, any dynamic
                        // effects that point to them must be corrected.
                        NiDynamicEffect* pkEffect = (NiDynamicEffect*)pkChild;

                        const NiNodeList& pkAffectedNL = 
                            pkEffect->GetAffectedNodeList();

                        NiTListIterator kIter = pkAffectedNL.GetHeadPos();
                        while(kIter)
                        {
                            NiNode* pkNode = pkAffectedNL.GetNext(kIter);

                            if (pkNode == NULL)
                                continue;

                            if (pkNode->GetName() == pkRoom->GetName())
                            {
                                pkEffect->DetachAffectedNode(pkNode);
                                pkEffect->AttachAffectedNode(pkRoom);
                            }
                        }

                        // Also check unaffected list
                        const NiNodeList& pkUnaffecteNL = 
                            pkEffect->GetUnaffectedNodeList();

                        kIter = pkUnaffecteNL.GetHeadPos();
                        while(kIter)
                        {
                            NiNode* pkNode = pkUnaffecteNL.GetNext(kIter);

                            if (pkNode == NULL)
                                continue;

                            if (pkNode->GetName() == pkRoom->GetName())
                            {
                                pkEffect->DetachUnaffectedNode(pkNode);
                                pkEffect->AttachUnaffectedNode(pkRoom);
                            }
                        }
                    }
                }
            }
        }
    }
    
    // The convex hulls have been computed for all the rooms.  We determine
    // which of them intersect.  For each room i, we test for intersections
    // with room j (i < j <= numRooms).  After all intersections have been
    // checked, we simplify the room (coplanar facets are grouped together),
    // then add the walls to the room and add the room to the room group.
    kIter = m_kRooms.GetFirstPos();
    while (kIter)
    {
        // Get room i.
        RoomMapping* pkRoomMap0;
        const char* acKey0;
        m_kRooms.GetNext(kIter, acKey0, pkRoomMap0);
        NiNode* pkRoomNode0 = pkRoomMap0->m_spRoomNode;
        NiRoom* pkRoom0 = pkRoomMap0->m_spRoom;
        HullManager* pkHull0 = pkRoomMap0->m_pkHull;

        // Test this room against all other rooms for intersection.  As
        // indexed rooms, room[i] is compared to room[j] only when i < j.
        // To do this, save the current iterator for i, to be restored
        // later, and use the current iterator for j.
        NiTMapIterator kSaveIter = kIter;
        while (kIter)
        {
            // Get room j.
            RoomMapping* pkRoomMap1;
            const char* acKey1;
            m_kRooms.GetNext(kIter, acKey1, pkRoomMap1);
            NiRoom* pkRoom1 = pkRoomMap1->m_spRoom;
            HullManager* pkHull1 = pkRoomMap1->m_pkHull;

            float fFraction = pkHull0->AreSeparated(pkHull1);
            if (fFraction > 0.0f)
            {
                NiSprintf(m_acTemp, 512,
                    "WARNING: Rooms \"%s\" and \"%s\" intersect "
                    "(penetration fraction %f).\n",
                    pkRoom0->GetName(), pkRoom1->GetName(), fFraction);
                m_kMessages += NiString(m_acTemp);
            }         
            else if (fFraction == -1.0f)
            {
                // fFraction will only be negative if error.
                NiSprintf(m_acTemp, 512,
                    "Error: Occurred in Room \"%s\" or \"%s\".\n",
                    pkRoom0->GetName(), pkRoom1->GetName());
                m_kMessages += NiString(m_acTemp);

                NiStrcpy(m_acTemp, 512,
                    "        A ConvexHull with mal-formed topology was " \
                    "generated that is preventing Portal Conversion from\n" \
                    "        completing. This sort of error can result when "\
                    "the involved vertices are " \
                    "nearly collinear or coplanar.\n" \
                    "        Try adjusting the vertices or use "\
                    "NDLWL geometry to remove this error.\n");
                m_kMessages += NiString(m_acTemp);
                return false;
            }
        }

        // Because of the order of comparison of rooms, we are finished with
        // the hull of room i.  Add the hull planes as walls of the room and
        // attach the room to the room group.
        AddWallsToRoom(pkHull0->GetNumPlanes(), pkHull0->GetPlanes(),
            pkRoom0);
        pkRoomGroup->AttachRoom(pkRoom0);

        // Report the statistics about the room.
        if (pkHull0->UsesHull())
        {
            NiSprintf(m_acTemp, 512,
                "Room \"%s\" has %d vertices, %d planes, and uses its "
                "convex hull.\n", pkRoomNode0->GetName(),
                pkHull0->GetNumVertices(), pkHull0->GetNumPlanes());
        }
        else
        {
            NiSprintf(m_acTemp, 512, 
                "Room \"%s\" uses its bounding box.\n",
                pkRoomNode0->GetName());
        }
        m_kMessages += m_acTemp;

        // Restore the previous iterator, for room i, to continue with
        // comparisons for room i+1.
        kIter = kSaveIter;
    }

    return true;
}
//---------------------------------------------------------------------------
bool PortalConverter::HasTaggedWalls(const NiAVObject* pkObject)
{
    const char* acName = pkObject->GetName();
    if (FindSub(acName, "*NDLWL") || FindSub(acName, "_NDLWL"))
        return true;

    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        const unsigned int uiArrayCount = pkNode->GetArrayCount();
        for (unsigned int i = 0; i < uiArrayCount; i++)
        {
            NiAVObject* pkChild = pkNode->GetAt(i);
            if (pkChild && HasTaggedWalls(pkChild))
                return true;
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void PortalConverter::AddWallsToRoom(int iNumPlanes, const NiPlane* akPlanes,
    NiRoom* pkRoom)
{
    // Add the planes as walls to the room.  The model planes must be in the
    // RoomGroup's coordinate system.
    NiMatrix3 kRot = pkRoom->GetRotate();
    NiPoint3 kTrn = pkRoom->GetTranslate();
    float fInvScale = 1.0f / pkRoom->GetScale();

    NiTPrimitiveArray<NiRoom::Wall*>& kWalls = pkRoom->GetWallArray();
    for (int i = 0; i < iNumPlanes; i++)
    {
        const NiPlane& kPlane = akPlanes[i];
        NiPoint3 kXFormNormal = kPlane.GetNormal() * kRot;
        float fXFormConstant = (kPlane.GetConstant() - kPlane.GetNormal() *
            kTrn) * fInvScale;

        NiRoom::Wall* pkWall = NiNew NiRoom::Wall;
        pkWall->m_kModelPlane.SetNormal(kXFormNormal);
        pkWall->m_kModelPlane.SetConstant(fXFormConstant);
        kWalls.Add(pkWall);
    }
}
//---------------------------------------------------------------------------
bool PortalConverter::ParsePortal(const NiAVObject* pkPortalObj,
    NiRoom* pkRoom)
{
    // Construct the portal polygon, a rectangle.
    NiPoint3* akPolygon = NiNew NiPoint3[4];
    if (!ConstructPolygon(pkPortalObj, akPolygon))
    {
        NiSprintf(m_acTemp, 512, 
            "No suitable polygon found for portal \"%s\".\n",
            pkPortalObj->GetName());
        m_kMessages += m_acTemp;
        NiDelete[] akPolygon;
        return false;
    }

    NiPortalPtr spPortal = NiNew NiPortal(4, akPolygon);
    spPortal->SetName(pkPortalObj->GetName());

    // Determine the adjoining room by searching the string extra data that
    // stores this information.
    NiRoomPtr spAdjoiner = 0;
    char* acAdjoinerName = GetAdjoinerName(pkPortalObj);
    if (!acAdjoinerName)
    {
        NiSprintf(m_acTemp, 512,
            "No NDLADJOINER key found for portal \"%s\".\n",
            pkPortalObj->GetName());
        m_kMessages += m_acTemp;
        return false;
    }

    // Locate the adjoining room identified by the name.
    RoomMapping* pkMap;
    if (m_kRooms.GetAt(acAdjoinerName, pkMap))
    {
        spAdjoiner = pkMap->m_spRoom;
        NIASSERT(spAdjoiner);
        NiFree(acAdjoinerName);
    }
    else
    {
        NiSprintf(m_acTemp, 512,
            "Referenced adjoiner \"%s\" not found for portal "
            "\"%s\"", acAdjoinerName, pkPortalObj->GetName());
        m_kMessages += m_acTemp;
        NiFree(acAdjoinerName);
        return false;
    }

    // Establish the "room -> portal -> adjoiner" graph connections.
    pkRoom->AttachOutgoingPortal(spPortal);
    spPortal->SetAdjoiner(spAdjoiner);
    spAdjoiner->AttachIncomingPortal(spPortal);

    // Requests for two-way portals are made by providing the "NDLTWOWAY" in
    // the string extra data.
    if (IsTwoWay(pkPortalObj))
    {
        // The portal polygon (RA) for room-to-adjoiner must be duplicated to
        // create the portal polygon (AR) for adjoiner-to-room.  The vertex
        // order of AR must be opposite that of RA.  The vertices of RA must
        // be transformed into the coordinate system of the adjoiner room.

        // Set up the transform from room R to adjoiner A.  This is performed
        // by a composition of "R to world" and "world to A".  Let X0 be a
        // point in room R, with transformation Y = s0*R0*X0+T0, where S0 is
        // scale, R0 is rotation, and T0 is rotation.  Similarly, define a
        // point in room A as X1 and Y = s1*R1*X1+T1, where S1 is scale, R1
        // is rotation, and T1 is translation.  The composition is
        //   X1 = (s0/s1)*R1^t*R0*X0 + (1/S1)*R0^t*(T0-T1)
        // where superscript t denotes transpose.
        const NiMatrix3& kR0 = pkRoom->GetRotate();
        const NiMatrix3& kR1 = spAdjoiner->GetRotate();
        const NiPoint3& kT0 = pkRoom->GetTranslate();
        const NiPoint3& kT1 = spAdjoiner->GetTranslate();
        const float fS0 = pkRoom->GetScale();
        const float fS1 = spAdjoiner->GetScale();
        NiPoint3 kTrn = ((kT0 - kT1) * kR1) / fS1;
        NiMatrix3 kMat = kR1.TransposeTimes(kR0) * (fS0 / fS1);

        // Construct the portal polygon AR (akPolygonRev) from RA (akPolygon).
        NiPoint3* akPolygonRev = NiNew NiPoint3[4];
        for (unsigned int i = 0; i < 4; i++)
            akPolygonRev[i] = kMat*akPolygon[3-i] + kTrn;

        NiPortalPtr spPortalRev = NiNew NiPortal(4, akPolygonRev);

        // The reverse portal uses the name of the original portal appended
        // with "-Reverse", so we need strlen(oldName)+strlen("-Reverse")+1
        // characters for storage of the new name.
        size_t stLength = strlen(pkPortalObj->GetName()) + 9;
        char* acPortalName = NiAlloc(char, stLength);
        NiSprintf(acPortalName, stLength, "%s-Reverse",
            pkPortalObj->GetName());
        spPortalRev->SetName(acPortalName);
        NiFree(acPortalName);

        // Establish the "adjoiner -> portal -> room" graph connections.
        spAdjoiner->AttachOutgoingPortal(spPortalRev);
        spPortalRev->SetAdjoiner(pkRoom);
        pkRoom->AttachIncomingPortal(spPortalRev);

        NiDelete[] akPolygonRev;
    }
    
    NiDelete[] akPolygon;
    return true;
}
//---------------------------------------------------------------------------
bool PortalConverter::ConstructPolygon(const NiAVObject* pkObject,
    NiPoint3 akRectangle[4])
{
    // Traverse the subtree rooted at the input object and collect all the
    // portal polygon points.  The points are in the coordinate system of the
    // input object.
    unsigned int uiNumPoints = 0;
    NiPoint3* akPoints = 0;
    GetPolygonPoints(pkObject, false, uiNumPoints, akPoints);
    if (uiNumPoints == 0)
    {
        NiDelete [] akPoints;
        return false;
    }

    NiPoint3 kP0, kP1, kP2;
    if (!GetPolygonTriangle(pkObject, kP0, kP1, kP2))
    {
        NiDelete [] akPoints;
        return false;
    }
    // Compute a bounding rectangle for the points within the plane of the
    // points.  This requires a coordinate system for the plane.  If P is a
    // point on the plane and W is a unit-length normal for the plane, select
    // unit-length vectors U and V so that {U,V,W} is an orthonormal set.
    // Any point Q on the plane has a representation Q = P + x*U + y*V.  The
    // bounding rectangle is [xmin,xmax]x[ymin,ymax], where the min and max
    // values are those of the coefficients of U and V in the equation.
    //
    // Choices for {P;U,V,W} are many.  We locate a subtriangle of the portal
    // polygon, <P0,P1,P2>.  A normal vector W is constructed for the
    // triangle.  However, we have infinitely many choices for U and V.
    // Because a rectangle is the common portal shape, two subtriangles exist
    // and are right triangles.  We will attempt to choose the two edges
    // sharing the right angle for U and V.  This gives us the tightest
    // fitting bounding rectangle for the common case.

    NiPoint3 akEdge[3] = { kP1 - kP0, kP2 - kP1, kP0 - kP2 };
    int i0, i1;
    for (i1 = 0; i1 < 3; i1++)
        akEdge[i1].Unitize();

    float fMinDot = 2.0f;
    int iMinIndex = -1;
    for (i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
    {
        float fDot = NiAbs(akEdge[i0].Dot(akEdge[i1]));
        if (fDot < fMinDot)
        {
            fMinDot = fDot;
            iMinIndex = i0;
        }
    }

    NiPoint3 kU = akEdge[iMinIndex];
    NiPoint3 kW = akEdge[iMinIndex].UnitCross(akEdge[(iMinIndex + 1) % 3]);
    NiPoint3 kV = kW.Cross(kU);

    float fInvLength;
    if ( NiAbs(kW.x) >= NiAbs(kW.y) )
    {
        // W.x or W.z is the largest magnitude component, swap them.
        fInvLength = 1.0f / NiSqrt(kW.x * kW.x + kW.z * kW.z);
        kU.x = -kW.z * fInvLength;
        kU.y = 0.0f;
        kU.z = kW.x * fInvLength;
    }
    else
    {
        // W.y or W.z is the largest magnitude component, swap them.
        fInvLength = 1.0f / NiSqrt(kW.y * kW.y + kW.z * kW.z);
        kU.x = 0.0f;
        kU.y = kW.z * fInvLength;
        kU.z = -kW.y * fInvLength;
    }
    kV = kW.Cross(kU);

    // The first point in the array acts as P, so (x,y) = (0,0) in the
    // {P;U,V} coordinate system.  Initialize min and max to (0,0).
    NiPoint3 kP = akPoints[0];
    NiPoint2 kMin(0.0f, 0.0f), kMax(0.0f, 0.0f);
    for (unsigned int i = 1; i < uiNumPoints; i++)
    {
        NiPoint3 kDiff = akPoints[i] - kP;
        float fU = kU.Dot(kDiff), fV = kV.Dot(kDiff);

        if (fU < kMin.x)
            kMin.x = fU;
        else if (fU > kMax.x)
            kMax.x = fU;

        if (fV < kMin.y)
            kMin.y = fV;
        else if (fV > kMax.y)
            kMax.y = fV;
    }

    // Construct the rectangle in the original coordinate system.
    akRectangle[0] = kP + kMin.x * kU + kMin.y * kV;
    akRectangle[1] = kP + kMax.x * kU + kMin.y * kV;
    akRectangle[2] = kP + kMax.x * kU + kMax.y * kV;
    akRectangle[3] = kP + kMin.x * kU + kMax.y * kV;

    NiDelete [] akPoints;
    return true;
}
//---------------------------------------------------------------------------
void PortalConverter::GetPolygonPoints(const NiAVObject* pkObject,
    bool bSkipMeshes, unsigned int& uiNumPoints, NiPoint3*& akPoints)
{
    // When bSkipMeshes is 'false', search the entire subtree rooted at the
    // input object for all vertices from NiMesh objects.  Store
    // these for computing the convex hull of the room.  The vertices for the
    // current NiMesh object must be transformed into the parent's
    // coordinate system.
    //
    // When bSkipMeshes is 'true', the artist has tagged the walls of the
    // room.  The geometry for these walls is used for computing the convex
    // hull of the room.  The nontagged room geometry is skipped.

    if (bSkipMeshes)
    {
        const char* acName = pkObject->GetName();
        if (FindSub(acName, "*NDLWL") || FindSub(acName, "_NDLWL") ||
            FindSub(acName, "*NDLPR") || FindSub(acName, "_NDLPR"))
        {
            bSkipMeshes = false;
        }
    }

    unsigned int i;

    if (!bSkipMeshes)
    {
        if (NiIsKindOf(NiMesh, pkObject))
        {
            NiMesh* pkMesh = (NiMesh*)pkObject;

            // Grow the array to add the new points.
            unsigned int uiNewNumPoints = uiNumPoints +
                pkMesh->GetVertexCount();
            NiPoint3* akNewPoints = NiNew NiPoint3[uiNewNumPoints];
            if (akPoints)
            {
                unsigned int uiSize = uiNumPoints * sizeof(NiPoint3);
                NiMemcpy(akNewPoints, akPoints, uiSize);
                NiDelete[] akPoints;
                akPoints = NULL;
            }

            NiDataStreamElementLock kLockVerts(pkMesh,
                NiCommonSemantics::POSITION(), 0, 
                NiDataStreamElement::F_FLOAT32_3, 
                NiDataStream::LOCK_TOOL_READ);
            NIASSERT(kLockVerts.IsLocked());
           
            i = uiNumPoints;

            for (NiUInt32 ui = 0; ui < pkMesh->GetSubmeshCount(); ui++)
            {
                // It is assumed that there is a single shared fully used,
                // continuous vertex region.
                NiVertIter kVertIter = kLockVerts.begin<NiPoint3>(ui);
                NiVertIter kEnd = kLockVerts.end<NiPoint3>(ui);

                // Add the new points to the end of the array.  These points 
                // are transformed into the parent's coordinate system.
                while (kVertIter != kEnd)
                {
                    akNewPoints[i] = (pkMesh->GetRotate() *
                        (*kVertIter++)) * pkMesh->GetScale() +
                        pkMesh->GetTranslate();

                    i++;
                }
            }

            NIASSERT(i == uiNewNumPoints);
           
            akPoints = akNewPoints;
            uiNumPoints = uiNewNumPoints;
            return;
        }
    }

    NiNode* pkNode = NiDynamicCast(NiNode, pkObject);
    if (pkNode)
    {
        // Save the number of points in the array prior to traversing the
        // children of the node.  This allows us to transform only those
        // points added by the traversal.
        unsigned int uiOldNumPoints = uiNumPoints;

        // Recurse on the subtree rooted at this node to locate more points
        // in geometric objects.
        const unsigned int uiArrayCount = pkNode->GetArrayCount();
        for (i = 0; i < uiArrayCount; i++)
        {
            NiAVObject* pkChild = pkNode->GetAt(i);
            if (pkChild)
                GetPolygonPoints(pkChild, bSkipMeshes, uiNumPoints, akPoints);
        }

        // Transform the newly added points into the parent's coordinate
        // system.
        for (i = uiOldNumPoints; i < uiNumPoints; i++)
        {
            akPoints[i] = (pkNode->GetRotate() * akPoints[i]) *
                pkNode->GetScale() + pkNode->GetTranslate();
        }
    }
}
//---------------------------------------------------------------------------
bool PortalConverter::GetPolygonTriangle(const NiAVObject* pkObj,
    NiPoint3& kP0, NiPoint3& kP1, NiPoint3& kP2)
{
    unsigned int i;

    if (NiIsKindOf(NiMesh, pkObj))
    {
        NiMesh* pkMesh = (NiMesh*)pkObj;

        // Get the first triangle and find its plane.
        unsigned int uiTQuantity = pkMesh->GetTotalPrimitiveCount();
        if (uiTQuantity == 0)
            return false;

        NiDataStreamElementLock kLockVerts(pkMesh,
            NiCommonSemantics::POSITION(),
            0, NiDataStreamElement::F_FLOAT32_3, 
            NiDataStream::LOCK_TOOL_READ);
        NIASSERT(kLockVerts.IsLocked());

        // It is assumed that there is a single shared fully used,
        // continuous vertex region.
        NiVertIter kVertIter = kLockVerts.begin<NiPoint3>();

        FirstNonDegenerate kFirstNonDegenerate(kVertIter);

        // We expect only 1 submesh though.
        NiMeshAlgorithms::ForEachPrimitiveAllSubmeshes(pkMesh,
            kFirstNonDegenerate, NiDataStream::LOCK_TOOL_READ, true);

        if (kFirstNonDegenerate.m_bFoundFirst == false)
            return false;

        // Transform the triangle vertices into the parent's coordinate
        // system.
        kP0 = pkMesh->GetScale() * (pkMesh->GetRotate() *
            (kFirstNonDegenerate.m_kV0)) + pkMesh->GetTranslate();
        kP1 = pkMesh->GetScale() * (pkMesh->GetRotate() *
            (kFirstNonDegenerate.m_kV1)) + pkMesh->GetTranslate();
        kP2 = pkMesh->GetScale() * (pkMesh->GetRotate() *
            (kFirstNonDegenerate.m_kV2)) + pkMesh->GetTranslate();

        return true;
    }

    NiNode* pkNode = NiDynamicCast(NiNode, pkObj);
    if (pkNode)
    {
        // Recursively search for the portal polygon.  This is necessary only
        // if the tagged portal object is not itself a triangle mesh, but
        // is constructed as a node-rooted subtree of the scene.
        const unsigned int uiArrayCount = pkNode->GetArrayCount();
        for (i = 0; i < uiArrayCount; i++)
        {
            NiAVObject* pkChild = pkNode->GetAt(i);
            if (pkChild)
            {
                if (GetPolygonTriangle(pkChild, kP0, kP1, kP2))
                {
                    // Transform the triangle vertices into the parent's
                    // coordinate system.
                    kP0 = pkNode->GetScale() * (pkNode->GetRotate() * kP0) +
                        pkNode->GetTranslate();
                    kP1 = pkNode->GetScale() * (pkNode->GetRotate() * kP1) +
                        pkNode->GetTranslate();
                    kP2 = pkNode->GetScale() * (pkNode->GetRotate() * kP2) +
                        pkNode->GetTranslate();
                    return true;
                }
            }
        }
    }

    return false;
}
//---------------------------------------------------------------------------
void PortalConverter::CopyData(const NiAVObject* pkSrc, NiAVObject* pkDest)
{
    // copy transforms
    pkDest->SetTranslate(pkSrc->GetTranslate());
    pkDest->SetRotate(pkSrc->GetRotate());
    pkDest->SetScale(pkSrc->GetScale());

    // copy properties
    pkDest->DetachAllProperties();
    NiTListIterator kIter = pkSrc->GetPropertyList().GetHeadPos();
    while(kIter)
        pkDest->AttachProperty(pkSrc->GetPropertyList().GetNext(kIter));

    // copy extra data
    pkDest->RemoveAllExtraData();
    const unsigned int uiSize = pkSrc->GetExtraDataSize();
    for (unsigned int i = 0; i < uiSize; i++)
    {
        NiExtraData* pkData = pkSrc->GetExtraDataAt((unsigned short)i);
        if (pkData)
            pkDest->AddExtraData(pkData);
    }

    // copy collision data
    pkDest->SetCollisionObject(pkDest->GetCollisionObject());

    // copy name
    pkDest->SetName(pkSrc->GetName());

    // The following items are explicitly not copied.
    //
    // * Runtime flags:  Most of these are runtime-only or related to objects
    //   that the portal system need not convert (e.g. Billboards).  The 
    //   remainder are related to UpdateSelected, and the safest option in
    //   that case is to leave the flags at the default (i.e. update all).
    //
    // * Time controllers:  Few if any time controllers make sense or will 
    //   work when attached to portals, rooms and room groups.
}
//---------------------------------------------------------------------------
char* PortalConverter::GetAdjoinerName(const NiObjectNET* pkObj)
{
    const unsigned int uiSize = pkObj->GetExtraDataSize();
    for (unsigned int i = 0; i < uiSize; i++)
    {
        NiStringExtraData* pkExtra = NiDynamicCast(NiStringExtraData,
            pkObj->GetExtraDataAt((unsigned short)i));

        if (pkExtra)
        {
            const char* acMatch = FindSub(pkExtra->GetValue(), "NDLADJOINER");
            if (acMatch)
            {
                // Search the string for an opening quote.
                while (acMatch[0] && acMatch[0]!='\"')
                    acMatch++;

                if (acMatch[0])
                {
                    // An opening quote was found; skip it.
                    acMatch++;

                    // Search the string for a closing quote/
                    unsigned int uiLength = 0;
                    while (acMatch[uiLength] && acMatch[uiLength]!='\"')
                        uiLength++;

                    if (acMatch[uiLength])
                    {
                        // A closing quote was found; copy the substring
                        // between the quotes and return it.
                        char* acRet = NiAlloc(char, uiLength+1);
                        NiMemcpy(acRet, acMatch, uiLength);
                        acRet[uiLength] = '\0';
                        return acRet;
                    }
                }
                    
            }
        }
    }

    return 0;
}
//---------------------------------------------------------------------------
bool PortalConverter::IsTwoWay(const NiObjectNET* pkPortalObj)
{
    const unsigned int uiSize = pkPortalObj->GetExtraDataSize();
    for (unsigned int i = 0; i < uiSize; i++)
    {
        NiStringExtraData* pkExtra = NiDynamicCast(NiStringExtraData,
            pkPortalObj->GetExtraDataAt((unsigned short)i));

        if (pkExtra && FindSub(pkExtra->GetValue(), "NDLTWOWAY"))
            return true;
    }
    return false;
}
//---------------------------------------------------------------------------
const char* PortalConverter::FindSub(const char* acStr, const char* acSub)
{
    size_t stLength = strlen(acSub);
    while (acStr && acStr[0])
    {
        if (!strncmp(acStr, acSub, stLength))
            return acStr;

        acStr++;
    }
    return 0;
}
//---------------------------------------------------------------------------
