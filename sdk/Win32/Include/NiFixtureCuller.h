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

#ifndef NIFIXTURECULLER_H
#define NIFIXTURECULLER_H

#include "NiPortalLibType.h"
#include "NiMeshCullingProcess.h"
#include "NiTMap.h"

class NIPORTAL_ENTRY NiFixtureCuller : public NiMeshCullingProcess
{
public:
    NiFixtureCuller(NiSPWorkflowManager* pkWorkflowManager);
    virtual ~NiFixtureCuller();

protected:
    // Construct the potentially visible set of fixtures in a room group.
    // A room group will call this method during OnVisible. NiRoomGroup is
    // declared a friend rather than redefining the access specifier for 
    // Process on this subclass.
    friend class NiRoomGroup;
    virtual void Process(const NiCamera* pkCamera, NiAVObject* pkScene,
        NiVisibleArray* pkVisibleSet);

    // Insert the input object into the potentially visible set, but only if
    // the object is not already in the set.  Since this overrides the base
    // class functionality, we need to be sure to signal to the base class
    // (in the constructor initialization list) that the virtual version of
    // Append ahould be used.
    virtual void AppendVirtual(NiRenderObject& kVisible);

    // Support for deferred drawing of fixtures in the rooms.  NiTSet does
    // not provide accessors that are O(1), so NiTMap is used but its value
    // field is irrelevant.  The map is used to guarantee the potentially
    // visible set has only unique objects.  The array itself preserves the
    // order of objects produced by the scene traversal.  This avoids side
    // effects that specialized nodes might introduce regarding sorted
    // drawing.
    NiTMap<NiRenderObject*,int> m_kFixtureMap;
};

#endif
