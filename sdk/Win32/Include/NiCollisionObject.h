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

#ifndef NICOLLISIONOBJECT_H
#define NICOLLISIONOBJECT_H

#include "NiObject.h"

class NiAVObject;

class NIMAIN_ENTRY NiCollisionObject : public NiObject
{
    NiDeclareRTTI;
    NiDeclareAbstractClone(NiCollisionObject);
    NiDeclareStream;
    NiDeclareViewerStrings;

public:
    // Constructors and Destructors
    NiCollisionObject(NiAVObject* pkSceneObject);
    virtual ~NiCollisionObject() = 0;

    // scene graph object access
    NiAVObject* GetSceneGraphObject() const;
    virtual void SetSceneGraphObject(NiAVObject* pkSceneObject);

    virtual void UpdateWorldData() = 0;
    virtual void RecreateWorldData() = 0;

    // *** begin Emergent internal use only ***   
    typedef struct 
    {
        NiAVObject* m_pkAVObject;
        NiStream* m_pkStream;
        bool m_bABV;
        unsigned short m_usProgagationBits;
    } OldNifInit;

    inline virtual void Initialize(void*){};
    virtual void AddViewerStrings(NiViewerStringsArray* pkStrings);
    inline virtual void Convert(unsigned int, void* ){};
    // *** end Emergent internal use only ***
    
protected:
    // To prevent public access to a constructor with no scene association
    NiCollisionObject();

    NiAVObject* m_pkSceneObject;
};

typedef NiPointer<NiCollisionObject> NiCollisionObjectPtr;

#endif



