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

#ifndef NIDIRECTIONALLIGHT_H
#define NIDIRECTIONALLIGHT_H

#include "NiLight.h"
#include "NiPoint3.h"

class NIMAIN_ENTRY NiDirectionalLight : public NiLight
{
    NiDeclareRTTI;
    NiDeclareClone(NiDirectionalLight);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    NiDirectionalLight();
    
    // The model direction of the light is (1,0,0).  The world direction is
    // the first column of the world rotation matrix.
    inline const NiPoint3& GetWorldDirection() const;

protected:
    virtual void UpdateWorldData();

    // world direction
    NiPoint3 m_kWorldDir;
};

typedef NiPointer<NiDirectionalLight> NiDirectionalLightPtr;

#include "NiDirectionalLight.inl"

#endif
