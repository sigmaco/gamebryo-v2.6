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

#ifndef NIDRAWABLEBV_H
#define NIDRAWABLEBV_H

#include "NiCollisionLibType.h"
#include "NiAVObject.h"
#include "NiColor.h"
class NiBoundingVolume;
class NiBox;
class NiCapsule;
class NiHalfSpaceBV;
class NiNode;
class NiSphere;
class NiUnionBV;
class NiMesh;

class NICOLLISION_ENTRY NiDrawableBV : public NiMemObject
{
    // cloning: no cloning necessary, as this creates a NiMesh and 
    // attaches it to the scene graph as a regular node, which will have its
    // own cloning functionality
public:
    // for creation of wireframe representation of the bounding volume
    static NiNode* CreateWireframeBV(const NiBoundingVolume* pkBound,
        const NiColor& kColor = NiColor::BLACK, const float fMult = 1.0f, 
        NiAVObject* pkObject = 0);


    // *** begin Emergent internal use only ***

    // the following do not attach any properties
    static NiMesh* CreateFromBox(const NiBox& kBox, 
        const float fMult);
    static NiMesh* CreateFromCapsule(const NiCapsule& kCapsule,
        const float fMult);
    static NiMesh* CreateFromSphere(const NiSphere& kSphere,
        const float fMult);
    static NiMesh* CreateFromHalfSpaceBV(const NiHalfSpaceBV* pkBound, 
        const float fMult, const NiAVObject* pkObject);
    static void CreateFromUnion(const NiUnionBV* pkUnion, NiNode* pkParent,
        const float fMult);

    static NiMesh* CreateTubeFromCapsule(const NiCapsule& kCapsule,
        const float fMult, const unsigned int uiNumSides);

    static NiMesh* CreateMeshFromVertexIndexData(NiPoint3* kVerts, 
        unsigned int uiNumVerts, unsigned short* pusIndices, 
        unsigned int uiNumIndices);

    // *** end Emergent internal use only ***
};

#endif
