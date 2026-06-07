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

#ifndef NIOBJECT_H
#define NIOBJECT_H

#include "NiRTTI.h"
#include "NiSmartPointer.h"
#include "NiClone.h"
#include "NiRefObject.h"
#include "NiStreamMacros.h"
#include "NiViewerStrings.h"
#include "NiTPointerList.h"

class NiObjectGroup;

NiSmartPointer(NiObject);
template <class TKEY, class TVAL, class HASH, class EQUALS>
    class NiTPointerMap;

class NIMAIN_ENTRY NiObject : public NiRefObject
{
    NiDeclareRootRTTI(NiObject);
    NiDeclareClone(NiObject);
    NiDeclareStream;
    NiDeclareViewerStrings;
    inline virtual void AddViewerStrings (NiViewerStringsArray*) {};

public:
    
    enum CloningBehavior
    {
        CLONE_SHARE,
        CLONE_COPY,
        CLONE_BLANK_COPY
    };

    // destruction (constructor is protected, abstract base class)
    virtual ~NiObject();

    // cloning
    NiObject* Clone();
    NiObject* Clone(NiCloningProcess& kCloning);
    // Safely create a clone of this object even if it's already cloned.
    NiObject* CreateSharedClone(NiCloningProcess& kCloning);
    virtual void ProcessClone(
        NiCloningProcess& kCloning);

    NiObjectPtr CreateDeepCopy();

    // *** begin Emergent internal use only ***

    // streaming support
    virtual void PostLinkObject(NiStream& kStream);
    virtual bool StreamCanSkip();
    virtual bool GetStreamableRTTIName(char* acName, 
        unsigned int uiMaxSize) const;

    // block allocation
    virtual unsigned int GetBlockAllocationSize() const;
    virtual NiObjectGroup* GetGroup() const;
    virtual void SetGroup(NiObjectGroup* pkGroup);

    // *** end Emergent internal use only ***

protected:
    // constructor for streaming
    NiObject();

private:
    // To prevent an application from inadvertently causing the compiler to
    // generate the default copy constructor or default assignment operator,
    // these methods are declared as private. They are not defined anywhere,
    // so code that attempts to use them will not link.
    // Applications should use NiObject::Clone() to duplicate NiObject-derived
    // classes.
    NiObject(const NiObject& kObject);
    NiObject& operator=(const NiObject& kObject);
};

// smart pointer type for objects
typedef NiPointer<NiObject> NiObjectPtr;
typedef NiTPointerList<NiObjectPtr> NiObjectList;

#endif

