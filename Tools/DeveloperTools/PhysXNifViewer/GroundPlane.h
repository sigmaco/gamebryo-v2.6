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

#ifndef GROUNDPLANE_H
#define GROUNDPLANE_H

#include <NiMain.h>
#include <NiPhysX.h>

class GroundPlane : public NiRefObject {
  
public:
    GroundPlane(NiPhysXScene* pkScene, NiPoint3& kNorm, const float fD,
        const float fRad);
    ~GroundPlane();

    NiAVObject* GetGeometry();
    
    NxActor* GetActor();
    
protected:
    NiMeshPtr PlaneGeometry(const NiPoint3& kNorm, const float fD,
        const float fRad);
    
    NxScene* m_pkScene;
    NxActor* m_pkActor;
    NiAVObjectPtr m_spGeometry;
};

typedef NiPointer<GroundPlane> GroundPlanePtr;

#endif // #ifndef GROUNDPLANE_H

